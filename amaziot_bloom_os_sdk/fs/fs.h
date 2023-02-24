//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : fs.h
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
#ifndef _FS_H_
#define _FS_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "FDI_TYPE.h"
#include "FDI_FILE.h"

// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
int fs_write_data(const char* key, const unsigned char* buffer, unsigned int length);
int fs_read_data(const char* key, unsigned char* buffer, unsigned int* length);
int fs_delete_data(const char* key);

#endif /* ifndef _FS_H_.2021-11-24 16:39:31 by: win */

