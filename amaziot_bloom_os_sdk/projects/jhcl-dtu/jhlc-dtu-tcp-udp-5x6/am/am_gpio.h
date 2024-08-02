//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_gpio.h
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
#ifndef _AM_GPIO_H_
#define _AM_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

#define GPIO_LINK_PIN    24

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
#define GPIO_SA_PIN      13
#define GPIO_SB_PIN      54
#endif

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
#define GPIO_GNSS_PIN    21
#endif

// Public functions prototypes --------------------------------------------------

void dtu_trans_net_led_init(void);

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
void dtu_trans_sim_init(void);
void dtu_trans_sim_switch(void);
#endif

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
void dtu_gnss_pw_init(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_GPIO_H_.2023-8-28 12:05:47 by: zhaoning */

