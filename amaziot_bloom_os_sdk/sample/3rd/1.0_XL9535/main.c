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

#include "drv_xl9535.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_xl9535_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_xl9535_catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_xl9535_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_ISR_PIN_NUM        126

#define SAMPLE_XL9535_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

OS_HISR         sample_xl9535_int_detect_hisr;
OSATimerRef sample_xl9535_int_detect_timer_ref = NULL;
OSFlagRef      sample_xl9535_int_detect_flg_ref = NULL;
OSATaskRef     sample_xl9535_int_detect_task_ref = NULL;
char         sample_xl9535_int_detect_stack_ptr[SAMPLE_XL9535_STACK_SIZE] = {0};
static UINT32  event_ticks = 0;
static UINT8   int_status = 0;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_xl9535_task(void *ptr);
void sample_xl9535_irq_handler (void);
void sample_xl9535_detect_task(void *param);
void sample_xl9535_detect_timer_handler(UINT32 arg);
void sample_xl9535_detect_handler(void);

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
    GPIOConfiguration config = {0};

    //创建定时器
    OSATimerCreate(&sample_xl9535_int_detect_timer_ref);
    //创建中断事件
    OSAFlagCreate( &sample_xl9535_int_detect_flg_ref);  
    Os_Create_HISR(&sample_xl9535_int_detect_hisr, "sample_xl9535_int_detect_hisr", sample_xl9535_detect_handler, 2);
    //创建中断处理任务
    OSATaskCreate(&sample_xl9535_int_detect_task_ref, sample_xl9535_int_detect_stack_ptr, SAMPLE_XL9535_STACK_SIZE, 100, "detect_task", sample_xl9535_detect_task, NULL);

    //初始化int引脚，这里使用70脚 gpio126
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
    config.isr = sample_xl9535_irq_handler;
    GpioInitConfiguration(SAMPLE_GPIO_ISR_PIN_NUM, config);
}

