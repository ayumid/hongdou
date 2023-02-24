//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : wmri.c
// Auther      : win
// Version     :
// Date : 2021-12-31
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-31
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "sdk_api.h"
#include "gpio.h"
#include "cgpio.h"
#include "cgpio_HW.h"

#include "wmri.h"

// Private defines / typedefs ---------------------------------------------------
#define SPI_PRINT(fmt, args...) do { RTI_LOG("[sdk]"fmt, ##args); } while(0)
// Private variables ------------------------------------------------------------
int spi_apply_flag;
int wifi_type;
int qw_at_ok=0;
int qwifi = 0;
char qwssid[128];
UINT16 qwssid_len;
int qwbcast;
int qencry;
int qauthhex;
int qauthindex;
char qauthkey[128];
UINT16 qauthkey_len;

OSMsgQRef  MsgSpiQ;

static UINT8 gsSeqNum = 0;
static UINT8 revBuf[1500];

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void SPINSS(UINT32 status);
static int StringToIpaddr(const char *buf, UINT8 *addr);
static UINT8 RICheckSum(UINT8 *data, UINT16 len);
static UINT32 RICreateCmdHeader(WM_SPI_HDR *header, UINT8 cmd, UINT16 len);
static UINT32 RICreateDataHeader(WM_RI_HDR *header, UINT32 fd, UINT16 len, UINT16 type);

static UINT32 AppRICheckSPIHeader(WM_RI_HDR *ri);
static void AppSPIProc(UINT8* buff);
static UINT32 AppRICmdProc(WM_RI_CMD_HDR *cmd);

