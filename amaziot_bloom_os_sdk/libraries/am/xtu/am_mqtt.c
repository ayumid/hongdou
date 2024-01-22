//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_mqtt.c
// Auther      : zhaoning
// Version     :
// Date : 2023-8-28
// Description :上行最多995字节，topic OJUZTBOP9O/dev001/data 下行最多997字节，topic topic OJUZTBOP9O/dev001/data
//          
//          
// History     :
//     
//    1. Time         :  2023-8-28
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_mqtt.h"

#include "mqttclient.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

DTU_MQTT_PARAM_T dtu_mqtt_t;

// Private functions prototypes -------------------------------------------------

static void dtu_mqtt_cachercv_thread(void);
static void dtu_mqtt_msg_dl_arriver(void* dtu_mqtt_client, message_data_t* data);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_mqtt_ctx
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_MQTT_PARAM_T* dtu_get_mqtt_ctx(void)
{
    return &dtu_mqtt_t;
}

/**
  * Function    : dtu_mqtt_wait_init_mqtt
  * Description : 初始化mqtt
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_mqtt_wait_init_mqtt(void)
{
    
    mqtt_log_init();

//    uprintf("%s()... mqtt init begin...", __FUNCTION__);
        
    //初始化
    if(dtu_mqtt_t.dtu_mqtt_client == NULL)
    {
        dtu_mqtt_t.dtu_mqtt_client = mqtt_lease();
    }
    //判断是否初始化成功，失败就复位，一般不会失败
    if(dtu_mqtt_t.dtu_mqtt_client == NULL)
    {
        dtu_sleep(3);
        PM812_SW_RESET();
    }

}

/**
  * Function    : dtu_mqtt_wait_connect_mqtt
  * Description : 等待mqtt连接
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_mqtt_wait_connect_mqtt(void)
{
    int rc = -1;
    int errorCnt = 0;
    char portstr[16] = {0};
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;
        
    dtu_mqtt_file_ctx = dtu_get_file_ctx();

    //MV9一次发布数据1024，
    mqtt_set_version(dtu_mqtt_t.dtu_mqtt_client,4);
wait_connect:    
    memset(portstr, 0, 16);
    sprintf(portstr, "%d", dtu_mqtt_file_ctx->mqtt.port);
    mqtt_set_host(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.ip);
    mqtt_set_port(dtu_mqtt_t.dtu_mqtt_client, portstr);
    if(0 != strncmp(dtu_mqtt_file_ctx->mqtt.username, "NULL", 4))
    {
        mqtt_set_user_name(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.username);
    }
    if(0 != strncmp(dtu_mqtt_file_ctx->mqtt.password, "NULL", 4))
    {
        mqtt_set_password(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.password);
    }
    mqtt_set_client_id(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.clientid);
    mqtt_set_clean_session(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.cleansession);
    mqtt_set_keep_alive_interval(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.keeplive);

    mqtt_set_interceptor_handler(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_msg_dl_arriver);

    if((rc = mqtt_connect(dtu_mqtt_t.dtu_mqtt_client)) != 0)
    {
        uprintf("return code from mqtt connect is %d\n",rc);
        dtu_sleep(3);
        errorCnt++;
        if(errorCnt>10){
            //reset
            dtu_send_to_uart("\r\nmqtt connect error! moudle rebooting...\r\n", 40);
            dtu_sleep(3);//delay for write file
            PM812_SW_RESET();
        }    
        goto wait_connect;
    }
    else
    {
        
        uprintf("mqtt connected\n");
    }
    
    errorCnt = 0;
}

/**
  * Function    : dtu_mqtt_subscribe_topic
  * Description : 订阅topic
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_mqtt_subscribe_topic(void)
{
    int rc = 0;
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;
        
    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    
    if(dtu_mqtt_file_ctx->mqtt.subflag == 1)
    {
        if(strlen(dtu_mqtt_file_ctx->mqtt.subtopic) > 0)
        {
            if((rc = mqtt_subscribe(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_file_ctx->mqtt.subtopic,dtu_mqtt_file_ctx->mqtt.subqos,dtu_mqtt_msg_dl_arriver)) != 0)
            {
                uprintf("Return code from Mqtt subcribe is %d\n",rc);
            }
            else
            {
                uprintf("Subscribe Success!\n");
                dtu_mqtt_t.dtu_mqtt_subflag = 1;
                
            }
        }
        if(strlen(dtu_mqtt_file_ctx->mqtt.subtopic1) > 0)
        {
            if((rc = mqtt_subscribe(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_file_ctx->mqtt.subtopic1,dtu_mqtt_file_ctx->mqtt.subqos,dtu_mqtt_msg_dl_arriver)) != 0)
            {
                uprintf("Return code from Mqtt subcribe1 is %d\n",rc);
            }
            else
            {
                uprintf("Subscribe1 Success!\n");
                dtu_mqtt_t.dtu_mqtt_subflag = 1;
                
            }
        }
        if(strlen(dtu_mqtt_file_ctx->mqtt.subtopic2) > 0)
        {
            if((rc = mqtt_subscribe(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_file_ctx->mqtt.subtopic2,dtu_mqtt_file_ctx->mqtt.subqos,dtu_mqtt_msg_dl_arriver)) != 0)
            {
                uprintf("Return code from Mqtt subcribe2 is %d\n",rc);
            }
            else
            {
                uprintf("Subscribe2 Success!\n");
                dtu_mqtt_t.dtu_mqtt_subflag = 1;
                
            }
        }
    }
    else
    {
        if(strlen(dtu_mqtt_file_ctx->mqtt.subtopic) > 0)
        {
            if((rc = mqtt_unsubscribe(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_file_ctx->mqtt.subtopic)) != 0)
            {
                uprintf("Return code from Mqtt unsubcribe is %d\n",rc);
            }
            else
            {
                uprintf("UnSubscribe Success!\n");
            }
        }
        if(strlen(dtu_mqtt_file_ctx->mqtt.subtopic1) > 0)
        {
            if((rc = mqtt_unsubscribe(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_file_ctx->mqtt.subtopic1)) != 0)
            {
                uprintf("Return code from Mqtt unsubcribe1 is %d\n",rc);
            }
            else
            {
                uprintf("UnSubscribe1 Success!\n");
            }
        }
        if(strlen(dtu_mqtt_file_ctx->mqtt.subtopic2) > 0)
        {
            if((rc = mqtt_unsubscribe(dtu_mqtt_t.dtu_mqtt_client,dtu_mqtt_file_ctx->mqtt.subtopic2)) != 0)
            {
                uprintf("Return code from Mqtt unsubcribe2 is %d\n",rc);
            }
            else
            {
                uprintf("UnSubscribe2 Success!\n");
            }
        }
        
    }
    
}

/**
  * Function    : dtu_mqtt_is_connect
  * Description : 判断mqtt连接状态
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
bool dtu_mqtt_is_connect(mqtt_client_t* c)
{
    bool status = false;
    
    if(dtu_mqtt_t.dtu_mqtt_client)
    {
        if(dtu_mqtt_t.dtu_mqtt_client->mqtt_client_state == CLIENT_STATE_CONNECTED)
            status = true;
    }
    
    return status;
}

/**
  * Function    : dtu_mqtt_disconnect
  * Description : 断开mqtt连接状态
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_mqtt_disconnect(mqtt_client_t* c)
{
    mqtt_disconnect(c);
}

/**
  * Function    : dtu_mqtt_msg_dl_arriver
  * Description : mqtt下行数据回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_mqtt_msg_dl_arriver(void* dtu_mqtt_client, message_data_t* data)
{
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;
        
    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    
    (void)dtu_mqtt_t.dtu_mqtt_client;
    //deal down data
//    UART_SEND_DATA((UINT8 *)data->message->payload, data->message->payloadlen);
    uprintf("%s[%d] len:%d",__FUNCTION__,__LINE__,data->message->payloadlen);
    //判断数据最前几个字节是否是网络at指令标志
    if(is_begin_with(data->message->payload,dtu_mqtt_file_ctx->net_at.cmdpw) == 1 && strstr((const char*)data->message->payload,"\r\n") != NULL)
    {
        //执行网络at
        dtu_process_at_cmd_mode(data->message->payload, data->message->payloadlen);
    }
    //判断是否是服务器下发的json消息
#ifdef DTU_TYPE_JSON_INCLUDE
    else if(NULL != strstr(data->message->payload, "msgType"))
    {
        //判断具体是协议中哪个json
        dtu_json_data_prase(data->message->payloadlen, data->message->payload);
    }
#endif /* ifdef DTU_TYPE_JSON_INCLUDE.2023-10-30 18:49:06 by: zhaoning */
    else
    {
        //发送数据到串口
        UART_SEND_DATA((UINT8 *)data->message->payload, data->message->payloadlen);
    }
}

