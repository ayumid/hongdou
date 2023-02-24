//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_os_mqtt.c
// Auther      : win
// Version     :
// Date : 2021-12-10
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-10
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "lib_os_mqtt.h"
#include "lib_net.h"
#include "cJSON.h"

#ifdef INCL_EXAMPLES_OSMQTT
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;
// Private variables ------------------------------------------------------------
char MQTT_IP[] = "studio-mqtt.heclouds.com";             /* onenet MQTT IP地址 */
char MQTT_PORT[] = "1883";                                /* onenet MQTT 端口号 */
char MQTT_SUB_TOPIC[128] = {0};
char MQTT_PUB_TOPIC[128] = {0};

char OS_PRODUCT_ID[]    = "66p7569eOu";                                     /* onenet 产品ID */
char OS_DEVICE_NAME[]   = "AM430E01";                                       /* onenet 设备名称*/
char OS_ACCESS_KEY[]    = "KUdpJdcrzW19Af3J9znlhTfhwYvlFUGx8x9BWzGRFUE=";   /* onenet 产品秘钥*/

static mqtt_client_t *client = NULL;

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
    msg.payload = (void *)"{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"CurrentTemperature\":{\"value\":30.5}}}";

	sprintf(MQTT_PUB_TOPIC,"$sys/%s/%s/thing/property/post",OS_PRODUCT_ID,OS_DEVICE_NAME);
	
    return lib_mqtt_publish(client, MQTT_PUB_TOPIC, &msg);
}

static void _task(void *ptr)
{
    int ret;
	char token[DEV_TOKEN_LEN]={0};
	ret = lib_net_tcpip_ok(50);
	if(ret != 0){
		 sdk_uart_printf("join net error!\n");
	}	
    client = lib_mqtt_client_init();

	lib_get_os_mqtt_token(token,1640793600,OS_PRODUCT_ID,OS_DEVICE_NAME,OS_ACCESS_KEY);

    lib_mqtt_set_host(client, MQTT_IP);
    lib_mqtt_set_port(client, MQTT_PORT);
    lib_mqtt_set_client_id(client, OS_DEVICE_NAME);
	lib_mqtt_set_user_name(client, OS_PRODUCT_ID);
	lib_mqtt_set_password(client,token);
    lib_mqtt_set_clean_session(client, 1);
	
   	ret = lib_mqtt_connect(client);
	sdk_uart_printf("%s() lib_mqtt_connect ret code:%d", __FUNCTION__,ret);
	
	OSATaskSleep(100);
	
	sprintf(MQTT_SUB_TOPIC,"$sys/%s/%s/thing/property/post/reply",OS_PRODUCT_ID,OS_DEVICE_NAME);
    ret = lib_mqtt_subscribe(client, MQTT_SUB_TOPIC, QOS0, sub_topic_handle1);
    sdk_uart_printf("%s() lib_mqtt_subscribe ret code:%d", __FUNCTION__,ret);
	
    while (1) {
        ret = mqtt_publish_handle1(client);
	    sdk_uart_printf("%s() mqtt_publish_handle1 ret code:%d", __FUNCTION__,ret);
        sleep(10);
    }
    
}
#endif /* ifdef INCL_EXAMPLES_OSMQTT.2021-12-10 15:10:45 by: win */

// End of file : examples_os_mqtt.h 2021-12-10 14:07:58 by: win 

