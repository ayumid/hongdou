/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file tm_subdev.h
 * @brief
 */

#ifndef __TM_SUBDEV_H__
#define __TM_SUBDEV_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef CONFIG_TM_GATEWAY

    /*****************************************************************************/
    /* External Definition ( Constant and Macro )                                */
    /*****************************************************************************/

    /*****************************************************************************/
    /* External Structures, Enum and Typedefs                                    */
    /*****************************************************************************/
    struct tm_subdev_cbs
    {
        // 属性获取回调
        int32_t (*subdev_props_get)(const uint8_t* product_id, const uint8_t* dev_name, const uint8_t* props_list, uint8_t** props_data);

        // 属性设置回调
        int32_t (*subdev_props_set)(const uint8_t* product_id, const uint8_t* dev_name, uint8_t* props_data);

        // 服务调用回调
        int32_t (*subdev_service_invoke)(const uint8_t* product_id, const uint8_t* dev_name, const uint8_t* svc_id, uint8_t* in_data, uint8_t** out_data);

        // 拓扑关系回调
        int32_t (*subdev_topo)(uint8_t* topo_data);
    };

    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/

    /// @brief 网关子设备属性设置、属性获取、服务调用、拓扑关系回调函数初始化
    /// @param callbacks
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_init(struct tm_subdev_cbs callbacks);

    /// @brief 网关与子设备关系绑定
    /// @param product_id 子设备产品ID
    /// @param dev_name 子设备设备名称
    /// @param access_key 子设备登陆秘钥
    /// @param timeout_ms 超时
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_add(const uint8_t* product_id, const uint8_t* dev_name, const uint8_t* access_key, uint32_t timeout_ms);

    /// @brief 网关与子设备关系绑定
    /// @param product_id 子设备产品ID
    /// @param dev_name 子设备设备名称
    /// @param access_key 子设备登陆秘钥
    /// @param timeout_ms 超时
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_delete(const uint8_t* product_id, const uint8_t* dev_name, const uint8_t* access_key, uint32_t timeout_ms);

    /// @brief 拓扑关系获取
    /// @param timeout_ms 超时
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_topo_get(uint32_t timeout_ms);

    /// @brief 子设备登陆
    /// @param product_id 子设备产品ID
    /// @param dev_name 子设备设备名称
    /// @param timeout_ms 超时
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_login(const uint8_t* product_id, const uint8_t* dev_name, uint32_t timeout_ms);

    /// @brief 子设备退出登陆
    /// @param product_id 子设备产品ID
    /// @param dev_name 子设备设备名称
    /// @param timeout_ms 超时
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_logout(const uint8_t* product_id, const uint8_t* dev_name, uint32_t timeout_ms);

    /// @brief 子设备上报数据
    /// @param product_id 产品ID
    /// @param dev_name 设备名称
    /// @param prop_json 属性上报
    /// @param event_json 事件上报
    /// @param timeout_ms 超时
    /// @return 0：成功;Others：失败
    int32_t tm_subdev_post_data(const uint8_t* product_id, const uint8_t* dev_name, uint8_t* prop_json, uint8_t* event_json, uint32_t timeout_ms);

#endif
#ifdef __cplusplus
}
#endif

#endif
