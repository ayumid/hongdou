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

#define _STACK_SIZE            1024*16
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

extern int app_main(void);
static void _task(void *ptr)
{
    wait_network_ready();
    
    app_main();
}