/**
  * Function    : sample_xl9535_irq_handler
  * Description : 中断回调函数，通知sample_xl9535_detect_handler
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_xl9535_irq_handler (void)
{
    OS_Activate_HISR(&sample_xl9535_int_detect_hisr);
}

/**
  * Function    : sample_xl9535_detect_timer_handler
  * Description : 定时器超时回调函数，产生一个事件标志
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_xl9535_detect_timer_handler(UINT32 arg)
{
    OS_STATUS os_status;

    os_status = OSAFlagSet(sample_xl9535_int_detect_flg_ref, 0x01, OSA_FLAG_OR);
    ASSERT(os_status == OS_SUCCESS);
}

/**
  * Function    : sample_xl9535_detect_handler
  * Description : 中断发生处理，开启一个定时器，超时后，发送一个事件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_xl9535_detect_handler(void)
{

    OS_STATUS status;

    OSATimerStop(sample_xl9535_int_detect_timer_ref);
    status = OSATimerStart(sample_xl9535_int_detect_timer_ref,
                             2,
                             0,
                             sample_xl9535_detect_timer_handler,
                             0);
}

/**
  * Function    : sample_xl9535_detect_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_xl9535_detect_task(void *param)
{
    GPIO_ReturnCode ret = 0;
    UINT32 value = 0;
    OSA_STATUS status = OS_SUCCESS;
    UINT32 flag_value = 0;
    unsigned char RegAddr = 0;
    unsigned char RegWriteValue = 0;
    unsigned char RegReadValue0 = 0;
    unsigned char RegReadValue1 = 0;
    UINT8 down_en = 0;
    UINT8 up_en = 0;
    UINT8 sig_en = 0;
    UINT8 mul_en = 0;
    UINT8 read_en = 0;
    UINT8 de_en = 0;
    UINT8 ck_en = 0;
    UINT8 sim_en = 0;
    UINT8 gnss_en = 0;
    UINT8 nb_en = 0;
    UINT8 fourg_en = 0;

    //初始化i2c总线
    ret = drv_xl9535_i2c_init();
    sample_xl9535_catstudio_printf("ql_i2c_init ret %d", ret);

    //上电后，读xl9535 io扩展芯片的8个寄存器，测试用
    RegAddr       = 0x00;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 00 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x01;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 01 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x02;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 02 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x03;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 03 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x04;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 04 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x05;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 05 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x06;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 06 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    RegAddr       = 0x07;
    ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
    sample_xl9535_catstudio_printf("read reg 07 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);

    //等1s
    OSATaskSleep(1 * 200);

    //初始化xl0535 io扩展芯片的06 07寄存器，设置所有的io都是输入
    RegAddr       = 0x06;
    RegWriteValue = 0xFF;
    ret = drv_xl9535_i2c_write(&RegAddr, &RegWriteValue);
    sample_xl9535_catstudio_printf("write i2c value=0x%x, ret=%d\n", RegWriteValue, ret);
    RegAddr       = 0x07;
    RegWriteValue = 0xFF;
    ret = drv_xl9535_i2c_write(&RegAddr, &RegWriteValue);
    sample_xl9535_catstudio_printf("write i2c value=0x%x, ret=%d\n", RegWriteValue, ret);
    
    while(1)
    {
//        sample_xl9535_catstudio_printf("sample_xl9535_detect_task\r\n");
        status = OSAFlagWait(sample_xl9535_int_detect_flg_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);

        //红豆版，设置了边沿，但是还是会上升沿，下降沿都触发，所以会执行两次
        if(flag_value & 0x01)
        {
            RegAddr       = 0x00;
            //ret = ql_i2c_read(i2c_no, SAMPLE_I2C_SLAVE_ADDR, RegAddr, &RegReadValue0, 1);
            ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue0);
//            sample_xl9535_catstudio_printf("read reg 00 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
            RegAddr       = 0x01;
            //ret = ql_i2c_read(i2c_no, SAMPLE_I2C_SLAVE_ADDR, RegAddr, &RegReadValue0, 1);
            ret = drv_xl9535_i2c_read(&RegAddr, &RegReadValue1);
//            sample_xl9535_catstudio_printf("read reg 01 i2c value=0x%x, ret=%d\n", RegReadValue1, ret);

            //寄存器值非默认值，表示有io输入，否则表示是上升沿中断
            if(RegReadValue0 != 0xff || RegReadValue1 != 0x07)
//            if(GpioGetLevel(SAMPLE_GPIO_ISR_PIN_NUM) == 0)
            {
                event_ticks = OSAGetTicks();
                int_status = 0;

                //判断是那个IO这里使用寄存器值判断，后续可以改为根据位判断
                if(RegReadValue1 == DRV_XL9535_BUTTON_DOWN_EN)
                {
//                    sample_xl9535_catstudio_printf("down int");
                    down_en = 1;
                }
                else if(RegReadValue1 == DRV_XL9535_BUTTON_UP_EN)
                {
//                    sample_xl9535_catstudio_printf("up int");
                    up_en = 1;
                }
                else if(RegReadValue1 == DRV_XL9535_BUTTON_SIG_EN)
                {
//                    sample_xl9535_catstudio_printf("sig int");
                    sig_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_MUL_EN)
                {
//                    sample_xl9535_catstudio_printf("mul int");
                    mul_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_READ_EN)
                {
//                    sample_xl9535_catstudio_printf("read int");
                    read_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_DE_EN)
                {
//                    sample_xl9535_catstudio_printf("de int");
                    de_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_CK_EN)
                {
//                    sample_xl9535_catstudio_printf("ck int");
                    ck_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_SIM_EN)
                {
//                    sample_xl9535_catstudio_printf("sim int");
                    sim_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_GNSS_EN)
                {
//                    sample_xl9535_catstudio_printf("gnss int");
                    gnss_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_NB_EN)
                {
//                    sample_xl9535_catstudio_printf("nb int");
                    nb_en = 1;
                }
                else if(RegReadValue0 == DRV_XL9535_BUTTON_4G_EN)
                {
//                    sample_xl9535_catstudio_printf("4g int");
                    fourg_en = 1;
                }
            }
            else
            {
                if(int_status == 0)
                {
                    UINT32 keep_ticks = OSAGetTicks() - event_ticks;
                    
                    if(keep_ticks > 30)
                    {
                        //检测到某个IO被按下后，可以在这里，或者发送信号，消息等去其它任务执行相应操作
                        sample_xl9535_catstudio_printf("keep_ticks:%d, event_ticks:%d", keep_ticks, event_ticks);
                        event_ticks = 0;
                        if(down_en == 1)
                        {
                            sample_xl9535_catstudio_printf("down en");
                        }
                        else if(up_en == 1)
                        {
                            sample_xl9535_catstudio_printf("up en");
                        }
                        else if(sig_en == 1)
                        {
                            sample_xl9535_catstudio_printf("sig en");
                        }
                        else if(mul_en == 1)
                        {
                            sample_xl9535_catstudio_printf("mul en");
                        }
                        else if(read_en == 1)
                        {
                            sample_xl9535_catstudio_printf("read en");
                        }
                        else if(de_en == 1)
                        {
                            sample_xl9535_catstudio_printf("de en");
                        }
                        else if(ck_en == 1)
                        {
                            sample_xl9535_catstudio_printf("ck en");
                        }
                        else if(sim_en == 1)
                        {
                            sample_xl9535_catstudio_printf("sim en");
                        }
                        else if(gnss_en == 1)
                        {
                            sample_xl9535_catstudio_printf("gnss en");
                        }
                        else if(nb_en == 1)
                        {
                            sample_xl9535_catstudio_printf("nb en");
                        }
                        else if(fourg_en == 1)
                        {
                            sample_xl9535_catstudio_printf("4g en");
                        }
                    }
                    down_en = 0;
                    up_en = 0;
                    sig_en = 0;
                    mul_en = 0;
                    read_en = 0;
                    de_en = 0;
                    ck_en = 0;
                    sim_en = 0;
                    gnss_en = 0;
                    nb_en = 0;
                    fourg_en = 0;
                }
                int_status = 1;
            }
            
        }
    }
}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

