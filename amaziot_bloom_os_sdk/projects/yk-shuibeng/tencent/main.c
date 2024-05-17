//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2024-3-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-3-7
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

#include <signal.h>
#include <stdbool.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "service_mqtt.h"
#include "utils_getopt.h"
#include "lite-utils.h"
#include "utils_common.h"
#include "am_gpio.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_mqtt_tencent_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_mqtt_tencent_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define tencent_sleep(x) OSATaskSleep((x)*200) // second
#define TENCENT_TASK_STACK_SIZE 1024 * 16

// Private variables ------------------------------------------------------------

static UINT8 tflag;
static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;
static void* _send_task_stack = NULL;
static OSTaskRef _send_task_ref = NULL;

static void tencent_iot_explorer_task(void *ptr);
static void tencent_iot_explorer_send_task(void *ptr);

typedef struct {
    bool     power_off;
    uint8_t  brightness;
    uint16_t color;
    char     device_name[MAX_SIZE_OF_DEVICE_NAME + 1];
} LedInfo;

static DeviceInfo sg_devInfo;
static LedInfo    sg_led_info;
OSMsgQRef    send_msgq;
extern UINT8 MoudleLedStatus;

// led attributes, corresponding to struct LedInfo
static char *sg_property_name[] = {"power_switch", "brightness", "color", "name"};

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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

// user's log print callback
static bool log_handler(const char *message)
{
    // return true if print success
    return false;
}

