/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file main.c
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "iot_api.h"
#include "common/log.h"
#include "plat_osl.h"
#include "plat_time.h"

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/
/** 心跳时间*/
#define CN_LIFE_TIME_MQTTS 120

/** OneNET平台产品ID*/
#define CN_PRODUCT_ID_MQTTS "nl30j6Z9o9"

/** 设备唯一识别码，产品内唯一*/
#define CN_DEVICE_NAME_MQTTS "ASR1602_001"

/** 认证信息--设备key*/
#define CN_ACCESS_KEY_MQTTS "OFY3bnVNVG90dlpqdUJPOVQ0ajVrQU1TbGlXQ2x4enc="

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/

/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/
int32_t iot_mqtts_cb_func(enum iot_dev_event_e event, void* data, uint32_t data_len);

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/
struct iot_data_s data_notify_list[] = { { .data_id = (const uint8_t*)"data_stream_demo", .data_type = IOT_DATA_TYPE_INT, .buf.val_int = 0 } };
int32_t           data_notify_num    = sizeof(data_notify_list) / sizeof(struct iot_data_s);
/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
int32_t iot_mqtts_cb_func(enum iot_dev_event_e event, void* data, uint32_t data_len)
{
    struct iot_dev_cmd_t* cmd_data_ptr = (struct iot_dev_cmd_t*)data;

    switch (event) {
        case IOT_DEV_CMD_RECEIVED: {
            /** 这里回环回去，测试下发命令是否成功*/
            osl_memcpy(cmd_data_ptr->resp, cmd_data_ptr->data, cmd_data_ptr->data_len);
            cmd_data_ptr->resp_len = cmd_data_ptr->data_len;
            logd("mqtts cmd arrived!");
            break;
        }
        case IOT_DEV_RESOURCE_PUSHED: {
            logd("mqtts other topic cmd received!");
            break;
        }
        default:
            break;
    }
    return 0;
}

//int main(int arg, char* argv[])
int onenet_connet(void)
{
    void* iot_dev_ptr = NULL;

    int32_t             loop;
    handle_t            timer_handle = 0;
    uint32_t            timer_period = 2;
    struct iot_cloud_config_t mqtts_config = {
        .product_id = (uint8_t*)CN_PRODUCT_ID_MQTTS,
        .device_sn  = (uint8_t*)CN_DEVICE_NAME_MQTTS,
        .auth_code  = (uint8_t*)CN_ACCESS_KEY_MQTTS,
        .life_time  = CN_LIFE_TIME_MQTTS,
    };

    /** 打开设备*/
    iot_dev_ptr = iot_dev_open(IOT_PROTO_MQTTS, &mqtts_config, &iot_mqtts_cb_func, 60000);

    if (iot_dev_ptr != NULL) {
        logd("mqtts iot dev open sucessed!");
    } else {
        logd("mqtts iot dev open failed");
        goto exit;
    }

    /** 设备处理主循环*/
    while (1) {
        /** 循环处理，包括心跳*/
        if (0 != iot_dev_step(iot_dev_ptr, 2000)) {
            break;
        }
		
        #if 0
        /** 定时上报数据示例*/
        if (timer_handle == 0) {
            timer_handle = countdown_start(timer_period * 1000);
        } else if (countdown_is_expired(timer_handle) == 1) {
            /** 上报数据到OneNET*/
            for (loop = 0; loop < data_notify_num; loop++) {
                data_notify_list[loop].buf.val_int += 2;
                if (0 == iot_dev_notify(iot_dev_ptr, &data_notify_list[loop], 10000)) {
                    logd("mqtts notify ok");
                }
            }

            countdown_set(timer_handle, timer_period * 1000);
        }
        #endif
		
        time_delay_ms(50);
    }

exit:
    /** 关闭设备*/
    iot_dev_close(iot_dev_ptr, 3000);
    logd("mqtts test device closed!");

    return 0;
}
