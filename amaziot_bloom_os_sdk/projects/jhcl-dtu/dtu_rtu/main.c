//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-7-2
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-7-2
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "sockets.h"
#include "teldef.h"

#include "am_at.h"
#include "am_gpio.h"
#include "am_socket.h"

#include "am.h"

#include "utils_string.h"

// Private defines / typedefs ---------------------------------------------------

//关机：OnKeyPoweroff_Start()，重启：PM812_SW_RESET();

#define DTU_TASK_STACK_SIZE     DTU_DEFAULT_THREAD_STACKSIZE * 2

// Private variables ------------------------------------------------------------

static OSTaskRef dtu_task_ref = NULL;
static UINT32 dtu_task_stack[DTU_TASK_STACK_SIZE/sizeof(UINT32)];

// Public variables -------------------------------------------------------------

#ifdef DTU_TYPE_EXSIM
extern UINT32 SDK_INIT_SIM_SELECT_VALUE;
#endif

DTU_TIME_PARAM_T st_timer;

// Private functions prototypes -------------------------------------------------

static void dtu_main_check_net_thread(void * argv);
static void dtu_wait_dev_reg_net(void);

// Public functions prototypes --------------------------------------------------

extern BOOL IsAtCmdSrvReady(void);
extern void PM812_SW_RESET(void);
extern int SendATCMDWaitResp(int sATPInd,char *in_str, int timeout, char *ok_fmt, int ok_flag,
                            char *err_fmt, char *out_str, int resplen);

// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
//    s_uart_cfg uartCfg = {0};
    char ver_data[128] = {0};
    // 设置UART初始化电平，只能在Phase1Inits_enter调用
    /*
    0: 1.8v -- default
    1: 2.8V
    2: 3.3V
    */
//    UART_INIT_VOLT(2);
    UART_SET_RDY_REPORT_FLAG(0);
    snprintf(ver_data, 128, "SWVER: [%s] BUILDTIME: [%s %s]", DTU_VERSION, BUILD_DATE, BUILD_TIME);
    update_the_cp_ver(ver_data);    //可以使用AT+CPVER读取，部分客户需要提供版本给原厂生产，便于原厂识别版本号

#ifdef DTU_TYPE_EXSIM
    //使用SIM2
    SDK_INIT_SIM_SELECT_VALUE = 2;
#endif

}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    OSA_STATUS status = 0;
    s_uart_cfg uartCfg = {0};
    s_uart_cfg uart4Cfg = {0};
#ifdef DTU_BASED_ON_TCP
    DTU_SOCKET_PARAM_T* dtu_socket_ctx = NULL;
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:06:04 by: zhaoning */
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
#ifdef DTU_BASED_ON_TCP
    dtu_socket_ctx = dtu_get_socket_ctx();
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:06:09 by: zhaoning */
    dtu_uart_ctx = dtu_get_uart_ctx();
    //打开串口，并设置回调函数
    UART_OPEN(dtu_uart_data_recv_cbk); 
    //获取串口配置
    UART_GET_CONFIG(&uartCfg);
    
#ifdef DTU_UART_9600_BAUD
    uartCfg.baudrate = 9600;
#endif /* ifdef DTU_UART_9600_BAUD.2023-7-5 12:28:41 by: zhaoning */
#ifdef DTU_UART_38400_BAUD
    uartCfg.baudrate = 38400;
#endif /* ifdef DTU_UART_38400_BAUD.2023-7-5 12:28:41 by: zhaoning */
#ifdef DTU_UART_115200_BAUD
    uartCfg.baudrate = 115200;
#endif /* ifdef DTU_UART_115200_BAUD.2023-7-5 12:28:41 by: zhaoning */
    //设置串口配置
    UART_SET_CONFIG(&uartCfg);
    
#ifdef DTU_TYPE_GNSS_INCLUDE
    //配置串口4，用于接收amg10发来的gnss nmea信息
    UART4_OPEN(dtu_uart4_data_recv_cbk); 
    UART4_GET_CONFIG(&uart4Cfg);
    uart4Cfg.baudrate = 9600;
    UART4_SET_CONFIG(&uart4Cfg);
#endif

    //初始化信息存储文件，包括激活码，各种标志位等
    dtu_trans_conf_file_init();

#ifdef DTU_TYPE_3IN1
    //初始化sim卡
    dtu_trans_sim_init();
#endif

    //初始化led灯控制io
    dtu_trans_net_led_init();  
    
    printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    //初始化数据发送消息队列，包括串口透传数据和心跳注册包数据
    status = OSAMsgQCreate(&dtu_uart_ctx->dtu_msgq_uart, "dtu_msgq_uart", sizeof(DTU_MSG_UART_DATA_PARAM_T), 50, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);
    
