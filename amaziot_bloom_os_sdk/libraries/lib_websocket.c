//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_websocket.c
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

#include "lib_websocket.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : lib_webSocket_connectServer
  * Description : 建立websocket连接
  * Input       : ip:服务器IP
  *				  port:服务器port
  *				  interface_path:接口参数
  *				  sslFlag:加密模式: 0 不加密 1 ssl加密
  *               
  * Output      : 
  * Return      : 成功返回websocket描述符
  * Auther      : win
  * Others      : 
  **/
int lib_webSocket_connectServer(char *ip, int port, char *interface_path, int sslFlag)
{
	return webSocket_clientLinkToServer(ip,port,interface_path,sslFlag);
}
/**
  * Function    : lib_webSocket_send
  * Description : 发送websocket数据
  * Input       : fd:websocket描述符
  *			      data:发送数据
  *               dataLen:发送数据长度
  *               type：数据格式
  *               
  * Output      : 
  * Return      : 成功 0 失败 Other
  * Auther      : win
  * Others      : 
  **/
int lib_webSocket_send(int fd,char *data,int dataLen,bool isMask,WebsocketData_Type type)
{
	return webSocket_send(fd,data,dataLen,isMask,type);
}
/**
  * Function    : lib_webSocket_recv
  * Description : 接收websocket数据
  * Input       : fd:websocket描述符
  *			      data:接收数据
  *               dataLen:接收数据长度
  *               type：数据格式
  *               
  * Output      : 
  * Return      : 成功 0 失败 Other
  * Auther      : win
  * Others      : 
  **/
int lib_webSocket_recv(int fd,char *data,int dataMaxLen,WebsocketData_Type *dataType)
{
	return webSocket_recv(fd,data,dataMaxLen,dataType);
}
/**
  * Function    : webSocket_close
  * Description : 关闭websocket
  * Input       : fd:websocket描述符
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_webSocket_close(int fd)
{
	websocket_close(fd);
}
// End of file : lib_websocket.h 2021-12-8 10:24:55 by: win 

