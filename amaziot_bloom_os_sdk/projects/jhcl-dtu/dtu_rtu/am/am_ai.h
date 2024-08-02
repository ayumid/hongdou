//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ai.h
// Auther      : zhaoning
// Version     :
// Date : 2023-8-29
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-29
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_AI_H_
#define _AM_AI_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

UINT16 dtu_ai_read(void);
void dtu_ai_task_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_AI_H_.2023-8-29 9:58:41 by: zhaoning */

