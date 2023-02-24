//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_ali_mqtt.h
// Auther      : win
// Version     :
// Date : 2021-12-2
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-2
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _LIB_ALI_MQTT_H_
#define _LIB_ALI_MQTT_H_

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
  * Function    : lib_ali_mqtt_authenticate
  * Description : 阿里云鉴权
  * Input       : c              MQTT客户端实例
  *               product_key    产品唯一标识
  *               device_name    设备名称
  *               device_secret  设备秘钥
  *               product_secret 产品秘钥
  *               reg_flag	注册方式 0 普通连接 1 动态注册
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int lib_ali_mqtt_authenticate(mqtt_client_t *c,char*		product_key,char* device_name,char*	device_secret,char* product_secret,unsigned char reg_flag);

#endif /* ifndef _LIB_ALI_MQTT_H_.2021-12-2 15:21:46 by: win */

