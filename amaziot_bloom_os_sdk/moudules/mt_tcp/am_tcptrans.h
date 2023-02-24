/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

  File Name     : am_tcptrans.h
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/


#ifndef _AM_TCPTRANS_H_
#define _AM_TCPTRANS_H_
#include "incl_config.h"

#ifdef INCL_MOUDULES_MT_TCP
typedef struct
{
    int fd;
    char remoteIp[256];
    int remotePort;
    int socketType; //0:TCP  1:UDP
}socketParam;

typedef struct
{
    int fd;
    socketParam *sock;
}sockRcvType;

void init_socket_conf(void);
void setSockCfn(void);
int socket_write(int sockfd, const void *data, size_t data_sz);
#endif /* ifdef INCL_MOUDULES_MT_TCP.2022-1-25 17:37:52 by: win */

#endif
