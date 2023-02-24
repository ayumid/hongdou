//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_gpio.h
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
#ifndef _DRV_GPIO_H_
#define _DRV_GPIO_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"

// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_gpio_set_direction
  * Description : 设置gpio输入输出
  * Input       : portHandle    gpio引脚
  *               dir 引脚输入输出状态
  *               
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
GPIOReturnCode drv_gpio_set_direction(UINT32 portHandle, GPIOPinDirection dir);

/**
  * Function    : drv_gpio_write_pin
  * Description : 设置gpio电平
  * Input       : portHandle    gpio引脚
  *               value 引脚状态
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
GPIOReturnCode drv_gpio_write_pin(UINT32 portHandle, UINT32 value);
/**
  * Function    : drv_gpio_read_pin
  * Description : 读取gpio电平
  * Input       : portHandle    gpio引脚
  *               
  * Output      : 
  * Return      : 引脚状态
  * Auther      : zhaoning
  * Others      : 
  **/
GPIOReturnCode drv_gpio_read_pin(UINT32 portHandle);
/**
  * Function    : drv_gpio_init
  * Description : gpio初始化
  * Input       : portHandle    gpio引脚
  *               config  初始化结构体
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
GPIOReturnCode drv_gpio_init(UINT32 portHandle, GPIOConfiguration config);

#endif /* ifndef _DRV_GPIO_H_.2021-11-23 17:48:09 by: win */

