/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file iot_api.h
 * @date 2020/01/07
 * @brief 设备接入侧统一功能接口定义。
 */

#ifndef __IOT_API_H__
#define __IOT_API_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*****************************************************************************/
    /* External Definition ( Constant and Macro )                                */
    /*****************************************************************************/

    /*****************************************************************************/
    /* External Structures, Enum and Typedefs                                    */
    /*****************************************************************************/
    /**
     * @brief 云端连接参数，可选TLS证书或DTLS PSK
     *
     */
    union network_cert_u
    {
        /** TLS证书缓冲区地址，CA证书形式*/
        const uint8_t* ca_cert;
        /** DTLS鉴权用PSK缓冲区地址*/
        const uint8_t* psk;
    };

    struct network_param_t
    {
        const uint8_t*       remote_addr;
        uint16_t             remote_port;
        uint16_t             cert_len;
        union network_cert_u cert_buf;
    };

    enum iot_proto_e
    {
        IOT_PROTO_EDP = 0,
        IOT_PROTO_MQTT,
        IOT_PROTO_MQTTS,
        IOT_PROTO_NBIOT
    };

    /**
     * @brief OneNET云端服务器连接参数
     *
     */
    struct iot_cloud_config_t
    {
        /** 服务器网络连接参数，设置为空时使用协议内置的默认服务器配置*/
        struct network_param_t* server_param;
        /** OneNET平台创建的产品ID*/
        uint8_t* product_id;
        /** 设备唯一识别码，产品内唯一。建议使用IMEI*/
        uint8_t* device_sn;
        /** 认证信息*/
        uint8_t* auth_code;
        /** 设备保活时间, 单位为s*/
        uint32_t life_time;
    };

    enum iot_dev_status_e
    {
        IOT_DEV_STATUS_DUMMY
    };

    struct iot_dev_info_t
    {
        uint8_t* device_id;
        uint8_t* api_key;
    };

    enum iot_data_type_e
    {
        IOT_DATA_TYPE_INT = 0, /**<整型数据，长度8字节 */
        IOT_DATA_TYPE_FLOAT,   /**<单精度浮点型数据 */
        IOT_DATA_TYPE_STRING,  /**<字符串型数据 */
        IOT_DATA_TYPE_BINARY,  /**<二进制数据 */
        IOT_DATA_TYPE_DUMMY = 0xFF	//0xFFFFFFFF
    };

    /**
     * @brief 数据缓冲区定义
     *
     */
    union iot_data_buf_u
    {
        /** 数据为int型时使用*/
        int64_t val_int;
        /** 数据为float型时使用*/
        float64_t val_float;
        /** 数据为string型时使用*/
        uint8_t* val_str;
        /** 数据为binary型时使用*/
        struct
        {
            uint32_t buf_len;
            uint8_t* buf;
        } val_bin;
    };

    /**
     * @brief 上传下发数据结构体定义
     *
     */
    struct iot_data_s
    {
        /** 数据标识，对应OneNET数据流或资源定义。*/
        const uint8_t* data_id;
        /** 数据时间戳，UNIX标准时间戳格式。上传时为空则平台自动以收到数据的时间为时间戳*/
        uint32_t timestamp;
        /** 数据类型*/
        enum iot_data_type_e data_type;
        /** 数据缓冲区*/
        union iot_data_buf_u buf;
    };

    struct iot_dev_cmd_t
    {
        /** 数据标识，当数据为下发命令时，取值为平台分配的cmd_id；当为数据推送时，取值为数据来源标识*/
        const uint8_t* data_id;
        /** 数据缓冲区地址*/
        uint8_t* data;
        /** 数据长度*/
        uint32_t data_len;
        /** 需要响应的数据*/
        uint8_t* resp;
        /** 响应数据的长度*/
        uint32_t resp_len;
    };

    enum iot_dev_event_e
    {
        /** 无参数*/
        IOT_DEV_REGISTER_OK = 0,
        /** 无参数*/
        IOT_DEV_REGISTER_FAILED,
        /** 无参数*/
        IOT_DEV_NET_CONNECTED,
        /** 无参数*/
        IOT_DEV_NET_DISCONNECT,
        /** 无参数*/
        IOT_DEV_CLOUD_CONNECTED,
        /** 无参数*/
        IOT_DEV_CLOUD_DISCONNECTED,
        /** 参见 struct iot_dev_cmd_t*/
        IOT_DEV_CMD_RECEIVED,
        /** 参见 struct iot_dev_cmd_t*/
        IOT_DEV_RESOURCE_PUSHED
    };

    typedef int32_t (*ont_dev_event_cb)(enum iot_dev_event_e /*event*/, void* /*data*/, uint32_t /*data_len*/);
    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/
    /**
     * @brief 打开一个OneNET设备。
     *
     * @param proto 指定登录平台需要使用的接入协议
     * @param cloud 配置设备使用的OneNET接入服务器及设备信息
     * @param event_handle 设置设备事件处理回调接口
     * @param timeout_ms 超时时间
     * @return void* 设备操作句柄
     */
    void* iot_dev_open(enum iot_proto_e proto, struct iot_cloud_config_t* cloud, ont_dev_event_cb event_handle, uint32_t timeout_ms);

    /**
     * @brief 获取设备信息。
     *
     * @param dev 设备操作句柄
     * @param dev_info 返回设备信息
     * @return int32_t
     */
    int32_t iot_dev_get_info(void* dev, struct iot_dev_info_t* dev_info);

    /**
     * @brief 关闭指定的OneNET设备
     *
     * @param dev 设备操作句柄
     * @param timeout_ms 超时时间
     * @retval 0 - 操作成功
     */
    int32_t iot_dev_close(void* dev, uint32_t timeout_ms);

    /**
     * @brief 主循环
     *
     * @param dev 设备操作句柄
     * @param timeout_ms 处理超时时间。为0时表示阻塞执行，PLAT_MULTI_TASK开启时有效
     * @return int32_t
     */
    int32_t iot_dev_step(void* dev, uint32_t timeout_ms);

    /**
     * @brief 推送原始数据
     *
     * @param dev 设备操作句柄
     * @param resource 指定数据的推送目标，MQTT协议时使用topic，EDP协议使用目标设备ID。为空时直接发往云端存储，功能与iot_dev_upload一致。
     * @param raw_data 需要发送的数据缓冲区
     * @param data_len 需要发送的数据长度
     * @return int32_t
     */
    int32_t iot_dev_push_rawdata(void* dev, const uint8_t* resource, void* raw_data, uint32_t data_len, uint32_t timeout_ms);

    /**
     * @brief 上传数据点到平台并存储
     *
     * @param dev 设备操作句柄
     * @param data 需要上传的数据点
     * @return int32_t
     */
    int32_t iot_dev_notify(void* dev, struct iot_data_s* data, uint32_t timeout_ms);

    /**
     * @brief 订阅指定资源的数据
     *
     * @param dev 设备操作句柄
     * @param resource 需要订阅的资源标识
     * @param timeout_ms 超时时间
     * @return int32_t
     */
    int32_t iot_dev_observe(void* dev, const uint8_t* resource, uint32_t timeout_ms);

    /**
     * @brief 取消已订阅的资源
     *
     * @param dev 设备操作句柄
     * @param resource 需要取消订阅的资源标识
     * @return int32_t
     */
    int32_t iot_dev_observe_cancel(void* dev, const uint8_t* resource);

#ifdef __cplusplus
}
#endif

#endif
