//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_os_mqtt.c
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

// Includes ---------------------------------------------------------------------

#include "lib_os_mqtt.h"

// Private defines / typedefs ---------------------------------------------------

/*******************************************************************************
* 函数名 : AsciiToHex
* 描述   : ASCALL码转换成字符
* 输入   : unsigned char cNum  ASC-II字符码
* 输出   : unsigned char HEX码
* 返回   :
* 注意   : 如：{'A'} --> 0xA
*******************************************************************************/
unsigned char AsciiToHex(unsigned char cNum)
{
	if(cNum>='0'&&cNum<='9')
	{
		cNum -= '0';
	}
	else if(cNum>='A'&&cNum<='F')
	{
		cNum -= 'A';
		cNum += 10;
	}
	else if(cNum>='a'&&cNum<='f')
	{
		cNum -= 'a';
		cNum += 10;
	}
	return cNum;
}

/*******************************************************************************
* 函数名 : StrToHex
* 描述   : 字符串转换hex
* 输入   : char *ptr字符存储区，unsigned short len数据长度
* 输出   :
* 返回   : 
* 注意   : 如："C8329BFD0E01" -->  {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
*******************************************************************************/
void StrToHex(char *ptr, unsigned short len)
{
	unsigned char n1  = 0;
	unsigned char n2  = 0;
	unsigned char num = 0;
	unsigned short i = 0;
	unsigned short index = 0;

	for(i=0; i<len; i++)
	{
		index = i << 1;				//index=i*2
		n1 = AsciiToHex(ptr[index]);
		n2 = AsciiToHex(ptr[index+1]);
		num = (n1<<4) + n2;
		ptr[i] = num;
	}
}

// Functions --------------------------------------------------------------------
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
int lib_get_os_mqtt_token(char *token, unsigned long exp_time, const char *product_id, const char *dev_name, const char *access_key)
{
    char base64_data[64] = { 0 };
    char str_for_sig[64] = { 0 };
    char sign_buf[128] = { 0 };
    unsigned int base64_data_len = sizeof(base64_data);
	unsigned int sign_len = 20;
    char i = 0;
    char *tmp = NULL;
	unsigned int olen=0;


	sprintf(token, "version=%s", DEV_TOKEN_VERISON_STR);

    sprintf(token + strlen(token), "&res=products%%2F%s%%2Fdevices%%2F%s", product_id, dev_name);

    sprintf(token + strlen(token), "&et=%u", exp_time);
	
	sprintf(token + strlen(token), "&method=%s", "sha1");

    mbedtls_base64_decode(base64_data, base64_data_len, &olen,access_key,strlen(access_key));
	
    sprintf(str_for_sig, "%u\n%s\nproducts/%s/devices/%s\n%s", exp_time, "sha1", product_id, dev_name, DEV_TOKEN_VERISON_STR);

	utils_hmac_sha1(str_for_sig,strlen(str_for_sig),sign_buf,base64_data,base64_data_len);
	//dmh get byte for hmac_sha1 result
	StrToHex(sign_buf, strlen(sign_buf));
	
    memset(base64_data, 0, sizeof(base64_data));
    base64_data_len = sizeof(base64_data);

	mbedtls_base64_encode(base64_data, base64_data_len, &olen,sign_buf,sign_len);

    strcat(token, "&sign=");
    tmp = token + strlen(token);

    for(i = 0; i < base64_data_len; i++)
    {
        switch(base64_data[i])
        {
        case '+':
            strcat(tmp, "%2B");
            tmp += 3;
            break;
        case ' ':
            strcat(tmp, "%20");
            tmp += 3;
            break;
        case '/':
            strcat(tmp, "%2F");
            tmp += 3;
            break;
        case '?':
            strcat(tmp, "%3F");
            tmp += 3;
            break;
        case '%':
            strcat(tmp, "%25");
            tmp += 3;
            break;
        case '#':
            strcat(tmp, "%23");
            tmp += 3;
            break;
        case '&':
            strcat(tmp, "%26");
            tmp += 3;
            break;
        case '=':
            strcat(tmp, "%3D");
            tmp += 3;
            break;
        default:
            *tmp = base64_data[i];
            tmp += 1;
            break;
        }
    }

    return 0;
}

// End of file : lib_os_mqtt.h 2021-12-9 11:30:25 by: win 

