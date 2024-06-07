//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : am_utils_sha1.h
// Auther      : zhaoning
// Version     :
// Date : 2021-12-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2021-12-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _UTILS_SHA1_H_
#define _UTILS_SHA1_H_

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"
#include <stdint.h>
#include <stdbool.h>

// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);
//void utils_hmac_sha1_hex(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#endif /* ifndef _UTILS_SHA1_H_.2021-12-7 11:57:45 by: zhaoning */

