//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_clock.h
// Auther      : zhaoning
// Version     :
// Date : 2023-10-24
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-10-24
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_CLOCK_H_
#define _AM_CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

#define         DTU_CLK_ONE_HOUR_HAS_MIN            (60ul)
#define         DTU_CLK_ONE_MIN_HAS_SEC             (60ul)
#define         DTU_CLK_ONE_HOUR_HAS_SEC            (3600ul)
#define         DTU_CLK_ONE_DAY_HAS_HOUR            (24ul)
#define         DTU_CLK_ONE_DAY_HAS_SEC             (86400ul)

// Public functions prototypes --------------------------------------------------

void dtu_clk_times_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_CLOCK_H_.2023-10-24 9:25:59 by: zhaoning */

