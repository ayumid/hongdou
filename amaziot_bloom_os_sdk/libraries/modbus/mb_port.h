#ifndef __MB_PORT_H
#define __MB_PORT_H
typedef unsigned char				uint8_t;
typedef unsigned short				uint16_t;

/**
  ******************************************************************************
  * @file    mb_port.h
  * @author  Derrick Wang
  * @brief   modebus移植接口头文件
  ******************************************************************************
  * @note
  * 该文件为modbus移植接口的实现，根据不同的MCU平台进行移植
  ******************************************************************************
  */


typedef enum
{
	MB_PARITY_NONE=0X00,	//无奇偶校验，两个停止位
	MB_PARITY_ODD, 			//奇校验
	MB_PARITY_EVEN			//偶校验
}mbParity;

/**
 * 	@brief  MODBUS串口初始化接口
 * 	@param	NONE
 * 	@return	NONE
*/
void mb_port_uartInit(void);
/**
 * 	@brief  MODBUS串口发送
 * 	@param	NONE
 * 	@return	NONE
*/
void mbh_uartTxIsr(void);

#endif

