//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_os_mqtt.h
// Auther      : win
// Version     :
// Date : 2021-12-9
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-9
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _LIB_OS_MQTT_H_
#define _LIB_OS_MQTT_H_

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
#include "base64.h"
#include "utils_common.h"

#include "lib_mqtt.h"

// Public defines / typedef -----------------------------------------------------
#define DEV_TOKEN_LEN 256
#define DEV_TOKEN_VERISON_STR "2018-10-31"

// Public functions prototypes --------------------------------------------------
/**
  * Function    : lib_get_os_mqtt_token
  * Description : 获取OneNetStudio的MQTT的鉴权token
  * Input       : token        鉴权token缓存地址
  *				  exp_time     访问过期时间
  *				  product_id   产品ID
  *				  dev_name     设备名称
  *				  access_key   OneNET为资源分配的访问密钥(产品级、设备级均可)      
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
int lib_get_os_mqtt_token(char *token, unsigned long exp_time, const char *product_id, const char *dev_name, const char *access_key);

#endif /* ifndef _LIB_OS_MQTT_H_.2021-12-9 12:06:02 by: win */

