/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file iot_api.c
 * @date 2020/01/07
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "iot_api.h"
#include "err_def.h"
#include "plat_osl.h"

#include "iot_api.h"
#ifdef CONFIG_IOT_EDP
#include "iot_edp.h"
#endif
#ifdef CONFIG_IOT_MODBUS
#include "iot_modbus.h"
#endif

#ifdef CONFIG_IOT_MQTTS
#include "iot_mqtts.h"
#endif
#ifdef CONFIG_IOT_NBIOT
#include "iot_nbiot.h"
#endif

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/
struct iot_dev_t
{
    void* dev;

    void* (*open)(struct iot_cloud_config_t*, ont_dev_event_cb, uint32_t);
    int32_t (*step)(void*, uint32_t);
    int32_t (*push)(void*, const uint8_t*, void*, uint32_t, uint32_t);
    int32_t (*notify)(void*, struct iot_data_s*, uint32_t);
    int32_t (*close)(void*, uint32_t);
    int32_t (*observe)(void*, const uint8_t*, uint32_t);
    int32_t (*observe_cancel)(void*, const uint8_t*, uint32_t);
};

/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
void* iot_dev_open(enum iot_proto_e proto, struct iot_cloud_config_t* cloud, ont_dev_event_cb event_cb, uint32_t timeout_ms)
{
    struct iot_dev_t* iot_dev = NULL;

    iot_dev = osl_malloc(sizeof(*iot_dev));

    if (NULL == iot_dev) {
        return iot_dev;
    }
    osl_memset(iot_dev, 0, sizeof(*iot_dev));

    switch (proto) {
#ifdef CONFIG_IOT_EDP
        case IOT_PROTO_EDP:
            iot_dev->open   = iot_edp_open;
            iot_dev->close  = iot_edp_close;
            iot_dev->notify = iot_edp_notify;
            iot_dev->push   = iot_edp_push_rawdata;
            iot_dev->step   = iot_edp_step;
            break;
#endif
#ifdef CONFIG_IOT_MODBUS
        case IOT_PROTO_MODBUS: {
            break;
        }
#endif

#ifdef CONFIG_IOT_MQTTS
        case IOT_PROTO_MQTTS: {
            iot_dev->open           = iot_mqtts_login;
            iot_dev->close          = iot_mqtts_logout;
            iot_dev->notify         = iot_mqtts_notify;
            iot_dev->push           = iot_mqtts_push_rawdata;
            iot_dev->step           = iot_mqtts_step;
            iot_dev->observe        = iot_mqtts_observe;
            iot_dev->observe_cancel = iot_mqtts_observe_cancel;
            break;
        }
#endif
#ifdef CONFIG_IOT_NBIOT
        case IOT_PROTO_NBIOT: {
            iot_dev->open           = iot_nbiot_open;
            iot_dev->close          = iot_nbiot_close;
            iot_dev->notify         = iot_nbiot_notify;
            iot_dev->push           = iot_nbiot_push_rawdata;
            iot_dev->step           = iot_nbiot_step;
            iot_dev->observe        = iot_nbiot_observe;
            iot_dev->observe_cancel = iot_nbiot_observe_cancel;
            break;
        }
#endif
        default:
            break;
    }

    iot_dev->dev = iot_dev->open(cloud, event_cb, timeout_ms);

    if (iot_dev->dev) {
        return iot_dev;
    } else {
        osl_free(iot_dev);
        return NULL;
    }
}

int32_t iot_dev_close(void* dev, uint32_t timeout_ms)
{
    struct iot_dev_t* iot_dev = (struct iot_dev_t*)dev;

    if (iot_dev) {
        iot_dev->close(iot_dev->dev, timeout_ms);
        osl_free(iot_dev);
    } else
        return ERR_INVALID_PARAM;

    return ERR_OK_;
}

int32_t iot_dev_push_rawdata(void* dev, const uint8_t* resource, void* raw_data, uint32_t data_len, uint32_t timeout_ms)
{
    struct iot_dev_t* iot_dev = (struct iot_dev_t*)dev;

    if (iot_dev) {
        if (iot_dev->push)
            return iot_dev->push(iot_dev->dev, resource, raw_data, data_len, timeout_ms);
        else
            return ERR_NOT_SUPPORT;
    } else {
        return ERR_INVALID_PARAM;
    }
}

int32_t iot_dev_notify(void* dev, struct iot_data_s* data, uint32_t timeout_ms)
{
    struct iot_dev_t* iot_dev = (struct iot_dev_t*)dev;

    if (iot_dev) {
        if (iot_dev->notify)
            return iot_dev->notify(iot_dev->dev, data, timeout_ms);
        else
            return ERR_NOT_SUPPORT;
    } else {
        return ERR_INVALID_PARAM;
    }
}

int32_t iot_dev_step(void* dev, uint32_t timeout_ms)
{
    struct iot_dev_t* iot_dev = (struct iot_dev_t*)dev;

    if (iot_dev) {
        return iot_dev->step(iot_dev->dev, timeout_ms);
    } else {
        return ERR_INVALID_PARAM;
    }
}

int32_t iot_dev_observe(void* dev, const uint8_t* resource, uint32_t timeout_ms)
{
    struct iot_dev_t* iot_dev = (struct iot_dev_t*)dev;

    if (iot_dev) {
        if (iot_dev->observe)
            return iot_dev->observe(iot_dev->dev, resource, timeout_ms);
        else
            return ERR_NOT_SUPPORT;
    } else
        return ERR_INVALID_PARAM;
}
