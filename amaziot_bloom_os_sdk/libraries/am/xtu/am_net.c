//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_net.c
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

#include "sockets.h"
#include "teldef.h"

#include "am_net.h"
#include "am_gpio.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_TASK_STACK_SIZE     DTU_DEFAULT_THREAD_STACKSIZE * 2

// Private variables ------------------------------------------------------------

static OSTaskRef dtu_task_ref = NULL;
static UINT32 dtu_task_stack[DTU_TASK_STACK_SIZE / sizeof(UINT32)];

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void dtu_main_check_net_thread(void * argv);
static void dtu_wait_dev_reg_net(void);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

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
        uprintf("wait net ...\n");
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

    //根据标志位设置是否需要打开心跳定时器
    if(dtu_file_ctx->hb.heartflag == 1)
    {
        dtu_hb_timer_start();
    }
    //根据标志位设置是否拼接注册包
    if(dtu_file_ctx->reg.linkflag == 1 || dtu_file_ctx->reg.linkflag == 3)
    {
        UINT16 link_size = 0;
        char* hex_data = NULL;
        
        link_size = strlen(dtu_file_ctx->reg.link);

        char *data = malloc(link_size);
        memcpy(data, dtu_file_ctx->reg.link, link_size);

        //发送数据到服务器
        if(0 != dtu_socket_write((void *)data,link_size))
        {
            goto PRO_START;
        }
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
#ifdef DTU_TYPE_GNSS_INCLUDE
    if(1 == dtu_file_ctx->gnss.gpsflag)
    {
        dtu_gnss_timer_start();
    }
#endif /* ifdef DTU_TYPE_GNSS_INCLUDE.2023-11-7 15:11:09 by: zhaoning */
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
            uprintf("%s[%d] restart socket", __FUNCTION__,__LINE__); 
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
    OSATaskSleep(100);//delay for sub error
    //订阅Topic
    dtu_mqtt_subscribe_topic();
    //根据设置，确定是否打开心跳定时器
    if(1 == dtu_mqtt_file_ctx->hb.heartflag)
    {
        dtu_hb_timer_start();
    }
    //根据设置，确定是否需要发送注册包
    if(dtu_mqtt_file_ctx->reg.linkflag == 1 || dtu_mqtt_file_ctx->reg.linkflag == 3)
    {
        dtu_mqtt_send((void *)dtu_mqtt_file_ctx->reg.link, strlen(dtu_mqtt_file_ctx->reg.link));
    }
#ifdef DTU_TYPE_GNSS_INCLUDE
    if(1 == dtu_mqtt_file_ctx->gnss.gpsflag)
    {
        dtu_gnss_timer_start();
    }
#endif /* ifdef DTU_TYPE_GNSS_INCLUDE.2023-11-7 15:11:09 by: zhaoning */
    while(1)//检测是否连接，断开连接后跳出死循环，重新执行
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
  * Function    : dtu_checknet_task_init
  * Description : 创建网络重连任务，以及相关资源初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_checknet_task_init(void)
{
    OSA_STATUS status = 0;
    
    //创建主任务，包括网络连接，死循环中检查网络连接情况，断连后重新连接
    status = OSATaskCreate(&dtu_task_ref, dtu_task_stack, DTU_TASK_STACK_SIZE, 150, "main_task", dtu_main_check_net_thread, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_net.c 2023-11-17 9:25:42 by: zhaoning 

