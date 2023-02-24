/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : am_file.c
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "FDI_TYPE.h"
#include "FDI_FILE.h"

#include "am_file.h"
#include "utils_common.h"


trans_conf transconf;//weihu yi ge save in flash

/*function for file*/
void trans_conf_file_init(void)
{
	int fd;
	//init transconf0
	//init_trans_conf0();
	fd = FDI_fopen("trans_file", "rb");
	if (fd) {
	  printf("FDI_fopen is OK!\n");
	  transconf = trans_conf_file_read(fd);
	} else {
	  printf("FDI_fopen is ERROR!\n");
	  //init write tans_file
	  transconf = init_trans_conf(transconf);
	  trans_conf_file_write(transconf);
	}  
}
trans_conf trans_conf_file_read(int fd)
{
	trans_conf transconf;
	
    FDI_fread(&transconf,sizeof(transconf),1,fd);
	FDI_fclose(fd);
    
	return transconf;
}
int trans_conf_file_write(trans_conf transconf)
{
   int fd = 0;
   fd = FDI_fopen("trans_file", "wb");
   if (fd != 0){
        printf("debug> open file for write OK");
		FDI_fwrite(&transconf,sizeof(transconf), 1, fd);
   		FDI_fclose(fd);
		printf("debug> trans_file write OK");
   } else {
        printf("debug> open file for write error");
		return -1;
   }
   return 0;
}
trans_conf init_trans_conf(trans_conf transconf)
{
	
#ifdef INCL_MOUDULES_MT_MQTT
	sprintf(transconf.ip,"183.230.40.39");
	sprintf(transconf.port,"6002");
	
	sprintf(transconf.clientid,"534808251");
	transconf.keeplive = 120;
	transconf.cleansession = 0;
	sprintf(transconf.username,"259084");
	sprintf(transconf.password,"subscribe");

	sprintf(transconf.subtopic,"aaa");
	sprintf(transconf.subtopic1,"");
	sprintf(transconf.subtopic2,"");
	transconf.subflag = 1;
	transconf.subqos = 0;

	
	sprintf(transconf.pubtopic,"aaa");
	transconf.pubqos = 0;
	transconf.duplicate = 0;
	transconf.retain = 0;
	
	transconf.heartflag=1;
	sprintf(transconf.heart,"hearttest");
	transconf.hearttime=30;
	
	transconf.linkflag=1;
	sprintf(transconf.link,"linktest");

	sprintf(transconf.cmdpw,"am.iot");
	transconf.sim=1;
	transconf.simlock=0;
	
#endif /* ifdef INCL_MOUDULES_MT_MQTT.2022-1-25 17:11:26 by: win */
#ifdef INCL_MOUDULES_MT_TCP
	transconf.type = 0;
	sprintf(transconf.ip,"183.230.40.40");
	transconf.port=1811;
	transconf.heartflag=1;
	sprintf(transconf.heart,"hearttest");
	transconf.hearttime=30;
	transconf.linkflag=1;
	sprintf(transconf.link,"*275619#amaziot4000mt#AP4000MT*");
	transconf.yzcsq = 10;
	sprintf(transconf.cmdpw,"am.iot");
	transconf.sim=1;
	transconf.simlock=0;
#endif /* ifdef INCL_MOUDULES_MT_TCP.2022-1-25 17:12:37 by: win */

	return transconf;
}


