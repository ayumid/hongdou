//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_sslsocket.c
// Auther      : win
// Version     :
// Date : 2021-12-8
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-8
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
#include "lib_sslsocket.h"

#ifdef INCL_EXAMPLES_SSLSOCKET
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     256*1024
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------
#define SSL_DATA "GET /index.html HTTP/1.1\r\nHost: www.baidu.com\r\nUser-Agent: yuge http client\r\nConnection: close\r\n\r\n"
#define SSL_DATASIZE sizeof(SSL_DATA)

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

    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    OSATimerStart(_timer_ref, 30 * 200, 30 * 200, _timer_callback, 0); // 30 seconds timer
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
    struct ssl_client * ssl_c = NULL;

    sdk_uart_printf("_testsocket:\n");

	
	sock = lib_create_socket("www.baidu.com",443);

    if (sock < 0) {
        sdk_uart_printf("_testsocket: lib_create_socket error");
        return;
    }

	if (!ssl_c){
    	ssl_c = lib_ssl_client_init(sock);		
    }
	if(ssl_c == NULL)
   		 sdk_uart_printf(" ssl_client_init ssl_c %d \n", (ssl_c == NULL));			
    if (ssl_c){
		
    	sdk_uart_printf("ssl_write\n");		
    	ret = lib_ssl_client_write(&(ssl_c->ssl), SSL_DATA, SSL_DATASIZE);
    	if (ret != SSL_DATASIZE) {
    		sdk_uart_printf("ssl_write: write error:%d",ret);
    		lib_close(sock);
    		return;
    	}
    }else{
    	ret = send(sock, SSL_DATA, SSL_DATASIZE, 0);
    	if (ret != SSL_DATASIZE) {
    		sdk_uart_printf("_testsocket: send error :%d",ret);
    		lib_close(sock);
    		return;
    	}
    }

    if (ssl_c){
    	ret = lib_ssl_client_read(&(ssl_c->ssl), (unsigned char *)buf, sizeof(buf) - 1);
    	if (ret <= 0) {
    		sdk_uart_printf("lib_ssl_client_read: read error: %d", ret);
    		lib_close(sock);
    		return;
    	}
    }else{
    	ret = recv(sock, buf, sizeof(buf) - 1, 0);
    	if (ret <= 0) {
    		sdk_uart_printf("_testsocket: recv error: %d", ret);
    		lib_close(sock);
    		return;
    	}
    }

    buf[ret] = '\0';
    sdk_uart_printf("_testsocket: data: %s", buf);
	
    if (ssl_c){
    	lib_ssl_client_shutdown(ssl_c);
    }

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

#endif /* ifdef INCL_EXAMPLES_SSLSOCKET.2021-12-8 17:56:16 by: win */

// End of file : examples_sslsocket.h 2021-12-8 17:39:06 by: win 

