//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_ad_hx710b.c
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

// Includes ---------------------------------------------------------------------

#include "drv_ad_hx710b.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
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
void drv_hx710b_gpio_init(void)
{
	UINT32 tempaddr = 0;
	volatile unsigned int value;
	int ret;
	
	tempaddr = GPIO_MFPR_ADDR(HX710B_ADSK_PIN);
	value = *((volatile UINT32 *)tempaddr);
	sdk_uart_printf("HX710B_ADSK_PIN value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//多功能pin脚，大部分GPIO功能都是默认最低位为0
	}	
	*((volatile UINT32 *)tempaddr) = value;
	sdk_uart_printf("HX710B_ADSK_PIN value %lx\n", *((volatile UINT32 *)tempaddr));	

	tempaddr = GPIO_MFPR_ADDR(HX710B_ADDO_PIN);
	value = *((volatile UINT32 *)tempaddr);
	sdk_uart_printf("HX710B_ADDO_PIN value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//多功能pin脚，大部分GPIO功能都是默认最低位为0
	}	
	*((volatile UINT32 *)tempaddr) = value;
	sdk_uart_printf("HX710B_ADDO_PIN value %lx\n", *((volatile UINT32 *)tempaddr));	

	ret = GpioSetDirection(HX710B_ADSK_PIN, GPIO_OUT_PIN);
	ASSERT(ret == GPIORC_OK);	

	ret = GpioSetDirection(HX710B_ADDO_PIN, GPIO_IN_PIN);
	ASSERT(ret == GPIORC_OK);
}
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
unsigned long drv_hx710b_read_data(void)
{
	unsigned long i;   
	unsigned long hx710B_temp=0;
	GpioSetLevel(HX710B_ADSK_PIN,0);
	while(GpioGetLevel(HX710B_ADDO_PIN));
	for(i=0;i<24;i++)
	{  
		 GpioSetLevel(HX710B_ADSK_PIN,1);
		 hx710B_temp=hx710B_temp<<1;
		 GpioSetLevel(HX710B_ADSK_PIN,0); 
		 if(GpioGetLevel(HX710B_ADDO_PIN)) 
		 	hx710B_temp++;
	}
	GpioSetLevel(HX710B_ADSK_PIN,1);
	hx710B_temp=hx710B_temp^0x800000;
	GpioSetLevel(HX710B_ADSK_PIN,0); 

	return(hx710B_temp);  
}

// End of file : drv_ad_hx710b.h 2022-1-5 11:02:13 by: win 

