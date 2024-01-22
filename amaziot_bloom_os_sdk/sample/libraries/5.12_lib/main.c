#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"
#include "sdk_api.h"
#include "testlib.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define _STACK_SIZE            0x2000
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

static char oem_sw_ver[32] = "OEM_VERSION_V1.0";

extern void update_the_cp_ver(char *cp_ver);

void Phase1Inits_enter(void)
{
    //update_the_cp_ver(oem_sw_ver);    //可以使用AT+CPVER读取，部分客户需要提供版本给原厂生产，便于原厂识别版本号
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



static void _task(void *ptr)
{    
    test_api();
    
    OSATimerCreate(&_task_delete_timer_ref);
    OSATimerStart(_task_delete_timer_ref, 400, 0, DeleteTaskByTimer, 0);
}
