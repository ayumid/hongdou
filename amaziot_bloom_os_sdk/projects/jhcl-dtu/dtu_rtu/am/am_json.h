//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_json.h
// Auther      : zhaoning
// Version     :
// Date : 2023-9-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-18
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_JSON_H_
#define _AM_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

#define             DTU_JSON_TASK_STACK_SIZE                    DTU_DEFAULT_THREAD_STACKSIZE * 2

// Public functions prototypes --------------------------------------------------

void dtu_json_task_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_JSON_H_.2023-9-18 14:30:36 by: zhaoning */

