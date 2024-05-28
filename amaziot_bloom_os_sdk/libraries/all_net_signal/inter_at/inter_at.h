//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : inter_at.h
// Auther      : zhaoning
// Version     :
// Date : 2024-5-27
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-27
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _INTER_AT_H_
#define _INTER_AT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

typedef struct sdkCellInfo_struct
{
    UINT16                         phyCellId;
    UINT32                         euArfcn;
    UINT8                          rsrp;
    UINT8                          rsrq;
    INT8                           sinr;    
    
    UINT16                         mcc;
    UINT8                          lenOfMnc;   /**< Length of MNC, value range (2,3) */
    UINT16                         mnc;        /**< 2-3 digits of MNC */
    UINT16                         tac;
    UINT32                         cellId;
}LIB_CELL_INFO_T;

// Public functions prototypes --------------------------------------------------

int lib_get_rssi(void);
void lib_get_imei(char *buffer);
void lib_get_imsi(char *buffer);
void lib_get_iccid(char *buffer);
void lib_get_cell_info(LIB_CELL_INFO_T* cell_info);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _INTER_AT_H_.2024-5-27 15:16:13 by: zhaoning */

