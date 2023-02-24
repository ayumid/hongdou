//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_http.h
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
#ifndef _LIB_HTTP_H_
#define _LIB_HTTP_H_

// Includes ---------------------------------------------------------------------
#include "libhttpclient.h"
// Public defines / typedef -----------------------------------------------------
/**
  * Function    : lib_http_init
  * Description : 创建HTTP/HTTPS实例
  * Input       : NONE
  *               
  * Output      : 
  * Return      : HTTP/HTTPS实例
  * Auther      : win
  * Others      : 
  **/
#define lib_http_init http_client_init
/**
  * Function    : lib_http_setopt
  * Description : 配置HTTP/HTTPS
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_http_setopt http_client_setopt
/**
  * Function    : lib_http_perform
  * Description : 执行HTTP/HTTPS
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_http_perform http_client_perform
/**
  * Function    : lib_http_getinfo
  * Description : 获取HTTP/HTTPS信息
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_http_getinfo http_client_getinfo
/**
  * Function    : lib_http_shutdown
  * Description : 销毁HTTP/HTTPS实例
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_http_shutdown http_client_shutdown
/**
  * Function    : lib_http_stop
  * Description : 关闭HTTP/HTTPS实例
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
#define lib_http_stop http_client_stop

// Public functions prototypes --------------------------------------------------

#endif /* ifndef _LIB_HTTP_H_.2021-11-24 11:23:56 by: win */

