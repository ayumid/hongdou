/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

  File Name     : am_file.h
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/


#ifndef _AM_FILE_H_
#define _AM_FILE_H_
#include <stdbool.h>
#include "incl_config.h"

typedef struct
{
#ifdef INCL_MOUDULES_MT_MQTT
	char ip[65];
	char port[10];

	char clientid[512];
	UINT16 keeplive;
	UINT8 cleansession;
	char username[512];
	char password[512];
	char subtopic[130];
	char subtopic1[130];
	char subtopic2[130];
	UINT8 subflag;
	UINT8 subqos;
	char pubtopic[130];
	UINT8 pubqos;
	UINT8 duplicate;
	UINT8 retain;
	
	UINT8 heartflag;
	int hearttime;
	char heart[52];
	UINT8 linkflag;
	char link[52];
#endif /* ifdef INCL_MOUDULES_MT_MQTT.2022-1-25 17:13:37 by: win */


#ifdef INCL_MOUDULES_MT_TCP
	UINT8 type;
	char ip[65];
	int port;
	UINT8 heartflag;
	int hearttime;
	char heart[52];
	UINT8 linkflag;
	char link[52];
	UINT8 yzcsq;
#endif /* ifdef INCL_MOUDULES_MT_TCP.2022-1-25 17:13:59 by: win */
	
	char cmdpw[10];
	UINT8 sim;
	UINT8 simlock;
}trans_conf;

void trans_conf_file_init(void);
trans_conf trans_conf_file_read(int fd);
int trans_conf_file_write(trans_conf transconf);
trans_conf init_trans_conf(trans_conf transconf);


#endif
