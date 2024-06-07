/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file iot_mqtts.h
 * @date 2020/01/07
 * @brief
 */

#ifndef __IOT_MQTTS_H__
#define __IOT_MQTTS_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "iot_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*****************************************************************************/
    /* External Definition ( Constant and Macro )                                 */
    /*****************************************************************************/

    /*****************************************************************************/
    /* External Structures, Enum and Typedefs                                    */
    /*****************************************************************************/

    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/

    typedef struct iot_mqtts_s ont_mqtt_t;

    ///@brief 使用mqtt登录OneNET平台
    ///@param ont_cfg OneNET登录相关参数
    ///@param event_cb 事件回调函数
    ///@param timeout_ms 超时
    ///@return ont_mqtt_t句柄
    ont_mqtt_t* iot_mqtts_login(struct iot_cloud_config_t* ont_cfg, ont_dev_event_cb event_cb, uint32_t timeout_ms);

    ///@brief OneNET mqtt事件处理主进程
    ///@param ont_mqtt ont_mqtt_t句柄
    ///@param timeout_ms 超时
    ///@return 0:成功;Others:失败
    int32_t iot_mqtts_step(ont_mqtt_t* ont_mqtt, uint32_t timeout_ms);

    ///@brief OneNET mqtt推送数据
    ///@param ont_mqtt ont_mqtt_t句柄
    ///@param target_topic 目的topic
    ///@param raw_data 数据地址
    ///@param data_len 数据长度
    ///@param timeout_ms 超时
    ///@return 0:成功;Others:失败
    int32_t iot_mqtts_push_rawdata(ont_mqtt_t* ont_mqtt, const uint8_t* target_topic, void* raw_data, uint32_t data_len, uint32_t timeout_ms);

    ///@brief 向OneNET平台报告
    ///@param ont_mqtt ont_mqtt_t句柄
    ///@param data iot_data_s封装的数据
    ///@param timeout_ms 超时
    ///@return 0:成功;Others:失败
    int32_t iot_mqtts_notify(ont_mqtt_t* ont_mqtt, struct iot_data_s* data, uint32_t timeout_ms);

    ///@brief 退出OneNET
    ///@param ont_mqtt ont_mqtt_t句柄
    ///@param timeout_ms 超时
    ///@return 0:成功;Others:失败
    int32_t iot_mqtts_logout(ont_mqtt_t* ont_mqtt, uint32_t timeout_ms);

    ///@brief 订阅
    ///@param ont_mqtt ont_mqtt_t句柄
    ///@param topic topic
    ///@param timeout_ms 超时
    ///@return 0:成功;Others:失败
    int32_t iot_mqtts_observe(ont_mqtt_t* ont_mqtt, const uint8_t* topic, uint32_t timeout_ms);

    ///@brief 取消订阅
    ///@param ont_mqtt ont_mqtt_t句柄
    ///@param topic topic
    ///@param timeout_ms 超时
    ///@return 0:成功;Others:失败
    int32_t iot_mqtts_observe_cancel(ont_mqtt_t* ont_mqtt, const uint8_t* topic, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
