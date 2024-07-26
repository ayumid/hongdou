//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_485.c
// Auther      : zhaoning
// Version     :
// Date : 2023-9-20
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-20
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_485.h"
//#include "am_file.h"
#include "agile_modbus.h"
#include "utils_common.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_485_TASK_MSGQ_MSG_SIZE              (sizeof(DTU_MSG_UART_DATA_PARAM_T))
#define DTU_485_TASK_MSGQ_QUEUE_SIZE            (50)

#define SW_DTU_TIMEOUT                  60

// Private variables ------------------------------------------------------------

static UINT8 dtu_485_task_stack[DTU_MODBUS_TASK_STACK_SIZE];
static OSMsgQRef    dtu_485_msgq = NULL;

static OSTaskRef dtu_485_msgq_task_ref = NULL;

static UINT8 dtu_alive_task_stack[DTU_MODBUS_TASK_STACK_SIZE];
//static OSMsgQRef    dtu_alive_msgq = NULL;

static OSTaskRef dtu_alive_msgq_task_ref = NULL;

static DTU_MODBUS_T st_dtu_md;

SW_DTU_ALIVE sw_dtu_alive[SW_DTU_NUM_MAX];

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : sw_dtu_get_alive
  * Description : 获取在线标志
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
SW_DTU_ALIVE* sw_dtu_get_alive(void)
{
    return sw_dtu_alive;
}

