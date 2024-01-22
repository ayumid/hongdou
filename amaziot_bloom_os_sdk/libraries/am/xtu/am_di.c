//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_di.c
// Auther      : zhaoning
// Version     :
// Date : 2023-8-29
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-29
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_di.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static OSATimerRef dtu_di1_report_timer_ref = NULL;
static OSATimerRef dtu_di2_report_timer_ref = NULL;
static OSATimerRef dtu_di3_report_timer_ref = NULL;
static OSATimerRef dtu_di4_report_timer_ref = NULL;
static OSATimerRef dtu_di5_report_timer_ref = NULL;

static OSATimerRef _dtu_di1_int_timer_ref1 = NULL;
static OSATimerRef _dtu_di2_int_timer_ref2 = NULL;
static OSATimerRef _dtu_di3_int_timer_ref3 = NULL;
static OSATimerRef _dtu_di4_int_timer_ref4 = NULL;
static OSATimerRef _dtu_di5_int_timer_ref5 = NULL;

static OS_HISR  dtu_gpio_hisr1;
static OS_HISR  dtu_gpio_hisr2;
static OS_HISR  dtu_gpio_hisr3;
static OS_HISR  dtu_gpio_hisr4;
static OS_HISR  dtu_gpio_hisr5;

//static UINT32  dtu_elink_time1 = 0;
//static UINT32  dtu_elink_time2 = 0;
//static UINT32  dtu_elink_time3 = 0;
//static UINT32  dtu_elink_time4 = 0;
//static UINT32  dtu_elink_time5 = 0;

//static UINT8   lastKeyStatus0 = 0;
//static UINT8   lastKeyStatus1 = 0;
//static UINT8   lastKeyStatus2 = 0;
//static UINT8   lastKeyStatus3 = 0;
//static UINT8   lastKeyStatus4 = 0;

//OSFlagRef  dtu_gpio_flg_ref = NULL;//gpio status up flag

//OSATimerRef _pulsetimer_ref0 = NULL;
//OSATimerRef _pulsetimer_ref1 = NULL;
//OSATimerRef _pulsetimer_ref2 = NULL;
//OSATimerRef _pulsetimer_ref3 = NULL;
//OSATimerRef _pulsetimer_ref4 = NULL;

//OSFlagRef  PluseFlgRef0 = NULL;
//OSFlagRef  PluseFlgRef1 = NULL;
//OSFlagRef  PluseFlgRef2 = NULL;
//OSFlagRef  PluseFlgRef3 = NULL;
//OSFlagRef  PluseFlgRef4 = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void dtu_gpio_irq_handler1 (void);
static void dtu_gpio_irq_handler2 (void);
static void dtu_gpio_irq_handler3 (void);
static void dtu_gpio_irq_handler4 (void);
static void dtu_gpio_irq_handler5 (void);

static void dtu_gpio_handler1(void);
static void dtu_gpio_handler2(void);
static void dtu_gpio_handler3(void);
static void dtu_gpio_handler4(void);
static void dtu_gpio_handler5(void);

static void dtu_gpio_timer_cbk1(UINT32 tmrId);
static void dtu_gpio_timer_cbk2(UINT32 tmrId);
static void dtu_gpio_timer_cbk3(UINT32 tmrId);
static void dtu_gpio_timer_cbk4(UINT32 tmrId);
static void dtu_gpio_timer_cbk5(UINT32 tmrId);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_di1_report_timer_callback
  * Description : DI1周期上报定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di1_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T di_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_di1_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        di_send_msg.msgId = DTU_DI_MSG_ID_DI_PROACTIVE_REPORT;
        di_send_msg.channel = DTU_GPIO_CHNNEL_1;
        di_send_msg.status = GpioGetLevel(DTU_GPIO_DI_PIN1);
        //发送消息给json任务
        dtu_json_task_send_msgq(&di_send_msg);
    }
}

