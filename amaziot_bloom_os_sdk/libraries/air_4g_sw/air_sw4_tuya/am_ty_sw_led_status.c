//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_led_status.c
// Auther      : zhaoning
// Version     :
// Date : 2024-7-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "osa.h"
#include "cgpio.h"

#include "am_ty_sw_led_status.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static OSATimerRef lib_led_status_timer_ref = NULL;
static UINT8 lib_led_status_100ms_stamp = 0;

// Public variables -------------------------------------------------------------

//使用这个组件，需要extern这个变量，通过改变这个变量，来控制LED的状态
UINT8 lib_led_status;

// Private functions prototypes -------------------------------------------------

static void lib_led_status_timer_cbk(UINT32 tmrId);
static void lib_led_status_lighting(void);
static void lib_led_status_100ms_flashing1(void);
static void lib_led_status_100ms_flashing2(void);
static void lib_led_status_100ms_flashing3(void);
static void lib_led_status_1s_flashing(void);
static void lib_led_status_100ms_flashing(void);
static void lib_led_status_factory(void);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : lib_led_status_level_flip
  * Description : 改变LED 亮 灭 状态
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_level_flip(void)  
{
    if(GpioGetLevel(GPIO_LED_STATUS) == 0)
    {
      GpioSetLevel(GPIO_LED_STATUS,1);
    }
    else if(GpioGetLevel(GPIO_LED_STATUS) == 1)
    {
      GpioSetLevel(GPIO_LED_STATUS,0);
    }
}

/**
  * Function    : lib_led_status_init
  * Description : 初始化LED GPIO 和 定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_led_status_init(void)
{
    int ret = 0;

    ret = GpioSetDirection(GPIO_LED_STATUS, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    
    ret = OSATimerCreate(&lib_led_status_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    
    OSATimerStart(lib_led_status_timer_ref, 20, 20, lib_led_status_timer_cbk, 0); //100ms
}

/**
  * Function    : lib_led_status_lighting
  * Description : LED常亮
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_lighting(void)
{
    lib_led_status_100ms_stamp = 0;
    //常亮
    GpioSetLevel(GPIO_LED_STATUS, 0);
}

/**
  * Function    : lib_led_status_100ms_flashing
  * Description : 100ms闪烁1次
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_100ms_flashing1(void)
{
    lib_led_status_100ms_stamp++;
    if(lib_led_status_100ms_stamp == 1)
        GpioSetLevel(GPIO_LED_STATUS,1);
    if(lib_led_status_100ms_stamp == 2)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp > 20)
    {
        lib_led_status_100ms_stamp = 0;
    }
}

/**
  * Function    : lib_led_status_100ms_flashing2
  * Description : 100ms闪烁两次
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_100ms_flashing2(void)
{
    lib_led_status_100ms_stamp++;
    if(lib_led_status_100ms_stamp == 1)
        GpioSetLevel(GPIO_LED_STATUS,1);
    if(lib_led_status_100ms_stamp == 2)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 3)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 4)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp > 20)
    {
        lib_led_status_100ms_stamp = 0;
    }
}

/**
  * Function    : lib_led_status_100ms_flashing3
  * Description : 100ms闪烁3
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_100ms_flashing3(void)
{
    lib_led_status_100ms_stamp++;
    if(lib_led_status_100ms_stamp == 1)
        GpioSetLevel(GPIO_LED_STATUS,1);
    if(lib_led_status_100ms_stamp == 2)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 3)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 4)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 5)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 6)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp > 20)
    {
        lib_led_status_100ms_stamp = 0;
    }
}

/**
  * Function    : lib_led_status_1s_flashing
  * Description : 间隔1s闪烁
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_1s_flashing(void)
{
    lib_led_status_100ms_stamp++;
    if(lib_led_status_100ms_stamp > 10)
    {
        lib_led_status_level_flip();
        lib_led_status_100ms_stamp = 0;
    }
}

/**
  * Function    : lib_led_status_100ms_flashing
  * Description : 间隔100ms闪烁
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_100ms_flashing(void)
{
    lib_led_status_100ms_stamp = 0;
    lib_led_status_level_flip();
}

/**
  * Function    : lib_led_status_factory
  * Description : 工厂模式闪烁
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_factory(void)
{
    lib_led_status_100ms_stamp++;
    if(lib_led_status_100ms_stamp == 1)
        GpioSetLevel(GPIO_LED_STATUS,0);
    if(lib_led_status_100ms_stamp == 2)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 3)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 4)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 5)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 6)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 7)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 8)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp == 9)
    {
        GpioSetLevel(GPIO_LED_STATUS,0);
    }
    if(lib_led_status_100ms_stamp == 10)
    {
        GpioSetLevel(GPIO_LED_STATUS,1);
    }
    if(lib_led_status_100ms_stamp > 20)
    {
        lib_led_status_100ms_stamp = 0;
    }
}

/**
  * Function    : lib_led_status_timer_cbk
  * Description : LED定时器回调函数，在这里改变Led的状态
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_led_status_timer_cbk(UINT32 tmrId)
{
    switch(lib_led_status)
    {
        case NORMAL:
            lib_led_status_lighting();
        break;
        case NOGPRS:
            lib_led_status_100ms_flashing1();
        break;
        case NOSERVER:
            lib_led_status_100ms_flashing2();
        break;
        case UNREGISTERED:
            lib_led_status_1s_flashing();
        break;
        case UPGRADE:
            lib_led_status_100ms_flashing3();
        break;
        case NOTLIVE:
            lib_led_status_100ms_flashing();
        break;
        case FACTORY:
            lib_led_status_factory();
        break;
        default:
        
        break;
    }
}

// End of file : am_ty_led_status.c 2024-7-17 11:04:42 by: zhaoning 

