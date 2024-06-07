/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file err_def.h
 * @brief 错误码定义。
 */

#ifndef __ERR_DEF_H__
#define __ERR_DEF_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
/**
 * @name 操作成功
 * @{
 */
/** 成功*/
#define ERR_OK_ 0
/** @} */

/**
 * @name 系统错误
 * @{
 */
/** 内存不足*/
#define ERR_ALLOC -1
/** 接口参数错误*/
#define ERR_INVALID_PARAM -2
/** 数据错误*/
#define ERR_INVALID_DATA -3
/** 资源忙*/
#define ERR_RESOURCE_BUSY -4
/** 设备未初始化*/
#define ERR_UNINITIALIZED -5
/** 设备初始化未完成*/
#define ERR_INITIALIZING -6
/** 重复操作*/
#define ERR_REPETITIVE -7
/** IO/内部通信错误*/
#define ERR_IO -8
/** 功能不支持*/
#define ERR_NOT_SUPPORT -9
/** 系统调用错误*/
#define ERR_SYSTEM_CONTROL -10
/** 网络错误*/
#define ERR_NETWORK -11
/** 云端错误*/
#define ERR_CLOUD -12
/** 超时*/
#define ERR_TIMEOUT_ -13
/** 请求处理失败*/
#define ERR_REQUEST_FAILED -14
/** 数据溢出*/
#define ERR_OVERFLOW -15
/** 其他错误*/
#define ERR_OTHERS -16
/** @} */

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/
#include "common/log.h"
#ifndef CHECK_EXPR_GOTO
#define CHECK_EXPR_GOTO(expr, label, fmt...)                                                                                                                   \
    do {                                                                                                                                                       \
        if (expr) {                                                                                                                                            \
            log_error(fmt);                                                                                                                                    \
            goto label;                                                                                                                                        \
        }                                                                                                                                                      \
    } while (0)
#endif
#ifdef __cplusplus
}
#endif

#endif
