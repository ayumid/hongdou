//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_trans.c
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

#include "am_trans.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

DTU_UART_PARAM_T st_at_uart;

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_uart_ctx
  * Description : 获取串口结构体
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_UART_PARAM_T* dtu_get_uart_ctx(void)
{
    return &st_at_uart;
}

/**
  * Function    : dtu_uart_data_recv_cbk
  * Description : 串口数据接收回调函数，通过UART_OPEN注册给底层
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_uart_data_recv_cbk(UINT8 *data, UINT32 len)
{
    DTU_MSG_UART_DATA_PARAM_T uart_temp = {0};
    OSA_STATUS status = 0;
    char* data_rcv = NULL;
    
    data_rcv = malloc(len + 1);
    memset(data_rcv, 0, len + 1);
    memcpy(data_rcv, (char *)data, len);
    
    uart_temp.UArgs = (UINT8 *)data_rcv;
    uart_temp.len = len;
    
    status = OSAMsgQSend(st_at_uart.dtu_msgq_uart, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&uart_temp, OSA_NO_SUSPEND);
    ASSERT(status == OS_SUCCESS);
        
}

/**
  * Function    : dtu_uart_data_recv_thread
  * Description : 创建串口接收数据线程，以及相关资源初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_uart_data_recv_thread(void)
{
    DTU_MSG_UART_DATA_PARAM_T uart_temp = {0};
    OSA_STATUS status = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    
    //默认是透传模式
    dtu_uart_ctx->uart_mode = DTU_DATA_MODE;
    
    while (1)
    {
        //清空接收数据缓冲区
        memset(&uart_temp, 0, sizeof(DTU_MSG_UART_DATA_PARAM_T));
        //等待来自底层的串口数据，MV9一次最多发送1024字节
        status = OSAMsgQRecv(st_at_uart.dtu_msgq_uart, (UINT8 *)&uart_temp, sizeof(DTU_MSG_UART_DATA_PARAM_T), OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            if (NULL != uart_temp.UArgs)
            {
//                uprintf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                uprintf("%s[%d]: uart_temp len:%d\n", __FUNCTION__, __LINE__, uart_temp.len);
                //处理串口数据
                dtu_handle_serial_data(&uart_temp);

                //释放数据内存
                free(uart_temp.UArgs);
                //置空
                uart_temp.UArgs = NULL;
            }
        }
    }
}

/**
  * Function    : dtu_trans_uart_init
  * Description : 透传at口初始化。根据makefile或者am.h是否定义宏，来编译不同的代码
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_trans_uart_init(void)
{
#ifdef DTU_UART_REGULAR_BAUD
    s_uart_cfg uartCfg = {0};
#endif /* ifdef DTU_UART_REGULAR_BAUD.2023-11-14 13:29:06 by: zhaoning */
    //打开串口，并设置回调函数
    UART_OPEN(dtu_uart_data_recv_cbk); 
#ifdef DTU_UART_REGULAR_BAUD
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
#endif /* ifdef DTU_UART_REGULAR_BAUD.2023-11-14 13:29:28 by: zhaoning */

}

/**
  * Function    : dtu_trans_task_init
  * Description : 初始化透传任务，以及相关资源
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_trans_task_init(void)
{
    OSA_STATUS status = 0;

    //初始化数据发送消息队列，包括串口透传数据和心跳注册包数据
    status = OSAMsgQCreate(&st_at_uart.dtu_msgq_uart, "dtu_msgq_uart", sizeof(DTU_MSG_UART_DATA_PARAM_T), 50, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);

    //串口数据接收线程
    sys_thread_new("dtu_uart_data_recv_thread", (lwip_thread_fn)dtu_uart_data_recv_thread, NULL, DTU_DEFAULT_THREAD_STACKSIZE * 4, 161);
    
}

// End of file : am_trans.c 2023-11-17 9:25:36 by: zhaoning 

