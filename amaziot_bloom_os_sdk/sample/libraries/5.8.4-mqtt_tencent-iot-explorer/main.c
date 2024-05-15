//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2024-3-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-3-7
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

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "teldef.h"
#include "UART.h"
#include "sys.h"
#include "sdk_api.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_mqtt_tencent_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_mqtt_tencent_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define tencent_sleep(x) OSATaskSleep((x)*200) // second
#define TENCENT_TASK_STACK_SIZE 1024 * 16

// Private variables ------------------------------------------------------------

static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;
static void* _send_task_stack = NULL;
static OSTaskRef _send_task_ref = NULL;

static void tencent_iot_explorer_task(void *ptr);
static void tencent_iot_explorer_send_task(void *ptr);

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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

    _task_stack = malloc(TENCENT_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);

    ret = OSATaskCreate(&_task_ref, _task_stack, TENCENT_TASK_STACK_SIZE, 100, "iot_explorer_task", tencent_iot_explorer_task, NULL);
    ASSERT(ret == OS_SUCCESS);
    
//    _send_task_stack = malloc(TENCENT_TASK_STACK_SIZE);
//    ASSERT(_send_task_stack != NULL);

//    ret = OSATaskCreate(&_send_task_ref, _send_task_stack, TENCENT_TASK_STACK_SIZE, 100, "send_task", tencent_iot_explorer_send_task, NULL);
//    ASSERT(ret == OS_SUCCESS);

}

static void wait_network_ready(void)
{
    int count = 0;
    int ready = 0;

    while (!ready)
    {
        if (getCeregReady(isMasterSim0() ? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
        {
            ready = 1;
        }
        sample_mqtt_tencent_catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();

        OSATaskSleep(200);
    }
}

static void tencent_iot_explorer_task(void *ptr)
{
    int ret = 0;
    
    wait_network_ready();
    sample_mqtt_tencent_catstudio_printf("\n\n\nSuccess in the net 2\n\n\n");
    
    mqtt_sample();
}

static void tencent_iot_explorer_send_task(void *ptr)
{

    while(1)
    {
        tencent_sleep(3);
    }
}

// End of file : main.c 2024-3-7 9:03:03 by: zhaoning 