/**
  * Function    : dtu_485_task_send_msgq
  * Description : modbus发送消息
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_485_task_send_msgq(DTU_MSG_UART_DATA_PARAM_T * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(dtu_485_msgq, DTU_485_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        uprintf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->len, status);
        ret = -1;
    }    
    
    return ret;
}

/**
  * Function    : dtu_485_interval_timer_callback
  * Description : 指令间隔定时器超时回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_485_interval_timer_callback(UINT32 tmrId)
{
    int ret = 0;
    //发送消息到上报线程
    DTU_MSG_UART_DATA_PARAM_T uart_send = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();

    //如果是透传模式，发消息给modbus任务，驱动modbus
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        uart_send.id = DTU_MODBUS_TIMER_INTERVAL_MSG;
        
        dtu_485_task_send_msgq(&uart_send);
    }
}

/**
  * Function    : dtu_485_interval_timer_init
  * Description : 初始化指令间隔定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_485_interval_timer_init(void)
{
    OSA_STATUS status = 0;
    
    //初始化指令间隔定时器
    status = OSATimerCreate(&st_dtu_md.md_timer_ref);
    ASSERT(status == OS_SUCCESS);
    
}

/**
  * Function    : dtu_modbus_interval_timer_start
  * Description : 开始指令间隔定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_interval_timer_start(void)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    OSATimerStart(st_dtu_md.md_timer_ref, dtu_file_ctx->modbus.config.interval * 200, dtu_file_ctx->modbus.config.interval * 200 , dtu_485_interval_timer_callback, 0);
}

/**
  * Function    : dtu_modbus_interval_timer_stop
  * Description : 停止指令间隔定时器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_interval_timer_stop(void)
{
    OSATimerStop(st_dtu_md.md_timer_ref);
}

/**
  * Function    : dtu_485_protocol_check
  * Description : 检测当前串口数据是否是modbus帧数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
/*UINT8 dtu_485_protocol_check(UINT8* data, UINT8 len)
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
  * Function    : dtu_485_task
  * Description : modbus主任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_485_task(void *ptr)
{
    OSA_STATUS status = 0;
    int ret = 0;
    int i = 0;
//    int id = 0;
    int send_len = 0;
    DTU_MSG_UART_DATA_PARAM_T uart_data = {0};
    uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
    uint8_t ctx_read_buf[1];
    UINT32 timestamp = 0;
    UINT16 id = 0;
//    uint8_t send[100] = {0};
//    uint8_t recv[100] = {0};
//    uint8_t* temp = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    agile_modbus_rtu_t ctx_dtu = {0};
    agile_modbus_t *ctx = &ctx_dtu._ctx;
    agile_modbus_rtu_init(&ctx_dtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));

    dtu_485_interval_timer_init();
    st_dtu_md.state = DTU_MODBUS_POOLLING_STATE;

    if(DTU_MODBUS_TYPE_ENABLE == dtu_file_ctx->modbus.config.type)
    {
        dtu_modbus_interval_timer_start();
    }
//#define AGILE_MODBUS_FC_READ_COILS               0x01
//#define AGILE_MODBUS_FC_READ_DISCRETE_INPUTS     0x02
//#define AGILE_MODBUS_FC_READ_HOLDING_REGISTERS   0x03
//#define AGILE_MODBUS_FC_READ_INPUT_REGISTERS     0x04
//#define AGILE_MODBUS_FC_WRITE_SINGLE_COIL        0x05
//#define AGILE_MODBUS_FC_WRITE_SINGLE_REGISTER    0x06
//#define AGILE_MODBUS_FC_READ_EXCEPTION_STATUS    0x07
//#define AGILE_MODBUS_FC_WRITE_MULTIPLE_COILS     0x0F
//#define AGILE_MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10
//#define AGILE_MODBUS_FC_REPORT_SLAVE_ID          0x11
//#define AGILE_MODBUS_FC_MASK_WRITE_REGISTER      0x16
//#define AGILE_MODBUS_FC_WRITE_AND_READ_REGISTERS 0x17
    while(1)
    {
        //阻塞1s等待从机回复的数据
        status = OSAMsgQRecv(dtu_485_msgq, (void *)&uart_data, DTU_485_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);

        if(status == OS_SUCCESS)
        {
            if(DTU_MODBUS_TIMER_INTERVAL_MSG == uart_data.id)
            {
//                uprintf("%s[%d] timer msg\r\n", __FUNCTION__, __LINE__);
                if(DTU_MODBUS_POOLLING_STATE == st_dtu_md.state)
                {
                    //for循环查询列表中哪些指令激活，按照激活指令参数配置 指令
                    for(i = st_dtu_md.id; i < DTU_MODBUS_CMD_NUM; i++)
                    {
                        //当前列表第i指令被激活
                        if(DTU_MODBUS_ACTIVE == dtu_file_ctx->modbus.cmd[i].active)
                        {
                            //设置从机地址
                            agile_modbus_set_slave(ctx, dtu_file_ctx->modbus.cmd[i].slave_addr);
                            uprintf("%s[%d] slave addr: %d id: %d cmd: 0x%02X\n", __FUNCTION__, __LINE__, dtu_file_ctx->modbus.cmd[i].slave_addr, st_dtu_md.id + 1, dtu_file_ctx->modbus.cmd[i].fn);
                        }
                        else
                        {
//                            uprintf("%s[%d] none cmd continue i:%d id: %d\n", __FUNCTION__, __LINE__, i + 1, st_dtu_md.id);
                            st_dtu_md.id = i + 1;
                            continue;
                        }
                        //根据指令01 来组包
                        if(dtu_file_ctx->modbus.cmd[i].fn == AGILE_MODBUS_FC_READ_COILS)
                        {
                            //组包数据
                            send_len = agile_modbus_serialize_read_bits(ctx, dtu_file_ctx->modbus.cmd[i].reg_addr, dtu_file_ctx->modbus.cmd[i].reg_n_d);
                        }
                        //根据指令02 来组包
                        else if(dtu_file_ctx->modbus.cmd[i].fn == AGILE_MODBUS_FC_READ_DISCRETE_INPUTS)
                        {
                            //组包数据
                            send_len = agile_modbus_serialize_read_input_bits(ctx, dtu_file_ctx->modbus.cmd[i].reg_addr, dtu_file_ctx->modbus.cmd[i].reg_n_d);
                        }
                        //根据指令03 来组包
                        else if(dtu_file_ctx->modbus.cmd[i].fn == AGILE_MODBUS_FC_READ_HOLDING_REGISTERS)
                        {
                            //组包数据
                            send_len = agile_modbus_serialize_read_registers(ctx, dtu_file_ctx->modbus.cmd[i].reg_addr, dtu_file_ctx->modbus.cmd[i].reg_n_d);
                        }
                        //根据指令04 来组包
                        else if(dtu_file_ctx->modbus.cmd[i].fn == AGILE_MODBUS_FC_READ_INPUT_REGISTERS)
                        {
                            //组包数据
                            send_len = agile_modbus_serialize_read_input_registers(ctx, dtu_file_ctx->modbus.cmd[i].reg_addr, dtu_file_ctx->modbus.cmd[i].reg_n_d);
                        }
                        //根据指令05 来组包
                        else if(dtu_file_ctx->modbus.cmd[i].fn == AGILE_MODBUS_FC_WRITE_SINGLE_COIL)
                        {
                            //组包数据
                            send_len = agile_modbus_serialize_write_bit(ctx, dtu_file_ctx->modbus.cmd[i].reg_addr, dtu_file_ctx->modbus.cmd[i].reg_n_d);
                        }
                        //根据指令06 来组包
                        else if(dtu_file_ctx->modbus.cmd[i].fn == AGILE_MODBUS_FC_WRITE_SINGLE_REGISTER)
                        {
                            //组包数据
                            send_len = agile_modbus_serialize_write_register(ctx, dtu_file_ctx->modbus.cmd[i].reg_addr, dtu_file_ctx->modbus.cmd[i].reg_n_d);
                        }
//                        uprintf("%s[%d] find cmd\r\n", __FUNCTION__, __LINE__);
                        //当前激活id，跳出这次循环
                        break;
                    }
                    st_dtu_md.id++;
//                    uprintf("%s[%d] sendbuf len: %d mdid: %d\r\n", __FUNCTION__, __LINE__, send_len, st_dtu_md.id);
                    if(st_dtu_md.id > DTU_MODBUS_CMD_NUM)
                    {
                        st_dtu_md.id = 0;
                        st_dtu_md.state = DTU_MODBUS_POOLLING_WN_STATE;
                        uprintf("%s[%d] poolling  end", __FUNCTION__, __LINE__);
                        //停止modbus驱动定时器
//                        dtu_modbus_interval_timer_stop();
                        //轮训完毕后，重新开始之前等待时间
//                        dtu_sleep(dtu_file_ctx->modbus.config.delay);
                        //开始modbus驱动定时器
//                        dtu_modbus_interval_timer_start();
                    }
                    else
                    {
//                        uprintf("%s[%d] send md hex\r\n", __FUNCTION__, __LINE__);
//                        int j = 0;
//                        for(j = 0; j < send_len; j++)
//                        {
//                            uprintf("reg data[%d]: %X\r\n", j, ctx->send_buf[j]);
//                        }
                        //发送数据到串口
                        UART_SEND_DATA(ctx->send_buf, send_len);
                        //根据第一个字节来确定，收到了哪台设备的回复，标记时间，存储
                        timestamp = utils_utc8_2_timestamp();
                        id = *ctx->send_buf;
                        
                        sw_dtu_alive[id].timercv = timestamp;
                        uprintf("poll %d: %d\r\n", id, sw_dtu_alive[id].timercv);
                        
                    }
                }
                else if(DTU_MODBUS_POOLLING_WN_STATE == st_dtu_md.state)
                {
                    //for循环查询列表中哪些指令激活，按照激活指令参数配置 指令
                    for(i = st_dtu_md.id; i < DTU_MODBUS_CMD_WN_NUM; i++)
                    {
                        //当前列表第i指令被激活
                        if(DTU_MODBUS_ACTIVE == dtu_file_ctx->modbus.cmd_wn[i].active)
                        {
                            //设置从机地址
                            agile_modbus_set_slave(ctx, dtu_file_ctx->modbus.cmd_wn[i].slave_addr);
                            uprintf("%s[%d] slave addr: %d id: %d cmd: 0x%02X", __FUNCTION__, __LINE__, dtu_file_ctx->modbus.cmd_wn[i].slave_addr, st_dtu_md.id + 1, dtu_file_ctx->modbus.cmd_wn[i].fn);
                        }
                        else
                        {
//                            uprintf("%s[%d] none cmd continue i:%d id: %d\n", __FUNCTION__, __LINE__, i + 1, st_dtu_md.id);
                            st_dtu_md.id = i + 1;
                            continue;
                        }
                        
                        //根据指令0F 来组包
//                        if(dtu_file_ctx->modbus.cmd_wn[i].fn == AGILE_MODBUS_FC_WRITE_MULTIPLE_COILS)
//                        {
//                            uprintf("cmd 0x10 regnum: %d\r\n", dtu_file_ctx->modbus.cmd_wn[i].reg_n_d);
                            //组包数据
//                            send_len = agile_modbus_serialize_write_registers(ctx, dtu_file_ctx->modbus.cmd_wn[i].reg_addr, dtu_file_ctx->modbus.cmd_wn[i].reg_n, (UINT16*)&dtu_file_ctx->modbus.cmd_wn[i].reg_data);
//                            int j = 0;
//                            for(j = 0; j < dtu_file_ctx->modbus.cmd_wn[i].reg_n_d; j++)
//                            {
//                                uprintf("reg data[%d]: %X\r\n", j, dtu_file_ctx->modbus.cmd_wn[i].reg_data[j]);
//                            }
//                        }
                        //根据指令10 来组包
                        if(dtu_file_ctx->modbus.cmd_wn[i].fn == AGILE_MODBUS_FC_WRITE_MULTIPLE_REGISTERS)
                        {
//                            uprintf("cmd 0x10 regnum: %d\r\n", dtu_file_ctx->modbus.cmd_wn[i].reg_n_d);
                            //组包数据
                            send_len = agile_modbus_serialize_write_registers(ctx, dtu_file_ctx->modbus.cmd_wn[i].reg_addr, dtu_file_ctx->modbus.cmd_wn[i].reg_n, (UINT16*)&dtu_file_ctx->modbus.cmd_wn[i].reg_data);
//                            int j = 0;
//                            for(j = 0; j < dtu_file_ctx->modbus.cmd_wn[i].reg_n_d; j++)
//                            {
//                                uprintf("reg data[%d]: %X\r\n", j, dtu_file_ctx->modbus.cmd_wn[i].reg_data[j]);
//                            }
                        }
//                        uprintf("%s[%d] find cmd\r\n", __FUNCTION__, __LINE__);
                        //当前激活id，跳出这次循环
                        break;
                    }
                    st_dtu_md.id++;
//                    uprintf("%s[%d] sendbuf len: %d mdid: %d\r\n", __FUNCTION__, __LINE__, send_len, st_dtu_md.id);
                    if(st_dtu_md.id > DTU_MODBUS_CMD_WN_NUM)
                    {
                        st_dtu_md.id = 0;
                        st_dtu_md.state = DTU_MODBUS_POOLLING_STATE;
                        uprintf("%s[%d] wn poolling end", __FUNCTION__, __LINE__);
                        //停止modbus驱动定时器
                        dtu_modbus_interval_timer_stop();
                        //轮训完毕后，重新开始之前等待时间
                        dtu_sleep(dtu_file_ctx->modbus.config.delay);
                        //开始modbus驱动定时器
                        dtu_modbus_interval_timer_start();
                    }
                    else
                    {
//                        uprintf("%s[%d] send md hex\r\n", __FUNCTION__, __LINE__);
//                        int j = 0;
//                        for(j = 0; j < send_len; j++)
//                        {
//                            uprintf("reg data[%d]: %X\r\n", j, ctx->send_buf[j]);
//                        }
                        //发送数据到串口
                        UART_SEND_DATA(ctx->send_buf, send_len);
                        //根据第一个字节来确定，收到了哪台设备的回复，标记时间，存储
                        timestamp = utils_utc8_2_timestamp();
                        id = *ctx->send_buf;
                        
                        sw_dtu_alive[id].timercv = timestamp;
                        uprintf("poll %d: %d\r\n", id, sw_dtu_alive[id].timercv);
                    }
                }
            }
            else if(DTU_MODBUS_DATA_MSG == uart_data.id && NULL != uart_data.UArgs)
            {
//                uprintf("%s[%d] modbus res", __FUNCTION__, __LINE__);
//                
//                UINT8* p = uart_data.UArgs;
//                for (i = 0; i < uart_data.len; i++)
//                {
//                    uprintf("0x%02X" , p[i]);
//                }
                
                //如果是modbus数据，判断目前是modbus rtu上报模式，modbus tcp上报模式，还是json上报格式
                if(DTU_MODBUS_RESPONSE_JSON_TYPE == dtu_file_ctx->modbus.config.res_type)
                {
                    uprintf("modbus json res");
//                    extern void dtu_modbus_json_modbus_res(UINT8* data, UINT32 len);
                    dtu_modbus_json_modbus_res(uart_data.UArgs, uart_data.len);
                }
//                else if(DTU_MODBUS_RESPONSE_TCP_TYPE == dtu_file_ctx->modbus.config.res_type)
//                {
//                    //判断是否是modbus一帧数据
//                    ret = dtu_485_protocol_check(uart_data.UArgs, uart_data.len);
//                    if(DTU_MODBUS_PROTOCOL_SUCCESS == ret)
//                    {
//                        uprintf("modbus tcp res");
//                    }
//                }
                else
                {
                    uprintf("modbus rtu res");
                    //modbus rtu帧，直接发
#ifdef DTU_BASED_ON_TCP
                    dtu_socket_write(uart_data.UArgs, uart_data.len);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-31 12:01:18 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
                    dtu_mqtt_send(uart_data.UArgs, uart_data.len);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-31 12:01:28 by: zhaoning */
                }

                //释放数据内存
                free(uart_data.UArgs);
                //置空
                uart_data.UArgs = NULL;

            }
        }
    }
}

