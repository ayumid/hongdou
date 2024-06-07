#ifndef _ESP_USR_LOG_H_
#define _ESP_USR_LOG_H_

/*
 * hlog is thread-safe,
 * 2021/01/25 suchangwei
 */

#include "config.h"
#include <string.h>
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_STR "/"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#ifndef LOG_ENABLEE_COLOR
#define LOG_ENABLEE_COLOR 1
#endif

#ifndef LOG_MAX_BUFSIZE
#define LOG_MAX_BUFSIZE (1 << 14)    // 16k
#endif

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        LOG_LEVEL_VERBOSE = 0,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_FATAL,
        LOG_LEVEL_BREAKPOINT,
        LOG_LEVEL_SILENT
    } log_level_e;
    // color

    /// @brief 日志输出
    /// @param level 日志等级
    /// @param file 日志所在文件
    /// @param line 日志所在文件行数
    /// @param func 日志所在函数
    /// @param fmt 日志
    /// @param
    /// @return
    int logger_print(int level, const char* file, int line, const char* func, const char* fmt, ...);

#define logd(...) logger_print(LOG_LEVEL_DEBUG, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define logi(...) logger_print(LOG_LEVEL_INFO, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define logw(...) logger_print(LOG_LEVEL_WARN, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define loge(...) logger_print(LOG_LEVEL_ERROR, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define logb(...) logger_print(LOG_LEVEL_BREAKPOINT, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define logf(...) logger_print(LOG_LEVEL_FATAL, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

#ifndef log_debug
#define log_debug logd
#endif

#ifndef log_info
#define log_info logi
#endif

#ifndef log_warn
#define log_warn logw
#endif

#ifndef log_error
#define log_error loge
#endif

// LOG_LEVEL_BREAKPOINT
#define breakpoint() logb("breakpoint")

#ifdef __cplusplus
}    // extern "C"
#endif

#endif    // HV_LOG_H_
