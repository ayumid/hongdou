//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_gpio.c
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

// Includes ---------------------------------------------------------------------

#include "drv_gpio.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
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
GPIOReturnCode drv_gpio_set_direction(UINT32 portHandle, GPIOPinDirection dir)
{
	return GpioSetDirection(portHandle,dir);
}
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
GPIOReturnCode drv_gpio_write_pin(UINT32 portHandle, UINT32 value)
{
    return GpioSetLevel(portHandle,value);
}

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
GPIOReturnCode drv_gpio_read_pin(UINT32 portHandle)
{
    return GpioGetLevel(portHandle);;
}
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
GPIOReturnCode drv_gpio_init(UINT32 portHandle, GPIOConfiguration config)
{
    return GpioInitConfiguration(portHandle,config);
}



