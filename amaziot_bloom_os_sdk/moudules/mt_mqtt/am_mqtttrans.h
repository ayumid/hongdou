/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

  File Name     : am_mqtttrans.h
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/


#ifndef _AM_MQTTTRANS_H_
#define _AM_MQTTTRANS_H_
#include "incl_config.h"

#ifdef INCL_MOUDULES_MT_MQTT
void init_mqtt_conf(void);
void wait_init_mqtt(void);
void wait_connect_mqtt(void);
void mqtt_subscribe_topic(void);
bool MQTTIsConnected(mqtt_client_t* c);
void MQTTDisconnect(mqtt_client_t* c);
int mqtt_send(const char *data, size_t data_sz);
#endif /* ifdef INCL_MOUDULES_MT_MQTT.2022-1-25 17:37:05 by: win */

#endif
