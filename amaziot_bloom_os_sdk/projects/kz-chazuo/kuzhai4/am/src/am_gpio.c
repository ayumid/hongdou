//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_gpio.c
// Auther      : zhaoning
// Version     :
// Date : 2023-3-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-3-18
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "osa.h"
#include "cgpio.h"
#include "am_file.h"
#include "am_gpio.h"
#include "am_common.h"
#include "utils_common.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static OSATimerRef _ledtimer_ref = NULL;

static OSATimerRef _gpiotimer_ref0 = NULL;
static OSATimerRef _gpiotimer_ref1 = NULL;
static OSATimerRef _gpiotimer_ref2 = NULL;
static OSATimerRef _gpiotimer_ref3 = NULL;
static OSATimerRef _gpiotimer_ref_alarm = NULL;

static OS_HISR  Gpio_Hisr0;
static OS_HISR  Gpio_Hisr1;
static OS_HISR  Gpio_Hisr2;
static OS_HISR  Gpio_Hisr3;
static OS_HISR  Gpio_Hisr_alarm;

static UINT32  elink_time0 = 0;
static UINT32  elink_time1 = 0;
static UINT32  elink_time2 = 0;
static UINT32  elink_time3 = 0;

static UINT8   lastKeyStatus0 = 0;
static UINT8   lastKeyStatus1 = 0;
static UINT8   lastKeyStatus2 = 0;
static UINT8   lastKeyStatus3 = 0;

// Public variables -------------------------------------------------------------

UINT8 MoudleLedStatus = 0;

//OSFlagRef  GpioFlgRef = NULL;//gpio status up flag
//OSFlagRef  GpioFlgRef_alarm = NULL;//alarm status up flag

OSATimerRef _pulsetimer_ref0 = NULL;
OSATimerRef _pulsetimer_ref1 = NULL;
OSATimerRef _pulsetimer_ref2 = NULL;
OSATimerRef _pulsetimer_ref3 = NULL;

OSFlagRef  PluseFlgRef0 = NULL;
OSFlagRef  PluseFlgRef1 = NULL;
OSFlagRef  PluseFlgRef2 = NULL;
OSFlagRef  PluseFlgRef3 = NULL;

extern trans_conf transconf;//weihu yi ge save in flash

// functions prototypes -------------------------------------------------

static void GPIOIRQHandler0 (void);
static void GPIOIRQHandler1 (void);
static void GPIOIRQHandler2 (void);
static void GPIOIRQHandler3 (void);
static void GPIOIRQHandler_alarm (void);

static void Gpio_Handler0(void);
static void Gpio_Handler1(void);
static void Gpio_Handler2(void);
static void Gpio_Handler3(void);
static void Gpio_Handler_alarm(void);

static void _gpiotimer_callback0(UINT32 tmrId);
static void _gpiotimer_callback1(UINT32 tmrId);
static void _gpiotimer_callback2(UINT32 tmrId);
static void _gpiotimer_callback3(UINT32 tmrId);
static void _gpiotimer_callback_alarm(UINT32 tmrId);

static void led_level_flip(void);
static void gpio_level_flip(UINT32 portHandle,OSFlagRef OsFlagRef,OSATimerRef OsaTimeRef);
static void gpio0_level_flip_lock(void);
static void gpio1_level_flip_lock(void);
static void gpio2_level_flip_lock(void);
static void gpio3_level_flip_lock(void);

static void _ledtimer_callback(UINT32 tmrId);

// Functions --------------------------------------------------------------------

static void led_level_flip(void)  
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

void gpio_open(UINT32 portHandle, OSFlagRef OsFlagRef)
{
    GpioSetLevel(portHandle,1);
    //pluse open
    OSAFlagSet(OsFlagRef, 0x01, OSA_FLAG_OR);
}

void gpio_close(UINT32 portHandle, OSATimerRef OsaTimeRef)
{
    GpioSetLevel(portHandle,0);
    //pluse timer close
    OSATimerStop(OsaTimeRef);
}

