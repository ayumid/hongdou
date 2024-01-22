#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ctype.h"
#include "time.h"
#include "osa.h"
#include "pmic.h"
#include "pmic_rtc.h"
#include "teldef.h"
#include "telatci.h"
#include "telutl.h"

#include "sockets.h"
#include "netdb.h"

#include "gpio.h"
#include "cgpio.h"

#include "UART.h"


#define sdk_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)


#define app_log_debug(fmt, args...) do {catstudio_printf("App: "fmt, ##args);} while(0)
#define app_log_info(fmt, args...) do {catstudio_printf("App: "fmt, ##args);} while(0)
#define app_log_err(fmt, args...) do {catstudio_printf("App: "fmt, ##args);} while(0)

OS_HISR 	gpio_detect_hisr_ref;
OSATimerRef gpio_detect_timer_ref = NULL;
OSFlagRef  gpio_detect_flag_ref=NULL;
OSATaskRef gpio_detect_task_ref=NULL;
char gpio_detect_task_stack[1024]={0};

UINT32 gpio_detect_gpio = 27;
UINT32 gpio_debounce_time = 100;

#define GPIO_DET_BIT		0x100

UINT32 get_gpio_status(void)
{
    UINT32 value;

	value=GpioGetLevel(gpio_detect_gpio);

	return value;
}

void gpio_det_lisr (void)
{
	STATUS status;

	GpioDisableEdgeDetectionBit(gpio_detect_gpio, GPIO_TWO_EDGE);
	
	status = OS_Activate_HISR(&gpio_detect_hisr_ref);
	ASSERT(status == OS_SUCCESS);
}


void gpio_det_trigger(void)
{
    OSAFlagSet(gpio_detect_flag_ref, GPIO_DET_BIT, OSA_FLAG_OR);
}

void gpio_det_task_entry(void *ptr)
{
	GPIO_ReturnCode ret;
	UINT32 value;
	OSA_STATUS status = OS_SUCCESS;
	UINT32 flag_value = 0;
	UINT32 flag_mask = GPIO_DET_BIT;
	UINT32 gpio_status;

	
	while(1)
	{
		status = OSAFlagWait( gpio_detect_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		
		if(flag_value & GPIO_DET_BIT)
		{
			gpio_status = get_gpio_status();

			app_log_info("gpio_det_task_entry gpio_status = %u", gpio_status);

			if (gpio_status == 0){
				// wakeup
				OEM_SET_ALLOW_ENTER_SLEEP_FALG(0);		
			}else{
				// sleep
				OEM_SET_ALLOW_ENTER_SLEEP_FALG(1);						
			}
		}
	}
}

void gpio_det_timer_handler(UINT32 arg)
{
	OS_STATUS os_status;
	UINT32 gpio_status;

	gpio_status = get_gpio_status();

	app_log_info("gpio_status = %u", gpio_status);
	
	GpioEnableEdgeDetectionBit(gpio_detect_gpio, GPIO_TWO_EDGE);
}

static void gpio_det_hisr(void)
{
	UINT32 timeout;
	
	OS_STATUS status;
	
	OSATimerStop(gpio_detect_timer_ref);

	timeout=gpio_debounce_time/5;

	if(timeout==0)
		timeout=1;

	app_log_info("gpio_det_hisr %lu 0x%lx, gpio_status=%u", timeout, get_current_TStimer_tick(), get_gpio_status());

	status = OSATimerStart(gpio_detect_timer_ref,
                             timeout,
                             0,
                             gpio_det_timer_handler,
                             0);
}

int app_init_1(void)
{

	GPIOConfiguration config;

    OSATimerCreate(&gpio_detect_timer_ref);
    OSAFlagCreate( &gpio_detect_flag_ref);  
    Os_Create_HISR(&gpio_detect_hisr_ref, "gpio_det_hisr", gpio_det_hisr, 2);
    OSATaskCreate(&gpio_detect_task_ref, gpio_detect_task_stack, 1024, 60, "gpio_det_task_entry", gpio_det_task_entry, NULL);
	
	config.pinDir = GPIO_IN_PIN;
	config.pinEd = GPIO_TWO_EDGE;
	config.pinPull = GPIO_PULLUP_ENABLE;
	config.isr = gpio_det_lisr;
	GpioInitConfiguration(gpio_detect_gpio, config);

	OEM_SET_ALLOW_ENTER_SLEEP_FALG(1);	

	return 0;
}


