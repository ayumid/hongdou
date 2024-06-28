/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file tm_api.h
 * @brief
 */

#ifndef __TM_API_H__
#define __TM_API_H__

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
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef ADD_MSG2PAYLOAD
#define ADD_MSG2PAYLOAD(_payload, _msg)                                                                                                                        \
    do {                                                                                                                                                       \
        int _payload_len = osl_strlen(_payload);                                                                                                               \
        int _msg_len     = osl_strlen(_msg);                                                                                                                   \
        if (CONFIG_PACKET_PAYLOAD_LEN_MAX > _payload_len + _msg_len) {                                                                                         \
            osl_strcat(_payload, _msg);                                                                                                                        \
        } else {                                                                                                                                               \
            loge(                                                                                                                                              \
                "payload length(%d) more than the "                                                                                                            \
                "CONFIG_PACKET_PAYLOAD_LEN_MAX(%d)",                                                                                                           \
                _payload_len + _msg_len,                                                                                                                       \
                CONFIG_PACKET_PAYLOAD_LEN_MAX);                                                                                                                \
        }                                                                                                                                                      \
    } while (0)
#endif

#define TM_PROPERTY_RW(x)                                                                                                                                      \
    {                                                                                                                                                          \
#x, tm_prop_##x##_rd_cb, tm_prop_##x##_wr_cb                                                                                                           \
    }

#define TM_PROPERTY_RO(x)                                                                                                                                      \
    {                                                                                                                                                          \
#x, tm_prop_##x##_rd_cb, NULL                                                                                                                          \
    }

#define TM_SERVICE(x)                                                                                                                                          \
    {                                                                                                                                                          \
#x, tm_svc_##x##_cb                                                                                                                                    \
    }

    /*****************************************************************************/
    /* External Structures, Enum and Typedefs                                    */
    /*****************************************************************************/
    typedef int32_t (*tm_prop_read_cb)(void* res);
    typedef int32_t (*tm_prop_write_cb)(void* res);
    typedef int32_t (*tm_event_read_cb)(void* res);
    typedef int32_t (*tm_svc_invoke_cb)(void* in, void* out);

#ifdef CONFIG_GENERAL_OTA
    typedef int32_t (*tm_ota_cb)(void);
#endif

    struct tm_prop_tbl_t
    {
        const uint8_t*   name;
        tm_prop_read_cb  tm_prop_rd_cb;
        tm_prop_write_cb tm_prop_wr_cb;
    };

    struct tm_svc_tbl_t
    {
        const uint8_t*   name;
        tm_svc_invoke_cb tm_svc_cb;
    };

    struct tm_downlink_tbl_t
    {
        struct tm_prop_tbl_t* prop_tbl;
        struct tm_svc_tbl_t*  svc_tbl;
        uint16_t              prop_tbl_size;
        uint16_t              svc_tbl_size;
#ifdef CONFIG_GENERAL_OTA
        tm_ota_cb ota_cb;
#endif
    };
    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/
    /**
     * @brief 设备初始化
     *
     * @param downlink_tbl 下行数据处理回调接口表，包含属性和服务的处理接口
     * @return 0 - 成功；-1 - 失败
     */
    int32_t tm_init(struct tm_downlink_tbl_t* downlink_tbl);
    int32_t tm_deinit(void);

    /**
     * @brief 向平台发起设备登录请求
     *
     * @param product_id 产品ID
     * @param dev_name 设备名称
     * @param access_key 产品key或设备key
     * @param timeout_ms 登录超时时间
     * @return 0 - 登录成功；-1 - 失败
     */
    int32_t tm_login(const uint8_t* product_id, const uint8_t* dev_name, const uint8_t* access_key, uint64_t expire_time, uint32_t timeout_ms);

    /**
     * @brief 设备登出
     *
     * @param timeout_ms 超时时间
     * @return 0 - 登出成功；-1 - 失败
     */
    int32_t tm_logout(uint32_t timeout_ms);

    int32_t tm_post_property(void* prop_data, uint32_t timeout_ms);
    int32_t tm_post_event(void* event_data, uint32_t timeout_ms);
    int32_t tm_get_desired_props(uint32_t timeout_ms);
    int32_t tm_delete_desired_props(uint32_t timeout_ms);

    /**
     * @brief 打包设备的属性和事件数据，可用于子设备
     *
     * @param data 需要打包的目标指针地址，用于后续调用上报接口。设置为空时由接口内部分配空间，并通过返回值返回地址
     * @param product_id 需要打包数据的产品id
     * @param dev_name 需要打包数据的设备名称
     * @param prop
     * 传入属性数据。支持json格式(as_raw为1)，也可以仿照tm_user文件的数据上传接口使用tm_data接口构造数据（as_raw为0）
     * @param event 定义同prop，用于传入事件数据
     * @param as_raw 是否为原始json格式的数据
     * @return 打包后的数据指针地址
     */
    void*   tm_pack_device_data(void* data, const uint8_t* product_id, const uint8_t* dev_name, void* prop, void* event, int8_t as_raw);
    int32_t tm_post_pack_data(void* pack_data, uint32_t timeout_ms);
    int32_t tm_post_history_data(void* history_data, uint32_t timeout_ms);

#ifdef CONFIG_TM_GATEWAY
    typedef int32_t (*tm_subdev_cb)(const uint8_t* name, void* data, uint32_t data_len);

    int32_t tm_set_subdev_callback(tm_subdev_cb callback);

    int32_t tm_post_raw(const uint8_t* name, uint8_t* raw_data, uint32_t raw_data_len, uint8_t** reply_data, uint32_t* reply_data_len, uint32_t timeout_ms);
    int32_t
    tm_send_request(const uint8_t* name, uint8_t as_raw, void* data, uint32_t data_len, void** reply_data, uint32_t* reply_data_len, uint32_t timeout_ms);
    int32_t
    tm_send_response(const uint8_t* name, uint8_t* msg_id, int32_t msg_code, uint8_t as_raw, void* resp_data, uint32_t resp_data_len, uint32_t timeout_ms);
#endif
    int32_t tm_step(uint32_t timeout_ms);


#ifdef __cplusplus
}
#endif

#endif
