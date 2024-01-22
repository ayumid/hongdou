//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_lora_call.c
// Auther      : zhaoning
// Version     :
// Date : 2024-1-12
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-1-12
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_lora_call.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_LORA_CALL_MSGQ_MSG_SIZE              (sizeof(DTU_MSG_UART_DATA_PARAM_T))
#define DTU_LORA_CALL_TASK_MSGQ_QUEUE_SIZE            (50)

// Private variables ------------------------------------------------------------

static UINT8 dtu_lora_call_task_stack[DTU_LORA_CALL_TASK_STACK_SIZE];
static OSMsgQRef    dtu_lora_call_msgq = NULL;

static OSTaskRef dtu_lora_call_msgq_task_ref = NULL;

static DTU_LORA_T st_dtu_lora;

// Public variables -------------------------------------------------------------

extern DTU_FORM_FILE_PARAM_T st_dtu_form_file_t[256];

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_lora_time_task_send_msgq
  * Description : 巡检发送消息
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_lora_call_task_send_msgq(LORA_RCV_DATA_MSG_T * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(dtu_lora_call_msgq, DTU_LORA_CALL_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        uprintf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->len, status);
        ret = -1;
    }    
    
    return ret;
}

/**
  * Function    : dtu_lora_call_interval_timer_callback
  * Description : 指令间隔定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_lora_call_interval_timer_callback(UINT32 tmrId)
{
    int ret = 0;
    //发送消息到上报线程
    LORA_RCV_DATA_MSG_T uart_send = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();

    //如果是透传模式，发消息给巡检任务，驱动巡检
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        uart_send.id = DTU_LORA_TIMER_INTERVAL_MSG;
        
        dtu_lora_call_task_send_msgq(&uart_send);
    }
}

/**
  * Function    : dtu_lora_call_interval_timer_init
  * Description : 初始化指令间隔定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_lora_call_interval_timer_init(void)
{
    OSA_STATUS status = 0;
    
    //初始化指令间隔定时器
    status = OSATimerCreate(&st_dtu_lora.lora_call_ref);
    ASSERT(status == OS_SUCCESS);
    
}

/**
  * Function    : dtu_lora_call_interval_timer_start
  * Description : 开始指令间隔定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_lora_call_interval_timer_start(void)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();


    OSATimerStart(st_dtu_lora.lora_call_ref, dtu_file_ctx->lora.interval * 200, dtu_file_ctx->lora.interval * 200 , dtu_lora_call_interval_timer_callback, 0);
}

/**
  * Function    : dtu_lora_call_interval_timer_stop
  * Description : 停止指令间隔定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_lora_call_interval_timer_stop(void)
{
    OSATimerStop(st_dtu_lora.lora_call_ref);
}

/**
  * Function    : dtu_modbus_protocol_check
  * Description : 检测当前串口数据是否是modbus帧数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
/*UINT8 dtu_modbus_protocol_check(UINT8* data, UINT8 len)
{
    UINT8 res = DTU_MODBUS_PROTOCOL_SUCCESS;
    UINT16 crc16mb = 0xffff;
    UINT8 crc16a = 0;
    UINT8 crc16b = 0;
    int i = 0;
    int j = 0;

    //此处的len - 2    -- 要校验的位数为一帧数据减去末尾两个校验个
    for(i = 0; i < len - 2; i++)
    {
        crc16mb = data[i] ^ crc16mb;
        //此处的8 -- 指每一个char类型又8bit，每bit都要处理
        for(j = 0; j < 8; j++)
        {
            if(crc16mb & 0x01)
            {
                crc16mb = crc16mb >> 1;
                crc16mb = crc16mb ^ 0xa001;
            }   
            else
            {
                crc16mb = crc16mb >> 1;
            }   
        }   
    }   
    //CRC校验后的值
//    uprintf("crc16: 0x%04X\n",crc16mb);
    crc16a = crc16mb >> 8;
    crc16b = crc16mb;
//    uprintf("a: 0x%X b: 0x%X\n",crc16a, crc16b);
    //此时的crc16mb和modbus帧里末尾两字节颠倒
    //如果crc16mb和传入校验数据不一样，返回错误
    if(data[len - 2] != crc16b || data[len - 1] != crc16a)
    {
        res = DTU_MODBUS_PROTOCOL_ERROR;
    }

    return res;
}*/