// MQTT event callback
static void _mqtt_event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg)
{
    MQTTMessage *mqtt_messge = (MQTTMessage *)msg->msg;
    uintptr_t    packet_id   = (uintptr_t)msg->msg;

    switch (msg->event_type) {
        case MQTT_EVENT_UNDEF:
            Log_i("undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            Log_i("MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            Log_i("MQTT reconnect.");
            break;

        case MQTT_EVENT_PUBLISH_RECVEIVED:
            Log_i(
                "topic message arrived but without any related handle: topic=%.*s, "
                "topic_msg=%.*s",
                mqtt_messge->topic_len, mqtt_messge->ptopic, mqtt_messge->payload_len, mqtt_messge->payload);
            break;
        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            Log_i("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_NACK:
            Log_i("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;
        default:
            Log_i("Should NOT arrive here.");
            break;
    }
}

// Setup MQTT construct parameters
static int _setup_connect_init_params(MQTTInitParams *initParams)
{
    int ret;

    ret = HAL_GetDevInfo((void *)&sg_devInfo);
    if (QCLOUD_RET_SUCCESS != ret) {
        return ret;
    }

    initParams->region      = sg_devInfo.region;
    initParams->device_name = sg_devInfo.device_name;
    initParams->product_id  = sg_devInfo.product_id;

#ifdef AUTH_MODE_CERT
    char  certs_dir[PATH_MAX + 1] = "certs";
    char  current_path[PATH_MAX + 1];
    char *cwd = getcwd(current_path, sizeof(current_path));

    if (cwd == NULL) {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }

#ifdef WIN32
    sprintf(sg_cert_file, "%s\\%s\\%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
    sprintf(sg_key_file, "%s\\%s\\%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);
#else
    sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
    sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);
#endif

    initParams->cert_file = sg_cert_file;
    initParams->key_file  = sg_key_file;
#else
    initParams->device_secret = sg_devInfo.device_secret;
#endif

    initParams->command_timeout        = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

    initParams->auto_connect_enable  = 1;
    initParams->event_handle.h_fp    = _mqtt_event_handler;
    initParams->event_handle.context = NULL;

    return QCLOUD_RET_SUCCESS;
}

static void property_topic_publish(void *pClient, const char *message, int message_len)
{
    char topic[256] = {0};
    int  size;

    size = HAL_Snprintf(topic, 256, "$thing/up/property/%s/%s", sg_devInfo.product_id, sg_devInfo.device_name);

    if (size < 0 || size > 256 - 1) {
        Log_e("buf size < topic length!");
        return;
    }

    PublishParams pubParams = DEFAULT_PUB_PARAMS;
    pubParams.qos           = QOS0;
    pubParams.payload_len   = message_len;
    pubParams.payload       = (void *)message;

    IOT_MQTT_Publish(pClient, topic, &pubParams);
}

static void property_get_status(void *pClient)
{
    char       message[256]        = {0};
    int        message_len         = 0;
    static int sg_get_status_index = 0;

    sg_get_status_index++;
    message_len = HAL_Snprintf(message, sizeof(message), "{\"method\":\"get_status\", \"clientToken\":\"%s-%d\"}",
                               sg_devInfo.product_id, sg_get_status_index);
    property_topic_publish(pClient, message, message_len);
}

static void yk_shuibeng_property_report(void *pClient, UINT8 flag)
{
    char       message[256]    = {0};
    int        message_len     = 0;
    static int sg_report_index = 0;

    sg_report_index++;
    message_len = HAL_Snprintf(message, sizeof(message),
                               "{\"method\":\"report\", \"clientToken\":\"%s-%d\", "
                               "\"params\":{\"power_switch\":%d, \"color\":%d, \"brightness\":%d, \"name\":\"%s\"}}",
                               sg_devInfo.product_id, sg_report_index, flag, sg_led_info.color,
                               sg_led_info.brightness, sg_devInfo.device_name);
    property_topic_publish(pClient, message, message_len);
}

static void property_control_handle(void *pClient, const char *token, const char *control_data)
{
    char *params         = NULL;
    char *property_param = NULL;
    char  message[256]   = {0};
    int   message_len    = 0;
    int i = 0;
    
    params = LITE_json_value_of("params", (char *)control_data);
    if (NULL == params) {
        Log_e("Fail to parse params");
        return;
    }

    for (i = 0; i < sizeof(sg_property_name) / sizeof(sg_property_name[0]); i++) {
        property_param = LITE_json_value_of(sg_property_name[i], params);
        if (NULL != property_param) {
            Log_i("\t%-16s = %-10s", sg_property_name[i], property_param);
            //这里的i代表sg_property_name里序号
            if(i == 0)
            {
                sg_led_info.power_off = atoi(property_param);
                if (sg_led_info.power_off == true)
                {
                    printf("Switch ON");
                    tflag = 1;
                    GpioSetLevel(GPIO_CLR_PIN0, 1);
                }
                else
                {
                    printf("Switch OFF");
                    tflag = 0;
                    GpioSetLevel(GPIO_CLR_PIN0, 0);
                }
            }
            else if(i == 1)
            {
                // only change the brightness in the demo
                sg_led_info.brightness = atoi(property_param);
            }
            else if(i == 2)
            {
                // only change the brightness in the demo
                sg_led_info.color = atoi(property_param);
            }
            HAL_Free(property_param);
        }
    }

    // method: control_reply
    message_len = HAL_Snprintf(message, sizeof(message),
                               "{\"method\":\"control_reply\", \"code\":0, \"clientToken\":\"%s\"}", token);
    property_topic_publish(pClient, message, message_len);

    HAL_Free(params);
}

static void property_get_status_reply_handle(const char *get_status_reply_data)
{
    char *data           = NULL;
    char *report         = NULL;
    char *property_param = NULL;
    int i = 0;
    
    data = LITE_json_value_of("data", (char *)get_status_reply_data);
    if (NULL == data) {
        Log_e("Fail to parse data");
        return;
    }
    report = LITE_json_value_of("reported", (char *)data);
    if (NULL == report) {
        Log_e("Fail to parse report");
        HAL_Free(data);
        return;
    }

    for (i = 0; i < sizeof(sg_property_name) / sizeof(sg_property_name[0]); i++) {
        property_param = LITE_json_value_of(sg_property_name[i], report);
        if (NULL != property_param) {
            Log_i("\t%-16s = %-10s", sg_property_name[i], property_param);
            if (i == 1) {
                sg_led_info.brightness = atoi(property_param);
            }
            HAL_Free(property_param);
        }
    }

    HAL_Free(report);
    HAL_Free(data);
}

static void property_report_reply_handle(const char *report_reply_data)
{
    char *status = NULL;

    status = LITE_json_value_of("status", (char *)report_reply_data);
    if (NULL == status) {
        Log_e("Fail to parse data");
        return;
    }
    Log_i("report reply status: %s", status);
    HAL_Free(status);
}

void unbind_device_callback(void *pContext, const char *msg, uint32_t msgLen)
{
    Log_i("unbind device.");
}

// callback when MQTT msg arrives
static void property_message_callback(void *pClient, MQTTMessage *message, void *userData)
{
    char  property_data_buf[QCLOUD_IOT_MQTT_RX_BUF_LEN + 1] = {0};
    int   property_data_len                                 = 0;
    char *type_str                                          = NULL;
    char *token_str                                         = NULL;

    if (message == NULL) {
        return;
    }
    Log_i("Receive Message With topicName:%.*s, payload:%.*s", (int)message->topic_len, message->ptopic,
          (int)message->payload_len, (char *)message->payload);
    cprintf("rcv:%s",(char *)message->payload);
    cprintf("rcv:%s",(char *)message->payload + 150);
    property_data_len =
        sizeof(property_data_buf) > message->payload_len ? message->payload_len : QCLOUD_IOT_MQTT_RX_BUF_LEN;
    memcpy(property_data_buf, message->payload, property_data_len);

    type_str = LITE_json_value_of("method", property_data_buf);
    if (NULL == type_str) {
        Log_e("Fail to parse method");
        return;
    }
    token_str = LITE_json_value_of("clientToken", property_data_buf);
    if (NULL == type_str) {
        Log_e("Fail to parse token");
        HAL_Free(type_str);
        return;
    }

    if (0 == strncmp(type_str, "control", sizeof("control") - 1)) {
        // method: control
        property_control_handle(pClient, token_str, property_data_buf);
    } else if (0 == strncmp(type_str, "get_status_reply", sizeof("get_status_reply") - 1)) {
        // method: get_status_reply
        property_get_status_reply_handle(property_data_buf);
    } else if (0 == strncmp(type_str, "report_reply", sizeof("report_reply") - 1)) {
        // method: report_reply
        property_report_reply_handle(property_data_buf);
    } else {
        // do nothing
    }

    HAL_Free(token_str);
    HAL_Free(type_str);
}

// subscribe MQTT topic and wait for sub result
static int subscribe_property_topic_wait_result(void *client)
{
    char topic_name[128] = {0};

    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "$thing/down/property/%s/%s", sg_devInfo.product_id,
                            sg_devInfo.device_name);
    if (size < 0 || size > sizeof(topic_name) - 1) {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }

    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.qos                = QOS0;
    sub_params.on_message_handler = property_message_callback;

    int rc = IOT_MQTT_Subscribe(client, topic_name, &sub_params);
    if (rc < 0) {
        Log_e("MQTT subscribe FAILED: %d", rc);
        return rc;
    }

    int wait_cnt = 10;
    while (!IOT_MQTT_IsSubReady(client, topic_name) && (wait_cnt > 0)) {
        // wait for subscription result
        rc = IOT_MQTT_Yield(client, 1000);
        if (rc) {
            Log_e("MQTT error: %d", rc);
            return rc;
        }
        wait_cnt--;
    }

    if (wait_cnt > 0) {
        return QCLOUD_RET_SUCCESS;
    } else {
        Log_e("wait for subscribe result timeout!");
        return QCLOUD_ERR_FAILURE;
    }
}

#ifdef LOG_UPLOAD
// init log upload module
static int _init_log_upload(MQTTInitParams *init_params)
{
    LogUploadInitParams log_init_params;
    memset(&log_init_params, 0, sizeof(LogUploadInitParams));

    log_init_params.region      = init_params->region;
    log_init_params.product_id  = init_params->product_id;
    log_init_params.device_name = init_params->device_name;
#ifdef AUTH_MODE_CERT
    log_init_params.sign_key = init_params->cert_file;
#else
    log_init_params.sign_key = init_params->device_secret;
#endif

#if defined(__linux__) || defined(WIN32)
    log_init_params.read_func     = HAL_Log_Read;
    log_init_params.save_func     = HAL_Log_Save;
    log_init_params.del_func      = HAL_Log_Del;
    log_init_params.get_size_func = HAL_Log_Get_Size;
#endif

    return IOT_Log_Init_Uploader(&log_init_params);
}

#endif

static bool sg_loop_test = false;
static int  parse_arguments(int argc, char **argv)
{
    int c;
    while ((c = utils_getopt(argc, argv, "c:l")) != EOF) switch (c) {
            case 'c':
                if (HAL_SetDevInfoFile(utils_optarg))
                    return -1;
                break;

            case 'l':
                sg_loop_test = true;
                break;

            default:
                HAL_Printf(
                    "usage: %s [options]\n"
                    "  [-c <config file for DeviceInfo>] \n"
                    "  [-l ] loop test or not\n",
                    argv[0]);
                return -1;
        }
    return 0;
}
void *client;

int tencent_iot_task(void)
{
    int rc;
    // init log level
    IOT_Log_Set_Level(eLOG_DEBUG);
    IOT_Log_Set_MessageHandler(log_handler);

    // parse arguments for device info file and loop test;
//    rc = parse_arguments(argc, argv);
//    if (rc != QCLOUD_RET_SUCCESS) {
//        Log_e("parse arguments error, rc = %d", rc);
//        return rc;
//    }
    sg_loop_test = true;
    // init connection
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc                         = _setup_connect_init_params(&init_params);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("init params error, rc = %d", rc);
        return rc;
    }

#ifdef LOG_UPLOAD
    // _init_log_upload should be done after _setup_connect_init_params and before IOT_MQTT_Construct
    rc = _init_log_upload(&init_params);
    if (rc != QCLOUD_RET_SUCCESS)
        Log_e("init log upload error, rc = %d", rc);
#endif

    // create MQTT client and connect with server
    client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        rc = IOT_MQTT_GetErrCode();
        Log_e("MQTT Construct failed, rc = %d", rc);
        return QCLOUD_ERR_FAILURE;
    }

#ifdef SYSTEM_COMM
    long time = 0;
    // get system timestamp from server
    rc = IOT_Get_Sys_Resource(client, eRESOURCE_TIME, &sg_devInfo, &time);
    if (QCLOUD_RET_SUCCESS == rc) {
        Log_i("system time is %ld", time);
    } else {
        Log_e("get system time failed!");
    }
#endif

    // subscribe normal topics here
    rc = subscribe_property_topic_wait_result(client);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        return rc;
    }
    // when platform unbind this device. the callback function will run
    IOT_Unbind_Device_ByCloud(client, unbind_device_callback, NULL);
    // method: get_status
    property_get_status(client);
    do {
        rc = IOT_MQTT_Yield(client, 500);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            if (rc == QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT) {
                Log_e(
                    "exit. mqtt reconnect timeout! Please check the network connection, or try to increase "
                    "MAX_RECONNECT_WAIT_INTERVAL(%d)",
                    MAX_RECONNECT_WAIT_INTERVAL);
            } else {
                Log_e("exit with error: %d", rc);
            }
            cprintf("111rc: %d", rc);
            break;
        }

        if (sg_loop_test)
            HAL_SleepMs(1000);

        // method: report
//        yk_shuibeng_property_report(client)/;
    } while (sg_loop_test);
    cprintf("222rc: %d", rc);
    rc = IOT_Unbind_Device_Request(client, 5000);
    if (rc != QCLOUD_RET_SUCCESS) {
        Log_e("unbind device request error.");
    }
    rc = IOT_MQTT_Destroy(&client);
    IOT_Log_Upload(true);

