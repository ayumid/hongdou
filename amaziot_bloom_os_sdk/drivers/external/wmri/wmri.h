//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : wmri.h
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _WMRI_H_
#define _WMRI_H_

// Includes ---------------------------------------------------------------------
#include "drv_spi.h"
// Public defines / typedef -----------------------------------------------------
#define H_SPI_CS         8	//PIN 1
#define H_SPI_INT        2	//PIN 2

typedef struct
{
	int type;
	char ip[128];
	int port;
}wifi_conf;


typedef struct{
    
    int len;
    UINT8 *UArgs;
}MsgSpiParam;

//////////////////app//////////////////////////////////
typedef struct _APP_SYS{
	UINT32 WlanStatus;
	UINT32 FdTcp;
	UINT32 FdUdp;
}APP_SYS;

/***********************************************************************
* WLAN
***********************************************************************/
#define APP_WLAN_STA			0
#define APP_WLAN_AP				2

#define APP_ENCRY_OPEN			0
#define APP_ENCRY_WEP64			1
#define APP_ENCRY_WEP128		2
#define APP_ENCRY_WPA_TKIP		3
#define APP_ENCRY_WPA_CCMP		4
#define APP_ENCRY_WPA2_TKIP		5
#define APP_ENCRY_WPA2_CCMP		6

#define APP_KEY_HEX				0
#define APP_KEY_ASCII			1

#define APP_KEY_DHCP_ENABLE		0
#define APP_KEY_DHCP_DISABLE	1

/***********************************************************************
* WLAN Message ID
***********************************************************************/
#define APP_WLAN_UP				512
#define APP_WLAN_DOWN			513


/////////////////app//////////////////////////////////


/***************************************************************
 * SPI Format definition
 ***************************************************************/

#define SPI_RX_DESC_NUM			1000
#define SPI_REG_TIMEOUT			200
#define SPI_TIMEOUT				100		// 200mS

#define SPI_REG_INT_STTS		0x06
#define SPI_REG_RX_DAT_LEN		0x02
#define SPI_REG_TX_BUFF_AVAIL	0x03
#define SPI_CMD_RX_DATA			0x10
#define SPI_CMD_TX_CMD			0x91
#define SPI_CMD_TX_DATA			0x90
/*
typedef struct _WM_SPI_RX_DESC{
    UINT8 valid;
}WM_SPI_RX_DESC;

*/
/***************************************************************
 * High speed/HSPI DATA/CMD/EVENT/RSP Format definition
 ***************************************************************/
#define RI_CMD_NOP				0
#define RI_CMD_RESET			1
#define RI_CMD_PS				2
#define RI_CMD_RESET_FLASH		3
#define RI_CMD_PMTF				4
#define RI_CMD_GPIO				5
#define RI_CMD_MAC				6
#define RI_CMD_VER				7
#define RI_CMD_WJOIN			0x20
#define RI_CMD_WLEAVE			0x21
#define RI_CMD_WSCAN			0x22
#define RI_CMD_LINK_STATUS		0x23
#define RI_CMD_WPSST			0x24
#define RI_CMD_SKCT				0x28
#define RI_CMD_SKSTT			0x29
#define RI_CMD_SKCLOSE			0x2A
#define RI_CMD_SKSDF			0x2B
#define RI_CMD_ONESHOT				0x2C
#define RI_CMD_HTTPC				0x2D
#define RI_CMD_WPRT					0x40
#define RI_CMD_SSID					0x41
#define RI_CMD_KEY					0x42
#define RI_CMD_ENCRYPT				0x43
#define RI_CMD_BSSID				0x44
#define RI_CMD_BRD_SSID				0x45
#define RI_CMD_CHNL					0x46
#define RI_CMD_WREG					0x47
#define RI_CMD_WBGR					0x48
#define RI_CMD_WATC					0x49
#define RI_CMD_WPSM					0x4A
#define RI_CMD_WARM					0x4B
#define RI_CMD_WPS					0x4C
#define RI_CMD_NIP					0x60
#define RI_CMD_ATM					0x61
#define RI_CMD_ATRM					0x62
#define RI_CMD_AOLM					0x63
#define RI_CMD_PORTM				0x64
#define RI_CMD_UART					0x65
#define RI_CMD_ATLT					0x66
#define RI_CMD_DNS					0x67
#define RI_CMD_DDNS					0x68
#define RI_CMD_UPNP					0x69
#define RI_CMD_DNAME				0x6A
#define RI_CMD_PASS				    0x6B
#define RI_CMD_DBG					0xF0
#define RI_CMD_REGR					0xF1
#define RI_CMD_REGW					0xF2
#define RI_CMD_RFR					0xF3
#define RI_CMD_RFW					0xF4
#define RI_CMD_FLSR					0xF5
#define RI_CMD_FLSW					0xF6
#define RI_CMD_UPDM					0xF7
#define RI_CMD_UPDD 				0xF8

