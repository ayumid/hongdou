//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_mqtt.c
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
#include "lib_mqtt.h"
#include "drv_timer.h"
#include "lib_net.h"


#ifdef INCL_EXAMPLES_MQTT
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

// Private variables ------------------------------------------------------------
mqtt_client_t *client = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
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
{
    int ret;

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
	
}
static void sub_topic_handle1(void* client, message_data_t* msg)
{
    (void) client;
    sdk_uart_printf("-----------------------------------------------------------------------------------");
    sdk_uart_printf("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    sdk_uart_printf("-----------------------------------------------------------------------------------");
}
static int mqtt_publish_handle1(mqtt_client_t *client)
{
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

    msg.qos = QOS0;
    msg.payload = (void *)"this is a kawaii mqtt test ...";

    return lib_mqtt_publish(client, "rtt-topic", &msg);
}

static void _task(void *ptr)
{
    int ret;
	ret = lib_net_tcpip_ok(50);
	if(ret != 0){
		 sdk_uart_printf("join net error!\n");
	}	
    client = lib_mqtt_client_init();

    lib_mqtt_set_host(client, "test.mosquitto.org");
    lib_mqtt_set_port(client, "1883");
    lib_mqtt_set_client_id(client, "asr-mqtt-a5a5");
    lib_mqtt_set_clean_session(client, 1);
   	ret = lib_mqtt_connect(client);
	sdk_uart_printf("%s() lib_mqtt_connect ret code:%d", __FUNCTION__,ret);
	OSATaskSleep(100);
    ret = lib_mqtt_subscribe(client, "test", QOS0, sub_topic_handle1);
    sdk_uart_printf("%s() lib_mqtt_subscribe ret code:%d", __FUNCTION__,ret);
    while (1) {
        ret = mqtt_publish_handle1(client);
	    sdk_uart_printf("%s() mqtt_publish_handle1 ret code:%d", __FUNCTION__,ret);
        sleep(10);
    }
    
}

#endif /* ifdef INCL_EXAMPLES_MQTT.2021-11-25 16:58:26 by: win */

// End of file : examples_mqtt.h 2021-11-25 16:56:53 by: win 

