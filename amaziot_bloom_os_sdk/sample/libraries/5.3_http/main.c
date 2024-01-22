//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-18
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

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include "osa.h"
#include "utils.h"
#include "libhttpclient.h"
#include "utlAtParser.h"
#include "teldef.h"
#include "sdk_api.h"
#include "teldef.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_https_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)


#define sample_https_sleep(x) OSATaskSleep((x) * 200)//second
#define SAMPLE_HTTPS_TASK_STACK_SIZE     1024*10

// Private variables ------------------------------------------------------------

static void* sample_https_task_stack = NULL;
static OSTaskRef sample_https_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_https_task(void *ptr);

// Public functions prototypes --------------------------------------------------

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

    sample_https_task_stack = malloc(SAMPLE_HTTPS_TASK_STACK_SIZE);
    ASSERT(sample_https_task_stack != NULL);

    ret = OSATaskCreate(&sample_https_task_ref, sample_https_task_stack, SAMPLE_HTTPS_TASK_STACK_SIZE, 89, "test-task", sample_https_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

extern BOOL isMasterSim0(void);
extern void PM812_SW_RESET(void);
extern UINT8 getCeregReady(UINT32 atHandle);

//detect net status, enable or disable
static void wait_dev_reg_net(void)
{
    int count = 0;

    while(!getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){

        OSATaskSleep(200);
        count++;
        if (count > 150){
            PM812_SW_RESET();
        }
    }
}
int sample_https_test(void);
static void sample_https_task(void *ptr)
{
    wait_dev_reg_net();
    
    sample_https_test();
}

#include "core_http.h"
#include "aiot_state_api.h"


/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;


/* 日志回调函数, SDK的日志会从这里输出 */
int32_t demo_state_logcb(int32_t code, char *message)
{
    printf("%s", message);
    return 0;
}


/* HTTP事件回调函数, 当SDK读取到网络报文时被触发, 报文描述类型 */
void sample_https_recv_handler(void *handle, const http_recv_t *packet, void *userdata)
{
    printf("%s, type %d", __FUNCTION__, packet->type);
    switch (packet->type) {
        case HTTPRECV_STATUS_CODE: {
            /* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP状态码, 如404, 200, 302等 */
            printf("status code: %d\n", packet->data.status_code.code);
        }
        break;

        case HTTPRECV_HEADER: {
            /* TODO: 以下代码如果不被注释, SDK收到HTTP报文时, 会通过这个用户回调打印HTTP首部, 如Content-Length等 */
            printf("key: %s, value: %s\n", packet->data.header.key, packet->data.header.value);
        }
        break;

        /* TODO: 如果需要处理云平台的HTTP回应报文, 修改这里, 现在只是将回应打印出来 */
        case HTTPRECV_BODY: {
            printf("%d,%s\r\n", packet->data.body.len, packet->data.body.buffer);
        }
        break;

        default: {
        }
        break;

    }
}

char  *http_host = "www.baidu.com";
//uint16_t host_port = 80;
uint16_t host_port = 443;
uint16_t connect_timeout = CORE_HTTP_DEFAULT_CONNECT_TIMEOUT_MS*2;
uint16_t send_timeout = CORE_HTTP_DEFAULT_SEND_TIMEOUT_MS*2;
uint16_t recv_timeout = CORE_HTTP_DEFAULT_RECV_TIMEOUT_MS*2;
uint16_t header_line_max_len = 512;

int sample_https_test(void)
{
    int32_t     res = -1;
    core_http_handle_t *http_handle = NULL;

    core_http_request_t request;
    core_http_response_t response;
    uint64_t timenow_ms = 0;
    aiot_sysdep_network_cred_t cred;
    uint8_t isTLS = 1;
    

    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile, isTLS);    
    /* 配置SDK的日志输出 */
    //aiot_state_set_logcb(demo_state_logcb);

    http_handle = core_http_init();
    if (http_handle == NULL) {
        printf("%s[%d], res: %d", __FUNCTION__, __LINE__, res);
        return res;
    }

    http_handle->long_connection = 0;

    res = core_http_setopt(http_handle, CORE_HTTPOPT_HOST, http_host);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_HOST, res);
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_PORT, &host_port);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_PORT, res);

    if (isTLS){        
        /* 创建SDK的安全凭据, 用于建立TLS连接 */
        memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
        cred.ssl_verify_type = MBEDTLS_SSL_VERIFY_NONE;
        //cred.x509_server_cert = ali_ca_cert;
        //cred.x509_server_cert_len = strlen(ali_ca_cert);
        
        res = core_http_setopt(http_handle, CORE_HTTPOPT_NETWORK_CRED, &cred);
        printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_NETWORK_CRED, res);        
    }
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_CONNECT_TIMEOUT_MS, &connect_timeout);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_CONNECT_TIMEOUT_MS, res);
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_SEND_TIMEOUT_MS, &send_timeout);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_SEND_TIMEOUT_MS, res);
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_RECV_TIMEOUT_MS, &recv_timeout);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_RECV_TIMEOUT_MS, res);
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_HEADER_LINE_MAX_LEN, &header_line_max_len);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_HEADER_LINE_MAX_LEN, res);
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_RECV_HANDLER, sample_https_recv_handler);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_RECV_HANDLER, res);

    res = core_http_setopt(http_handle, CORE_HTTPOPT_USERDATA, (void *)&response);
    printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_RECV_HANDLER, res);
   
    if ((res = core_http_connect(http_handle)) < STATE_SUCCESS) {
        printf("%s[%d], res: %d", __FUNCTION__, __LINE__, res);
        goto exit;
    }

    memset(&request, 0, sizeof(core_http_request_t));
    request.method = "GET";
    //request.method = "POST";
    request.path = "/";
    request.header = (http_handle->long_connection == 0) ? ("Content-Type: application/json\r\nConnection: close\r\n") :
                     ("Content-Type: application/json\r\n");
    //request.content = (uint8_t *)content;
    //request.content_len = (uint32_t)strlen(content);

    res = core_http_send(http_handle, &request);
    printf("%s[%d], res: %d", __FUNCTION__, __LINE__, res);
    //http_handle->sysdep->core_sysdep_free(path);
    //http_handle->sysdep->core_sysdep_free(content);
    
    timenow_ms = http_handle->sysdep->core_sysdep_time();
    while (1) {
        if (timenow_ms >= http_handle->sysdep->core_sysdep_time()) {
            timenow_ms = http_handle->sysdep->core_sysdep_time();
        }
        if (http_handle->sysdep->core_sysdep_time() - timenow_ms >= http_handle->recv_timeout_ms) {
            break;
        }

        res = core_http_recv(http_handle);
        if (res < STATE_SUCCESS) {
            break;
        }
    }    
    
    if (res == STATE_HTTP_READ_BODY_FINISHED || res == STATE_HTTP_READ_BODY_EMPTY){        
        printf("HTTP FINISHED!\r\n");
    }
exit:
    /* 销毁HTTP实例 */
    core_http_deinit((void **)&http_handle);
    printf("program exit as normal return\r\n");

    return 0;
}

// End of file : main.c 2023-5-18 9:24:29 by: zhaoning 

