//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_ali_mqtt.c
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

#include "incl_config.h"
#include "utils_common.h"
#include "lib_ali_mqtt.h"
#include "lib_net.h"
#include "cJSON.h"
#include "lib_ali_http.h"



#ifdef INCL_EXAMPLES_ALI_MQTT
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;


static OSMsgQRef MqttSDKMsgQ = NULL;
#define MI_MQTT_REG 0
#define MI_MQTT_DATA 1
typedef struct{    
	int paramID;
    int len;
	void *data;
}mqttParam;

// Private variables ------------------------------------------------------------
static mqtt_client_t *client = NULL;
static int regFlag = 0;

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
	OSA_STATUS status;
    
	status = OSAMsgQCreate(&MqttSDKMsgQ, "MsgMQTT_sdk", sizeof(mqttParam), 500, OS_FIFO);

	ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
	
}
static void sub_topic_handle1(void* client, message_data_t* msg)
 {
	 (void) client;
	 mqttParam apiMsg = { 0 };
	 sdk_uart_printf("-----------------------------------------------------------------------------------");
	 sdk_uart_printf("%s:%d %s()...\n,topic: %s\n,message:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
	 sdk_uart_printf("-----------------------------------------------------------------------------------");
	char* data = (char*)malloc(1024);
	memcpy(data,msg->message->payload,msg->message->payloadlen);
	apiMsg.data =  (void*)data;
	apiMsg.len = msg->message->payloadlen;
	if(memcmp(msg->topic_name,"/ext/register", strlen("/ext/register")) == 0)
	{
		apiMsg.paramID = MI_MQTT_REG;
	}else{
		apiMsg.paramID = MI_MQTT_DATA;
	}
	 if (OS_SUCCESS != OSAMsgQSend(MqttSDKMsgQ, sizeof(mqttParam), (UINT8 *)&apiMsg, OSA_NO_SUSPEND)) 
	{
		return;
	}
 }
 static int mqtt_publish_handle1(mqtt_client_t *client)
 {
	 mqtt_message_t msg;
	 memset(&msg, 0, sizeof(msg));
 
	 msg.qos = QOS0;
	 msg.payload = (void *)"this is a kawaii mqtt test ...";
 
	 return lib_mqtt_publish(client, "/a1JtaPZxlFA/dmh003/user/update", &msg);
 }

void mqtt_sample1(void)
{
    int rc = -1;
	
	sdk_uart_printf("%s()... mqtt test begin...", __FUNCTION__);
	
	mqttParam apiMsg = { 0 };
	
	char ProductKey[] = "a1JtaPZxlFA";
	char DeviceName[] = "dmh003";
	char DeviceSecret[64]   = {0};
	char product_secret[] = "PuiL030X5KnRiRSh";
	regFlag = 1; //0-普通连接 1-动态注册
	char MQTT_SUB_TOPIC[] = "/a1JtaPZxlFA/dmh003/user/get";
    char MQTT_PUB_TOPIC[] = "/a1JtaPZxlFA/dmh003/user/update";

	lib_ali_http_authenticate(ProductKey,DeviceName,DeviceSecret,product_secret);//dmh test

ReCONN:
	client = lib_mqtt_client_init();
	rc = lib_ali_mqtt_authenticate(client,ProductKey,DeviceName,DeviceSecret,product_secret,regFlag);
	sdk_uart_printf("%s() iotx_ali_guider_authenticate rc code:%d", __FUNCTION__,rc);
	sdk_uart_printf("device_secret:%s",DeviceSecret);

	client->mqtt_host = (char *)malloc(256);
	if (client->mqtt_host == NULL) {
		return ;
	}
	memset(client->mqtt_host,0,256);
	//snprintf(client->mqtt_host, 256,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",ProductKey);
	snprintf(client->mqtt_host, 256,"%s.%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",ProductKey,ProductKey);
    lib_mqtt_set_port(client, "1883");
    lib_mqtt_set_clean_session(client, 1);
	lib_mqtt_set_clean_session(client, 1);
	sdk_uart_printf("mqtt_client addr: %s", client->mqtt_host);
	sdk_uart_printf("mqtt_client client_id: %s", client->mqtt_client_id);
	sdk_uart_printf("mqtt_client port: %s", client->mqtt_port);
	sdk_uart_printf("mqtt_client timeOut: %d", client->mqtt_cmd_timeout);
	sdk_uart_printf("mqtt_client version: %d", client->mqtt_version);
	sdk_uart_printf("mqtt_client usr: %s", client->mqtt_user_name);
	sdk_uart_printf("mqtt_client password: %s", client->mqtt_password);

	lib_mqtt_set_ssl(client,regFlag);
	lib_mqtt_set_interceptor_handler(client,sub_topic_handle1);
	
   	rc = lib_mqtt_connect(client);
	sdk_uart_printf("%s() mqtt_connect rc code:%d", __FUNCTION__,rc);

	
	sdk_uart_printf("mqtt_connect status:%d", client->mqtt_client_state);
	if(regFlag == 0)
	{
		OSATaskSleep(100);
    	rc = lib_mqtt_subscribe(client, MQTT_SUB_TOPIC, QOS0, sub_topic_handle1);
   	 	sdk_uart_printf("%s() lib_mqtt_subscribe rc code:%d", __FUNCTION__,rc);
	 	rc =  mqtt_publish_handle1(client);
		sdk_uart_printf("%s() mqtt_publish_handle1 rc code:%d", __FUNCTION__,rc);
	}
	while(1)
	{
		memset(&apiMsg,0,sizeof(mqttParam));
		cJSON * json = NULL;
		OSAMsgQRecv(MqttSDKMsgQ, (UINT8 *)&apiMsg, sizeof(mqttParam), OSA_SUSPEND);
		if(apiMsg.len)
		{
			switch(apiMsg.paramID)
			{
				case MI_MQTT_REG:
					sdk_uart_printf("len:%d,data:%s",apiMsg.len,(char*)apiMsg.data);
					json = cJSON_Parse((char*)apiMsg.data); 
					if(json)
					{
						cJSON * deviceSecret = NULL;
						deviceSecret = cJSON_GetObjectItem(json, "deviceSecret"); 
						if(deviceSecret)
						{
							//memcpy(DeviceSecret,deviceSecret->valuestring,strlen(deviceSecret->valuestring));
							sprintf(DeviceSecret,"%s",deviceSecret->valuestring);
							regFlag = 0;
							lib_mqtt_disconnect(client);
							lib_mqtt_shutdown(client);
							goto ReCONN;
						}
					}
					break;
				case MI_MQTT_DATA:
					sdk_uart_printf("len:%d,data:%s",apiMsg.len,(char*)apiMsg.data);
					break;
			}

			if(apiMsg.data)
				free(apiMsg.data);
		}
	}
}
static void _task(void *ptr)
{
	int ret;
	ret = lib_net_tcpip_ok(50);
	if(ret != 0){
		 sdk_uart_printf("join net error!\n");
	}	
	
	mqtt_sample1();
}
#endif /* ifdef INCL_EXAMPLES_ALI_MQTT.2021-12-2 17:04:04 by: win */

// End of file : examples_ali_mqtt.h 2021-12-2 16:47:08 by: win 

