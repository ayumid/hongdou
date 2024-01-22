#ifndef _WEBSOCKET_CLIENT_H_
#define _WEBSOCKET_CLIENT_H_
#include <stdbool.h>
#include "sys.h"        /*lwip: sys.h*/
#include "sockets.h"
#include "ip.h"
#include "netdb.h"
#include "UART.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/debug.h"
#include "mbedtls/compat-1.3.h" 
#include "library/ssl_misc.h"

struct ssl_client {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config config;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
	int fd;
};

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 2
#endif

typedef enum
{
	WDT_MINDATA = -20,
	WDT_TXTDATA = -19,
	WDT_BINDATA = -18,
	WDT_DISCONN = -17,
	WDT_PING	= -16,
	WDT_PONG	= -15,
	WDT_ERR	 	= -1,
	WDT_NULL	= 0
}WebsocketData_Type;

int webSocket_clientLinkToServer(char *ip, int port, char *interface_path, int sslFlag);

int webSocket_serverLinkToClient(int fd,char *recvBuf,int bufLen);

int webSocket_send(int fd,char *data,int dataLen,bool isMask,WebsocketData_Type type);
int webSocket_recv(int fd,char *data,int dataMaxLen,WebsocketData_Type *dataType);

void webSocket_delayms(unsigned int ms);

int webSocket_getIpByHostName(char *hostName,char *backIp);
int netCheck_setIp(char *devName,char *ip);
int netCheck_getIp(char *devName,char *ip);
void  websocket_close(int fd);
#endif