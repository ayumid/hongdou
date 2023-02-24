//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_ali_mqtt.c
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

// Includes ---------------------------------------------------------------------

#include "lib_ali_mqtt.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static int _mqtt_dynreg_sign_clientid(char *client_id, char *rand,iotx_device_info_t* meta_info)
{
  	const char *clientid1 = "|securemode=2,authType=register,random=";
    const char *clientid2 = ",signmethod=hmacsha1|";

    uint32_t clientid_len = 0;

    clientid_len = strlen(meta_info->product_key) + 1 + strlen(meta_info->device_name) +
                   strlen(clientid1) + strlen(rand) + strlen(clientid2) + 1;
    memset(client_id, 0, clientid_len);
    memcpy(client_id, meta_info->product_key, strlen(meta_info->product_key));
    memcpy(client_id + strlen(client_id), ".", strlen("."));
    memcpy(client_id + strlen(client_id), meta_info->device_name, strlen(meta_info->device_name));
    memcpy(client_id + strlen(client_id), clientid1, strlen(clientid1));
    memcpy(client_id + strlen(client_id), rand, strlen(rand));
    memcpy(client_id + strlen(client_id), clientid2, strlen(clientid2));
    printf("ending %s client_id:%s ...", __FUNCTION__,client_id);
    return 0;
}
static int _mqtt_dynreg_sign_password(char* product_secret,char *password, char *rand,iotx_device_info_t* meta_info)
{
	printf("enter %s ...", __FUNCTION__);

    char signsource[200] = {0};
    uint16_t signsource_len = 0;
    const char sign_fmt[] = "deviceName%sproductKey%srandom%s";
    uint8_t sign_hex[32] = {0};


    signsource_len = strlen(sign_fmt) + strlen(meta_info->device_name) + 1 + strlen(meta_info->product_key) + strlen(
                                 rand) + 1;
    if (signsource_len >= 200) {
        return -1;
    }
    memset(signsource, 0, signsource_len);
    memcpy(signsource, "deviceName", strlen("deviceName"));
    memcpy(signsource + strlen(signsource), meta_info->device_name, strlen(meta_info->device_name));
    memcpy(signsource + strlen(signsource), "productKey", strlen("productKey"));
    memcpy(signsource + strlen(signsource), meta_info->product_key, strlen(meta_info->product_key));
    memcpy(signsource + strlen(signsource), "random", strlen("random"));
    memcpy(signsource + strlen(signsource), rand, strlen(rand));
    utils_hmac_sha1(signsource, strlen(signsource),password,product_secret,
                      strlen(product_secret));
	
    printf("ending %s password:%s ...", __FUNCTION__,password);
    return 0;
}

static int util_ali_calc_hmac_signature( char *hmac_sigbuf,
            const int hmac_buflen,
            const char *timestamp_str,iotx_device_info_t*        dev)
{
    char                    signature[GUIDER_SIGN_LEN];
    char                    hmac_source[512];
    int                     rc = -1;
	printf("enter :%s",__FUNCTION__);
    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    rc = snprintf(hmac_source,
                  sizeof(hmac_source),
                  "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                  dev->device_id,
                  dev->device_name,
                  dev->product_key,
                  timestamp_str);
    ASSERT(rc < sizeof(hmac_source));
	
	printf("hmac_source:%s",hmac_source);
	printf("device_secret:%s",dev->device_secret);
    utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    dev->device_secret,
                    strlen(dev->device_secret));
    memcpy(hmac_sigbuf, signature, hmac_buflen);
	printf("hmac_sigbuf:%s",hmac_sigbuf);
    return 0;
}


static void ali_guider_get_timestamp_str(char *buf, int len)
{
	snprintf(buf, len, "%s", GUIDER_DEFAULT_TS_STR);
	return;
}

static SECURE_MODE ali_guider_get_secure_mode(void)
{
    SECURE_MODE     rc = MODE_TLS_GUIDER;

#ifdef MQTT_DIRECT

#ifdef SUPPORT_ITLS
    rc = MODE_ITLS_DNS_ID2;
#else
#ifdef SUPPORT_TLS
    rc = MODE_TLS_DIRECT;
#else
    rc = MODE_TCP_DIRECT_PLAIN;
#endif
#endif /* SUPPORT_ITLS */

#else

#ifdef SUPPORT_TLS
    rc = MODE_TLS_GUIDER;
#else
    rc = MODE_TCP_GUIDER_PLAIN;
#endif  /* SUPPORT_TLS */

#endif  /* MQTT_DIRECT */
	rc = MODE_TLS_DIRECT;
    return  rc;
}

static int HAL_GetPartnerID(char *pid_str)
{
    memset(pid_str, 0x0, PID_STRLEN_MAX);
    strcpy(pid_str, "example.demo.partner-id");
    return strlen(pid_str);
}

