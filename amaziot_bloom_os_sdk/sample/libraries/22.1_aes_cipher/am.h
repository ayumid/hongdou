//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am.h
// Auther      : zhaoning
// Version     :
// Date : 2023-8-28
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-28
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_H_
#define _AM_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdlib.h>

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define uprintf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
//#define cprintf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define dtu_sleep(x) OSATaskSleep((x) * 200)//second
#define dtu_ms_sleep(x) OSATaskSleep((x) * 20)//100*msecond
#define dtu_10ms_sleep(x) OSATaskSleep((x) * 2)//10*msecond

// Public functions prototypes --------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_H_.2023-8-28 10:35:38 by: zhaoning */

