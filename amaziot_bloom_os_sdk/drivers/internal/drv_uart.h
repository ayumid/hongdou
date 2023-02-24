//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_uart.h
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_UART_H_
#define _DRV_UART_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "sdk_api.h"
#include "UART.h"

// Public defines / typedef -----------------------------------------------------

#define                 BLOOM_OS_DRV_UART_1                 1
#define                 BLOOM_OS_DRV_UART_3                 3
#define                 BLOOM_OS_DRV_UART_4                 4

// Public functions prototypes --------------------------------------------------

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
void drv_uart_init(UINT8 uart_num, UART_BaudRates baudRate, uartRecvCallback recv_cb);

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
void drv_uart_send_data(UINT8 uart_num, unsigned char *bufPtr, unsigned long length);

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
UART_BaudRates drv_uart_get_baud(UINT8 uart_num);


#endif /* ifndef _DRV_UART_H_.2021-11-23 18:14:16 by: win */

