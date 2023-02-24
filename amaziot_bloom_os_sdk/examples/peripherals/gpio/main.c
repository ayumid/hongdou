//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-2-9
// Description : 本例实现了GPIO31 GPIO 74,使用定时器2s翻转一次
//          
//          
// History     :
//     
//    1. Time         :  2023-2-9
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
#include "drv_gpio.h"

#include "bsp.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define _TASK_STACK_SIZE     1080

//KV3 GPIO序号，DF = 默认功能 Fx = 第x个功能
#define                 EXAMPLE_PIN_1_GPIO_08_DF                   8
#define                 EXAMPLE_PIN_2_GPIO_02_DF                   2
#define                 EXAMPLE_PIN_3_GPIO_78_F1                   78

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------

static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void _timer_callback(UINT32 tmrId);
static void examples_gpio_task(void *ptr);

static void examples_gpio_default_function(void);
static void examples_gpio_pins_multiplexing(void);

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
    examples_gpio_default_function();
    examples_gpio_pins_multiplexing();
}

/**
  * Function    : examples_gpio_default_function
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void examples_gpio_default_function(void)
{
    GPIOReturnCode status = GPIORC_OK;

    UINT32 tempaddr = 0;
    volatile unsigned int value;
    
    // 大部分GPIO都可以直接使用，，如果遇到部分GPIO不能直接控制，参照Phase2Inits_exit_bak(),或者连续FAE
    status = drv_gpio_set_direction(EXAMPLE_PIN_1_GPIO_08_DF, GPIO_OUT_PIN);
    if (status != GPIORC_OK){
        catstudio_printf("status: 0x%lx", status);
    }
    
    status = drv_gpio_set_direction(EXAMPLE_PIN_2_GPIO_02_DF, GPIO_OUT_PIN);
    if (status != GPIORC_OK){
        catstudio_printf("status: 0x%lx", status);
    }
}
/**
  * Function    : examples_gpio_pins_multiplexing
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void examples_gpio_pins_multiplexing(void)
{
    int ret;
    GPIOReturnCode status = GPIORC_OK;

    UINT32 tempaddr = 0;
    volatile unsigned int value;

    tempaddr = GPIO_MFPR_ADDR(EXAMPLE_PIN_3_GPIO_78_F1);
    value = *((volatile UINT32 *)tempaddr);
    catstudio_printf("EXAMPLE_PIN_3_GPIO_78_F1 value %lx\n", value);    
    // 该处只针对 GPIO78 默认不是GPIO的pin脚，但部分GPIO都不需以下配置，如有相关GPIO使用遇到问题，联系FAE
    if ((IsChipCraneM()))        // KV3 YV9   1603 GPIO78 function 1 is GPIO
        *((volatile UINT32 *)tempaddr) = 0xd041;
    else                        // RV3 RC3   1601 GPIO78 function 0 is GPIO
        *((volatile UINT32 *)tempaddr) = 0xd040;
    catstudio_printf("EXAMPLE_PIN_3_GPIO_78_F1 value %lx\n", *((volatile UINT32 *)tempaddr));        
    
    status = drv_gpio_set_direction(EXAMPLE_PIN_3_GPIO_78_F1, GPIO_OUT_PIN);
    if (status != GPIORC_OK){
        catstudio_printf("status: 0x%lx", status);
    }
    
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = drv_timer_init(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "gpio_task", examples_gpio_task, NULL);
    ASSERT(ret == OS_SUCCESS);

    drv_timer_start(_timer_ref, 2 * 200, 2 * 200, _timer_callback, 0); // 2 seconds timer
}

static int _gpio_set(UINT32 GPIO_NUM, int val)
{
    GPIOReturnCode status = GPIORC_OK;
    
    status = drv_gpio_write_pin(GPIO_NUM, val ? 1 : 0);
    if (status != GPIORC_OK){
        catstudio_printf("status: 0x%lx", status);
        return -1;
    }
    
    return 0;
}

static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void examples_gpio_task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;

    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            static int count = 0;
            count++;
            catstudio_printf("examples_gpio_task: timer out\n");
            if (count & 0x01) 
            {
                _gpio_set(EXAMPLE_PIN_1_GPIO_08_DF, 1);
                _gpio_set(EXAMPLE_PIN_2_GPIO_02_DF, 1);
                _gpio_set(EXAMPLE_PIN_3_GPIO_78_F1, 1);
                catstudio_printf("examples_gpio_task: gpio set high\n");
            } 
            else 
            {
                _gpio_set(EXAMPLE_PIN_1_GPIO_08_DF, 0);
                _gpio_set(EXAMPLE_PIN_2_GPIO_02_DF, 0);
                _gpio_set(EXAMPLE_PIN_3_GPIO_78_F1, 0);
                catstudio_printf("examples_gpio_task: gpio set low\n");
            }
        }
    }
}

// End of file : main.h 2023-2-9 14:17:39 by: zhaoning 

