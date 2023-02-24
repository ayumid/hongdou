/**
  ******************************************************************************
  * @file    mb_hook.c
  * @author  Derrick Wang
  * @brief   modebus回调函数接口
  ******************************************************************************
  * @note	
  * 针对modbus的回调处理，请再该文件中添加		
  ******************************************************************************
  */

#include "mb_include.h"
extern MBHost mbHost;
/*
//接收正确,进行解析处理
void mbh_exec(uint8_t *pframe,uint8_t len)
{
	uint8_t datalen=len-2;
	switch(pframe[1])//cmd
	{
		case 1:
			mbh_hook_rec01(pframe[0],(pframe+2),datalen);
			break;
		case 2:
			mbh_hook_rec02(pframe[0],(pframe+2),datalen);
			break;
		case 3:
			mbh_hook_rec03(pframe[0],(pframe+2),datalen);
			break;
		case 4:
			mbh_hook_rec04(pframe[0],(pframe+2),datalen);
			break;
		case 5:
			mbh_hook_rec05(pframe[0],(pframe+2),datalen);
			break;
		case 6:
			mbh_hook_rec06(pframe[0],(pframe+2),datalen);
			break;
		case 15:
			mbh_hook_rec15(pframe[0],(pframe+2),datalen);
			break;
		case 16:
			mbh_hook_rec16(pframe[0],(pframe+2),datalen);
			break;
	}
}

void mbh_hook_rec01(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec01");
}
void mbh_hook_rec02(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec02");
}
void mbh_hook_rec03(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec03");
}
void mbh_hook_rec04(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec04");
}
void mbh_hook_rec05(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec05");
}
void mbh_hook_rec06(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec06");
}
void mbh_hook_rec15(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec15");
}
void mbh_hook_rec16(uint8_t add,uint8_t *data,uint8_t datalen)
{
	printf("asr_test  mbh_hook_rec16");
}
*/

void mbh_hook_timesErr(uint8_t add,uint8_t cmd)
{
	printf("asr_test  mbh_hook_timesErr");
	mbHost.state=MBH_STATE_IDLE;
}

