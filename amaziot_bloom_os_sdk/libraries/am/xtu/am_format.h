//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_format.h
// Auther      : zhaoning
// Version     :
// Date : 2023-12-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-12-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_FORMAT_H_
#define _AM_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_format_send(char * raw_data, UINT16 len);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_FORMAT_H_.2023-12-7 11:50:15 by: zhaoning */

