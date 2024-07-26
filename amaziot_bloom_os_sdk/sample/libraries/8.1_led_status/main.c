//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-16
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-16
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

#include "am_ty_sw_led_status.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_led_status_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_led_status_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_led_status_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_LED_STATUS_TASK_STACK_SIZE     1024 * 2

// Private variables ------------------------------------------------------------

static UINT8 sample_led_status_task_stack[SAMPLE_LED_STATUS_TASK_STACK_SIZE];
static OSTaskRef sample_led_status_task_ref = NULL;

// Public variables -------------------------------------------------------------

extern UINT8 lib_led_status;

// Private functions prototypes -------------------------------------------------

static void sample_gpio_task_gpio(void *ptr);

// Public functions prototypes --------------------------------------------------

extern UINT32 GPIO_MFPR_ADDR(UINT32 n);

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

    ret = OSATaskCreate(&sample_led_status_task_ref, sample_led_status_task_stack, SAMPLE_LED_STATUS_TASK_STACK_SIZE, 120, "sample_gpio_task_gpio", sample_gpio_task_gpio, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_led_status_uart_printf("Phase2Inits_exit\n");
}

static void sample_gpio_task_gpio(void *ptr)
{
    lib_led_status_init();
    
    while(1)
    {
        //每一种形态显示10s
        lib_led_status = NORMAL;
        sample_led_status_uart_printf("%s[%d] NORMAL", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
        lib_led_status = NOGPRS;
        sample_led_status_uart_printf("%s[%d] NOGPRS", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
        lib_led_status = NOSERVER;
        sample_led_status_uart_printf("%s[%d] NOSERVER", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
        lib_led_status = UNREGISTERED;
        sample_led_status_uart_printf("%s[%d] UNREGISTERED", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
        lib_led_status = UPGRADE;
        sample_led_status_uart_printf("%s[%d] UPGRADE", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
        lib_led_status = NOTLIVE;
        sample_led_status_uart_printf("%s[%d] NOTLIVE", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
        lib_led_status = FACTORY;
        sample_led_status_uart_printf("%s[%d] FACTORY", __FUNCTION__, __LINE__);
        sample_led_status_sleep(10);
    }
}

// End of file : main.c 2023-5-16 9:01:12 by: zhaoning 