/**
  * Function    : dtu_di2_report_timer_callback
  * Description : DI2周期上报定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di2_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T di_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_di2_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        di_send_msg.msgId = DTU_DI_MSG_ID_DI_PROACTIVE_REPORT;
        di_send_msg.channel = DTU_GPIO_CHNNEL_2;
        di_send_msg.status = GpioGetLevel(DTU_GPIO_DI_PIN2);
        //发送消息给json任务
        dtu_json_task_send_msgq(&di_send_msg);
    }
}

/**
  * Function    : dtu_di3_report_timer_callback
  * Description : DI3周期上报定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di3_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T di_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_di3_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        di_send_msg.msgId = DTU_DI_MSG_ID_DI_PROACTIVE_REPORT;
        di_send_msg.channel = DTU_GPIO_CHNNEL_3;
        di_send_msg.status = GpioGetLevel(DTU_GPIO_DI_PIN3);
        //发送消息给json任务
        dtu_json_task_send_msgq(&di_send_msg);
    }
}

/**
  * Function    : dtu_di4_report_timer_callback
  * Description : DI4周期上报定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di4_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T di_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_di4_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        di_send_msg.msgId = DTU_DI_MSG_ID_DI_PROACTIVE_REPORT;
        di_send_msg.channel = DTU_GPIO_CHNNEL_4;
        di_send_msg.status = GpioGetLevel(DTU_GPIO_DI_PIN4);
        //发送消息给json任务
        dtu_json_task_send_msgq(&di_send_msg);
    }
}

/**
  * Function    : dtu_di5_report_timer_callback
  * Description : DI5周期上报定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di5_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T di_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_di5_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        di_send_msg.msgId = DTU_DI_MSG_ID_DI_PROACTIVE_REPORT;
        di_send_msg.channel = DTU_GPIO_CHNNEL_5;
        di_send_msg.status = GpioGetLevel(DTU_GPIO_DI_PIN5);
        //发送消息给json任务
        dtu_json_task_send_msgq(&di_send_msg);
    }
}

/**
  * Function    : dtu_di_report_timer_stop
  * Description : 停止主动上报定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_report_timer_stop(UINT8 index)
{
    //关闭定时器
    if(DTU_TIME_INDEX_1 == index)
    {
        OSATimerStop(dtu_di1_report_timer_ref);
    }
    else if(DTU_TIME_INDEX_2 == index)
    {
        OSATimerStop(dtu_di2_report_timer_ref);
    }
    else if(DTU_TIME_INDEX_3 == index)
    {
        OSATimerStop(dtu_di3_report_timer_ref);
    }
    else if(DTU_TIME_INDEX_4 == index)
    {
        OSATimerStop(dtu_di4_report_timer_ref);
    }
    else if(DTU_TIME_INDEX_5 == index)
    {
        OSATimerStop(dtu_di5_report_timer_ref);
    }

}

/**
  * Function    : dtu_do_report_timer_start
  * Description : 主动上报定时器启动
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_report_timer_start(UINT8 index, UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

//    uprintf("start di timer %d %d\n", index, uc_time);
    //开启定时器
    if(DTU_TIME_INDEX_1 == index)
    {
        OSATimerStop(dtu_di1_report_timer_ref);
        OSATimerStart(dtu_di1_report_timer_ref, uc_time * 200, uc_time * 200, dtu_di1_report_timer_callback, 0);
    }
    else if(DTU_TIME_INDEX_2 == index)
    {
        OSATimerStop(dtu_di2_report_timer_ref);
        OSATimerStart(dtu_di2_report_timer_ref, uc_time * 200, uc_time * 200, dtu_di2_report_timer_callback, 0);
    }
    else if(DTU_TIME_INDEX_3 == index)
    {
        OSATimerStop(dtu_di3_report_timer_ref);
        OSATimerStart(dtu_di3_report_timer_ref, uc_time * 200, uc_time * 200, dtu_di3_report_timer_callback, 0);
    }
    else if(DTU_TIME_INDEX_4 == index)
    {
        OSATimerStop(dtu_di4_report_timer_ref);
        OSATimerStart(dtu_di4_report_timer_ref, uc_time * 200, uc_time * 200, dtu_di4_report_timer_callback, 0);
    }
    else if(DTU_TIME_INDEX_5 == index)
    {
        OSATimerStop(dtu_di5_report_timer_ref);
        OSATimerStart(dtu_di5_report_timer_ref, uc_time * 200, uc_time * 200, dtu_di5_report_timer_callback, 0);
    }
    
}

/**
  * Function    : dtu_di_int_init1
  * Description : 初始化DI1，配置为中断模式，判断NV项，是否需要打开周期上报定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_int_init1(void)
{
    int ret = 0;
    GPIOConfiguration config;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    memset(&config, 0, sizeof(GPIOConfiguration));

    //init qudou
    ret = OSATimerCreate(&_dtu_di1_int_timer_ref1);
    ASSERT(ret == OS_SUCCESS);

    Os_Create_HISR(&dtu_gpio_hisr1, "Gpio_Hisr", dtu_gpio_handler1, 2);

    //init in gpio
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;

    config.pinPull = GPIO_PULLDN_ENABLE;
//    uprintf("%s[%d] DI1 pulldown", __FUNCTION__, __LINE__);

    config.isr = dtu_gpio_irq_handler1;
    GpioInitConfiguration(DTU_GPIO_DI_PIN1,config);

    //判断是否需要打开定时上报
    if(DTU_DI_REPORT_INITIATIVE == dtu_file_ctx->di.params[DTU_BUF_INDEX_1].type)
    {
        //打开定时器
        dtu_di_report_timer_start(DTU_TIME_INDEX_1, dtu_file_ctx->di.params[DTU_BUF_INDEX_1].interval);
    }

}

void dtu_di_int_init2(void)
{
    int ret = 0;
    GPIOConfiguration config;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    memset(&config, 0, sizeof(GPIOConfiguration));

    //init qudou
    ret = OSATimerCreate(&_dtu_di2_int_timer_ref2);
    ASSERT(ret == OS_SUCCESS);

    Os_Create_HISR(&dtu_gpio_hisr2, "Gpio_Hisr", dtu_gpio_handler2, 2);

    //init in gpio
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;

    config.pinPull = GPIO_PULLDN_ENABLE;
//    uprintf("%s[%d] DI2 pulldown", __FUNCTION__, __LINE__);

    config.isr = dtu_gpio_irq_handler2;
    GpioInitConfiguration(DTU_GPIO_DI_PIN2,config);

    //判断是否需要打开定时上报
    if(DTU_DI_REPORT_INITIATIVE == dtu_file_ctx->di.params[DTU_BUF_INDEX_2].type)
    {
        //打开定时器
        dtu_di_report_timer_start(DTU_TIME_INDEX_2, dtu_file_ctx->di.params[DTU_BUF_INDEX_2].interval);
    }

}

/**
  * Function    : dtu_di_int_init3
  * Description : 初始化DI3
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_int_init3(void)
{
    int ret = 0;
    GPIOConfiguration config;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    memset(&config, 0, sizeof(GPIOConfiguration));

    //init qudou
    ret = OSATimerCreate(&_dtu_di3_int_timer_ref3);
    ASSERT(ret == OS_SUCCESS);

    Os_Create_HISR(&dtu_gpio_hisr3, "Gpio_Hisr", dtu_gpio_handler3, 2);

    //init in gpio
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;

    config.pinPull = GPIO_PULLDN_ENABLE;
//    uprintf("%s[%d] DI3 pulldown", __FUNCTION__, __LINE__);

    config.isr = dtu_gpio_irq_handler3;
    GpioInitConfiguration(DTU_GPIO_DI_PIN3,config);

    //判断是否需要打开定时上报
    if(DTU_DI_REPORT_INITIATIVE == dtu_file_ctx->di.params[DTU_BUF_INDEX_3].type)
    {
        //打开定时器
        dtu_di_report_timer_start(DTU_TIME_INDEX_3, dtu_file_ctx->di.params[DTU_BUF_INDEX_3].interval);
    }

}

void dtu_di_int_init4(void)
{
    int ret = 0;
    GPIOConfiguration config;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    memset(&config, 0, sizeof(GPIOConfiguration));

    //init qudou
    ret = OSATimerCreate(&_dtu_di4_int_timer_ref4);
    ASSERT(ret == OS_SUCCESS);

    Os_Create_HISR(&dtu_gpio_hisr4, "Gpio_Hisr", dtu_gpio_handler4, 2);

    //init in gpio
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;
    
    config.pinPull = GPIO_PULLDN_ENABLE;
//    uprintf("%s[%d] DI4 pulldown", __FUNCTION__, __LINE__);

    config.isr = dtu_gpio_irq_handler4;
    GpioInitConfiguration(DTU_GPIO_DI_PIN4,config);

    //判断是否需要打开定时上报
    if(DTU_DI_REPORT_INITIATIVE == dtu_file_ctx->di.params[DTU_BUF_INDEX_4].type)
    {
        //打开定时器
        dtu_di_report_timer_start(DTU_TIME_INDEX_4, dtu_file_ctx->di.params[DTU_BUF_INDEX_4].interval);
    }

}

void dtu_di_int_init5(void)
{
    int ret = 0;
    GPIOConfiguration config;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    memset(&config, 0, sizeof(GPIOConfiguration));

    //init qudou
    ret = OSATimerCreate(&_dtu_di5_int_timer_ref5);
    ASSERT(ret == OS_SUCCESS);

    Os_Create_HISR(&dtu_gpio_hisr5, "Gpio_Hisr", dtu_gpio_handler5, 2);

    //init in gpio
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_TWO_EDGE;

    config.pinPull = GPIO_PULLDN_ENABLE;
//    uprintf("%s[%d] DI5 pulldown", __FUNCTION__, __LINE__);

    config.isr = dtu_gpio_irq_handler5;
    GpioInitConfiguration(DTU_GPIO_DI_PIN5,config);

    //判断是否需要打开定时上报
    if(DTU_DI_REPORT_INITIATIVE == dtu_file_ctx->di.params[DTU_BUF_INDEX_5].type)
    {
        //打开定时器
        dtu_di_report_timer_start(DTU_TIME_INDEX_5, dtu_file_ctx->di.params[DTU_BUF_INDEX_5].interval);
    }

}

/**
  * Function    : dtu_di_int_init_all
  * Description : 初始化所有的DI
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_int_init_all(void)
{
    dtu_di_int_init1();
    dtu_di_int_init2();
    dtu_di_int_init3();
    dtu_di_int_init4();
    dtu_di_int_init5();
}

/**
  * Function    : dtu_di_read_pin
  * Description : 读DI电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT8 dtu_di_read_pin(UINT8 channel)
{
    return GpioGetLevel(channel);
}

static void dtu_gpio_irq_handler1 (void)
{
    OS_Activate_HISR(&dtu_gpio_hisr1);
}

static void dtu_gpio_irq_handler2 (void)
{
    OS_Activate_HISR(&dtu_gpio_hisr2);
}

static void dtu_gpio_irq_handler3 (void)
{
    OS_Activate_HISR(&dtu_gpio_hisr3);
}

static void dtu_gpio_irq_handler4 (void)
{
    OS_Activate_HISR(&dtu_gpio_hisr4);
}

static void dtu_gpio_irq_handler5 (void)
{
    OS_Activate_HISR(&dtu_gpio_hisr5);
}

static void dtu_gpio_handler1 (void)
{
    OSATimerStop(_dtu_di1_int_timer_ref1);
    OSATimerStart(_dtu_di1_int_timer_ref1, 2, 0, dtu_gpio_timer_cbk1, 0); 
    
}
static void dtu_gpio_handler2 (void)
{
    OSATimerStop(_dtu_di2_int_timer_ref2);
    OSATimerStart(_dtu_di2_int_timer_ref2, 2, 0, dtu_gpio_timer_cbk2, 0); 
    
}
static void dtu_gpio_handler3 (void)
{
    OSATimerStop(_dtu_di3_int_timer_ref3);
    OSATimerStart(_dtu_di3_int_timer_ref3, 2, 0, dtu_gpio_timer_cbk3, 0); 
    
}
static void dtu_gpio_handler4 (void)
{
    OSATimerStop(_dtu_di4_int_timer_ref4);
    OSATimerStart(_dtu_di4_int_timer_ref4, 2, 0, dtu_gpio_timer_cbk4, 0); 
    
}
static void dtu_gpio_handler5(void)
{
    OSATimerStop(_dtu_di5_int_timer_ref5);
    OSATimerStart(_dtu_di5_int_timer_ref5, 2, 0, dtu_gpio_timer_cbk5, 0); 
    
}

/**
  * Function    : dtu_gpio_timer_cbk1
  * Description : DI1中断发生延时检测定时器回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_gpio_timer_cbk1(UINT32 tmrId)
{
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    p_smsg.msgId = DTU_DI_MSG_ID_DI_TRIGGER_REPORT;
    p_smsg.channel = DTU_GPIO_CHNNEL_1;
    p_smsg.status = GpioGetLevel(DTU_GPIO_DI_PIN1);

    if(DTU_DO_FLOW_DI_FORWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_1])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_1, p_smsg.status);
    }
    else if(DTU_DO_FLOW_DI_BACKWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_1])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_1, ~p_smsg.status);
    }
    uprintf("%s PIN1 status: %d \n", __FUNCTION__, p_smsg.status);
    if(DTU_DI_REPORT_TRIGGER == dtu_file_ctx->di.params[DTU_BUF_INDEX_1].type)
    {
        if(p_smsg.status == dtu_file_ctx->di.params[DTU_BUF_INDEX_1].edge)
        {
            dtu_json_task_send_msgq(&p_smsg);
        }
    }
    
}

/**
  * Function    : dtu_gpio_timer_cbk2
  * Description : DI2中断发生延时检测定时器回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_gpio_timer_cbk2(UINT32 tmrId)
{
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    p_smsg.msgId = DTU_DI_MSG_ID_DI_TRIGGER_REPORT;
    p_smsg.channel = DTU_GPIO_CHNNEL_2;
    p_smsg.status = GpioGetLevel(DTU_GPIO_DI_PIN2);

    if(DTU_DO_FLOW_DI_FORWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_2])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_2, p_smsg.status);
    }
    else if(DTU_DO_FLOW_DI_BACKWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_2])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_2, ~p_smsg.status);
    }
    uprintf("%s PIN2 status: %d \n", __FUNCTION__, p_smsg.status);
    if(DTU_DI_REPORT_TRIGGER == dtu_file_ctx->di.params[DTU_BUF_INDEX_2].type)
    {
        if(p_smsg.status == dtu_file_ctx->di.params[DTU_BUF_INDEX_3].edge)
        {
            dtu_json_task_send_msgq(&p_smsg);
        }
    }

}

/**
  * Function    : dtu_gpio_timer_cbk3
  * Description : DI3中断发生延时检测定时器回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_gpio_timer_cbk3(UINT32 tmrId)
{
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    p_smsg.msgId = DTU_DI_MSG_ID_DI_TRIGGER_REPORT;
    p_smsg.channel = DTU_GPIO_CHNNEL_3;
    p_smsg.status = GpioGetLevel(DTU_GPIO_DI_PIN3);

    if(DTU_DO_FLOW_DI_FORWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_3])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_3, p_smsg.status);
    }
    else if(DTU_DO_FLOW_DI_BACKWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_3])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_3, ~p_smsg.status);
    }
    uprintf("%s PIN3 status: %d \n", __FUNCTION__, p_smsg.status);
    if(DTU_DI_REPORT_TRIGGER == dtu_file_ctx->di.params[DTU_BUF_INDEX_3].type)
    {
        if(p_smsg.status == dtu_file_ctx->di.params[DTU_BUF_INDEX_3].edge)
        {
            dtu_json_task_send_msgq(&p_smsg);
        }
    }

}

/**
  * Function    : dtu_gpio_timer_cbk4
  * Description : DI4中断发生延时检测定时器回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_gpio_timer_cbk4(UINT32 tmrId)
{
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    p_smsg.msgId = DTU_DI_MSG_ID_DI_TRIGGER_REPORT;
    p_smsg.channel = DTU_GPIO_CHNNEL_4;
    p_smsg.status = GpioGetLevel(DTU_GPIO_DI_PIN4);

    if(DTU_DO_FLOW_DI_FORWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_4])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_4, p_smsg.status);
    }
    else if(DTU_DO_FLOW_DI_BACKWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_4])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_4, ~p_smsg.status);
    }
    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    if(DTU_DI_REPORT_TRIGGER == dtu_file_ctx->di.params[DTU_BUF_INDEX_4].type)
    {
        if(p_smsg.status == dtu_file_ctx->di.params[DTU_BUF_INDEX_4].edge)
        {
            dtu_json_task_send_msgq(&p_smsg);
        }
    }

}

/**
  * Function    : dtu_gpio_timer_cbk5
  * Description : DI5中断发生延时检测定时器回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_gpio_timer_cbk5(UINT32 tmrId)
{
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    p_smsg.msgId = DTU_DI_MSG_ID_DI_TRIGGER_REPORT;
    p_smsg.channel = DTU_GPIO_CHNNEL_5;
    p_smsg.status = GpioGetLevel(DTU_GPIO_DI_PIN5);

    if(DTU_DO_FLOW_DI_FORWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_5])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_5, p_smsg.status);
    }
    else if(DTU_DO_FLOW_DI_BACKWARD == dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_5])
    {
        dtu_do_write_pin(DTU_GPIO_CHNNEL_5, ~p_smsg.status);
    }
    uprintf("%s PIN5 status: %d \n", __FUNCTION__, p_smsg.status);
    if(DTU_DI_REPORT_TRIGGER == dtu_file_ctx->di.params[DTU_BUF_INDEX_5].type)
    {
        if(p_smsg.status == dtu_file_ctx->di.params[DTU_BUF_INDEX_5].edge)
        {
            dtu_json_task_send_msgq(&p_smsg);
        }
    }
}

/**
  * Function    : dtu_di_times_init
  * Description : 主动上报定时器句柄初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_times_init(void)
{
    int ret = 0;

    ret = OSATimerCreate(&dtu_di1_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_di2_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_di3_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_di4_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_di5_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
}

// End of file : am_di.c 2023-8-29 10:11:12 by: zhaoning 

