//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : menu.c
// Auther      : zhaoning
// Version     :
// Date : 2024-7-3
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-3
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "menu.h"
#include "stdio.h"
#include "drv_ssd1315_oled.h"
#include "utils_common.h"
#include "cgpio.h"

// Private defines / typedefs ---------------------------------------------------

//按键引脚宏定义
//#define INPUT_KEY0  21//16 pin
//#define INPUT_KEY1  24//19 pin
//#define INPUT_KEY2  22//21 pin
//#define INPUT_KEY3  13//26 pin
#define INPUT_KEY0  15//16 pin
#define INPUT_KEY1  14//19 pin
#define INPUT_KEY2  53//21 pin
#define INPUT_KEY3  22//26 pin

// Private variables ------------------------------------------------------------

//按键相关变量
uint8_t input_key0 = 0;
uint8_t input_key1 = 0;
uint8_t input_key2 = 0;
uint8_t input_key3 = 0;

static OSATimerRef input_key0_timer_ref = NULL;
static OSATimerRef input_key1_timer_ref = NULL;
static OSATimerRef input_key2_timer_ref = NULL;
static OSATimerRef input_key3_timer_ref = NULL;

static OS_HISR  input_key0_hisr;
static OS_HISR  input_key1_hisr;
static OS_HISR  input_key2_hisr;
static OS_HISR  input_key3_hisr;

static UINT32  input_key0_ticks = 0;
static UINT32  input_key1_ticks = 0;
static UINT32  input_key2_ticks = 0;
static UINT32  input_key3_ticks = 0;

static uint8_t   input_key0_last_status = 0;
static uint8_t   input_key1_last_status = 0;
static uint8_t   input_key2_last_status = 0;
static uint8_t   input_key3_last_status = 0;

static OSFlagRef  PluseFlgRef0 = NULL;
static OSFlagRef  PluseFlgRef1 = NULL;
static OSFlagRef  PluseFlgRef2 = NULL;
static OSFlagRef  PluseFlgRef3 = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void input_key0_irq_hadler (void);
static void input_key1_irq_hadler (void);
static void input_key2_irq_hadler (void);
static void input_key3_irq_hadler (void);

static void input_key0_hadler(void);
static void input_key1_hadler(void);
static void input_key2_hadler(void);
static void input_key3_hadler(void);

static void input_key0_callback(UINT32 tmrId);
static void input_key1_callback(UINT32 tmrId);
static void input_key2_callback(UINT32 tmrId);
static void input_key3_callback(UINT32 tmrId);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : input_key_init
  * Description : 初始化菜单按键使用的gpio
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void input_key_init(void)
{
    int ret = 0;
    
    GPIOConfiguration config;

    memset(&config, 0, sizeof(GPIOConfiguration));
    
    input_key0_last_status = 1;    //20201016
    input_key1_last_status = 1;    //20201016
    input_key2_last_status = 1;    //20201016
    input_key3_last_status = 1;    //20201016

    //init qudou
    ret = OSATimerCreate(&input_key0_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&input_key1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&input_key2_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&input_key3_timer_ref);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSAFlagCreate( &GpioFlgRef); 
//    ASSERT(ret == OS_SUCCESS);
    Os_Create_HISR(&input_key0_hisr, "Gpio_Hisr", input_key0_hadler, 2);
    Os_Create_HISR(&input_key1_hisr, "Gpio_Hisr", input_key1_hadler, 2);
    Os_Create_HISR(&input_key2_hisr, "Gpio_Hisr", input_key2_hadler, 2);
    Os_Create_HISR(&input_key3_hisr, "Gpio_Hisr", input_key3_hadler, 2);
    
    //init in gpio
    config.pinDir = GPIO_IN_PIN;
//    config.initLv = 1;
    config.pinEd = GPIO_TWO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
    
    config.isr = input_key0_irq_hadler;
    GpioInitConfiguration(INPUT_KEY0,config);
    config.isr = input_key1_irq_hadler;
    GpioInitConfiguration(INPUT_KEY1,config);
    config.isr = input_key2_irq_hadler;
    GpioInitConfiguration(INPUT_KEY2,config);
    config.isr = input_key3_irq_hadler;
    GpioInitConfiguration(INPUT_KEY3,config);

}
static void input_key0_irq_hadler (void)
{
    OS_Activate_HISR(&input_key0_hisr);
}

