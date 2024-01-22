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

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_uarts_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_uarts_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

typedef struct{
    int port;   // 0:uart, 1:uart3, 2:uart4
    int len;
    UINT8 *data;
}uartParam;

#define SAMPLE_UARTS_TASK_STACK_SIZE     1024 * 8

// Private variables ------------------------------------------------------------

static OSMsgQRef sample_uarts_msgq = NULL;

static void* sample_uarts_task_stack = NULL;
static OSTaskRef sample_uarts_task_ref = NULL;

static void sample_uarts_uart_thread(void *param);

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

    sample_uarts_uart_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&sample_uarts_msgq, "sample_uarts_msgq", sizeof(uartParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    sample_uarts_task_stack = malloc(SAMPLE_UARTS_TASK_STACK_SIZE);
    ASSERT(sample_uarts_task_stack != NULL);

    status = OSATaskCreate(&sample_uarts_task_ref, sample_uarts_task_stack, SAMPLE_UARTS_TASK_STACK_SIZE, 82, "sample_uarts_uart_thread", sample_uarts_uart_thread, NULL);
    ASSERT(status == OS_SUCCESS);    
}

void sample_uarts_main_uart_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    sample_uarts_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 0;
    
    osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

void sample_uarts_uart3_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    sample_uarts_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 1;

    osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

void sample_uarts_uart4_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    sample_uarts_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 2;

    osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

static void sample_uarts_uart_thread(void *param)
{
    OSA_STATUS status;
    uartParam uart_data = {0};
    s_uart_cfg uartCfg = {0};
    s_uart_cfg uart3Cfg = {0};
    s_uart_cfg uart4Cfg = {0};
    
    // uart uart3 uart4 默认波特率都是115200
    UART_OPEN(sample_uarts_main_uart_recv_cbk); 
    UART_GET_CONFIG(&uartCfg);
    uartCfg.baudrate = 9600;
    UART_SET_CONFIG(&uartCfg);

    UART3_OPEN(sample_uarts_uart3_recv_cbk); 
    UART3_GET_CONFIG(&uart3Cfg);
    uart3Cfg.baudrate = 9600;
    UART3_SET_CONFIG(&uart3Cfg);    

    UART4_OPEN(sample_uarts_uart4_recv_cbk); 
    UART4_GET_CONFIG(&uart4Cfg);
    uart4Cfg.baudrate = 9600;
    UART4_SET_CONFIG(&uart4Cfg);
    
    while (1) {
        memset(&uart_data, 0x00, sizeof(uartParam));        
        status = OSAMsgQRecv(sample_uarts_msgq, (UINT8 *)&uart_data, sizeof(uartParam), OSA_SUSPEND);    //recv data from uart
        if (status == OS_SUCCESS) {
            if (uart_data.data) {
                sample_uarts_uart_printf("%s: port %d len:%d, data:%s\n", __FUNCTION__, uart_data.port, uart_data.len, (char *)(uart_data.data));
                
                if (uart_data.port == 0)
                    UART_SEND_DATA(uart_data.data,uart_data.len);
                else if (uart_data.port == 1)
                    UART3_SEND_DATA(uart_data.data,uart_data.len);
                else if (uart_data.port == 2)
                    UART4_SEND_DATA(uart_data.data,uart_data.len);
                
                free(uart_data.data);
            }
        }
    }
}

// End of file : main.c 2023-5-16 9:39:37 by: zhaoning 

