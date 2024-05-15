//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_3rd.h
// Auther      : zhaoning
// Version     :
// Date : 2024-5-13
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-13
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_3RD_H_
#define _DRV_3RD_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

// Public defines / typedefs ----------------------------------------------------

#define drv_3rd_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define drv_3rd_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

// Public functions prototypes --------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_3RD_H_.2024-5-13 16:03:12 by: zhaoning */

