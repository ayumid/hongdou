//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_lora.c
// Auther      : zhaoning
// Version     :
// Date : 2024-1-11
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-1-11
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_lora.h"

#include "lib_llcc68_hal.h"

// Private defines / typedefs ---------------------------------------------------

#define LORA_TASK_STACK_SIZE     1024 * 8

typedef struct _LORA_DATA_MSG_S
{
    UINT32 len;//消息长度
    UINT8* data;
}LORA_DATA_MSG_T, *P_LORA_DATA_MSG_T;

typedef struct _LORA_INT_MSG_S
{
    UINT8 id;//消息长度
}LORA_INT_MSG_T, *P_LORA_INT_MSG_T;

#define DTU_LORA_TASK_MSGQ_MSG_SIZE              (sizeof(LORA_DATA_MSG_T))
#define DTU_LORA_TASK_MSGQ_QUEUE_SIZE            (50)

#define DTU_LORA_INT_TASK_MSGQ_MSG_SIZE              (sizeof(LORA_INT_MSG_T))
#define DTU_LORA_INT_TASK_MSGQ_QUEUE_SIZE            (50)

#define DEV_LORA_DATA_LEN 200
#define DEV_MASTER_CHIP_LEN 200

#define pack_gap 10      //ms
#define tx_power 22      // -9 ~ 22
#define total_pack 500
#define threshold_pack 450

typedef enum
{
    DIO1_STATE_IDLE = 0,
    DIO1_STATE_SEND,
}DEV_SLAVE_DIO1_STATE;

typedef struct _DEV_SLAVE_S
{
    DEV_SLAVE_DIO1_STATE state;
    
}DEV_LORA_T;

// Private variables ------------------------------------------------------------

OS_HISR         dio1_gpio_detect_hisr;
OSATimerRef dio1_gpio_detect_timer_ref = NULL;
//OSFlagRef      dio1_gpio_detect_flg_ref = NULL;
OSATaskRef     dio1_gpio_detect_task_ref = NULL;
char         dio1_gpio_detect_stack_ptr[LORA_TASK_STACK_SIZE] = {0};

DEV_LORA_T dev_lora_state;

// Public variables -------------------------------------------------------------

OSMsgQRef lora_data_msgq;
OSMsgQRef lora_int_msgq;

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dev_lora_module_init
  * Description : 初始化lora模组
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dev_lora_module_init(void)
{
    SX1262_ModulationParams.PacketType = PACKET_TYPE_LORA;
    SX1262_ModulationParams.Params.LoRa.SpreadingFactor = LORA_SF9;//
    SX1262_ModulationParams.Params.LoRa.Bandwidth = LORA_BW_125;//
    SX1262_ModulationParams.Params.LoRa.CodingRate = LORA_CR_4_5;
    SX1262_ModulationParams.Params.LoRa.LowDatarateOptimize = 0x0;
    
    SX1262_PacketParams.PacketType = PACKET_TYPE_LORA;
    SX1262_PacketParams.Params.LoRa.PreambleLength = 8;
    SX1262_PacketParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;
    SX1262_PacketParams.Params.LoRa.PayloadLength = DEV_LORA_DATA_LEN;
    SX1262_PacketParams.Params.LoRa.CrcMode = LORA_CRC_ON;
    SX1262_PacketParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;
    
    SX1262_Init();
    SX1262_SetStandby(STDBY_RC);
    SX1262_SetPacketType(SX1262_ModulationParams.PacketType);
    SX1262_SetRegulatorMode(USE_DCDC);
    SX1262_SetDio2AsRfSwitchCtrl(true);
    SX1262_SetRfFrequency(433000000);
    SX1262_SetBufferBaseAddresses(0x0,0x0);
    SX1262_SetModulationParams(&SX1262_ModulationParams);
    SX1262_SetPacketParams(&SX1262_PacketParams);
    SX1262_SetTxParams(tx_power,RADIO_RAMP_80_US);
    SX1262_SetDioIrqParams(IRQ_RADIO_ALL,IRQ_TX_DONE|IRQ_RX_DONE,IRQ_RADIO_NONE,IRQ_RADIO_NONE);
    
    SX1262_SetStandby(STDBY_XOSC);
    SX1262_ClearIrqStatus(IRQ_RADIO_ALL);

    SX1262_SetRxBoosted(0x0);
    SX1262_SetRx(0x0);

}

