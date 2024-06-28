//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
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
#include "osa_mem.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

#include "drv_wtn6x.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_sc7a20_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_sc7a20_catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_wtn6x_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_ISR_PIN_NUM        17

#define SAMPLE_SC7A20_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

//OS_HISR         sample_sc7a20_int_detect_hisr;
//OSATimerRef sample_sc7a20_int_detect_timer_ref = NULL;
//OSFlagRef      sample_sc7a20_int_detect_flg_ref = NULL;
OSATaskRef     sample_sc7a20_int_detect_task_ref = NULL;
char         sample_sc7a20_int_detect_stack_ptr[SAMPLE_SC7A20_STACK_SIZE] = {0};
static UINT32  event_ticks = 0;
static UINT8   int_status = 0;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_sc7a20_task(void *ptr);
void sample_sc7a20_irq_handler (void);
void sample_sc7a20_detect_task(void *param);
void sample_sc7a20_detect_timer_handler(UINT32 arg);
void sample_sc7a20_detect_handler(void);

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
    int ret = 0;
//    GPIOConfiguration config = {0};

    //创建定时器
//    OSATimerCreate(&sample_sc7a20_int_detect_timer_ref);
    //创建中断事件
//    OSAFlagCreate( &sample_sc7a20_int_detect_flg_ref);  
//    Os_Create_HISR(&sample_sc7a20_int_detect_hisr, "sample_sc7a20_int_detect_hisr", sample_sc7a20_detect_handler, 2);
    //创建中断处理任务
    OSATaskCreate(&sample_sc7a20_int_detect_task_ref, sample_sc7a20_int_detect_stack_ptr, SAMPLE_SC7A20_STACK_SIZE, 100, "detect_task", sample_sc7a20_detect_task, NULL);

    //初始化int引脚，这里使用70脚 gpio126
//    config.pinDir = GPIO_IN_PIN;
//    config.pinEd = GPIO_TWO_EDGE;
//    config.pinPull = GPIO_PULLUP_ENABLE;
//    config.isr = sample_sc7a20_irq_handler;
//    GpioInitConfiguration(SAMPLE_GPIO_ISR_PIN_NUM, config);
}

/**
  * Function    : sample_sc7a20_irq_handler
  * Description : 中断回调函数，通知sample_sc7a20_detect_handler
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void sample_sc7a20_irq_handler (void)
//{
//    OS_Activate_HISR(&sample_sc7a20_int_detect_hisr);
//}

/**
  * Function    : sample_sc7a20_detect_timer_handler
  * Description : 定时器超时回调函数，产生一个事件标志
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void sample_sc7a20_detect_timer_handler(UINT32 arg)
//{
//    OS_STATUS os_status;

//    os_status = OSAFlagSet(sample_sc7a20_int_detect_flg_ref, 0x01, OSA_FLAG_OR);
//    ASSERT(os_status == OS_SUCCESS);
//}

/**
  * Function    : sample_sc7a20_detect_handler
  * Description : 中断发生处理，开启一个定时器，超时后，发送一个事件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void sample_sc7a20_detect_handler(void)
//{

//    OS_STATUS status;

//    OSATimerStop(sample_sc7a20_int_detect_timer_ref);
//    status = OSATimerStart(sample_sc7a20_int_detect_timer_ref,
//                             2,
//                             0,
//                             sample_sc7a20_detect_timer_handler,
//                             0);
//}

/**
  * Function    : sample_sc7a20_detect_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_sc7a20_detect_task(void *param)
{
    drv_virtual_i2c_sda_out_dir();
//    drv_wtn6x_1_line_write(0xEF);
    drv_wtn60x0_busy_pin_init();
    sample_wtn6x_sleep(1);
    while(1)
    {
        //第一段语音
        drv_wtn60x0_write(0);
        sample_wtn6x_sleep(3);
        //第二段语音
        drv_wtn60x0_write(1);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(2);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(3);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(4);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(5);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(6);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(7);
        sample_wtn6x_sleep(3);
        //第三段语音
        drv_wtn60x0_write(8);
        sample_wtn6x_sleep(3);

    }
}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

