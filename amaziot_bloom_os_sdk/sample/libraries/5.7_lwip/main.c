//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
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
#include "lwip_example.h"
#include "enc28j60.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define _TASK_STACK_SIZE     1024

// Private variables ------------------------------------------------------------

static void* _task_stack = NULL;

static OSTaskRef _task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void _task(void *ptr);

// Public functions prototypes --------------------------------------------------

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

    _task_stack = malloc(_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 81, "test-task", _task, NULL);       //task优先级建议设置 > 80
    ASSERT(ret == OS_SUCCESS);
}


OSATimerRef DeleteTaskTimer = NULL;

void DeleteTaskByTimer(UINT32 id)
{
    OSA_STATUS status;

    if(_task_ref)
    {
        status = OSATaskDelete(_task_ref);
        ASSERT(status == OS_SUCCESS);

        if(_task_stack)
        {
            free(_task_stack);
            _task_stack = NULL;
        }
    }

    OSATimerDelete (DeleteTaskTimer);
}


static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){
            ready = 1;
        }
        catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}

static void _task(void *ptr)
{
    wait_network_ready();
    
    netifapi_example_setup();

    OSATimerCreate(&DeleteTaskTimer);
    OSATimerStart(DeleteTaskTimer, 400, 0, DeleteTaskByTimer, 0);       // task运行结束，需要关闭task的情况，建议这样处理，起一个2s的定时器，在定时器中删除task
}

// End of file : main.c 2023-5-17 14:11:43 by: zhaoning 