static void gpio_level_flip(UINT32 portHandle,OSFlagRef OsFlagRef,OSATimerRef OsaTimeRef)  
{
    if(GpioGetLevel(portHandle) == 0)
    {
      gpio_open(portHandle,OsFlagRef);    
    }
    else if(GpioGetLevel(portHandle) == 1)
    {
      gpio_close(portHandle,OsaTimeRef);
    }    
}

static void gpio0_level_flip_lock(void)  
{
    if(GpioGetLevel(GPIO_CLR_PIN0) == 0)
    {
      gpio_open(GPIO_CLR_PIN0,PluseFlgRef0);
      gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
      gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);  
      gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);  
    }
    else if(GpioGetLevel(GPIO_CLR_PIN0) == 1)
    {
      gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);               
    }    
}

static void gpio1_level_flip_lock(void)  
{
    if(GpioGetLevel(GPIO_CLR_PIN1) == 0)
    {
      gpio_open(GPIO_CLR_PIN1,PluseFlgRef1);
      gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
      gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);  
      gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);  
    }
    else if(GpioGetLevel(GPIO_CLR_PIN1) == 1)
    {
      gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);               
    }    
}

static void gpio2_level_flip_lock(void)  
{
    if(GpioGetLevel(GPIO_CLR_PIN2) == 0)
    {
      gpio_open(GPIO_CLR_PIN2,PluseFlgRef2);
      gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
      gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);  
      gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);  
    }
    else if(GpioGetLevel(GPIO_CLR_PIN2) == 1)
    {
      gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);               
    }    
}

static void gpio3_level_flip_lock(void)  
{
    if(GpioGetLevel(GPIO_CLR_PIN3) == 0)
    {
      gpio_open(GPIO_CLR_PIN3,PluseFlgRef3);
      gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
      gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);  
      gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);  
    }
    else if(GpioGetLevel(GPIO_CLR_PIN3) == 1)
    {
      gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);               
    }    
}

