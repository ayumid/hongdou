//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_485.h
// Auther      : zhaoning
// Version     :
// Date : 2023-9-20
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-20
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_MB_H_
#define _AM_MB_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

#define DTU_MODBUS_TASK_STACK_SIZE     DTU_DEFAULT_THREAD_STACKSIZE * 2
#define SW_DTU_NUM_MAX                  32

typedef struct
{
    UINT8 alive;
    UINT32 timercv;
}SW_DTU_ALIVE;

// Public functions prototypes --------------------------------------------------

void dtu_modbus_task_init(void);
SW_DTU_ALIVE* sw_dtu_get_alive(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_MB_H_.2023-9-20 14:35:49 by: zhaoning */

