#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define _STACK_SIZE			0x2000
static void* _task_stack = NULL;

static OSTaskRef _task_ref = NULL;


OSATimerRef _task_delete_timer_ref = NULL;

static void _task(void *ptr);

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
	
	_task_stack = malloc(_STACK_SIZE);
	
    ret = OSATaskCreate(&_task_ref, _task_stack, _STACK_SIZE, 100, "_task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void DeleteTaskByTimer(UINT32 id)
{
    OSA_STATUS status;

    if(_task_ref)
    {
        status = OSATaskDelete(_task_ref);
        ASSERT(status == OS_SUCCESS);
        _task_ref = NULL;
	   
        if(_task_stack)
        {
            free(_task_stack);
            _task_stack = NULL;
        }
    }

    OSATimerDelete (_task_delete_timer_ref);
}

extern void DelayInMilliSecond(unsigned int us);
extern void Timer0_Switch(unsigned char OnOff);

static void _task(void *ptr)
{
	int count = 5;
	unsigned int current_os_ticks;
	while(count--){		
		
		OSATaskSleep(1);		// 5ms
		catstudio_printf("=====test1======count %d\n", count);
		OSATaskSleep(1);		// 5ms
		catstudio_printf("=====test2======count %d\n", count);
		
		Timer0_Switch(1);
		DelayInMilliSecond(1000);	// 1ms
		catstudio_printf("=====test3======count %d\n", count);
		DelayInMilliSecond(1000);	// 1ms
		catstudio_printf("=====test4======count %d\n", count);
		Timer0_Switch(0);
	}
	
	OSATimerCreate(&_task_delete_timer_ref);
	OSATimerStart(_task_delete_timer_ref, 400, 0, DeleteTaskByTimer, 0);
}
