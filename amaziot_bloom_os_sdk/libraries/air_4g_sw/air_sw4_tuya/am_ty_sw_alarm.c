//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_alarm.c
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

#include "am_ty_sw_alarm.h"
#include "am_ty_sw_n.h"

#include "lib_common.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static OSATimerRef ty_sw_alarm_ref_alarm = NULL;
static OS_HISR  ty_sw_alarm_hisr_alarm;

// Public variables -------------------------------------------------------------

extern OSMsgQRef am_ty_sw_alarm_msgq;

// Private functions prototypes -------------------------------------------------

static void lib_power_outage_alarm_cbk(UINT32 tmrId);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : lib_power_alarm_irq_handler_alarm
  * Description : 产生hisr。在gpio初始化是注册到gpio结构体中。
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_power_alarm_irq_handler_alarm(void)
{
    OS_Activate_HISR(&ty_sw_alarm_hisr_alarm);
}

/**
  * Function    : lib_power_outage_alarm_handler
  * Description : gpio中断回调函数，启动搞一个定时器，去抖用
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_power_outage_alarm_handler(void)
{
    OSATimerStop(ty_sw_alarm_ref_alarm);
    OSATimerStart(ty_sw_alarm_ref_alarm, 40, 0, lib_power_outage_alarm_cbk, 0); 
    
}

/**
  * Function    : lib_power_outage_alarm_cbk
  * Description : gpio检测断电回调函数，发送消息给上报队列，上报服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void lib_power_outage_alarm_cbk(UINT32 tmrId)
{
    lib_uart_printf("lib_power_outage_alarm_cbk");
    
    if(GpioGetLevel(AM_TY_SW_OG_ALARM) == 0)
    {
        //检测到状态变化后，这里进行相应的处理
        //4G空开 DTU 中，这里会发送消息给上报队列
        //libraries/8.2 示例中，仅接收，不上报
        app_ty_sw_msgq_msg alarmMsg = {0};

        alarmMsg.msgId = TY_SW_SEND_TASK_MSG_ALARM_MSG;
        app_ty_sw_send_msgq(&alarmMsg);
    }
}

/**
  * Function    : lib_power_outage_alarm_init
  * Description : 断电报警初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_power_outage_alarm_init(void)
{
    int ret = 0;
    
    GPIOConfiguration config;

    memset(&config, 0, sizeof(GPIOConfiguration));
    //init qudou
    ret = OSATimerCreate(&ty_sw_alarm_ref_alarm);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSAFlagCreate( &GpioFlgRef_alarm); 
//    ASSERT(ret == OS_SUCCESS);
    Os_Create_HISR(&ty_sw_alarm_hisr_alarm, "Gpio_Hisr", lib_power_outage_alarm_handler, 2);
        
    //init in gpio
    config.pinDir = GPIO_IN_PIN;
//    config.initLv = 1;
    config.pinEd = GPIO_FALL_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
    config.isr = lib_power_alarm_irq_handler_alarm;
    GpioInitConfiguration(AM_TY_SW_OG_ALARM,config);

}

// End of file : am_ty_alarm.c 2024-7-17 11:06:58 by: zhaoning 

