//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_websocket.c
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
#include "lib_websocket.h"
#ifdef INCL_EXAMPLES_WEBSOCKET
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1024*256
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
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

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

}
static void _task(void *ptr)
{
    int ret, running = 1;
    int fd;
    char buff[4096];

	ret = lib_net_tcpip_ok(50);
	if(ret != 0){
		 sdk_uart_printf("join net error!\n");
	}	
    
    sdk_uart_printf("\n\n\nSuccess in the net \n\n\n");
    char ip[] = "52.80.19.131";
    int port = 8080;
    char buf[1024];

    memset(buff, 0, sizeof(buff));
    sprintf(buff, "Say hi~ from client");
res:
    if (((fd = lib_webSocket_connectServer(ip, port, "api/ws",1)) <= 0)) //1-ssl  
    {
        sdk_uart_printf("client link to server failed !\r\n");
        return ;
    }else
    {
        sdk_uart_printf("websocket connect success! \n");
    }
    sdk_uart_printf("fd:%d\n",fd);
    ret = lib_webSocket_send(fd, buff, strlen(buff), true, WDT_TXTDATA);
    if(ret <= 0) 
        sdk_uart_printf("websocet send err\n");
    else
        sdk_uart_printf("websocket send buf:%s\n",buff);
    while(running){


        memset(buf, 0, sizeof(buf));
        ret = lib_webSocket_recv(fd,buf,sizeof(buf) - 1,NULL);
        if (ret <= 0) {
            sdk_uart_printf("lib_webSocket_recv: recv error: %d", ret);
            if(ret == -30848)
            {
                lib_webSocket_close(fd);
                goto res;
            }
			break;
        } 
        sdk_uart_printf("lib_webSocket_recv buf: %s,ret:%d\n",buf,ret);	
      
     
    }
    
    lib_webSocket_close(fd);
    sleep(5);
	return;
}
#endif /* ifdef INCL_EXAMPLES_WEBSOCKET.2021-12-8 15:14:21 by: win */

// End of file : examples_websocket.h 2021-12-8 15:02:11 by: win 

