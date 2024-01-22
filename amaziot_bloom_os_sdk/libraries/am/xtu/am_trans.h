//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_trans.h
// Auther      : zhaoning
// Version     :
// Date : 2023-11-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-11-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_TRANS_H_
#define _AM_TRANS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_uart_data_recv_thread(void);
void dtu_uart_data_recv_cbk(UINT8 *data, UINT32 len);
void dtu_trans_uart_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TRANS_H_.2023-11-17 9:25:39 by: zhaoning */

