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
#include <math.h>

#include "UART.h"
#include "osa.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)
    
#define _TASK_STACK_SIZE     1280

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

#define PI 3.14159265

// Private variables ------------------------------------------------------------

static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void _timer_callback(UINT32 tmrId);
static void _task(void *ptr);

// Public functions prototypes --------------------------------------------------

extern void update_the_cp_ver(char *cp_ver);    // max length 128
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
    
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    catstudio_printf("Phase2Inits_exit OSATimerStart\n");

    OSATimerStart(_timer_ref, 10 * 200, 10 * 200, _timer_callback, 0); // 10 seconds timer
}

static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;
    double x, ret, val;
    
    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT)
        {
            static int count = 0;
            count++;
            catstudio_printf("math_test: count: %d\n", count);
            
            x = 45.0;
            val = PI / 180;
            ret = sin(x*val);
            catstudio_printf("math_test: %lf sin is %lf", x, ret);
            
            x = 60.0;
            val = PI / 180;
            ret = cos(x*val);
            catstudio_printf("math_test: %lf cos is %lf", x, ret);

            x = 1.0;
            val = 180.0 / PI;
            ret = atan (x) * val;
            catstudio_printf("math_test: %lf atan is %lf", x, ret);            
            
            if (count > 10) {
                RTI_LOG("_task: stop timer");
                OSATimerStop(_timer_ref);
            }
        }
    }
}

// End of file : main.c 2023-5-16 17:56:18 by: zhaoning 

