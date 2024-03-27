//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_lora_time.c
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

#include "am_lora_time.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_LORA_TIME_MSGQ_MSG_SIZE              (sizeof(DTU_MSG_UART_DATA_PARAM_T))
#define DTU_LORA_TIME_TASK_MSGQ_QUEUE_SIZE            (50)

// Private variables ------------------------------------------------------------

static UINT8 dtu_lora_time_task_stack[DTU_LORA_TIME_TASK_STACK_SIZE];

static OSTaskRef dtu_lora_time_msgq_task_ref = NULL;

static DTU_LORA_T st_dtu_lora;

// Public variables -------------------------------------------------------------

extern DTU_FORM_FILE_PARAM_T st_dtu_form_file_t[256];

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

extern OSMsgQRef lora_data_msgq;

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_lora_time_task
  * Description : modbus主任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_lora_time_task(void *ptr)
{
    OSA_STATUS status = 0;
    int ret = 0;
    int i = 0;
//    int id = 0;
    int send_len = 0;
    LORA_RCV_DATA_MSG_T rcv_data = {0};
    UINT32 dtime = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    while(1)
    {
        //阻塞1s等待从机回复的数据
        status = OSAMsgQRecv(lora_data_msgq, (void *)&rcv_data, sizeof(LORA_RCV_DATA_MSG_T), OSA_SUSPEND);

        if(status == OS_SUCCESS)
        {
            if(DTU_LORA_TIMER_DATA_MSG == rcv_data.id && NULL != rcv_data.data)
            {
                //从机回复了，上报到服务器
                uprintf("lora time slave proactively res");

                do 
                {
                    DTU_LORA_DATA_HEAD_T* pst = NULL;
                    DTU_LORA_RES_DATA_HEAD_T data = {0};

                    pst = (DTU_LORA_DATA_HEAD_T*)rcv_data.data;
                    uprintf("lg: 0x%X id: %d devid: %s cmd: %d\n", pst->maddr, pst->daddr, pst->devid, pst->cmd);
                    //判断消息ID,如果不是发送给主机的，跳出
                    if(0xFFFF != pst->maddr)
                    {
                        break;
                    }
                    uprintf("stroedevid: %s", st_dtu_form_file_t[pst->daddr - 1].devid);
                    //判断消息ID后面是否是从机ID，ID前缀是否和设置一样
                    if(0 != strncmp(st_dtu_form_file_t[pst->daddr - 1].devid, pst->devid, DTU_LORA_ID_MAX_LEN))
                    {
                        break;
                    }
                    
                    if(DTU_LORA_CMD1 == pst->cmd)
                    {
                        data.maddr = 0xFFFF;
                        data.daddr = st_dtu_lora.id;
                        data.timetsamp = st_dtu_form_file_t[pst->daddr - 1].day_timestamp;
                        //发送数据到从机
                        dev_lora_module_packet_init(sizeof(DTU_LORA_RES_DATA_HEAD_T));
                        dev_lora_send((void*)&data, sizeof(DTU_LORA_RES_DATA_HEAD_T));
                    }
                    else if(DTU_LORA_CMD2 == pst->cmd)
                    {
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
                    }
                }while(0);
                //释放数据内存
                free(rcv_data.data);
                //置空
                rcv_data.data = NULL;
                
            }
        }
    }
}

/**
  * Function    : dtu_lora_time_task_init
  * Description : 初始化modbus任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_lora_time_task_init(void)
{
    OSA_STATUS status = 0;

    status = OSATaskCreate(&dtu_lora_time_msgq_task_ref, dtu_lora_time_task_stack, DTU_LORA_TIME_TASK_STACK_SIZE, 151, "dtu_lora_time_task", dtu_lora_time_task, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_lora_time.c 2024-1-12 12:14:40 by: zhaoning 

