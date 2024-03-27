#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tuya_log.h"
#include "tuya_config.h"
#include "tuya_iot.h"
#include "cJSON.h"
#include "log.h"

#include "sdk_api.h"

#include "utils_common.h"

#define SOFTWARE_VER     "1.0.0"
 
/* Tuya device handle */
tuya_iot_client_t client;
 
#define SWITCH_DP_ID_KEY "101"

UINT8 connect_flg;

void example_qrcode_print(const char* productkey, const char* uuid)
{
    printf("https://smartapp.tuya.com/s/p?p=%s&uuid=%s&v=2.0", productkey, uuid);
    printf("(Use this URL to generate a static QR code for the Tuya APP scan code binding)");
}
 
/* Hardware switch control function */
void hardware_switch_set(bool value)
{
    if (value == true) {
        printf("Switch ON");
    } else {
        printf("Switch OFF");
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

int switch_demo_non_qrcode(void)
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

