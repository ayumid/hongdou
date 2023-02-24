//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_http.c
// Auther      : win
// Version     :
// Date : 2021-11-25
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-25
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "lib_http.h"
#include "drv_timer.h"
#include "lib_net.h"

#ifdef INCL_EXAMPLES_HTTP
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------
struct http_data_s {
    unsigned data_sz;
    UINT8 data[45*1024];
};

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void _timer_callback(UINT32 tmrId);
static void _task(void *ptr);
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
{int ret;

    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = drv_timer_init(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

 
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    drv_timer_start(_timer_ref, 20 * 200, 20 * 200, _timer_callback, 0); // 10 seconds timer
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
static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}


static void _task(void *ptr)
{
    int ret;
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;
    struct http_client *client = NULL;
    struct http_data_s *client_data = NULL;
    int response_code = 0;
    int i = 0;
    
    ret = lib_net_tcpip_ok(50);
	if(ret != 0){
		 sdk_uart_printf("join net error!\n");
	}	
PRO_START:
    client_data = malloc(sizeof(*client_data));
    if (!client_data)
        goto clean;
    memset(client_data, 0, sizeof(*client_data));

    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
        response_code = 0;
        client = lib_http_init();
        if (!client)
            goto clean;

        lib_http_setopt(client, HTTPCLIENT_OPT_URL, "http://www.baidu.com");
        lib_http_setopt(client, HTTPCLIENT_OPT_RESPONSECB, _response_cb);
        lib_http_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
        lib_http_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);
        lib_http_perform(client);
        lib_http_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
        sdk_uart_printf("[http_client_test]Get tcp state %d\n", response_code);

        if (response_code >= 200 && response_code < 300)
         {
             if(client_data->data_sz)
             {
                sdk_uart_printf("\r\n data_sz=%u, %s", client_data->data_sz,client_data->data);
                sdk_uart_printf("\r\nresult:");
                for (i = 0; i < 100; i++)
                {
                    sdk_uart_printf("%02x ",client_data->data[i]);
                }
             }
        }else if (response_code == 404) {
            sdk_uart_printf("response_code == %d\r\n%s",response_code ,client_data->data);
        }
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            static int count = 0;
            count++;
            sdk_uart_printf("asr_test  _task: count: %d\n", count);

            sleep(3);

            if (count > 10) {
                sdk_uart_printf("asr_test _task: stop timer");
                drv_timer_stop(_timer_ref);
            }

        }
    }
clean:
    if (client)
        lib_http_shutdown(client);
    if (client_data)
        free(client_data);
    drv_timer_stop(_timer_ref);
    goto PRO_START;
}

#endif /* ifdef INCL_EXAMPLES_HTTP.2021-11-25 16:40:06 by: win */

// End of file : examples_http.h 2021-11-25 16:39:56 by: win 

