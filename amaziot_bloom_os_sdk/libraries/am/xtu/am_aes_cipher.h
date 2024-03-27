//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_aes_cipher.h
// Auther      : zhaoning
// Version     :
// Date : 2024-2-21
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-2-21
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_AES_CIPHER_H_
#define _AM_AES_CIPHER_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "cipher.h"

// Public defines / typedefs ----------------------------------------------------

typedef struct _AM_AES_CIPHER_S
{
    unsigned char* data;//待处理数据，存储区由调用函数提供
    unsigned char* exdata;//加解密后的输出数据指针，存储区由调用函数提供
    unsigned char* key;//加密key，存储区由调用函数提供
    unsigned char* iv;//加密iv向量，存储区由调用函数提供
    UINT16 data_len;//输入数据长度
    UINT16 exdata_len;//输出数据长度
    mbedtls_cipher_type_t type;//加密类型，参考mbedtls_cipher_type_t
    mbedtls_cipher_padding_t padding;//对齐方式，参考mbedtls_cipher_padding_t
}AM_AES_CIPHER_T;

// Public functions prototypes --------------------------------------------------

int am_aes_cipher_encrypt(AM_AES_CIPHER_T* paes);
int am_aes_cipher_decrypt(AM_AES_CIPHER_T* paes);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_AES_CIPHER_H_.2024-2-21 9:55:12 by: zhaoning */

