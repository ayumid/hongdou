/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file main.c
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "data_types.h"
#include "plat_time.h"
#include "tm_api.h"
#include "tm_data.h"
#include "tm_user.h"
#include "utils.h"

/** OneNET平台产品ID*/
#ifndef TM_PRODUCT_ID
#define TM_PRODUCT_ID ""
#endif
/** 设备唯一识别码，产品内唯一*/

#ifndef TM_DEVICE_NAME
#define TM_DEVICE_NAME ""
#endif

/** 认证信息--设备key*/
#ifndef TM_ACCESS_KEY
#define TM_ACCESS_KEY ""
#endif

/** token有效时间，默认设置为2030年12月*/
#define TM_EXPIRE_TIME 1924833600

static void* ont_tm_pthread_func()
{

    struct tm_downlink_tbl_t dl_tbl;
    int                      ret = 0;

    dl_tbl.prop_tbl      = tm_prop_list;
    dl_tbl.prop_tbl_size = tm_prop_list_size;
    dl_tbl.svc_tbl       = tm_svc_list;
    dl_tbl.svc_tbl_size  = tm_svc_list_size;

    /** 设备初始化*/
    ret = tm_init(&dl_tbl);
    CHECK_EXPR_GOTO(ERR_OK_ != ret, _END, "ThingModel init failed!\n");
    logi("ThingModel init ok");

    /** 设备登录*/
    ret = tm_login(TM_PRODUCT_ID, TM_DEVICE_NAME, TM_ACCESS_KEY, TM_EXPIRE_TIME, 3000);
    CHECK_EXPR_GOTO(ERR_OK_ != ret, _END, "ThingModel login failed!");
    logi("ThingModel login ok");

    while (1) {
        if (0 != (ret = tm_step(200))) {
            logi("ThingModel tm_step failed,ret is %d", ret);
            break;
        }
        time_delay_ms(50);
    }

    /** 设备注销*/
    tm_logout(3000);

_END:
    return NULL;
}

#define RECONNECT_NUM 5    // 重连OneNET次数

int onenet_sample(void)
{
    pthread_t tm_tid;
    int i = 0;
    
    for (i = 0; i < RECONNECT_NUM; i++) {
        pthread_create(&tm_tid, NULL, ont_tm_pthread_func, NULL);

        pthread_join(tm_tid, NULL);

        logw("Client logout!,try to relogin %d ", i + 1);

        /// @attention 随机避让机制：在[5,30]闭区间内随机生成一个整数，并作为休眠时长参数。避免大量客户端频繁重连，无重连退避时间导致形成 DDOS 攻击服务端 Broker
        time_delay_ms(rand_number(5, 30) * 1000);
    }

    loge("exceeded the maximum value!");

    return 0;
}