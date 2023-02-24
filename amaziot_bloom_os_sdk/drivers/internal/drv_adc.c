//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_adc.c
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

#include "drv_adc.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : drv_adc_init
  * Description : 初始化 ADC 寄存器
  * Input       : 
  *               
  * Output      : 
  * Return      : 0 初始化 ADC 寄存器成功
  *	             -1 初始化 ADC 寄存器失败
  * Auther      : win
  * Others      : 
  **/
int drv_adc_init(void)
{
	return ql_adc_init();
}
/**
  * Function    : drv_adc_read
  * Description : 读取 ADC 通道中的模拟电压值
  * Input       : adc_channel	[In] 	指定 ADC 通道。
  * 	          p_batvol	  	[Out] 	量取的电压值。误差：约±2 %。单位：mV。
  *               
  * Output      : 
  * Return      : 0 读取成功
  *              -1 读取失败
  * Auther      : win
  * Others      : 
  **/
int drv_adc_read(unsigned char adc_channel, unsigned short *p_batvol)
{
	return ql_adc_read(adc_channel,p_batvol);
}
// End of file : drv_adc.h 2021-11-23 17:42:10 by: win 

