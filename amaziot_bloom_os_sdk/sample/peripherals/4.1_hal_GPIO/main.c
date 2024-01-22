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

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_gpio_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_gpio_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_gpio_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_TASK_STACK_SIZE     1024 * 2

#define SAMPLE_GPIO_ISR_PIN_NUM        50
#define SAMPLE_GPIO_OUT_PIN_NUM          18

// Private variables ------------------------------------------------------------

static UINT8 sample_gpio_task_stack[SAMPLE_GPIO_TASK_STACK_SIZE];

static OSTaskRef sample_gpio_task_ref = NULL;
static void sample_gpio_task_gpio(void *ptr);

OS_HISR         sample_gpio_detect_hisr;
OSATimerRef sample_gpio_detect_timer_ref = NULL;
OSFlagRef      sample_gpio_detect_flg_ref=NULL;
OSATaskRef     sample_gpio_detect_task_ref=NULL;
char         sample_gpio_detect_stack_ptr[1024]={0};

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

void sample_gpio_irq_handler (void);
void sample_gpio_detect_task_entry(void *param);
void sample_gpio_detect_timer_handler(UINT32 arg);
void sample_gpio_detect_handler(void);

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

    ret = OSATaskCreate(&sample_gpio_task_ref, sample_gpio_task_stack, SAMPLE_GPIO_TASK_STACK_SIZE, 120, "sample_gpio_task_gpio", sample_gpio_task_gpio, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_gpio_uart_printf("Phase2Inits_exit\n");
}

static int sample_gpio_set(UINT32 GPIO_NUM, int val)
{
    GPIOReturnCode status = GPIORC_OK;
    
    status = GpioSetLevel(GPIO_NUM, val ? 1 : 0);
    if (status != GPIORC_OK)
    {
        sample_gpio_uart_printf("status: 0x%lx", status);
        return -1;
    }
    
    return 0;
}

static void sample_gpio_task_gpio(void *ptr)
{        
    int highlow = 0;
    GPIOReturnCode status = GPIORC_OK;
    GPIOConfiguration config;
    
    // 大部分GPIO都可以直接使用，，如果遇到部分GPIO不能直接控制,参照下面处理
    // SDK中，GPIO14 和GPIO15 默认被配置成UART3，如果需要作为GPIO使用需要重新配置下，参照《CraneL_A0_top_0929.xlsx》和《ASR1603 MFPR软件配置.V1.0.pdf》
//#if 1    // 大部分GPIO不需要配置，如果遇到使用功能受限可参照修改
//    UINT32 tempaddr = 0;
//    volatile unsigned int value;
//    
//    //GPIO 14 15 默认被使能为UART3，作为GPIO使用的话，需要重新修改下对应的寄存器，####
//    tempaddr = GPIO_MFPR_ADDR(SAMPLE_GPIO_ISR_PIN_NUM);
//    *((volatile UINT32 *)tempaddr) = 0xD0B0;    //主要关注最后一个func配置成GPIO即可，Gpio接口也会配置对应属性
//    sample_gpio_uart_printf("SAMPLE_GPIO_ISR_PIN_NUM value %lx\n", *((volatile UINT32 *)tempaddr));    
//#endif
            
/**********GPIO中断***********/        
    OSATimerCreate(&sample_gpio_detect_timer_ref);
    OSAFlagCreate( &sample_gpio_detect_flg_ref);  
    Os_Create_HISR(&sample_gpio_detect_hisr, "sample_gpio_detect_hisr", sample_gpio_detect_handler, 2);
    OSATaskCreate(&sample_gpio_detect_task_ref, sample_gpio_detect_stack_ptr, 1024, 100, "Detect_task", sample_gpio_detect_task_entry, NULL);

    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_RISE_EDGE;
    config.pinPull = GPIO_PULLDN_ENABLE;
    config.isr = sample_gpio_irq_handler;
    GpioInitConfiguration(SAMPLE_GPIO_ISR_PIN_NUM,config);
/**********GPIO中断***********/        



/**********GPIO输出***********/        
    status = GpioSetDirection(SAMPLE_GPIO_OUT_PIN_NUM, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        sample_gpio_uart_printf("status: 0x%lx", status);
    }

    while(1)
    {
        sample_gpio_sleep(5);
//        sample_gpio_uart_printf("%s: highlow: %d\n", __FUNCTION__, highlow);    
        if (highlow)
        {
            highlow = 0;
            sample_gpio_set(SAMPLE_GPIO_OUT_PIN_NUM, 1);
    
        }
        else
        {
            highlow = 1;
            sample_gpio_set(SAMPLE_GPIO_OUT_PIN_NUM, 0);
        }

    }
/**********GPIO输出***********/    

}


void sample_gpio_irq_handler (void)
{
    OS_Activate_HISR(&sample_gpio_detect_hisr);
}

void sample_gpio_detect_task_entry(void *param)
{
    GPIO_ReturnCode ret = 0;
    UINT32 value = 0;
    OSA_STATUS status = OS_SUCCESS;
    UINT32 flag_value = 0;

    while(1)
    {
        sample_gpio_uart_printf("sample_gpio_detect_task_entry\r\n");
        status = OSAFlagWait(sample_gpio_detect_flg_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);

        if(flag_value & 0x01)
        {
            value = GpioGetLevel(SAMPLE_GPIO_ISR_PIN_NUM);
            sample_gpio_uart_printf("value: %d\r\n", value);
        }
    }
}

void sample_gpio_detect_timer_handler(UINT32 arg)
{
    OS_STATUS os_status;

    os_status = OSAFlagSet(sample_gpio_detect_flg_ref, 0x01, OSA_FLAG_OR);
    ASSERT(os_status==OS_SUCCESS);    
}

void sample_gpio_detect_handler(void)
{

    OS_STATUS status;

    OSATimerStop(sample_gpio_detect_timer_ref);
    status = OSATimerStart(sample_gpio_detect_timer_ref,
                             2,
                             0,
                             sample_gpio_detect_timer_handler,
                             0);
}

// End of file : main.c 2023-5-16 9:01:12 by: zhaoning 

