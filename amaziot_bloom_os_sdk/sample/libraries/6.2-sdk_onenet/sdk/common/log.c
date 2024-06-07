#include "common/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <sys/time.h>
#include "time.h"//<time.h>

// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#if 0
#define RED(str) "\033[31m" str "\033[0m"
#define B_RED(str) "\033[1;31m" str "\033[0m"
#define B_GREEN(str) "\033[1;32m" str "\033[0m"
#define B_WHITE(str) "\033[1;37m" str "\033[0m"
#define YELLOW(str) "\033[33m" str "\033[0m"
#define MAGENTA(str) "\033[35m" str "\033[0m"
#define CYAN(str) "\033[36m" str "\033[0m"
#else
#define RED(str) str
#define B_RED(str) str
#define B_GREEN(str) str
#define B_WHITE(str) str
#define YELLOW(str) str
#define MAGENTA(str) str
#define CYAN(str) str
#endif
char onenet_log_buf[LOG_MAX_BUFSIZE] = { 0 };

char* strupr(char* str)
{
#ifndef IN_RANGE
#define IN_RANGE(_begin, n, _end) (n >= _begin && n <= _end) ? (1) : (0)
#endif
    char* orign = str;
    for (; *orign != '\0'; orign++) {
        if (IN_RANGE(97, *orign, 123)) *orign -= 32;
    }

    return str;
}

int logger_print(int level, const char* file, int line, const char* func, const char* fmt, ...)
{
    if (level < LOG_LEVEL) return -10;

    int year, month, day, hour, min, sec, ms;

    struct timeval tv;
    struct tm*     tm = NULL;
    #ifndef WOLFSSL_NO_SOCK
    gettimeofday(&tv, NULL);
    #endif
    time_t tt = tv.tv_sec;
    tm        = localtime(&tt);
    year      = tm->tm_year + 1900;
    month     = tm->tm_mon + 1;
    day       = tm->tm_mday;
    hour      = tm->tm_hour;
    min       = tm->tm_min;
    sec       = tm->tm_sec;
    ms        = tv.tv_usec / 1000;

    char level_str[64]      = { 0 };
    char timestamp_str[128] = { 0 };
    switch (level) {
        case LOG_LEVEL_DEBUG:
            snprintf(level_str, sizeof(level_str), "%s", LOG_ENABLEE_COLOR ? B_WHITE("DEBUG") : ("DEBUG"));
            break;
        case LOG_LEVEL_INFO:
            snprintf(level_str, sizeof(level_str), "%s", LOG_ENABLEE_COLOR ? B_GREEN("INFO ") : ("INFO "));
            break;
        case LOG_LEVEL_WARN:
            snprintf(level_str, sizeof(level_str), "%s", LOG_ENABLEE_COLOR ? YELLOW("WARN ") : ("WARN "));
            break;
        case LOG_LEVEL_ERROR:
            snprintf(level_str, sizeof(level_str), "%s", LOG_ENABLEE_COLOR ? RED("ERROR") : ("ERROR"));
            break;
        case LOG_LEVEL_BREAKPOINT:
            snprintf(level_str, sizeof(level_str), "%s", LOG_ENABLEE_COLOR ? CYAN("POINT") : ("POINT"));
            break;
        case LOG_LEVEL_FATAL:
            snprintf(level_str, sizeof(level_str), "%s", LOG_ENABLEE_COLOR ? B_RED("FATAL") : ("FATAL"));
            break;
        default:
            break;
    }

    if (LOG_ENABLEE_COLOR) {
        snprintf(timestamp_str, sizeof(timestamp_str), MAGENTA("%04d-%02d-%02d %02d:%02d:%02d.%03d"), year, month, day, hour, min, sec, ms);
    } else {
        snprintf(timestamp_str, sizeof(timestamp_str), "%04d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day, hour, min, sec, ms);
    }

    char* buf     = onenet_log_buf;
    int   bufsize = LOG_MAX_BUFSIZE;
    int   len     = 0;

    len += snprintf(buf + len, bufsize - len, "%s %s ", timestamp_str, level_str);

    va_list ap;
    va_start(ap, fmt);
    len += vsnprintf(buf + len, bufsize - len, fmt, ap);
    va_end(ap);

    if (LOG_ENABLEE_COLOR) {
        len += snprintf(buf + len, bufsize - len, B_WHITE("%s[%s:%d:%s]\n"), (buf[len - 1] == '\n') ? ("") : (" "), file, line, func);
    } else {
        len += snprintf(buf + len, bufsize - len, "%s[%s:%d:%s]\n", (buf[len - 1] == '\n') ? ("") : (" "), file, line, func);
    }

    //printf("%s", buf);
    catstudio_printf("%s", buf);
    return len;
}