/***********************************************************************
*    PLUSE TIME RELATED DEFINE
***********************************************************************/
void init_led_status(void)
{
    int ret = 0;

    ret = GpioSetDirection(GPIO_LED_STATUS, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    
    ret = OSATimerCreate(&_ledtimer_ref);
    ASSERT(ret == OS_SUCCESS);
    
    OSATimerStart(_ledtimer_ref, 20, 20, _ledtimer_callback, 0); //100ms
}

static void init_out_gpio(void)
{
    int ret = 0;
    //init out gpio
    ret = GpioSetDirection(GPIO_CLR_PIN0, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(GPIO_CLR_PIN1, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(GPIO_CLR_PIN2, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(GPIO_CLR_PIN3, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);

    //init out status
    if(transconf.sdstatus0 == 2)
        GpioSetLevel(GPIO_CLR_PIN0, transconf.initstatus0);
    else if(transconf.sdstatus0 == 1)
        GpioSetLevel(GPIO_CLR_PIN0, 1);
    else
        GpioSetLevel(GPIO_CLR_PIN0, 0);

    if(transconf.sdstatus1 == 2)
        GpioSetLevel(GPIO_CLR_PIN1, transconf.initstatus1);
    else if(transconf.sdstatus1 == 1)
        GpioSetLevel(GPIO_CLR_PIN1, 1);
    else
        GpioSetLevel(GPIO_CLR_PIN1, 0);

    if(transconf.sdstatus2 == 2)
        GpioSetLevel(GPIO_CLR_PIN2, transconf.initstatus2);
    else if(transconf.sdstatus2 == 1)
        GpioSetLevel(GPIO_CLR_PIN2, 1);
    else
        GpioSetLevel(GPIO_CLR_PIN2, 0);

    if(transconf.sdstatus3 == 2)
        GpioSetLevel(GPIO_CLR_PIN3, transconf.initstatus3);
    else if(transconf.sdstatus3 == 1)
        GpioSetLevel(GPIO_CLR_PIN3, 1);
    else
        GpioSetLevel(GPIO_CLR_PIN3, 0);

}

static void init_key_gpio(void)
{
    int ret = 0;
    
    GPIOConfiguration config;

    memset(&config, 0, sizeof(GPIOConfiguration));
    
    lastKeyStatus0 = 1;    //20201016
    lastKeyStatus1 = 1;    //20201016
    lastKeyStatus2 = 1;    //20201016
    lastKeyStatus3 = 1;    //20201016

    //init qudou
    ret = OSATimerCreate(&_gpiotimer_ref0);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&_gpiotimer_ref1);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&_gpiotimer_ref2);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&_gpiotimer_ref3);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSAFlagCreate( &GpioFlgRef); 
//    ASSERT(ret == OS_SUCCESS);
    Os_Create_HISR(&Gpio_Hisr0, "Gpio_Hisr", Gpio_Handler0, 2);
    Os_Create_HISR(&Gpio_Hisr1, "Gpio_Hisr", Gpio_Handler1, 2);
    Os_Create_HISR(&Gpio_Hisr2, "Gpio_Hisr", Gpio_Handler2, 2);
    Os_Create_HISR(&Gpio_Hisr3, "Gpio_Hisr", Gpio_Handler3, 2);
    
    //init in gpio
    config.pinDir = GPIO_IN_PIN;
//    config.initLv = 1;
    config.pinEd = GPIO_TWO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
    
    config.isr = GPIOIRQHandler0;
    GpioInitConfiguration(GPIO_KEY_PIN0,config);
    config.isr = GPIOIRQHandler1;
    GpioInitConfiguration(GPIO_KEY_PIN1,config);
    config.isr = GPIOIRQHandler2;
    GpioInitConfiguration(GPIO_KEY_PIN2,config);
    config.isr = GPIOIRQHandler3;
    GpioInitConfiguration(GPIO_KEY_PIN3,config);

}

static void init_alarm_gpio(void)
{
    int ret = 0;
    
    GPIOConfiguration config;

    memset(&config, 0, sizeof(GPIOConfiguration));
    //init qudou
    ret = OSATimerCreate(&_gpiotimer_ref_alarm);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSAFlagCreate( &GpioFlgRef_alarm); 
//    ASSERT(ret == OS_SUCCESS);
    Os_Create_HISR(&Gpio_Hisr_alarm, "Gpio_Hisr", Gpio_Handler_alarm, 2);
        
    //init in gpio
    config.pinDir = GPIO_IN_PIN;
//    config.initLv = 1;
    config.pinEd = GPIO_FALL_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
    config.isr = GPIOIRQHandler_alarm;
    GpioInitConfiguration(GPIO_DD_ALARM,config);

}

void init_gpio_conf(void)
{
    //init out gpio
    init_out_gpio();
    //init key gpio
    init_key_gpio();
    //init alarm gpio
    init_alarm_gpio();
}

static void GPIOIRQHandler0 (void)
{
    OS_Activate_HISR(&Gpio_Hisr0);
}

static void GPIOIRQHandler1 (void)
{
    OS_Activate_HISR(&Gpio_Hisr1);
}

static void GPIOIRQHandler2 (void)
{
    OS_Activate_HISR(&Gpio_Hisr2);
}

static void GPIOIRQHandler3 (void)
{
    OS_Activate_HISR(&Gpio_Hisr3);
}

static void GPIOIRQHandler_alarm (void)
{
    OS_Activate_HISR(&Gpio_Hisr_alarm);
}

static void Gpio_Handler0 (void)
{
    OSATimerStop(_gpiotimer_ref0);
    OSATimerStart(_gpiotimer_ref0, 2, 0, _gpiotimer_callback0, 0); 
    
}
static void Gpio_Handler1 (void)
{
    OSATimerStop(_gpiotimer_ref1);
    OSATimerStart(_gpiotimer_ref1, 2, 0, _gpiotimer_callback1, 0); 
    
}
static void Gpio_Handler2 (void)
{
    OSATimerStop(_gpiotimer_ref2);
    OSATimerStart(_gpiotimer_ref2, 2, 0, _gpiotimer_callback2, 0); 
    
}
static void Gpio_Handler3 (void)
{
    OSATimerStop(_gpiotimer_ref3);
    OSATimerStart(_gpiotimer_ref3, 2, 0, _gpiotimer_callback3, 0); 
    
}
static void Gpio_Handler_alarm(void)
{
    OSATimerStop(_gpiotimer_ref_alarm);
    OSATimerStart(_gpiotimer_ref_alarm, 40, 0, _gpiotimer_callback_alarm, 0); 
    
}

static void _gpiotimer_callback0(UINT32 tmrId)
{
    cprintf("_gpiotimer_callback0");
    if(GpioGetLevel(GPIO_KEY_PIN0) == 0)
    {
        elink_time0 = OSAGetTicks();    
        lastKeyStatus0 = 0;//20201016
    }
    else
    {
        if(lastKeyStatus0 == 0)
        {
            //20201016
            UINT32 diff0=OSAGetTicks()-elink_time0;
            cprintf("diff0:%d,elink_time0:%d",diff0,elink_time0);
            if(diff0 > 1000)
            {
                cprintf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff0 > 20)
            {
                //20201022
                cprintf("change gpio0 status");
                //oc gpio
                if(transconf.lockstatus == 1)
                    gpio0_level_flip_lock();
                else
                    gpio_level_flip(GPIO_CLR_PIN0,PluseFlgRef0,_pulsetimer_ref0); 
                
//                OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                app_msgq_msg gpioMsg = {0};

                gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                send_task_send_msgq(&gpioMsg);
            }
        }
        lastKeyStatus0 = 1;    //20201016    
    }
}

static void _gpiotimer_callback1(UINT32 tmrId)
{
    cprintf("_gpiotimer_callback1");
    
    if(GpioGetLevel(GPIO_KEY_PIN1) == 0)
    {
        elink_time1 = OSAGetTicks();    
        lastKeyStatus1 = 0;//20201016
    }
    else
    {
        if(lastKeyStatus1 == 0)
        {
            //20201016
            UINT32 diff1=OSAGetTicks()-elink_time1;
            cprintf("diff1:%d,elink_time1:%d",diff1,elink_time1);
            if(diff1 > 1000)
            {
                cprintf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff1 > 20)
            {//20201022
                cprintf("change gpio1 status");
                //oc gpio
                if(transconf.lockstatus == 1)
                    gpio1_level_flip_lock();
                else
                    gpio_level_flip(GPIO_CLR_PIN1,PluseFlgRef1,_pulsetimer_ref1); 
                
//                OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                app_msgq_msg gpioMsg = {0};

                gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                send_task_send_msgq(&gpioMsg);
            }
        }
        lastKeyStatus1 = 1;    //20201016    
    }
}

static void _gpiotimer_callback2(UINT32 tmrId)
{
    cprintf("_gpiotimer_callback2");
    
    if(GpioGetLevel(GPIO_KEY_PIN2) == 0)
    {
        elink_time2 = OSAGetTicks();    
        lastKeyStatus2 = 0;//20201016
    }
    else
    {
        if(lastKeyStatus2 == 0)
        {
            //20201016
            UINT32 diff2 = OSAGetTicks() - elink_time2;
            cprintf("diff2:%d,elink_time2:%d",diff2,elink_time2);
            if(diff2 > 1000)
            {
                cprintf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff2 > 20)
            {
                //20201022
                cprintf("change gpio2 status");
                //oc gpio
                if(transconf.lockstatus == 1)
                    gpio2_level_flip_lock();
                else
                    gpio_level_flip(GPIO_CLR_PIN2,PluseFlgRef2,_pulsetimer_ref2); 
                
//                OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                app_msgq_msg gpioMsg = {0};

                gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                send_task_send_msgq(&gpioMsg);
            }
        }
        lastKeyStatus2 = 1;    //20201016    
    }
}

static void _gpiotimer_callback3(UINT32 tmrId)
{
    cprintf("_gpiotimer_callback3");
    if(GpioGetLevel(GPIO_KEY_PIN3) == 0)
    {
        elink_time3 = OSAGetTicks();    
        lastKeyStatus3 = 0;//20201016
    }
    else
    {
        if(lastKeyStatus3 == 0)
        {
            //20201016
            UINT32 diff3=OSAGetTicks()-elink_time3;
            cprintf("diff3:%d,elink_time3:%d",diff3,elink_time3);
            if(diff3 > 1000)
            {
                cprintf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff3 > 20)
            {
                //20201022
                cprintf("change gpio3 status");
                //oc gpio
                if(transconf.lockstatus == 1)
                    gpio3_level_flip_lock();
                else
                    gpio_level_flip(GPIO_CLR_PIN3,PluseFlgRef3,_pulsetimer_ref3); 
                
//                OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                app_msgq_msg gpioMsg = {0};

                gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                send_task_send_msgq(&gpioMsg);
            }
        }
        lastKeyStatus3 = 1;    //20201016    
    }
}

static void _gpiotimer_callback_alarm(UINT32 tmrId)
{
    cprintf("_gpiotimer_callback_alarm");
    if(GpioGetLevel(GPIO_DD_ALARM) == 0)
    {
//        OSAFlagSet(GpioFlgRef_alarm, 0x01, OSA_FLAG_OR);
        app_msgq_msg alarmMsg = {0};

        alarmMsg.msgId = SEND_TASK_MSG_ALARM_MSG;
        send_task_send_msgq(&alarmMsg);
    }
}

/*function for led timer*/
static void _ledtimer_callback(UINT32 tmrId)
{
    static UINT8 led_100ms_stamp = 0;
    switch(MoudleLedStatus)
    {
        case NORMAL:
            led_100ms_stamp=0;
            //常亮
            GpioSetLevel(GPIO_LED_STATUS,transconf.sledstatus);
        break;
        case NOGPRS:
            //100ms闪烁1
            led_100ms_stamp++;
            if(led_100ms_stamp == 1)
                GpioSetLevel(GPIO_LED_STATUS,1);
            if(led_100ms_stamp == 2)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp > 20)
            {
                led_100ms_stamp = 0;
            }
        break;
        case NOSERVER:
            //100ms闪烁两次
            led_100ms_stamp++;
            if(led_100ms_stamp == 1)
                GpioSetLevel(GPIO_LED_STATUS,1);
            if(led_100ms_stamp == 2)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 3)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 4)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp > 20)
            {
                led_100ms_stamp = 0;
            }
        break;
        case UNREGISTERED:
            //1s闪烁
            led_100ms_stamp++;
            if(led_100ms_stamp>10)
            {
                led_level_flip();
                led_100ms_stamp = 0;
            }
        break;
        case UPGRADE:
            //100ms闪烁3        
            led_100ms_stamp++;
            if(led_100ms_stamp == 1)
                GpioSetLevel(GPIO_LED_STATUS,1);
            if(led_100ms_stamp == 2)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 3)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 4)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 5)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 6)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp > 20)
            {
                led_100ms_stamp = 0;
            }
        break;
        case NOTLIVE:
            led_100ms_stamp=0;
            //100ms闪烁
            led_level_flip();
        break;
        case FACTORY:
            led_100ms_stamp++;
            if(led_100ms_stamp == 1)
                GpioSetLevel(GPIO_LED_STATUS,0);
            if(led_100ms_stamp == 2)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 3)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 4)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 5)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 6)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 7)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 8)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp == 9)
            {
                GpioSetLevel(GPIO_LED_STATUS,0);
            }
            if(led_100ms_stamp == 10)
            {
                GpioSetLevel(GPIO_LED_STATUS,1);
            }
            if(led_100ms_stamp > 20)
            {
                led_100ms_stamp = 0;
            }
        break;
        default:
        
        break;
    }
}

// End of file : am_gpio.h 2023-3-18 17:12:57 by: zhaoning 

