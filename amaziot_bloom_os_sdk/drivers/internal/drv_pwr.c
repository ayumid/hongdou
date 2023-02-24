//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_pwr.c
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

#include "drv_pwr.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

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
void drv_pwr_enable_sys_sleep(void)
{
    OEM_SET_ALLOW_ENTER_SLEEP_FALG(1);

	OEM_SLEEP_DISABLE_USB();

	OEM_FAST_ENABLE_UART_SLEEP();
}
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
void drv_pwr_disable_sys_sleep(void)
{
    OEM_SET_ALLOW_ENTER_SLEEP_FALG(0);
}

// End of file : drv_pwr.h 2021-11-23 18:05:17 by: win 

