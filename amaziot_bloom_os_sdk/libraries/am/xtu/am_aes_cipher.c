//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_aes_cipher.c
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

// Includes ---------------------------------------------------------------------

#include "am_aes_cipher.h"
#include "am.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : am_aes_cipher_encrypt
  * Description : 
  * Input       : data     加密数据
  *               exdata   加密后的数据
  *               data_len 加密数据长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int am_aes_cipher_encrypt(AM_AES_CIPHER_T* paes)
{
    int ret = 0;
    mbedtls_cipher_context_t ctx = {0};
    const mbedtls_cipher_info_t* info = NULL;
    size_t ilen = 0;
    size_t olen = 0;
    size_t slen = 0;
    AM_AES_CIPHER_T* p_aes_cipher = NULL;

    p_aes_cipher = paes;

    if(NULL == paes->data || NULL == paes->exdata || NULL == paes->key || NULL == paes->iv || 0 == paes->data_len)
    {
        uprintf("%s[%d] params err", __FUNCTION__, __LINE__);
        return -1;
    }
    //初始化cipher结构体
    mbedtls_cipher_init(&ctx);
    //获取AES模式info
    info = mbedtls_cipher_info_from_type(p_aes_cipher->type);
    //设置cipher模式
    ret = mbedtls_cipher_setup(&ctx, info);
    if(0 != ret)
    {
        uprintf("%s[%d] setup err", __FUNCTION__, __LINE__);
        goto exit;
    }
    //设置padding模式
    ret = mbedtls_cipher_set_padding_mode(&ctx, p_aes_cipher->padding); 
    if(0 != ret)
    {
        uprintf("%s[%d] padding err", __FUNCTION__, __LINE__);
        goto exit;
    }
    //设置密钥
//#ifdef DTU_DATA_PRINTF_DATA_MODE
//    uprintf("key:%s", dtu_file_ctx->format.key);
//#endif /* ifdef DTU_DATA_PRINTF_DATA_MODE.2023-12-21 15:36:29 by: zhaoning */
    ret = mbedtls_cipher_setkey(&ctx, p_aes_cipher->key, strlen((void*)p_aes_cipher->key) * 8, MBEDTLS_ENCRYPT); 
    if(0 != ret)
    {
        uprintf("%s[%d] setkey err", __FUNCTION__, __LINE__);
        goto exit;
    }
    //设置初始化向量IV
//#ifdef DTU_DATA_PRINTF_DATA_MODE
//    uprintf("iv:%s", dtu_file_ctx->format.iv);
//#endif /* ifdef DTU_DATA_PRINTF_DATA_MODE.2023-12-21 15:36:33 by: zhaoning */
    ret = mbedtls_cipher_set_iv(&ctx, p_aes_cipher->iv, strlen((void*)p_aes_cipher->iv));
    if(0 != ret)
    {
        uprintf("%s[%d] setiv err", __FUNCTION__, __LINE__);
        goto exit;
    }
    //更新输入数据，olen = format_size / 16
    ilen = p_aes_cipher->data_len;
    ret = mbedtls_cipher_update(&ctx, p_aes_cipher->data, ilen, p_aes_cipher->exdata, &olen);
    if(0 != ret)
    {
        uprintf("%s[%d] update err", __FUNCTION__, __LINE__);
        goto exit;
    }
//#ifdef DTU_DATA_PRINTF_DATA_MODE
//    temp = malloc(encrypt_len * 2 + 1);
//    memset(temp, 0, encrypt_len * 2 + 1);
//    utils_hex2ascii_str(temp, (UINT8*)encrypt, olen);
//    uprintf("olen:%d", olen);
//    uprintf("after:%s", temp);
//    free(temp);
//#endif /* ifdef DTU_DATA_PRINTF_DATA_MODE.2023-12-25 15:00:02 by: zhaoning */
    //偏移olen = format_size / 16字节，完成最后一个16字节计算
    slen = olen;
    //完成计算，olen = 16
    ret = mbedtls_cipher_finish(&ctx, (unsigned char*)(p_aes_cipher->exdata + slen), &olen);
    if(0 != ret)
    {
        uprintf("%s[%d] finish err", __FUNCTION__, __LINE__);
        goto exit;
    }

    //打印加密后的数据，转为str后，使用uprintf输出，输出十六进制多回车换行
#if 0
    char* temp = NULL;
    UINT16 encrypt_len = 0;
    char arr_temp[33] = {0};
    UINT32 p_len = 0;
    
    encrypt_len = (data_len / 16 + 1) * 16;
    temp = malloc(encrypt_len * 2 + 1);
    p_len = encrypt_len / 16;
    if(NULL != temp)
    {
        memset(temp, 0, encrypt_len * 2 + 1);
        utils_hex2ascii_str(temp, (UINT8*)exdata, encrypt_len);
        uprintf("%s[%d] slen:%d olen:%d after encrypt len:%d", __FUNCTION__, __LINE__, slen, olen, encrypt_len);
        memset(arr_temp, 0, 17);
        for(UINT32 i = 0; i < p_len;i++)
        {
            memcpy(arr_temp, temp + 32 * i, 32);
            uprintf("%s", arr_temp);
        }
        free(temp);
    }
//    dtu_send_to_uart(encrypt, encrypt_len);
#endif /* ifdef DTU_DATA_PRINTF_DATA_MODE.2023-12-21 11:03:22 by: zhaoning */

exit:

    //释放cipher结构体
    mbedtls_cipher_free(&ctx);

    return ret;
}

