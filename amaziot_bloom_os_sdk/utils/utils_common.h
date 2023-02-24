//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : utils_common.h
// Auther      : win
// Version     :
// Date : 2021-11-24
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-24
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _UTILS_COMMON_H_
#define _UTILS_COMMON_H_

// Includes ---------------------------------------------------------------------
#include "utils.h"
#include "UART.h"
// Public defines / typedef -----------------------------------------------------
#define sleep(x) OSATaskSleep((x) * 200)//second
#define msleep(x) OSATaskSleep((x) * 20)//100*msecond //add by dmh

#undef printf
#define printf(fmt, args...) do { CPUartLogPrintf("[sdk]"fmt, ##args); } while(0)
// debug uart log
#define sdk_uart_printf(fmt, args...) do { RTI_LOG("[sdk]"fmt, ##args); } while(0)

#undef ASSERT
#define ASSERT(cOND)	{if (!(cOND)) {utilsAssertFail(#cOND, __FILE__, (short)__LINE__, 1);}}

// Public functions prototypes --------------------------------------------------

#endif /* ifndef _UTILS_COMMON_H_.2021-11-24 17:00:37 by: win */

