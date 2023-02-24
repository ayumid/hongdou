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
#include "teldef.h"
#include "sdk_api.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second
#define msleep(x) OSATaskSleep((x) * 20)//100*msecond
    
#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef _task_ref = NULL;
static void _task(void *ptr);

typedef struct
{
    int socket_fd;
}socket_msgq_msg;


#define SOCKET_MSGQ_MSG_SIZE              (sizeof(socket_msgq_msg))
#define SOCKET_MSGQ_QUEUE_SIZE            (4)

static OSMsgQRef    socketMsgQ;

#define RECV_TASK_STACK_SIZE     1280

// Private variables ------------------------------------------------------------

static UINT32 recv_task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef recv_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void recv_task(void *ptr);


extern BOOL IsAtCmdSrvReady(void);

extern int SendATCMDWaitResp(int sATPInd,char *in_str, int timeout, char *ok_fmt, int ok_flag,
                            char *err_fmt, char *out_str, int resplen);

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
    OEM_SET_SLEEP_GPIO_CONFIG_FALG(1);    // disable the sleep relate gpio init
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

    /*creat msgq*/
    ret = OSAMsgQCreate(&socketMsgQ, "socketMsgQ", SOCKET_MSGQ_MSG_SIZE, SOCKET_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 150, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&recv_task_ref, recv_task_stack, RECV_TASK_STACK_SIZE, 150, "recv-task", recv_task, NULL);
    ASSERT(ret == OS_SUCCESS);

}

static void wait_network_ready(void)
{

    OS_STATUS osa_status;

    int ret;
    int ready=0;
    char at_str[32]={'\0'};
    char resp_str[64]={'\0'};

    while(!IsAtCmdSrvReady()){
        sleep(1);
    }

    sprintf(at_str, "AT^SYSINFO\r");
    
    while (!ready){

        memset(resp_str, 0x00, sizeof(resp_str));
        ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, "^SYSINFO:",1,NULL, resp_str, sizeof(resp_str));
        
        sdk_uart_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
        
        if(strstr(resp_str, "^SYSINFO: 2,3") != NULL || strstr(resp_str, "^SYSINFO: 2,2") != NULL){
            ready = 1;
        }
        
        sleep(1);
    }
}


static void enable_system_sleep(void)
{
    sdk_uart_printf("enable_system_sleep\n");

    OEM_SET_ALLOW_ENTER_SLEEP_FALG(1);

    OEM_SLEEP_DISABLE_USB();

    OEM_FAST_ENABLE_UART_SLEEP();
}

static void disable_system_sleep(void)
{
    sdk_uart_printf("disable_system_sleep\n");

    OEM_SET_ALLOW_ENTER_SLEEP_FALG(0);
}

UINT16 heartbeat_ack_miss_count = 0;

static void _task(void *ptr)
{
    int ret, sock = -1;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char buf[128] = {0};
    int timeout;
    socket_msgq_msg msg;

    wait_network_ready();

    enable_system_sleep();
    
RECONNECT:
    sdk_uart_printf("_task connenct or reconnenct server\n");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    ret = getaddrinfo("101.200.35.208", "8089", &hints, &result);
    if (ret != 0) {
        sdk_uart_printf("_testsocket: resolve error\n");
        return;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) {
            sdk_uart_printf("_testsocket: socket error\n");
            continue;
        }

        timeout = 20 * 1000;    // 20s recv timeout
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        if (ret < 0) {
            sdk_uart_printf("_testsocket: connect error\n");
            close(sock);
            continue;
        } else {
            break;
        }
    }

    freeaddrinfo(result);

    if (sock < 0) {
        sdk_uart_printf("_testsocket: connect error\n");
        goto RECONNECT;
    }

    msg.socket_fd = sock;
    OSAMsgQSend(socketMsgQ, SOCKET_MSGQ_MSG_SIZE, (void*)&msg, OSA_NO_SUSPEND);

    while(1){
        ret = send(sock, "**000**", 7, 0);        // send heartbeat packet
        if (ret != 7) {
            sdk_uart_printf("_testsocket: send error\n");
            close(sock);
            goto RECONNECT;
        }
        
        sleep(60*5);    // not set timer, In sleep state, the timer is unfaithful
        heartbeat_ack_miss_count++;
        if (heartbeat_ack_miss_count >= 3){
            
            heartbeat_ack_miss_count = 0;
            close(sock);
           goto RECONNECT;
        }
    }

}

#define RECV_FD_MAX 64
static void recv_task(void *ptr)
{
    int fd = -1;
    socket_msgq_msg msg;

    int fdmax, j, bytes;
    fd_set master, read_fds;
    int errorno=0;
    char buf[128] = {0};
    struct sockaddr_in server;
    socklen_t addrlen = sizeof(server);
    
    fdmax = RECV_FD_MAX;
    
    while(1)
    {
        OSAMsgQRecv(socketMsgQ, (void *)&msg, SOCKET_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        fd = msg.socket_fd;

        FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(fd, &master);
        while (1) {
            read_fds = master;
            if (select(fdmax, &read_fds, NULL, NULL, NULL) == -1) {
                errorno=lwip_getsockerrno(fd);
                sdk_uart_printf("%s, select error %d", __FUNCTION__, errorno);
                if(errorno==ENETRESET || errorno==ECONNABORTED || errorno==ENOTCONN){
                    FD_CLR(fd, &master);
                    break;
                }
                sleep(1);
                continue;
            }

            for (j = 0; j < fdmax; j++) {
                if (!FD_ISSET(j, &read_fds)) {
                    continue;
                }

                memset(buf, 0, sizeof(buf));

                bytes = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&server, &addrlen);    // once, max recv 1460    
                sdk_uart_printf("recvfrom: bytes %d\n", bytes);
                if (bytes <= 0) {
                    close(fd);
                }else{
                    sdk_uart_printf("recvfrom: bytes %d, %s\n", bytes, buf);

                    if (strncmp("**111**", buf, 7) == 0){
                        heartbeat_ack_miss_count = 0;
                    }else if (strncmp("**222**", buf, 7) == 0){
                        disable_system_sleep();
                    }else{

                    }
                }
            }
        }
    }

}

// End of file : main.h 2023-2-23 10:02:59 by: zhaoning 

