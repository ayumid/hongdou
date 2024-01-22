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

#include "osa.h"
#include "UART.h"
#include "sdk_api.h"
#include "teldef.h"

// Private macros / types / typedef ---------------------------------------------

#undef ASSERT
#define ASSERT(cOND)    {if (!(cOND)) {utilsAssertFail(#cOND, __FILE__, (short)__LINE__, 1);}}

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define _TASK_STACK_SIZE     1280

#define WIFI_MSGQ_MSG_SIZE (sizeof(wifiDataParam))
#define WIFI_MSGQ_QUEUE_SIZE            (50)

typedef struct{
    
    int len;
    UINT8 *UArgs;
}wifiDataParam;

// Private variables ------------------------------------------------------------

static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

OSMsgQRef    wifiDataMsgQ;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;

static void _task(void *ptr);

// Public functions prototypes --------------------------------------------------

extern char* strdup(const char* str);

// Functions --------------------------------------------------------------------

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
    int ret;
    
    ret = OSAMsgQCreate(&wifiDataMsgQ, "wifiDataMsgQ", WIFI_MSGQ_MSG_SIZE, WIFI_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(ret == OS_SUCCESS);
    
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 180, "wifi-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

}

void wifi_scan_callback(UINT8 * recv_data, UINT32 recv_len)
{
    
    wifiDataParam wifi_msg;
    
    wifi_msg.len = recv_len;
    wifi_msg.UArgs = (UINT8 *)strdup((char *)recv_data);
    
    OSAMsgQSend(wifiDataMsgQ, WIFI_MSGQ_MSG_SIZE, (void*)&wifi_msg, OSA_NO_SUSPEND);
}

static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){
            ready = 1;
        }
        sdk_uart_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    wifiDataParam wifi_msg = {0, NULL};
    char *wifi_ptr = NULL;

    wait_network_ready();

    WIFI_SCAN_START(wifi_scan_callback);
/*len 38, *WIFICELLINFO: "20AB48A4E0BC",-61, 2



len 38, *WIFICELLINFO: "B8F883CE5CC7",-72, 1



len 22, *WIFICELLINFO: ,0, 0*/
    while(1){
        status = OSAMsgQRecv(wifiDataMsgQ, (void *)&wifi_msg, WIFI_MSGQ_MSG_SIZE, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);

        if(wifi_msg.len){
            wifi_ptr = (char *)wifi_msg.UArgs;
            sdk_uart_printf("len %d, %s", wifi_msg.len, wifi_ptr);
        }

        if(wifi_msg.UArgs)
            free(wifi_msg.UArgs);

    }
}

// End of file : main.c 2023-5-16 17:56:07 by: zhaoning 

