//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_socket.c
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

#include "lib_socket.h"
#include "utils_common.h"
// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : lib_create_socket
  * Description : 创建套接字连接
  * Input       : ip :服务器地址
  *               port :端口号
  * Output      : 
  * Return      : 套接字句柄
  * Auther      : win
  * Others      : 
  **/
int lib_create_socket(char *ip, int port)
{
	int ret, sock = -1;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
	char strport[16] = { 0 };
    sprintf(strport, "%d", port);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    ret = getaddrinfo(ip, strport, &hints, &result);
    if (ret != 0) {
        printf("lib_create_socket: resolve error\n");
        return sock;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) {
            printf("lib_create_socket: socket error\n");
            continue;
        }

        ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        if (ret < 0) {
            printf("lib_create_socket:socket connect error\n");
            close(sock);
            sock = -1;
            continue;
        } else {
            break;
        }
    }
	freeaddrinfo(result);
	return sock;
	
}
/**
  * Function    : lib_create_socket_udp
  * Description : 创建UDP套接字连接
  * Input       : ip :服务器地址
  *               port :端口号
  *               s_addr:套接字目标地址信息
  * Output      : 
  * Return      : 套接字句柄
  * Auther      : win
  * Others      : 
  **/
int lib_create_socket_udp(char *ip, int port,struct sockaddr_in* s_addr)
{
	int ret, sock = -1;
	struct addrinfo hints;
    struct addrinfo *result, *rp;
	char strport[16] = { 0 };
    sprintf(strport, "%d", port);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    ret = getaddrinfo(ip, strport, &hints, &result);
    if (ret != 0) {
        printf("lib_create_socket_udp: resolve error\n");
        return sock;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) {
            printf("lib_create_socket_udp: socket error\n");
            continue;
        }else{
			memcpy(s_addr,rp->ai_addr,rp->ai_addrlen);
			break;
		}
    }
	freeaddrinfo(result);
	return sock;
	
}


// End of file : lib_socket.h 2021-12-8 16:53:17 by: win 

