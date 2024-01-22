
#ifndef _MAIN_LOG_H_
#define _MAIN_LOG_H_

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...)       \
    do                                      \
    {                                       \
        sdklogConfig(1);                    \
        sdkLogPrintf("yuge: " fmt, ##args); \
    } while (0)
// CATStudio usb log
#define catstudio_printf(fmt, args...)      \
    do                                      \
    {                                       \
        sdklogConfig(0);                    \
        sdkLogPrintf("yuge: " fmt, ##args); \
    } while (0)

// debug uart log
#define sdk_uart_ali_printf(fmt, args...)  \
    do                                     \
    {                                      \
        sdklogConfig(1);                   \
        sdkLogPrintf("ali: " fmt, ##args); \
    } while (0)
// CATStudio usb log
#define catstudio_ali_printf(fmt, args...) \
    do                                     \
    {                                      \
        sdklogConfig(0);                   \
        sdkLogPrintf("ali: " fmt, ##args); \
    } while (0)

// debug uart log
#define sdk_uart_network_printf(fmt, args...)  \
    do                                         \
    {                                          \
        sdklogConfig(1);                       \
        sdkLogPrintf("network: " fmt, ##args); \
    } while (0)
// CATStudio usb log
#define catstudio_network_printf(fmt, args...) \
    do                                         \
    {                                          \
        sdklogConfig(0);                       \
        sdkLogPrintf("network: " fmt, ##args); \
    } while (0)

// debug uart log
#define sdk_uart_mbedtls_printf(fmt, args...)  \
    do                                         \
    {                                          \
        sdklogConfig(1);                       \
        sdkLogPrintf("mbedtls: " fmt, ##args); \
    } while (0)
// CATStudio usb log
#define catstudio_mbedtls_printf(fmt, args...) \
    do                                         \
    {                                          \
        sdklogConfig(0);                       \
        sdkLogPrintf("mbedtls: " fmt, ##args); \
    } while (0)
#endif /* _MAIN_LOG_H_ */