#ifdef DTU_BASED_ON_TCP
    //初始化下行消息队列
    status = OSAMsgQCreate(&dtu_socket_ctx->dtu_msgq_socket_recv, "dtu_msgq_socket_recv", sizeof(DTU_SOCKET_RECV_TYPE_T), 5, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:05:59 by: zhaoning */
    
#ifdef DTU_TYPE_GNSS_INCLUDE
    //初始化gnss数据上报队列
    status = OSAMsgQCreate(&dtu_uart_ctx->dtu_msgq_uart4, "dtu_msgq_uart4", sizeof(DTU_MSG_UART_DATA_PARAM_T), 10, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);
#endif

    //初始化网络连接情况超时检查定时器
    status = OSATimerCreate(&st_timer.dtu_timer_ref);
    ASSERT(status == OS_SUCCESS);
#ifdef DTU_BASED_ON_TCP
    //下行数据接收线程
    sys_thread_new("dtu_sockrcv_thread", (lwip_thread_fn)dtu_sockrcv_thread, NULL, DTU_DEFAULT_THREAD_STACKSIZE * 5, 161);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:44:19 by: zhaoning */
    //串口数据接收线程
    sys_thread_new("dtu_uart_data_recv_thread", (lwip_thread_fn)dtu_uart_data_recv_thread, NULL, DTU_DEFAULT_THREAD_STACKSIZE * 4, 161);

#ifdef DTU_TYPE_GNSS_INCLUDE
    //创建gnss串口4接收任务
    sys_thread_new("dtu_uart4_data_send_thread", (lwip_thread_fn)dtu_uart4_data_send_thread, NULL, DTU_DEFAULT_THREAD_STACKSIZE * 4, 161);
#endif

    //初始化和服务器之间json通信任务
#ifdef DTU_TYPE_JSON_INCLUDE
    dtu_json_task_init();
#endif /* ifdef DTU_TYPE_CJSON_INCLUDE.2023-10-30 18:33:25 by: zhaoning */
    
#ifdef DTU_TYPE_MODBUS_INCLUDE
    //初始化modbus任务
    dtu_modbus_task_init();
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:30:49 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
    //初始化http任务
    dtu_http_s_task_init();
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:07:28 by: zhaoning */

    //创建主任务，包括网络连接，死循环中检查网络连接情况，断连后重新连接
    status = OSATaskCreate(&dtu_task_ref, dtu_task_stack, DTU_TASK_STACK_SIZE, 150, "main_task", dtu_main_check_net_thread, NULL);
    ASSERT(status == OS_SUCCESS);

}

/**
  * Function    : dtu_get_timer_ctx
  * Description : 获取定时器上下文
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_TIME_PARAM_T* dtu_get_timer_ctx(void)
{
    return &st_timer;
}

/**
  * Function    : dtu_wait_dev_reg_net
  * Description : 等待驻网，1分钟后失败重启
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static void wait_network_ready(void)
//{
//    int count = 0;
//    int ready = 0;

//    while (!ready)
//    {
//        if (getCeregReady(isMasterSim0() ? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
//        {
//            ready = 1;
//        }
//        catstudio_printf("wait_network_ready: %d s", count++);
//        if (count > 300)
//            PM812_SW_RESET();

//        OSATaskSleep(200);
//    }
//}

static void dtu_wait_dev_reg_net(void)
{
    int errorCnt = 0;
    
    while(!getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
    {
        printf("wait net ...\n");
        dtu_sleep(3);
        errorCnt++;
        if(errorCnt > 20)
        {
            //reset
            dtu_send_to_uart("\r\ncheck net error! moudle rebooting...\r\n", 40);

#ifdef DTU_TYPE_3IN1
            //sim switch
            dtu_trans_sim_switch();
#endif
            dtu_sleep(3);//delay for write file
            PM812_SW_RESET();
        }
    }
}

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_main_check_net_thread
  * Description : 维护驻网任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_main_check_net_thread(void * argv)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    DTU_SOCKET_PARAM_T* dtu_socket_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    dtu_socket_ctx = dtu_get_socket_ctx();
    
    //等待驻网
    dtu_wait_dev_reg_net();
//    wait_dev_check_csq();

PRO_START:
    //初始化socket fn
    dtu_set_socket_fn();
    //电量网络指示灯
    GpioSetLevel(GPIO_LINK_PIN, 0);
//    printf("udp00");
    //根据标志位设置是否需要打开心跳定时器
    if(dtu_file_ctx->hb.heartflag)
        OSATimerStart(st_timer.dtu_timer_ref, dtu_file_ctx->hb.hearttime * 200, dtu_file_ctx->hb.hearttime * 200 , dtu_hb_timer_callback, 0);
    //根据标志位设置是否拼接注册包
    if(dtu_file_ctx->reg.linkflag == 1 || dtu_file_ctx->reg.linkflag == 3)
    {
        UINT16 link_size = 0;
        char* hex_data = NULL;

#ifdef DTU_UART_JHCL
        if(dtu_file_ctx->reg.linktype == 0)
        {
            link_size = strlen(dtu_file_ctx->reg.link);
        }
        else if(dtu_file_ctx->reg.linktype == 1)
        {
            link_size = strlen(dtu_file_ctx->reg.link) / 2;
            hex_data = malloc(link_size);
            utils_ascii_str2hex(0, hex_data, dtu_file_ctx->reg.link, strlen(dtu_file_ctx->reg.link));
        }
        char *data = malloc(link_size);
        if(dtu_file_ctx->reg.linktype == 0)
        {
            memcpy(data, dtu_file_ctx->reg.link, link_size);
        }
        else if(dtu_file_ctx->reg.linktype == 1)
        {
            memcpy(data, hex_data, link_size);
        }
#else
        link_size = strlen(dtu_file_ctx->reg.link);

        char *data = malloc(link_size);
        memcpy(data, dtu_file_ctx->reg.link, link_size);
#endif /* ifdef DDTU_UART_JHCL.2023-10-31 15:33:10 by: zhaoning */

        //发送数据到服务器
        if(0 != dtu_socket_write((void *)data,link_size))
            goto PRO_START;
        //释放内存，置空
        if(NULL != hex_data)
        {
            free(hex_data);
            hex_data = NULL;
        }
        
        if(NULL != data)
        {
            free(data);
            data = NULL;
        }
    }
    