void dev_lora_module_packet_init(UINT8 len)
{
    SX1262_PacketParams.PacketType = PACKET_TYPE_LORA;
    SX1262_PacketParams.Params.LoRa.PreambleLength = 8;
    SX1262_PacketParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;
    SX1262_PacketParams.Params.LoRa.PayloadLength = len;
    SX1262_PacketParams.Params.LoRa.CrcMode = LORA_CRC_ON;
    SX1262_PacketParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;

    SX1262_SetPacketParams(&SX1262_PacketParams);
}
/**
  * Function    : dev_lora_data_rcv
  * Description : 读lora模块的数据，检测到dio1高电平，之后调用本函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dev_lora_data_rcv(UINT8* data)
{
    UINT8 lora_rx_len = 0;

//    SX1262_SetStandby(STDBY_XOSC);
//    SX1262_ClearIrqStatus(IRQ_RADIO_ALL);
    memset(data, 0, DEV_MASTER_CHIP_LEN);
    SX1262_GetPayload(data, &lora_rx_len, DEV_MASTER_CHIP_LEN);
//    dev_slave_hex_printf(arr, DEV_SLAVE_PLAIN_LEN);
//    uprintf("len: %d, %s", lora_rx_len, data);
//    SX1262_SetRxBoosted(0x0);
//    SX1262_SetRx(0x0);

    return lora_rx_len;
}

/**
  * Function    : dev_lora_send
  * Description : 发送lora数据，将状态改为发送，这样在任务中收到dio1中断，会按照发送数据处理后续
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dev_lora_send(UINT8* data, UINT8 len)
{
    dev_lora_state.state = DIO1_STATE_SEND;
    SX1262_SendPayload(data, len, 2000);
    
}

void dio1_gpio_irq_handler (void)
{
    OS_Activate_HISR(&dio1_gpio_detect_hisr);
}

void dio1_gpio_detect_timer_handler(UINT32 arg)
{
    OS_STATUS status;

//    status = OSAFlagSet(dio1_gpio_detect_flg_ref, 0x01, OSA_FLAG_OR);
//    ASSERT(os_status==OS_SUCCESS);    
    LORA_INT_MSG_T sdata = {0};

    status = OSAMsgQSend(lora_int_msgq, DTU_LORA_TASK_MSGQ_MSG_SIZE, (void*)&sdata, OSA_NO_SUSPEND);
}

void dio1_gpio_detect_handler(void)
{
    OS_STATUS status;

    OSATimerStop(dio1_gpio_detect_timer_ref);
    status = OSATimerStart(dio1_gpio_detect_timer_ref,
                             2,
                             0,
                             dio1_gpio_detect_timer_handler,
                             0);
}

void dio1_gpio_detect_task_entry(void *param)
{
//    GPIO_ReturnCode ret = 0;
    UINT32 value = 0;
    OSA_STATUS status = OS_SUCCESS;
    UINT32 flag_value = 0;
    LORA_DATA_MSG_T send_data = {0};
    LORA_INT_MSG_T rdata = {0};
    UINT8* ldata = NULL;
    unsigned char lora_data[DEV_MASTER_CHIP_LEN] = {0};
    uint8_t lora_rx_len = 0;

    while(1)
    {
//        status = OSAFlagWait(dio1_gpio_detect_flg_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        status = OSAMsgQRecv(lora_int_msgq, (void *)&rdata, sizeof(LORA_INT_MSG_T), OSA_SUSPEND);
//        uprintf("dio1_gpio_detect_task_entry\r\n");
        //接收数据
        lora_rx_len = dev_lora_data_rcv(lora_data);
        //接收数据是0，表示当次是发送数据产生的中断
        if(0 != lora_rx_len)
        {
            uprintf("length: %d, %s", lora_rx_len, lora_data);
            ldata = malloc(lora_rx_len);
            if(NULL != ldata)
            {
                memcpy(ldata, lora_data, lora_rx_len);
                send_data.len = lora_rx_len;
                send_data.data = ldata;
                //发送消息给json主任务
                status = OSAMsgQSend(lora_data_msgq, DTU_LORA_TASK_MSGQ_MSG_SIZE, (void*)&send_data, OSA_NO_SUSPEND);
                if (status != OS_SUCCESS)
                {
                    uprintf("%s, err", __FUNCTION__);
                }
            }
        }
        //设置为接收状态
        SX1262_SetStandby(STDBY_XOSC);
        SX1262_ClearIrqStatus(IRQ_RADIO_ALL);
        SX1262_SetRxBoosted(0x0);
        SX1262_SetRx(0x0);
    }
}

/**
  * Function    : dev_lora_dio1_task_init
  * Description : 初始化lora数据任务，初始化dio1中断相关资源
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dev_lora_dio1_task_init(void)
{
    OSA_STATUS status = 0;
    GPIOConfiguration config = {0};
    
    OSATimerCreate(&dio1_gpio_detect_timer_ref);
//    OSAFlagCreate( &dio1_gpio_detect_flg_ref);  
    Os_Create_HISR(&dio1_gpio_detect_hisr, "dio1_gpio_detect_hisr", dio1_gpio_detect_handler, 2);

    /*creat message*/
    status = OSAMsgQCreate(&lora_data_msgq, "lora_data_msgq", DTU_LORA_TASK_MSGQ_MSG_SIZE, DTU_LORA_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);
    /*creat message*/
    status = OSAMsgQCreate(&lora_int_msgq, "lora_int_msgq", DTU_LORA_INT_TASK_MSGQ_MSG_SIZE, DTU_LORA_INT_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);
    
    status = OSATaskCreate(&dio1_gpio_detect_task_ref, dio1_gpio_detect_stack_ptr, LORA_TASK_STACK_SIZE, 100, "Detect_task", dio1_gpio_detect_task_entry, NULL);
    ASSERT(status == OS_SUCCESS);
//    uprintf("SX1262Dio1In\r\n");

    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_RISE_EDGE;
    config.pinPull = GPIO_PULL_DISABLE;
    config.isr = dio1_gpio_irq_handler;
    GpioInitConfiguration(DIO1_PIN, config);
}

// End of file : am_lora.c 2024-1-11 14:28:40 by: zhaoning 