static UINT8 SPITxCmd(UINT8 *TXBuf, UINT16 CmdLen);
static UINT8 SPITxData(UINT8 *TXBuf, UINT16 DataLen);
/***********************************************************************
* Description : SPI片选
* Arguments   : status:状态值；
* Returns     : 
* Author      : dmh
***********************************************************************/
static void SPINSS(UINT32 status)
{
	if(status)
	{
		GpioSetLevel(H_SPI_CS,1);
	}
	else
	{
		GpioSetLevel(H_SPI_CS,0);
	}
}
/***********************************************************************
* Description : StringToIpaddr
* Arguments   : 
* Returns     : 
* Author      : houxf 
***********************************************************************/
static int StringToIpaddr(const char *buf, UINT8 *addr)
{
	int count = 0, rc = 0;
	int in[4];
	char c;

	rc = sscanf(buf, "%u.%u.%u.%u%c", &in[0], &in[1], &in[2], &in[3], &c);
	if (rc != 4 && (rc != 5 || c != '\n'))
	{
		return -1;
	}
	for (count = 0; count < 4; count++) 
	{
		if (in[count] > 255)
		{
			return -1;
		}
		addr[count] = in[count];
	}
	return 0;
}
/***********************************************************************
* Description : 计算校验和
* Arguments   : 
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT8 RICheckSum(UINT8 *data, UINT16 len)
{
	UINT16 i;
	UINT16 sum = 0;
	
	for(i = 0; i < len; i++)
	{
		sum += *data++;
	}
	return sum;
}
/***********************************************************************
* Description : Pack Cmd Header
* Arguments   : 
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT32 RICreateCmdHeader(WM_SPI_HDR *header, UINT8 cmd, UINT16 len)
{	
	if(NULL == header)
	{
		return 1;
	}
	SPI_PRINT("kevin debug CMD cmd = 0x%x, len = %d\r\n", cmd, len);
	header->hdr.sync		= RI_CMD_SYNC;
	header->hdr.type		= RI_CMD;
	header->hdr.length		= Swap16(len + sizeof(WM_RI_CMD_HDR));
	header->hdr.seq_num		= gsSeqNum++;
	header->hdr.flag		= 0;
	header->hdr.dest_addr	= 0;
	header->hdr.chk			= RICheckSum(&header->hdr.type, 6);
	header->cmd.msg_type	= RI_CMD_MT_REQ;
	header->cmd.code		= cmd;
	header->cmd.err			= 0;
	if(len)
	{
		header->cmd.ext		= 1;
	}
	else
	{
		header->cmd.ext		= 0;
	}
	SPI_PRINT("sync= 0x%x,type= 0x%x,length=0x%x,seq_num=0x%x,flag= 0x%x,dest_addr= 0x%x,chk	=0x%x,msg_type=0x%x,code= 0x%x,err= 0x%x,ext 0x%x\r\n", header->hdr.sync,	header->hdr.type,	header->hdr.length,	header->hdr.seq_num	,header->hdr.flag,header->hdr.dest_addr,	header->hdr.chk,header->cmd.msg_type,header->cmd.code,header->cmd.err,header->cmd.ext);
	return 0;
}
/***********************************************************************
* Description : 
* Arguments   : 
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT32 RICreateDataHeader(WM_RI_HDR *header, UINT32 fd, UINT16 len, UINT16 type)
{
	if(NULL == header)
	{
		return 1;
	}
//	SPI_PRINT("kevin debug CMD cmd = 0x%x, len = %d\r\n", cmd, len);
	header->sync		= RI_CMD_SYNC;
	header->type		= RI_DATA;
	header->length		= Swap16(len);
	header->seq_num		= gsSeqNum++;
	header->flag		= 0;
	header->dest_addr	= fd & 0x3F;
	if(type)
	{
		header->dest_addr |= 0x40; 
	}
	header->chk = RICheckSum(&header->type, 6);
	return 0;
}


/***********************************************************************
* Description : 检查RI指令同步码 
* Arguments   : 
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT32 AppRICheckSPIHeader(WM_RI_HDR *ri)
{
	if((NULL == ri) || (0xAA != ri->sync))
	{
		SPI_PRINT("kevin debug ri->sync = %x\r\n", ri->sync);
		return 1;
	}
	return 0;
}
/***********************************************************************
* Description :SPI响应处理
* Arguments   : 
* Returns     : 
* Author      : houxf
***********************************************************************/
static void AppSPIProc(UINT8* buff)
{
	WM_SPI_HDR *ri = (WM_SPI_HDR *)buff;
	UINT8 *pdata;
	UINT16 data_len;

	MsgSpiParam MsgSpi_Data={0};
	char *databuf;

	if(AppRICheckSPIHeader(&ri->hdr))
	{
		return;
	}
	switch(ri->hdr.type)
	{
		case RI_CMD:
			AppRICmdProc(&ri->cmd);
			break;
		case RI_DATA:
			data_len =ri->hdr.length;
			data_len = htons(data_len);
			pdata = (UINT8*)ri + sizeof(WM_RI_HDR);
			SPI_PRINT("kevin debug RI_DATA = %d\r\n",data_len);
		
			if(qwifi){

					databuf = (char *)malloc(data_len+10);
				    memset(databuf, 0x0, data_len+10);
				    memcpy(databuf, (char *)pdata, data_len);
					memset(&MsgSpi_Data, 0, sizeof(MsgSpiParam));
					MsgSpi_Data.UArgs = (UINT8 *)(databuf);
		    		MsgSpi_Data.len = data_len;
					OSAMsgQSend(MsgSpiQ, sizeof(MsgSpiParam), (UINT8*)&MsgSpi_Data, OSA_NO_SUSPEND);
			}	
			break;
		default:
			break;
	}
	return;
}
/***********************************************************************
* Description : RI指令响应处理函数
* Arguments   : 
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT32 AppRICmdProc(WM_RI_CMD_HDR *cmd)
{
	UINT8 *pdata;
	UINT8 ssid_len;
		
	if(NULL == cmd)
	{
		return 1;
	}
	SPI_PRINT("kevin debug RICmdProc code = %x, err = %x, ext = %x\r\n", cmd->code, cmd->err, cmd->ext);
	switch(cmd->code)
	{
		case RI_EVENT_INIT_END:
			break;
		case RI_EVENT_LINKUP:
			SPI_PRINT("### kevin debug RI_EVENT_LINKUP\r\n");
			break;
		case RI_EVENT_LINKDOWN:
			SPI_PRINT("### kevin debug RI_EVENT_LINKDOWN\r\n");
			break;
		case RI_CMD_SKCT:
			//pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);
			//gsSTSysCtrol.FdTcp = *pdata;
			//SPI_PRINT("### kevin debug AppCreatSocketProc = %d\r\n", gsSTSysCtrol.FdTcp);
			break;
		case RI_CMD_LINK_STATUS:
			//pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);
			//status = *pdata++;
			//ip = *(UINT32*)pdata;
			//SPI_PRINT("### kevin debug RI_CMD_LINK_STATUS = %d, %d,%d.%d.%d\r\n", 
				//status, (ip & 0xFF),  (ip & 0xFF00) >> 8, (ip & 0xFF0000) >> 16, (ip & 0xFF000000) >> 24);
			break;	
		case RI_CMD_SSID:
			
			if(cmd->ext){
				pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);//+1 跳过ssid_len
				ssid_len=*pdata++;
				snprintf(qwssid,ssid_len+1,pdata);
				//gsSTSysCtrol.FdTcp = *pdata;
				SPI_PRINT("### kevin debug RI_CMD_SSID =%d, %s\r\n",ssid_len, qwssid);
			}
			qw_at_ok = 1;
			break;
		case RI_CMD_BRD_SSID:
			
			if(cmd->ext){
				pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);
				qwbcast=*pdata++;
				SPI_PRINT("### kevin debug RI_CMD_BRD_SSID =%d\r\n",qwbcast);
			}
			qw_at_ok = 1;
			break;
		case RI_CMD_ENCRYPT:
			
			if(cmd->ext){
				pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);
				qencry=*pdata++;
				SPI_PRINT("### kevin debug RI_CMD_ENCRYPT =%d\r\n",qencry);
			}
			qw_at_ok = 1;
			break;
		case RI_CMD_KEY:
			
			if(cmd->ext){
				pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);
				qauthhex=*pdata++;
			    qauthindex = *pdata++;
				qauthkey_len = *pdata++;
				snprintf(qauthkey,qauthkey_len+1,pdata);
				SPI_PRINT("### kevin debug RI_CMD_KEY =%d,%d,%d,%s\r\n",qauthhex,qauthindex,qauthkey_len,qauthkey);
			}
			qw_at_ok = 1;
			break;
		case RI_CMD_RESET:
			qw_at_ok = 1;
			break;
		case RI_CMD_RESET_FLASH:
			qw_at_ok = 1;
			break;
		case RI_CMD_PMTF:
			qw_at_ok = 1;
			break;
		case RI_CMD_NOP:
			spi_apply_flag=1;
			break;
		case RI_CMD_WPRT:
			if(cmd->ext){
				pdata = (UINT8*)cmd + sizeof(WM_RI_CMD_HDR);
				wifi_type=*pdata++;
				SPI_PRINT("### kevin debug RI_CMD_WPRT =%d\r\n",wifi_type);
			}
			break;
	}
	return 0;
}
/***********************************************************************
* Description : SPI发送命令
* Arguments   : UINT8 *TXBuf, UINT16 CmdLen
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT8 SPITxCmd(UINT8 *TXBuf, UINT16 CmdLen)
{
	UINT16 temp = 0;
	UINT16 i;
	UINT32 retry = 0;

	UINT8 temp1[3];
	UINT8 cmd1[3]={SPI_REG_TX_BUFF_AVAIL,0xFF,0xFF};
	
	if(NULL == TXBuf)
	{
		SPI_PRINT("###kevin debug SPITxCmd buff == NULL\r\n");
		return 0;
	}
	while((temp != 0xffff) && (0 == (temp & 0x02)))	
	{
		retry++;
		temp = 0;
		SPINSS(0);
		drv_spi_readwrite_data(temp1,cmd1,3);
		temp |= temp1[1];					//读寄存器，字节序为小端
		temp |= temp1[2] << 8;
		SPINSS(1);
		SPI_PRINT("kevin debug temp1 = 0x%x, 0x%x\r\n", temp1[1], temp1[2]);
		if(retry > SPI_TIMEOUT)
		{
			SPI_PRINT("###kevin debug SPI_CMD_TIMEOUT,temp:0x%x\r\n",temp);
			return 0;
		}
		OSATaskSleep(1);
	}
	if(CmdLen > 0)
	{
		if(CmdLen % 4)
		{
			CmdLen = ((CmdLen + 3) / 4) << 2;
		}
		SPI_PRINT("kevin debug TX_BUFF_AVAIL = 0x%x, cmdlen=%d\r\n", temp, CmdLen);

		UINT8 cmd=SPI_CMD_TX_CMD;
		UINT8 *cmdbuf = (char *)malloc(CmdLen+10);
		memcpy(cmdbuf,&cmd,1);
		memcpy(cmdbuf+1,TXBuf,CmdLen);
		SPINSS(0);
		drv_spi_write_data(cmdbuf, CmdLen+1);
		SPINSS(1);
		if(cmdbuf)
			free(cmdbuf);
	}
	return 1;	
}
/***********************************************************************
* Description : SPI发送数据
* Arguments   : UINT8 *TXBuf, UINT16 CmdLen
* Returns     : 
* Author      : houxf
***********************************************************************/
static UINT8 SPITxData(UINT8 *TXBuf, UINT16 DataLen)
{
	UINT16 temp = 0;
	UINT16 i;
	UINT16 retry=0;

	UINT8 temp1[3];
	UINT8 cmd1[3]={SPI_REG_TX_BUFF_AVAIL,0xFF,0xFF};
	
	if(NULL == TXBuf)
	{
		SPI_PRINT("###kevin debug SPITxData buff == NULL\r\n");
		return 0;
	}
	while((temp != 0xffff) && (0 == (temp & 0x01)))	
	{
		retry++;
		temp = 0;
		SPINSS(0);
		drv_spi_readwrite_data(temp1,cmd1,3);
		temp |= temp1[1];					//读寄存器，字节序为小端
		temp |= temp1[2] << 8;
		SPINSS(1);
		SPI_PRINT("kevin debug temp1 = 0x%x, 0x%x\r\n", temp1[1], temp1[2]);
		if(retry > SPI_TIMEOUT)
		{
			SPI_PRINT("###kevin debug SPI_CMD_TIMEOUT,temp:0x%x\r\n",temp);
			return 0;
		}
		OSATaskSleep(1);
	}
	if(DataLen > 0)
	{
		if(DataLen % 4)
		{
			DataLen = ((DataLen + 3) / 4) << 2;
		}
		SPI_PRINT("kevin debug TX_BUFF_AVAIL = 0x%x, DataLen=%d\r\n", temp, DataLen);

		UINT8 cmd=SPI_CMD_TX_DATA;
		UINT8 *cmdbuf = (char *)malloc(DataLen+10);
		memcpy(cmdbuf,&cmd,1);
		memcpy(cmdbuf+1,TXBuf,DataLen);
		SPINSS(0);
		drv_spi_write_data(cmdbuf, DataLen+1);
		SPINSS(1);
		if(cmdbuf)
			free(cmdbuf);
	}	
	return 1;
}


