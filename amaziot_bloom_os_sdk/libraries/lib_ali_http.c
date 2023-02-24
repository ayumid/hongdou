//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_ali_http.c
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

// Includes ---------------------------------------------------------------------

#include "lib_ali_http.h"
#include "libhttpclient.h"
#include "cJSON.h"


// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------
struct http_data_s {
    unsigned data_sz;
    UINT8 data[10*1024];
};

static char jsonStr[5*1024]={0};

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static int _http_sign_password(char* product_secret,char *password,char* device_name, char*		product_key,char *rand)
{
	printf("enter %s ...", __FUNCTION__);

    char signsource[200] = {0};
    uint16_t signsource_len = 0;
    const char sign_fmt[] = "deviceName%sproductKey%srandom%s";
    
    signsource_len = strlen(sign_fmt) + strlen(device_name) + 1 + strlen(product_key) + strlen(
                                 rand) + 1;
    if (signsource_len >= 200) {
        return -1;
    }
    memset(signsource, 0, signsource_len);
    memcpy(signsource, "deviceName", strlen("deviceName"));
    memcpy(signsource + strlen(signsource), device_name, strlen(device_name));
    memcpy(signsource + strlen(signsource), "productKey", strlen("productKey"));
    memcpy(signsource + strlen(signsource), product_key, strlen(product_key));
    memcpy(signsource + strlen(signsource), "random", strlen("random"));
    memcpy(signsource + strlen(signsource), rand, strlen(rand));
    utils_hmac_sha1(signsource, strlen(signsource),password,product_secret,
                      strlen(product_secret));
	
    printf("ending %s password:%s ...", __FUNCTION__,password);
    return 0;
}

static void package_json_post_data(char*		product_key,char* device_name,char* product_secret)
{
	char random[10]="1234";
	char password[64]={0};

	_http_sign_password(product_secret,password,device_name,product_key,random);
	
	sprintf(jsonStr,"productKey=%s&deviceName=%s&random=%s&sign=%s&signMethod=HmacSHA1",product_key,device_name,random,password);
    
} 
static int parse_json_post_data(char *str,char*	device_secret)
{
	int ret = -1;
	cJSON * hiDevice = cJSON_Parse(str);  
	if(hiDevice!=NULL){
		cJSON * pItem;

		pItem = cJSON_GetObjectItem(hiDevice, "data");
		if(pItem != NULL)
		{
			printf("pItem->valuestring:%s",cJSON_Print(pItem));
			pItem = cJSON_GetObjectItem(pItem, "deviceSecret");
			if(pItem != NULL){
				sprintf(device_secret,pItem->valuestring);
				ret = 0;
			}else{
				printf("get deviceSecret error!!!!");
			}
		}
	}else{
		printf("parse_json_post_data error!!!!");
	}
	cJSON_Delete(hiDevice);

	return ret;
} 


static int _response_cb(char *buffer, int size, int nitems, void *private_data)
{
    struct http_data_s *client_data = private_data;
    if ((client_data->data_sz + size) < sizeof(client_data->data)) {
        memcpy(client_data->data + client_data->data_sz, buffer, size);
        client_data->data_sz += size;
        return 0;
    }
    return -1;
}
// Functions --------------------------------------------------------------------
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
int lib_ali_http_authenticate(char*		product_key,char* device_name,char*	device_secret,char* product_secret)
{
	int ret;
	struct http_client *client = NULL;
    struct http_data_s *client_data = NULL;
	struct http_client_list * client_hdr = NULL;//dmh
	int response_code = 0;
	
	PRO_START:
		client_data = malloc(sizeof(*client_data));
		if (!client_data)
			goto PRO_ERROR;
		memset(client_data, 0, sizeof(*client_data));
	
		response_code = 0;
		client = http_client_init();
		if (!client)
			goto PRO_ERROR;
	
		package_json_post_data(product_key,device_name,product_secret);
		
        http_client_setopt(client, HTTPCLIENT_OPT_URL, "https://iot-auth.cn-shanghai.aliyuncs.com/auth/register/device");
        http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, _response_cb);
        http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
        http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_POST);
        http_client_setopt(client, HTTPCLIENT_OPT_POSTDATA, jsonStr);
	    http_client_setopt(client, HTTPCLIENT_OPT_POSTLENGTH, strlen(jsonStr));


		//添加 HTTP header
		client_hdr = http_client_list_append(client_hdr, "Content-Type: application/x-www-form-urlencoded\r\n");	//内容为表单数据
		client_hdr = http_client_list_append(client_hdr, "Accept: text/html, */* \r\n");	//可接收内容类型
		client_hdr = http_client_list_append(client_hdr, "Connection: Keep-Alive\r\n");	//keep alive
		http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, client_hdr);	//设置header

		http_client_perform(client);
        http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
        sdk_uart_printf("[http_client_test]Get tcp state %d\n", response_code);
	
		if (response_code >= 200 && response_code < 300)
		 {
			 if(client_data->data_sz)
			 {
				printf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);
				ret = parse_json_post_data(client_data->data,device_secret);
				if(ret == 0)
					goto PRO_END;
			 }
		}else if (response_code == 404) {
			printf("response_code == %d\r\n%s",response_code ,client_data->data);
		}
			
	PRO_ERROR:
		if (client)
			http_client_shutdown(client);
		if (client_data)
			free(client_data);
		return -1;
		  
	PRO_END:
		if (client)
			http_client_shutdown(client);
		if (client_data)
			free(client_data);
		return 0;
}

// End of file : lib_ali_http.h 2022-3-7 11:00:23 by: win 

