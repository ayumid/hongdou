//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-15
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-15
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define led_debug_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define led_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define led_blink_sleep(x) OSATaskSleep((x) * 200)//second

#define LED_BLINK_TASK_STACK_SIZE     1024 * 2

#define LED_BLINK_GPIO_OUT_PIN_NUM          17

// Private variables ------------------------------------------------------------

static UINT8 led_blink_task_stack[LED_BLINK_TASK_STACK_SIZE];

static OSTaskRef led_blink_task_ref = NULL;
static void sample_task_led_blink_task(void *ptr);

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    int ret;

    ret = OSATaskCreate(&led_blink_task_ref, led_blink_task_stack, LED_BLINK_TASK_STACK_SIZE, 120, "sample_task_led_blink_task", sample_task_led_blink_task, NULL);
    ASSERT(ret == OS_SUCCESS);

    led_debug_uart_printf("Phase2Inits_exit\n");
}

static int sample_led_blink_gpio_set(UINT32 GPIO_NUM, int val)
{
    GPIOReturnCode status = GPIORC_OK;
    
    status = GpioSetLevel(GPIO_NUM, val);
    if (status != GPIORC_OK)
    {
        led_debug_uart_printf("status: 0x%lx", status);
        return -1;
    }
    
    return 0;
}

static void sample_task_led_blink_task(void *ptr)
{
    int highlow = 0;
    GPIOReturnCode status = GPIORC_OK;
    GPIOConfiguration config;

    status = GpioSetDirection(LED_BLINK_GPIO_OUT_PIN_NUM, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        led_debug_uart_printf("status: 0x%lx", status);
    }

    while(1)
    {
        led_blink_sleep(1);
        if(highlow)
        {
            highlow = 0;
            sample_led_blink_gpio_set(LED_BLINK_GPIO_OUT_PIN_NUM, 1);
            led_debug_uart_printf("high: %d\n", GpioGetLevel(LED_BLINK_GPIO_OUT_PIN_NUM));
        }
        else
        {
            highlow = 1;
            sample_led_blink_gpio_set(LED_BLINK_GPIO_OUT_PIN_NUM, 0);
            led_debug_uart_printf("low: %d\n",  GpioGetLevel(LED_BLINK_GPIO_OUT_PIN_NUM));
        }

    }
}

// End of file : main.c 2023-5-15 9:34:58 by: zhaoning 

