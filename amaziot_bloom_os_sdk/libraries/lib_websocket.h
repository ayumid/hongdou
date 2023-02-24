//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_websocket.h
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _LIB_WEBSOCKET_H_
#define _LIB_WEBSOCKET_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "websocket.h"
// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
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
int lib_webSocket_connectServer(char *ip, int port, char *interface_path, int sslFlag);
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
int lib_webSocket_send(int fd,char *data,int dataLen,bool isMask,WebsocketData_Type type);
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
int lib_webSocket_recv(int fd,char *data,int dataMaxLen,WebsocketData_Type *dataType);
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
void lib_webSocket_close(int fd);


#endif /* ifndef _LIB_WEBSOCKET_H_.2021-12-8 10:25:03 by: win */