/**
  * Function    : dtu_alive_task
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_alive_task(void *ptr)
{
    OSA_STATUS status = 0;
    int i = 0;
    UINT32 timestamp = 0;
//    int id = 0;
    
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    while(1)
    {
//        uprintf("%s[%d] timer msg\r\n", __FUNCTION__, __LINE__);
        //for循环查询列表中哪些指令激活，按照激活指令参数配置 指令
        for(i = 0; i < SW_DTU_NUM_MAX; i++)
        {
            //当前列表第i指令被激活
            if(DTU_MODBUS_ACTIVE == dtu_file_ctx->modbus.cmd[i].active)
            {
                //获取当前时间
                timestamp = utils_utc8_2_timestamp();
                if(timestamp -  sw_dtu_alive[i].timercv > SW_DTU_TIMEOUT)
                {
                    sw_dtu_alive[i].alive = 0;
                }
                uprintf("%s[%d] slave addr: %d id: %d cmd: 0x%02X\n", __FUNCTION__, __LINE__, dtu_file_ctx->modbus.cmd[i].slave_addr, st_dtu_md.id + 1, dtu_file_ctx->modbus.cmd[i].fn);
            }
        }
        sleep(1);
    }
}

/**
  * Function    : dtu_485_task_init
  * Description : 初始化modbus任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_485_task_init(void)
{
    OSA_STATUS status = 0;

    /*creat message*/
    status = OSAMsgQCreate(&dtu_485_msgq, "dtu_485_msgq", DTU_485_TASK_MSGQ_MSG_SIZE, DTU_485_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSATaskCreate(&dtu_485_msgq_task_ref, dtu_485_task_stack, DTU_MODBUS_TASK_STACK_SIZE, 161, "modbus_task", dtu_485_task, NULL);
    ASSERT(status == OS_SUCCESS);
    status = OSATaskCreate(&dtu_alive_msgq_task_ref, dtu_alive_task_stack, DTU_MODBUS_TASK_STACK_SIZE, 161, "alive_task", dtu_alive_task, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_485.c 2023-9-20 14:35:46 by: zhaoning 

