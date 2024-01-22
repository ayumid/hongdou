//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_hb.c
// Auther      : zhaoning
// Version     :
// Date : 2023-11-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-11-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_hb.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

DTU_HB_TIME_PARAM_T st_hb_timer;

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_hb_timer_ctx
  * Description : 获取定时器上下文
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_HB_TIME_PARAM_T* dtu_get_hb_timer_ctx(void)
{
    return &st_hb_timer;
}

/**
  * Function    : dtu_hb_timer_callback
  * Description : 心跳定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_hb_timer_callback(UINT32 tmrId)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
#ifdef DTU_BASED_ON_TCP
    DTU_SOCKET_PARAM_T* dtu_socket_ctx = NULL;
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:32:10 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    DTU_MQTT_PARAM_T* dtu_mqtt_ctx = NULL;
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:32:27 by: zhaoning */
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
#ifdef DTU_BASED_ON_TCP
    dtu_socket_ctx = dtu_get_socket_ctx();
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:32:14 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    dtu_mqtt_ctx = dtu_get_mqtt_ctx();
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:32:33 by: zhaoning */
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_hb_timer_callback");
    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        //判断是否配置了心跳
#ifdef DTU_BASED_ON_TCP
        if(dtu_socket_ctx->fd && dtu_file_ctx->hb.heartflag == 1)
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:31:29 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
        if(dtu_mqtt_is_connect(dtu_mqtt_ctx->dtu_mqtt_client) && dtu_file_ctx->hb.heartflag == 1)
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:31:39 by: zhaoning */
        {
            //发送消息到上报线程
            DTU_MSG_UART_DATA_PARAM_T heart = {0};
            char *heartbuf = (char *)malloc(100);
            memset(heartbuf, 0x0, 100);
            sprintf(heartbuf, "%s", dtu_file_ctx->hb.heart);
            heart.len = strlen(heartbuf);
            heart.UArgs = (UINT8 *)heartbuf;
            ret = OSAMsgQSend(dtu_uart_ctx->dtu_msgq_uart, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&heart, OSA_NO_SUSPEND);
            if(0 != ret)
            {
                uprintf("hb send err");
            }
            else
                uprintf("%s[%d] hb send len: %d\n", __FUNCTION__, __LINE__, heart.len);
        }
    }
}

/**
  * Function    : dtu_hb_timer_stop
  * Description : 关闭心跳定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_hb_timer_stop(void)
{
#ifdef DTU_BASED_ON_TCP
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    if(dtu_file_ctx->hb.heartflag)
    {
        OSATimerStop(st_hb_timer.dtu_hb_timer_ref);
    }
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-17 9:36:08 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;

    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    
    if(dtu_mqtt_file_ctx->hb.heartflag)
    {
        OSATimerStop(st_hb_timer.dtu_hb_timer_ref);
    }
#endif /* ifdef DTU_BASED_ON_MQTT.2023-11-17 9:38:27 by: zhaoning */

}

/**
  * Function    : dtu_hb_timer_start
  * Description : 打开心跳定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_hb_timer_start(void)
{
#ifdef DTU_BASED_ON_TCP
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    if(dtu_file_ctx->hb.heartflag)
    {
        OSATimerStart(st_hb_timer.dtu_hb_timer_ref, dtu_file_ctx->hb.hearttime * 200, dtu_file_ctx->hb.hearttime * 200 , dtu_hb_timer_callback, 0);
    }
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-17 9:36:08 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;

    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    
    if(dtu_mqtt_file_ctx->hb.heartflag)
    {
        OSATimerStart(st_hb_timer.dtu_hb_timer_ref, dtu_mqtt_file_ctx->hb.hearttime * 200, dtu_mqtt_file_ctx->hb.hearttime * 200 , dtu_hb_timer_callback, 0);
    }
#endif /* ifdef DTU_BASED_ON_MQTT.2023-11-17 9:38:27 by: zhaoning */
}

/**
  * Function    : dtu_hb_timer_init
  * Description : 心跳定时器初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_hb_timer_init(void)
{
    OSA_STATUS status = 0;

    //初始化网络连接情况超时检查定时器
    status = OSATimerCreate(&st_hb_timer.dtu_hb_timer_ref);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_hb.c 2023-11-17 9:25:48 by: zhaoning 

