//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_interlocking.c
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

#include "am_ty_sw_interlocking.h"

// Private defines / typedefs ---------------------------------------------------

//这里的硬件使用的是 开源 4G 空开 DTU，LED是低电平亮 高电平灭
#define TY_SW_OPEN 0
#define TY_SW_CLOSE 1

// Private variables ------------------------------------------------------------

OSATimerRef _pulsetimer_ref0 = NULL;
OSATimerRef _pulsetimer_ref1 = NULL;
OSATimerRef _pulsetimer_ref2 = NULL;
OSATimerRef _pulsetimer_ref3 = NULL;

OSFlagRef  PluseFlgRef0 = NULL;
OSFlagRef  PluseFlgRef1 = NULL;
OSFlagRef  PluseFlgRef2 = NULL;
OSFlagRef  PluseFlgRef3 = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : lib_interlocking_gpio_open
  * Description : gpio输出高
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 这里的硬件使用的是 开源 4G 空开 DTU，LED是低电平亮
  **/
void lib_interlocking_gpio_open(UINT32 portHandle, OSFlagRef OsFlagRef)
{
    GpioSetLevel(portHandle, TY_SW_OPEN);
    //pluse open
//    OSAFlagSet(OsFlagRef, 0x01, OSA_FLAG_OR);
}

/**
  * Function    : lib_interlocking_gpio_close
  * Description : gpio输出低
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 这里的硬件使用的是 开源 4G 空开 DTU，LED是高电平灭
  **/
void lib_interlocking_gpio_close(UINT32 portHandle, OSATimerRef OsaTimeRef)
{
    GpioSetLevel(portHandle, TY_SW_CLOSE);
    //pluse timer close
//    OSATimerStop(OsaTimeRef);
}

