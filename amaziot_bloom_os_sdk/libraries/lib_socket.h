//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_socket.h
// Auther      : win
// Version     :
// Date : 2021-11-24
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-24
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _LIB_SOCKET_H_
#define _LIB_SOCKET_H_

// Includes ---------------------------------------------------------------------
#include "sockets.h"
#include "netdb.h"

// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
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
int lib_create_socket(char *ip, int port);
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
int lib_create_socket_udp(char *ip, int port,struct sockaddr_in* s_addr);


#if LWIP_COMPAT_SOCKETS
/**
  * Function    : lib_accept
  * Description : 接收一个套接字中已建立的连接
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_accept(a,b,c)         lwip_accept(a,b,c)
/**
  * Function    : lib_bind
  * Description : 将本地地址与一套接口捆绑
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_bind(a,b,c)           lwip_bind(a,b,c)
/**
  * Function    : lib_shutdown
  * Description : 禁止在一个套接口上进行数据的接收与发送
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_shutdown(a,b)         lwip_shutdown(a,b)
/**
  * Function    : lib_closesocket
  * Description : 释放套接口描述字
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_closesocket(s)        lwip_close(s)
/**
  * Function    : lib_connect
  * Description : 建立与指定socket的连接
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_connect(a,b,c)        lwip_connect(a,b,c)
/**
  * Function    : lib_getsockname
  * Description : 获取一个套接字的名字
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_getsockname(a,b,c)    lwip_getsockname(a,b,c)
/**
  * Function    : lib_getpeername
  * Description : 获取与套接口相连的端地址
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_getpeername(a,b,c)    lwip_getpeername(a,b,c)
/**
  * Function    : lib_setsockopt
  * Description : 设置套接口选项值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_setsockopt(a,b,c,d,e) lwip_setsockopt(a,b,c,d,e)
/**
  * Function    : lib_getsockopt
  * Description : 获取套接口选项值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_getsockopt(a,b,c,d,e) lwip_getsockopt(a,b,c,d,e)
/**
  * Function    : lib_listen
  * Description : 让一个套接字处于监听到来的连接请求的状态
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_listen(a,b)           lwip_listen(a,b)
/**
  * Function    : lib_recv
  * Description : 接收远端主机经指定的socket 传来的数据, 并把数据存到由参数buf 指向的内存空间
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_recv(a,b,c,d)         lwip_recv(a,b,c,d)
/**
  * Function    : lib_recvfrom
  * Description : 从（已连接）套接口上接收数据，并捕获数据发送源的地址
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_recvfrom(a,b,c,d,e,f) lwip_recvfrom(a,b,c,d,e,f)
/**
  * Function    : lib_send
  * Description : 向一个已经连接的socket发送数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_send(a,b,c,d)         lwip_send(a,b,c,d)
/**
  * Function    : lib_sendto
  * Description : 将数据由指定的socket 传给对方主机
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_sendto(a,b,c,d,e,f)   lwip_sendto(a,b,c,d,e,f)
/**
  * Function    : lib_socket
  * Description : 创建套接字
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_socket(a,b,c)         lwip_socket(a,b,c)
/**
  * Function    : lib_select
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_select(a,b,c,d,e)     lwip_select(a,b,c,d,e)
/**
  * Function    : lib_eventfd
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_eventfd(a, b)         lwip_eventfd(a,b)
/**
  * Function    : lib_ioctlsocket
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_ioctlsocket(a,b,c)    lwip_ioctl(a,b,c)
/**
  * Function    : lib_socket_with_callback
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_socket_with_callback(a,b,c,d)   lwip_socket_with_callback(a,b,c,d)

/**
  * Function    : lib_readv
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_readv(s,iov,iovcnt)             lwip_readv(s,iov,iovcnt)
/**
  * Function    : lib_writev
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_writev(s,iov,iovcnt)            lwip_writev(s,iov,iovcnt)
/**
  * Function    : lib_recvmsg
  * Description : 用来接收远程主机经指定的socket 传来的数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_recvmsg(s,message,flags)        lwip_recvmsg(s,message,flags)
/**
  * Function    : lib_sendmsg
  * Description : 发送消息到另一个套接字
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_sendmsg(s,message,flags)        lwip_sendmsg(s,message,flags)

#if LWIP_POSIX_SOCKETS_IO_NAMES
/**
  * Function    : lib_read
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_read(a,b,c)           lwip_read(a,b,c)
/**
  * Function    : lib_write
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_write(a,b,c)          lwip_write(a,b,c)
/**
  * Function    : lib_close
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_close(s)              lwip_close(s)
/**
  * Function    : lib_fcntl
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_fcntl(a,b,c)          lwip_fcntl(a,b,c)
/**
  * Function    : lib_eventfd
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_eventfd(a, b)         lwip_eventfd(a,b)
#endif /* LWIP_POSIX_SOCKETS_IO_NAMES */
#endif

#if LWIP_COMPAT_SOCKETS
/**
  * Function    : lib_gethostbyname
  * Description : 从域名查询其IP地址的功能
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_gethostbyname(name) lwip_gethostbyname(name)
/**
  * Function    : lib_gethostbyname_with_pcid
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_gethostbyname_with_pcid(name, pcid) lwip_gethostbyname_with_pcid(name, pcid)
/**
  * Function    : lib_gethostbyname_r
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_gethostbyname_r(name, ret, buf, buflen, result, h_errnop) lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop)
/**
  * Function    : lib_gethostbyname_r_with_pcid
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_gethostbyname_r_with_pcid(name, ret, buf, buflen, result, h_errnop, pcid) lwip_gethostbyname_r_with_pcid(name, ret, buf, buflen, result, h_errnop, pcid)
/**
  * Function    : lib_gethostbyname_with_netif
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_gethostbyname_with_netif(name, netif_name) lwip_gethostbyname_with_netif(name, netif_name)
/**
  * Function    : lib_gethostbyname_r_with_netif
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_gethostbyname_r_with_netif(name, ret, buf, buflen, result, h_errnop, netif_name) lwip_gethostbyname_r_with_netif(name, ret, buf, buflen, result, h_errnop, netif_name)
/**
  * Function    : lib_freeaddrinfo
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_freeaddrinfo(addrinfo) lwip_freeaddrinfo(addrinfo)
/**
  * Function    : lib_getaddrinfo
  * Description : 处理名字到地址以及服务到端口这两种转换
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_getaddrinfo(nodname, servname, hints, res) lwip_getaddrinfo(nodname, servname, hints, res)
/**
  * Function    : lib_getaddrinfo_with_pcid
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_getaddrinfo_with_pcid(nodname, servname, hints, res, pcid) lwip_getaddrinfo_with_pcid(nodname, servname, hints, res, pcid)       
/**
  * Function    : lib_getaddrinfo_with_netif
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_getaddrinfo_with_netif(nodname, servname, hints, res, netif_name) lwip_getaddrinfo_with_netif(nodname, servname, hints, res, netif_name)       
#endif /* LWIP_COMPAT_SOCKETS */

#endif /* ifndef _LIB_SOCKET_H_.2021-11-24 15:26:54 by: win */

