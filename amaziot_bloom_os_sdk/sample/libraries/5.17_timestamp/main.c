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

#include "osa.h"
#include "UART.h"
#include "sys.h"
#include "sdk_api.h"
#include "UART.h"
#include "osa.h"
#include "sockets.h"
#include "netdb.h"

#include "utils_string.h"

#define WEBSOCKET_SHAKE_KEY_LEN 16

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)


#define sleep(x) OSATaskSleep((x) * 200)//second
#define _TASK_STACK_SIZE     1024*16
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

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

static void _task(void *ptr);

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

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
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

static void _task(void *ptr)
{
    UINT32 time = 0;
    int tz = 0;
    wait_network_ready();

    t_rtc st_tm = {0};

    while(1)
    {
        SDK_GET_BEIJING_TIME(&st_tm);
        sdk_uart_printf("BJ_year=%d", st_tm.tm_year);
        sdk_uart_printf("BJ_mon=%d", st_tm.tm_mon);
        sdk_uart_printf("BJ_mday=%d", st_tm.tm_mday);
        sdk_uart_printf("BJ_wday=%d", st_tm.tm_wday);
        sdk_uart_printf("BJ_hour=%d", st_tm.tm_hour);
        sdk_uart_printf("BJ_min=%d", st_tm.tm_min);
        sdk_uart_printf("BJ_sec=%d", st_tm.tm_sec);
        SDK_GET_UTC_TIME(&st_tm, &tz);
        sdk_uart_printf("UTC_year=%d", st_tm.tm_year);
        sdk_uart_printf("UTC_mon=%d", st_tm.tm_mon);
        sdk_uart_printf("UTC_mday=%d", st_tm.tm_mday);
        sdk_uart_printf("UTC_wday=%d", st_tm.tm_wday);
        sdk_uart_printf("UTC_hour=%d", st_tm.tm_hour);
        sdk_uart_printf("UTC_min=%d", st_tm.tm_min);
        sdk_uart_printf("UTC_sec=%d", st_tm.tm_sec);
        
        time = utils_utc8_2_timestamp();

        sdk_uart_printf("BJ timestamp=%ld", time);

        sleep(5);
    }
}