static void input_key1_irq_hadler (void)
{
    OS_Activate_HISR(&input_key1_hisr);
}

static void input_key2_irq_hadler (void)
{
    OS_Activate_HISR(&input_key2_hisr);
}

static void input_key3_irq_hadler (void)
{
    OS_Activate_HISR(&input_key3_hisr);
}

static void input_key0_hadler (void)
{
    OSATimerStop(input_key0_timer_ref);
    OSATimerStart(input_key0_timer_ref, 2, 0, input_key0_callback, 0); 
    
}
static void input_key1_hadler (void)
{
    OSATimerStop(input_key1_timer_ref);
    OSATimerStart(input_key1_timer_ref, 2, 0, input_key1_callback, 0); 
    
}
static void input_key2_hadler (void)
{
    OSATimerStop(input_key2_timer_ref);
    OSATimerStart(input_key2_timer_ref, 2, 0, input_key2_callback, 0); 
    
}
static void input_key3_hadler (void)
{
    OSATimerStop(input_key3_timer_ref);
    OSATimerStart(input_key3_timer_ref, 2, 0, input_key3_callback, 0); 
    
}

static void input_key0_callback(UINT32 tmrId)
{
    cprintf("input_key0_callback");
    if(GpioGetLevel(INPUT_KEY0) == 0)
    {
        input_key0_ticks = OSAGetTicks();    
        input_key0_last_status = 0;//20201016
    }
    else
    {
        if(input_key0_last_status == 0)
        {
            //20201016
            UINT32 diff0 = OSAGetTicks()-input_key0_ticks;
            cprintf("diff0:%d,input_key0_ticks:%d",diff0,input_key0_ticks);
            if(diff0 > 20)
            {
                //20201022
                cprintf("change gpio0 status");
                //oc gpio
                input_key0 = 1;
            }
        }
        input_key0_last_status = 1;    //20201016    
    }
}

static void input_key1_callback(UINT32 tmrId)
{
    cprintf("input_key1_callback");
    
    if(GpioGetLevel(INPUT_KEY1) == 0)
    {
        input_key1_ticks = OSAGetTicks();    
        input_key1_last_status = 0;//20201016
    }
    else
    {
        if(input_key1_last_status == 0)
        {
            //20201016
            UINT32 diff1 = OSAGetTicks()-input_key1_ticks;
            cprintf("diff1:%d,input_key1_ticks:%d",diff1,input_key1_ticks);
            if(diff1 > 20)
            {//20201022
                cprintf("change gpio1 status");
                //oc gpio
               input_key1 = 1;
            }
        }
        input_key1_last_status = 1;    //20201016    
    }
}

static void input_key2_callback(UINT32 tmrId)
{
    cprintf("input_key2_callback");
    
    if(GpioGetLevel(INPUT_KEY2) == 0)
    {
        input_key2_ticks = OSAGetTicks();    
        input_key2_last_status = 0;//20201016
    }
    else
    {
        if(input_key2_last_status == 0)
        {
            //20201016
            UINT32 diff2 = OSAGetTicks() - input_key2_ticks;
            cprintf("diff2:%d,input_key2_ticks:%d",diff2,input_key2_ticks);
            if(diff2 > 20)
            {
                //20201022
                cprintf("change gpio2 status");
                //oc gpio
                input_key2 = 1;
            }
        }
        input_key2_last_status = 1;    //20201016    
    }
}

static void input_key3_callback(UINT32 tmrId)
{
    cprintf("input_key3_callback");
    if(GpioGetLevel(INPUT_KEY3) == 0)
    {
        input_key3_ticks = OSAGetTicks();    
        input_key3_last_status = 0;//20201016
    }
    else
    {
        if(input_key3_last_status == 0)
        {
            //20201016
            UINT32 diff3 = OSAGetTicks()-input_key3_ticks;
            cprintf("diff3:%d,input_key3_ticks:%d",diff3,input_key3_ticks);
            if(diff3 > 20)
            {
                //20201022
                cprintf("change gpio3 status");
                //oc gpio
                input_key3 = 1;
            }
        }
        input_key3_last_status = 1;    //20201016    
    }
}

// End of file : menu.c 2024-7-3 12:03:02 by: zhaoning 

