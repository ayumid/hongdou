//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_gnss.c
// Auther      : zhaoning
// Version     :
// Date : 2023-9-11
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-11
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_gnss.h"
#include "utils_string.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static DTU_GNSS_NMEA_T dtu_gnss_nmea_t;
static OSATimerRef dtu_gnss_timer_ref;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_gnss_ctx
  * Description : 获取gnss上下文
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_GNSS_NMEA_T* dtu_get_gnss_ctx(void)
{
    return &dtu_gnss_nmea_t;
}

/**
  * Function    : dtu_gnss_data_prase
  * Description : 解析gnss语句，存入缓冲区
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_data_prase(char* gnss_data, UINT32 len)
{
    memset(dtu_gnss_nmea_t.a_nmea, 0, sizeof(dtu_gnss_nmea_t.a_nmea));
    memset(dtu_gnss_nmea_t.a_nmea_gngga, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gngga));
    memset(dtu_gnss_nmea_t.a_nmea_gngll, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gngll));
    memset(dtu_gnss_nmea_t.a_nmea_gngsa, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gngsa));
    memset(dtu_gnss_nmea_t.a_nmea_gpgsv, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gpgsv));
    memset(dtu_gnss_nmea_t.a_nmea_bdgsv, 0, sizeof(dtu_gnss_nmea_t.a_nmea_bdgsv));
    memset(dtu_gnss_nmea_t.a_nmea_gnrmc, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gnrmc));
    memset(dtu_gnss_nmea_t.a_nmea_gnvtg, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gnvtg));
    memset(dtu_gnss_nmea_t.a_nmea_gnzda, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gnzda));
    memset(dtu_gnss_nmea_t.a_nmea_gptxt, 0, sizeof(dtu_gnss_nmea_t.a_nmea_gptxt));
    //用户进行数据处理，此处demo仅将接收到的数据回写
    memcpy(dtu_gnss_nmea_t.a_nmea, gnss_data, len);
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gngga, "$GNGGA", "$GNGLL");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gngll, "$GNGLL", "$GNGSA");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gngsa, "$GNGSA", "$GPGSV");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gpgsv, "$GPGSV", "$BDGSV");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_bdgsv, "$BDGSV", "$GNRMC");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gnrmc, "$GNRMC", "$GNVTG");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gnvtg, "$GNVTG", "$GNZDA");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gnzda, "$GNZDA", "$GPTXT");
    utils_nmea_cpy(dtu_gnss_nmea_t.a_nmea, dtu_gnss_nmea_t.a_nmea_gptxt, "$GPTXT", NULL);

//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gngga);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gngll);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gngsa);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gpgsv);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_bdgsv);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gnrmc);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gnvtg);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gnzda);
//    uprintf("%s\n", dtu_gnss_nmea_t.a_nmea_gptxt);
}

/**
  * Function    : dtu_gnss_timer_init
  * Description : 定时器初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_timer_init(void)
{
    int ret = 0;
    
    //初始化网络连接情况超时检查定时器
    ret = OSATimerCreate(&dtu_gnss_timer_ref);
    ASSERT(ret == OS_SUCCESS);
}

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_gnss_timer_callback
  * Description : gnss超时函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    DTU_SOCKET_PARAM_T* dtu_socket_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    dtu_socket_ctx = dtu_get_socket_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_gnss_timer_callback");
    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        //判断是否配置了心跳
        if(dtu_socket_ctx->fd && dtu_file_ctx->gnss.gpsflag == 1)
        {
//            uprintf("%s --- %d",__FUNCTION__,__LINE__);
            //发送消息到上报线程
            DTU_MSG_UART_DATA_PARAM_T gnss = {0};

            gnss.id = DTU_UART4_MSG_ID_SEND;

            OSAMsgQSend(dtu_uart_ctx->dtu_msgq_uart4, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&gnss, OSA_NO_SUSPEND);
        }
    }
}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:16:56 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
/**
  * Function    : dtu_gnss_timer_callback
  * Description : gnss超时函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_timer_callback(UINT32 tmrId)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    DTU_MQTT_PARAM_T* dtu_mqtt_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    dtu_mqtt_ctx = dtu_get_mqtt_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    
//    uprintf("dtu_gnss_timer_callback");
    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        //判断是否配置了心跳
        if(dtu_mqtt_is_connect(dtu_mqtt_ctx->dtu_mqtt_client) && dtu_file_ctx->gnss.gpsflag == 1)
        {
//            uprintf("%s --- %d",__FUNCTION__,__LINE__);
            //发送消息到上报线程
            DTU_MSG_UART_DATA_PARAM_T gnss = {0};

            gnss.id = DTU_UART4_MSG_ID_SEND;

            OSAMsgQSend(dtu_uart_ctx->dtu_msgq_uart4, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&gnss, OSA_NO_SUSPEND);
        }
    }
}

#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:18:26 by: zhaoning */
/**
  * Function    : dtu_gnss_timer_start
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_timer_start(void)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    OSATimerStop(dtu_gnss_timer_ref);
    //根据标志位设置是否需要打开心跳定时器
    if(dtu_file_ctx->gnss.gpsflag)
        OSATimerStart(dtu_gnss_timer_ref, dtu_file_ctx->gnss.gpstime * 200, dtu_file_ctx->gnss.gpstime * 200 , dtu_gnss_timer_callback, 0);
}

/**
  * Function    : dtu_gnss_timer_stop
  * Description : 停止定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_timer_stop(void)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if(dtu_file_ctx->gnss.gpsflag)
        OSATimerStop(dtu_gnss_timer_ref);
}

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_send_gnss_data_to_server
  * Description : 发送gnss数据到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_send_gnss_data_to_server(void)
{
    int ret = 0;
    char gnss_buf[MODULES_GNSS_NMEA_RES_LEN] = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
        
    char *GNSS_NMEA[]={"ALL",    //读取 $GNGGA 语句 0
                       "RMC",    //读取 $GNGLL 语句 1
                       "GGA",    //读取 $GNGSA 语句 2
                       "GSA",    //读取 $GPGSV 语句 3
                       "GLL",    //读取 $BDGSV 语句 4
                       "GSV"};   //读取 $GPTXT 语句 5

    dtu_file_ctx = dtu_get_file_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        if(1 == dtu_file_ctx->gnss.gpsflag)
        {
            //根据配置选择上报内容
            if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[0],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s",dtu_gnss_nmea_t.a_nmea);
            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[1],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gnrmc);
            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[2],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gngga);

            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[3],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gngsa);

            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[4],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gngll);

            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[5],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s%s", dtu_gnss_nmea_t.a_nmea_gpgsv, dtu_gnss_nmea_t.a_nmea_bdgsv);
            }
//            uprintf("gnss send:\n%s\n", gnss_buf);
            //数据发送到服务器
            ret = dtu_socket_write((void *)gnss_buf, strlen(gnss_buf));
        }
    }
}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:16:37 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
/**
  * Function    : dtu_mqtt_send_gnss_data_to_server
  * Description : 发送gnss数据到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_send_gnss_data_to_server(void)
{
    int ret = 0;
    char gnss_buf[MODULES_GNSS_NMEA_RES_LEN] = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    DTU_MQTT_PARAM_T* dtu_mqtt_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
        
    char *GNSS_NMEA[]={"ALL",    //读取 $GNGGA 语句 0
                       "RMC",    //读取 $GNGLL 语句 1
                       "GGA",    //读取 $GNGSA 语句 2
                       "GSA",    //读取 $GPGSV 语句 3
                       "GLL",    //读取 $BDGSV 语句 4
                       "GSV"};   //读取 $GPTXT 语句 5

    dtu_file_ctx = dtu_get_file_ctx();
    dtu_mqtt_ctx = dtu_get_mqtt_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    //判断是否是在透传模式
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        if(0 != dtu_mqtt_is_connect(dtu_mqtt_ctx->dtu_mqtt_client) && 1 == dtu_file_ctx->gnss.gpsflag)
        {
            //根据配置选择上报内容
            if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[0],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s",dtu_gnss_nmea_t.a_nmea);
            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[1],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gnrmc);
            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[2],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gngga);

            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[3],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gngsa);

            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[4],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s", dtu_gnss_nmea_t.a_nmea_gngll);

            }
            else if(memcmp(dtu_file_ctx->gnss.gpsnmea, GNSS_NMEA[5],3) == 0)
            {
                snprintf(gnss_buf, MODULES_GNSS_NMEA_RES_LEN,"%s%s", dtu_gnss_nmea_t.a_nmea_gpgsv, dtu_gnss_nmea_t.a_nmea_bdgsv);
            }
//            uprintf("gnss send:\n%s\n", gnss_buf);
            //数据发送到服务器
            ret = dtu_mqtt_send((void *)gnss_buf, strlen(gnss_buf));
            if(0 != ret)
            {
                uprintf("%s[%d] gnss send err", __FUNCTION__, __LINE__);
            }
            else
                uprintf("%s[%d] gnss send len: %d\n", __FUNCTION__, __LINE__, strlen(gnss_buf));
        }
    }
}
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:18:35 by: zhaoning */

// End of file : am_gnss.c 2023-9-11 17:09:45 by: zhaoning 

