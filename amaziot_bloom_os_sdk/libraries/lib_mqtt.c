//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_mqtt.c
// Auther      : win
// Version     :
// Date : 2021-11-24
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-24
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "lib_mqtt.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : lib_mqtt_client_init
  * Description : MQTT客户端初始化
  * Input       : 
  * Output      : 
  * Return      : MQTT客户端实例
  * Auther      : win
  * Others      : 
  **/
mqtt_client_t * lib_mqtt_client_init(void)
{
    return mqtt_lease();
}
/**
  * Function    : lib_mqtt_connect
  * Description : 连接
  * Input       : c          MQTT客户端实例
  * Output      : 
  * Return      : 成功: 0   失败： -1
  * Auther      : win
  * Others      : 
  **/
int lib_mqtt_connect(mqtt_client_t* c)
{
    return mqtt_connect(c);
}

/**
  * Function    : lib_mqtt_subscribe
  * Description : 订阅
  * Input       : c              MQTT客户端实例
  *               topic_filter   订阅topic字符串指针
  *               qos     		 Qos
  *				  msg_handler    订阅接收回调函数
  * Output      : 
  * Return      : 成功: 0   失败： -1
  * Auther      : zhaoning
  * Others      : 
  **/
int lib_mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t msg_handler)
{
    return mqtt_subscribe(c,topic_filter,qos,msg_handler);
}
/**
  * Function    : lib_mqtt_unsubscribe
  * Description : 取消订阅
  * Input       : c              MQTT客户端实例
  *               topic_filter   订阅topic字符串指针             
  * Output      : 
  * Return      : 成功: 0   失败： -1
  * Auther      : win
  * Others      : 
  **/
int lib_mqtt_unsubscribe(mqtt_client_t* c, const char* topic_filter)
{
    return mqtt_unsubscribe(c,topic_filter);
}

/**
  * Function    : lib_mqtt_publish
  * Description : 发布
  * Input       : c               MQTT客户端实例
  *               topic_filter    发布topic字符串指针
  *               msg      		  发布消息指针
  * Output      : 
  * Return      : 成功: 0   失败： -1
  * Auther      : win
  * Others      : 
  **/
int lib_mqtt_publish(mqtt_client_t* c, const char* topic_filter, mqtt_message_t* msg)
{
    return mqtt_publish(c,topic_filter,msg);;
}

/**
  * Function    : lib_mqtt_disconnect
  * Description : 断开连接
  * Input       : c          MQTT客户端实例
  * Output      : 
  * Return      : 成功: 0   失败： -1
  * Auther      : zhaoning
  * Others      : 
  **/
int lib_mqtt_disconnect(mqtt_client_t* c)
{
    return mqtt_disconnect(c);
}
/**
  * Function    : lib_mqtt_shutdown
  * Description : 注销MQTT客户端实例
  * Input       : c          MQTT客户端实例
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
int lib_mqtt_shutdown(mqtt_client_t* c)
{
    return mqtt_release(c);
}
/**
  * Function    : lib_mqtt_set_host
  * Description : 设置要连接的MQTT服务器地址
  * Input       : c   MQTT客户端实例
  *               ip  域名/IP
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_host(mqtt_client_t* c,const char* ip)
{
	mqtt_set_host(c, ip);   
}
/**
  * Function    : lib_mqtt_set_port
  * Description : 设置要连接的MQTT服务器端口号
  * Input       : c     MQTT客户端实例
  *               port  端口号
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_port(mqtt_client_t* c,const char* port)
{
	mqtt_set_port(c, port);
}
/**
  * Function    : lib_mqtt_set_client_id
  * Description : 设置客户端的ID
  * Input       : c     MQTT客户端实例
  *               clientid  客户端的ID
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_client_id(mqtt_client_t* c,const char* clientid)
{
	mqtt_set_client_id(c, clientid);
}
/**
  * Function    : lib_mqtt_set_user_name
  * Description : 设置客户端的用户名
  * Input       : c     MQTT客户端实例
  *               username  客户端的用户名
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_user_name(mqtt_client_t* c,const char* username)
{
	mqtt_set_user_name(c, username);
}
/**
  * Function    : lib_mqtt_set_password
  * Description : 设置客户端的密码
  * Input       : c     MQTT客户端实例
  *               password  客户端的密码
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_password(mqtt_client_t* c,const char* password)
{
	mqtt_set_password(c,password);
}
/**
  * Function    : lib_mqtt_set_clean_session
  * Description : 设置在断开连接后清除会话
  * Input       : c     MQTT客户端实例
  *               cleansession  清除标志
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_clean_session(mqtt_client_t* c,int cleansession)
{
	 mqtt_set_clean_session(c, 1);
}
/**
  * Function    : lib_mqtt_set_keep_alive_interval
  * Description : 设置心跳间隔时间（秒）
  * Input       : c     MQTT客户端实例
  *               keepalive  心跳时间
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_keep_alive_interval(mqtt_client_t* c,int keepalive)
{
	mqtt_set_keep_alive_interval(c,keepalive);
}
/**
  * Function    : lib_mqtt_set_read_buf_size
  * Description : 设置读数据缓冲区的大小
  * Input       : c     MQTT客户端实例
  *               size  读数据缓冲区的大小
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_read_buf_size(mqtt_client_t* c,int size)
{
	mqtt_set_read_buf_size(c,size);
}
/**
  * Function    : lib_mqtt_set_write_buf_size
  * Description : 设置写数据缓冲区的大小
  * Input       : c     MQTT客户端实例
  *               size  写数据缓冲区的大小
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_write_buf_size(mqtt_client_t* c,int size)
{
	mqtt_set_write_buf_size(c,size);
}
/**
  * Function    : lib_mqtt_set_version
  * Description : 设置MQTT协议的版本，默认值是4，MQTT版本为3.1.1
  * Input       : c     MQTT客户端实例
  *               version  MQTT协议的版本
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_version(mqtt_client_t* c,int version)
{
	mqtt_set_version(c,version);
}
/**
  * Function    : lib_mqtt_set_interceptor_handler
  * Description : 设置拦截器处理函数，将所有底层数据上报给用户
  * Input       : c     MQTT客户端实例
  *               int_handler  拦截器处理函数
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_interceptor_handler(mqtt_client_t* c,interceptor_handler_t int_handler)
{
	mqtt_set_interceptor_handler(c,int_handler);
}
/**
  * Function    : lib_mqtt_set_ssl
  * Description : 设置MQTT的SSL加密
  * Input       : c     MQTT客户端实例
  *               sslFlag  1加密 0 不加密 默认为0
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_ssl(mqtt_client_t* c,int sslFlag)
{
	network_set_channel(c->mqtt_network,sslFlag);
}

/**
  * Function    : lib_mqtt_set_ca
  * Description : 设置要连接的MQTT服务器ca证书
  * Input       : c     MQTT客户端实例
  *               ca    服务器ca证书字符串
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void lib_mqtt_set_ca(mqtt_client_t* c, const char *ca)
{
	network_set_ca(c->mqtt_network,ca);
}



// End of file : lib_mqtt.h 2021-11-24 11:49:05 by: win 

