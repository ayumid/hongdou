/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : am_tcptrans.c
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "sys.h"

#include "am_tcptrans.h"
#include "am_file.h"
#include "am_gpio.h"
#include "am_at_cmds.h"
#include "utils_common.h"
#include "utils_string.h"


#ifdef INCL_MOUDULES_MT_TCP
extern trans_conf transconf;//weihu yi ge save in flash

//uart
extern OSFlagRef _sendflag_ref;
extern unsigned int send_count;
extern char sendBuf[4096];


/***********************************************************************
*	SOCKET FUNCTION PROTOTYPES
***********************************************************************/
#define RECV_MAX_BUFF 2048 //socket revBuf length
OSMsgQRef	 MsgqsockRcv;
socketParam sockGroup = {0};
struct sockaddr_in nDestAddr;//for UDP param

/***********************************************************************
*	LOCAL FUNCTION PROTOTYPES
***********************************************************************/
static void sockrcv_thread(void);
static void cachercv_thread(void);
static int socket_init(void *param);
static int Msg_tcp_client_recv(int fd,void *sock);
static void pre_serverhandle(int len , char *rcvdata);
/***********************************************************************
*	SOCKET FUNCTION RELATED DEFINE
***********************************************************************/
/*function for socket init */
void init_socket_conf(void)
{
	int ret;
	//init socket recv queue
	ret = OSAMsgQCreate(&MsgqsockRcv, "MsgqsockRcv", sizeof(sockRcvType), 500, OS_FIFO);
    DIAG_ASSERT(ret == OS_SUCCESS);
	//socket recv
	sys_thread_new("sockrcv_thread", sockrcv_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 161);
	//cache thread
	sys_thread_new("cachercv_thread", cachercv_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 161);
	
}
/*thread for socket rev */
static void sockrcv_thread(void)
{    
    sockRcvType sock_temp;
    int rcv = 0;
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);
    while (1){
        memset(&sock_temp,0,sizeof(sockRcvType));
        OSAMsgQRecv(MsgqsockRcv, (UINT8 *)&sock_temp, sizeof(sockRcvType), OSA_SUSPEND);
        printf("debug> %s -- %u,  fd=%u",__FUNCTION__,__LINE__,sock_temp.fd);
        rcv = Msg_tcp_client_recv(sock_temp.fd, (void *)&sock_temp);
         
         if(-1 == rcv)
         {
             printf("debug> %s -- %u, restart socket",__FUNCTION__,__LINE__);
             if(sockGroup.fd){
                 close(sockGroup.fd);
                 sockGroup.fd = 0;
             }
         }
         
         printf("debug> %s -- %u,  sock err.",__FUNCTION__,__LINE__);
    }
}
/*thread for cache rev */
static void cachercv_thread(void)
{    
    OSA_STATUS status;
    UINT32 flag_value;
    
    while(1) {
        status = OSAFlagWait(_sendflag_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
        if (flag_value & 0x01) {
           if(sockGroup.fd){
				uint16_t send_size = send_count;
				//add link len
				if(transconf.linkflag==2 || transconf.linkflag==3)
					send_size += strlen(transconf.link);
				char *data = malloc(send_size);
		        if (data == NULL)
		        {
		        	printf("dmhtest:malloc(send_size) error ,send_size:%d",send_size);
		            return;
		        }
		        memset(data, 0, send_size);
				//add link data
				if(send_size>send_count){
					memcpy(data, transconf.link, strlen(transconf.link));
					memcpy(data+strlen(transconf.link), sendBuf, send_count);
				}else
		        	memcpy(data, sendBuf, send_count);
				socket_write(sockGroup.fd,(void *)data,send_size);	
				free(data);
			}
			memset(sendBuf, 0, sizeof(sendBuf));
			send_count = 0;
        }
    }	
}
/*function for create socket connect*/
void setSockCfn(void)
{
	int errorCnt=0;
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);
	sockGroup.socketType = transconf.type;
    sockGroup.remotePort = transconf.port;
	memcpy(sockGroup.remoteIp,transconf.ip,60);
	printf("%s --- %d, ip=%s, port=%u", __FUNCTION__,__LINE__,sockGroup.remoteIp,sockGroup.remotePort);
    while(socket_init(&sockGroup))
	{
		sleep(3);
		errorCnt++;
		sockGroup.socketType = transconf.type;
		sockGroup.remotePort = transconf.port;
		memcpy(sockGroup.remoteIp,transconf.ip,60);
		if(errorCnt>10){
			//reset
			send_to_uart("\r\nConnect Server Error! Moudle Rebooting...\r\n", 45);
			msleep(1);
			PM812_SW_RESET();
		}
	}
    
    printf("%s --- %d",__FUNCTION__,__LINE__);
}
/*function for init socket*/
static int socket_init(void *param)
{
    struct hostent* host_entry;
    int fd;
    struct sockaddr_in server;
    
    int res=-1;
    
    OSA_STATUS osaStatus;
    sockRcvType rcvType = {0};
    int result = 0;
    socketParam *sock = (socketParam *)param;
    printf("debug> %s -- %u, socketType = %u",__FUNCTION__,__LINE__,sock->socketType);

    if(sock->fd >0)
    {
        printf(" %s, create_req,  previous socket not closed!\n",__FUNCTION__);
        
    }
    
    host_entry = gethostbyname(sock->remoteIp);
    if (host_entry == NULL) {
        printf("%s,  socket error",__FUNCTION__);
        return -1;
    }
    printf("%s[],DNS gethostbyname,Get %s ip %u.%u.%u.%u\n", __FUNCTION__,sock->remoteIp, host_entry->h_addr_list[0][0] & 0xff,
    host_entry->h_addr_list[0][1] & 0xff, host_entry->h_addr_list[0][2] & 0xff, host_entry->h_addr_list[0][3] & 0xff);

	//add by dmh //add UDP
	if(sock->socketType == 0)
		fd = socket(AF_INET, SOCK_STREAM, sock->socketType);
	else
		fd = socket(AF_INET, SOCK_DGRAM, sock->socketType);
    if(fd <=0 ){
        printf("%s,  socket error",__FUNCTION__);
        return -1;
    }

	if(sock->socketType == 0){//add by dmh judge is TCP
	   server.sin_family = AF_INET;
	   server.sin_port = htons(sock->remotePort);
	   server.sin_addr.s_addr= * (UINT32 *) host_entry->h_addr_list[0];

        //sock-connect
        res = connect(fd, (struct sockaddr *)&server, sizeof(server));
        if(res <0)
        {
            printf("%s[%u],  connect error",__FUNCTION__,__LINE__);
			close(fd);//dmh20201231
            return -1;
        }
	}else{
	    nDestAddr.sin_family = AF_INET;
	    nDestAddr.sin_port = htons(sock->remotePort);
		nDestAddr.sin_len = sizeof(nDestAddr);
	    nDestAddr.sin_addr.s_addr= * (UINT32 *) host_entry->h_addr_list[0];
	}

    printf("debug> %s -- %u, fd = %u, sockGroup.fd = %u",__FUNCTION__,__LINE__,fd, sock->fd);
    sock->fd = fd;
    rcvType.fd = sock->fd;
    rcvType.sock = sock;
    printf("debug> %s -- %u, fd = %u, sockGroup.fd = %u",__FUNCTION__,__LINE__,fd, sock->fd);
    send_to_uart("\r\nCONNECT OK\r\n", 14);
    osaStatus = OSAMsgQSend(MsgqsockRcv, sizeof(sockRcvType), (UINT8*)&rcvType, OSA_NO_SUSPEND);
 	if(osaStatus == OS_SUCCESS){
        result = 0;
 	}else{
 	    printf("%s: error!send msg failed!osaStatus - %u\n", __func__, osaStatus);
        result = -1;
 	}
   return result;         
}

