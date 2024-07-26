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

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

#include "am_ty_sw_alarm.h"
#include "am_ty_sw_n.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_power_alarm_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_power_alarm_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_power_alarm_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_TASK_STACK_SIZE     1024 * 2

#define SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE              (sizeof(app_ty_sw_msgq_msg))
#define SAMPLE_MSGQ_TASK_MSGQ_QUEUE_SIZE            (200)

// Private variables ------------------------------------------------------------

static UINT8 sample_gpio_task_stack[SAMPLE_GPIO_TASK_STACK_SIZE];
static OSTaskRef sample_gpio_task_ref = NULL;

// Public variables -------------------------------------------------------------

OSMsgQRef am_ty_sw_alarm_msgq;

// Private functions prototypes -------------------------------------------------

static void sample_power_alarm_task(void *ptr);

// Public functions prototypes --------------------------------------------------

extern UINT32 GPIO_MFPR_ADDR(UINT32 n);

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
    status = OSAMsgQCreate(&am_ty_sw_alarm_msgq, "am_ty_sw_alarm_msgq", SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE, SAMPLE_MSGQ_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);
    
    ret = OSATaskCreate(&sample_gpio_task_ref, sample_gpio_task_stack, SAMPLE_GPIO_TASK_STACK_SIZE, 120, "sample_power_alarm_task", sample_power_alarm_task, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_power_alarm_uart_printf("Phase2Inits_exit\n");
}

int app_ty_sw_send_msgq(app_ty_sw_msgq_msg * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(am_ty_sw_alarm_msgq, SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        sample_power_alarm_uart_printf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgId, status);
        if (msg->data)
        {
            free(msg->data);
        }
        ret = -1;
    }    
    
    return ret;
}

static void sample_power_alarm_task(void *ptr)
{ 
    OS_STATUS  status;
    app_ty_sw_msgq_msg sdkMsg = {0};

    lib_power_outage_alarm_init();
    
    while(1)
    {
        status = OSAMsgQRecv(am_ty_sw_alarm_msgq, (void *)&sdkMsg, SAMPLE_MSGQ_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            sample_power_alarm_uart_printf("%s: sdkMsg msgId: %d\n", __FUNCTION__, sdkMsg.msgId);
        }
        
        if (sdkMsg.data)
            free(sdkMsg.data);
    }
}

// End of file : main.c 2023-5-16 9:01:12 by: zhaoning 

