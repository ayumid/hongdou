//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-2-21
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-2-21
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
#include "drv_uart.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define sleep(x) OSATaskSleep((x) * 200)//second


static OSMsgQRef uartMsgQ;

//修改不同的串口测试
//#define                 EXAMPLES_UART_NUM                   BLOOM_OS_DRV_UART_1//at uart
#define                 EXAMPLES_UART_NUM                   BLOOM_OS_DRV_UART_3//pin38 39 gpio4 5
//#define                 EXAMPLES_UART_NUM                   BLOOM_OS_DRV_UART_4//pin143 144 gpio14 15

typedef struct{    
    int len;
    UINT8 *data;
}uartParam;

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void examples_uart_task(void *param);

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

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{

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

    catstudio_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&uartMsgQ, "uartMsgQ", sizeof(uartParam), 300, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);

    sys_thread_new("examples_uart_task", examples_uart_task, NULL, DEFAULT_THREAD_STACKSIZE*2, 181);
}

/**
  * Function    : examples_uart_rcv_cbk
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void examples_uart_rcv_cbk(UINT8 * recv_data, UINT32 recv_len)
{
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    catstudio_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    
    osa_status = OSAMsgQSend(uartMsgQ, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

/**
  * Function    : examples_uart_task
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void examples_uart_task(void *param)
{
    uartParam uart_temp;    
    OSA_STATUS status;
    UART_BaudRates baud;

    catstudio_printf("%s: task uart\n", __FUNCTION__);
    //测试串口3，需要打开下面的代码，默认14 15 开发板不通 改为53 54可以
#if 1
    UINT32 tempaddr = 0;

    volatile unsigned int value;

    catstudio_printf("uart3_thread!!!!!!!!!"); 
    //  disable gpio 14 15 uart3
    tempaddr = GPIO_MFPR_ADDR(14);
    catstudio_printf("GPIO_MFPR_ADDR(14): %p value %lx\n", (volatile UINT32 *)tempaddr ,*((volatile UINT32 *)tempaddr)); 
    *((volatile UINT32 *)tempaddr) = 0x8040;
    catstudio_printf("GPIO_MFPR_ADDR(14) value %lx\n", *((volatile UINT32 *)tempaddr)); 
    tempaddr = GPIO_MFPR_ADDR(15);
    catstudio_printf("GPIO_MFPR_ADDR(15): %p value %lx\n", (volatile UINT32 *)tempaddr, *((volatile UINT32 *)tempaddr)); 
    *((volatile UINT32 *)tempaddr) = 0x8040;
    catstudio_printf("GPIO_MFPR_ADDR(15) value %lx\n", *((volatile UINT32 *)tempaddr)); 

    //  enabel gpio 53 54 uart3
    tempaddr = GPIO_MFPR_ADDR(53);
    *((volatile UINT32 *)tempaddr) = 0xd0c2;
    catstudio_printf("GPIO_MFPR_ADDR(14) value %lx\n", *((volatile UINT32 *)tempaddr)); 
    tempaddr = GPIO_MFPR_ADDR(54);
    *((volatile UINT32 *)tempaddr) = 0xd0c2;
    catstudio_printf("GPIO_MFPR_ADDR(15) value %lx\n", *((volatile UINT32 *)tempaddr)); 
    catstudio_printf("%s: baud %d\n", __FUNCTION__, baud);
#endif
    drv_uart_init(EXAMPLES_UART_NUM, 115200, examples_uart_rcv_cbk); 
    
    baud = drv_uart_get_baud(EXAMPLES_UART_NUM);
    
    catstudio_printf("%s: baud %d\n", __FUNCTION__, baud);
    
    while (1) {
        memset(&uart_temp, 0, sizeof(uartParam));
        
        status = OSAMsgQRecv(uartMsgQ, (UINT8 *)&uart_temp, sizeof(uartParam), OSA_SUSPEND);//recv data from uart
        
        if (status == OS_SUCCESS) {
            if (uart_temp.data) {
                catstudio_printf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.data));
                
                drv_uart_send_data(EXAMPLES_UART_NUM, uart_temp.data,uart_temp.len);
                
                free(uart_temp.data);
            }
        }
    }
}

// End of file : main.h 2023-2-21 16:08:32 by: zhaoning 

