//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_adc.h
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
#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "ql_adc_api.h"
// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
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
int drv_adc_init(void);
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
int drv_adc_read(unsigned char adc_channel, unsigned short *p_batvol);

#endif /* ifndef _DRV_ADC_H_.2021-11-23 17:42:25 by: win */

