//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_ad_hx710b.h
// Auther      : win
// Version     :
// Date : 2022-1-5
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2022-1-5
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_AD_HX710B_H_
#define _DRV_AD_HX710B_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "utils_common.h"
// Public defines / typedef -----------------------------------------------------
#define HX710B_ADSK_PIN 25
#define HX710B_ADDO_PIN 26

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_hx710b_gpio_init
  * Description : HX710B对应GPIO初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_hx710b_gpio_init(void);
/**
  * Function    : drv_hx710b_read_data
  * Description : HX710B读取数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
unsigned long drv_hx710b_read_data(void);

#endif /* ifndef _DRV_AD_HX710B_H_.2022-1-5 11:02:22 by: win */

