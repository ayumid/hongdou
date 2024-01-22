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
#define sample_socket_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_socket_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)
	
#define SAMPLE_SOCKET_TASK_STACK_SIZE     1280
static UINT32 sample_socket_task_stack[SAMPLE_SOCKET_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef sample_socket_task_ref = NULL;
static OSATimerRef sample_socket_timer_ref = NULL;
static OSFlagRef sample_socket_flag_ref = NULL;

#define SAMPLE_SOCKET_TASK_TIMER_CHANGE_FLAG_BIT    0x01

static void sample_socket_timer_callback(UINT32 tmrId);
static void sample_socket_task(void *ptr);

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

    ret = OSAFlagCreate(&sample_socket_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATimerCreate(&sample_socket_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&sample_socket_task_ref, sample_socket_task_stack, SAMPLE_SOCKET_TASK_STACK_SIZE, 200, "test-task", sample_socket_task, NULL);
    ASSERT(ret == OS_SUCCESS);

    OSATimerStart(sample_socket_timer_ref, 2 * 200, 2 * 200, sample_socket_timer_callback, 0); // 30 seconds timer
}

static void sample_socket_timer_callback(UINT32 tmrId)
{
    OSAFlagSet(sample_socket_flag_ref, SAMPLE_SOCKET_TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void sample_socket_test(void)
{
    int ret, sock = -1;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char buf[128];

    sample_socket_uart_printf("sample_socket_test:");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    ret = getaddrinfo("101.200.35.208", "8089", &hints, &result);
    if (ret != 0) {
        sample_socket_uart_printf("sample_socket_test: resolve error");
        return;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) {
            sample_socket_uart_printf("sample_socket_test: socket error");
            continue;
        }

        ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        if (ret < 0) {
            sample_socket_uart_printf("sample_socket_test: connect error");
            close(sock);
            sock = -1;
            continue;
        } else {
            break;
        }
    }

    freeaddrinfo(result);

    if (sock < 0) {
        sample_socket_uart_printf("sample_socket_test: connect error");
        return;
    }

    ret = send(sock, "amaziot", 7, 0);
    if (ret != 7) {
        sample_socket_uart_printf("sample_socket_test: send error");
        close(sock);
        return;
    }

    ret = recv(sock, buf, sizeof(buf) - 1, 0);
    if (ret <= 0) {
        sample_socket_uart_printf("sample_socket_test: recv error: %d", ret);
        close(sock);
        return;
    }
    buf[ret] = '\0';
    sample_socket_uart_printf("sample_socket_test: data: %s", buf);

    close(sock);
}

static void sample_socket_task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = SAMPLE_SOCKET_TASK_TIMER_CHANGE_FLAG_BIT;

    while(1) {
        status = OSAFlagWait(sample_socket_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        sample_socket_uart_printf("flag_value: %d", flag_value);
        if (flag_value & SAMPLE_SOCKET_TASK_TIMER_CHANGE_FLAG_BIT) {
            sample_socket_test();
        }
    }
}

// End of file : main.h 2023-2-23 15:30:49 by: zhaoning 

