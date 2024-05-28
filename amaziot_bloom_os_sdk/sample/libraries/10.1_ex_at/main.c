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
#define sample_exat_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_exat_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_exat_sleep(x) OSATaskSleep((x) * 200)//second

typedef struct
{
    int len;
    UINT8 *data;
}uartrcvParam;

typedef struct
{
    int id;
}uartParam;

#define SAMPLE_EXAT_TASK_STACK_SIZE     1024 * 2
#define SAMPLE_EXAT_RCV_TASK_STACK_SIZE     1024 * 4

// Private variables ------------------------------------------------------------

static OSMsgQRef sample_exat_rcv_msgq = NULL;
static OSMsgQRef sample_exat_msgq = NULL;

static OSATimerRef sample_exat_timer_ref = NULL;

static void* sample_exat_rcv_task_stack = NULL;
static OSTaskRef sample_exat_rcv_task_ref = NULL;
static void* sample_exat_task_stack = NULL;
static OSTaskRef sample_exat_task_ref = NULL;

static void sample_exat_rcv_uart_task(void *param);
static void sample_exat_task(void *param);

static UINT8 flg;

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

    sample_exat_uart_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&sample_exat_rcv_msgq, "sample_exat_rcv_msgq", sizeof(uartrcvParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSAMsgQCreate(&sample_exat_msgq, "sample_exat_msgq", sizeof(uartParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSATimerCreate(&sample_exat_timer_ref);
    ASSERT(status == OS_SUCCESS);
    
    sample_exat_rcv_task_stack = malloc(SAMPLE_EXAT_RCV_TASK_STACK_SIZE);
    ASSERT(sample_exat_rcv_task_stack != NULL);

    status = OSATaskCreate(&sample_exat_rcv_task_ref, sample_exat_rcv_task_stack, SAMPLE_EXAT_RCV_TASK_STACK_SIZE, 82, "sample_exat_rcv_uart_task", sample_exat_rcv_uart_task, NULL);
    ASSERT(status == OS_SUCCESS);

    sample_exat_task_stack = malloc(SAMPLE_EXAT_TASK_STACK_SIZE);
    ASSERT(sample_exat_task_stack != NULL);

    status = OSATaskCreate(&sample_exat_task_ref, sample_exat_task_stack, SAMPLE_EXAT_TASK_STACK_SIZE, 151, "sample_exat_task", sample_exat_task, NULL);
    ASSERT(status == OS_SUCCESS);
}

void sample_uarts_main_uart_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartrcvParam uart_data = {0};
    OSA_STATUS osa_status = 0;
    
    char *tempbuf = (char *)malloc(recv_len + 10);
    memset(tempbuf, 0x0, recv_len + 10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
//    sample_exat_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;    

    osa_status = OSAMsgQSend(sample_exat_rcv_msgq, sizeof(uartrcvParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

static void sample_exat_rcv_uart_task(void *param)
{
    OSA_STATUS status = 0;
    uartrcvParam uart_data = {0};
    int earfn = 0;
    int cell_id = 0;
    int primary_cell = 0;
    int rsrp = 0;
    int rsrq = 0;
    int rssi = 0;
    int snr = 0;
    int csq = 0;
    int ber = 0;
    char* pcell = NULL;
    char* pcsq = NULL;
    
    // uart uart3 uart4 默认波特率都是115200
    UART_OPEN(sample_uarts_main_uart_recv_cbk); 
//    UART_GET_CONFIG(&uartCfg);
//    uartCfg.baudrate = 9600;
//    UART_SET_CONFIG(&uartCfg);

    while(1)
    {
        memset(&uart_data, 0x00, sizeof(uartrcvParam));
        status = OSAMsgQRecv(sample_exat_rcv_msgq, (UINT8 *)&uart_data, sizeof(uartrcvParam), OSA_SUSPEND);//recv data from uart
        if (status == OS_SUCCESS)
        {
            if(NULL != uart_data.data)
            {
//                sample_exat_uart_printf("%s len:%d, data:%s", __FUNCTION__, uart_data.len, (char *)(uart_data.data)); 
                //<type> = CELL 小区信息，最多支持 5 个小区：
                //<earfcn> absolute radio-frequency channel number
                //<physical cell id> physical id of the cell
                //<primary cell> 1 indicates the current serving cell
                //<rsrp> reference signal received power
                //<rsrq> reference signal received quality
                //<rssi> received signal strength indicator
                //<snr> signal to noise ratio
                //TUESTATS:CELL,3686,14,1,73,0,-44,1
                pcell = strstr((char *)uart_data.data, "TUESTATS:CELL");
                if(NULL != pcell)
                {
                    sscanf(pcell, "TUESTATS:CELL,%d,%d,%d,%d,%d,%d", &earfn, &cell_id, &primary_cell, &rsrp, &rsrq, &rssi);
                    sample_exat_uart_printf("%s[%d]: earfn:%d, cell_id:%d, primary_cell:%d, rsrp:%d, rsrq:%d, rssi:%d", __FUNCTION__, __LINE__, earfn, cell_id, primary_cell, rsrp, rsrq, rssi); 
                }
                pcell = NULL;
                //+CSQ: 31,99
                pcsq = strstr((char *)uart_data.data, "+CSQ:");
                if(NULL != pcsq)
                {
                    sscanf(pcsq, "+CSQ: %d,%d", &csq, &ber);
                    sample_exat_uart_printf("%s[%d]: csq:%d", __FUNCTION__, __LINE__, csq); 
                }
                pcsq = NULL;
                
                free(uart_data.data);
                uart_data.data = NULL;
            }
        }
    }
}

static void sample_exat_timer_callback(UINT32 tmrId)
{
    OSA_STATUS osa_status = 0;
    uartParam uart_data = {0};

    if(0 == flg)
    {
        uart_data.id = flg;
        flg = 1;
        osa_status = OSAMsgQSend(sample_exat_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
        ASSERT(osa_status == OS_SUCCESS);
    }
    else if(1 == flg)
    {
        uart_data.id = flg;
        flg = 0;
        osa_status = OSAMsgQSend(sample_exat_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
        ASSERT(osa_status == OS_SUCCESS);
    }
}

static void sample_exat_task(void *param)
{
    OSA_STATUS status = 0;
    uartParam uart_data = {0};
    
    OSATimerStart(sample_exat_timer_ref, 3 * 200, 3 * 200, sample_exat_timer_callback, 0); // 3 seconds timer
    
    while(1)
    {
        memset(&uart_data, 0x00, sizeof(uartParam));
        status = OSAMsgQRecv(sample_exat_msgq, (UINT8 *)&uart_data, sizeof(uartParam), OSA_SUSPEND);//recv data from uart
        if(status == OS_SUCCESS)
        {
            if(0 == uart_data.id)
            {
                UART_SEND_DATA("AT+TUESTATS=\"CELL\"\r\n", strlen("AT+TUESTATS=\"CELL\"\r\n"));
//                sample_exat_uart_printf("%s[%d]: send:%s", __FUNCTION__, __LINE__, "AT+TUESTATS=\"CELL\""); 
            }
            else
            {
                UART_SEND_DATA("AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
//                sample_exat_uart_printf("%s[%d]: send:%s", __FUNCTION__, __LINE__, "AT+CSQ"); 
            }
        }
    }
}


// End of file : main.c 2023-5-16 9:39:37 by: zhaoning 

