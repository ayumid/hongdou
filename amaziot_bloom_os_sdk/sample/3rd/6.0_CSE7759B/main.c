//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-16
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-16
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "osa.h"
#include "UART.h"
#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "sys.h"
#include "sdk_api.h"

#include "am_ty_sw_ele.h"
#include "drv_ssd1315_oled.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_cse7759b_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_cse7759b_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN           32
typedef struct
{
    int port;   // 0:uart, 1:uart3, 2:uart4
    int len;
    UINT8 *data;
}uartParam;

typedef struct
{
    unsigned short  voltage;//当前电压值，单位为0.1V
    unsigned short  electricity;//当前电流值,单位为0.01A
    unsigned short  power;//当前电流值,单位为0.01A
    float quantity;
}eleParam;

#define SAMPLE_CSE7759B_TASK_STACK_SIZE     1024 * 8
//#define sample_ssd1315_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_CSE7759B_OLED_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

static OSMsgQRef sample_cse7795b_msgq = NULL;
static OSMsgQRef sample_cse7795b_oled_msgq = NULL;

static void* sample_cse7795b_task_stack = NULL;
static OSTaskRef sample_cse7795b_task_ref = NULL;

OSATaskRef     sample_cse7759b_oled_task_ref = NULL;
static void* sample_cse7759b_oled_stack_ptr = NULL;

static void sample_cse7759b_uart_thread(void *param);
static void sample_cse7759b_oled_task(void *param);

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

