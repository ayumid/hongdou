//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_pwr.h
// Auther      : win
// Version     :
// Date : 2021-11-23
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-23
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_PWR_H_
#define _DRV_PWR_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "sdk_api.h"
// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_pwr_enable_sys_sleep
  * Description : 允许睡眠
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_pwr_enable_sys_sleep(void);
/**
  * Function    : drv_pwr_disable_sys_sleep
  * Description : 禁止睡眠
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_pwr_disable_sys_sleep(void);


#endif /* ifndef _DRV_PWR_H_.2021-11-23 18:06:40 by: win */