#ifdef LOG_UPLOAD
    IOT_Log_Fini_Uploader();
#endif
    return rc;
}

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
    OSA_STATUS status = 0;
    
    _task_stack = malloc(TENCENT_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);

    /*creat message*/
    status = OSAMsgQCreate(&send_msgq, "send_msgq", SEND_MSGQ_TASK_MSGQ_MSG_SIZE, SEND_MSGQ_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);
    ret = OSATaskCreate(&_task_ref, _task_stack, TENCENT_TASK_STACK_SIZE, 100, "iot_explorer_task", tencent_iot_explorer_task, NULL);
    ASSERT(ret == OS_SUCCESS);
//    
    _send_task_stack = malloc(TENCENT_TASK_STACK_SIZE);
    ASSERT(_send_task_stack != NULL);

    ret = OSATaskCreate(&_send_task_ref, _send_task_stack, TENCENT_TASK_STACK_SIZE, 100, "send_task", tencent_iot_explorer_send_task, NULL);
    ASSERT(ret == OS_SUCCESS);

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
        sample_mqtt_tencent_catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();

        OSATaskSleep(200);
    }
}

static void tencent_iot_explorer_task(void *ptr)
{
    int ret = 0;
    
    wait_network_ready();
    sample_mqtt_tencent_catstudio_printf("\n\n\nSuccess in the net 2\n\n\n");
    
    tencent_iot_task();
}

