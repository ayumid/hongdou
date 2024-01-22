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
#include "teldef.h"
#include "pmic_rtc.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_stimer_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_stimer_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_stimer_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_STIMER_TASK_STACK_SIZE     1024 * 2

#define SAMPLE_STIMER_TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------

static UINT8 sample_stimer_task_stack[SAMPLE_STIMER_TASK_STACK_SIZE];

static OSTaskRef sample_stimer_task_ref = NULL;
static OSATimerRef sample_stimer_timer_ref = NULL;
static OSFlagRef sample_stimer_flag_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_stimer_timer_callback(UINT32 tmrId);
static void sample_stimer_task_timer(void *ptr);

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
    
    ret = OSAFlagCreate(&sample_stimer_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATimerCreate(&sample_stimer_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&sample_stimer_task_ref, sample_stimer_task_stack, SAMPLE_STIMER_TASK_STACK_SIZE, 120, "sample_stimer_task_timer", sample_stimer_task_timer, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_stimer_uart_printf("Phase2Inits_exit OSATimerStart\n");
    OSATimerStart(sample_stimer_timer_ref, 10 * 200, 10 * 200, sample_stimer_timer_callback, 0); // 10 seconds timer
}

static void sample_stimer_timer_callback(UINT32 tmrId)
{
    OSAFlagSet(sample_stimer_flag_ref, SAMPLE_STIMER_TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);            // timer的回调函数不要做有延迟的处理，通过flag或者msgQ转发到task处理
}

static void sample_stimer_task_timer(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = SAMPLE_STIMER_TASK_TIMER_CHANGE_FLAG_BIT;
    int count = 0;
                
    while(1) {
        status = OSAFlagWait(sample_stimer_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        
        if (status == OS_SUCCESS){
            if (flag_value & SAMPLE_STIMER_TASK_TIMER_CHANGE_FLAG_BIT) {
                
                count++;
                sample_stimer_uart_printf("_task: count: %d\n", count);
                if (count > 10) {
                    sample_stimer_uart_printf("_task: stop timer");
                    OSATimerStop(sample_stimer_timer_ref);
                }
            }
        }
    }
}

// End of file : main.c 2023-5-15 15:26:28 by: zhaoning 

