//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-15
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-15
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

#include "UART.h"
#include "osa.h"
#include "osa_mem.h"
#include "teldef.h"
// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_msgq_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_msgq_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_msgq_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE              (sizeof(sample_msgq_msg))
#define SAMPLE_MSGQ_TASK_MSGQ_QUEUE_SIZE            (200)

#define SAMPLE_MSGQ_TASK_1_THREAD_PRIO                110
#define SAMPLE_MSGQ_TASK_2_THREAD_PRIO                111

#define SAMPLE_MSGQ_TASK_1_STACK_SIZE     1024 * 2
#define SAMPLE_MSGQ_TASK_2_STACK_SIZE     1024 * 2

typedef struct _sample_msgq
{
    UINT32        msgId;
    void*        data;
    UINT32        length;
}sample_msgq_msg, *p_sample_msgq_msg;

// Private variables ------------------------------------------------------------

static OSMsgQRef    sample_msgq;

static UINT8 sample_msgq_task_1_stack[SAMPLE_MSGQ_TASK_1_STACK_SIZE];
static UINT8 sample_msgq_task_2_stack[SAMPLE_MSGQ_TASK_2_STACK_SIZE];

static OSTaskRef sample_msgq_task_1_ref = NULL;
static OSTaskRef sample_msgq_task_2_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_msgq_task_1(void *ptr);
static void sample_msgq_task_2(void *ptr);

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
    int ret;
    OS_STATUS  status;
    
    /*creat message*/
    status = OSAMsgQCreate(&sample_msgq, "sample_msgq", SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE, SAMPLE_MSGQ_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    ret = OSATaskCreate(&sample_msgq_task_1_ref, sample_msgq_task_1_stack, SAMPLE_MSGQ_TASK_1_STACK_SIZE, SAMPLE_MSGQ_TASK_1_THREAD_PRIO, "sample_msgq_task_1", sample_msgq_task_1, NULL);
    ASSERT(ret == OS_SUCCESS);
    
    ret = OSATaskCreate(&sample_msgq_task_2_ref, sample_msgq_task_2_stack, SAMPLE_MSGQ_TASK_2_STACK_SIZE, SAMPLE_MSGQ_TASK_2_THREAD_PRIO, "sample_msgq_task_2", sample_msgq_task_2, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_msgq_uart_printf("Phase2Inits_exit\n");
}


int sdk_task_send_msgq(sample_msgq_msg * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(sample_msgq, SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        sample_msgq_uart_printf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgId, status);
        if (msg->data)
        {
            free(msg->data);
        }
        ret = -1;
    }    
    
    return ret;
}

static char* sample_msgq_sdk_strdup(const char* str)
{
    char* dst;

    dst = (char*) malloc (strlen(str) + 1);
    if (dst != NULL)
    {
        strcpy(dst, str);
    }

    return dst;
}

static void sample_msgq_task_1(void *ptr)
{
    OS_STATUS  status;
    sample_msgq_msg sdkMsg = {0};

    while(1) {
        status = OSAMsgQRecv(sample_msgq, (void *)&sdkMsg, SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            sample_msgq_uart_printf("%s: sdkMsg msgId: %d\n", __FUNCTION__, sdkMsg.msgId);
            sample_msgq_uart_printf("%s: sdkMsg data: %s\n", __FUNCTION__, sdkMsg.data);
            sample_msgq_uart_printf("%s: sdkMsg length: %d\n", __FUNCTION__, sdkMsg.length);
        }
        
        if (sdkMsg.data)
            free(sdkMsg.data);
    }
}

static void sample_msgq_task_2(void *ptr)
{
    sample_msgq_msg sdkMsg = {0};    
    p_sample_msgq_msg pSdkMsg = &sdkMsg;
    
    int msgID = 0;
    
    while(1)
    {
        sample_msgq_sleep(5);
        
        memset(pSdkMsg, 0x00, sizeof(sample_msgq_msg));
        
        pSdkMsg->msgId = msgID;        
        pSdkMsg->data = (char *)sample_msgq_sdk_strdup("This is a test message!");
        pSdkMsg->length = strlen("This is a test message!");
                
        sample_msgq_uart_printf("%s: send msgID: %d \n", __FUNCTION__, msgID);
        sdk_task_send_msgq(pSdkMsg);
        
        msgID++;
    }
}

// End of file : main.c 2023-5-15 14:21:25 by: zhaoning 