static void tencent_iot_explorer_send_task(void *ptr)
{
    OS_STATUS  status;
    app_msgq_msg sdkMsg = {0};
    char dp_muti_buf[128] = {0};
    char dp_buf[16] = {0};

    //"{\"103\":\"866663060145165\",\"104\":\"898604A41921C0467222\"}"
//    tuya_get_imei(imei);
//    tuya_get_iccid(iccid);
//    snprintf(dp_muti_buf, 128, "{\"103\":\"%s\",\"104\":\"%s\"}", imei, iccid);
//    tuya_send_multiple_value_data(dp_muti_buf);
    //init led sattus
    init_led_status();
    init_gpio_conf();

    while(1)
    {
        status = OSAMsgQRecv(send_msgq, (void *)&sdkMsg, SEND_MSGQ_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
//            printf("%s, msgid=%d", __FUNCTION__, sdkMsg.msgId);
            switch(sdkMsg.msgId)
            {
//                case SEND_TASK_MSG_HB_MSG:
//                    heart_thread();
//                break;
//                case SEND_TASK_MSG_RSSI_MSG:
//                    rssi_thread();
//                break;
//                case SEND_TASK_MSG_RESEND_MSG:
//                    resend_thread();
//                break;
//                case SEND_TASK_MSG_CLOCKUPDATA_MSG:
//                    clockupdate_thread();
//                break;
                case SEND_TASK_MSG_GPIO_MSG:
                    if(tflag == 0)
                    {
                        tflag = 1;
                        GpioSetLevel(GPIO_CLR_PIN0, 1);
                    }
                    else
                    {
                        tflag = 0;
                        GpioSetLevel(GPIO_CLR_PIN0, 0);
                    }
                    
                    yk_shuibeng_property_report(client, tflag);
                break;
//                case SEND_TASK_MSG_ALARM_MSG:
//                    alarm_thread();
//                break;
                default:
                break;
            }
        }
    }
}

// End of file : main.c 2024-3-7 9:03:03 by: zhaoning 

