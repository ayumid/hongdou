//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : mb_host.h
// Auther      : win
// Version     :
// Date : 2021-12-22
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-22
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _MB_HOST_H_
#define _MB_HOST_H_

// Includes ---------------------------------------------------------------------

// Public defines / typedef -----------------------------------------------------
#define int8_t      char

#define MBH_RTU_MIN_SIZE	4
#define MBH_RTU_MAX_SIZE	255	//最大不超过255
#define MBH_ERR_MAX_TIMES	3
#define MBH_REC_TIMEOUT		100  //单位3.5T

typedef enum
{
	MBH_STATE_IDLE=0X00,
	MBH_STATE_TX,
	MBH_STATE_TX_END,
	MBH_STATE_RX,
	MBH_STATE_RX_CHECK,
	MBH_STATE_EXEC,
	MBH_STATE_REC_ERR,		//接收错误状态
	MBH_STATE_TIMES_ERR,	//传输
	
}mb_host_state;

typedef void (*data_handler_t)(uint8_t *data,uint8_t len);

// Public functions prototypes --------------------------------------------------
/**
  * Function    : lib_mbh_init
  * Description : modbus主机初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mbh_init(void);
/**
  * Function    : lib_mbh_getState
  * Description : 获取modbus主机状态
  * Input       : 
  *               
  * Output      : 
  * Return      : modbus主机状态
  * Auther      : win
  * Others      : 
  **/
uint8_t lib_mbh_getState(void);

/**
  * Function    : lib_mbh_send
  * Description : modbus主机发送
  * Input       : add      从机地址
  *               cmd 	   功能码
  *               data     数据
  *               data_len 数据长度
  *              
  * Output      : 
  * Return      : -1:发送失败	0:发送成功
  * Auther      : win
  * Others      : 该函数为非阻塞式，调用后立即返回
  **/
int8_t lib_mbh_send(uint8_t add,uint8_t cmd,uint8_t *data,uint8_t data_len);
/**
  * Function    : lib_mbh_poll
  * Description : modbus状态轮训
  * Input       : mbh_exec 主机接收数据处理函数
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 该函数必须不断轮询，用来底层核心状态进行切换
  *			      可在操作系统任务中运行，但应该尽可能短的延时间隔
  **/
void lib_mbh_poll(data_handler_t mbh_exec);

#endif /* ifndef _MB_HOST_H_.2021-12-22 11:21:53 by: win */

