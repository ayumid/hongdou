//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_air_sw_n.h
// Auther      : zhaoning
// Version     :
// Date : 2024-6-27
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-27
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_AIR_SW_N_H_
#define _AM_AIR_SW_N_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

typedef struct _app_led_msgq
{
    UINT32 msgId;
}app_led_msgq_msg, *p_app_led_msgq_msg;

enum 
{
    LED_TASK_MSG_KEY1_MSG = 0,
    LED_TASK_MSG_KEY2_MSG,
    LED_TASK_MSG_KEY3_MSG,
    LED_TASK_MSG_KEY4_MSG
};

// Public functions prototypes --------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_AIR_SW_N_H_.2024-6-27 10:43:00 by: zhaoning */

