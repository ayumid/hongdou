//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_wtn6.c
// Auther      : win
// Version     :
// Date : 2022-3-18
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2022-3-18
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_wtn6.h"
#include "utils_common.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : drv_wtn6_gpio_init
  * Description : WTN6一线控制GPIO初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_wtn6_gpio_init(void)
{
	GPIOReturnCode status = GPIORC_OK;
    
 	status = drv_gpio_set_direction(GPIO_WTN6_PIN, GPIO_OUT_PIN);
	if (status != GPIORC_OK){
		sdk_uart_printf("status: 0x%lx", status);
	}	
	drv_gpio_write_pin(GPIO_WTN6_PIN, 1);
}
/**
  * Function    : drv_wtn6_1_write
  * Description : WTN6一线控制地址发送
  * Input       : addr 地址
  *               
  * Output      : 
  * Return      : None
  * Auther      : win
  * Others      : 
  **/
void drv_wtn6_1_write(unsigned char addr)
{
	int i;
	drv_gpio_write_pin(GPIO_WTN6_PIN, 0);
	delay_iic_us(5000); /*>2ms*/
	for(i=0;i<8;i++)
	{
		drv_gpio_write_pin(GPIO_WTN6_PIN, 1);
		if(addr&1)
		{
		delay_iic_us(600);  /*>600us*/
		drv_gpio_write_pin(GPIO_WTN6_PIN, 0);
		delay_iic_us(200);  /*>200us*/
		}
		else
		{
		delay_iic_us(200); /*>200us*/
		drv_gpio_write_pin(GPIO_WTN6_PIN, 0);
		delay_iic_us(600);  /*>600us*/
		}
		addr>>=1; /*地址值右移一位*/
	}
	drv_gpio_write_pin(GPIO_WTN6_PIN, 1);
}

// End of file : drv_wtn6.h 2022-3-18 15:03:57 by: win 

