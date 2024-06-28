//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_tm7711.c
// Auther      : zhaoning
// Version     :
// Date : 2022-1-12
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2022-1-12
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_tm7711.h"
#include "sdk_api.h"
#include "drv_3rd.h"

// Private defines / typedefs ---------------------------------------------------

#define                 PD_SCK                                 49
#define                 DOUT                                   50

#define DRV_TM7711_GPIO_HIGH               1
#define DRV_TM7711_GPIO_LOW                0

#define                 DRIVER_TM7711_DOUT_INPUT_R()                    GpioGetLevel(DOUT)

#define                 DRIVER_TM7711_PDSCK_OUTPUT_L() GpioSetLevel(PD_SCK, DRV_TM7711_GPIO_LOW)
#define                 DRIVER_TM7711_PDSCK_OUTPUT_H() GpioSetLevel(PD_SCK, DRV_TM7711_GPIO_HIGH)

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_virtual_i2c_sad_in_dir
  * Description : 模拟i2c，初始化SDA引脚为输入
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_tm7711_dout_pin_init(void)
{
    GPIOConfiguration config = {0};

    //初始化int引脚
    config.pinDir = GPIO_IN_PIN;
    config.pinPull = GPIO_PULLUP_ENABLE;
    GpioInitConfiguration(DOUT, config);

}

/**
  * Function    : drv_virtual_i2c_scl_out_dir
  * Description : 模拟i2c，初始化SCL引脚为输出
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_tm7711_pdsck_pin_init(void)
{
    GPIOReturnCode status = GPIORC_OK;
    GPIOConfiguration config = {0};

    //初始化 CS CLK MOSI
    status = GpioSetDirection(PD_SCK, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }

    //设置为高电平
    GpioSetLevel(PD_SCK, DRV_TM7711_GPIO_HIGH);
}

/**
  * Function    : drv_tm7711_read_data
  * Description : 读取tm7711数据
  * Input       : uc_next   选择下一次A/D 转换的输出数据速率和输入信号
  *               PD_SCK脉冲数         输入选择        速率
  *                    25           差分信号        10Hz
  *                    26           温度测量        40Hz
  *                    27           差分信号        40Hz
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static UINT32 drv_tm7711_read_data(UINT8 uc_next)
{
    UINT32 ul_ret_data = 0;
    int i = 0;
    
//    osCoreEnterCritical();
    for(i = 0; i < 24; i++)
    {
        DRIVER_TM7711_PDSCK_OUTPUT_H(); //在global.h 文件中定义，置SCK 脚输出高电平
//        drv_3rd_uart_printf("sck h");
        ul_ret_data <<= 1;
        drv_3rd_delay_us(1); //延时5 个微秒，根据不同的MCU 自定义此函数

        if(DRIVER_TM7711_DOUT_INPUT_R()) //判断DOUT 是否为高电平
        {
            ul_ret_data |= 1;
        }
        DRIVER_TM7711_PDSCK_OUTPUT_L();//在global.h 文件中定义，置SCK 脚输出低电平
//        drv_3rd_uart_printf("sck l");
        drv_3rd_delay_us(8);
    }
    
    switch(uc_next) //确定下一次数据更新率或者切换通道
    {
        case DRV_TM7711_CH1_10HZ:
            DRIVER_TM7711_PDSCK_OUTPUT_H();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_L();
        break;
        
        case DRV_TM7711_CH1_40HZ:
            DRIVER_TM7711_PDSCK_OUTPUT_H();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_L();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_H();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_L();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_H();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_L();
        break;
        
        case DRV_TM7711_CH2_TEMP:
            DRIVER_TM7711_PDSCK_OUTPUT_H();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_L();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_H();
            drv_3rd_delay_us(1);
            DRIVER_TM7711_PDSCK_OUTPUT_L();
        break;
        
        default:
        break;
    }
//    osCoreExitCritical();
    
    return(ul_ret_data); //返回从TM7711 中读出的数据
}

/**
  * Function    : drv_tm7711_get_val
  * Description : 获取tm7711采样值
  * Input       : uc_next   选择下一次A/D 转换的输出数据速率和输入信号
  *               data      获取的传感器数据
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 drv_tm7711_get_val(UINT8 uc_next, INT32* data)
{
    UINT32 ret = 0;
    int val = 0;
    UINT32 temp = 0;
    int i = 0;
    
    while(DRIVER_TM7711_DOUT_INPUT_R() && i < 10 )
    {
        OSATaskSleep(100);
        i++;

        if(i >= 9)
            return 101;
    }

    temp = drv_tm7711_read_data(uc_next);
//    drv_3rd_uart_printf("temp:%d",temp);
    if(temp & 0x800000)
    {
        temp -= 1;
        temp = ~temp;
        temp = temp & 0xFFFFFF;
        val = 0 - temp;
    }
    else
    {
        val = temp;
    }

    *data = val;
    
    return ret;
}

/**
  * Function    : drv_tm7711_get_weight_value
  * Description : 获取重量
  * Input       : k    二元一次方程，在坐标系中斜线的斜率
  *               b    二元一次返程，在坐标系中和Y轴交点到原点的距离
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
float drv_tm7711_get_weight_value(int k, int b)
{
    UINT32 ret = 0;
    INT32 data = 0;
    float real_value = 0;
    
    ret = drv_tm7711_get_val(DRV_TM7711_CH1_10HZ, &data);

    if(101 == ret)
    {
        drv_3rd_uart_printf("get tm7711 val err");
    }
    
    real_value = (float)data * k + b;
    real_value = real_value / 1000;

    drv_3rd_uart_printf("[MQTT] tm7711 data: %ld value: %f", data, real_value);
    

    return real_value;
}

// End of file : drv_tm7711.h 2022-1-12 14:51:33 by: zhaoning 

