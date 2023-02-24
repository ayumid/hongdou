//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_tcp.c
// Auther      : win
// Version     :
// Date : 2021-11-25
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-25
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "lib_socket.h"
#include "drv_timer.h"
#include "lib_net.h"


#ifdef INCL_EXAMPLES_TCP
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void _timer_callback(UINT32 tmrId);
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

    ret = drv_timer_init(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    drv_timer_start(_timer_ref, 30 * 200, 30 * 200, _timer_callback, 0); // 30 seconds timer
}
static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void _testsocket(void)
{
    int ret, sock = -1;
    char buf[128];

    sdk_uart_printf("_testsocket:");

	sock = lib_create_socket("101.200.35.208",8080);

    if (sock < 0) {
        sdk_uart_printf("_testsocket: lib_create_socket error");
        return;
    }

    ret = lib_send(sock, "abcdefg", 7, 0);
    if (ret != 7) {
        sdk_uart_printf("_testsocket: lib_send error");
        lib_close(sock);
        return;
    }

    ret = lib_recv(sock, buf, sizeof(buf) - 1, 0);
    if (ret <= 0) {
        sdk_uart_printf("_testsocket: lib_recv error: %d", ret);
        lib_close(sock);
        return;
    }
    buf[ret] = '\0';
    sdk_uart_printf("_testsocket: data: %s", buf);

    lib_close(sock);
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;

    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            _testsocket();
        }
    }
}

#endif /* ifdef INCL_EXAMPLES_TCP.2021-11-25 17:17:24 by: win */

// End of file : examples_tcp.h 2021-11-25 17:15:01 by: win 

