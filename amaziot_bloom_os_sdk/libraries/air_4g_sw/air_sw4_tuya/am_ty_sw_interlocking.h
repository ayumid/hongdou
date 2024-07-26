//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_op_ctrl.h
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
#ifndef _AM_TY_OP_CTRL_H_
#define _AM_TY_OP_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

#define LIB_TY_SW_OUT_PIN1  50//AM430EV5 65 pin 开源4G 空开 DTU 硬件 LED1
#define LIB_TY_SW_OUT_PIN2  49//AM430EV5 48 pin 开源4G 空开 DTU 硬件 LED2
#define LIB_TY_SW_OUT_PIN3  9//AM430EV5 54 pin 开源4G 空开 DTU 硬件 LED3
#define LIB_TY_SW_OUT_PIN4  25//AM430EV5 66 pin 开源4G 空开 DTU 硬件 LED4

// Public functions prototypes --------------------------------------------------

void lib_interlocking_gpio_init(void);
void lib_interlocking_gpio_level_flip(UINT32 portHandle,OSFlagRef OsFlagRef,OSATimerRef OsaTimeRef);
void lib_interlocking_gpio0_level_flip_lock(void);
void lib_interlocking_gpio1_level_flip_lock(void);
void lib_interlocking_gpio2_level_flip_lock(void);
void lib_interlocking_gpio3_level_flip_lock(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TY_OP_CTRL_H_.2024-7-17 11:37:32 by: zhaoning */

