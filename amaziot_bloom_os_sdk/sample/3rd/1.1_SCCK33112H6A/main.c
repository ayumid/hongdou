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

#include "drv_scck33112h6a.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_scck33112h6a_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_scck33112h6a_catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_scck33112h6a_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_ISR_PIN_NUM        126

#define SAMPLE_SSCK33112H6A_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

OS_HISR         sample_scck33112h6a_int_detect_hisr;
OSATimerRef sample_scck33112h6a_int_detect_timer_ref = NULL;
OSFlagRef      sample_scck33112h6a_int_detect_flg_ref = NULL;
OSATaskRef     sample_scck33112h6a_int_detect_task_ref = NULL;
char         sample_scck33112h6a_int_detect_stack_ptr[SAMPLE_SSCK33112H6A_STACK_SIZE] = {0};
static UINT32  event_ticks = 0;
static UINT8   int_status = 0;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_scck33112h6a_task(void *ptr);
void sample_scck33112h6a_irq_handler (void);
void sample_scck33112h6a_detect_task(void *param);
void sample_scck33112h6a_detect_timer_handler(UINT32 arg);
void sample_scck33112h6a_detect_handler(void);

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
    OSATimerCreate(&sample_scck33112h6a_int_detect_timer_ref);
    //创建中断事件
    OSAFlagCreate( &sample_scck33112h6a_int_detect_flg_ref);  
    Os_Create_HISR(&sample_scck33112h6a_int_detect_hisr, "sample_scck33112h6a_int_detect_hisr", sample_scck33112h6a_detect_handler, 2);
    //创建中断处理任务
    OSATaskCreate(&sample_scck33112h6a_int_detect_task_ref, sample_scck33112h6a_int_detect_stack_ptr, SAMPLE_SSCK33112H6A_STACK_SIZE, 100, "detect_task", sample_scck33112h6a_detect_task, NULL);

    //初始化int引脚，这里使用70脚 gpio126
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
    config.isr = sample_scck33112h6a_irq_handler;
    GpioInitConfiguration(SAMPLE_GPIO_ISR_PIN_NUM, config);
}

/**
  * Function    : sample_scck33112h6a_irq_handler
  * Description : 中断回调函数，通知sample_scck33112h6a_detect_handler
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_scck33112h6a_irq_handler (void)
{
    OS_Activate_HISR(&sample_scck33112h6a_int_detect_hisr);
}

/**
  * Function    : sample_scck33112h6a_detect_timer_handler
  * Description : 定时器超时回调函数，产生一个事件标志
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_scck33112h6a_detect_timer_handler(UINT32 arg)
{
    OS_STATUS os_status;

    os_status = OSAFlagSet(sample_scck33112h6a_int_detect_flg_ref, 0x01, OSA_FLAG_OR);
    ASSERT(os_status == OS_SUCCESS);
}

/**
  * Function    : sample_scck33112h6a_detect_handler
  * Description : 中断发生处理，开启一个定时器，超时后，发送一个事件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_scck33112h6a_detect_handler(void)
{

    OS_STATUS status;

    OSATimerStop(sample_scck33112h6a_int_detect_timer_ref);
    status = OSATimerStart(sample_scck33112h6a_int_detect_timer_ref,
                             2,
                             0,
                             sample_scck33112h6a_detect_timer_handler,
                             0);
}

/**
  * Function    : sample_scck33112h6a_detect_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_scck33112h6a_detect_task(void *param)
{
    GPIO_ReturnCode ret = 0;
    UINT32 value = 0;
    OSA_STATUS status = OS_SUCCESS;
    UINT32 flag_value = 0;
    unsigned char RegAddr = 0;
    UINT8 pR1_Data[2] = {0};

    //初始化i2c总线
    ret = drv_scck33112h6a_i2c_init();
    sample_scck33112h6a_catstudio_printf("ql_i2c_init ret %d", ret);

//    //上电后，读scck33112h6a io扩展芯片的8个寄存器，测试用
//    RegAddr       = 0x00;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 00 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x01;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 01 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x02;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 02 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x03;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 03 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x04;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 04 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x05;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 05 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x06;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 06 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
//    RegAddr       = 0x07;
//    ret = drv_scck33112h6a_i2c_read(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("read reg 07 i2c value=0x%x, ret=%d\n", RegReadValue0, ret);
    ql_i2c_read_ext(DRV_SCCK33112H6A_I2C_NUM, DRV_SCCK33112H6A_I2C_SLAVE_ADDR, DRV_SCCK33112H6A_REGISTER0_ADDRESS , 1, pR1_Data, 2);
    ql_i2c_read_ext(DRV_SCCK33112H6A_I2C_NUM, DRV_SCCK33112H6A_I2C_SLAVE_ADDR, DRV_SCCK33112H6A_REGISTER0_ADDRESS , 1, pR1_Data, 2);

    //等1s
    OSATaskSleep(1 * 200);

//    //初始化xl0535 io扩展芯片的06 07寄存器，设置所有的io都是输入
//    RegAddr       = 0x06;
//    RegWriteValue = 0xFF;
//    ret = drv_scck33112h6a_i2c_write(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("write i2c value=0x%x, ret=%d\n", RegWriteValue, ret);
//    RegAddr       = 0x07;
//    RegWriteValue = 0xFF;
//    ret = drv_scck33112h6a_i2c_write(&RegAddr, &RegReadValue0);
//    sample_scck33112h6a_catstudio_printf("write i2c value=0x%x, ret=%d\n", RegWriteValue, ret);
    
    while(1)
    {
//        sample_scck33112h6a_catstudio_printf("sample_scck33112h6a_detect_task\r\n");
//        status = OSAFlagWait(sample_scck33112h6a_int_detect_flg_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        //采集温度
//        drv_scck33112h6a_i2c_read(DRV_SCCK33112H6A_I2C_SLAVE_ADDR, DRV_SCCK33112H6A_REGISTER0_ADDRESS, pR1_Data, 2);
        ql_i2c_read_ext(DRV_SCCK33112H6A_I2C_NUM, DRV_SCCK33112H6A_I2C_SLAVE_ADDR, DRV_SCCK33112H6A_REGISTER0_ADDRESS , 1, pR1_Data, 2);
        float temp = 0.0;
        int data = 0;
        data = (pR1_Data[0] << 8 | pR1_Data[1]) >> 4;
        //判断如果是负数，需要按照二进制补码，去掉符号位，减一取反后取低12位，再乘0.0625，再用0减变为负数
        if((data & DRV_SCCK33112H6A_TEMP_SIGNED) == DRV_SCCK33112H6A_TEMP_SIGNED)
        {
            temp = (float)(0 - (~((data & DRV_SCCK33112H6A_TEMP_MSK) - 1) & DRV_SCCK33112H6A_TEMP_MSK) * DRV_SCCK33112H6A_TEMP_1LSB);
        }
        //正数直接乘0.0625
        else
        {
            temp = (float)data * DRV_SCCK33112H6A_TEMP_1LSB;
        }
        sample_scck33112h6a_catstudio_printf("data: 0x%X data: %d temp:%f\n", data, data, temp);
        sample_scck33112h6a_catstudio_printf("temprature:%f\n", temp);
        //等1s
        OSATaskSleep(1 * 200);
    }
}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

