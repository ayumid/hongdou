//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_ty_sw.c
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
//#include "am_file.h"
#include "am_ty_sw_.h"
//#include "am_common.h"
#include "utils_common.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

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

extern OSMsgQRef    send_msgq;

trans_conf transconf;//weihu yi ge save in flash

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

// Functions --------------------------------------------------------------------

int send_task_send_msgq(app_msgq_msg * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(send_msgq, SEND_MSGQ_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        printf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgId, status);
        ret = -1;
    }    
    
    return ret;
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

void init_gpio_conf(void)
{
    //init out gpio
//    init_out_gpio();
    //init key gpio
    init_key_gpio();
    //init alarm gpio
//    init_alarm_gpio();
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

static void _gpiotimer_callback0(UINT32 tmrId)
{
    printf("_gpiotimer_callback0");
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
            printf("diff0:%d,elink_time0:%d",diff0,elink_time0);
            if(diff0 > 1000)
            {
                printf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff0 > 20)
            {
                //20201022
                printf("change gpio0 status");
                //oc gpio
//                if(transconf.lockstatus == 1)
//                    gpio0_level_flip_lock();
//                else
//                    gpio_level_flip(GPIO_CLR_PIN0,PluseFlgRef0,_pulsetimer_ref0); 
                
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
    printf("_gpiotimer_callback1");
    
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
            printf("diff1:%d,elink_time1:%d",diff1,elink_time1);
            if(diff1 > 1000)
            {
                printf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff1 > 20)
            {//20201022
                printf("change gpio1 status");
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
    printf("_gpiotimer_callback2");
    
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
            printf("diff2:%d,elink_time2:%d",diff2,elink_time2);
            if(diff2 > 1000)
            {
                printf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff2 > 20)
            {
                //20201022
                printf("change gpio2 status");
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
    printf("_gpiotimer_callback3");
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
            printf("diff3:%d,elink_time3:%d",diff3,elink_time3);
            if(diff3 > 1000)
            {
                printf("activeFlag=1");
                transconf.activeFlag = 1;
            }
            else if(diff3 > 20)
            {
                //20201022
                printf("change gpio3 status");
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

// End of file : am_ty_sw.h 2023-3-18 17:12:57 by: zhaoning 