/**
  * Function    : dtu_mqtt_send
  * Description : mqtt信息发送
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_mqtt_send(const char *data, size_t data_sz)
{
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;
        
    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    
    mqtt_message_t message = {0};
    memset(&message, 0, sizeof(message));

    message.qos = dtu_mqtt_file_ctx->mqtt.pubqos;
    message.retained = dtu_mqtt_file_ctx->mqtt.retain;
    message.payload = (void*)data;
    message.payloadlen = data_sz;

    return mqtt_publish(dtu_mqtt_t.dtu_mqtt_client, dtu_mqtt_file_ctx->mqtt.pubtopic, &message);
}

/**
  * Function    : dtu_mqtt_send_serial_data
  * Description : at接口数据发送到mqtt服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_mqtt_send_serial_data(DTU_MSG_UART_DATA_PARAM_T * uartData)
{
    int result = 0;
    uint32_t send_size = 0;
    char *data = NULL;
    DTU_FILE_PARAM_T* dtu_mqtt_file_ctx = NULL;
    DTU_MQTT_PARAM_T* dtu_mqtt_ctx = NULL;
    
    dtu_mqtt_file_ctx = dtu_get_file_ctx();
    dtu_mqtt_ctx = dtu_get_mqtt_ctx();
    
    uprintf("%s[%d]: DTU_MQTT_DATA_MODE recvlen: %d\n", __FUNCTION__, __LINE__, uartData->len);

    //判断是否连接是否正常
    if(dtu_mqtt_is_connect(dtu_mqtt_ctx->dtu_mqtt_client))
    {
        send_size = uartData->len;
        //判断linkflag，是否需要拼接注册包
        if(dtu_mqtt_file_ctx->reg.linkflag == 2 || dtu_mqtt_file_ctx->reg.linkflag == 3)
        {
            send_size += strlen(dtu_mqtt_file_ctx->reg.link);
        }
        
        data = malloc(send_size);
        if (data == NULL)
        {
            uprintf("malloc %d error",send_size);
        }
        
        memset(data, 0, send_size);
        //add link data
        if(send_size > uartData->len)
        {
            memcpy(data, dtu_mqtt_file_ctx->reg.link, strlen(dtu_mqtt_file_ctx->reg.link));
            memcpy(data + strlen(dtu_mqtt_file_ctx->reg.link), uartData->UArgs, uartData->len);
        }
        else
        {
            memcpy(data, uartData->UArgs, uartData->len);
        }
        //数据发送到服务器
        result = dtu_mqtt_send(data,send_size);
        //释放内存
        if(NULL != data)
        {
            free(data);
            data = NULL;
        }
    }
}

// End of file : am_mqtt.c 2023-8-28 14:57:22 by: zhaoning 