/**
  * Function    : lib_interlocking_gpio_level_flip
  * Description : gpio输出翻转
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_interlocking_gpio_level_flip(UINT32 portHandle,OSFlagRef OsFlagRef,OSATimerRef OsaTimeRef)  
{
    if(GpioGetLevel(portHandle) == TY_SW_CLOSE)
    {
      lib_interlocking_gpio_open(portHandle,OsFlagRef);    
    }
    else if(GpioGetLevel(portHandle) == TY_SW_OPEN)
    {
      lib_interlocking_gpio_close(portHandle,OsaTimeRef);
    }    
}

/**
  * Function    : lib_interlocking_gpio0_level_flip_lock
  * Description : gpio 第一路输出高 其它关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_interlocking_gpio0_level_flip_lock(void)  
{
    if(GpioGetLevel(LIB_TY_SW_OUT_PIN1) == TY_SW_CLOSE)
    {
      lib_interlocking_gpio_open(LIB_TY_SW_OUT_PIN1,PluseFlgRef0);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN2,_pulsetimer_ref1);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN3,_pulsetimer_ref2);  
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN4,_pulsetimer_ref3);  
    }
    else if(GpioGetLevel(LIB_TY_SW_OUT_PIN1) == TY_SW_OPEN)
    {
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN1,_pulsetimer_ref0);               
    }    
}

/**
  * Function    : lib_interlocking_gpio1_level_flip_lock
  * Description : gpio 第二路输出高 其它关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_interlocking_gpio1_level_flip_lock(void)  
{
    if(GpioGetLevel(LIB_TY_SW_OUT_PIN2) == TY_SW_CLOSE)
    {
      lib_interlocking_gpio_open(LIB_TY_SW_OUT_PIN2,PluseFlgRef1);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN1,_pulsetimer_ref0);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN3,_pulsetimer_ref2);  
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN4,_pulsetimer_ref3);  
    }
    else if(GpioGetLevel(LIB_TY_SW_OUT_PIN2) == TY_SW_OPEN)
    {
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN2,_pulsetimer_ref1);               
    }    
}

/**
  * Function    : lib_interlocking_gpio2_level_flip_lock
  * Description : gpio 第三路输出高 其它关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_interlocking_gpio2_level_flip_lock(void)  
{
    if(GpioGetLevel(LIB_TY_SW_OUT_PIN3) == TY_SW_CLOSE)
    {
      lib_interlocking_gpio_open(LIB_TY_SW_OUT_PIN3,PluseFlgRef2);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN2,_pulsetimer_ref1);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN1,_pulsetimer_ref0);  
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN4,_pulsetimer_ref3);  
    }
    else if(GpioGetLevel(LIB_TY_SW_OUT_PIN3) == TY_SW_OPEN)
    {
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN3,_pulsetimer_ref2);               
    }    
}

/**
  * Function    : lib_interlocking_gpio3_level_flip_lock
  * Description : gpio 第四路输出高 其它关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_interlocking_gpio3_level_flip_lock(void)
{
    if(GpioGetLevel(LIB_TY_SW_OUT_PIN4) == TY_SW_CLOSE)
    {
      lib_interlocking_gpio_open(LIB_TY_SW_OUT_PIN4,PluseFlgRef3);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN2,_pulsetimer_ref1);
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN3,_pulsetimer_ref2);  
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN1,_pulsetimer_ref0);  
    }
    else if(GpioGetLevel(LIB_TY_SW_OUT_PIN4) == TY_SW_OPEN)
    {
      lib_interlocking_gpio_close(LIB_TY_SW_OUT_PIN4,_pulsetimer_ref3);               
    }    
}

/**
  * Function    : lib_interlocking_gpio_init
  * Description : 互锁GPIO初始化，上电根据上次断电前状态，或者设置的上电状态，初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_interlocking_gpio_init(void)
{
    int ret = 0;
    //init out gpio
    ret = GpioSetDirection(LIB_TY_SW_OUT_PIN1, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(LIB_TY_SW_OUT_PIN2, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(LIB_TY_SW_OUT_PIN3, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(LIB_TY_SW_OUT_PIN4, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);

//空开中，多通道的输出上电保持，保持上一次断电前状态。或者是默认ON 或 OFF
#ifdef AM_SW_INTERLOCK_POWERON_KEEP
    //init out status
    if(transconf.sdstatus0 == 2)
        GpioSetLevel(LIB_TY_SW_OUT_PIN1, transconf.initstatus0);
    else if(transconf.sdstatus0 == 1)
        GpioSetLevel(LIB_TY_SW_OUT_PIN1, 1);
    else
        GpioSetLevel(LIB_TY_SW_OUT_PIN1, 0);

    if(transconf.sdstatus1 == 2)
        GpioSetLevel(LIB_TY_SW_OUT_PIN2, transconf.initstatus1);
    else if(transconf.sdstatus1 == 1)
        GpioSetLevel(LIB_TY_SW_OUT_PIN2, 1);
    else
        GpioSetLevel(LIB_TY_SW_OUT_PIN2, 0);

    if(transconf.sdstatus2 == 2)
        GpioSetLevel(LIB_TY_SW_OUT_PIN3, transconf.initstatus2);
    else if(transconf.sdstatus2 == 1)
        GpioSetLevel(LIB_TY_SW_OUT_PIN3, 1);
    else
        GpioSetLevel(LIB_TY_SW_OUT_PIN3, 0);

    if(transconf.sdstatus3 == 2)
        GpioSetLevel(LIB_TY_SW_OUT_PIN4, transconf.initstatus3);
    else if(transconf.sdstatus3 == 1)
        GpioSetLevel(LIB_TY_SW_OUT_PIN4, 1);
    else
        GpioSetLevel(LIB_TY_SW_OUT_PIN4, 0);
#endif /* ifdef AM_SW_INTERLOCK_POWERON_KEEP.2024-7-26 11:03:05 by: zhaoning */

}

// End of file : am_ty_interlocking.c 2024-7-17 11:37:29 by: zhaoning 

