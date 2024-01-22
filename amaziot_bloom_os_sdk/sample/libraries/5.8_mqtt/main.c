//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
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
#include "teldef.h"
#include "UART.h"
#include "sys.h"
#include "sdk_api.h"
#include "mqttclient.h"
#include "platform_timer.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_mqtt_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_mqtt_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define sample_mqtt_sleep(x) OSATaskSleep((x)*200) // second
#define SAMPLE_MQTT_TASK_STACK_SIZE 1024 * 2

// Private variables ------------------------------------------------------------

static void* sample_mqtt_task_stack = NULL;
static OSTaskRef sample_mqtt_task_ref = NULL;

/*
char MQTT_IP[] = "183.230.40.39";
int MQTT_PORT = 6002;
char MQTTCLIENT_ID[]  = "600324264";
char MQTTUSERNAME[]   = "324875";
char MQTTPASSWORD[]   = "hT=wtearqIsmztZ=S5CN4ttqOig=";
char MQTT_SUB_TOPIC[]  = "mqtt/sample/#";
char MQTT_PUB_TOPIC[]  = "mqtt/sample/mifi";
*/

mqtt_client_t *sample_mqtt_client = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_mqtt_task(void *ptr);

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

    sample_mqtt_task_stack = malloc(SAMPLE_MQTT_TASK_STACK_SIZE);
    ASSERT(sample_mqtt_task_stack != NULL);
    
    ret = OSATaskCreate(&sample_mqtt_task_ref, sample_mqtt_task_stack, SAMPLE_MQTT_TASK_STACK_SIZE, 100, "test-task", sample_mqtt_task, NULL);
    ASSERT(ret == OS_SUCCESS);

}

static void sample_mqtt_sub_topic_handle1(void *sample_mqtt_client, message_data_t *msg)
{
    (void)sample_mqtt_client;
    sample_mqtt_uart_printf("-----------------------------------------------------------------------------------");
    sample_mqtt_uart_printf("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char *)msg->message->payload);
    sample_mqtt_uart_printf("-----------------------------------------------------------------------------------");
}
static int sample_mqtt_publish_handle1(mqtt_client_t *sample_mqtt_client)
{
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

    msg.qos = QOS0;
    msg.payload = (void *)"this is a mqtt test ...";

    return mqtt_publish(sample_mqtt_client, "/sys/a1H5CPxGtRU/dev001/thing/event/property/post", &msg);
}

void sample_mqtt_sample1(void)
{
    int rc = -1;

    sample_mqtt_uart_printf("%s()... mqtt test begin...", __FUNCTION__);

    sample_mqtt_client = mqtt_lease();

    mqtt_set_host(sample_mqtt_client, "a1H5CPxGtRU.iot-as-mqtt.cn-shanghai.aliyuncs.com");
    mqtt_set_port(sample_mqtt_client, "1883");
    mqtt_set_user_name(sample_mqtt_client, "dev001&a1H5CPxGtRU");
    mqtt_set_password(sample_mqtt_client, "e4a90ef3f364e978c68fde4b1a91e10bef8509d7");
    mqtt_set_client_id(sample_mqtt_client, "a1H5CPxGtRU.dev001|securemode=3,signmethod=hmacsha1|");
    mqtt_set_clean_session(sample_mqtt_client, 1);
    rc = mqtt_connect(sample_mqtt_client);
    sample_mqtt_uart_printf("%s() mqtt_connect rc code:%d", __FUNCTION__, rc);
    OSATaskSleep(100);
    rc = mqtt_subscribe(sample_mqtt_client, "/sys/a1H5CPxGtRU/dev001/thing/service/property/set", QOS0, sample_mqtt_sub_topic_handle1);
    sample_mqtt_uart_printf("%s() mqtt_subscribe rc code:%d", __FUNCTION__, rc);
    while (1)
    {
        rc = sample_mqtt_publish_handle1(sample_mqtt_client);
        sample_mqtt_uart_printf("%s() sample_mqtt_publish_handle1 rc code:%d", __FUNCTION__, rc);
        OSATaskSleep(2000);
    }
}

static void wait_network_ready(void)
{
    int count = 0;
    int ready = 0;

    while (!ready)
    {
        if (getCeregReady(isMasterSim0() ? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
        {
            ready = 1;
        }
        sample_mqtt_uart_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();

        OSATaskSleep(200);
    }
}

static void sample_mqtt_task(void *ptr)
{
    wait_network_ready();
    sample_mqtt_uart_printf("\n\n\nSuccess in the net 2\n\n\n");
    sample_mqtt_sample1();
}

// End of file : main.c 2023-5-17 14:14:26 by: zhaoning 