/**
  * Function    : am_aes_cipher_decrypt
  * Description : 下行数据处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int am_aes_cipher_decrypt(AM_AES_CIPHER_T* paes)
{
    int ret = 0;
    mbedtls_cipher_context_t ctx = {0};
    const mbedtls_cipher_info_t* info = NULL;
    size_t olen = 0;
    size_t slen = 0;
    AM_AES_CIPHER_T* p_aes_cipher = NULL;

    p_aes_cipher = paes;

    if(NULL == paes->data || NULL == paes->exdata || NULL == paes->key || NULL == paes->iv || 0 == paes->data_len)
    {
        uprintf("%s[%d] params err", __FUNCTION__, __LINE__);
        return -1;
    }
    //初始化cipher结构体
    mbedtls_cipher_init(&ctx);
    //获取AES模式info
    info = mbedtls_cipher_info_from_type(p_aes_cipher->type);
    //设置cipher模式
    ret = mbedtls_cipher_setup(&ctx, info);
    if(0 != ret)
    {
        uprintf("%s[%d] setup err", __FUNCTION__, __LINE__);
            goto exit;
    }
    //设置padding模式
    ret = mbedtls_cipher_set_padding_mode(&ctx, p_aes_cipher->padding); 
    if(0 != ret)
    {
        uprintf("%s[%d] padding err", __FUNCTION__, __LINE__);
            goto exit;
    }
    //设置密钥
    ret = mbedtls_cipher_setkey(&ctx, p_aes_cipher->key, strlen((void*)p_aes_cipher->key) * 8, MBEDTLS_DECRYPT); 
//    mbedtls_cipher_setkey(&ctx, (unsigned char*)"]4_PeXYZI_G2KyEf", 128, MBEDTLS_DECRYPT); 
    if(0 != ret)
    {
        uprintf("%s[%d] setkey err", __FUNCTION__, __LINE__);
            goto exit;
    }
    //设置初始化向量IV
    ret = mbedtls_cipher_set_iv(&ctx, p_aes_cipher->iv, strlen((void*)p_aes_cipher->iv));
//    mbedtls_cipher_set_iv(&ctx, (unsigned char*)"-5A_#Xi-D1PpS_wD", 16);
    if(0 != ret)
    {
        uprintf("%s[%d] setiv err", __FUNCTION__, __LINE__);
            goto exit;
    }
    //更新输入数据，olen = format_size / 16
    ret = mbedtls_cipher_update(&ctx, p_aes_cipher->data, p_aes_cipher->data_len, p_aes_cipher->exdata, &olen);
    if(0 != ret)
    {
        uprintf("%s[%d] update err", __FUNCTION__, __LINE__);
            goto exit;
    }
#ifdef DTU_DATA_PRINTF_DATA_MODE
//    char* temp = NULL;
//    UINT16 dncrypt_len = 0;

//    temp = malloc(dncrypt_len * 2 + 1);
//    memset(temp, 0, dncrypt_len * 2 + 1);
//    utils_hex2ascii_str(temp, (UINT8*)data->message->payload, data->message->payloadlen);
//    
//    uprintf("dl:%s", temp);
//    free(temp);

//    uprintf("%s[%d] olen:%d before:%s", __FUNCTION__, __LINE__, olen, data->message->payload);
//    uprintf("%s[%d] after1:%s", __FUNCTION__, __LINE__, dncrypt);
#endif /* ifdef DTU_DATA_PRINTF_DATA_MODE.2023-12-25 15:00:02 by: zhaoning */
    //偏移olen = format_size / 16字节，完成最后一个16字节计算
    slen = olen;
    //完成计算，olen = 16
    ret = mbedtls_cipher_finish(&ctx, (unsigned char*)(p_aes_cipher->exdata + slen), &olen);
    if(0 != ret)
    {
        uprintf("%s[%d] finish err", __FUNCTION__, __LINE__);
            goto exit;
    }

    p_aes_cipher->exdata_len = slen + olen;
    //打印加密后的数据，转为str后，使用uprintf输出，输出十六进制多回车换行
//    uprintf("%s[%d] after2:%s", __FUNCTION__, __LINE__, dncrypt);
#ifdef DTU_DATA_PRINTF_DATA_MODE
//    char* temp = NULL;
//    temp = malloc((slen + olen) * 2 + 1);

//    if(NULL != temp)
//    {
//        memset(temp, 0, (slen + olen) * 2 + 1);
//        utils_hex2ascii_str(temp, (UINT8*)dncrypt, (slen + olen));
//        uprintf("%s[%d] slen:%d olen:%d after dncrypt len:%d", __FUNCTION__, __LINE__, slen, olen, dncrypt_len);

//        uprintf("%s[%d] after:%s", __FUNCTION__, __LINE__, temp);
//        free(temp);
//    }
#endif /* ifdef DTU_DATA_PRINTF_DATA_MODE.2023-12-21 11:03:22 by: zhaoning */

exit:

    //释放cipher结构体
    mbedtls_cipher_free(&ctx);

    return ret;
}

// End of file : am_aes_cipher.c 2024-2-21 9:55:17 by: zhaoning 

