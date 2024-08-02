//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_socket.h
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
#ifndef _AM_SOCKET_H_
#define _AM_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

#ifdef DTU_BASED_ON_TCP

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_sockrcv_thread(void* ptr);
void dtu_set_socket_fn(void);
int dtu_socket_write(const void *data, size_t data_sz);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:36:24 by: zhaoning */

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_SOCKET_H_.2023-8-28 10:59:54 by: zhaoning */

