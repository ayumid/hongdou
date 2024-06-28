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

#include "drv_sc7a20.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_sc7a20_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_sc7a20_catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_sc7a20_sleep(x) OSATaskSleep((x) * 200)//second

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
    GPIO_ReturnCode ret = 0;
    UINT32 value = 0;
    OSA_STATUS status = OS_SUCCESS;
    UINT32 flag_value = 0;
    unsigned char RegAddr = 0;
    unsigned char RegWriteValue = 0;
    unsigned char RegReadValue0 = 0;
    unsigned char RegReadValue1 = 0;
    INT16 accX = 0;
    INT16 accY = 0;
    INT16 accZ = 0;
    float pitch = 0.0;
    float roll = 0.0;
    float zAngle = 0.0;
    //初始化i2c总线
    ret = drv_sc7a20_i2c_init();
    sample_sc7a20_catstudio_printf("ql_i2c_init ret %d", ret);
    OSATaskSleep(1 * 200);

    //上电后，读sc7a20 io扩展芯片的个寄存器，测试用
    RegAddr       = 0x0F;
    ret = drv_sc7a20_i2c_read(&RegAddr, &RegReadValue0);
    sample_sc7a20_catstudio_printf("read reg 00 i2c value=0x%x, ret=%d", RegReadValue0, ret);

    //等1s
    OSATaskSleep(1 * 200);
    //初始化sc7a20
    RegAddr       = 0x20;
    RegWriteValue = 0x47;
    ret = drv_sc7a20_i2c_write(&RegAddr, &RegWriteValue);//50Hz+正常模式xyz使能
    sample_sc7a20_catstudio_printf("write i2c value=0x%x, ret=%d", RegWriteValue, ret);
    RegAddr       = 0x21;
    RegWriteValue = 0x00;
    ret = drv_sc7a20_i2c_write(&RegAddr, &RegWriteValue);//关闭滤波器，手册上面没有滤波器截止频率设置说明，开启后无法测量静止状态下的重力加速度
    sample_sc7a20_catstudio_printf("write i2c value=0x%x, ret=%d", RegWriteValue, ret);
    RegAddr       = 0x22;
    RegWriteValue = 0x00;
    ret = drv_sc7a20_i2c_write(&RegAddr, &RegWriteValue);//关闭中断
    sample_sc7a20_catstudio_printf("write i2c value=0x%x, ret=%d", RegWriteValue, ret);
    RegAddr       = 0x23;
    RegWriteValue = 0x88;
    ret = drv_sc7a20_i2c_write(&RegAddr, &RegWriteValue);//读取完成再更新，小端模式，、2g+正常模式，高精度模式
    sample_sc7a20_catstudio_printf("write i2c value=0x%x, ret=%d", RegWriteValue, ret);

    //等1s
    OSATaskSleep(1 * 200);
    
    while(1)
    {
//        status = OSAFlagWait(sample_sc7a20_int_detect_flg_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);

//        //红豆版，设置了边沿，但是还是会上升沿，下降沿都触发，所以会执行两次
//        if(flag_value & 0x01)
//        {
//        }
        RegAddr       = 0x27;
        ret = drv_sc7a20_i2c_read(&RegAddr, &RegReadValue0);
//        sample_sc7a20_catstudio_printf("read reg 27 i2c value=0x%x, ret=%d", RegReadValue0, ret);
        RegReadValue0 = RegReadValue0 & 0x0F;
        
        if(0x0F == RegReadValue0)
        {
            drv_sc7a20_get_acc(&accX, &accY, &accZ);
            pitch = drv_sc7a20_get_pitch(accX, accY, accZ);
            roll = drv_sc7a20_get_roll(accX, accY, accZ);
            drv_sc7a20_get_angle(&zAngle);

            sample_sc7a20_catstudio_printf("aX:%d aY:%d aZ:%d pitch:%.04f roll:%.04f Angle: %.04f", accX, accY, accZ, pitch, roll, zAngle);
        }
        
        sample_sc7a20_sleep(1);
    }
}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

