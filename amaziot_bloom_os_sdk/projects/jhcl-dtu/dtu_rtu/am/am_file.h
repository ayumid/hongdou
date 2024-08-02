//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_file.h
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
#ifndef _AM_FILE_H_
#define _AM_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"
#include "am.h"

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_trans_conf_file_init(void);
int dtu_trans_conf_file_write(DTU_FILE_PARAM_T* st_dtu_file);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_FILE_H_.2023-8-28 11:43:47 by: zhaoning */

