//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_http.h
// Auther      : zhaoning
// Version     :
// Date : 2023-9-22
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-22
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_HTTP_H_
#define _AM_HTTP_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

#define DTU_HTTP_S_TASK_STACK_SIZE     DTU_DEFAULT_THREAD_STACKSIZE * 2

#define DTU_HTTP_S_RCV_LEN     512
#define DTU_HTTP_S_URL_LEN     512

// Public functions prototypes --------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_HTTP_H_.2023-9-22 14:50:57 by: zhaoning */

