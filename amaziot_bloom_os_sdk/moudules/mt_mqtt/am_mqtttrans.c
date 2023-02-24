/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : am_mqtttrans.c
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "UART.h"
#include "osa.h"
#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "sys.h"
#include "mqttclient.h"

#include "am_mqtttrans.h"
#include "am_file.h"
#include "am_gpio.h"
#include "am_at_cmds.h"
#include "utils_common.h"

#ifdef INCL_MOUDULES_MT_MQTT
extern trans_conf transconf;//weihu yi ge save in flash

//uart
extern OSFlagRef _sendflag_ref;
extern unsigned int send_count;
extern char sendBuf[4096];

/***********************************************************************
*	MQTT FUNCTION PROTOTYPES
***********************************************************************/
UINT8 subFlag=0;
mqtt_client_t *client = NULL;  //dmh20210329

/***********************************************************************
*	LOCAL FUNCTION PROTOTYPES
***********************************************************************/
static void cachercv_thread(void);
static void messageArrived(void* client, message_data_t* data);
static void pre_serverhandle(int len , char *rcvdata);

/***********************************************************************
*	SOCKET FUNCTION RELATED DEFINE
***********************************************************************/
/*function for socket init */
void init_mqtt_conf(void)
{
	//cache thread
	sys_thread_new("cachercv_thread", cachercv_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 161);
	
}
/*thread for cache rev */
static void cachercv_thread(void)
{    
    OSA_STATUS status;
    UINT32 flag_value;
    
    while(1) {
        status = OSAFlagWait(_sendflag_ref, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
        if (flag_value & 0x01) {
           if(MQTTIsConnected(client)){
				uint16_t send_size = send_count;
				//add link len
				if(transconf.linkflag==2 || transconf.linkflag==3)
					send_size += strlen(transconf.link);
				char *data = malloc(send_size);
		        if (data == NULL)
		        {
		        	printf("dmhtest:malloc(send_size) error ,send_size:%d",send_size);
		            return;
		        }
		        memset(data, 0, send_size);
				//add link data
				if(send_size>send_count){
					memcpy(data, transconf.link, strlen(transconf.link));
					memcpy(data+strlen(transconf.link), sendBuf, send_count);
				}else
		        	memcpy(data, sendBuf, send_count);
				mqtt_send(data,send_size);	
				free(data);
			}
			memset(sendBuf, 0, sizeof(sendBuf));
			send_count = 0;
        }
    }	
}
/*function for server data*/
static void pre_serverhandle(int len , char *rcvdata)
{

    printf("%s---%d, len=%d",__FUNCTION__,__LINE__,len);
	//judge is net atcmd 
	if(utils_is_begin_with(rcvdata,transconf.cmdpw) == 1){
		process_at_cmd_mode(rcvdata,len);
	}
	else
    	send_to_uart(rcvdata, len);

}
/*function for messageArrived*/
static void messageArrived(void* client, message_data_t* data)
{
    (void)client;
	//deal down data
	//UART_SEND_DATA((UINT8 *)data->message->payload, data->message->payloadlen);
	pre_serverhandle(data->message->payloadlen,data->message->payload);
}
/*function for init mqtt*/
void wait_init_mqtt(void)
{
	int errorCnt=0;//dmh
	
	mqtt_log_init();

    printf("%s()... mqtt init begin...", __FUNCTION__);

	    
	wait_init:

	if(client==NULL){
		client = mqtt_lease();	
	}

	if (client==NULL)
    {
		sleep(3);
		errorCnt++;
		if(errorCnt>10){
			//reset
			send_to_uart("\r\nMqtt Init Error! Moudle Rebooting...\r\n", 40);
		    //sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
		}	
        goto wait_init;
    }
	else
    {
    	mqtt_set_read_buf_size(client,4096);
		mqtt_set_write_buf_size(client,4096);
		printf("mqtt_lease ok!\n");
    }
	errorCnt = 0;
	
    return 0;

}
/*function for mqtt connect*/
void wait_connect_mqtt(void)
{
	int rc = -1;

    int errorCnt=0;//dmh
    mqtt_set_version(client,4);
	wait_connect:	
	mqtt_set_host(client, transconf.ip);
    mqtt_set_port(client, transconf.port);
	if(0 != strncmp(transconf.username, "NULL", 4))
    	mqtt_set_user_name(client, transconf.username);
	if(0 != strncmp(transconf.password, "NULL", 4))
    	mqtt_set_password(client, transconf.password);
    mqtt_set_client_id(client, transconf.clientid);
    mqtt_set_clean_session(client, transconf.cleansession);
	mqtt_set_keep_alive_interval(client, transconf.keeplive);

	mqtt_set_interceptor_handler(client,messageArrived);

    if((rc = mqtt_connect(client)) != 0)
    {
        printf("Return code from Mqtt connect is %d\n",rc);
		sleep(3);
		errorCnt++;
		if(errorCnt>10){
			//reset
			send_to_uart("\r\nMqtt Connect Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
		}	
        goto wait_connect;
    }
    else
    {
    	
        printf("Mqtt connected\n");
    }
	errorCnt = 0;
    return 0;
	
	
}
/*function for mqtt subscribe*/
void mqtt_subscribe_topic(void)
{
	int rc = 0;
	if(transconf.subflag == 1){
		if(strlen(transconf.subtopic)>0){
			if((rc = mqtt_subscribe(client,transconf.subtopic,transconf.subqos,messageArrived)) != 0)
		    {
		        printf("Return code from Mqtt subcribe is %d\n",rc);
		    }else
			{
				printf("Subscribe Success!\n");
				subFlag=1;
				
			}
		}
		if(strlen(transconf.subtopic1)>0){
			if((rc = mqtt_subscribe(client,transconf.subtopic1,transconf.subqos,messageArrived)) != 0)
		    {
		        printf("Return code from Mqtt subcribe1 is %d\n",rc);
		    }else
			{
				printf("Subscribe1 Success!\n");
				subFlag=1;
				
			}
		}
		if(strlen(transconf.subtopic2)>0){
			if((rc = mqtt_subscribe(client,transconf.subtopic2,transconf.subqos,messageArrived)) != 0)
		    {
		        printf("Return code from Mqtt subcribe2 is %d\n",rc);
		    }else
			{
				printf("Subscribe2 Success!\n");
				subFlag=1;
				
			}
		}
	}else{
		if(strlen(transconf.subtopic)>0){
			if((rc = mqtt_unsubscribe(client,transconf.subtopic)) != 0)
		    {
		        printf("Return code from Mqtt unsubcribe is %d\n",rc);
		    }else
			{
				printf("UnSubscribe Success!\n");
			}
		}
		if(strlen(transconf.subtopic1)>0){
			if((rc = mqtt_unsubscribe(client,transconf.subtopic1)) != 0)
		    {
		        printf("Return code from Mqtt unsubcribe1 is %d\n",rc);
		    }else
			{
				printf("UnSubscribe1 Success!\n");
			}
		}
		if(strlen(transconf.subtopic2)>0){
			if((rc = mqtt_unsubscribe(client,transconf.subtopic2)) != 0)
		    {
		        printf("Return code from Mqtt unsubcribe2 is %d\n",rc);
		    }else
			{
				printf("UnSubscribe2 Success!\n");
			}
		}
		
	}
    return 0;
	
}
/*function for check mqtt is connected*/
bool MQTTIsConnected(mqtt_client_t* c)
{
	bool status = false;
	if(client){
		if(client->mqtt_client_state == CLIENT_STATE_CONNECTED)
			status = true;
	}
	return status;
}
/*function for mqtt disconnect*/
void MQTTDisconnect(mqtt_client_t* c)
{
	mqtt_disconnect(c);
}
/*function for mqtt send*/
int mqtt_send(const char *data, size_t data_sz)
{
    mqtt_message_t message;
    memset(&message, 0, sizeof(message));

   	message.qos = transconf.pubqos;
	message.retained = transconf.retain;
	message.payload = data;
	message.payloadlen = data_sz;

    return mqtt_publish(client, transconf.pubtopic, &message);
}

#endif /* ifdef INCLI_MOUDULES_MT_MQTT.2022-1-25 17:09:29 by: win */



