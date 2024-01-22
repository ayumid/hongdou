
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "UART.h"
#include "osa.h"
#include "sockets.h"
#include "netdb.h"
#include "websocket.h"

#define WEBSOCKET_LOGIN_CONNECT_TIMEOUT 3000
#define WEBSOCKET_LOGIN_RESPOND_TIMEOUT (1000 + WEBSOCKET_LOGIN_CONNECT_TIMEOUT)
#define WEBSOCKET_SHAKE_KEY_LEN 16

#define MQTT_DEFAULT_THREAD_STACKSIZE        2048
#define MQTT_DEFAULT_THREAD_PRIO             90
#define printf(fmt, args...)    do { CPUartLogPrintf("websocket: "fmt, ##args); } while(0)

#define web_printf(fmt, args...) fatal_printf("[websocket]: "fmt, ##args)
#define WEBSOCKET_DEBUG

static struct ssl_client * web_ssl_c = NULL;
int web_use_ssl = 0;

#ifdef CA_CERT
const unsigned char ca1_cert[] = {
"-----BEGIN CERTIFICATE-----\r\n"
"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\r\n"
"-----END CERTIFICATE-----\r\n"
};
#endif

#ifdef OWN_CERT
const unsigned char client_cert[] = {
};

const unsigned char client_key[] = {
};

#endif
static unsigned int havege_rand(void *unused, unsigned char * unusedstr, size_t unusedt)
{
	char * p = malloc(16);
	unsigned int rand = (unsigned int)p;
	free(p);
	if ((rand & 0xff) == 0) {
		rand |= 0x96;
	}
	return rand;
}

static void debug(void *userdata, int level,
                     const char *filename, int line,
                     const char *msg)
{
	//ssl_debug("%s", msg);
}

static int ssl_client_recv(void * ctx, unsigned char * buf, unsigned int len)
{
	struct ssl_client * client = (struct ssl_client *)ctx;
	return recv(client->fd, buf, len, 0);
}

static int ssl_client_send(void * ctx, const unsigned char *buf, unsigned int len)
{
    struct ssl_client * client = (struct ssl_client *)ctx;
	return send(client->fd, buf, len, 0);
}

#ifdef CA_CERT
#define HOSTNAME "asr"
#endif