//    printf("debug> %s -- %u",__FUNCTION__,__LINE__);

    while (1)
    {
        //检查soket连接情况
        if(dtu_socket_ctx->fd)
        {
            dtu_sleep(1);
        }
        //断开连接后关闭网络指示灯，发送错误到串口
        else
        {
            printf("%s[%d] restart socket", __FUNCTION__,__LINE__); 
            //Close LinkA
            GpioSetLevel(GPIO_LINK_PIN, 0);
            dtu_send_to_uart("\r\nSOCKET RESTART\r\n", 18);
            //跳出循环重新初始化
            break;
        }
    }
    goto PRO_START;
}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:28:04 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
/**
  * Function    : dtu_mqtt_main_task
  * Description : 主任务，维护mqtt连接
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_main_check_net_thread(void *ptr)
{
    int rc = 0;
    int count = 0;
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;
    DTU_MQTT_PARAM_T* dtu_mqtt_ctx = NULL;

    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    dtu_mqtt_ctx = dtu_get_mqtt_ctx();
    
    //等待驻网
    dtu_wait_dev_reg_net();
PRO_START:
    //初始化mqtt
    dtu_mqtt_wait_init_mqtt();
    //连接mqtt服务器
    dtu_mqtt_wait_connect_mqtt();
    //串口发送成功信息
    dtu_send_to_uart("\r\nCONNECT OK\r\n", 14);
    //点亮网络灯
    GpioSetLevel(GPIO_LINK_PIN, 1);

    OSATaskSleep(100);//delay for sub error//dmh20210329
    //订阅Topic
    dtu_mqtt_subscribe_topic();

    //根据设置，确定是否打开心跳定时器
    if(dtu_mqtt_file_ctx->hb.heartflag)
    {
        OSATimerStart(st_timer.dtu_timer_ref, dtu_mqtt_file_ctx->hb.hearttime * 200, dtu_mqtt_file_ctx->hb.hearttime * 200 , dtu_hb_timer_callback, 0);
    }
    
    //根据设置，确定是否需要发送注册包
    if(dtu_mqtt_file_ctx->reg.linkflag == 1 || dtu_mqtt_file_ctx->reg.linkflag == 3)
    {
        dtu_mqtt_send((void *)dtu_mqtt_file_ctx->reg.link, strlen(dtu_mqtt_file_ctx->reg.link));
    }
    //检测是否连接，断开连接后跳出死循环，重新执行
    while(1)
    {
        if(dtu_mqtt_is_connect(dtu_mqtt_ctx->dtu_mqtt_client))
        {
            OSATaskSleep(200);
        }
        else
        {
            break;
        }
    }
    dtu_mqtt_ctx->dtu_mqtt_subflag = 0;
    dtu_send_to_uart("\r\nMQTT RESTART\r\n", 16);
    GpioSetLevel(GPIO_LINK_PIN, 0);
    
    goto PRO_START;
}
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:28:18 by: zhaoning */

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
    
//    printf("dtu_hb_timer_callback");
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
                printf("hb send err");
            }
            else
                printf("%s[%d] hb send len: %d\n", __FUNCTION__, __LINE__, heart.len);
        }
    }
}

// End of file : main.c 2023-7-2 17:24:37 by: zhaoning 

