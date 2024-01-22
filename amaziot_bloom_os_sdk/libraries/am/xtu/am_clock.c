//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_clock.c
// Auther      : zhaoning
// Version     :
// Date : 2023-10-24
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-10-24
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_clock.h"

// Private defines / typedefs ---------------------------------------------------

static OSATimerRef dtu_clock1_timer_ref = NULL;//时钟1定时器
static OSATimerRef dtu_clock2_timer_ref = NULL;//时钟2定时器
static OSATimerRef dtu_clock3_timer_ref = NULL;//时钟3定时器
static OSATimerRef dtu_clock4_timer_ref = NULL;//时钟4定时器
static OSATimerRef dtu_clock5_timer_ref = NULL;//时钟5定时器

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

void dtu_clk_timer_stop(UINT8 index);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_clk1_timer_callback
  * Description : 闹钟1回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk1_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    uprintf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk1_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].num, dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].level);
}

/**
  * Function    : dtu_clk2_timer_callback
  * Description : 闹钟2回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk2_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    uprintf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk2_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].num, dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].level);
}

/**
  * Function    : dtu_clk3_timer_callback
  * Description : 闹钟3回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk3_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    uprintf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk3_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].num, dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].level);
}

/**
  * Function    : dtu_clk4_timer_callback
  * Description : 闹钟4回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk4_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    uprintf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk4_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].num, dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].level);
}

/**
  * Function    : dtu_clk5_timer_callback
  * Description : 闹钟5回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk5_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    uprintf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk5_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].num, dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].level);
}

/**
  * Function    : dtu_clk_timer_start
  * Description : 闹钟1开启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk_timer_start(UINT8 index)
{
    t_rtc bj_time;
    UINT32 time_sec_now = 0;
    UINT32 time_sec_alarm = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    //获取当前北京时间
    SDK_GET_BEIJING_TIME(&bj_time);
    //计算当前时间和闹钟时间秒数
    time_sec_now = bj_time.tm_hour * DTU_CLK_ONE_HOUR_HAS_SEC + bj_time.tm_min * DTU_CLK_ONE_MIN_HAS_SEC + bj_time.tm_sec;
    time_sec_alarm = dtu_file_ctx->clk.params[index - 1].h * DTU_CLK_ONE_HOUR_HAS_SEC + dtu_file_ctx->clk.params[index - 1].m * DTU_CLK_ONE_MIN_HAS_SEC + dtu_file_ctx->clk.params[index - 1].s;
    uprintf("clk%d: %d:%d:%d,%d %d:%d:%d,%d", index, bj_time.tm_hour, bj_time.tm_min, bj_time.tm_sec,
                                            time_sec_now,
                                            dtu_file_ctx->clk.params[index - 1].h, dtu_file_ctx->clk.params[index - 1].m, dtu_file_ctx->clk.params[index - 1].s,
                                            time_sec_alarm);
    dtu_clk_timer_stop(index);
    //判断闹钟时间是在今天还是明天
    //闹钟时间在今天当前时间之后
    if(time_sec_now < time_sec_alarm)
    {
        //开启定时器
        if(DTU_TIME_INDEX_1 == index)
        {
            OSATimerStart(dtu_clock1_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk1_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_2 == index)
        {
            OSATimerStart(dtu_clock2_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk2_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_3 == index)
        {
            OSATimerStart(dtu_clock3_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk3_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_4 == index)
        {
            OSATimerStart(dtu_clock4_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk4_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_5 == index)
        {
            OSATimerStart(dtu_clock5_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk5_timer_callback, 0);
        }
    }
    else
    {
        //开启定时器
        if(DTU_TIME_INDEX_1 == index)
        {
            OSATimerStart(dtu_clock1_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk1_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_2 == index)
        {
            OSATimerStart(dtu_clock2_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk2_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_3 == index)
        {
            OSATimerStart(dtu_clock3_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk3_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_4 == index)
        {
            OSATimerStart(dtu_clock4_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk4_timer_callback, 0);
        }
        else if(DTU_TIME_INDEX_5 == index)
        {
            OSATimerStart(dtu_clock5_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk5_timer_callback, 0);
        }
    }
}

/**
  * Function    : dtu_clk_timer_stop
  * Description : 闹钟1停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk_timer_stop(UINT8 index)
{
    //开启定时器
    if(DTU_TIME_INDEX_1 == index)
    {
        OSATimerStop(dtu_clock1_timer_ref);
    }
    else if(DTU_TIME_INDEX_2 == index)
    {
        OSATimerStop(dtu_clock2_timer_ref);
    }
    else if(DTU_TIME_INDEX_3 == index)
    {
        OSATimerStop(dtu_clock3_timer_ref);
    }
    else if(DTU_TIME_INDEX_4 == index)
    {
        OSATimerStop(dtu_clock4_timer_ref);
    }
    else if(DTU_TIME_INDEX_5 == index)
    {
        OSATimerStop(dtu_clock5_timer_ref);
    }
}

/**
  * Function    : dtu_clk_times_init
  * Description : 时钟定时器初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk_times_init(void)
{
    int ret = 0;

    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock2_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock3_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock4_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock5_timer_ref);
    ASSERT(ret == OS_SUCCESS);

}

// End of file : am_clock.c 2023-10-24 9:25:57 by: zhaoning 

