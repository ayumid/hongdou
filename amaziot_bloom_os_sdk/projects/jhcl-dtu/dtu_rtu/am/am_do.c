//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_do.c
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

#include "am_do.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static OSATimerRef dtu_do1_report_timer_ref = NULL;//DO1上报定时器
//static OSATimerRef dtu_do1_timer_ref = NULL;//DO1翻转定时器
static OSATimerRef dtu_do2_report_timer_ref = NULL;//DO2上报定时器
//static OSATimerRef dtu_do2_timer_ref = NULL;//DO2翻转定时器
static OSATimerRef dtu_do3_report_timer_ref = NULL;//DO3上报定时器
//static OSATimerRef dtu_do3_timer_ref = NULL;//DO3翻转定时器
static OSATimerRef dtu_do4_report_timer_ref = NULL;//DO4上报定时器
//static OSATimerRef dtu_do4_timer_ref = NULL;//DO4翻转定时器
static OSATimerRef dtu_do5_report_timer_ref = NULL;//DO5上报定时器
//static OSATimerRef dtu_do5_timer_ref = NULL;//DO5翻转定时器

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

//static void dtu_do_flip(UINT8 io);
static void dtu_do1_report_timer_callback(UINT32 tmrId);
void dtu_do1_report_timer_start(UINT32 time);
void dtu_do1_report_timer_stop(void);
static void dtu_do2_report_timer_callback(UINT32 tmrId);
void dtu_do2_report_timer_start(UINT32 time);
void dtu_do2_report_timer_stop(void);
static void dtu_do3_report_timer_callback(UINT32 tmrId);
void dtu_do3_report_timer_start(UINT32 time);
void dtu_do3_report_timer_stop(void);
static void dtu_do4_report_timer_callback(UINT32 tmrId);
void dtu_do4_report_timer_start(UINT32 time);
void dtu_do4_report_timer_stop(void);
static void dtu_do5_report_timer_callback(UINT32 tmrId);
void dtu_do5_report_timer_start(UINT32 time);
void dtu_do5_report_timer_stop(void);

//static void dtu_do1_timer_start(UINT8 type, UINT32 time);
//static void dtu_do2_timer_start(UINT8 type, UINT32 time);
//static void dtu_do3_timer_start(UINT8 type, UINT32 time);
//static void dtu_do4_timer_start(UINT8 type, UINT32 time);
//static void dtu_do5_timer_start(UINT8 type, UINT32 time);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_do_flip
  * Description : 翻转io电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do_flip(UINT8 io)
//{
//    if(DTU_GPIO_LOW == GpioGetLevel(io))
//    {
//        uprintf("do set high\n");
//        GpioSetLevel(io, 1);
//    }
//    else if(DTU_GPIO_HIGH == GpioGetLevel(io))
//    {
//        uprintf("do set low\n");
//        GpioSetLevel(io, 0);
//    }
//}

