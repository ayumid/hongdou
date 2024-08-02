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
#include "tuya_config.h"
#include "utils_common.h"

#include <assert.h>

#include "tuya_log.h"
#include "tuya_config.h"
#include "tuya_iot.h"
#include "cJSON.h"
#include "log.h"
#include "am_gpio.h"

// Private defines / typedefs ---------------------------------------------------

#define SOFTWARE_VER     "1.0.0"
#define SWITCH_DP_ID_KEY "101"

#define TUYA_TASK_STACK_SIZE 1024 * 16

// Private variables ------------------------------------------------------------

static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;
static void* _send_task_stack = NULL;
static OSTaskRef _send_task_ref = NULL;

static void tuya_connectkit_task(void *ptr);
static void tuya_send_task(void *ptr);

OSMsgQRef    send_msgq;

/* Tuya device handle */
tuya_iot_client_t client;
 
UINT8 connect_flg;
extern UINT8 MoudleLedStatus;
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

int tuya_get_csq(void)
{
    char atresbuf[16] = {0};
    int err = 0;
    char *p = NULL;
    int csqValue = 99;

    memset(atresbuf, 0, sizeof(atresbuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 2, "+CSQ", 1, NULL, atresbuf, sizeof(atresbuf));

    if(strlen(atresbuf) >= 10 )
    {
        p = strchr(atresbuf,':');
        csqValue = atoi(p + 1);
    }

    return csqValue;
}

void tuya_get_imei(char *buffer)
{
    char imei[24] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(imei, 0, sizeof(imei));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CGSN\r\n", 2, NULL, 1, NULL, imei, sizeof(imei));

    if(strlen(imei) >= 15 )
    {
        snprintf(buffer, 16, imei);
        return;
    }
}

void tuya_get_iccid(char *buffer)
{
    char iccid[36] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(iccid, 0, sizeof(iccid));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+ICCID\r\n", 2, "+ICCID", 1, NULL, iccid, sizeof(iccid));

    if(strstr(iccid, "+ICCID") != NULL )
    {
        p = strchr(iccid,':');
        snprintf(buffer, 21, p + 2);
        return;
    }
}
static UINT8 flag;

static void tuya_send_task(void *ptr)
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
                    if(flag == 0)
                    {
                        flag = 1;
                        GpioSetLevel(GPIO_CLR_PIN0, 1);
                        snprintf(dp_muti_buf, 128, "{\"101\":true}");
                    }
                    else
                    {
                        flag = 0;
                        GpioSetLevel(GPIO_CLR_PIN0, 0);
                        snprintf(dp_muti_buf, 128, "{\"101\":false}");
                    }
                    
                    tuya_send_multiple_value_data(dp_muti_buf);
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

void example_qrcode_print(const char* productkey, const char* uuid)
{
    printf("https://smartapp.tuya.com/s/p?p=%s&uuid=%s&v=2.0", productkey, uuid);
    printf("(Use this URL to generate a static QR code for the Tuya APP scan code binding)");
}
 
/* Hardware switch control function */
void hardware_switch_set(bool value)
{
    if (value == true)
    {
        printf("Switch ON");
        flag = 1;
        GpioSetLevel(GPIO_CLR_PIN0, 1);
    }
    else
    {
        printf("Switch OFF");
        flag = 0;
        GpioSetLevel(GPIO_CLR_PIN0, 0);
    }
}
 
/* DP data reception processing function */
void tuya_iot_dp_download(tuya_iot_client_t* client, const char* json_dps)
{
    printf("Data point download value:%s", json_dps);
 
    /* Parsing json string to cJSON object */
    cJSON* dps = cJSON_Parse(json_dps);
    if (dps == NULL) {
        printf("JSON parsing error, exit!");
        return;
    }
 
    /* Process dp data */
    //SWITCH_DP_ID_KEY 表示json_dps中的对象，例如{"101":true} 中的"101"
    cJSON* switch_obj = cJSON_GetObjectItem(dps, SWITCH_DP_ID_KEY);
    if (cJSON_IsTrue(switch_obj)) {
        hardware_switch_set(true);
 
    } else if (cJSON_IsFalse(switch_obj)) {
        hardware_switch_set(false);
    }
 
    /* relese cJSON DPS object */
    cJSON_Delete(dps);
 
    /* Report the received data to synchronize the switch status. */
    tuya_iot_dp_report_json(client, json_dps);
}
 
/* Tuya SDK event callback */
static void user_event_handler_on(tuya_iot_client_t* client, tuya_event_msg_t* event)
{
    switch(event->id){
    case TUYA_EVENT_BIND_START:
        example_qrcode_print(client->config.productkey, client->config.uuid);
        break;
 
    case TUYA_EVENT_MQTT_CONNECTED:
        printf("Device MQTT Connected!");
        connect_flg = 1;
        break;

    case TUYA_EVENT_DP_RECEIVE:
        tuya_iot_dp_download(client, (const char*)event->value.asString);
        break;
 
    default:
        break;
    }
}

int tuya_iot_task(void)
{
    int ret = OPRT_OK;
 
    /* Initialize Tuya device configuration */
    ret = tuya_iot_init(&client, &(const tuya_iot_config_t){
        .software_ver = SOFTWARE_VER,
        .productkey = TUYA_PRODUCT_KEY,
        .uuid = TUYA_DEVICE_UUID,
        .authkey = TUYA_DEVICE_AUTHKEY,
        .event_handler = user_event_handler_on
    });
 
    assert(ret == OPRT_OK);
 
    /* Start tuya iot task */
    tuya_iot_start(&client);
 
    for(;;) {
        /* Loop to receive packets, and handles client keepalive */
        tuya_iot_yield(&client);
    }
}

int tuya_send_multiple_value_data(char* buf)
{
    int ret = OPRT_OK;

    //等待mqtt连接
    while(!connect_flg)
    {
        sleep(1);
    }
    MoudleLedStatus = NORMAL;
    printf("start report");
    tuya_iot_dp_report_json(&client, buf);

    return ret;
}

int tuya_send_csq_data(char* buf)
{
    int ret = OPRT_OK;
 
    tuya_iot_dp_report_json(&client, buf);

    return ret;
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
    
    _task_stack = malloc(TUYA_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);
    /*creat message*/
    status = OSAMsgQCreate(&send_msgq, "send_msgq", SEND_MSGQ_TASK_MSGQ_MSG_SIZE, SEND_MSGQ_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, TUYA_TASK_STACK_SIZE, 100, "start_kit_task", tuya_connectkit_task, NULL);
    ASSERT(ret == OS_SUCCESS);
    
    _send_task_stack = malloc(TUYA_TASK_STACK_SIZE);
    ASSERT(_send_task_stack != NULL);

    ret = OSATaskCreate(&_send_task_ref, _send_task_stack, TUYA_TASK_STACK_SIZE, 100, "send_task", tuya_send_task, NULL);
    ASSERT(ret == OS_SUCCESS);

}

static void wait_network_ready(void)
{
    int count = 0;
    int ready = 0;
    MoudleLedStatus = NOGPRS;

    while (!ready)
    {
        if (getCeregReady(isMasterSim0() ? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
        {
            ready = 1;
        }
        printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();

        OSATaskSleep(200);
    }
}

static void tuya_connectkit_task(void *ptr)
{
    int ret = 0;
    
    wait_network_ready();
    printf("\n\n\nSuccess in the net 2\n\n\n");
    MoudleLedStatus = NOSERVER;
//    cellular_demo();
    tuya_iot_task();
}

// End of file : main.c 2024-3-7 9:03:03 by: zhaoning 

