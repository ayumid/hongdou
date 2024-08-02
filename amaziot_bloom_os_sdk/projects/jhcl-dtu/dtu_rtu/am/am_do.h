//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_do.h
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
#ifndef _AM_DO_H_
#define _AM_DO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_do_init_do(char* channel, DTU_DO* doo);
void dtu_do_init(void);
void dtu_do_write_pin(char* channel, UINT8 status);
void dtu_do_task_init(void);
UINT8 dtu_do_read_pin(UINT8 channel);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_DO_H_.2023-8-29 10:15:13 by: zhaoning */