/**
  * Function    : dtu_do1_report_timer_callback
  * Description : 主动上报定时器超时回调函数，发送消息给主任务，由主任务上报信息到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do1_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T do_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_do1_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        do_send_msg.msgId = DTU_DO_MSG_ID_DO_PROACTIVE_REPORT;
        do_send_msg.channel = DTU_GPIO_CHNNEL_1;
        do_send_msg.status = GpioGetLevel(DTU_GPIO_DO_PIN1);

//        uprintf("%s[%d] %d", __FUNCTION__, __LINE__, do_send_msg.status);
        //发送消息给json主任务
        dtu_json_task_send_msgq(&do_send_msg);
    }
}

/**
  * Function    : dtu_do1_report_timer_start
  * Description : 主动上报定时器启动
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do1_report_timer_start(UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

//    uprintf("start type timer %d\n", uc_time);
    //开启定时器
    OSATimerStart(dtu_do1_report_timer_ref, uc_time * 200, uc_time * 200, dtu_do1_report_timer_callback, 0);
}

/**
  * Function    : dtu_do1_report_timer_stop
  * Description : 主动上报定时器停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do1_report_timer_stop(void)
{
    //关闭定时器
    OSATimerStop(dtu_do1_report_timer_ref);
}


/**
  * Function    : dtu_do1_timer_callback
  * Description : 输出保持或者定时翻转定时器回调函数，超时后，翻转电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do1_timer_callback(UINT32 tmrId)
//{
//    dtu_do_flip(DTU_GPIO_DO_PIN1);
//}

/**
  * Function    : dtu_do1_timer_start
  * Description : 启动输出保持或者定时翻转定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do1_timer_start(UINT8 type, UINT32 time)
//{
//    UINT32 schedule = 0;

//    //判断是定时翻转还是输出保持时间
//    if(DTU_DO_TIME == type)
//    {
//        schedule = 0;
//        //开启定时器
//        OSATimerStart(dtu_do1_timer_ref, time * 200, schedule * 200, dtu_do1_timer_callback, 0);
//    }
//    else if(DTU_DO_FLIP == type)
//    {
//        schedule = time;
//        //开启定时器
//        OSATimerStart(dtu_do1_timer_ref, time * 200, schedule * 200, dtu_do1_timer_callback, 0);
//    }
    //uprintf("type:%d schedule:%d\n", type, schedule);
//}

/**
  * Function    : dtu_do1_timer_stop
  * Description : do1定时器关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do1_timer_stop(void)
//{
//    //开启定时器
//    OSATimerStop(dtu_do1_timer_ref);

//}

/**
  * Function    : dtu_do1_report_timer_callback
  * Description : 主动上报定时器超时回调函数，发送消息给主任务，由主任务上报信息到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do2_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T do_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_do2_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        do_send_msg.msgId = DTU_DO_MSG_ID_DO_PROACTIVE_REPORT;
        do_send_msg.channel = DTU_GPIO_CHNNEL_2;
        do_send_msg.status = GpioGetLevel(DTU_GPIO_DO_PIN2);

//        uprintf("%s[%d] %d", __FUNCTION__, __LINE__, do_send_msg.status);
        //发送消息给json主任务
        dtu_json_task_send_msgq(&do_send_msg);
    }
}

/**
  * Function    : dtu_do1_report_timer_start
  * Description : 主动上报定时器启动
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do2_report_timer_start (UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

//    uprintf("start type timer %d\n", uc_time);
    //开启定时器
    OSATimerStart(dtu_do2_report_timer_ref, uc_time * 200, uc_time * 200, dtu_do2_report_timer_callback, 0);
}

/**
  * Function    : dtu_do2_report_timer_stop
  * Description : 主动上报定时器停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do2_report_timer_stop(void)
{
    //关闭定时器
    OSATimerStop(dtu_do2_report_timer_ref);
}

/**
  * Function    : dtu_do1_timer_callback
  * Description : 输出保持或者定时翻转定时器回调函数，超时后，翻转电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do2_timer_callback(UINT32 tmrId)
//{
//    dtu_do_flip(DTU_GPIO_DO_PIN2);
//}

/**
  * Function    : dtu_do1_timer_start
  * Description : 启动输出保持或者定时翻转定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do2_timer_start(UINT8 type, UINT32 time)
//{
//    UINT32 schedule = 0;

//    //判断是定时翻转还是输出保持时间
//    if(DTU_DO_TIME == type)
//    {
//        schedule = 0;
//        //开启定时器
//        OSATimerStart(dtu_do2_timer_ref, time * 200, schedule * 200, dtu_do2_timer_callback, 0);
//    }
//    else if(DTU_DO_FLIP == type)
//    {
//        schedule = time;
//        //开启定时器
//        OSATimerStart(dtu_do2_timer_ref, time * 200, schedule * 200, dtu_do2_timer_callback, 0);
//    }
    //uprintf("type:%d schedule:%d\n", type, schedule);
//}

/**
  * Function    : dtu_do2_timer_stop
  * Description : do2定时器关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do2_timer_stop(void)
//{
//    //开启定时器
//    OSATimerStop(dtu_do2_timer_ref);

//}

/**
  * Function    : dtu_do1_report_timer_callback
  * Description : 主动上报定时器超时回调函数，发送消息给主任务，由主任务上报信息到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do3_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T do_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_do3_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        do_send_msg.msgId = DTU_DO_MSG_ID_DO_PROACTIVE_REPORT;
        do_send_msg.channel = DTU_GPIO_CHNNEL_3;
        do_send_msg.status = GpioGetLevel(DTU_GPIO_DO_PIN3);

//        uprintf("%s[%d] %d", __FUNCTION__, __LINE__, do_send_msg.status);
        //发送消息给json主任务
        dtu_json_task_send_msgq(&do_send_msg);
    }
}

/**
  * Function    : dtu_do1_report_timer_start
  * Description : 主动上报定时器启动
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do3_report_timer_start(UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

//    uprintf("start type timer %d\n", uc_time);
    //开启定时器
    OSATimerStart(dtu_do3_report_timer_ref, uc_time * 200, uc_time * 200, dtu_do3_report_timer_callback, 0);
}

/**
  * Function    : dtu_do3_report_timer_stop
  * Description : 主动上报定时器停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do3_report_timer_stop(void)
{
    //关闭定时器
    OSATimerStop(dtu_do3_report_timer_ref);
}

/**
  * Function    : dtu_do1_timer_callback
  * Description : 输出保持或者定时翻转定时器回调函数，超时后，翻转电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do3_timer_callback(UINT32 tmrId)
//{
//    dtu_do_flip(DTU_GPIO_DO_PIN3);
//}

/**
  * Function    : dtu_do1_timer_start
  * Description : 启动输出保持或者定时翻转定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do3_timer_start(UINT8 type, UINT32 time)
//{
//    UINT32 schedule = 0;

//    //判断是定时翻转还是输出保持时间
//    if(DTU_DO_TIME == type)
//    {
//        schedule = 0;
//        //开启定时器
//        OSATimerStart(dtu_do3_timer_ref, time * 200, schedule * 200, dtu_do3_timer_callback, 0);
//    }
//    else if(DTU_DO_FLIP == type)
//    {
//        schedule = time;
//        //开启定时器
//        OSATimerStart(dtu_do3_timer_ref, time * 200, schedule * 200, dtu_do3_timer_callback, 0);
//    }
    //uprintf("type:%d schedule:%d\n", type, schedule);
//}

/**
  * Function    : dtu_do3_timer_stop
  * Description : do3定时器关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do3_timer_stop(void)
//{
//    //开启定时器
//    OSATimerStop(dtu_do3_timer_ref);

//}

/**
  * Function    : dtu_do1_report_timer_callback
  * Description : 主动上报定时器超时回调函数，发送消息给主任务，由主任务上报信息到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do4_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T do_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_do4_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        do_send_msg.msgId = DTU_DO_MSG_ID_DO_PROACTIVE_REPORT;
        do_send_msg.channel = DTU_GPIO_CHNNEL_4;
        do_send_msg.status = GpioGetLevel(DTU_GPIO_DO_PIN4);

//        uprintf("%s[%d] %d", __FUNCTION__, __LINE__, do_send_msg.status);
        //发送消息给json主任务
        dtu_json_task_send_msgq(&do_send_msg);
    }
}

/**
  * Function    : dtu_do4_report_timer_start
  * Description : 主动上报定时器启动
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do4_report_timer_start(UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

//    uprintf("start type timer %d\n", uc_time);
    //开启定时器
    OSATimerStart(dtu_do4_report_timer_ref, uc_time * 200, uc_time * 200, dtu_do4_report_timer_callback, 0);
}

/**
  * Function    : dtu_do4_report_timer_stop
  * Description : 主动上报定时器停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do4_report_timer_stop(void)
{
    //关闭定时器
    OSATimerStop(dtu_do4_report_timer_ref);
}

/**
  * Function    : dtu_do1_timer_callback
  * Description : 输出保持或者定时翻转定时器回调函数，超时后，翻转电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do4_timer_callback(UINT32 tmrId)
//{
//    dtu_do_flip(DTU_GPIO_DO_PIN4);
//}

/**
  * Function    : dtu_do1_timer_start
  * Description : 启动输出保持或者定时翻转定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do4_timer_start(UINT8 type, UINT32 time)
//{
//    UINT32 schedule = 0;

//    //判断是定时翻转还是输出保持时间
//    if(DTU_DO_TIME == type)
//    {
//        schedule = 0;
//        //开启定时器
//        OSATimerStart(dtu_do4_timer_ref, time * 200, schedule * 200, dtu_do4_timer_callback, 0);
//    }
//    else if(DTU_DO_FLIP == type)
//    {
//        schedule = time;
//        //开启定时器
//        OSATimerStart(dtu_do4_timer_ref, time * 200, schedule * 200, dtu_do4_timer_callback, 0);
//    }
    //uprintf("type:%d schedule:%d\n", type, schedule);
//}

/**
  * Function    : dtu_do4_timer_stop
  * Description : do4定时器关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do4_timer_stop(void)
//{
//    //开启定时器
//    OSATimerStop(dtu_do4_timer_ref);

//}

/**
  * Function    : dtu_do5_report_timer_callback
  * Description : 主动上报定时器超时回调函数，发送消息给主任务，由主任务上报信息到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do5_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T do_send_msg = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();

//    uprintf("dtu_do5_report_timer_callback\n");

    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        do_send_msg.msgId = DTU_DO_MSG_ID_DO_PROACTIVE_REPORT;
        do_send_msg.channel = DTU_GPIO_CHNNEL_5;
        do_send_msg.status = GpioGetLevel(DTU_GPIO_DO_PIN5);

//        uprintf("%s[%d] %d", __FUNCTION__, __LINE__, do_send_msg.status);
        //发送消息给json主任务
        dtu_json_task_send_msgq(&do_send_msg);
    }
}

/**
  * Function    : dtu_do5_report_timer_start
  * Description : 主动上报定时器启动
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do5_report_timer_start(UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

//    uprintf("start type timer %d\n", uc_time);
    //开启定时器
    OSATimerStart(dtu_do5_report_timer_ref, uc_time * 200, uc_time * 200, dtu_do5_report_timer_callback, 0);
}

/**
  * Function    : dtu_do5_report_timer_stop
  * Description : 主动上报定时器停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do5_report_timer_stop(void)
{
    //关闭定时器
    OSATimerStop(dtu_do5_report_timer_ref);
}

/**
  * Function    : dtu_do1_timer_callback
  * Description : 输出保持或者定时翻转定时器回调函数，超时后，翻转电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void dtu_do5_timer_callback(UINT32 tmrId)
//{
//    dtu_do_flip(DTU_GPIO_DO_PIN5);
//}

/**
  * Function    : dtu_do5_timer_start
  * Description : 启动输出保持或者定时翻转定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do5_timer_start(UINT8 type, UINT32 time)
//{
//    UINT32 schedule = 0;

//    //判断是定时翻转还是输出保持时间
//    if(DTU_DO_TIME == type)
//    {
//        schedule = 0;
//        //开启定时器
//        OSATimerStart(dtu_do5_timer_ref, time * 200, schedule * 200, dtu_do5_timer_callback, 0);
//    }
//    else if(DTU_DO_FLIP == type)
//    {
//        schedule = time;
//        //开启定时器
//        OSATimerStart(dtu_do5_timer_ref, time * 200, schedule * 200, dtu_do5_timer_callback, 0);
//    }
//    uprintf("type:%d schedule:%d\n", type, schedule);
//}

/**
  * Function    : dtu_do5_timer_stop
  * Description : do5定时器关闭
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void dtu_do5_timer_stop(void)
//{
//    //开启定时器
//    OSATimerStop(dtu_do5_timer_ref);

//}

/**
  * Function    : dtu_do_init_do
  * Description : 根据NV项，判断是否需要开启do上报
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_init_do(char* channel, DTU_DO* doo)
{
    if(strcmp((const char*)channel, "DO1") == 0)
    {
        //如果主动上报
        if(DTU_DO_REPORT_INITIATIVE == doo->do_type)
        {
            //启动主动上报定时器
            dtu_do1_report_timer_start(doo->do_interval);
        }
        else if(DTU_DO_REPORT_NONE == doo->do_type)
        {
            //关闭主动上报定时器
            OSATimerStop(dtu_do1_report_timer_ref);
        }
    }
    else if(strcmp((const char*)channel, "DO2") == 0)
    {
        //如果主动上报
        if(DTU_DO_REPORT_INITIATIVE == doo->do_type)
        {
            //启动主动上报定时器
            dtu_do2_report_timer_start(doo->do_interval);
        }
        else if(DTU_DO_REPORT_NONE == doo->do_type)
        {
            //关闭主动上报定时器
            OSATimerStop(dtu_do2_report_timer_ref);
        }
    }
    else if(strcmp((const char*)channel, "DO3") == 0)
    {
        //如果主动上报
        if(DTU_DO_REPORT_INITIATIVE == doo->do_type)
        {
            //启动主动上报定时器
            dtu_do3_report_timer_start(doo->do_interval);
        }
        else if(DTU_DO_REPORT_NONE == doo->do_type)
        {
            //关闭主动上报定时器
            OSATimerStop(dtu_do3_report_timer_ref);
        }
    }
    else if(strcmp((const char*)channel, "DO4") == 0)
    {
        //如果主动上报
        if(DTU_DO_REPORT_INITIATIVE == doo->do_type)
        {
            //启动主动上报定时器
            dtu_do4_report_timer_start(doo->do_interval);
        }
        else if(DTU_DO_REPORT_NONE == doo->do_type)
        {
            //关闭主动上报定时器
            OSATimerStop(dtu_do4_report_timer_ref);
        }
    }
    else if(strcmp((const char*)channel, "DO5") == 0)
    {
        //如果主动上报
        if(DTU_DO_REPORT_INITIATIVE == doo->do_type)
        {
            //启动主动上报定时器
            dtu_do5_report_timer_start(doo->do_interval);
        }
        else if(DTU_DO_REPORT_NONE == doo->do_type)
        {
            //关闭主动上报定时器
            OSATimerStop(dtu_do5_report_timer_ref);
        }
    }

//    if(0 == doo->do_time)
//    {
//        if(strcmp((const char*)channel, "DO1") == 0)
//        {
//            //关闭输出保持定时器
//            OSATimerStop(dtu_do1_timer_ref);
//        }
//        if(strcmp((const char*)channel, "DO2") == 0)
//        {
//            //关闭输出保持定时器
//            OSATimerStop(dtu_do2_timer_ref);
//        }
//        if(strcmp((const char*)channel, "DO3") == 0)
//        {
//            //关闭输出保持定时器
//            OSATimerStop(dtu_do3_timer_ref);
//        }
//        if(strcmp((const char*)channel, "DO4") == 0)
//        {
//            //关闭输出保持定时器
//            OSATimerStop(dtu_do4_timer_ref);
//        }
//        if(strcmp((const char*)channel, "DO5") == 0)
//        {
//            //关闭输出保持定时器
//            OSATimerStop(dtu_do5_timer_ref);
//        }
//    }
//    //如果输出保持时间不为0，而且当前是输出高电平
//    else if(DTU_GPIO_HIGH == GpioGetLevel(DTU_GPIO_DO_PIN1) && 0 != doo->do_time)
//    {
//        if(strcmp((const char*)channel, "DO1") == 0)
//        {
//            //启动输出保持定时器
//            dtu_do1_timer_start(DTU_DO_TIME, doo->do_time);
//        }
//        if(strcmp((const char*)channel, "DO2") == 0)
//        {
//            //启动输出保持定时器
//            dtu_do1_timer_start(DTU_DO_TIME, doo->do_time);
//        }
//        if(strcmp((const char*)channel, "DO3") == 0)
//        {
//            //启动输出保持定时器
//            dtu_do1_timer_start(DTU_DO_TIME, doo->do_time);
//        }
//        if(strcmp((const char*)channel, "DO4") == 0)
//        {
//            //启动输出保持定时器
//            dtu_do1_timer_start(DTU_DO_TIME, doo->do_time);
//        }
//        if(strcmp((const char*)channel, "DO5") == 0)
//        {
//            //启动输出保持定时器
//            dtu_do1_timer_start(DTU_DO_TIME, doo->do_time);
//        }
//    }

//    //如果输出保持时间没有配置，配置定时翻转
//    if(0 == doo->do_time && 0 != doo->do_flip)
//    {
//        if(strcmp((const char*)channel, "DO1") == 0)
//        {
//            //启动定时翻转定时器
//            dtu_do1_timer_start(DTU_DO_FLIP, doo->do_flip);
//        }
//        if(strcmp((const char*)channel, "DO2") == 0)
//        {
//            //启动定时翻转定时器
//            dtu_do1_timer_start(DTU_DO_FLIP, doo->do_flip);
//        }
//        if(strcmp((const char*)channel, "DO3") == 0)
//        {
//            //启动定时翻转定时器
//            dtu_do1_timer_start(DTU_DO_FLIP, doo->do_flip);
//        }
//        if(strcmp((const char*)channel, "DO4") == 0)
//        {
//            //启动定时翻转定时器
//            dtu_do1_timer_start(DTU_DO_FLIP, doo->do_flip);
//        }
//        if(strcmp((const char*)channel, "DO5") == 0)
//        {
//            //启动定时翻转定时器
//            dtu_do1_timer_start(DTU_DO_FLIP, doo->do_flip);
//        }
//    }

}

/**
  * Function    : dtu_do_init
  * Description : 上电初始化所有的do
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_init(void)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    //init out gpio
    ret = GpioSetDirection(DTU_GPIO_DO_PIN1, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(DTU_GPIO_DO_PIN2, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(DTU_GPIO_DO_PIN3, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(DTU_GPIO_DO_PIN4, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    ret = GpioSetDirection(DTU_GPIO_DO_PIN5, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);

    //初始化就停止定时器
    OSATimerStop(dtu_do1_report_timer_ref);
//    OSATimerStop(dtu_do1_timer_ref);
    OSATimerStop(dtu_do2_report_timer_ref);
//    OSATimerStop(dtu_do2_timer_ref);
    OSATimerStop(dtu_do3_report_timer_ref);
//    OSATimerStop(dtu_do3_timer_ref);
    OSATimerStop(dtu_do4_report_timer_ref);
//    OSATimerStop(dtu_do4_timer_ref);
    OSATimerStop(dtu_do5_report_timer_ref);
//    OSATimerStop(dtu_do5_timer_ref);

    //初始化DO状态
    if(dtu_file_ctx->doo.do1.do_status == DTU_DO_OUT_STATUS_LAST)
    {
//        uprintf("%s init set restore:%d\n", channel, doo->do_out);
        GpioSetLevel(DTU_GPIO_DO_PIN1, dtu_file_ctx->doo.do1.do_out);
    }
    else if(dtu_file_ctx->doo.do1.do_status == DTU_DO_OUT_STATUS_HIGH)
    {
//        uprintf("%s init set high\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN1, 1);
    }
    else
    {
//        uprintf("%s init set low\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN1, 0);
    }
    if(dtu_file_ctx->doo.do2.do_status == DTU_DO_OUT_STATUS_LAST)
    {
//        uprintf("%s init set restore:%d\n", channel, doo->do_out);
        GpioSetLevel(DTU_GPIO_DO_PIN2, dtu_file_ctx->doo.do2.do_out);
    }
    else if(dtu_file_ctx->doo.do2.do_status == DTU_DO_OUT_STATUS_HIGH)
    {
//        uprintf("%s init set high\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN2, 1);
    }
    else
    {
//        uprintf("%s init set low\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN2, 0);
    }
    if(dtu_file_ctx->doo.do3.do_status == DTU_DO_OUT_STATUS_LAST)
    {
//        uprintf("%s init set restore:%d\n", channel, doo->do_out);
        GpioSetLevel(DTU_GPIO_DO_PIN3, dtu_file_ctx->doo.do3.do_out);
    }
    else if(dtu_file_ctx->doo.do3.do_status == DTU_DO_OUT_STATUS_HIGH)
    {
//        uprintf("%s init set high\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN3, 1);
    }
    else
    {
//        uprintf("%s init set low\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN3, 0);
    }
    if(dtu_file_ctx->doo.do4.do_status == DTU_DO_OUT_STATUS_LAST)
    {
//        uprintf("%s init set restore:%d\n", channel, doo->do_out);
        GpioSetLevel(DTU_GPIO_DO_PIN4, dtu_file_ctx->doo.do4.do_out);
    }
    else if(dtu_file_ctx->doo.do4.do_status == DTU_DO_OUT_STATUS_HIGH)
    {
//        uprintf("%s init set high\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN4, 1);
    }
    else
    {
//        uprintf("%s init set low\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN4, 0);
    }
    if(dtu_file_ctx->doo.do5.do_status == DTU_DO_OUT_STATUS_LAST)
    {
//        uprintf("%s init set restore:%d\n", channel, doo->do_out);
        GpioSetLevel(DTU_GPIO_DO_PIN5, dtu_file_ctx->doo.do5.do_out);
    }
    else if(dtu_file_ctx->doo.do5.do_status == DTU_DO_OUT_STATUS_HIGH)
    {
//        uprintf("%s init set high\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN5, 1);
    }
    else
    {
//        uprintf("%s init set low\n", channel);
        GpioSetLevel(DTU_GPIO_DO_PIN5, 0);
    }

    //初始化do上报状态
    dtu_do_init_do("DO1", &dtu_file_ctx->doo.do1);
    dtu_do_init_do("DO2", &dtu_file_ctx->doo.do2);
    dtu_do_init_do("DO3", &dtu_file_ctx->doo.do3);
    dtu_do_init_do("DO4", &dtu_file_ctx->doo.do4);
    dtu_do_init_do("DO5", &dtu_file_ctx->doo.do5);

}

/**
  * Function    : dtu_do_read_pin
  * Description : 读DO电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT8 dtu_do_read_pin(UINT8 channel)
{
    return GpioGetLevel(channel);
}

/**
  * Function    : dtu_do_write_pin_1
  * Description : 写do1
  * Input       : status        电平，1设置高电平，0设置低电平
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_write_pin(char* channel, UINT8 status)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if(strcmp((const char*)channel, "DO1") == 0)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN1, status);
        dtu_file_ctx->doo.do1.do_out = status;
        //如果输出保持时间不为0，而且当前是输出高电平
//        if(DTU_GPIO_HIGH == status && 0 != dtu_file_ctx->doo.do1.do_time)
//        {
//            //启动输出保持定时器
//            dtu_do1_timer_start(DTU_DO_TIME, dtu_file_ctx->doo.do1.do_time);
//        }
    }
    if(strcmp((const char*)channel, "DO2") == 0)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN2, status);
        dtu_file_ctx->doo.do2.do_out = status;
        //如果输出保持时间不为0，而且当前是输出高电平
//        if(DTU_GPIO_HIGH == status && 0 != dtu_file_ctx->doo.do2.do_time)
//        {
//            //启动输出保持定时器
//            dtu_do2_timer_start(DTU_DO_TIME, dtu_file_ctx->doo.do2.do_time);
//        }
    }
    if(strcmp((const char*)channel, "DO3") == 0)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN3, status);
        dtu_file_ctx->doo.do3.do_out = status;
        //如果输出保持时间不为0，而且当前是输出高电平
//        if(DTU_GPIO_HIGH == status && 0 != dtu_file_ctx->doo.do3.do_time)
//        {
//            //启动输出保持定时器
//            dtu_do3_timer_start(DTU_DO_TIME, dtu_file_ctx->doo.do3.do_time);
//        }
    }
    if(strcmp((const char*)channel, "DO4") == 0)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN4, status);
        dtu_file_ctx->doo.do4.do_out = status;
        //如果输出保持时间不为0，而且当前是输出高电平
//        if(DTU_GPIO_HIGH == status && 0 != dtu_file_ctx->doo.do4.do_time)
//        {
//            //启动输出保持定时器
//            dtu_do4_timer_start(DTU_DO_TIME, dtu_file_ctx->doo.do4.do_time);
//        }
    }
    if(strcmp((const char*)channel, "DO5") == 0)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN5, status);
        dtu_file_ctx->doo.do5.do_out = status;
        //如果输出保持时间不为0，而且当前是输出高电平
//        if(DTU_GPIO_HIGH == status && 0 != dtu_file_ctx->doo.do5.do_time)
//        {
//            //启动输出保持定时器
//            dtu_do5_timer_start(DTU_DO_TIME, dtu_file_ctx->doo.do5.do_time);
//        }
    }
}

/**
  * Function    : dtu_do_clk_timeout_write_pin
  * Description : 闹钟超时后，写对应闹钟配置的DO电平
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_clk_timeout_write_pin(UINT8 channel, UINT8 status)
{
    uprintf("%s[%d]timeout index:%d status:%d", __FUNCTION__, __LINE__, channel, status);
    if(DTU_GPIO_CHNNEL_1 == channel)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN1, status);
        uprintf("%s[%d]timeout do1 set", __FUNCTION__, __LINE__);
    }
    else if(DTU_GPIO_CHNNEL_2 == channel)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN2, status);
        uprintf("%s[%d]timeout do2 set", __FUNCTION__, __LINE__);
    }
    else if(DTU_GPIO_CHNNEL_3 == channel)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN3, status);
        uprintf("%s[%d]timeout do3 set", __FUNCTION__, __LINE__);
    }
    else if(DTU_GPIO_CHNNEL_4 == channel)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN4, status);
        uprintf("%s[%d]timeout do4 set", __FUNCTION__, __LINE__);
    }
    else if(DTU_GPIO_CHNNEL_5 == channel)
    {
        GpioSetLevel(DTU_GPIO_DO_PIN5, status);
        uprintf("%s[%d]timeout do5 set", __FUNCTION__, __LINE__);
    }
}

/**
  * Function    : dtu_do_times_init
  * Description : 初始化所有定时器句柄
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_times_init(void)
{
    int ret = 0;

    ret = OSATimerCreate(&dtu_do1_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSATimerCreate(&dtu_do1_timer_ref);
//    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_do2_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSATimerCreate(&dtu_do2_timer_ref);
//    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_do3_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSATimerCreate(&dtu_do3_timer_ref);
//    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_do4_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSATimerCreate(&dtu_do4_timer_ref);
//    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_do5_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);
//    ret = OSATimerCreate(&dtu_do5_timer_ref);
//    ASSERT(ret == OS_SUCCESS);
}

// End of file : am_do.c 2023-8-29 10:11:16 by: zhaoning 

