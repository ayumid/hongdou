/**
 * Copyright (c), 2012~2021 iot.10086.cn All Rights Reserved
 *
 * @file dev_discov.h
 * @brief
 */

#ifndef __DEV_DISCOV_H__
#define __DEV_DISCOV_H__

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
    struct discovery_info_t
    {
        /** 产品ID*/
        const uint8_t* product_id;
        /** 设备名称*/
        const uint8_t* dev_name;
        /** 设备IP地址（暂不支持IPv6），32位地址表示法*/
        uint32_t dev_addr;
        /** 主动通报间隔，单位秒。为0时不主动进行通报*/
        uint32_t notify_interval;
    };

    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/

    /// @brief 启动设备发现，支持主动通报和被动响应两种方式
    /// @param info
    /// @return info 设备信息
    int32_t discovery_start(struct discovery_info_t* info);

    /// @brief 设备发现主循环，用于处理MDNS数据，处理完成前需反复循环调用
    /// @param timeout_ms
    /// @return  0 - 处理完成，其他 - 超时
    int32_t discovery_step(uint32_t timeout_ms);

    /// @brief 停止设备发现
    /// @param
    /// @return
    int32_t discovery_stop(void);

#ifdef __cplusplus
}
#endif

#endif