/**
  * Function    : dtu_lora_call_task
  * Description : modbus主任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_lora_call_task(void *ptr)
{
    OSA_STATUS status = 0;
    int ret = 0;
    int i = 0;
    LORA_RCV_DATA_MSG_T rcv_data = {0};
    UINT32 dtime = 0;

    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    dtu_lora_call_interval_timer_init();
    dtu_lora_call_interval_timer_start();

    st_dtu_lora.id = 0;
//    st_dtu_form_file_t[0].id = 1;
//    st_dtu_form_file_t[0].day_timestamp = 61200;
//    memcpy(st_dtu_form_file_t[0].devid, "LORA000001", strlen("LORA000001"));
    while(1)
    {
        //阻塞1s等待从机回复的数据
        status = OSAMsgQRecv(dtu_lora_call_msgq, (void *)&rcv_data, sizeof(LORA_RCV_DATA_MSG_T), OSA_SUSPEND);

        if(status == OS_SUCCESS)
        {
            if(DTU_LORA_TIMER_INTERVAL_MSG == rcv_data.id)
            {
                uprintf("%s[%d] timer msg\r\n", __FUNCTION__, __LINE__);
                    //for循环查询列表中哪些指令激活，按照激活指令参数配置 指令
                for(i = st_dtu_lora.id; i < DTU_LORA_SLAVE_NUM; i++)
                {
                    //判断时间是否超时，超时后点名
                    dtime = utils_utc8_2_day_timestamp();
                    
                    if(0 != strlen(st_dtu_form_file_t[i].devid))
                    {
                        uprintf("%s[%d] slave id: %s\n", __FUNCTION__, __LINE__, st_dtu_form_file_t[i].devid);

                    }
                    else
                    {
//                        uprintf("%s[%d] none cmd continue i:%d id: %d\n", __FUNCTION__, __LINE__, i + 1, st_dtu_lora.id);
                        st_dtu_lora.id = i + 1;
                        continue;
                    }
                    
                    //当前激活id，跳出这次循环
                    break;
                }
                st_dtu_lora.id++;
//                    uprintf("%s[%d] sendbuf len: %d mdid: %d\r\n", __FUNCTION__, __LINE__, send_len, st_dtu_lora.id);
                if(st_dtu_lora.id > DTU_LORA_SLAVE_NUM)
                {
                    st_dtu_lora.id = 0;
                    uprintf("%s[%d] poolling  end", __FUNCTION__, __LINE__);

                }
                else
                {
                    DTU_LORA_DATA_HEAD_T data = {0};
                    data.maddr = 0xFFFF;
                    data.daddr = st_dtu_lora.id;
                    memcpy(data.devid, st_dtu_form_file_t[i].devid, strlen(st_dtu_form_file_t[i].devid));
                    data.cmd = 1;
                    //发送数据到从机
                    dev_lora_module_packet_init(sizeof(DTU_LORA_DATA_HEAD_T));
                    dev_lora_send((UINT8*)&data, sizeof(DTU_LORA_DATA_HEAD_T));
//                    dev_lora_module_packet_init(strlen("LORA000001"));
//                    dev_lora_send((UINT8*)"LORA000001", strlen("LORA000001"));
                    uprintf("%s[%d] send dev:%d", __FUNCTION__, __LINE__, st_dtu_lora.id - 1);
                }
            }
            else if(DTU_LORA_TIMER_DATA_MSG == rcv_data.id && NULL != rcv_data.data)
            {
                //从机回复了，上报到服务器
                uprintf("lora time slave res");
                //modbus rtu帧，直接发
#ifdef DTU_BASED_ON_TCP
                dtu_socket_write(rcv_data.data, rcv_data.len);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-31 12:01:18 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
                dtu_mqtt_send(rcv_data.data, rcv_data.len);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-31 12:01:28 by: zhaoning */
#ifdef DTU_TYPE_HTTP_INCLUDE
                DTU_MSG_UART_DATA_PARAM_T uart_send_temp = {0};
                if(DTU_HTTP_TYPE_ENABLE == dtu_file_ctx->http.config.type)
                {
                    uart_send_temp.UArgs = malloc(rcv_data.len);
                    memcpy(uart_send_temp.UArgs, rcv_data.data, rcv_data.len);
                    uart_send_temp.len = rcv_data.len;
                    dtu_http_s_task_send_msgq(&uart_send_temp);//发送串口收到的数据给HTTP任务进行处理
                }
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:08:57 by: zhaoning */

                //释放数据内存
                free(rcv_data.data);
                //置空
                rcv_data.data = NULL;

            }
        }
    }
}

/**
  * Function    : dtu_lora_call_task_init
  * Description : 初始化modbus任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_lora_call_task_init(void)
{
    OSA_STATUS status = 0;

    /*creat message*/
    status = OSAMsgQCreate(&dtu_lora_call_msgq, "dtu_lora_call_msgq", DTU_LORA_CALL_MSGQ_MSG_SIZE, DTU_LORA_CALL_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSATaskCreate(&dtu_lora_call_msgq_task_ref, dtu_lora_call_task_stack, DTU_LORA_CALL_TASK_STACK_SIZE, 151, "dtu_lora_call_task", dtu_lora_call_task, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_lora_call.c 2024-1-12 12:14:40 by: zhaoning 