#define RI_EVENT_INIT_END			0xE0
#define RI_EVENT_CRC_ERR			0xE1
#define RI_EVENT_SCAN_RES			0xE2
#define RI_EVENT_JOIN_RES			0xE3
#define RI_EVENT_STA_JOIN			0xE4
#define RI_EVENT_STA_LEAVE			0xE5
#define RI_EVENT_LINKUP				0xE6
#define RI_EVENT_LINKDOWN			0xE7
#define RI_EVENT_TCP_CONN			0xE8
#define RI_EVENT_TCP_JOIN			0xE9
#define RI_EVENT_TCP_DIS			0xEA
#define RI_EVENT_TX_ERR				0xEB 

#define RI_CMD_SYNC					0xAA
#define RI_DATA						0x00
#define RI_CMD						0x01
#define RI_CMD_MT_EVENT				0x00
#define RI_CMD_MT_REQ				0x01
#define RI_CMD_MT_RSP				0x02

typedef struct _WM_RI_HDR 
{
	UINT8 sync;
	UINT8 type;
	UINT16 length;
	UINT8 seq_num;
	UINT8 flag;
	UINT8 dest_addr;
	UINT8 chk; 
}WM_RI_HDR;

typedef struct _WM_RICMD_HDR 
{
	UINT8 msg_type;
	UINT8 code;
	UINT8 err;
	UINT8 ext;
}WM_RI_CMD_HDR;

typedef struct  _WM_RICMD_EXT_HDR 
{
	UINT32 remote_ip;
	UINT16 remote_port;
	UINT16 local_port;
}WM_RICMD_EXT_HDR;

typedef struct _WM_SPI_HDR 
{
	WM_RI_HDR hdr;
	WM_RI_CMD_HDR cmd;
}WM_SPI_HDR;


#define Swap16(v)	(((v & 0xff) << 8) | (v >> 8))

enum socket_protocol{
    SOCKET_PROTO_TCP,      /* TCP Protocol */
    SOCKET_PROTO_UDP,     /* UDP Protocol */
};

enum socket_cs_mode{
    SOCKET_CS_MODE_CLIENT,    /* Client mode */
    SOCKET_CS_MODE_SERVER,    /* Server mode */
};

typedef struct _WM_SOCKET_INFO{
	enum socket_protocol protocol;
	enum socket_cs_mode cs;
	UINT8 hostname[32];
	UINT16 remote;
	UINT16 local;
}WM_SOCKET_INFO;

// Public functions prototypes --------------------------------------------------
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
void SPIRxData(void);
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
UINT32 RISpiApply(void);
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
UINT32 RIGetWlanType(void);
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
UINT32 RISaveParam(void);
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
UINT32 RIResetDevice(void);
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
UINT32 RISetOneShotCfg(UINT8 flag);
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
UINT32 RISetWlanType(UINT32 type);
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
UINT32 RISetSSID(UINT8 *ssid);
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
UINT32 RIGetSSID(void);
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
UINT32 RISetKey(UINT8 type, UINT8 index, UINT8 *key);
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
UINT32 RIGetKey(void);
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
UINT32 RISetEncrypt(UINT32 type);
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
UINT32 RIGetEncrypt(void);
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
UINT32 RISetNip(UINT8 dhcp, UINT8 *ip, UINT8 *netmast, UINT8 *gateway, UINT8 *dns);
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
UINT32 RIWlanJoin(void);
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
UINT32 RIGetWlanStatus(void);
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
UINT32 RISocketCreate(WM_SOCKET_INFO *sk);
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
UINT32 RISocketClose(UINT32 fd);
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
UINT32 RIGetSocketReceiveLen(UINT32 fd);
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
UINT32 RISocketTCPSend(UINT32 fd, UINT8 *data, UINT16 len);

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
void WMRI_Init(void);

#endif /* ifndef _WMRI_H_.2021-12-31 16:29:59 by: win */


