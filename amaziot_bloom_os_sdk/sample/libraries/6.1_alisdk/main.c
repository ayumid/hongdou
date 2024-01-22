//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-19
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-19
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
#include "osa_mem.h"

#include "debug_log.h"

// Private macros / types / typedef ---------------------------------------------

#define sample_alisdk_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_ALISDK_STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_alisdk_task_stack = NULL;

static OSTaskRef sample_alisdk_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_alisdk_task(void *ptr);

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
    
    sample_alisdk_task_stack = malloc(SAMPLE_ALISDK_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_alisdk_task_ref, sample_alisdk_task_stack, SAMPLE_ALISDK_STACK_SIZE, 100, "sample_alisdk_task", sample_alisdk_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}


static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
        {
            ready = 1;
        }
        catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}

extern int ali_mqtt_init(void);

static void sample_alisdk_task(void *ptr)
{
    wait_network_ready();

    ali_mqtt_init();
}

// End of file : main.c 2023-5-19 10:02:55 by: zhaoning 

