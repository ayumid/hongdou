//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_sw_ele.c
// Auther      : zhaoning
// Version     :
// Date : 2024-7-23
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-23
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_ty_sw_ele.h"
#include "lib_common.h"

// Private defines / typedefs ---------------------------------------------------

#define CSE7758B_PKG_LEN           24
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
#define SAMPLE_RESISTANCE_MR    1//使用的采样锰铜电阻mR值

#define UART_IND_HD            0
#define UART_IND_5A            1
#define UART_IND_VK            2
#define UART_IND_VT            5
#define UART_IND_IK            8
#define UART_IND_IT            11
#define UART_IND_PK            14
#define UART_IND_PT            17
#define UART_IND_FG            20
#define UART_IND_EN            21
#define UART_IND_SM            23

#define ARRAY_LEN       3//平滑滤波buf长度
#define COUNT_NUM       1//超时更新数据次数

//7759B电能计数脉冲溢出时的数据
#define ENERGY_FLOW_NUM            65536   //电量采集，电能溢出时的脉冲计数值

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

CSE7759B_DATA_T cse7759b_data;

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : lib_get_cse7759b_data
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
CSE7759B_DATA_T* lib_get_cse7759b_data(void)
{
    return &cse7759b_data;
}

/**
  * Function    : am_ty_sw_get_VIP_value
  * Description : 获取电压、电流、功率的有限数据
  * Input       : arr    参数数组
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
unsigned long am_ty_sw_get_VIP_value(unsigned long *arr)//更新电压、电流、功率的列表
{
    int maxIndex = 0;
    int minIndex = 0;
    unsigned long sum = 0;
    int j = 0;
    for(j = 1; j < ARRAY_LEN; j++)
    {
        if(arr[maxIndex] <= arr[j])//避免所有数据一样时minIndex等于maxIndex
        {
            maxIndex = j;
        }
        if(arr[minIndex] > arr[j]){
            minIndex = j;
        }
    }
    
    for(j = 0; j < ARRAY_LEN; j++)
    {
        if((maxIndex == j) || (minIndex == j))
        {
            continue;
        }
        else{
            return arr[j];
        }
    }

    return 0;
}

/**
  * Function    : am_ty_sw_need_updata
  * Description : 检测电压电流功率是否需要更新
  * Input       : arr    当前参数
  *               dat    保存的旧参数
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int am_ty_sw_need_updata(unsigned long *arr,unsigned long dat)
{
    if(arr[0] == dat){
        return 0;
    }
    else{
        return 1;
    }
}

/**
  * Function    : am_ty_sw_updata_VIP_value
  * Description : 更新电压、电流、功率的列表
  * Input       : arr    参数数组
  *               dat    更新值
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void am_ty_sw_updata_VIP_value(unsigned long *arr,unsigned long dat)
{
    int ii = ARRAY_LEN-1;
    for(ii = ARRAY_LEN-1; ii > 0; ii--){
        arr[ii] = arr[ii-1];
    }
    arr[0] = dat;
}

/**
  * Function    : am_ty_sw_reset_VIP_value
  * Description : 更新所有电压、电流、功率的列表
  * Input       : arr    参数数组
  *               dat    更新值
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void am_ty_sw_reset_VIP_value(unsigned long *arr,unsigned long dat)
{    
    int ii = ARRAY_LEN-1;
    for(ii = ARRAY_LEN-1; ii >= 0; ii--){
        arr[ii] = dat;
    }
}

/**
  * Function    : am_ty_sw_deal_uart_data
  * Description : 处理串口收到的数据，计算出电压，电流，功率；数据错误处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int am_ty_sw_deal_uart_data(unsigned char *inData, int recvlen)
{
    unsigned char     startFlag     = 0;
    
    unsigned long      voltage_k        = 0;
    unsigned long      voltage_t     = 0;
    unsigned long      voltage       = 0;
    static unsigned long voltage_a[ARRAY_LEN]  = {0};
    static unsigned int  voltageCnt    = 0;

    unsigned long      electricity_k = 0;
    unsigned long      electricity_t = 0;
    unsigned long      electricity   = 0;
    static unsigned long electricity_a[ARRAY_LEN]  = {0};
    static unsigned int  electricityCnt    = 0;

    unsigned long      power_k = 0;
    unsigned long      power_t = 0;
    unsigned long      power      = 0;
    static unsigned long power_a[ARRAY_LEN]= {0};
    static unsigned int  powerCnt    = 0;
    static unsigned long powerNewFlag = 1;
    
    unsigned int      energy_cnt     = 0;
    unsigned char      energyFlowFlag = 0;

    float Q = 0;
    
    char dealBuffer[CSE7758B_PKG_LEN] = {0};
    //判断长度是否是24字节
    if(CSE7758B_PKG_LEN != recvlen)
    {
        return -1;
    }

    memcpy(dealBuffer, inData, CSE7758B_PKG_LEN);
    startFlag = dealBuffer[UART_IND_HD];
    switch(startFlag)
    {
        case 0x55:
            if((dealBuffer[UART_IND_FG]&0x40) == 0x40)//获取当前电压标致，为1时说明电压检测OK
            {
                voltage_k = ((dealBuffer[UART_IND_VK] << 16)|(dealBuffer[UART_IND_VK+1] << 8)|(dealBuffer[UART_IND_VK+2]));//电压系数
                voltage_t = ((dealBuffer[UART_IND_VT] << 16)|(dealBuffer[UART_IND_VT+1] << 8)|(dealBuffer[UART_IND_VT+2]));//电压周期
                
                if(am_ty_sw_need_updata(voltage_a,voltage_t))
                {
                    am_ty_sw_updata_VIP_value(voltage_a,voltage_t);
                    voltageCnt = 0;
                }
                else
                {
                    voltageCnt++;
                    if(voltageCnt >= COUNT_NUM)
                    {
                        voltageCnt = 0;
                        am_ty_sw_updata_VIP_value(voltage_a,voltage_t);
                    }
                }
//                lib_uart_printf("%s[%d] voltage:%d,%d,%d", __FUNCTION__, __LINE__, voltage_a[0],voltage_a[1],voltage_a[2]);
                voltage_t = am_ty_sw_get_VIP_value(voltage_a);
                
                if(voltage_t == 0)
                {
                    voltage = 0;
                }
                else
                {
                    voltage = voltage_k * 100 / voltage_t;//电压10mV值，避免溢出
                    voltage = voltage * 10;//电压mV值
                }
                
//                lib_uart_printf("%s[%d] Vk = %d,Vt = %d,v = %d", __FUNCTION__, __LINE__, voltage_k,voltage_t,voltage);
            }
            else
            {
                lib_uart_printf("%s(%d):V Flag Error", __FUNCTION__, __LINE__);
            }
            
            if((dealBuffer[UART_IND_FG]&0x20) == 0x20)
            {
                electricity_k = ((dealBuffer[UART_IND_IK] << 16)|(dealBuffer[UART_IND_IK+1] << 8)|(dealBuffer[UART_IND_IK+2]));//电流系数
                electricity_t = ((dealBuffer[UART_IND_IT] << 16)|(dealBuffer[UART_IND_IT+1] << 8)|(dealBuffer[UART_IND_IT+2]));//电流周期

                if(am_ty_sw_need_updata(electricity_a,electricity_t))
                {
                    am_ty_sw_updata_VIP_value(electricity_a,electricity_t);
                    electricityCnt = 0;
                }
                else
                {
                    electricityCnt++;
                    if(electricityCnt >= COUNT_NUM)
                    {
                        electricityCnt = 0;
                        am_ty_sw_updata_VIP_value(electricity_a,electricity_t);
                    }
                }
//                lib_uart_printf("%s[%d] electricity:%d,%d,%d", __FUNCTION__, __LINE__, electricity_a[0],electricity_a[1],electricity_a[2]);
                electricity_t = am_ty_sw_get_VIP_value(electricity_a);
                
                if(electricity_t == 0)
                {
                    electricity = 0;
                }
                else
                {
                    electricity = electricity_k * 100 / electricity_t;//电流10mA值，避免溢出
                    electricity = electricity * 10;//电流mA值
                    #if(SAMPLE_RESISTANCE_MR == 1)
                    //由于使用1mR的电阻，电流和功率需要不除以2
                    #elif(SAMPLE_RESISTANCE_MR == 2)
                    //由于使用2mR的电阻，电流和功率需要除以2
                    electricity >>= 1;
                    #endif
                }

//                lib_uart_printf("%s[%d] Ik = %d,It = %d,I = %d", __FUNCTION__, __LINE__, electricity_k,electricity_t,electricity);
            }
            else
            {
                lib_uart_printf("%s(%d):I Flag Error", __FUNCTION__,__LINE__);
            }

            if((dealBuffer[UART_IND_FG]&0x10) == 0x10)
            {
                powerNewFlag = 0;
                power_k = ((dealBuffer[UART_IND_PK] << 16)|(dealBuffer[UART_IND_PK+1] << 8)|(dealBuffer[UART_IND_PK+2]));//功率系数
                power_t = ((dealBuffer[UART_IND_PT] << 16)|(dealBuffer[UART_IND_PT+1] << 8)|(dealBuffer[UART_IND_PT+2]));//功率周期
                
                if(am_ty_sw_need_updata(power_a,power_t))
                {
                    am_ty_sw_updata_VIP_value(power_a,power_t);
                    powerCnt = 0;
                }
                else
                {
                    powerCnt++;
                    if(powerCnt >= COUNT_NUM)
                    {
                        powerCnt = 0;
                        am_ty_sw_updata_VIP_value(power_a,power_t);
                    }
                }
//                lib_uart_printf("%s[%d] power:%d,%d,%d", __FUNCTION__, __LINE__, power_a[0],power_a[1],power_a[2]);
                power_t = am_ty_sw_get_VIP_value(power_a);
                
                if(power_t == 0)
                {
                    power = 0;
                }
                else
                {
                    power = power_k * 100 / power_t;//功率10mw值，避免溢出
                    power = power * 10;//功率mw值
                    #if(SAMPLE_RESISTANCE_MR == 1)
                    //由于使用1mR的电阻，电流和功率需要不除以2
                    #elif(SAMPLE_RESISTANCE_MR == 2)
                    //由于使用2mR的电阻，电流和功率需要除以2
                    power >>= 1;
                    #endif
                }
                
//                lib_uart_printf("%s[%d] Pk = %d,Pt = %d,P = %d", __FUNCTION__, __LINE__, power_k,power_t,power);
            }
            else if(powerNewFlag == 0)
            {
                power_k = ((dealBuffer[UART_IND_PK] << 16)|(dealBuffer[UART_IND_PK+1] << 8)|(dealBuffer[UART_IND_PK+2]));//功率系数
                power_t = ((dealBuffer[UART_IND_PT] << 16)|(dealBuffer[UART_IND_PT+1] << 8)|(dealBuffer[UART_IND_PT+2]));//功率周期
                
                if(am_ty_sw_need_updata(power_a,power_t))
                {
                    unsigned long powerData;
                    powerData = am_ty_sw_get_VIP_value(power_a);
                    if(power_t > powerData)
                    {
                        if((power_t - powerData) > (powerData >> 2))
                        {
                            am_ty_sw_reset_VIP_value(power_a,power_t);
                        }
                    }
                }
//                lib_uart_printf("%s[%d] power:%d,%d,%d", __FUNCTION__, __LINE__, power_a[0],power_a[1],power_a[2]);
                power_t = am_ty_sw_get_VIP_value(power_a);
                
                if(power_t == 0)
                {
                    power = 0;
                }
                else
                {
                    power = power_k * 100 / power_t;//功率10mw值，避免溢出
                    power = power * 10;//功率mw值
                    #if(SAMPLE_RESISTANCE_MR == 1)
                    //由于使用1mR的电阻，电流和功率需要不除以2
                    #elif(SAMPLE_RESISTANCE_MR == 2)
                    //由于使用2mR的电阻，电流和功率需要除以2
                    power >>= 1;
                    #endif
                }
//                lib_uart_printf("%s[%d] Pk = %d,Pt = %d,P = %d",  __FUNCTION__, __LINE__, power_k,power_t,power);
            }
            
            energyFlowFlag = (dealBuffer[UART_IND_FG] >> 7);//获取当前电能计数溢出标致
            cse7759b_data.energyCurrent = ((dealBuffer[UART_IND_EN] << 8)|(dealBuffer[UART_IND_EN+1]));//更新当前的脉冲计数值
            if(cse7759b_data.energyFlowFlag != energyFlowFlag)//每次计数溢出时更新当前脉冲计数值
            {
                cse7759b_data.energyFlowFlag = energyFlowFlag;
                if(cse7759b_data.energyCurrent > cse7759b_data.energyLast)
                {
                    cse7759b_data.energyCurrent = 0;
                }
                energy_cnt = cse7759b_data.energyCurrent + ENERGY_FLOW_NUM - cse7759b_data.energyLast;
            }
            else
            {
                energy_cnt = cse7759b_data.energyCurrent - cse7759b_data.energyLast;
            }
            cse7759b_data.energyLast = cse7759b_data.energyCurrent;
            cse7759b_data.energy += (energy_cnt * 10);//电能个数累加时扩大10倍，计算电能是除数扩大10倍，保证计算精度
            
            cse7759b_data.energyUnit = 0xD693A400 >> 1;
            cse7759b_data.energyUnit /= (power_k >> 1);//1mR采样电阻0.001度电对应的脉冲个数
            #if(SAMPLE_RESISTANCE_MR == 1)
            //1mR锰铜电阻对应的脉冲个数
            #elif(SAMPLE_RESISTANCE_MR == 2)
            //2mR锰铜电阻对应的脉冲个数
            cse7759b_data.energyUnit = (cse7759b_data.energyUnit << 1);//2mR采样电阻0.001度电对应的脉冲个数
            #endif
            cse7759b_data.energyUnit = cse7759b_data.energyUnit * 10;//0.001度电对应的脉冲个数(计算个数时放大了10倍，所以在这里也要放大10倍)
            
            //电能使用量
            Q = (float)cse7759b_data.energy / (float)cse7759b_data.energyUnit;//单位是0.001度
            
            cse7759b_data.voltage = voltage / 1000;
            cse7759b_data.electricity = electricity;
            cse7759b_data.power = power;
            cse7759b_data.quantity = Q;
            
//            lib_uart_printf("%s[%d] vol:%dV elec:%dmA power:%dmW",  __FUNCTION__, __LINE__, cse7759b_data.voltage, cse7759b_data.electricity, cse7759b_data.power);
//            lib_uart_printf("%s[%d] energy:%ld Current:%d Last:%d FlowFlag:%d Unit:%d Q:%f",  __FUNCTION__, __LINE__, cse7759b_data.energy, cse7759b_data.energyCurrent, cse7759b_data.energyLast, cse7759b_data.energyFlowFlag, cse7759b_data.energyUnit, cse7759b_data.quantity);

            break;
        
        case 0xAA:
            //芯片未校准
            lib_uart_printf("%s[%d] CSE7759B not check",  __FUNCTION__, __LINE__);
            break;

        default :
            if((startFlag & 0xF1) == 0xF1)//存储区异常，芯片坏了
            {
                //芯片坏掉，反馈服务器
                lib_uart_printf("%s[%d] CSE7759B broken",  __FUNCTION__, __LINE__);
            }

            if((startFlag & 0xF2) == 0xF2)//功率异常
            {
                cse7759b_data.power = 0;//获取到的功率是以0.1W为单位
                power = 0;
                //lib_uart_printf("Power Error\r\n");
            }
            else
            {
                if((dealBuffer[UART_IND_FG]&0x10) == 0x10)
                {
                    powerNewFlag = 0;
                    power_k = ((dealBuffer[UART_IND_PK] << 16)|(dealBuffer[UART_IND_PK+1] << 8)|(dealBuffer[UART_IND_PK+2]));//功率系数
                    power_t = ((dealBuffer[UART_IND_PT] << 16)|(dealBuffer[UART_IND_PT+1] << 8)|(dealBuffer[UART_IND_PT+2]));//功率周期
                    
                    if(am_ty_sw_need_updata(power_a,power_t))
                    {
                        am_ty_sw_updata_VIP_value(power_a,power_t);
                        powerCnt = 0;
                    }
                    else{
                        powerCnt++;
                        if(powerCnt >= COUNT_NUM)
                        {
                            powerCnt = 0;
                            am_ty_sw_updata_VIP_value(power_a,power_t);
                        }
                    }
                    //lib_uart_printf("power:%d,%d,%d\r\n",power_a[0],power_a[1],power_a[2]);
                    power_t = am_ty_sw_get_VIP_value(power_a);
                    
                    if(power_t == 0)
                    {
                        power = 0;
                    }
                    else{
                        power = power_k * 100 / power_t;//功率10mw值，避免溢出
                        power = power * 10;//功率mw值
                        #if(SAMPLE_RESISTANCE_MR == 1)
                        //由于使用1mR的电阻，电流和功率需要不除以2
                        #elif(SAMPLE_RESISTANCE_MR == 2)
                        //由于使用2mR的电阻，电流和功率需要除以2
                        power >>= 1;
                        #endif
                    }
                    lib_uart_printf("%s[%d] Pk = %d,Pt = %d,P = %d", __FUNCTION__, __LINE__, power_k,power_t,power);
                }
                else if(powerNewFlag == 0)
                {
                    power_k = ((dealBuffer[UART_IND_PK] << 16)|(dealBuffer[UART_IND_PK+1] << 8)|(dealBuffer[UART_IND_PK+2]));//功率系数
                    power_t = ((dealBuffer[UART_IND_PT] << 16)|(dealBuffer[UART_IND_PT+1] << 8)|(dealBuffer[UART_IND_PT+2]));//功率周期
                    
                    if(am_ty_sw_need_updata(power_a,power_t))
                    {
                        unsigned long powerData = am_ty_sw_get_VIP_value(power_a);
                        if(power_t > powerData){
                            if((power_t - powerData) > (powerData >> 2))
                            {
                                am_ty_sw_reset_VIP_value(power_a,power_t);
                            }
                        }
                    }
                    //lib_uart_printf("power:%d,%d,%d\r\n",power_a[0],power_a[1],power_a[2]);
                    power_t = am_ty_sw_get_VIP_value(power_a);
                    
                    if(power_t == 0)
                    {
                        power = 0;
                    }
                    else
                    {
                        power = power_k * 100 / power_t;//功率10mw值，避免溢出
                        power = power * 10;//功率mw值
                        #if(SAMPLE_RESISTANCE_MR == 1)
                        //由于使用1mR的电阻，电流和功率需要不除以2
                        #elif(SAMPLE_RESISTANCE_MR == 2)
                        //由于使用2mR的电阻，电流和功率需要除以2
                        power >>= 1;
                        #endif
                    }
                    lib_uart_printf("%s[%d] Pk = %d,Pt = %d,P = %d", __FUNCTION__, __LINE__, power_k,power_t,power);
                }
            }

            if((startFlag & 0xF4) == 0xF4)//电流异常
            {
                cse7759b_data.electricity = 0;//获取到的电流以0.01A为单位
                electricity = 0;
            }
            else
            {
                if((dealBuffer[UART_IND_FG]&0x20) == 0x20)
                {
                    electricity_k = ((dealBuffer[UART_IND_IK] << 16)|(dealBuffer[UART_IND_IK+1] << 8)|(dealBuffer[UART_IND_IK+2]));//电流系数
                    electricity_t = ((dealBuffer[UART_IND_IT] << 16)|(dealBuffer[UART_IND_IT+1] << 8)|(dealBuffer[UART_IND_IT+2]));//电流周期

                    if(am_ty_sw_need_updata(electricity_a,electricity_t))
                    {
                        am_ty_sw_updata_VIP_value(electricity_a,electricity_t);
                        electricityCnt = 0;
                    }
                    else
                    {
                        electricityCnt++;
                        if(electricityCnt >= COUNT_NUM)
                        {
                            electricityCnt = 0;
                            am_ty_sw_updata_VIP_value(electricity_a,electricity_t);
                        }
                    }
                    //lib_uart_printf("electricity:%d,%d,%d\r\n",electricity_a[0],electricity_a[1],electricity_a[2]);
                    electricity_t = am_ty_sw_get_VIP_value(electricity_a);
                    
                    if(electricity_t == 0)
                    {
                        electricity = 0;
                    }
                    else
                    {
                        electricity = electricity_k * 100 / electricity_t;//电流10mA值，避免溢出
                        electricity = electricity * 10;//电流mA值
                        #if(SAMPLE_RESISTANCE_MR == 1)
                        //由于使用1mR的电阻，电流和功率需要不除以2
                        #elif(SAMPLE_RESISTANCE_MR == 2)
                        //由于使用2mR的电阻，电流和功率需要除以2
                        electricity >>= 1;
                        #endif
                    }
                    lib_uart_printf("%s[%d] Ik = %d,It = %d,I = %d", __FUNCTION__, __LINE__, electricity_k,electricity_t,electricity);
                }
                else{
                    lib_uart_printf("%s(%d):I Flag Error", __FUNCTION__, __LINE__);
                }
            }
            
            if((startFlag & 0xF8) == 0xF8)//电压异常
            {
                cse7759b_data.voltage = 0;//获取到的电压是以0.1V为单位
                voltage = 0;
            }
            else
            {
                if((dealBuffer[UART_IND_FG]&0x40) == 0x40)//获取当前电压标致，为1时说明电压检测OK
                {
                    voltage_k = ((dealBuffer[UART_IND_VK] << 16)|(dealBuffer[UART_IND_VK+1] << 8)|(dealBuffer[UART_IND_VK+2]));//电压系数
                    voltage_t = ((dealBuffer[UART_IND_VT] << 16)|(dealBuffer[UART_IND_VT+1] << 8)|(dealBuffer[UART_IND_VT+2]));//电压周期
                    
                    if(am_ty_sw_need_updata(voltage_a,voltage_t))
                    {
                        am_ty_sw_updata_VIP_value(voltage_a,voltage_t);
                        voltageCnt = 0;
                    }
                    else
                    {
                        voltageCnt++;
                        if(voltageCnt >= COUNT_NUM)
                        {
                            voltageCnt = 0;
                            am_ty_sw_updata_VIP_value(voltage_a,voltage_t);
                        }
                    }
                    //lib_uart_printf("voltage:%d,%d,%d\r\n",voltage_a[0],voltage_a[1],voltage_a[2]);
                    voltage_t = am_ty_sw_get_VIP_value(voltage_a);
                    
                    if(voltage_t == 0)
                    {
                        voltage = 0;
                    }
                    else
                    {
                        voltage = voltage_k * 100 / voltage_t;//电压10mV值，避免溢出
                        voltage = voltage * 10;//电压mV值
                    }
                    lib_uart_printf("%s[%d] Vk = %d,Vt = %d,v = %d", __FUNCTION__, __LINE__, voltage_k,voltage_t,voltage);
                }
                else
                {
                    lib_uart_printf("%s(%d):V Flag Error", __FUNCTION__, __LINE__);
                } 
            }
            lib_uart_printf("%s[%d] 0x%x:V = %d;I = %d;P = %d", __FUNCTION__, __LINE__, startFlag,voltage,electricity,power);
            break;
    }
    
    return 1;
}

// End of file : am_ty_sw_ele.c 2024-7-23 9:19:52 by: zhaoning 

