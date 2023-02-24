//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : mb_host.c
// Auther      : win
// Version     :
// Date : 2021-12-22
// Description :modbus主机实现代码
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

// Includes ---------------------------------------------------------------------

#include "mb_include.h"
#include "string.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------
MBHost mbHost;
// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

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
void lib_mbh_init(void)
{
	mb_port_uartInit();
}
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
uint8_t lib_mbh_getState(void)
{
	return mbHost.state;
}

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

int8_t lib_mbh_send(uint8_t add,uint8_t cmd,uint8_t *data,uint8_t data_len)
{
	uint16_t crc;
	if(mbHost.state!=MBH_STATE_IDLE)return -1; //busy state

	mbHost.txCounter=0;
	mbHost.rxCounter=0;	
	mbHost.txBuf[0]=add;
	mbHost.txBuf[1]=cmd;
	memcpy((mbHost.txBuf+2),data,data_len);
	mbHost.txLen=data_len+2; //data(n)+add(1)+cmd(1)
	crc=mb_crc16(mbHost.txBuf,mbHost.txLen);
	mbHost.txBuf[mbHost.txLen++]=(uint8_t)(crc&0xff);
	mbHost.txBuf[mbHost.txLen++]=(uint8_t)(crc>>8);
	
	mbHost.state=MBH_STATE_TX;
	mbh_uartTxIsr();
	return 0;
}
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
void lib_mbh_poll(data_handler_t mbh_exec)
{
	switch(mbHost.state)
	{
		/*接收完一帧数据,开始进行校验*/
		case MBH_STATE_RX_CHECK:  //接收完成，对一帧数据进行检查
			if((mbHost.rxCounter>=MBH_RTU_MIN_SIZE)&&(mb_crc16(mbHost.rxBuf,mbHost.rxCounter)==0)) 	//接收的一帧数据正确
			{
				if((mbHost.txBuf[0]==mbHost.rxBuf[0])&&(mbHost.txBuf[1]==mbHost.rxBuf[1]))			//发送帧数据和接收到的帧数据地址和功能码一样
				{
					mbHost.state=MBH_STATE_EXEC;
				}
				else mbHost.state=MBH_STATE_REC_ERR;
				
			}
			else mbHost.state=MBH_STATE_REC_ERR;
			break;
		/*接收一帧数据出错*/	
		case MBH_STATE_REC_ERR:
			mbHost.errTimes++;			
			if(mbHost.errTimes>=MBH_ERR_MAX_TIMES)  
			{
				mbHost.state=MBH_STATE_TIMES_ERR;
			}
			else  //重新再启动一次传输
			{
				mbHost.txCounter=0;
				mbHost.rxCounter=0;
				mbHost.state=MBH_STATE_TX;
				mbh_uartTxIsr();
			}
			break;
		/*超过最大错误传输次数*/
		case MBH_STATE_TIMES_ERR:
			mbh_hook_timesErr(mbHost.txBuf[0],mbHost.txBuf[1]);
			mbHost.txCounter=0;
			mbHost.rxCounter=0;
			break;
		/*确定接收正确执行回调*/
		case MBH_STATE_EXEC:      //主机发送接收完成，执行回调
			mbh_exec(mbHost.rxBuf,mbHost.rxCounter);
			mbHost.state=MBH_STATE_IDLE;
			break;
		
	}
}

// End of file : mb_host.h 2021-12-22 11:10:34 by: win 