// Functions --------------------------------------------------------------------
/**
  * Function    : SPIRxData
  * Description : SPI接收数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void SPIRxData(void)
{
	UINT16 temp = 0;
	UINT16 i;
	
	UINT8 tempdata;

	UINT8 cmdInt[3]={SPI_REG_INT_STTS,0xFF,0xFF};
	UINT8 tempInt[3];

	UINT8 cmdRxLen[3]={SPI_REG_RX_DAT_LEN,0xFF,0xFF};
	UINT8 tempRxLen[3];

	SPINSS(0);
	drv_spi_readwrite_data(tempInt,cmdInt,3);//查询SPI_INT_HOST_STTS
	temp |= tempInt[1];					//读寄存器，字节序为小端
	temp |= tempInt[2]<<8;
	SPINSS(1);
	if((temp != 0xffff) && (temp & 0x01))	//数据或命令已经准备好
	{
		temp = 0;
		SPINSS(0);
		drv_spi_readwrite_data(tempRxLen,cmdRxLen,3);//查询RX_DAT_LEN 
		temp |= tempRxLen[1];					//读寄存器，字节序为小端
		temp |= tempRxLen[2]<<8;
		SPINSS(1);
		if(temp > 0)
		{
			if(temp % 4)
			{
				temp = ((temp + 3) / 4) << 2;
			}
			SPI_PRINT("kevin debug rx len = %d\r\n",temp);

			UINT8 cmd = SPI_CMD_RX_DATA;
			//UINT8 revBuf[1500];
			memset(revBuf,0,sizeof(revBuf));
			UINT8 *cmdbuf = (char *)malloc(temp+10);
			memcpy(cmdbuf,&cmd,1);
			memset(cmdbuf+1,0xFF,temp);
			SPINSS(0);
			drv_spi_readwrite_data(revBuf,cmdbuf,temp+1);
			SPINSS(1);
			if(cmdbuf)
				free(cmdbuf);
			AppSPIProc(revBuf+1);
		}
	}
}
/**
  * Function    : RISpiApply
  * Description : 通信测试
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISpiApply(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	spi_apply_flag = 0;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_NOP, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	OSATaskSleep(40);
	
	return spi_apply_flag;
}
/**
  * Function    : RIGetWlanType
  * Description : 获取网络类型
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIGetWlanType(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	wifi_type = 0;	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_WPRT, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	OSATaskSleep(40);
	
	return wifi_type;
}
/**
  * Function    : RISaveParam
  * Description : 保存参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISaveParam(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_PMTF, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RIResetDevice
  * Description : 复位模块
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIResetDevice(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_RESET, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RISetOneShotCfg
  * Description : 设置一键配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISetOneShotCfg(UINT8 flag)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = flag; 
	len++;
	RICreateCmdHeader(header, RI_CMD_ONESHOT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RISetWlanType
  * Description : 设置网络类型
  * Input       : type : 网络类型
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISetWlanType(UINT32 type)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = type; 
	len++;
	RICreateCmdHeader(header, RI_CMD_WPRT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RISetSSID
  * Description : 设置SSID
  * Input       : ssid : ssid
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISetSSID(UINT8 *ssid)
{
	UINT8 buff[64];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	len = strlen((char *)ssid);
	buff[offset++] = len; 
	memcpy((void *)&buff[offset++], ssid, len);
	len++;
	RICreateCmdHeader(header, RI_CMD_SSID, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RIGetSSID
  * Description : 查询SSID
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIGetSSID(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_SSID, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
	
}
/**
  * Function    : RISetKey
  * Description : 设置密码
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISetKey(UINT8 type, UINT8 index, UINT8 *key)
{
	UINT8 buff[96];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = type; 
	len++;
	buff[offset++] = index; 
	len++;
	buff[offset++] = strlen((char *)key); 
	len++;
	memcpy((void *)&buff[offset++], key, strlen((char *)key));
	len += strlen((char *)key);
	RICreateCmdHeader(header, RI_CMD_KEY, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RIGetKey
  * Description : 获取密码
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIGetKey(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
		
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_KEY, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RISetEncrypt
  * Description : 设置加密方式
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISetEncrypt(UINT32 type)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = type; 
	len++;
	RICreateCmdHeader(header, RI_CMD_ENCRYPT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RIGetEncrypt
  * Description : 获取加密方式
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIGetEncrypt(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_ENCRYPT, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RISetNip
  * Description : 设置IP相关信息
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISetNip(UINT8 dhcp, UINT8 *ip, UINT8 *netmast, UINT8 *gateway, UINT8 *dns)
{
	UINT8 buff[96];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = dhcp; 
	len++;
//	if(dhcp)
//	{
		StringToIpaddr((char *)ip, &buff[offset]);
		offset += 4; len += 4;
		StringToIpaddr((char *)netmast, &buff[offset]);
		offset += 4; len += 4;
		StringToIpaddr((char *)gateway, &buff[offset]);
		offset += 4; len += 4;
		StringToIpaddr((char *)dns, &buff[offset]);
		offset += 4; len += 4;
//	}
	RICreateCmdHeader(header, RI_CMD_NIP, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;

}
/**
  * Function    : RIWlanJoin
  * Description : STA模式加入网络；AP模式创建网络
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIWlanJoin(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_WJOIN, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RIGetWlanStatus
  * Description : 获取网络连接状态
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIGetWlanStatus(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_LINK_STATUS, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RISocketCreate
  * Description : 创建socket连接
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISocketCreate(WM_SOCKET_INFO *sk)
{
	UINT8 buff[64];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = sk->protocol; 
	len++;
	buff[offset++] = sk->cs; 
	len++;
	buff[offset++] = 4; 
	len++;
	StringToIpaddr((char *)sk->hostname, &buff[offset]);
	offset += 4; len += 4;
	*(UINT16 *)&buff[offset] = Swap16(sk->remote);
	offset += 2; len += 2;
	*(UINT16 *)&buff[offset] = Swap16(sk->local);
	offset += 2; len += 2;
	RICreateCmdHeader(header, RI_CMD_SKCT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RISocketClose
  * Description : 关闭socket连接
  * Input       : fd : socket句柄
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISocketClose(UINT32 fd)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = fd; 
	len++;
	RICreateCmdHeader(header, RI_CMD_SKCLOSE, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RIGetSocketReceiveLen
  * Description : 获取当前收到的数据长度
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RIGetSocketReceiveLen(UINT32 fd)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = fd; 
	len++;
	RICreateCmdHeader(header, RI_CMD_SKSTT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RISocketTCPSend
  * Description : socket发送数据
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RISocketTCPSend(UINT32 fd, UINT8 *data, UINT16 len)
{
	UINT8 buff[64];
	WM_RI_HDR *header;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_RI_HDR *)buff;
	memcpy((void *)&buff[sizeof(WM_RI_HDR)], data, len);
	RICreateDataHeader(header, fd, len, 0);
	SPITxData(buff, sizeof(WM_RI_HDR) + len);
	return 1;
}
/**
  * Function    : RI_ENTS
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_ENTS(UINT8 ps_type, UINT8 wake_type, UINT16 delay_time,UINT16 wake_time)
{
	UINT8 buff[32]; UINT16* buff2;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = ps_type; 
	len++;

	buff[offset++] = wake_type; 
	len++;
	
    buff2 = (UINT16*)(buff+offset);
	
	buff2[0] = delay_time;
	len += 2;
	
	buff2[1] = wake_time;
	len += 2;
	
	RICreateCmdHeader(header, RI_CMD_PS, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_RSTF
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_RSTF(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_RESET_FLASH, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RI_IOC
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_IOC(UINT8 gpio, UINT8 direc, UINT8 status)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = gpio; 
	len++;
	
	buff[offset++] = direc; 
	len++;

	buff[offset++] = status; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_GPIO, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_QMAC
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_QMAC(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_MAC, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	
	return 0;	
}
/**
  * Function    : RI_QVER
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_QVER(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_VER, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;

}
/**
  * Function    : RI_WLEAVE
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WLEAVE(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_WLEAVE, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	
	return 0;
}
/**
  * Function    : RI_WSCAN
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WSCAN(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_WSCAN, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	
	return 0;
}

/**
  * Function    : RI_WPSST
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WPSST(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_WPSST, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RI_SKSDF
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_SKSDF(UINT8 socket)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);

	buff[offset++] = socket; 
	len++;

	RICreateCmdHeader(header, RI_CMD_SKSDF, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}

/**
  * Function    : RI_BSSID
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_BSSID(UINT8 enable,UINT8* bssid)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	memcpy((void*)&buff[offset],bssid,strlen(bssid));
	offset += strlen(bssid); len += strlen(bssid); 
	
	RICreateCmdHeader(header, RI_CMD_BSSID, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;		
}
/**
  * Function    : RI_BRDSSID
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_BRDSSID(UINT8 enable)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	RICreateCmdHeader(header, RI_CMD_BRD_SSID, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_GetBRDSSID
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_GetBRDSSID(void)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	
	RICreateCmdHeader(header, RI_CMD_BRD_SSID, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
/**
  * Function    : RI_CHL
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_CHL(UINT8 enable,UINT8 channel)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	buff[offset++] = channel;
	len++;
	
	RICreateCmdHeader(header, RI_CMD_CHNL, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR)+len);
	return 0;
}
/**
  * Function    : RI_WREG
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WREG(UINT16 region)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset] = region; 
	len += 2;
	
	RICreateCmdHeader(header, RI_CMD_WREG, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_WBGR
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WBGR(UINT8 bg_mode,UINT8 max_rate)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = bg_mode; 
	len++;
	buff[offset++] = max_rate; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_WBGR, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_WATC
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WATC(UINT8 enable)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_WATC, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_WPSM
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WPSM(UINT8 enable)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_WPSM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_WARM
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WARM(UINT8 enable)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_WARM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}

/**
  * Function    : RI_WWPS
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_WWPS(UINT8 enable,UINT8 pin)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	
	buff[offset++] = pin; 
	len++;	
	
	RICreateCmdHeader(header, RI_CMD_WPS, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_ATM
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_ATM(UINT8 mode)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = mode; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_ATM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}

/**
  * Function    : RI_ATRM
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_ATRM(WM_SOCKET_INFO *sk)
{
	UINT8 buff[64];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = sk->protocol; 
	len++;
	buff[offset++] = sk->cs; 
	len++;
	buff[offset++] = 4; 
	len++;
	StringToIpaddr((char *)sk->hostname, &buff[offset]);
	offset += 4; len += 4;
	*(UINT16 *)&buff[offset] = Swap16(sk->remote);
	offset += 2; len += 2;
	*(UINT16 *)&buff[offset] = Swap16(sk->local);
	offset += 2; len += 2;
	RICreateCmdHeader(header, RI_CMD_ATRM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_PORTM
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_PORTM(UINT8 mode)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = mode; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_PORTM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_UART
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_UART(UINT32 baudrate,UINT8 databit,UINT8 stopbit,UINT8 parity,UINT8 flowcontrol)
{
	UINT8 buff[32];UINT32* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	ptr = (UINT32*)(buff + offset);
	ptr[0] = baudrate; 
	offset += 3;
	len += 3;
	
	buff[offset++] = databit; 
	len++;

	buff[offset++] = stopbit; 
	len++;	

	buff[offset++] = parity; 
	len++;	
	
	buff[offset++] = flowcontrol; 
	len++;	
	
	RICreateCmdHeader(header, RI_CMD_PORTM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;

}
/**
  * Function    : RI_ATLT
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_ATLT(UINT16 length)
{
	UINT8 buff[32];
	UINT16* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);

  	ptr = (UINT16*)&buff[offset];
	ptr[0] = length; 
	len += 2;
	
	RICreateCmdHeader(header, RI_CMD_ATLT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_DNS
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_DNS(UINT8* dns)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);

	len = strlen((char *)dns);
	buff[offset++] = len; 
	memcpy((void *)&buff[offset++], dns, len);
	
	RICreateCmdHeader(header, RI_CMD_DNS, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_DDNS
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_DDNS(UINT8 enable,UINT8* user,UINT8* pass)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len,len1,len2 = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);

    buff[offset++] = enable; 
	len++;

	len1 = strlen((char *)user);
	buff[offset++] = len1; 
	memcpy((void *)&buff[offset], user, len1);
	offset += len1;


	len2 = strlen((char *)pass);
	buff[offset++] = len2; 
	memcpy((void *)&buff[offset], pass, len2);
	offset += len2;

	len += len1;
	len += len2;
	
    RICreateCmdHeader(header, RI_CMD_DDNS, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_UPNP
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_UPNP(UINT8 enable)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = enable; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_UPNP, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_DNAME
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_DNAME(UINT8* DNAME)
{
	UINT8 buff[64];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	len = strlen((char *)DNAME);
	buff[offset++] = len; 
	memcpy((void *)&buff[offset++], DNAME, len);
	len++;
	
	RICreateCmdHeader(header, RI_CMD_DNAME, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;

}
/**
  * Function    : RI_PASS
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_PASS(UINT8* pass)
{
	UINT8 buff[64];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);

	len = strlen((char *)pass);
	memcpy((void *)&buff[offset], pass, len);

	RICreateCmdHeader(header, RI_CMD_PASS, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
/**
  * Function    : RI_REGR
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_REGR(UINT32 addr,UINT8 num)
{
	UINT8 buff[128];
	UINT32* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);	

	ptr = (UINT32*)&buff[offset];
	ptr[0] = addr;
	len += 32; offset += 32;
	buff[offset++] = num;
	len++;
	
	RICreateCmdHeader(header, RI_CMD_REGR, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}
/**
  * Function    : RI_REGW
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_REGW(UINT32 addr,UINT8 num,UINT32 val)
{
	UINT8 buff[128];
	UINT32* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);	

	ptr = (UINT32*)&buff[offset];
	ptr[0] = addr;
	len += 32; offset += 32;
	buff[offset++] = num;
	len++;
	
	ptr = (UINT32*)&buff[offset];
	ptr[0] = val;
	len += 32; offset += 32;
	
	RICreateCmdHeader(header, RI_CMD_REGW, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}
/**
  * Function    : RI_RFR
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_RFR(UINT16 addr,UINT8 num)
{
	UINT8 buff[128];
	UINT16* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);	

	ptr = (UINT16*)&buff[offset];
	ptr[0] = addr;
	len += 16; offset += 16;
	buff[offset++] = num;
	len++;
	
	RICreateCmdHeader(header, RI_CMD_RFR, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}
/**
  * Function    : RI_RFW
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_RFW(UINT16 addr,UINT8 num,UINT16 val)
{
	UINT8 buff[128];
	UINT16* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);	

	ptr = (UINT16*)&buff[offset];
	ptr[0] = addr;
	len += 16; offset += 16;
	buff[offset++] = num;
	len++;

	ptr = (UINT16*)&buff[offset];
	ptr[0] = val;
	len += 16; offset += 16;
	
	RICreateCmdHeader(header, RI_CMD_RFW, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}
/**
  * Function    : RI_FLSR
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_FLSR(UINT16 addr,UINT8 num)
{
	UINT8 buff[128];
	UINT32* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);	

	ptr = (UINT32*)&buff[offset];
	ptr[0] = addr;
	len += 32; offset += 32;
	buff[offset++] = num;
	len++;
	
	RICreateCmdHeader(header, RI_CMD_FLSR, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}
/**
  * Function    : RI_FLSW
  * Description : val 的值暂时为一个    ，num==1
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_FLSW(UINT32 addr,UINT8 num,UINT32 val)
{
	UINT8 buff[128];
	UINT32* ptr;
	WM_SPI_HDR *header;
	UINT16 len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);	

	ptr = (UINT32*)&buff[offset];
	ptr[0] = addr;
	len += 32; offset += 32;
	buff[offset++] = num;
	len++;
	
	ptr = (UINT32*)&buff[offset];
	ptr[0] = val;
	len += 32; offset += 32;
	
	RICreateCmdHeader(header, RI_CMD_FLSW, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;	
}
/**
  * Function    : RI_UPDM
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
UINT32 RI_UPDM(UINT8 mode)
{
	UINT8 buff[32];
	WM_SPI_HDR *header;
	UINT16 len = 0, offset = 0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	
	buff[offset++] = mode; 
	len++;
	
	RICreateCmdHeader(header, RI_CMD_UPDM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
///////////////////////////////////dmh add for int gpio///////////////////////////////////////////
///////////////////////////////////dmh add for int gpio///////////////////////////////////////////
///////////////////////////////////dmh add for int gpio///////////////////////////////////////////
///////////////////////////////////dmh add for int gpio///////////////////////////////////////////
///////////////////////////////////dmh add for int gpio///////////////////////////////////////////
///////////////////////////////////dmh add for int gpio///////////////////////////////////////////

OS_HISR  Gpio_Hisr;
OSFlagRef  GpioFlgRef=NULL;
OSFlagRef  	DetectFlgRef=NULL;
OSATaskRef 	Detect_taskref=NULL;
char 		Detect_StackPtr[1024*200]={0};

void Gpio_Handler(void);
void GPIOIRQHandler (void);
void Detect_TaskEntry(void);
static void init_int_gpio(void);

static void init_int_gpio(void)
{
	int ret;
	GPIOConfiguration config;
	
	ret = OSAFlagCreate( &GpioFlgRef); 
	ASSERT(ret == OS_SUCCESS);
	Os_Create_HISR(&Gpio_Hisr, "Gpio_Hisr", Gpio_Handler, 2);
	OSAFlagCreate( &DetectFlgRef);  
    OSATaskCreate(&Detect_taskref, Detect_StackPtr,1024*200,1,"Detect_task",Detect_TaskEntry,NULL);
	config.pinDir = GPIO_IN_PIN;
	config.pinEd = GPIO_FALL_EDGE;
	config.pinPull = GPIO_PULLUP_ENABLE;
	config.isr = GPIOIRQHandler;
	GpioInitConfiguration(H_SPI_INT,config);

	SPINSS(1);
	
	
}
void GPIOIRQHandler (void)
{
	OS_Activate_HISR(&Gpio_Hisr);
	
}

void Gpio_Handler (void)
{
	
	OSAFlagSet(DetectFlgRef, 0x01, OSA_FLAG_OR);
}
void Detect_TaskEntry(void)
{
	GPIO_ReturnCode ret;
	UINT32 value;
	OSA_STATUS status = OS_SUCCESS;
	UINT32 flag_value = 0;

	while(1)
	{
		status = OSAFlagWait(DetectFlgRef, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);

		if(flag_value & 0x01)
		{
			SPI_PRINT("GPIOWAKEDetect_TaskEntry Spi Data Int \r\n");
			SPIRxData();
			
		}
	}
}
/**
  * Function    : WMRI_Init
  * Description : WMIOT602初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void WMRI_Init(void)
{
	init_int_gpio();
	drv_spi_init(SSP_SPI_MODE0, SPI_CLK_6_5MHZ);
	
}
// End of file : wmri.h 2021-12-31 16:03:59 by: win 