/*function for tcp client rev */
static int Msg_tcp_client_recv(int fd,void *sock)
{
	char buf[RECV_MAX_BUFF]={0};
	int bytes;
	fd_set master, read_fds;


    struct timeval timeout;
    timeout.tv_sec = 10;
	timeout.tv_usec = 0;
    int status;
    
    sockRcvType *sockparam = (sockRcvType *)sock;
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);

 	while(1) {
		
		FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
		status = select(fd + 1, &read_fds, NULL, NULL, &timeout);
		if(status < 0){
			printf("socket select fail");
			sleep(1);
			continue;
		}else if(status == 0){
			printf("socket select timeout");
			sleep(1);
			continue;
		}
        
        if(sockGroup.fd == 0)
            goto PRO_FAIL;
        
		if(FD_ISSET(fd, &read_fds)  >= 0){
            memset(buf,0,RECV_MAX_BUFF);
			//add by dmh judge type
			if(sockGroup.socketType == 0)
				bytes = recv(fd, buf, RECV_MAX_BUFF, 0);
			else
				bytes = recvfrom(fd, buf, RECV_MAX_BUFF, 0,(struct sockaddr *)&nDestAddr, &nDestAddr.sin_len);
			if(bytes <= 0) 
			{
				sleep(1);
				goto PRO_FAIL;
			}
			printf("%s[], transport mode,recv data length %u,%s",__FUNCTION__, bytes,buf);
            char *sendData = (char *)malloc(bytes+1);
            ASSERT(sendData != NULL);
            memcpy(sendData, buf, bytes);
            sendData[bytes] = '\0';
            pre_serverhandle(bytes,sendData);
		}
	}  

    
PRO_FAIL:
    printf(" %s -- %u socket close", __FUNCTION__,__LINE__);
    return -1;
}
/*function for server data*/
static void pre_serverhandle(int len , char *rcvdata){

    printf("%s---%d, len=%d",__FUNCTION__,__LINE__,len);
	//judge is net atcmd 
	if(utils_is_begin_with(rcvdata,transconf.cmdpw) == 1){
		process_at_cmd_mode(rcvdata,len);
	}
	else
    	send_to_uart(rcvdata, len);

}

/*function for socket write*/
int socket_write(int sockfd, const void *data, size_t data_sz)
{
    int err;
    size_t cur = 0;
	while (cur < data_sz) {
        do {
			//add by dmh judge type
			if(sockGroup.socketType == 0){
				err = write(sockfd, (const char *)data + cur, data_sz - cur);
			}
			else{
				err = sendto(sockfd, (const char *)data + cur, data_sz - cur,0,(struct sockaddr *)&nDestAddr, nDestAddr.sin_len);
			}
        } while (err < 0 && errno == EINTR);
        if (err <= 0)
            return -1;
        cur += err;
    }
    return 0;
}
#endif /* ifdef INCL_MOUDULES_MT_TCP.2022-1-25 17:31:10 by: win */



