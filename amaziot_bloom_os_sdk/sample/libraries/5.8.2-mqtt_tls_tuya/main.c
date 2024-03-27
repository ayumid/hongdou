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

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_mqtt_tls_tuya_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_mqtt_tls_tuya_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define sleep(x) OSATaskSleep((x)*200) // second
#define _TASK_STACK_SIZE 1024 * 5

const char *temp_ca_cert = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
    "-----END CERTIFICATE-----"
};


static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;

static void _task(void *ptr);
/*
char MQTT_IP[] = "183.230.40.39";
int MQTT_PORT = 6002;
char MQTTCLIENT_ID[]  = "600324264";
char MQTTUSERNAME[]   = "324875";
char MQTTPASSWORD[]   = "hT=wtearqIsmztZ=S5CN4ttqOig=";
char MQTT_SUB_TOPIC[]  = "mqtt/sample/#";
char MQTT_PUB_TOPIC[]  = "mqtt/sample/mifi";
*/

mqtt_client_t *client = NULL;

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

    _task_stack = malloc(_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);
	
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 100, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

}

static void sub_topic_handle1(void *client, message_data_t *msg)
{
    (void)client;
    sample_mqtt_tls_tuya_catstudio_printf("-----------------------------------------------------------------------------------");
    sample_mqtt_tls_tuya_catstudio_printf("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char *)msg->message->payload);
    sample_mqtt_tls_tuya_catstudio_printf("-----------------------------------------------------------------------------------");
}
static int mqtt_tls_publish_handle(mqtt_client_t *client)
{
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

    msg.qos = QOS0;
    msg.payload = (void *)"{\"msgId\":\"45lkj3551234\",\"time\":1709718918,\"data\":{\"signal_strength\":{\"value\":31,\"time\":1709718918}}}";

    return mqtt_publish(client, "tylink/26bdc975b6bd846231nqxr/thing/property/report", &msg);
}


//void mqtt_sample_tls_ca(void)
//{
//    int rc = -1;

//    sample_mqtt_tls_tuya_catstudio_printf("%s()... mqtt test begin...", __FUNCTION__);

//    client = mqtt_lease();

//    mqtt_set_host(client, "m1.tuyacn.com");
//    mqtt_set_port(client, "8883");
//    mqtt_set_user_name(client, "26bdc975b6bd846231nqxr|signMethod=hmacSha256,timestamp=1709718918,secureMode=1,accessType=1");
//    mqtt_set_password(client, "46626de7dd8b12d9321ac1d2505d55d202cedd326ba7ee7729d3aa47b15aed7d");
//    mqtt_set_client_id(client, "tuyalink_26bdc975b6bd846231nqxr");

//    //mqtt_set_channel(client, NETWORK_CHANNEL_TLS); //设置ca会自动enable tls,不设置也可以
//    mqtt_set_ca(client, (char*)temp_ca_cert);// 设置ca

//    mqtt_set_clean_session(client, 1);
//    rc = mqtt_connect(client);
//    sample_mqtt_tls_tuya_catstudio_printf("%s() mqtt_connect rc code:%d", __FUNCTION__, rc);
//    OSATaskSleep(100);
//    rc = mqtt_subscribe(client, "tylink/26bdc975b6bd846231nqxr/thing/property/report_response", QOS0, sub_topic_handle1);
//    sample_mqtt_tls_tuya_catstudio_printf("%s() mqtt_subscribe rc code:%d", __FUNCTION__, rc);
//    while (1)
//    {
//        rc = mqtt_tls_publish_handle(client);
//        sample_mqtt_tls_tuya_catstudio_printf("%s() mqtt_tls_publish_handle rc code:%d", __FUNCTION__, rc);
//        OSATaskSleep(2000);
//    }
//}



void mqtt_sample_tls_without_ca(void)
{
    int rc = -1;

    sample_mqtt_tls_tuya_catstudio_printf("%s()... mqtt test begin...", __FUNCTION__);

    client = mqtt_lease();

    mqtt_set_host(client, "m1.tuyacn.com");
    mqtt_set_port(client, "8883");
    mqtt_set_user_name(client, "26bdc975b6bd846231nqxr|signMethod=hmacSha256,timestamp=1709718918,secureMode=1,accessType=1");
    mqtt_set_password(client, "46626de7dd8b12d9321ac1d2505d55d202cedd326ba7ee7729d3aa47b15aed7d");
    mqtt_set_client_id(client, "tuyalink_26bdc975b6bd846231nqxr");

    mqtt_set_channel(client, NETWORK_CHANNEL_TLS);
    mqtt_set_ca(client, NULL);

    mqtt_set_clean_session(client, 1);
    rc = mqtt_connect(client);
    sample_mqtt_tls_tuya_catstudio_printf("%s() mqtt_connect rc code:%d", __FUNCTION__, rc);
    OSATaskSleep(100);
    rc = mqtt_subscribe(client, "tylink/26bdc975b6bd846231nqxr/thing/property/report_response", QOS0, sub_topic_handle1);
    sample_mqtt_tls_tuya_catstudio_printf("%s() mqtt_subscribe rc code:%d", __FUNCTION__, rc);
    while (1)
    {
        rc = mqtt_tls_publish_handle(client);
        sample_mqtt_tls_tuya_catstudio_printf("%s() mqtt_tls_publish_handle rc code:%d", __FUNCTION__, rc);
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
        sample_mqtt_tls_tuya_catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();

        OSATaskSleep(200);
    }
}

static void _task(void *ptr)
{
    wait_network_ready();
    sample_mqtt_tls_tuya_catstudio_printf("\n\n\nSuccess in the net 2\n\n\n");
//    mqtt_sample_tcp();
//    mqtt_sample_tls_ca();
    mqtt_sample_tls_without_ca();
}
