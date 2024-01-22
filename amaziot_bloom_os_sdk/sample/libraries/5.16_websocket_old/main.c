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

#include "ssl.h"

#include "entropy.h"
#include "ctr_drbg.h"
#include "websocket.h"
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
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;
#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

//int port = 9999;
//char ip[] = "203.156.205.55";
int port = 8080;
char ip[] = "52.80.19.131";
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
    int ret, timeCount = -1, running = 1;
    int fd;
    char buff[4096];

	wait_network_ready();
	
    catstudio_printf("\n\n\nSuccess in the net \n\n\n");
    char ip[] = "52.80.19.131";
    int port = 8080;
    char buf[1024];

    memset(buff, 0, sizeof(buff));
    sprintf(buff, "Say hi~ from client");
res:
    if (((fd = webSocket_clientLinkToServer(ip, port, "api/ws",1)) <= 0)) //1-ssl  
    {
        catstudio_printf("client link to server failed !\r\n");
        return ;
    }else
    {
        catstudio_printf("websocket connect success! \n");
    }
    catstudio_printf("fd:%d\n",fd);
  //  webSocket_delayms(100);
    ret = webSocket_send(fd, buff, strlen(buff), true, WDT_TXTDATA);
    if(ret <= 0) 
        catstudio_printf("websocet send err\n");
    else
        catstudio_printf("websocket send buf:%s\n",buff);
    while(running){

        memset(buf, 0, sizeof(buf));
        ret = webSocket_recv(fd,buf,sizeof(buf) - 1,NULL);
        if (ret <= 0) {
            catstudio_printf("_testsocket: recv error: %d", ret);
            if(ret == -30848)
            {
                websocket_close(fd);
                goto res;
            }//return;
        } 
        catstudio_printf("ssl_read buf: %s,ret:%d\n",buf,ret);	
      
		OSATaskSleep(200*3);
    }
    
    websocket_close(fd);
  //  goto res;
    sleep(5);

    return;
}