static struct ssl_client * ssl_client_init(int fd)
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
		printf( " failed\n	! ctr_drbg_init returned %d\n", ret );
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

	//mbedtls_ssl_conf_endpoint(&(client->config), MBEDTLS_SSL_IS_CLIENT);
	mbedtls_ssl_config_defaults(&client->config, MBEDTLS_SSL_IS_CLIENT,
								MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

#ifdef CA_CERT
	mbedtls_ssl_conf_authmode(&client->config, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
	mbedtls_ssl_conf_authmode(&client->config, MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif

	mbedtls_ssl_conf_rng(&client->config, mbedtls_ctr_drbg_random, &client->ctr_drbg);
	mbedtls_ssl_conf_dbg(&client->config, debug, NULL);
	//mbedtls_debug_set_threshold(1000);
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


static void ssl_client_shutdown(struct ssl_client * client)
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


void * web_ThreadStart(const char *name, void (*thread)(void*),
                                void *arg, int stacksize, int prio)
{
    OSTaskRef TaskRef = NULL;
    void* ptr_task_stack = NULL;

    ptr_task_stack = (void*)malloc(stacksize);
    ASSERT(ptr_task_stack != NULL);
    ASSERT(OS_SUCCESS == OSATaskCreate( &TaskRef,
                ptr_task_stack,
                stacksize,
                prio,
                (CHAR*)name,
                thread,
                arg));
    return TaskRef;
}
void *web_Calloc(size_t nums, size_t element_size)
{

    int i, all_size;
    all_size = nums * element_size;
    char *p = NULL;
    p = malloc(all_size);
    if (p == NULL)
        return NULL;
    for (i = 0; i < all_size; ++i)
        p[i] = 0;
    return p;
}
void webSocket_delayms(unsigned int ms)
{
	struct timeval tim;
	tim.tv_sec = ms / 1000;
	tim.tv_usec = (ms % 1000) * 1000;
	select(0,NULL,NULL,NULL,&tim);
}

typedef struct
{
	char ip[256];
	bool result;
	bool actionEnd;
}GetHostName_Struct;

void websocket_getHost_fun(void *arge)
{
    int ret;
    //int i;
    char buf[1024];
    struct hostent host_body, *host = NULL;
    struct in_addr **addr_list;
    GetHostName_Struct *gs = (GetHostName_Struct *)arge;

    /*  
    if((host = gethostbyname(gs->ip)) == NULL)
    //if((host = gethostbyname2(gs->ip, AF_INET)) == NULL)
    {
        gs->actionEnd = true;
        return NULL;
    }*/
    if (gethostbyname_r(gs->ip, &host_body, buf, sizeof(buf), &host, &ret))
    {
        gs->actionEnd = true;
        return ;
    }
    if (host == NULL)
    {
        gs->actionEnd = true;
        return ;
    }
    addr_list = (struct in_addr **)host->h_addr_list;
    //printf("ip name : %s\r\nip list : ", host->h_name);
    //for(i = 0; addr_list[i] != NULL; i++) printf("%s, ", inet_ntoa(*addr_list[i])); printf("\r\n");
    if (addr_list[0] == NULL)
    {
        gs->actionEnd = true;
        return ;
    }
    memset(gs->ip, 0, sizeof(gs->ip));
    strcpy(gs->ip, (char *)(inet_ntoa(*addr_list[0])));
    gs->result = true;
    gs->actionEnd = true;
    return ;
}


int webSocket_getIpByHostName(char * hostName, char * backIp)
{
	int i, timeOut = 1;
	GetHostName_Struct gs;
	if(hostName == NULL)
		return -1;	
	else if(strlen(hostName) < 1)
		return -1;
	memset(&gs,0,sizeof(GetHostName_Struct));
	strcpy(gs.ip,hostName);
	gs.result = false;
	gs.actionEnd = false;
	OSTaskRef TaskRef = NULL;
	// client->thread = ThreadStart("MQTT_Client", &MQTTRun, client, MQTT_DEFAULT_THREAD_STACKSIZE, MQTT_DEFAULT_THREAD_PRIO);
	if(web_ThreadStart("webSocket_client",websocket_getHost_fun, &gs,MQTT_DEFAULT_THREAD_STACKSIZE,MQTT_DEFAULT_THREAD_PRIO) == NULL)
		return -1;
	
	i = 0;
	while(!gs.actionEnd)
	{
		if(++i > 10)
		{
			i = 0;
			if(++timeOut > 1000)
				break;
		}
		webSocket_delayms(1000);
	}
	if(!gs.result)
		return -timeOut;
	memset(backIp,0,strlen(backIp));
	strcpy(backIp,gs.ip);
	printf("backIP:%s\n",backIp);
	return timeOut;
}

const char websocket_base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int websocket_base64_encode(const unsigned char *bindata,char* base64,int binlength)
{
	printf("enter %s ...\n", __FUNCTION__);
	int i,j;
	unsigned char current;
	for(i = 0,j = 0; i< binlength;i += 3)
	{
		current = (bindata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = websocket_base64char[(int)current];
		current  = ((unsigned char)(bindata[i] <<4)) & ((unsigned char)0x30);
		if(i + 1 >= binlength)
		{
			base64[j++] = websocket_base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = websocket_base64char[(int)current];
		current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
		if(i + 2 >= binlength)
		{
			base64[j++] = websocket_base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = websocket_base64char[(int)current];
		current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = websocket_base64char[(int)current];
	}
	base64[j] = '\0';
	printf("end %s ...\n", __FUNCTION__);	
	return j;

}

int websocket_base64_decode(const char *base64, unsigned char *bindata)
{
	printf("enter %s ...\n", __FUNCTION__);
    int i, j;
    unsigned char k;
    unsigned char temp[4];
	
    for (i = 0, j = 0; base64[i] != '\0'; i += 4)
    {
        memset(temp, 0xFF, sizeof(temp));
        for (k = 0; k < 64; k++)
        {
            if (websocket_base64char[k] == base64[i])
                temp[0] = k;
        }
        for (k = 0; k < 64; k++)
        {
            if (websocket_base64char[k] == base64[i + 1])
                temp[1] = k;
        }
        for (k = 0; k < 64; k++)
        {
            if (websocket_base64char[k] == base64[i + 2])
                temp[2] = k;
        }
        for (k = 0; k < 64; k++)
        {
            if (websocket_base64char[k] == base64[i + 3])
                temp[3] = k;
        }
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
                       ((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
        if (base64[i + 2] == '=')
            break;
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
                       ((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
        if (base64[i + 3] == '=')
            break;
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
                       ((unsigned char)(temp[3] & 0x3F));
    }

	printf("end %s ...\n", __FUNCTION__);

	return j;	
}


typedef struct SHA1Context
{
    unsigned Message_Digest[5];
    unsigned Length_Low;
    unsigned Length_High;
    unsigned char Message_Block[64];
    int Message_Block_Index;
    int Computed;
    int Corrupted;
} SHA1Context;

#define SHA1CircularShift(bits, word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32 - (bits))))

void SHA1ProcessMessageBlock(SHA1Context *context)
{
    const unsigned K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
    int t;
    unsigned temp;
    unsigned W[80];
    unsigned A, B, C, D, E;

    for (t = 0; t < 16; t++)
    {
        W[t] = ((unsigned)context->Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 3]);
    }

    for (t = 16; t < 80; t++)
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    context->Message_Digest[0] = (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] = (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] = (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] = (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] = (context->Message_Digest[4] + E) & 0xFFFFFFFF;
    context->Message_Block_Index = 0;
}

void SHA1Reset(SHA1Context *context)
{
    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;

    context->Message_Digest[0] = 0x67452301;
    context->Message_Digest[1] = 0xEFCDAB89;
    context->Message_Digest[2] = 0x98BADCFE;
    context->Message_Digest[3] = 0x10325476;
    context->Message_Digest[4] = 0xC3D2E1F0;

    context->Computed = 0;
    context->Corrupted = 0;
}

void SHA1PadMessage(SHA1Context *context)
{
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 64)
            context->Message_Block[context->Message_Block_Index++] = 0;
        SHA1ProcessMessageBlock(context);
        while (context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = 0;
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = 0;
    }
    context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
    context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
    context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
    context->Message_Block[59] = (context->Length_High) & 0xFF;
    context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
    context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
    context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
    context->Message_Block[63] = (context->Length_Low) & 0xFF;

    SHA1ProcessMessageBlock(context);

}

int SHA1Result(SHA1Context *context)
{
	printf("enter %s ...\n", __FUNCTION__);
    if (context->Corrupted)
    {
        return 0;
    }
    if (!context->Computed)
    {
        SHA1PadMessage(context);
        context->Computed = 1;
    }
	printf("end %s ...\n", __FUNCTION__);

    return 1;	
}

void SHA1Input(SHA1Context *context, const char *message_array, unsigned length)
{
	printf("enter %s ...\n", __FUNCTION__);
    if (!length)
        return;

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }

    while (length-- && !context->Corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);

        context->Length_Low += 8;

        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
                context->Corrupted = 1;
        }

        if (context->Message_Block_Index == 64)
        {
            SHA1ProcessMessageBlock(context);
        }
        message_array++;
    }
	printf("end %s ...\n", __FUNCTION__);

}


char *sha1_hash(const char *source)
{ // Main
	printf("enter %s ...\n", __FUNCTION__);

    SHA1Context sha;
    char *buf; //[128];

    SHA1Reset(&sha);
    SHA1Input(&sha, source, strlen(source));

    if (!SHA1Result(&sha))
    {
        printf("SHA1 ERROR: Could not compute message digest");
        return NULL;
    }
    else
    {
        buf = (char *)malloc(128);
        memset(buf, 0, 128);
        sprintf(buf, "%08X%08X%08X%08X%08X", sha.Message_Digest[0], sha.Message_Digest[1],
                sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
        //lr_save_string(buf, lrvar);

        //return strlen(buf);
		printf("end %s ...\n", __FUNCTION__);        
        return buf;
    }
}


int tolower(int c)
{
	//printf("enter %s ...\n", __FUNCTION__);

	if(c >= 'A' && c <= 'Z')
	{
		return c + 'a' - 'A';
	}
	else
	{
		return c;
	}
	//printf("end %s ...\n", __FUNCTION__);	
}

int htoi(const char s[],int start,int len)
{
	printf("enter %s ...\n", __FUNCTION__);

	int i,j;
	int n = 0;
	if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    {
		i = 2;
    }
	else
    {
		i = 0;
    }
	i += start;
	j = 0;
	for(;(s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F'); ++i)
	{
		if(j >= len)
        {
			break;
        }
		if(tolower(s[i]) > '9')
        {
			n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
		else
        {
			n = 16 * n + (tolower(s[i]) - '0');
        }
		j++;
	}

	printf("end %s ...\n", __FUNCTION__);

	return n;	
}

void websocket_getRandomString(unsigned char *buf,unsigned int len)
{
	printf("enter %s ...\n", __FUNCTION__);
	unsigned int i;
	unsigned char tmp;
	srand((int)time(0));
	for(i = 0;i < len;i++)
	{
		tmp = (unsigned char)(rand() % 256);
		if(tmp == 0)
			tmp = 128;
		buf[i] = tmp;
	}
	printf("end %s ...\n", __FUNCTION__);	
}


int webSocket_buildShakeKey(unsigned char *key)
{
	printf("enter %s ...\n", __FUNCTION__);
	unsigned char tmpKey[WEBSOCKET_SHAKE_KEY_LEN] = {0};
	websocket_getRandomString(tmpKey, WEBSOCKET_SHAKE_KEY_LEN);
	printf("end %s ...\n", __FUNCTION__);	
	return websocket_base64_encode((const unsigned char *) tmpKey, (char *)key, WEBSOCKET_SHAKE_KEY_LEN);
}
int webSocket_buildRespondShakeKey(unsigned char *acceptKey,unsigned int acceptKeyLen,unsigned char *respondKey)
{
	printf("enter %s ...\n", __FUNCTION__);
	char *clientKey;
	char *sha1DataTmp;
	char *sha1Data;
	int i,n;
	const char GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned int GUIDLEN;

	if(acceptKey == NULL)
		return 0;
	GUIDLEN = sizeof(GUID);
	clientKey = (char *)calloc(acceptKeyLen + GUIDLEN + 10,sizeof(char));
	memset(clientKey,0,(acceptKeyLen + GUIDLEN + 10));
	memcpy(clientKey,acceptKey,acceptKeyLen);
	memcpy(&clientKey[acceptKeyLen],GUID,GUIDLEN);
	clientKey[acceptKeyLen + GUIDLEN] = '\0';

	sha1DataTmp = sha1_hash(clientKey);
	n = strlen((const char *)sha1DataTmp);
	sha1Data = (char*)calloc(n / 2 + 1,sizeof(char));
	memset(sha1Data,0,n / 2 + 1);

	for(i = 0;i < n;i += 2)
		sha1Data[i / 2]  = htoi(sha1DataTmp,i,2);
	n = websocket_base64_encode((const unsigned char*)sha1Data,(char *)respondKey,(n / 2));

	free(sha1DataTmp);
	free(sha1Data);
	free(clientKey);
	printf("end %s ...\n", __FUNCTION__);	
	return n;
 
}
int webSocket_matchShakeKey(unsigned char *myKey,unsigned int myKeyLen,unsigned char *acceptKey,unsigned int acceptKeyLen)
{
	printf("enter %s ...\n", __FUNCTION__);
	int retLen;
	unsigned char tmpKey[256] = {0};

	retLen = webSocket_buildRespondShakeKey(myKey, myKeyLen, tmpKey);
	printf("webSocket_matchShakeKey:myKey:%s,acceptKey:%s\n",myKey,acceptKey);
	if(retLen != acceptKeyLen)
	{
		printf("webSocket_matchShakeKey : len err\r\n%s\r\n%s\r\n%s\r\n",myKey,tmpKey,acceptKey);
		return -1;
    }
    else if (strcmp((const char *)tmpKey, (const char *)acceptKey) != 0)
    {
        printf("webSocket_matchShakeKey : str err\r\n%s\r\n%s\r\n", tmpKey, acceptKey);
        return -1;
    }
    return 0;
}


void webSocket_buildHttpHead(char *ip,int port,char *interfacePath,unsigned char *shakeKey,char *package)
{
	printf("enter %s ...\n", __FUNCTION__);
#if 0
    const char httpDemo[] = "GET /  HTTP/1.1\r\n"
                            "Host:  %s:%d\r\n"
                            "Accept-Language: en-US,en;q=0.5\r\n"
                            "Accept-Encoding: gzip,deflate"
                            "Sec-WebSocket-Version:  13\r\n"
                            "Origin:  null\r\n"
                            "Sec-WebSocket-Extensions:  permessage-deflate\r\n"
                            "Sec-WebSocket-Key:  %s\r\n"
                            "Connection:  keep-alive,Upgrade\r\n"
                            "Pragma:  no-cache\r\n"
                            "Cache-Control: no-cache"
                            "Upgrade:  websocket\r\n\r\n";
    sprintf(package, httpDemo,ip, port, shakeKey);
#else	
	const char httpDemo[] = "GET /%s HTTP/1.1\r\n"
						"Host: %s\r\n" 
						"Upgrade: websocket\r\n"
						"Connection: Upgrade\r\n"
						"Sec-WebSocket-Key: %s\r\n"
						"Origin: null\r\n"
						"Sec-WebSocket-Version: 13\r\n\r\n"; 
	
	sprintf(package, httpDemo, interfacePath, ip, shakeKey);
#endif	
	printf("end %s ...\n", __FUNCTION__);
}

void webSocket_buildHttpRespond(unsigned char *acceptKey, unsigned int acceptKeyLen, char *package)
{
	printf("enter %s ...\n", __FUNCTION__);
    const char httpDemo[] = "HTTP/1.1 101 Switching Protocols\r\n"
                            "Upgrade: websocket\r\n"
                          //  "Server: Microsoft-HTTPAPI/2.0\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Accept: %s\r\n"
                            "%s\r\n\r\n"; 
    time_t now;
    struct tm *tm_now;
    char timeStr[256] = {0};
    unsigned char respondShakeKey[256] = {0};

    webSocket_buildRespondShakeKey(acceptKey, acceptKeyLen, respondShakeKey);

    time(&now);
    tm_now = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "Date: %a, %d %b %Y %T %Z", tm_now);

    sprintf(package, httpDemo, respondShakeKey, timeStr);
	printf("end %s ...\n", __FUNCTION__);
}


int webSocket_enPackage(unsigned char *data, unsigned int dataLen, unsigned char *package, unsigned int packageMaxLen, bool isMask, WebsocketData_Type type)
{
	printf("enter %s ...\n", __FUNCTION__);

    unsigned char maskKey[4] = {0}; 
    unsigned char temp1, temp2;
//	*package = 0x00;
    int count;
    unsigned int i, len = 0;
	printf("packageMaxLen:%d,type:%d,dataLen:%d,isMask:%d\n",packageMaxLen,type,dataLen,isMask);
	//147 -19 19 1
    if (packageMaxLen < 2)
        return -1;
	
    if (type == WDT_MINDATA)
        *package++ = 0x00;
    else if (type == WDT_TXTDATA)
	{
		printf("type0 = WDT_TXTDATA\n");
        *package++ = 0x81;
		printf("type1 = WDT_TXTDATA\n");
    }
    else if (type == WDT_BINDATA)
        *package++ = 0x82;
    else if (type == WDT_DISCONN)
        *package++ = 0x88;
    else if (type == WDT_PING)
        *package++ = 0x89;
    else if (type == WDT_PONG)
        *package++ = 0x8A;
    else
        return -1;
    //
    if (isMask)
		printf("ismask\n");
        *package = 0x80;
    len += 1;
    //
    if (dataLen < 126)
    {
    	printf("dataLen < 126\n");
        *package++ |= (dataLen & 0x7F);
        len += 1;
    }
    else if (dataLen < 65536)
    {
        if (packageMaxLen < 4)
            return -1;
        *package++ |= 0x7E;
        *package++ = (char)((dataLen >> 8) & 0xFF);
        *package++ = (unsigned char)((dataLen >> 0) & 0xFF);
        len += 3;
    }
    else if (dataLen < 0xFFFFFFFF)
    {
        if (packageMaxLen < 10)
            return -1;
        *package++ |= 0x7F;
        *package++ = 0;                              //(char)((dataLen >> 56) & 0xFF);   
        *package++ = 0;                              //(char)((dataLen >> 48) & 0xFF);
        *package++ = 0;                              //(char)((dataLen >> 40) & 0xFF);
        *package++ = 0;                              //(char)((dataLen >> 32) & 0xFF);
        *package++ = (char)((dataLen >> 24) & 0xFF); 
        *package++ = (char)((dataLen >> 16) & 0xFF);
        *package++ = (char)((dataLen >> 8) & 0xFF);
        *package++ = (char)((dataLen >> 0) & 0xFF);
        len += 9;
    }
    //
    if (isMask) 
    {
        if (packageMaxLen < len + dataLen + 4)
            return -1;
		printf("websocket_getRandomString\n");
        websocket_getRandomString(maskKey, sizeof(maskKey)); 
        *package++ = maskKey[0];
        *package++ = maskKey[1];
        *package++ = maskKey[2];
        *package++ = maskKey[3];
        len += 4;
        for (i = 0, count = 0; i < dataLen; i++)
        {
            temp1 = maskKey[count];
            temp2 = data[i];
            *package++ = (char)(((~temp1) & temp2) | (temp1 & (~temp2)));
            count += 1;
            if (count >= sizeof(maskKey)) 
                count = 0;
        }
        len += i;
        *package = '\0';
    }
    else 
    {
        if (packageMaxLen < len + dataLen)
            return -1;
        memcpy(package, data, dataLen);
        package[dataLen] = '\0';
        len += dataLen;
    }
    //
  //  sleep(2);
    printf("end %s ...\n", __FUNCTION__);
    return len;
}

int webSocket_dePackage(unsigned char *data, unsigned int dataLen, unsigned char *package, unsigned int packageMaxLen, unsigned int *packageLen, unsigned int *packageHeadLen)
{
	printf("enter %s ...\n", __FUNCTION__);

    unsigned char maskKey[4] = {0};
    unsigned char temp1, temp2;
    char Mask = 0, type;
    int count, ret;
    unsigned int i, len = 0, dataStart = 2;
    if (dataLen < 2)
        return WDT_ERR;

    type = data[0] & 0x0F;

    if ((data[0] & 0x80) == 0x80)
    {
        if (type == 0x01)
            ret = WDT_TXTDATA;
        else if (type == 0x02)
            ret = WDT_BINDATA;
        else if (type == 0x08)
            ret = WDT_DISCONN;
        else if (type == 0x09)
            ret = WDT_PING;
        else if (type == 0x0A)
            ret = WDT_PONG;
        else
            return WDT_ERR;
    }
    else if (type == 0x00)
        ret = WDT_MINDATA;
    else
        return WDT_ERR;
    //
    if ((data[1] & 0x80) == 0x80)
    {
        Mask = 1;
        count = 4;
    }
    else
    {
        Mask = 0;
        count = 0;
    }
    //
    len = data[1] & 0x7F;
    //
    if (len == 126)
    {
        if (dataLen < 4)
            return WDT_ERR;
        len = data[2];
        len = (len << 8) + data[3];
        if (packageLen)
            *packageLen = len; 
        if (packageHeadLen)
            *packageHeadLen = 4 + count;
        //
        if (dataLen < len + 4 + count)
            return WDT_ERR;
        if (Mask)
        {
            maskKey[0] = data[4];
            maskKey[1] = data[5];
            maskKey[2] = data[6];
            maskKey[3] = data[7];
            dataStart = 8;
        }
        else
            dataStart = 4;
    }
    else if (len == 127)
    {
        if (dataLen < 10)
            return WDT_ERR;
        if (data[2] != 0 || data[3] != 0 || data[4] != 0 || data[5] != 0) 
            return WDT_ERR;
        len = data[6];
        len = (len << 8) + data[7];
        len = (len << 8) + data[8];
        len = (len << 8) + data[9];
        if (packageLen)
            *packageLen = len; 
        if (packageHeadLen)
            *packageHeadLen = 10 + count;
        //
        if (dataLen < len + 10 + count)
            return WDT_ERR;
        if (Mask)
        {
            maskKey[0] = data[10];
            maskKey[1] = data[11];
            maskKey[2] = data[12];
            maskKey[3] = data[13];
            dataStart = 14;
        }
        else
            dataStart = 10;
    }
    else
    {
        if (packageLen)
            *packageLen = len; 
        if (packageHeadLen)
            *packageHeadLen = 2 + count;
        //
        if (dataLen < len + 2 + count)
            return WDT_ERR;
        if (Mask)
        {
            maskKey[0] = data[2];
            maskKey[1] = data[3];
            maskKey[2] = data[4];
            maskKey[3] = data[5];
            dataStart = 6;
        }
        else
            dataStart = 2;
    }
    //
    if (dataLen < len + dataStart)
        return WDT_ERR;
    //
    if (packageMaxLen < len + 1)
        return WDT_ERR;
    //
    if (Mask) 
    {
        for (i = 0, count = 0; i < len; i++)
        {
            temp1 = maskKey[count];
            temp2 = data[i + dataStart];
            *package++ = (char)(((~temp1) & temp2) | (temp1 & (~temp2))); 
            count += 1;
            if (count >= sizeof(maskKey)) 
                count = 0;
        }
        *package = '\0';
    }
    else 
    {
        memcpy(package, &data[dataStart], len);
        package[len] = '\0';
    }
    //
    printf("end %s ...\n", __FUNCTION__);
    return ret;
}
int webSocket_clientLinkToServer(char *ip, int port, char *interface_path, int sslFlag)
{
    int ret, fd, timeOut;
    int i;
    unsigned char loginBuf[1024] = {0}, recBuf[1024] = {0}, shakeKey[128] = {0}, *p;
    char tempIp[128] = {0};
    unsigned long on = 1;
    int fdmax, j, bytes;
    fd_set master, read_fds;	
    struct sockaddr_in report_addr;
	//struct ssl_client * ssl;
    memset(&report_addr, 0, sizeof(report_addr)); 
    report_addr.sin_family = AF_INET;            
    //report_addr.sin_addr.s_addr = inet_addr(ip);
    if ((report_addr.sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE) 
    {
        ret = webSocket_getIpByHostName(ip, tempIp);
       
        if (ret < 0)
            return ret;
        else if (strlen(tempIp) < 7)
            return -ret;
        else
            timeOut += ret;
        //
        if ((report_addr.sin_addr.s_addr = inet_addr(tempIp)) == INADDR_NONE)
            return -ret;
#ifdef WEBSOCKET_DEBUG
        printf("webSocket_clientLinkToServer : Host(%s) to Ip(%s)\r\n", ip, tempIp);
#endif
    }
    report_addr.sin_port = htons(port); 
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("webSocket_login : cannot create socket\r\n");
        return -1;
    }
	
//	ioctlsocket(fd, FIONBIO, &on);
    timeOut = 0;
	
	//printf("ip:%s,port:%d\n",report_addr.sin_addr.s_addr,report_addr.sin_port);
    while (connect(fd, (struct sockaddr *)&report_addr, sizeof(struct sockaddr)) == -1)
    {
        if (++timeOut > WEBSOCKET_LOGIN_CONNECT_TIMEOUT)
        {
            printf("webSocket_login : cannot connect to %s:%d ! %d\r\n", ip, port, timeOut);
            close(fd);
            return -timeOut;
        }
        webSocket_delayms(1); //1ms
        OSATaskSleep(200);
    }
	
    printf("web_ssl_client_init sslFlag %d, %d\n", sslFlag, (web_ssl_c == NULL));	
	
    if (sslFlag){
    	web_use_ssl = sslFlag;
    	if (!web_ssl_c){
    		web_ssl_c = ssl_client_init(fd);		
    	}		
    }
//
    printf("web_ssl_client_init %d\n",(web_ssl_c == NULL));

    OSATaskSleep(2);
    if ((web_use_ssl) && (!web_ssl_c))
	{
		close(fd);
		return -1;
    }

    memset(shakeKey, 0, sizeof(shakeKey));
    webSocket_buildShakeKey(shakeKey); 

    memset(loginBuf, 0, sizeof(loginBuf)); 
    webSocket_buildHttpHead(ip, port, interface_path, shakeKey, (char *)loginBuf);

    printf("loginBuf %d, %s\n", strlen((char *)loginBuf), loginBuf);

    if (!web_use_ssl){
		ret = send(fd, loginBuf, strlen((const char *)loginBuf), MSG_NOSIGNAL);
    }else{
		ret = ssl_write(&(web_ssl_c->ssl), (const unsigned char *)loginBuf, strlen((const char *)loginBuf));
    }
    printf("ssl_write ret %d,fd:%d", ret,fd);
	
#ifdef WEBSOCKET_DEBUG
    printf("\r\nret : %dms\r\nlen:%d\nlogin_send:\r\n%s\r\n", ret,strlen((const char *)loginBuf), loginBuf);
#endif
	fdmax = 64;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(fd, &master);

    while (1)
    {
        read_fds = master;
        if (select(fdmax, &read_fds, NULL, NULL, NULL) == -1) {
        	printf("%s[%d], select error %d", __FUNCTION__,0,errno);
        	OSATaskSleep(200);
        	continue;
        }
    
        memset(recBuf, 0, sizeof(recBuf));
        if (!web_use_ssl){		
        	ret = recv(fd, recBuf, sizeof(recBuf), MSG_NOSIGNAL);
        }else{
        	ret = ssl_read(&(web_ssl_c->ssl), recBuf, sizeof(recBuf));
        }
        printf("\r\nlogin_recv : %d / %dms\r\n %s\r\n", ret, timeOut, recBuf);
		
	if (ret == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY){
		return -1;
	}

        if (ret > 0)
        {
            printf("\r\nlogin_recv : %d / %dms\r\n%s\r\n", ret, timeOut, recBuf);
			webSocket_delayms(100);
            if (strncmp((const char *)recBuf, (const char *)"HTTP", strlen((const char *)"HTTP")) == 0) 
            {
               
#ifdef WEBSOCKET_DEBUG
                printf("\r\nlogin_recv : %d / %dms\r\n%s\r\n", ret, timeOut, recBuf);
#endif
                //
                if (((p = (unsigned char *)strstr((const char *)recBuf, (const char *)"Sec-WebSocket-Accept: ")) != NULL) 
					||((p = (unsigned char *)strstr((const char *)recBuf, (const char *)"sec-websocket-accept: ")) != NULL))
                {
                    p += strlen((const char *)"sec-websocket-accept: ");
                    sscanf((const char *)p, "%s\r\n", p);
					
                    printf("\r\nsec-websocket-accept:%s\r\n", p);

                    if (webSocket_matchShakeKey(shakeKey, strlen((const char *)shakeKey), p, strlen((const char *)p)) == 0){
                        return fd;     
                    } else{
                    	if (!web_use_ssl){
                    		ret = send(fd, loginBuf, strlen((const char *)loginBuf), MSG_NOSIGNAL);
                    	}else{
                    		ret = ssl_write(&(web_ssl_c->ssl), (const unsigned char *)loginBuf, strlen((const char *)loginBuf));
                    	}
                    }
                }
                else
                {
                    	if (!web_use_ssl){
                    		ret = send(fd, loginBuf, strlen((const char *)loginBuf), MSG_NOSIGNAL);
                    	}else{
                    		ret = ssl_write(&(web_ssl_c->ssl), (const unsigned char *)loginBuf, strlen((const char *)loginBuf));
                    	}
                }
            }
            else
            {
                if (recBuf[0] >= ' ' && recBuf[0] <= '~')
                    printf("\r\nlogin_recv : %d\r\n%s\r\n", ret, recBuf);
                else
                {
                    printf("\r\nlogin_recv : %d\r\n", ret);
                    for (i = 0; i < ret; i++)
                        printf("%.2X ", recBuf[i]);
                    printf("\r\n");
                }
            }
            // #endif
        }
        else if (ret <= 0)
            ;
        if (++timeOut > WEBSOCKET_LOGIN_RESPOND_TIMEOUT)
        {
            //close(fd);
            //return -timeOut;
            break;
        }
        webSocket_delayms(1); //1ms
        OSATaskSleep(2);
    }

    close(fd);
    return -timeOut;
}

int webSocket_serverLinkToClient(int fd, char *recvBuf, int bufLen)
{
    char *p;
    int ret;
    char recvShakeKey[512], respondPackage[1024];
    if ((p = strstr(recvBuf, "Sec-WebSocket-Key: ")) == NULL)
        return -1;
    p += strlen("Sec-WebSocket-Key: ");
    //
    memset(recvShakeKey, 0, sizeof(recvShakeKey));
    sscanf(p, "%s", recvShakeKey);
    ret = strlen(recvShakeKey);
    if (ret < 1)
        return -1;
    //
    memset(respondPackage, 0, sizeof(respondPackage));
    webSocket_buildHttpRespond((unsigned char *)recvShakeKey, (unsigned int)ret, respondPackage);
    //
    if (!web_use_ssl){
    	return send(fd, respondPackage, strlen(respondPackage), MSG_NOSIGNAL);;
    }else{
    	return ssl_write(&(web_ssl_c->ssl), (const unsigned char *)respondPackage, strlen(respondPackage));
    }	
}

int webSocket_send(int fd, char *data, int dataLen, bool isMask, WebsocketData_Type type)

{
    unsigned char *webSocketPackage = NULL;
    int retLen, ret;
#ifdef WEBSOCKET_DEBUG
    unsigned int i;
    printf("webSocket_send : %d\r\n", dataLen);
#endif

    webSocketPackage = (unsigned char *)web_Calloc(dataLen + 128, sizeof(char));
    retLen = webSocket_enPackage((unsigned char *)data, dataLen, webSocketPackage, (dataLen + 128), isMask, type);

#ifdef WEBSOCKET_DEBUG
    printf("webSocket_send : %d\r\n", retLen);
    for (i = 0; i < retLen; i++)
        printf("%.2X ", webSocketPackage[i]);
    printf("\r\n");
#endif
    if (!web_use_ssl){
    	ret = send(fd, webSocketPackage, retLen, MSG_NOSIGNAL);
    }else{
    	if(!web_ssl_c)
			return -1;
    	ret = ssl_write(&(web_ssl_c->ssl), (const unsigned char *)webSocketPackage, retLen);
    }	
    free(webSocketPackage);
    return ret;
}


int webSocket_recv(int fd, char *data, int dataMaxLen, WebsocketData_Type *dataType)
{
	printf("enter %s ...\n", __FUNCTION__);
    unsigned char *webSocketPackage = NULL, *recvBuf = NULL;
    int ret, dpRet = WDT_NULL, retTemp, retFinal = 0;
    int retLen = 0, retHeadLen = 0;
    int timeOut = 0; 
	int i,fdmax;
	fd_set master, read_fds;
	
    recvBuf = (unsigned char *)web_Calloc(dataMaxLen, sizeof(char));
	fdmax = 64;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(fd, &master);
	
	read_fds = master;
	if (select(fdmax, &read_fds, NULL, NULL, NULL) == -1) {
		printf("%s[%d], select error %d", __FUNCTION__,0,errno);
		OSATaskSleep(200);
	}

	
    if (!web_use_ssl){		
    	ret = recv(fd, recvBuf, dataMaxLen, MSG_NOSIGNAL);
    }else{
        if(!web_ssl_c)
			return -1;
    	ret = ssl_read(&(web_ssl_c->ssl), recvBuf, dataMaxLen);
    }
	printf("ret:%d\n",ret);
    if (ret == dataMaxLen)
        printf("webSocket_recv : warning !! recv buff too large !! (recv/%d)\r\n", ret);
    if (ret > 0)
    {
      	for (i = 0; i < ret; i++)
        	printf("%.2X ", recvBuf[i]);
        webSocketPackage = (unsigned char *)web_Calloc(ret + 128, sizeof(char));
        dpRet = webSocket_dePackage(recvBuf, ret, webSocketPackage, (ret + 128), (unsigned int *)&retLen, (unsigned int *)&retHeadLen);
		printf("dataLen:%d,packageLen:%d,headLen:%d,dpRet:%d\n",ret,retLen,retHeadLen,dpRet);
		if (dpRet == WDT_ERR && retLen == 0)
        {
            memset(data, 0, dataMaxLen);
            if (ret < dataMaxLen)
            {
                memcpy(data, recvBuf, ret);
                retFinal = -ret;
            }
            else
            {
                memcpy(data, recvBuf, dataMaxLen);
                retFinal = -dataMaxLen;
            }
        }
        else 
        {
            if (retLen > dataMaxLen)
            {
                printf("webSocket_recv : warning !! recv package too large !! (recvPackage/%d)\r\n", retLen);
                goto recv_return_null;
            }

#if 0
            if (ret > 10)
                printf("webSocket_recv : ret/%d, dpRet/%d, retLen/%d, head/%d : %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
                       ret, dpRet, retLen, retHeadLen,
                       recvBuf[0], recvBuf[1], recvBuf[2], recvBuf[3], recvBuf[4],
                       recvBuf[5], recvBuf[6], recvBuf[7], recvBuf[8], recvBuf[9]);
#endif
            if (ret < retHeadLen + retLen)
            {
                timeOut = 50; 
                while (ret < retHeadLen + retLen)
                {
                    webSocket_delayms(10);
                    if (!web_use_ssl){		
                    	retTemp = recv(fd, &recvBuf[ret], dataMaxLen - ret, MSG_NOSIGNAL);
                    }else{
                    	
                    	retTemp = ssl_read(&(web_ssl_c->ssl), &recvBuf[ret], dataMaxLen - ret);
                    }	
					printf("rettmp:%d\n",retTemp);
                    if (retTemp > 0)
                    {
                        timeOut = 50; 
                        ret += retTemp;
                    }
                    else
                    {
                        if (errno == EAGAIN || errno == EINTR)
                            ; 
                        else
                            goto recv_return_null;
                    }
                    if (--timeOut < 1)
                        goto recv_return_null;
                }
				if(webSocketPackage)
                	free(webSocketPackage);
                webSocketPackage = (unsigned char *)web_Calloc(ret + 128, sizeof(char));
                dpRet = webSocket_dePackage(recvBuf, ret, webSocketPackage, (ret + 128), (unsigned int *)&retLen, (unsigned int *)&retHeadLen);
                if (ret > 10)
                    printf("webSocket_recv : ret/%d, dpRet/%d, retLen/%d, head/%d : %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
                           ret, dpRet, retLen, retHeadLen,
                           recvBuf[0], recvBuf[1], recvBuf[2], recvBuf[3], recvBuf[4],
                           recvBuf[5], recvBuf[6], recvBuf[7], recvBuf[8], recvBuf[9]);
            }
            if (retLen > 0)
            {
                if (dpRet == WDT_PING)
                {
                    webSocket_send(fd, (char *)webSocketPackage, retLen, true, WDT_PONG);
                    printf("webSocket_recv : PING %d\r\n%s\r\n", retLen, webSocketPackage);
                }
                else if (dpRet == WDT_PONG)
                {
                    printf("webSocket_recv : PONG %d\r\n%s\r\n", retLen, webSocketPackage);
                }
                else //if(dpRet == WDT_TXTDATA || dpRet == WDT_BINDATA || dpRet == WDT_MINDATA)
                {
                    memcpy(data, webSocketPackage, retLen);
#if 0
                    if (webSocketPackage[0] >= ' ' && webSocketPackage[0] <= '~')
                        printf("\r\nwebSocket_recv : New Package StrFile dpRet:%d/retLen:%d\r\n%s\r\n", dpRet, retLen, webSocketPackage);
                    else
                    {
                        printf("\r\nwebSocket_recv : New Package BinFile dpRet:%d/retLen:%d\r\n", dpRet, retLen);
                        int i;
                        for (i = 0; i < retLen; i++)
                            printf("%.2X ", webSocketPackage[i]);
                        printf("\r\n");
                    }
#endif
                }
                retFinal = retLen;
            }
#if 0
            else
            {
                if (recvBuf[0] >= ' ' && recvBuf[0] <= '~')
                    printf("\r\nwebSocket_recv : ret:%d/dpRet:%d/retLen:%d\r\n%s\r\n", ret, dpRet, retLen, recvBuf);
                else
                {
                    printf("\r\nwebSocket_recv : ret:%d/dpRet:%d/retLen:%d\r\n%s\r\n", ret, dpRet, retLen, recvBuf);
                    int i;
                    for (i = 0; i < ret; i++)
                        printf("%.2X ", recvBuf[i]);
                    printf("\r\n");
                }
            }
#endif
        }
    }
    if (recvBuf)
        free(recvBuf);
    if (webSocketPackage)
        free(webSocketPackage);
    if (dataType)
        *dataType = dpRet;
	printf("end %s ...\n", __FUNCTION__);
    return retFinal;
recv_return_null:
    if (recvBuf)
        free(recvBuf);
    if (webSocketPackage)
        free(webSocketPackage);
    if (dataType)
        *dataType = dpRet;
	printf("end %s ...\n", __FUNCTION__);
    return 0;
}

void websocket_close(int fd)
{
	printf("enter %s ...\n", __FUNCTION__);
	if(fd)	
		closesocket(fd);
	fd = -1;
	
	if(web_ssl_c)
	{
		ssl_close_notify(&web_ssl_c->ssl);
		ssl_free(&web_ssl_c->ssl);
		free(web_ssl_c);
	}
	web_ssl_c = NULL;
	
	//ssl_client_shutdown(web_ssl_c);
	
}

