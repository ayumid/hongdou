//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-2-23
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-2-23
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
#include "sockets.h"
#include "netdb.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)
	
#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

static void _timer_callback(UINT32 tmrId);
static void _task(void *ptr);

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    OSATimerStart(_timer_ref, 2 * 200, 2 * 200, _timer_callback, 0); // 30 seconds timer
}

static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void _testsocket(void)
{
    int ret, sock = -1;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char buf[128];

    sdk_uart_printf("_testsocket:");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    ret = getaddrinfo("101.200.35.208", "8089", &hints, &result);
    if (ret != 0) {
        sdk_uart_printf("_testsocket: resolve error");
        return;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) {
            sdk_uart_printf("_testsocket: socket error");
            continue;
        }

        ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        if (ret < 0) {
            sdk_uart_printf("_testsocket: connect error");
            close(sock);
            sock = -1;
            continue;
        } else {
            break;
        }
    }

    freeaddrinfo(result);

    if (sock < 0) {
        sdk_uart_printf("_testsocket: connect error");
        return;
    }

    ret = send(sock, "amaziot", 7, 0);
    if (ret != 7) {
        sdk_uart_printf("_testsocket: send error");
        close(sock);
        return;
    }

    ret = recv(sock, buf, sizeof(buf) - 1, 0);
    if (ret <= 0) {
        sdk_uart_printf("_testsocket: recv error: %d", ret);
        close(sock);
        return;
    }
    buf[ret] = '\0';
    sdk_uart_printf("_testsocket: data: %s", buf);

    close(sock);
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;

    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        sdk_uart_printf("flag_value: %d", flag_value);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            _testsocket();
        }
    }
}

// End of file : main.h 2023-2-23 15:30:49 by: zhaoning 

