//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_nvc040.c
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

// Includes ---------------------------------------------------------------------

#include "drv_nvc040.h"
#include "utils_common.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
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
void drv_nvc040_gpio_init(void)
{
	GPIOReturnCode status = GPIORC_OK;
    
 	status = drv_gpio_set_direction(GPIO_NVC040_PIN, GPIO_OUT_PIN);
	if (status != GPIORC_OK){
		sdk_uart_printf("status: 0x%lx", status);
	}	
	drv_gpio_write_pin(GPIO_NVC040_PIN, 1);
}
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
void drv_nvc040_1_write(unsigned char addr)
{
	int i;
	drv_gpio_write_pin(GPIO_NVC040_PIN, 0);
	delay_iic_us(5000); /*>2ms*/
	for(i=0;i<8;i++)
	{
		drv_gpio_write_pin(GPIO_NVC040_PIN, 1);
		if(addr&1)
		{
		delay_iic_us(2400);  /*>2400us*/
		drv_gpio_write_pin(GPIO_NVC040_PIN, 0);
		delay_iic_us(800);  /*>800us*/
		}
		else
		{
		delay_iic_us(800); /*>800us*/
		drv_gpio_write_pin(GPIO_NVC040_PIN, 0);
		delay_iic_us(2400);  /*>2400us*/
		}
		addr>>=1; /*地址值右移一位*/
	}
	drv_gpio_write_pin(GPIO_NVC040_PIN, 1);
}


// End of file : drv_nvc040.h 2022-3-8 14:47:58 by: win 

