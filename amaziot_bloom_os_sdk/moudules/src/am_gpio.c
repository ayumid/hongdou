/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : am_gpio.c
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "cgpio.h"
#include "gpio.h"

#include "am_file.h"
#include "am_gpio.h"
#include "utils_common.h"

extern trans_conf transconf;//weihu yi ge save in flash


/***********************************************************************
*	GPIO FUNCTION RELATED DEFINE
***********************************************************************/
void init_gpio_conf(void)
{
	int ret;
    
	//init gpio
	ret = GpioSetDirection(GPIO_LINK_PIN, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    
   	ret = GpioSetDirection(GPIO_SA_PIN, GPIO_OUT_PIN);
	ASSERT(ret == GPIORC_OK);
		
	ret = GpioSetDirection(GPIO_SB_PIN, GPIO_OUT_PIN);
	ASSERT(ret == GPIORC_OK);

	//LinkA init
	ret = GpioSetLevel(GPIO_LINK_PIN,0);
	ASSERT(ret == GPIORC_OK);

	switch(transconf.sim){
		case 3:
			ret = GpioSetLevel(GPIO_SA_PIN,0);
			ASSERT(ret == GPIORC_OK);
			ret = GpioSetLevel(GPIO_SB_PIN,0);
			ASSERT(ret == GPIORC_OK);
			printf("dmhtest:GPIO_PIN_53-0&&GPIO_PIN_54-0");
			break;
		case 2:
		    ret = GpioSetLevel(GPIO_SA_PIN,1);
			ASSERT(ret == GPIORC_OK);
			ret = GpioSetLevel(GPIO_SB_PIN,0);
			ASSERT(ret == GPIORC_OK);
			printf("dmhtest:GPIO_PIN_53-1&&GPIO_PIN_54-0");
			break;
		case 1:
		    ret = GpioSetLevel(GPIO_SA_PIN,1);
			ASSERT(ret == GPIORC_OK);
			ret = GpioSetLevel(GPIO_SB_PIN,1);
			ASSERT(ret == GPIORC_OK);
			printf("dmhtest:GPIO_PIN_53-1&&GPIO_PIN_54-1");
			break;
		default:
			break;
	}
	
	
}
void trans_sim_switch(void)
{
	if(transconf.simlock == 0){
		transconf.sim++;
		if(transconf.sim >3)
			transconf.sim=1;
		printf("dmhtest:transconf.sim:%d",transconf.sim);
		trans_conf_file_write(transconf);	
	}
	
}