static int HAL_GetModuleID(char *mid_str)
{
    memset(mid_str, 0x0, MID_STRLEN_MAX);
    strcpy(mid_str, "example.demo.module-id");
    return strlen(mid_str);
}

static void _ali_ident_partner(char *buf, int len)
{
    char                tmp[PID_STRLEN_MAX] = {0};

    memset(tmp, 0, sizeof(tmp));
    HAL_GetPartnerID(tmp);
    if (strlen(tmp)) {
        snprintf(buf, len, ",pid=%s", tmp);
    } else {
        strcpy(buf, "");
    }

    return;
}

static void _ali_ident_module(char *buf, int len)
{
    char                tmp[MID_STRLEN_MAX] = {0};

    memset(tmp, 0, sizeof(tmp));
    HAL_GetModuleID(tmp);
    if (strlen(tmp)) {
        snprintf(buf, len, ",mid=%s", tmp);
    } else {
        strcpy(buf, "");
    }

    return;
}
 static int iotx_ali_guider_authenticate(mqtt_client_t *conn,iotx_device_info_t*		dev,char* product_secret,unsigned char regFlag)
{
	char				guider_sign[GUIDER_SIGN_LEN] = {0};
	char				tmp_clientID[CLIENT_ID_LEN] = {0};
	char				timestamp_str[GUIDER_TS_LEN] = {0};
	SECURE_MODE         secure_mode = MODE_TLS_GUIDER;
	char                partner_id[PID_STRLEN_MAX + 16] = {0};
    char                module_id[MID_STRLEN_MAX + 16] = {0};
	
	char			   *req_str = NULL;
	int 				gw = 0;
	int 				ext = 0;
	int len;
	int rc;
	printf("get device info:%s,%s,%s",dev->product_key,
              dev->device_name,
              dev->device_secret);
	_ali_ident_partner(partner_id, sizeof(partner_id));
    _ali_ident_module(module_id, sizeof(module_id));
	
	secure_mode = ali_guider_get_secure_mode();
	ali_guider_get_timestamp_str(timestamp_str, sizeof(timestamp_str));

	/* calculate the sign */
	if(regFlag == 1 &&product_secret )
	{
		_mqtt_dynreg_sign_password(product_secret,guider_sign,"3",dev);
	}else
	{
		util_ali_calc_hmac_signature(guider_sign, sizeof(guider_sign), timestamp_str,dev);
	}

	len = strlen(dev->device_name) + strlen(dev->product_key) + 2;

	conn->mqtt_user_name = (char *)malloc(len);
	if (conn->mqtt_user_name == NULL) {
		rc = -1;
		goto failed;
	}

	/* fill up username and password */
	snprintf(conn->mqtt_user_name, len,
					  "%s&%s",
					  dev->device_name,
					  dev->product_key);

	len = strlen(guider_sign) + 1;
	conn->mqtt_password = (char *)malloc(len);
	if (conn->mqtt_password == NULL) {
		rc = -1;
		goto failed;
	}
	snprintf(conn->mqtt_password, len, "%s", guider_sign);
	len = CLIENT_ID_LEN;
	conn->mqtt_client_id = (char *)malloc(len);
	if (conn->mqtt_client_id == NULL) {
		rc = -1;
		goto failed;
	}
	if(regFlag == 1)
	{
		_mqtt_dynreg_sign_clientid(tmp_clientID,"3",dev);
		snprintf(conn->mqtt_client_id, len,"%s",tmp_clientID);
	}else{
		snprintf(conn->mqtt_client_id, len,
						  "%s"
						  "|securemode=%d" ",_v=sdk-c-"LINKKIT_VERSION
						  ",timestamp=%s" ",signmethod="SHA_METHOD ",lan=C"
						  ",gw=%d"
						  ",ext=%d"
						  "%s"
						  "%s"
						  "|"
						  , dev->device_id
						  , secure_mode
						  , timestamp_str
						  , gw
						  , ext
						  , partner_id
						  , module_id
						 );
	}
	if (req_str) {
		free(req_str);
	}
	return 0;
	
failed:
	return rc;

}
// Functions --------------------------------------------------------------------
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
int lib_ali_mqtt_authenticate(mqtt_client_t *c,char*		product_key,char* device_name,char*	device_secret,char* product_secret,unsigned char reg_flag)
{
	iotx_device_info_t mqtt_info;
	
	sprintf(mqtt_info.product_key,"%s",product_key);
	sprintf(mqtt_info.device_name,"%s",device_name);
	sprintf(mqtt_info.device_secret,"%s",device_secret);
	snprintf(mqtt_info.device_id,sizeof(mqtt_info.device_id), "%s.%s",mqtt_info.product_key,mqtt_info.device_name);
	
	return iotx_ali_guider_authenticate(c,&mqtt_info,product_secret,reg_flag);
	
}

// End of file : lib_ali_mqtt.h 2021-12-2 15:21:35 by: win 

