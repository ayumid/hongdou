//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_led_status.h
// Auther      : zhaoning
// Version     :
// Date : 2024-7-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_TY_LED_STATUS_H_
#define _AM_TY_LED_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

#define GPIO_LED_STATUS  50//AM430EV5 65 pin 开源4G 空开 DTU 硬件 LED1

typedef enum
{
    NORMAL = 0,
    NOGPRS,
    NOSERVER,
    UNREGISTERED,
    UPGRADE,
    NOTLIVE,
    FACTORY
}GSMStatus;

// Public functions prototypes --------------------------------------------------

void lib_led_status_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TY_LED_STATUS_H_.2024-7-17 11:37:37 by: zhaoning */