/*
0: 1.8v -- default
1: 2.8V
2: 3.3V
*/
extern void UART_INIT_VOLT(int voltType);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
    // 设置UART初始化电平，只能在Phase1Inits_enter调用
    //UART_INIT_VOLT(2);
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    OSA_STATUS status;

    sample_cse7759b_uart_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&sample_cse7795b_msgq, "sample_cse7795b_msgq", sizeof(uartParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    sample_cse7795b_task_stack = malloc(SAMPLE_CSE7759B_TASK_STACK_SIZE);
    ASSERT(sample_cse7795b_task_stack != NULL);

    status = OSATaskCreate(&sample_cse7795b_task_ref, sample_cse7795b_task_stack, SAMPLE_CSE7759B_TASK_STACK_SIZE, 82, "sample_cse7759b_uart_thread", sample_cse7759b_uart_thread, NULL);
    ASSERT(status == OS_SUCCESS);    

    status = OSAMsgQCreate(&sample_cse7795b_oled_msgq, "sample_cse7795b_oled_msgq", sizeof(eleParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    sample_cse7759b_oled_stack_ptr = malloc(SAMPLE_CSE7759B_OLED_STACK_SIZE);
    ASSERT(sample_cse7759b_oled_stack_ptr != NULL);
    
    status = OSATaskCreate(&sample_cse7759b_oled_task_ref, sample_cse7759b_oled_stack_ptr, SAMPLE_CSE7759B_OLED_STACK_SIZE, 100, "cse7759b_oled_task", sample_cse7759b_oled_task, NULL);
    ASSERT(status == OS_SUCCESS);    
}

void sample_uarts_main_uart_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;

//    UART_SEND_DATA(recv_data, recv_len);
    char *tempbuf = (char *)malloc(recv_len);
    memset(tempbuf, 0x0, recv_len);
    memcpy(tempbuf, (char *)recv_data, recv_len);
//    
//    sample_cse7759b_uart_printf("%s[%d]: recv_len:%d", __FUNCTION__, __LINE__, recv_len); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 0;
    
    osa_status = OSAMsgQSend(sample_cse7795b_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

//void sample_uarts_uart3_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
//{    
//    uartParam uart_data = {0};
//    OSA_STATUS osa_status;
//    
//    char *tempbuf = (char *)malloc(recv_len+10);
//    memset(tempbuf, 0x0, recv_len+10);
//    memcpy(tempbuf, (char *)recv_data, recv_len);
//    
    //sample_cse7759b_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
//    uart_data.data = (UINT8 *)tempbuf;
//    uart_data.len = recv_len;
//    uart_data.port = 1;

//    osa_status = OSAMsgQSend(sample_cse7795b_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
//    ASSERT(osa_status == OS_SUCCESS);
//}

static void sample_cse7759b_uart_thread(void *param)
{
    OSA_STATUS status;
    uartParam uart_data = {0};
    uartParam uart_sdata = {0};
    s_uart_cfg uartCfg = {0};
    s_uart_cfg uart3Cfg = {0};
    s_uart_cfg uart4Cfg = {0};
    int i = 0;
    float q_temp = 0.0;
    
    CSE7759B_DATA_T* data = NULL;
    
    // uart uart3 uart4 默认波特率都是115200
    UART_OPEN(sample_uarts_main_uart_recv_cbk); 
    UART_GET_CONFIG(&uartCfg);
    uartCfg.baudrate = 4800;
    UART_SET_CONFIG(&uartCfg);

//    UART3_OPEN(sample_uarts_uart3_recv_cbk); 
//    UART3_GET_CONFIG(&uart3Cfg);
//    uart3Cfg.baudrate = 115200;
//    UART3_SET_CONFIG(&uart3Cfg);    
    
    while (1)
    {
        memset(&uart_data, 0x00, sizeof(uartParam));        
        status = OSAMsgQRecv(sample_cse7795b_msgq, (UINT8 *)&uart_data, sizeof(uartParam), OSA_SUSPEND);    //recv data from uart
        if (status == OS_SUCCESS)
        {
            if (uart_data.data)
            {
//                for(i = 0; i < uart_data.len; i++)
//                {
//                    sample_cse7759b_uart_printf("%02X", uart_data.data[i]);
//                }
                
                if (uart_data.port == 0)
                    am_ty_sw_deal_uart_data(uart_data.data,uart_data.len);
//                    UART_SEND_DATA(uart_data.data,uart_data.len);
//                else if (uart_data.port == 1)
//                    UART3_SEND_DATA(uart_data.data,uart_data.len);
//                else if (uart_data.port == 2)
//                    UART4_SEND_DATA(uart_data.data,uart_data.len);

                data = lib_get_cse7759b_data();
//                sample_cse7759b_uart_printf("%s[%d] V:%dV I:%dA P:%dW Q:%fKwh", __FUNCTION__, __LINE__, data->voltage, data->electricity / 1000, data->power / 1000, data->quantity * 0.001);
//                sample_cse7759b_uart_printf("%s[%d] V:%dV I:%dmA P:%dmW Q:%fwh", __FUNCTION__, __LINE__, data->voltage, data->electricity, data->power, data->quantity);
                
                if((data->quantity - q_temp) > 0.000001)
                {
                    eleParam *param = malloc(sizeof(eleParam));
                    memset(param, 0x0, sizeof(eleParam));
                    param->voltage = data->voltage;
                    param->electricity = data->electricity;
                    param->power = data->power;
                    param->quantity = data->quantity;

                    uart_sdata.data = (UINT8 *)param;
                    uart_sdata.len = sizeof(eleParam);
                    
                    status = OSAMsgQSend(sample_cse7795b_oled_msgq, sizeof(uartParam), (UINT8*)&uart_sdata, OSA_NO_SUSPEND);
                    ASSERT(status == OS_SUCCESS);
                }
                
                q_temp = data->quantity;
                
                free(uart_data.data);
                uart_data.data = NULL;
            }
        }
    }
}

/**
  * Function    : sample_cse7759b_oled_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_cse7759b_oled_task(void *param)
{
    void* data = NULL;
    OSA_STATUS status = OS_SUCCESS;
    uartParam uart_data = {0};
    eleParam* eleparam = NULL;
    char vol[SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN] = {0};
    char curr[SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN] = {0};
    char power[SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN] = {0};
    char quant[SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN] = {0};
    drv_ssd1315_init();//初始化OLED
    drv_ssd1315_color_turn(0);//0正常显示，1 反色显示
    drv_ssd1315_display_turn(0);//0正常显示 1 屏幕翻转显示
    
    while(1)
    {  
        status = OSAMsgQRecv(sample_cse7795b_oled_msgq, (UINT8 *)&uart_data, sizeof(eleParam), OSA_SUSPEND);
        if (status == OS_SUCCESS)
        {
            if (uart_data.data != NULL)
            {
                eleparam = (eleParam*)uart_data.data;
//                sample_cse7759b_uart_printf("%s[%d] V:%dV I:%dmA P:%dmW Q:%fwh", __FUNCTION__, __LINE__, eleparam->voltage, eleparam->electricity, eleparam->power, eleparam->quantity);
                drv_ssd1315_clear();
                //显示汉字
                drv_ssd1315_show_chinese(0,0,14,16,1);//
                drv_ssd1315_show_chinese(18,0,15,16,1);//
                drv_ssd1315_show_chinese(0,16,16,16,1);//
                drv_ssd1315_show_chinese(18,16,17,16,1);//
                drv_ssd1315_show_chinese(0,32,18,16,1);//
                drv_ssd1315_show_chinese(18,32,19,16,1);//
                drv_ssd1315_show_chinese(0,48,20,16,1);//
                drv_ssd1315_show_chinese(18,48,21,16,1);//
                
                //显示字符，数字
                if(eleparam->power > 0)
                {
                    sample_cse7759b_uart_printf("%s[%d] V:%dV I:%dmA P:%dmW Q:%fwh", __FUNCTION__, __LINE__, eleparam->voltage, eleparam->electricity, eleparam->power, eleparam->quantity);
                    snprintf(vol, SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN, ":%dV", eleparam->voltage);
                    snprintf(curr, SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN, ":%dmA", eleparam->electricity);
                    snprintf(power, SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN, ":%dmW", eleparam->power);
                    snprintf(quant, SAMPLE_CSE7759B_DISPLAY_BUF_MAX_LEN, ":%fkWh", eleparam->quantity / 1000);
                    drv_ssd1315_show_string(32, 0, (UINT8*)vol, 16, 1);
                    drv_ssd1315_show_string(32, 16, (UINT8*)curr, 16, 1);
                    drv_ssd1315_show_string(32, 32, (UINT8*)power, 16 ,1);  
                    drv_ssd1315_show_string(32, 48, (UINT8*)quant, 16, 1);
                }
                drv_ssd1315_refresh();
                
                free(uart_data.data);
                uart_data.data = NULL;
            }
        }
    }
}

// End of file : main.c 2023-5-16 9:39:37 by: zhaoning 

