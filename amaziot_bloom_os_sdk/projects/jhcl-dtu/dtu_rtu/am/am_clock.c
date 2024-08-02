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

void dtu_clk1_timer_stop(void);
void dtu_clk2_timer_stop(void);
void dtu_clk3_timer_stop(void);
void dtu_clk4_timer_stop(void);
void dtu_clk5_timer_stop(void);

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

    printf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk1_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.clk1.num, dtu_file_ctx->clk.clk1.level);
}

/**
  * Function    : dtu_clk1_timer_start
  * Description : 闹钟1开启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk1_timer_start(void)
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
    time_sec_alarm = dtu_file_ctx->clk.clk1.h * DTU_CLK_ONE_HOUR_HAS_SEC + dtu_file_ctx->clk.clk1.m * DTU_CLK_ONE_MIN_HAS_SEC + dtu_file_ctx->clk.clk1.s;
    printf("clk1: %d:%d:%d,%d %d:%d:%d,%d", bj_time.tm_hour, bj_time.tm_min, bj_time.tm_sec,
                                            time_sec_now,
                                            dtu_file_ctx->clk.clk1.h, dtu_file_ctx->clk.clk1.m, dtu_file_ctx->clk.clk1.s,
                                            time_sec_alarm);
    dtu_clk1_timer_stop();
    //判断闹钟时间是在今天还是明天
    //闹钟时间在今天当前时间之后
    if(time_sec_now < time_sec_alarm)
    {
        //开启定时器
        OSATimerStart(dtu_clock1_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk1_timer_callback, 0);
    }
    else
    {
        //开启定时器
        OSATimerStart(dtu_clock1_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk1_timer_callback, 0);
    }
}

/**
  * Function    : dtu_clk1_timer_stop
  * Description : 闹钟1停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk1_timer_stop(void)
{
    //开启定时器
    OSATimerStop(dtu_clock1_timer_ref);

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

    printf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk2_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.clk2.num, dtu_file_ctx->clk.clk2.level);
}

/**
  * Function    : dtu_clk2_timer_start
  * Description : 闹钟1开启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk2_timer_start(void)
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
    time_sec_alarm = dtu_file_ctx->clk.clk2.h * DTU_CLK_ONE_HOUR_HAS_SEC + dtu_file_ctx->clk.clk2.m * DTU_CLK_ONE_MIN_HAS_SEC + dtu_file_ctx->clk.clk2.s;
    printf("clk2: %d:%d:%d,%d %d:%d:%d,%d", bj_time.tm_hour, bj_time.tm_min, bj_time.tm_sec,
                                        time_sec_now,
                                        dtu_file_ctx->clk.clk1.h, dtu_file_ctx->clk.clk1.m, dtu_file_ctx->clk.clk1.s,
                                        time_sec_alarm);
    dtu_clk2_timer_stop();
    //判断闹钟时间是在今天还是明天
    //闹钟时间在今天当前时间之后
    if(time_sec_now < time_sec_alarm)
    {
        //开启定时器
        OSATimerStart(dtu_clock2_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk2_timer_callback, 0);
    }
    else
    {
        //开启定时器
        OSATimerStart(dtu_clock2_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk2_timer_callback, 0);
    }
}

/**
  * Function    : dtu_clk2_timer_stop
  * Description : 闹钟1停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk2_timer_stop(void)
{
    //开启定时器
    OSATimerStop(dtu_clock2_timer_ref);

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

    printf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk3_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.clk3.num, dtu_file_ctx->clk.clk3.level);
}

/**
  * Function    : dtu_clk3_timer_start
  * Description : 闹钟3开启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk3_timer_start(void)
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
    time_sec_alarm = dtu_file_ctx->clk.clk3.h * DTU_CLK_ONE_HOUR_HAS_SEC + dtu_file_ctx->clk.clk3.m * DTU_CLK_ONE_MIN_HAS_SEC + dtu_file_ctx->clk.clk3.s;
    printf("clk3: %d:%d:%d,%d %d:%d:%d,%d", bj_time.tm_hour, bj_time.tm_min, bj_time.tm_sec,
                                        time_sec_now,
                                        dtu_file_ctx->clk.clk1.h, dtu_file_ctx->clk.clk1.m, dtu_file_ctx->clk.clk1.s,
                                        time_sec_alarm);
    dtu_clk3_timer_stop();
    //判断闹钟时间是在今天还是明天
    //闹钟时间在今天当前时间之后
    if(time_sec_now < time_sec_alarm)
    {
        //开启定时器
        OSATimerStart(dtu_clock3_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk3_timer_callback, 0);
    }
    //闹钟时间在今天当前时间之前，第二天再动作
    else
    {
        //开启定时器
        OSATimerStart(dtu_clock3_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk3_timer_callback, 0);
    }
}

/**
  * Function    : dtu_clk3_timer_stop
  * Description : 闹钟3停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk3_timer_stop(void)
{
    //开启定时器
    OSATimerStop(dtu_clock3_timer_ref);

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

    printf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk4_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.clk4.num, dtu_file_ctx->clk.clk4.level);
}

/**
  * Function    : dtu_clk4_timer_start
  * Description : 闹钟4开启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk4_timer_start(void)
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
    time_sec_alarm = dtu_file_ctx->clk.clk4.h * DTU_CLK_ONE_HOUR_HAS_SEC + dtu_file_ctx->clk.clk4.m * DTU_CLK_ONE_MIN_HAS_SEC + dtu_file_ctx->clk.clk4.s;
    printf("clk4: %d:%d:%d,%d %d:%d:%d,%d", bj_time.tm_hour, bj_time.tm_min, bj_time.tm_sec,
                                        time_sec_now,
                                        dtu_file_ctx->clk.clk1.h, dtu_file_ctx->clk.clk1.m, dtu_file_ctx->clk.clk1.s,
                                        time_sec_alarm);
    dtu_clk4_timer_stop();
    //判断闹钟时间是在今天还是明天
    //闹钟时间在今天当前时间之后
    if(time_sec_now < time_sec_alarm)
    {
        //开启定时器
        OSATimerStart(dtu_clock4_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk4_timer_callback, 0);
    }
    //闹钟时间在今天当前时间之前，第二天再动作
    else
    {
        //开启定时器
        OSATimerStart(dtu_clock4_timer_ref, (time_sec_alarm - time_sec_now + 86400) * 200, 0, dtu_clk4_timer_callback, 0);
    }
}

/**
  * Function    : dtu_clk4_timer_stop
  * Description : 闹钟4停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk4_timer_stop(void)
{
    //开启定时器
    OSATimerStop(dtu_clock4_timer_ref);

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

    printf("%s[%d]", __FUNCTION__, __LINE__);
//    dtu_clk5_timer_stop();
    //按照配置输出对应DO
    dtu_do_clk_timeout_write_pin(dtu_file_ctx->clk.clk5.num, dtu_file_ctx->clk.clk5.level);
}

/**
  * Function    : dtu_clk5_timer_start
  * Description : 闹钟5开启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk5_timer_start(void)
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
    time_sec_alarm = dtu_file_ctx->clk.clk5.h * DTU_CLK_ONE_HOUR_HAS_SEC + dtu_file_ctx->clk.clk5.m * DTU_CLK_ONE_MIN_HAS_SEC + dtu_file_ctx->clk.clk5.s;
    printf("clk5: %d:%d:%d,%d %d:%d:%d,%d", bj_time.tm_hour, bj_time.tm_min, bj_time.tm_sec,
                                        time_sec_now,
                                        dtu_file_ctx->clk.clk1.h, dtu_file_ctx->clk.clk1.m, dtu_file_ctx->clk.clk1.s,
                                        time_sec_alarm);
    dtu_clk5_timer_stop();
    //判断闹钟时间是在今天还是明天
    //闹钟时间在今天当前时间之后
    if(time_sec_now < time_sec_alarm)
    {
        //开启定时器
        OSATimerStart(dtu_clock5_timer_ref, (time_sec_alarm - time_sec_now) * 200, 0, dtu_clk5_timer_callback, 0);
    }
    //闹钟时间在今天当前时间之前，第二天再动作
    else
    {
        //开启定时器
        OSATimerStart(dtu_clock5_timer_ref, (time_sec_alarm - time_sec_now + DTU_CLK_ONE_DAY_HAS_SEC) * 200, 0, dtu_clk5_timer_callback, 0);
    }
}

/**
  * Function    : dtu_clk5_timer_stop
  * Description : 闹钟5停止
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk5_timer_stop(void)
{
    //开启定时器
    OSATimerStop(dtu_clock5_timer_ref);

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
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&dtu_clock1_timer_ref);
    ASSERT(ret == OS_SUCCESS);

}

// End of file : am_clock.c 2023-10-24 9:25:57 by: zhaoning 

