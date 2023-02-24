//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_ali_http.h
// Auther      : win
// Version     :
// Date : 2022-3-7
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2022-3-7
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _LIB_ALI_HTTP_H_
#define _LIB_ALI_HTTP_H_

// Includes ---------------------------------------------------------------------
#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include "osa.h"
#include "sys.h"
#include "teldef.h"
#include "telmqtt.h"
#include "atnetssl.h"
#include "lib_mqtt.h"
#include "utils_common.h"

// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
/**
  * Function    : lib_ali_http_authenticate
  * Description : 阿里云鉴权
  * Input       : product_key    产品唯一标识
  *               device_name    设备名称
  *               device_secret  设备秘钥
  *               product_secret 产品秘钥
  *               
  * Output      : 
  * Return      : 0 成功 -1失败
  * Auther      : win
  * Others      : 
  **/
int lib_ali_http_authenticate(char*		product_key,char* device_name,char*	device_secret,char* product_secret);


#endif /* ifndef _LIB_ALI_HTTP_H_.2022-3-7 11:00:36 by: win */

