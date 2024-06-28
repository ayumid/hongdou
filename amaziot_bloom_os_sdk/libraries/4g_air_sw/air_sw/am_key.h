//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_key.h
// Auther      : zhaoning
// Version     :
// Date : 2024-6-26
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-26
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_KEY_H_
#define _AM_KEY_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

#define GPIO_KEY_PIN0  21//16 pin
#define GPIO_KEY_PIN1  24//19 pin
#define GPIO_KEY_PIN2  22//21 pin
#define GPIO_KEY_PIN3  13//26 pin

#define GPIO_CLR_PIN0  16//30 pin
#define GPIO_CLR_PIN1  17//31 pin
#define GPIO_CLR_PIN2  19//32 pin
#define GPIO_CLR_PIN3  18//33 pin

#define GPIO_LED_STATUS  53//22 pin

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



#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_KEY_H_.2024-6-26 10:19:30 by: zhaoning */

