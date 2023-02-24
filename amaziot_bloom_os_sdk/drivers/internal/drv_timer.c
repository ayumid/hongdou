//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_timer.c
// Auther      : win
// Version     :
// Date : 2021-11-23
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-23
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_timer.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : drv_timer_init
  * Description : 初始化定时器
  * Input       : timerRef 定时器标识
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int drv_timer_init(OSTimerRef* timerRef)
{
    //创建定时器
    return OSATimerCreate(timerRef );
}
/**
  * Function    : drv_timer_start
  * Description : 定时器开始
  * Input       : timerRef：定时器句柄
                  initialTime：初始定时时间
                  rescheduleTime：重载定时时间
                  (*callBackRoutine)(UINT32)：定时器回调函数
                  timerArgc：传递参数
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_timer_start(OSTimerRef timerRef,
               UINT32 initialTime,
               UINT32 rescheduleTime,
               void (*callBackRoutine)(UINT32),
               UINT32 timerArgc )
{
    //开启Timer
   OSATimerStart(timerRef,
               initialTime,
               rescheduleTime,
               callBackRoutine,
               timerArgc 
    ); 
}
/**
  * Function    : drv_timer_stop
  * Description : 定时器停止
  * Input       : timerRef 定时器标识             
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_timer_stop(OSTimerRef timerRef)
{
    //关闭Timer
   OSATimerStop(timerRef );
}

// End of file : drv_timer.h 2021-11-23 18:10:18 by: win 

