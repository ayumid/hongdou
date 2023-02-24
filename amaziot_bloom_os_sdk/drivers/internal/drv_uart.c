//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_uart.c
// Auther      : win
// Version     :
// Date : 2021-11-23
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-23
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_uart.h"

// Private defines / typedefs ---------------------------------------------------

enum
{
    bautrate = 0,
};

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_uart_init
  * Description : 串口初始化
  * Input       : uart_num:      串口号，只能是1,3,4
  *               recv_cb:       回调函数
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_uart_init(UINT8 uart_num, UART_BaudRates baudRate, uartRecvCallback recv_cb)
{
    if(BLOOM_OS_DRV_UART_1 == uart_num)
    {
        UART_OPEN(recv_cb);
        UART_SET_BAUD(baudRate);
    }
    else if(BLOOM_OS_DRV_UART_3 == uart_num)
    {
        UART3_OPEN(recv_cb);
        UART3_SET_BAUD(baudRate);
    }
    else if(BLOOM_OS_DRV_UART_4 == uart_num)
    {
        UART4_OPEN(recv_cb);
        UART4_SET_BAUD(baudRate);
    }
}

/**
  * Function    : drv_uart_send_data
  * Description : 串口发送数据
  * Input       : uart_num:      串口号，只能是1,3,4
  *               bufPtr:        发送数据首地址
  *               length:        发送数据长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_uart_send_data(UINT8 uart_num, unsigned char *bufPtr, unsigned long length)
{
    if(BLOOM_OS_DRV_UART_1 == uart_num)
    {
        UART_SEND_DATA(bufPtr,length);
    }
    else if(BLOOM_OS_DRV_UART_3 == uart_num)
    {
        UART3_SEND_DATA(bufPtr,length);
    }
    else if(BLOOM_OS_DRV_UART_4 == uart_num)
    {
        UART4_SEND_DATA(bufPtr,length);
    }
}

/**
  * Function    : drv_uart_get_baud
  * Description : uart_num:      串口号，只能是1,3,4
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UART_BaudRates drv_uart_get_baud(UINT8 uart_num)
{
    UART_BaudRates baudrate = (UART_BaudRates)bautrate;
    
    if(BLOOM_OS_DRV_UART_1 == uart_num)
    {
        baudrate = UART_GET_BAUD();
    }
    else if(BLOOM_OS_DRV_UART_3 == uart_num)
    {
        baudrate = UART3_GET_BAUD();
    }
    else if(BLOOM_OS_DRV_UART_4 == uart_num)
    {
        baudrate = UART4_GET_BAUD();
    }

    return baudrate;
}

// End of file : drv_uart.h 2021-11-23 18:14:09 by: win 

