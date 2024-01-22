//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_http.c
// Auther      : zhaoning
// Version     :
// Date : 2023-9-22
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-22
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_http.h"

#include "osa.h"
#include "utils.h"
#include "libhttpclient.h"
#include "utlAtParser.h"
#include "teldef.h"
#include "sdk_api.h"
#include "teldef.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_HTTP_TASK_MSGQ_MSG_SIZE              (sizeof(DTU_MSG_UART_DATA_PARAM_T))
#define DTU_HTTP_TASK_MSGQ_QUEUE_SIZE            (10)

// Private variables ------------------------------------------------------------

static UINT8 dtu_http_s_task_stack[DTU_HTTP_S_TASK_STACK_SIZE];
static OSMsgQRef    dtu_http_s_msgq = NULL;

static OSTaskRef dtu_http_s_msgq_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

#if 1
struct http_data_s {
    unsigned data_sz;
    UINT8 data[DTU_HTTP_S_RCV_LEN];
};

//#define POST_DATA "{\"payload\":\"testmsg\"}"

/**
  * Function    : dtu_http_response_cb
  * Description : http请求回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_http_response_cb(char *buffer, int size, int nitems, void *private_data)
{
    struct http_data_s *client_data = private_data;
    if ((client_data->data_sz + size) < sizeof(client_data->data)) {
        memcpy(client_data->data + client_data->data_sz, buffer, size);
        client_data->data_sz += size;
        return 0;
    }
    return -1;
}

/**
  * Function    : dtu_http_pg_data_get
  * Description : get请求实现
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_pg_data_get(DTU_MSG_UART_DATA_PARAM_T* data, UINT8 type, UINT8 channel)
{
    struct http_client *client = NULL;
    struct http_data_s *client_data = NULL;
    struct http_client_list * header = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    char url[DTU_HTTP_S_URL_LEN] = {0};
    char head_tmp[DTU_HTTP_HEAD_MAX_LEN] = {0};
    UINT8* d = NULL;
    int response_code = 0;
    int i = 0;

    dtu_file_ctx = dtu_get_file_ctx();
    response_code = 0;
    d = data->UArgs;
    client_data = malloc(sizeof(*client_data));
    if (!client_data){
        return ;            
    }
    memset(client_data, 0, sizeof(*client_data));
    
    client = http_client_init();
    if (!client){
        free(client_data);
        client_data = NULL;
        return ;
    }

    d[data->len] = '\0';
    if(1 == channel)
    {
        if(NULL == memchr(dtu_file_ctx->http.http1.url, '?', strlen(dtu_file_ctx->http.http1.url)))
        {
            snprintf(url, DTU_HTTP_S_URL_LEN, "%s?data=%s", dtu_file_ctx->http.http1.url, (char*)data->UArgs);
        }
        else
        {
            snprintf(url, DTU_HTTP_S_URL_LEN, "%s&data=%s", dtu_file_ctx->http.http1.url, (char*)data->UArgs);
        }
        http_client_setopt(client, HTTPCLIENT_OPT_URL, url);
    }
    else if(2 == channel)
    {
        if(NULL == memchr(dtu_file_ctx->http.http1.url, '?', strlen(dtu_file_ctx->http.http1.url)))
        {
            snprintf(url, DTU_HTTP_S_URL_LEN, "%s?data=%s", dtu_file_ctx->http.http2.url, (char*)data->UArgs);
        }
        else
        {
            snprintf(url, DTU_HTTP_S_URL_LEN, "%s&data=%s", dtu_file_ctx->http.http2.url, (char*)data->UArgs);
        }
        http_client_setopt(client, HTTPCLIENT_OPT_URL, url);
    }
    uprintf("http get url: %s", url);
    http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, dtu_http_response_cb);
    http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
    http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);
    // Add private HTTP header
    if(1 == channel)
    {
        if(strlen(dtu_file_ctx->http.http1.head1))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http1.head1);
            header = http_client_list_append(header, head_tmp);
            uprintf("head11: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http1.head2))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http1.head2);
            header = http_client_list_append(header, head_tmp);
            uprintf("head12: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http1.head3))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http1.head3);
            header = http_client_list_append(header, head_tmp);
            uprintf("head13: %s", head_tmp);
        }
    }
    else if(2 == channel)
    {
        if(strlen(dtu_file_ctx->http.http2.head1))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http2.head1);
            header = http_client_list_append(header, head_tmp);
            uprintf("head21: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http2.head2))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http2.head2);
            header = http_client_list_append(header, head_tmp);
            uprintf("head22: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http2.head3))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http2.head3);
            header = http_client_list_append(header, head_tmp);
            uprintf("head23: %s", head_tmp);
        }
    }
    
    http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);

    http_client_perform(client);
    http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
    uprintf("[http get]Get tcp state %d\n", response_code);

    if (response_code >= 200 && response_code < 300)
    {
        if(client_data->data_sz)
        {
            uprintf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);
            for (i = 0; i < 100; i++)
            {
                //uprintf("%02x ",client_data->data[i]);
            }
        }
    }else if (response_code == 404) {
        uprintf("response_code == %d\r\n%s",response_code ,client_data->data);
    }

    if (client_data)
    {
        free(client_data);
        client_data = NULL;
    }
        
    if (client)
    {
        http_client_shutdown(client);
        client = NULL;
    }

}

/**
  * Function    : dtu_http_pg_data_post
  * Description : post请求实现
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_pg_data_post(DTU_MSG_UART_DATA_PARAM_T* data, UINT8 type, UINT8 channel)
{
    struct http_client *client = NULL;
    struct http_data_s *client_data = NULL;
    struct http_client_list * header = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    char head_tmp[DTU_HTTP_HEAD_MAX_LEN] = {0};
    UINT8* d = NULL;
    int response_code = 0;
    int i = 0;
    
    dtu_file_ctx = dtu_get_file_ctx();
    response_code = 0;
    d = data->UArgs;
    
    client_data = malloc(sizeof(*client_data));
    if (!client_data){
        return ;            
    }
    memset(client_data, 0, sizeof(*client_data));
    
    client = http_client_init();
    if (!client){
        free(client_data);
        client_data = NULL;
        return ;
    }

    if(1 == channel)
    {
        http_client_setopt(client, HTTPCLIENT_OPT_URL, dtu_file_ctx->http.http1.url);
    }
    else if(2 == channel)
    {
        http_client_setopt(client, HTTPCLIENT_OPT_URL, dtu_file_ctx->http.http2.url);
    }
    http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, dtu_http_response_cb);
    http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
    http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_POST);
//    // Add private HTTP header
    if(1 == channel)
    {
        if(strlen(dtu_file_ctx->http.http1.head1))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http1.head1);
            header = http_client_list_append(header, head_tmp);
            uprintf("head11: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http1.head2))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http1.head2);
            header = http_client_list_append(header, head_tmp);
            uprintf("head12: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http1.head3))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http1.head3);
            header = http_client_list_append(header, head_tmp);
            uprintf("head13: %s", head_tmp);
        }
    }
    else if(2 == channel)
    {
        if(strlen(dtu_file_ctx->http.http2.head1))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http2.head1);
            header = http_client_list_append(header, head_tmp);
            uprintf("head21: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http2.head2))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http2.head2);
            header = http_client_list_append(header, head_tmp);
            uprintf("head22: %s", head_tmp);
        }
        if(strlen(dtu_file_ctx->http.http2.head3))
        {
            snprintf(head_tmp, DTU_HTTP_HEAD_MAX_LEN, "%s\r\n", dtu_file_ctx->http.http2.head3);
            header = http_client_list_append(header, head_tmp);
            uprintf("head23: %s", head_tmp);
        }
    }
    d[data->len] = '\0';
    http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);
    http_client_setopt(client, HTTPCLIENT_OPT_POSTDATA, data->UArgs); /*post data is http context*/
    http_client_setopt(client, HTTPCLIENT_OPT_POSTLENGTH, strlen(data->UArgs));    /*http context length*/      
    http_client_perform(client);
    http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
    uprintf("[http post]Get tcp state %d\n", response_code);

    if (response_code >= 200 && response_code < 300)
    {
         if(client_data->data_sz)
         {
            uprintf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);                
            for (i = 0; i < 100; i++)
            {
                //uprintf("%02x ",client_data->data[i]);
            }
         }
    }else if (response_code == 404) {
        uprintf("response_code == %d\r\n%s",response_code ,client_data->data);
    }
    
    if (client_data)
    {
        free(client_data);
        client_data = NULL;
    }
        
    if (client)
    {
        http_client_shutdown(client);
        client = NULL;
    }

}

