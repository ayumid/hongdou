//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ai.c
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

#include "am_ai.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_AI_AVG_MAX_NUM              100

// Private variables ------------------------------------------------------------

static OSATimerRef dtu_ai_report_timer_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------


/**
  * Function    : dtu_ai_read
  * Description : 读adc值，提供给调用函数计算外接信号电流
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT16 dtu_ai_read(void)
{
    UINT16 ul_adc_value = 0;
    UINT32 ul_adc_temp = 0;
    UINT16 adc_temp[DTU_AI_AVG_MAX_NUM] = {0};
    int i = 0;

    //平均值滤波
    for(i = 0; i < DTU_AI_AVG_MAX_NUM; i++)
    {
        adc_temp[i] = SDK_READ_ADC_VALUE_NEW(0, 1);
        ul_adc_temp += adc_temp[i];
        dtu_10ms_sleep(1);
    }
    ul_adc_value = ul_adc_temp / DTU_AI_AVG_MAX_NUM;
    uprintf("SDK_READ_ADC_VALUE 0 adcValue %d\n", ul_adc_value);

    return ul_adc_value;
}

/**
  * Function    : dtu_ai_report_timer_callback
  * Description : 主动上报回调函数，判断当前是否在透传模式，发送消息给JSON任务，上报到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_ai_report_timer_callback(UINT32 tmrId)
{
    DTU_JSON_MSG_T ai_send_msg = {0};
//    uprintf("dtu_do1_report_timer_callback\n");
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    dtu_file_ctx = dtu_get_file_ctx();
    
//    uprintf("dtu_gnss_timer_callback");
    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
//        if(DTU_AI_REPORT_INITIATIVE == dtu_file_ctx->ai.ai_res_rule)
//        {
            ai_send_msg.msgId = DTU_AI_MSG_ID_AI_PROACTIVE_REPORT;
//        }
//        else if(DTU_AI_REPORT_TRIGGER == dtu_file_ctx->ai.ai_res_rule)
//        {
//            ai_send_msg.msgId = DTU_AI_MSG_ID_AI_TRIGGER_REPORT;
//        }
        
        dtu_json_task_send_msgq(&ai_send_msg);
    }
}

/**
  * Function    : dtu_ai1_report_timer_start
  * Description : 开始AI周期上报定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ai1_report_timer_start(UINT32 time)
{
    UINT32 uc_time = 0;
    
    uc_time = time;

    OSATimerStop(dtu_ai_report_timer_ref);
//    uprintf("start type timer %d\n", uc_time);
    //开启定时器
    OSATimerStart(dtu_ai_report_timer_ref, uc_time * 200, uc_time * 200, dtu_ai_report_timer_callback, 0);
}

/**
  * Function    : dtu_ai1_report_timer_stop
  * Description : 关闭AI周期上报定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ai1_report_timer_stop(void)
{
    OSATimerStop(dtu_ai_report_timer_ref);
}

/**
  * Function    : dtu_ai_times_init
  * Description : AI主动上报定时器创建，根据NV项判断是否打开周期上报定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ai_times_init(void)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    ret = OSATimerCreate(&dtu_ai_report_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    if(DTU_AI_REPORT_NONE != dtu_file_ctx->ai.ai_type)
    {
        if(dtu_file_ctx->ai.ai_interval >= 10)
        {
            //打开周期上报定时器
            dtu_ai1_report_timer_start(dtu_file_ctx->ai.ai_interval);
        }
    }
}

// End of file : am_ai.c 2023-8-29 10:10:53 by: zhaoning 

