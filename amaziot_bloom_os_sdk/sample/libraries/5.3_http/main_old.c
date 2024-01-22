//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main_old.c
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
#define sample_http_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_http_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)


#define sample_http_sleep(x) OSATaskSleep((x) * 200)//second
#define _TASK_STACK_SIZE     1024*10

struct http_data_s {
    unsigned data_sz;
    UINT8 data[45*1024];
};

#define POST_DATA "info=FbaFoIuqwjuspcgU5BL3xwWI7mmgkq2bYDOh+AgnAQjV\r\nuNjXzouXDxoyFdCK+A8RYs2hqIuGZr1J2uCcYG/uV2PvoP+znM+hZtgm8rJtAiOqrJQS2Q8R+UoJiWte/icmCTEYkX4q2/w2jlnQTzhbUjswpYwtPqZ="

// Private variables ------------------------------------------------------------

static void* sample_http_task_stack = NULL;
static OSTaskRef sample_http_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_http_task(void *ptr);

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

extern BOOL isMasterSim0(void);
extern void PM812_SW_RESET(void);
extern UINT8 getCeregReady(UINT32 atHandle);

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

    sample_http_task_stack = malloc(_TASK_STACK_SIZE);
    ASSERT(sample_http_task_stack != NULL);

    ret = OSATaskCreate(&sample_http_task_ref, sample_http_task_stack, _TASK_STACK_SIZE, 89, "test-task", sample_http_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

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

static int sample_http_response_cb(char *buffer, int size, int nitems, void *private_data)
{
    struct http_data_s *client_data = private_data;
    if ((client_data->data_sz + size) < sizeof(client_data->data)) {
        memcpy(client_data->data + client_data->data_sz, buffer, size);
        client_data->data_sz += size;
        return 0;
    }
    return -1;
}

static void sample_http_task(void *ptr)
{
    struct http_client *client = NULL;
    struct http_data_s *client_data = NULL;
    struct http_client_list * header = NULL;
    int response_code = 0;
    int i = 0;
    
    wait_dev_reg_net();
    
    while(1) {

        response_code = 0;
        
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
        i = 1;
        if (i % 3 ==  0){

            http_client_setopt(client, HTTPCLIENT_OPT_URL, "http://www.baidu.com");
            http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, sample_http_response_cb);
            http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
            http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);
            // Add private HTTP header
            header = http_client_list_append(header, "Content-Type: text/xml;charset=UTF-8\r\n");
            header = http_client_list_append(header, "SOAPAction:\r\n");
            http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);

            http_client_perform(client);
            http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
            sample_http_catstudio_printf("[http_client_test]Get tcp state %d\n", response_code);

            if (response_code >= 200 && response_code < 300)
            {
                if(client_data->data_sz)
                {
                    sample_http_catstudio_printf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);                
                    for (i = 0; i < 100; i++)
                    {
                        //sample_http_catstudio_printf("%02x ",client_data->data[i]);
                    }
                }
            }else if (response_code == 404) {
                sample_http_catstudio_printf("response_code == %d\r\n%s",response_code ,client_data->data);
            }            
        }else if (i % 3 ==  1){

            http_client_setopt(client, HTTPCLIENT_OPT_URL, "http://www.baidu.com");
            http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, sample_http_response_cb);        // 只有METHOD设置成HTTPCLIENT_REQUEST_HEAD， 回调函数才会上报http header数据，GET或者POST的时候，不上报http header内容
            http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
            http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_HEAD);        
            // Add private HTTP header
            header = http_client_list_append(header, "Content-Type: text/xml;charset=UTF-8\r\n");
            header = http_client_list_append(header, "SOAPAction:\r\n");
            http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);

            http_client_perform(client);
            http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
            sample_http_catstudio_printf("[http_client_test]Get tcp state %d\n", response_code);

            if (response_code >= 200 && response_code < 300)
            {
                 if(client_data->data_sz)
                 {
                    sample_http_catstudio_printf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);                
                    for (i = 0; i < 100; i++)
                    {
                        //sample_http_catstudio_printf("%02x ",client_data->data[i]);
                    }
                 }
            }else if (response_code == 404) {
                sample_http_catstudio_printf("response_code == %d\r\n%s",response_code ,client_data->data);
            }        
        }else{
            
            http_client_setopt(client, HTTPCLIENT_OPT_URL, "http://eid.dnndo.com/idcard_sdk/api/");
            http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, sample_http_response_cb);
            http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
            http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_POST);
            http_client_setopt(client, HTTPCLIENT_OPT_POSTDATA, POST_DATA); /*post data is http context*/
            http_client_setopt(client, HTTPCLIENT_OPT_POSTLENGTH, strlen(POST_DATA));    /*http context length*/      
            http_client_perform(client);
            http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
            sample_http_catstudio_printf("[http_client_test]Get tcp state %d\n", response_code);

            if (response_code >= 200 && response_code < 300)
            {
                 if(client_data->data_sz)
                 {
                    sample_http_catstudio_printf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);                
                    for (i = 0; i < 100; i++)
                    {
                        //sample_http_catstudio_printf("%02x ",client_data->data[i]);
                    }
                 }
            }else if (response_code == 404) {
                sample_http_catstudio_printf("response_code == %d\r\n%s",response_code ,client_data->data);
            }            
                        
        }

        
        if (client_data){            
            free(client_data);
            client_data = NULL;
        }
            
        if (client){
            http_client_shutdown(client);
            client = NULL;
        }
                    
        OSATaskSleep(30*200);
        i++;
    }
}

// End of file : main_old.c 2023-5-18 9:24:23 by: zhaoning 