#endif /* if 0. 2023-4-14 11:48:18 by: zhaoning */

/**
  * Function    : dtu_http_s_task_send_msgq
  * Description : modbus发送消息
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_http_s_task_send_msgq(DTU_MSG_UART_DATA_PARAM_T * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(dtu_http_s_msgq, DTU_HTTP_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        uprintf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->len, status);
        ret = -1;
    }    
    
    return ret;
}


/**
  * Function    : dtu_http_s_task
  * Description : http主任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_s_task(void *ptr)
{
    OSA_STATUS status = 0;
    int ret = 0;
    DTU_MSG_UART_DATA_PARAM_T uart_data = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    while(1)
    {
        //阻塞1s等待从机回复的数据
        status = OSAMsgQRecv(dtu_http_s_msgq, (void *)&uart_data, DTU_HTTP_TASK_MSGQ_MSG_SIZE, 200);
        
        if(status == OS_SUCCESS)
        {
            uprintf("%s[%d] http trans\r\n", __FUNCTION__, __LINE__);
            if(NULL != uart_data.UArgs)
            {
                if(DTU_HTTP1_TYPE_GET == dtu_file_ctx->http.http1.type)
                {
                    dtu_http_pg_data_get(&uart_data, dtu_file_ctx->http.http1.type, 1);
                }
                else if(DTU_HTTP1_TYPE_POST == dtu_file_ctx->http.http1.type)
                {
                    dtu_http_pg_data_post(&uart_data, dtu_file_ctx->http.http1.type, 1);
                }
                
                if(DTU_HTTP2_TYPE_GET == dtu_file_ctx->http.http2.type)
                {
                    dtu_http_pg_data_get(&uart_data, dtu_file_ctx->http.http2.type, 2);
                }
                else if(DTU_HTTP2_TYPE_POST == dtu_file_ctx->http.http2.type)
                {
                    dtu_http_pg_data_post(&uart_data, dtu_file_ctx->http.http2.type, 2);
                }

                //释放数据内存
                free(uart_data.UArgs);
                //置空
                uart_data.UArgs = NULL;
            }
        }
    }
}

/**
  * Function    : dtu_http_s_task_init
  * Description : 初始化http任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_http_s_task_init(void)
{
    OSA_STATUS status = 0;

    /*creat message*/
    status = OSAMsgQCreate(&dtu_http_s_msgq, "dtu_http_s_msgq", DTU_HTTP_TASK_MSGQ_MSG_SIZE, DTU_HTTP_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSATaskCreate(&dtu_http_s_msgq_task_ref, dtu_http_s_task_stack, DTU_HTTP_S_TASK_STACK_SIZE, 161, "http_s_task", dtu_http_s_task, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_http.c 2023-9-22 14:50:54 by: zhaoning 

