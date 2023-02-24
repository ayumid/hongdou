//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_sslsocket.c
// Auther      : win
// Version     :
// Date : 2021-12-8
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-8
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "lib_sslsocket.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void debug(void *userdata, int level,const char *filename, int line,const char *msg)
{
	//ssl_debug("%s", msg);
}
static int ssl_client_recv(void * ctx, unsigned char * buf, unsigned int len)
{
	struct ssl_client * client = (struct ssl_client *)ctx;
	return lib_recv(client->fd, buf, len, 0);
}

static int ssl_client_send(void * ctx, const unsigned char *buf, unsigned int len)
{
    struct ssl_client * client = (struct ssl_client *)ctx;
	return lib_send(client->fd, buf, len, 0);
}

// Functions --------------------------------------------------------------------
/**
  * Function    : lib_ssl_client_init
  * Description : SSL客户端初始化
  * Input       : 套接字句柄
  *               
  * Output      : 
  * Return      : SSL客户端实例
  * Auther      : win
  * Others      : 
  **/
struct ssl_client * lib_ssl_client_init(int fd)
{
	char *pers = "ssl_client";
	int ret;
	struct ssl_client * client = (struct ssl_client *)malloc(sizeof(struct ssl_client));
	if (!client) {
		printf("Cannot malloc memory(ssl_client_init)\n");
		return NULL;
	}
	memset(client, 0, sizeof(struct ssl_client));

#ifdef CA_CERT
	mbedtls_x509_crt *ca_chain; 
	ca_chain = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
	memset(ca_chain, 0, sizeof(mbedtls_x509_crt));
#endif

	mbedtls_entropy_init(&client->entropy);
	mbedtls_ctr_drbg_init(&client->ctr_drbg);
	if((ret = mbedtls_ctr_drbg_seed(&client->ctr_drbg, mbedtls_entropy_func, 
                                    &client->entropy,
                                    (unsigned char *)pers, strlen(pers))) != 0)
	{
		printf( " failed\n  ! ctr_drbg_init returned %d\n", ret );
		goto failed;
	}

	mbedtls_ssl_init(&(client->ssl));
	mbedtls_ssl_config_init(&(client->config));
	client->fd = fd;
	mbedtls_ssl_conf_rng(&(client->config), mbedtls_ctr_drbg_random, &client->ctr_drbg);

#ifdef CA_CERT
    mbedtls_x509_crt_parse(ca_chain,ca1_cert,strlen(ca1_cert));
	mbedtls_ssl_conf_ca_chain(&(client->config), ca_chain, NULL);
    mbedtls_ssl_set_hostname(&(client->ssl), HOSTNAME);
#endif

#ifdef OWN_CERT
    mbedtls_x509_crt *client_chain = NULL;
    mbedtls_pk_context *client_rsa = NULL;

    client_chain = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
    memset(client_chain, 0, sizeof(mbedtls_x509_crt));
    mbedtls_x509_crt_parse(client_chain,client_cert,strlen(client_cert));
    
    client_rsa = (mbedtls_pk_context *)malloc(sizeof(mbedtls_pk_context));
    memset(client_rsa, 0, sizeof(mbedtls_rsa_context));  
    mbedtls_pk_parse_key(client_rsa, client_key, strlen(client_key), NULL,0);
    mbedtls_ssl_conf_own_cert(&client->config, client_chain, client_rsa);
#endif

	mbedtls_ssl_config_defaults(&client->config, MBEDTLS_SSL_IS_CLIENT,
								MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

#ifdef CA_CERT
	mbedtls_ssl_conf_authmode(&client->config, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
	mbedtls_ssl_conf_authmode(&client->config, MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif

	mbedtls_ssl_conf_rng(&client->config, mbedtls_ctr_drbg_random, &client->ctr_drbg);
	mbedtls_ssl_conf_dbg(&client->config, debug, NULL);
	mbedtls_ssl_set_bio(&client->ssl, client,
                        ssl_client_send, ssl_client_recv, NULL);

	mbedtls_ssl_setup(&client->ssl, &client->config);
	return client;
failed:
#ifdef CA_CERT
	if(ca_chain) free(ca_chain);
#endif
	if (client) free(client);
	return NULL;
}
/**
  * Function    : lib_ssl_client_shutdown
  * Description : 注销SSL客户端实例
  * Input       : SSL客户端实例
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_ssl_client_shutdown(struct ssl_client * client)
{

    mbedtls_ssl_config *config=NULL;

	if (!client) return;
#ifdef CA_CERT
    config = (mbedtls_ssl_config *)&(client->config);
	if(config->ca_chain){
        free(config->ca_chain);
	}
#endif

#ifdef OWN_CERT
    mbedtls_ssl_key_cert * cert=NULL;
    config = (mbedtls_ssl_config *)&(client->config);
    cert = config->key_cert;
    
	if(cert&&cert->cert){
        free(cert->cert);
        cert->cert=NULL;
	}
    if(cert&&cert->key){
        free(cert->key);
        cert->key=NULL;
	}
#endif

	printf("ssl close notify");
	mbedtls_ssl_close_notify(&client->ssl);
	printf("ssl ctr drbg free");
	mbedtls_ctr_drbg_free(&client->ctr_drbg);
	printf("ssl entropy free");
	mbedtls_entropy_free(&client->entropy);
	printf("ssl free");
	mbedtls_ssl_free(&client->ssl);
	printf("ssl config free");
	mbedtls_ssl_config_free(&client->config);
	printf("client free");
	free(client);
	client = NULL;
	printf("shutdown end");
}
/**
  * Function    : lib_ssl_client_write
  * Description : SSL发送数据
  * Input       : ssl SSL实例例上下文
  *               buf 发送数据
  *               len 发送数据长度
  * Output      : 
  * Return      : 成功 0 失败 Other
  * Auther      : win
  * Others      : 
  **/
int lib_ssl_client_write(mbedtls_ssl_context *ssl, const unsigned char *buf, int len )
{
    return ssl_write(ssl, buf, len );
}
/**
  * Function    : lib_ssl_client_read
  * Description : SSL读取数据
  * Input       : ssl SSL实例例上下文
  *               buf 读取数据
  *               len 读取数据最大长度
  * Output      : 
  * Return      : 成功 0 失败 Other
  * Auther      : win
  * Others      : 
  **/
int lib_ssl_client_read( mbedtls_ssl_context *ssl, unsigned char *buf, int len )
{
    return ssl_read( ssl, buf, len );
}

// End of file : lib_sslsocket.h 2021-12-8 10:18:29 by: win 

