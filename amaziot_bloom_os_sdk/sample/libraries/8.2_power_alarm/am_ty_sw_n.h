//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_sw_n.h
// Auther      : zhaoning
// Version     :
// Date : 2024-7-26
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-26
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_TY_SW_N_H_
#define _AM_TY_SW_N_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am_ty_sw_n.h"

// Public defines / typedefs ----------------------------------------------------

enum 
{
    TY_SW_SEND_TASK_MSG_HB_MSG = 0,
    TY_SW_SEND_TASK_MSG_RSSI_MSG,
    TY_SW_SEND_TASK_MSG_RESEND_MSG,
    TY_SW_SEND_TASK_MSG_CLOCKUPDATA_MSG,
    TY_SW_SEND_TASK_MSG_GPIO_MSG,
    TY_SW_SEND_TASK_MSG_ALARM_MSG
};

typedef struct _app_ty_sw_msgq
{
    UINT32        msgId;
    UINT8* data;
    
}app_ty_sw_msgq_msg, *p_app_ty_sw_msgq_msg;

// Public functions prototypes --------------------------------------------------

int app_ty_sw_send_msgq(app_ty_sw_msgq_msg * msg);
    

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TY_SW_N_H_.2024-7-26 10:00:07 by: zhaoning */

