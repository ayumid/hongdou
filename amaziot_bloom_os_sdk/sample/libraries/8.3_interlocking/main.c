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

#include "am_ty_sw_interlocking.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_lock_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_lock_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_lock_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_LOCK_TASK_STACK_SIZE     1024 * 2

// Private variables ------------------------------------------------------------

static UINT8 sample_lock_task_stack[SAMPLE_LOCK_TASK_STACK_SIZE];
static OSTaskRef sample_lock_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_interlock_task(void *ptr);

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

    ret = OSATaskCreate(&sample_lock_task_ref, sample_lock_task_stack, SAMPLE_LOCK_TASK_STACK_SIZE, 120, "sample_interlock_task", sample_interlock_task, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_lock_uart_printf("Phase2Inits_exit\n");
}

static void sample_interlock_task(void *ptr)
{
    //初始化4个led灯来演示互锁
    lib_interlocking_gpio_init();
    
    while(1)
    {
        //只有一个led点亮
        lib_interlocking_gpio0_level_flip_lock();
        sample_lock_uart_printf("%s[%d] LED1", __FUNCTION__, __LINE__);
        sample_lock_sleep(1);
        lib_interlocking_gpio1_level_flip_lock();
        sample_lock_uart_printf("%s[%d] LED2", __FUNCTION__, __LINE__);
        sample_lock_sleep(1);
        lib_interlocking_gpio2_level_flip_lock();
        sample_lock_uart_printf("%s[%d] LED4", __FUNCTION__, __LINE__);
        sample_lock_sleep(1);
        lib_interlocking_gpio3_level_flip_lock();
        sample_lock_uart_printf("%s[%d] LED4", __FUNCTION__, __LINE__);
        sample_lock_sleep(1);
     }
}

// End of file : main.c 2023-5-16 9:01:12 by: zhaoning 

