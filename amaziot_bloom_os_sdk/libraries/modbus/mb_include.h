#ifndef __MB_INCLUDE_H
#define __MB_INCLUDE_H

#include "mb_crc.h"
#include "mb_host.h"
#include "mb_port.h"
#include "mb_hook.h"
#include "utils_common.h"

typedef struct
{
	uint8_t state;						//modbus host状态
	uint8_t errTimes;  					//失败次数计数
	uint8_t txLen;     					//需要发送的帧长度
	uint8_t txCounter;					//已发送bytes计数
	uint8_t txBuf[MBH_RTU_MAX_SIZE];	//发送缓冲区
	uint8_t rxCounter;					//接收计数
	uint8_t rxBuf[MBH_RTU_MAX_SIZE];	//接收缓冲区
	uint8_t rxTimeOut;					//接收时的超时计数
	
}MBHost;

#endif

