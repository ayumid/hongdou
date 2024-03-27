//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-2-23
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-2-23
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "am_aes_cipher.h"
#include "utils_string.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_aes_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_aes_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

static OSTaskRef sample_aes_task_ref = NULL;
static void sample_aes_timer_callback(UINT32 param);
static void sample_aes_task(void *ptr);

#define SAMPLE_AES_TASK_STACK_SIZE     1024 * 10
static UINT32 sample_aes_task_stack[SAMPLE_AES_TASK_STACK_SIZE/sizeof(UINT32)];

#define dtu_sleep(x) OSATaskSleep((x) * 200)//second

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    int ret;
    sample_aes_uart_printf("enter main\n");

    ret = OSATaskCreate(&sample_aes_task_ref, sample_aes_task_stack, SAMPLE_AES_TASK_STACK_SIZE, 200, "aes-cipher-task", sample_aes_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

static void sample_aes_task(void *ptr)
{
    OSA_STATUS status;
    
    unsigned char* arr = "1234567890";
    AM_AES_CIPHER_T st_aes_cipher_en = {0};
    AM_AES_CIPHER_T st_aes_cipher_de = {0};
    unsigned char* data = NULL;
    unsigned char* exdata = NULL;;
    unsigned char* key = "5zgb'fA.O7.@0Z}p";
    unsigned char* iv = "38k6([ZAyd@81ueO";
    unsigned char out[100] = {0};
    int i = 0;
    
    while(1)
    {
        st_aes_cipher_en.data = arr;
        st_aes_cipher_en.key = key;
        st_aes_cipher_en.iv = iv;
        st_aes_cipher_en.exdata = malloc(100);
        memset(st_aes_cipher_en.exdata, 0, 100);
        st_aes_cipher_en.data_len = 10;//这里是原始加密数据的长度
        st_aes_cipher_en.type = MBEDTLS_CIPHER_AES_128_CBC;
        st_aes_cipher_en.padding = MBEDTLS_PADDING_PKCS7;
        
        am_aes_cipher_encrypt(&st_aes_cipher_en);

        memset(out, 0, 100);
        memcpy(out, st_aes_cipher_en.exdata, 16);
#if 1//加密后的数据转为字符串打印
        char* temp = NULL;
        UINT16 encrypt_len = 0;
        char arr_temp[33] = {0};
        UINT32 p_len = 0;
        
        encrypt_len = (10 / 16 + 1) * 16;//10字节的数据，运算后会是16字节
        temp = malloc(100);
        p_len = encrypt_len / 16;//计算出加密后数据一共有几个16字节
        if(NULL != temp)
        {
            memset(temp, 0, encrypt_len * 2 + 1);
            utils_hex2ascii_str(temp, (UINT8*)out, encrypt_len);//转换为字符串输出
            memset(arr_temp, 0, 17);
            for(i = 0; i < p_len;i++)
            {
                memcpy(arr_temp, temp + 32 * i, 32);//每次输出16字节数据的字符串形式32字节数据
                sample_aes_uart_printf("%s", arr_temp);
            }
            free(temp);
        }
#endif
        st_aes_cipher_de.data = st_aes_cipher_en.exdata;
        st_aes_cipher_de.key = key;
        st_aes_cipher_de.iv = iv;
        st_aes_cipher_de.exdata = malloc(100);
        memset(st_aes_cipher_de.exdata, 0, 100);
        st_aes_cipher_de.data_len = 16;//10字节数据，加密后会变为16字节
        st_aes_cipher_de.type = MBEDTLS_CIPHER_AES_128_CBC;
        st_aes_cipher_de.padding = MBEDTLS_PADDING_PKCS7;
        
        am_aes_cipher_decrypt(&st_aes_cipher_de);

        memset(out, 0, 100);
        memcpy(out, st_aes_cipher_de.exdata, 10);//解密后， 长度是原始数据10字节
        sample_aes_uart_printf("decrypt data:%s", out);

        if(NULL != st_aes_cipher_en.exdata)
        {
            free(st_aes_cipher_en.exdata);
            st_aes_cipher_en.exdata = NULL;
        }
        if(NULL != st_aes_cipher_de.exdata)
        {
            free(st_aes_cipher_de.exdata);
            st_aes_cipher_de.exdata = NULL;
        }

        dtu_sleep(5);
    }
}

// End of file : main.h 2023-2-23 14:31:52 by: zhaoning 

