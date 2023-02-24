//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_nvc040.h
// Auther      : win
// Version     :
// Date : 2022-3-8
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2022-3-8
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_NVC040_H_
#define _DRV_NVC040_H_

// Includes ---------------------------------------------------------------------
#include "drv_gpio.h"
#include "drv_simulator_i2c.h"
// Public defines / typedef -----------------------------------------------------
#define GPIO_NVC040_PIN        2

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_nvc040_gpio_init
  * Description : NVC040一线控制GPIO初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_nvc040_gpio_init(void);
/**
  * Function    : drv_nvc040_1_write
  * Description : NVC040一线控制地址发送
  * Input       : addr 地址
  *               
  * Output      : 
  * Return      : None
  * Auther      : win
  * Others      : 
  **/
void drv_nvc040_1_write(unsigned char addr);


#endif /* ifndef _DRV_NVC040_H_.2022-3-8 14:48:07 by: win */

