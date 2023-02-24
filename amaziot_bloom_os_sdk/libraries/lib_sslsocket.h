//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_sslsocket.h
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
#ifndef _LIB_SSLSOCKET_H_
#define _LIB_SSLSOCKET_H_

// Includes ---------------------------------------------------------------------
#include <stdio.h>
#include "osa.h"
#include "utils_common.h"
#include "lib_socket.h"
#include "ssl.h"
#include "mbedtls/config-no-entropy.h"
#include "ssl_internal.h"
#include "compat-1.3.h" // for polar -> mbedtls
#include "entropy.h"
#include "ctr_drbg.h"
// Public defines / typedef -----------------------------------------------------
struct ssl_client {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config config;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
	int fd;
};
#ifdef CA_CERT
const unsigned char ca1_cert[] = {
"-----BEGIN CERTIFICATE-----\r\n"
"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\r\n"
"-----END CERTIFICATE-----\r\n"
};
#endif

#ifdef OWN_CERT
const unsigned char client_cert[] = {
};

const unsigned char client_key[] = {
};
#endif

#ifdef CA_CERT
#define HOSTNAME "asr"
#endif

// Public functions prototypes --------------------------------------------------
/**
  * Function    : lib_ssl_client_init
  * Description : SSL客户端初始化
  * Input       : 套接字句柄
  *               
  * Output      : 
  * Return      : SSL客户端实例
  * Auther      : win
  * Others      : 
  **/
struct ssl_client * lib_ssl_client_init(int fd);
/**
  * Function    : lib_ssl_client_shutdown
  * Description : 注销SSL客户端实例
  * Input       : SSL客户端实例
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_ssl_client_shutdown(struct ssl_client * client);
/**
  * Function    : lib_ssl_client_write
  * Description : SSL发送数据
  * Input       : ssl SSL实例例上下文
  *               buf 发送数据
  *               len 发送数据长度
  * Output      : 
  * Return      : 成功 0 失败 Other
  * Auther      : win
  * Others      : 
  **/
int lib_ssl_client_write(mbedtls_ssl_context *ssl, const unsigned char *buf, int len );
/**
  * Function    : lib_ssl_client_read
  * Description : SSL读取数据
  * Input       : ssl SSL实例例上下文
  *               buf 读取数据
  *               len 读取数据最大长度
  * Output      : 
  * Return      : 成功 0 失败 Other
  * Auther      : win
  * Others      : 
  **/
int lib_ssl_client_read( mbedtls_ssl_context *ssl, unsigned char *buf, int len );


#endif /* ifndef _LIB_SSLSOCKET_H_.2021-12-8 10:24:30 by: win */

