//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_json.c
// Auther      : zhaoning
// Version     :
// Date : 2023-9-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-18
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_json.h"

#include "download.h"

#include "am.h"
#ifdef DTU_TYPE_DODIAI_INCLUDE
#include "am_di.h"
#include "am_do.h"
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-31 16:12:27 by: zhaoning */

// Private defines / typedefs ---------------------------------------------------

#define DTU_JSON_TASK_MSGQ_MSG_SIZE              (sizeof(DTU_JSON_MSG_T))
#define DTU_JSON_TASK_MSGQ_QUEUE_SIZE            (200)

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static OSMsgQRef dtu_json_msgq;

static UINT8 dtu_json_task_stack[DTU_JSON_TASK_STACK_SIZE];
static OSTaskRef dtu_json_task_ref = NULL;

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_json_task_send_msgq
  * Description : 发送消息给模块主任务，错误后串口输出信息
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_json_task_send_msgq(DTU_JSON_MSG_T * msg)
{
    int ret = 0;

    //发送消息给json主任务
    OSA_STATUS status = OSAMsgQSend(dtu_json_msgq, DTU_JSON_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        uprintf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgId, status);
        ret = -1;
    }    
    
    return ret;
}

/**
  * Function    : dtu_sys_json_get_sys_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_sys_json_get_sys_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_SYS_MSG_ID_GET_SYS_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_sys_json_get_sys_config_res
  * Description : 回复服务器获取sys配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_sys_json_get_sys_config_res(void)
{
    cJSON *gdocr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;

    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gdocr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enHb", cJSON_CreateNumber(dtu_file_ctx->hb.heartflag));
    cJSON_AddItemToObject(data, "cycHb", cJSON_CreateNumber(dtu_file_ctx->hb.hearttime));
    cJSON_AddItemToObject(data, "hbPkg", cJSON_CreateString(dtu_file_ctx->hb.heart));
    cJSON_AddItemToObject(data, "enReg", cJSON_CreateNumber(dtu_file_ctx->reg.linkflag));
    cJSON_AddItemToObject(data, "regpkg", cJSON_CreateString(dtu_file_ctx->reg.link));
    
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("getSysConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

/**
  * Function    : dtu_sys_json_set_sys_config
  * Description : 设置sys配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_sys_json_set_sys_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gdocg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_SYS_MSG_ID_SET_SYS_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gdocg =  cJSON_Parse(rcvdata);
    if(NULL != gdocg)
    {
        data = cJSON_GetObjectItem(gdocg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "enHb");
            dtu_file_ctx->hb.heartflag = item->valueint;
            item = cJSON_GetObjectItem(data, "cycHb");
            dtu_file_ctx->hb.hearttime =  item->valueint;
            item = cJSON_GetObjectItem(data, "hbPkg");
            memset(dtu_file_ctx->hb.heart, 0, sizeof(dtu_file_ctx->hb.heart));
            memcpy(dtu_file_ctx->hb.heart, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "enReg");
            dtu_file_ctx->reg.linkflag =  item->valueint;
            item = cJSON_GetObjectItem(data, "regpkg");
            memset(dtu_file_ctx->reg.link, 0, sizeof(dtu_file_ctx->reg.link));
            memcpy(dtu_file_ctx->reg.link, item->valuestring, strlen(item->valuestring));

            if(1 == dtu_file_ctx->hb.heartflag)
            {
                if(dtu_file_ctx->hb.hearttime >= 10)
                {
                    //开始心跳定时器
                    dtu_hb_timer_start();
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);
                    
                    //正常设置为0
                    res = 0;
                    
                    uprintf("enHb: %d, cycHb: %d, hbPkg: %s, enReg: %d, regpkg: %s\r\n", 
                            dtu_file_ctx->hb.heartflag,
                            dtu_file_ctx->hb.hearttime,
                            dtu_file_ctx->hb.heart,
                            dtu_file_ctx->reg.linkflag,
                            dtu_file_ctx->reg.link);
                }
            }
            else if(0 == dtu_file_ctx->hb.heartflag)
            {
                //关闭心跳定时器
                dtu_hb_timer_stop();
            }
        }
        //删除json对象
        cJSON_Delete(gdocg);
        gdocg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_sys_json_set_sys_config_res
  * Description : 回复服务器设置sys
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_sys_json_set_sys_config_res(UINT8 res)
{
    cJSON *gdocr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdocr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("setSysConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_net_json_get_net_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_net_json_get_net_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_NET_MSG_ID_GET_TCP_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_net_json_get_net_config_res
  * Description : 回复服务器获net配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_net_json_get_net_config_res(void)
{
    cJSON *gdocr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;

    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gdocr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "IP", cJSON_CreateString(dtu_file_ctx->socket.ip));
    cJSON_AddItemToObject(data, "port", cJSON_CreateNumber(dtu_file_ctx->socket.port));
    cJSON_AddItemToObject(data, "type", cJSON_CreateNumber(dtu_file_ctx->socket.type));
    
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("getNetConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

/**
  * Function    : dtu_net_json_set_net_config
  * Description : 设置网络配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_net_json_set_net_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gdocg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_NET_MSG_ID_SET_TCP_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gdocg =  cJSON_Parse(rcvdata);
    if(NULL != gdocg)
    {
        data = cJSON_GetObjectItem(gdocg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "IP");
            memset(dtu_file_ctx->socket.ip, 0, sizeof(dtu_file_ctx->socket.ip));
            memcpy(dtu_file_ctx->socket.ip, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "port");
            dtu_file_ctx->socket.port =  item->valueint;
            item = cJSON_GetObjectItem(data, "type");

            if(item->valueint >= 0 && item->valueint <= 1)
            {
                dtu_file_ctx->socket.type =  item->valueint;

                //写入文件
                dtu_trans_conf_file_write(dtu_file_ctx);

                //正常设置为0
                res = 0;
                
                uprintf("IP %s, port %d, type %d", 
                        dtu_file_ctx->socket.ip,
                        dtu_file_ctx->socket.port,
                        dtu_file_ctx->socket.type);
            }
        }
        //删除json对象
        cJSON_Delete(gdocg);
        gdocg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_net_json_set_net_config_res
  * Description : 回复服务器设置net
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_net_json_set_net_config_res(UINT8 res)
{
    cJSON *gdocr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdocr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("setNetConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:49:15 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
/**
  * Function    : dtu_net_json_get_mqtt_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_net_json_get_mqtt_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_NET_MSG_ID_GET_MQTT_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_net_json_get_mqtt_config_res
  * Description : 回复服务器获net配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_net_json_get_mqtt_config_res(void)
{
    cJSON *gdocr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;

    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gdocr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "IP", cJSON_CreateString(dtu_file_ctx->mqtt.ip));
    cJSON_AddItemToObject(data, "port", cJSON_CreateNumber(dtu_file_ctx->mqtt.port));
    cJSON_AddItemToObject(data, "clientId", cJSON_CreateString(dtu_file_ctx->mqtt.clientid));
    cJSON_AddItemToObject(data, "userName", cJSON_CreateString(dtu_file_ctx->mqtt.username));
    cJSON_AddItemToObject(data, "passWord", cJSON_CreateString(dtu_file_ctx->mqtt.password));
    cJSON_AddItemToObject(data, "keepLive", cJSON_CreateNumber(dtu_file_ctx->mqtt.keeplive));
    cJSON_AddItemToObject(data, "cleanSession", cJSON_CreateNumber(dtu_file_ctx->mqtt.cleansession));
    cJSON_AddItemToObject(data, "subTopic1", cJSON_CreateString(dtu_file_ctx->mqtt.subtopic));
    cJSON_AddItemToObject(data, "subTopic2", cJSON_CreateString(dtu_file_ctx->mqtt.subtopic1));
    cJSON_AddItemToObject(data, "subTopic3", cJSON_CreateString(dtu_file_ctx->mqtt.subtopic2));
    cJSON_AddItemToObject(data, "pubTopic", cJSON_CreateString(dtu_file_ctx->mqtt.pubtopic));
    cJSON_AddItemToObject(data, "subqos", cJSON_CreateNumber(dtu_file_ctx->mqtt.subqos));
    cJSON_AddItemToObject(data, "subflag", cJSON_CreateNumber(dtu_file_ctx->mqtt.subflag));
    cJSON_AddItemToObject(data, "pubqos", cJSON_CreateNumber(dtu_file_ctx->mqtt.pubqos));
    cJSON_AddItemToObject(data, "dupLicate", cJSON_CreateNumber(dtu_file_ctx->mqtt.duplicate));
    cJSON_AddItemToObject(data, "retain", cJSON_CreateNumber(dtu_file_ctx->mqtt.retain));
    
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("getMqttConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

/**
  * Function    : dtu_net_json_set_mqtt_config
  * Description : 设置网络配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_net_json_set_mqtt_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gdocg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_NET_MSG_ID_SET_MQTT_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gdocg =  cJSON_Parse(rcvdata);
    if(NULL != gdocg)
    {
        data = cJSON_GetObjectItem(gdocg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "IP");
            memset(dtu_file_ctx->mqtt.ip, 0, sizeof(dtu_file_ctx->mqtt.ip));
            memcpy(dtu_file_ctx->mqtt.ip, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "port");
            dtu_file_ctx->mqtt.port =  item->valueint;
            item = cJSON_GetObjectItem(data, "clientId");
            memset(dtu_file_ctx->mqtt.clientid, 0, sizeof(dtu_file_ctx->mqtt.clientid));
            memcpy(dtu_file_ctx->mqtt.clientid, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "userName");
            memset(dtu_file_ctx->mqtt.username, 0, sizeof(dtu_file_ctx->mqtt.username));
            memcpy(dtu_file_ctx->mqtt.username, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "passWord");
            memset(dtu_file_ctx->mqtt.password, 0, sizeof(dtu_file_ctx->mqtt.password));
            memcpy(dtu_file_ctx->mqtt.password, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "keepLive");
            dtu_file_ctx->mqtt.keeplive =  item->valueint;
            item = cJSON_GetObjectItem(data, "cleanSession");
            dtu_file_ctx->mqtt.cleansession =  item->valueint;
            item = cJSON_GetObjectItem(data, "subTopic1");
            memset(dtu_file_ctx->mqtt.subtopic, 0, sizeof(dtu_file_ctx->mqtt.subtopic));
            memcpy(dtu_file_ctx->mqtt.subtopic, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "subTopic2");
            memset(dtu_file_ctx->mqtt.subtopic1, 0, sizeof(dtu_file_ctx->mqtt.subtopic1));
            memcpy(dtu_file_ctx->mqtt.subtopic1, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "subTopic3");
            memset(dtu_file_ctx->mqtt.subtopic2, 0, sizeof(dtu_file_ctx->mqtt.subtopic2));
            memcpy(dtu_file_ctx->mqtt.subtopic2, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "pubTopic");
            memset(dtu_file_ctx->mqtt.pubtopic, 0, sizeof(dtu_file_ctx->mqtt.pubtopic));
            memcpy(dtu_file_ctx->mqtt.pubtopic, item->valuestring, strlen(item->valuestring));
            item = cJSON_GetObjectItem(data, "subqos");
            dtu_file_ctx->mqtt.subqos =  item->valueint;
            item = cJSON_GetObjectItem(data, "subflag");
            dtu_file_ctx->mqtt.subflag =  item->valueint;
            item = cJSON_GetObjectItem(data, "pubqos");
            dtu_file_ctx->mqtt.pubqos =  item->valueint;
            item = cJSON_GetObjectItem(data, "dupLicate");
            dtu_file_ctx->mqtt.duplicate =  item->valueint;
            item = cJSON_GetObjectItem(data, "retain");
            dtu_file_ctx->mqtt.retain =  item->valueint;

            //写入文件
            dtu_trans_conf_file_write(dtu_file_ctx);

            //正常设置为0
            res = 0;
            
            uprintf("IP %s, port %d, type %s", 
                    dtu_file_ctx->mqtt.ip,
                    dtu_file_ctx->mqtt.port,
                    dtu_file_ctx->mqtt.clientid);
        }
        //删除json对象
        cJSON_Delete(gdocg);
        gdocg = NULL;
    }
    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_net_json_set_mqtt_config_res
  * Description : 回复服务器设置net
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_net_json_set_mqtt_config_res(UINT8 res)
{
    cJSON *gdocr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdocr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("setMqttConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 15:14:28 by: zhaoning */

#ifdef DTU_TYPE_DODIAI_INCLUDE
/**
  * Function    : dtu_di_json_get_di_value
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_json_get_di_value(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_DI_MSG_ID_GET_VALUE_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_di_json_get_di_value_res
  * Description : 回复服务器获取di值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di_json_get_di_value_res(void)
{
    cJSON *gdivr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdivr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "DI1", cJSON_CreateNumber(dtu_di_read_pin(DTU_GPIO_DI_PIN1)));
    cJSON_AddItemToObject(data, "DI2", cJSON_CreateNumber(dtu_di_read_pin(DTU_GPIO_DI_PIN2)));
    cJSON_AddItemToObject(data, "DI3", cJSON_CreateNumber(dtu_di_read_pin(DTU_GPIO_DI_PIN3)));
    cJSON_AddItemToObject(data, "DI4", cJSON_CreateNumber(dtu_di_read_pin(DTU_GPIO_DI_PIN4)));
    cJSON_AddItemToObject(data, "DI5", cJSON_CreateNumber(dtu_di_read_pin(DTU_GPIO_DI_PIN5)));
    
    cJSON_AddItemToObject(gdivr, "msgType", cJSON_CreateString("getDiValueRes"));
    cJSON_AddItemToObject(gdivr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdivr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdivr);
    gdivr = NULL;
}

/**
  * Function    : dtu_di_json_get_di_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_json_get_di_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_DI_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_di_json_get_di_config_res
  * Description : 回复服务器获取di配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di_json_get_di_config_res(void)
{
    cJSON *gdicr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gdicr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enRpt1", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_1].type));
    cJSON_AddItemToObject(data, "cyc1", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_1].interval));
    cJSON_AddItemToObject(data, "edge1", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_1].edge));
    cJSON_AddItemToObject(data, "enRpt2", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_2].type));
    cJSON_AddItemToObject(data, "cyc2", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_2].interval));
    cJSON_AddItemToObject(data, "edge2", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_2].edge));
    cJSON_AddItemToObject(data, "enRpt3", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_3].type));
    cJSON_AddItemToObject(data, "cyc3", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_3].interval));
    cJSON_AddItemToObject(data, "edge3", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_3].edge));
    cJSON_AddItemToObject(data, "enRpt4", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_4].type));
    cJSON_AddItemToObject(data, "cyc4", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_4].interval));
    cJSON_AddItemToObject(data, "edge4", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_4].edge));
    cJSON_AddItemToObject(data, "enRpt5", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_5].type));
    cJSON_AddItemToObject(data, "cyc5", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_5].interval));
    cJSON_AddItemToObject(data, "edge5", cJSON_CreateNumber(dtu_file_ctx->di.params[DTU_BUF_INDEX_5].edge));
    
    cJSON_AddItemToObject(gdicr, "msgType", cJSON_CreateString("getDiConfigRes"));
    cJSON_AddItemToObject(gdicr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdicr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdicr);
    gdicr = NULL;
}

/**
  * Function    : dtu_di_json_set_di_config
  * Description : 设置di配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_di_json_set_di_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gdicg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 index = 0;//主动上报 or 触发上报
    UINT8 type = 0;//主动上报 or 触发上报
    UINT32 interval = 0;//主动上报间隔
    UINT8 edge = 0;//主动上报间隔
    
    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_DI_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gdicg =  cJSON_Parse(rcvdata);
    if(NULL != gdicg)
    {
        data = cJSON_GetObjectItem(gdicg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "index");
            if(NULL != item)
            {
                index = item->valueint;
                item = cJSON_GetObjectItem(data, "enRpt");
                type = item->valueint;
                item = cJSON_GetObjectItem(data, "cyc");
                interval = item->valueint;
                item = cJSON_GetObjectItem(data, "edge");
                edge = item->valueint;
                if(index <= DTU_DI_MAX_NUM  && index > 0)
                {
                    //判断下发数据是否符合参数范围
                    if(DTU_DI_REPORT_INITIATIVE == type)
                    {
                        dtu_file_ctx->di.params[index - 1].type = type;
                        if(interval > 4 && edge < 2)
                        {
                            dtu_file_ctx->di.params[index - 1].interval = interval;
                            dtu_file_ctx->di.params[index - 1].edge = edge;

                            //开定时器
                            dtu_di_report_timer_start(index, dtu_file_ctx->di.params[index - 1].interval);
                            //写入文件
                            dtu_trans_conf_file_write(dtu_file_ctx);
                            //正常设置为0
                            res = 0;
                        }
                    }
                    //触发上报和不上报，关闭定时器，保存type值
                    else
                    {
                        dtu_file_ctx->di.params[index - 1].type = type;
                        dtu_di_report_timer_stop(index);
                        //写入文件
                        dtu_trans_conf_file_write(dtu_file_ctx);
                        //正常设置为0
                        res = 0;
                    }
                }
            }

            uprintf("DIx:type %d, interval %d, edge %d\r\n",
                    index,
                    dtu_file_ctx->di.params[index - 1].type, 
                    dtu_file_ctx->di.params[index - 1].interval, 
                    dtu_file_ctx->di.params[index - 1].edge);
        }
        //删除json对象
        cJSON_Delete(gdicg);
        gdicg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_di_set_di_value_res
  * Description : 回复服务器设置di值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di_json_set_di_config_res(UINT8 res)
{
    cJSON *gdivr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdivr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gdivr, "msgType", cJSON_CreateString("setDiConfigRes"));
    cJSON_AddItemToObject(gdivr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gdivr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdivr);
    gdivr = NULL;
}

/**
  * Function    : dtu_di_json_res
  * Description : DI主动上报
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_di_json_res(UINT8 type, UINT8 channel, UINT8 level)
{
    cJSON *gdivr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
#ifdef DTU_BASED_ON_TCP
    DTU_SOCKET_PARAM_T* socket_ctx = NULL;

    //获取对应模块的上下文指针
    socket_ctx = dtu_get_socket_ctx();
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:58:17 by: zhaoning */

    //创建json对象
    gdivr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    if(DTU_GPIO_CHNNEL_1 == channel)
    {
        cJSON_AddItemToObject(data, "DI1", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_2 == channel)
    {
        cJSON_AddItemToObject(data, "DI2", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_3 == channel)
    {
        cJSON_AddItemToObject(data, "DI3", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_4 == channel)
    {
        cJSON_AddItemToObject(data, "DI4", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_5 == channel)
    {
        cJSON_AddItemToObject(data, "DI5", cJSON_CreateNumber(level));
    }

    if(DTU_DI_REPORT_INTER == type)
    {
        cJSON_AddItemToObject(gdivr, "msgType", cJSON_CreateString("diValueRpt"));
    }
    else if(DTU_DI_REPORT_TRIG == type)
    {
        cJSON_AddItemToObject(gdivr, "msgType", cJSON_CreateString("diValueRptTrig"));
    }
    cJSON_AddItemToObject(gdivr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdivr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdivr);
    gdivr = NULL;
}

/**
  * Function    : dtu_do_json_get_do_value
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_json_get_do_value(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

   //消息ID设置为对应ID
    p_smsg.msgId = DTU_DO_MSG_ID_GET_VALUE_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_do_json_get_do_value_res
  * Description : 回复服务器获取di值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do_json_get_do_value_res(void)
{
    cJSON *gdovr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdovr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "DO1", cJSON_CreateNumber(dtu_do_read_pin(DTU_GPIO_DO_PIN1)));
    cJSON_AddItemToObject(data, "DO2", cJSON_CreateNumber(dtu_do_read_pin(DTU_GPIO_DO_PIN2)));
    cJSON_AddItemToObject(data, "DO3", cJSON_CreateNumber(dtu_do_read_pin(DTU_GPIO_DO_PIN3)));
    cJSON_AddItemToObject(data, "DO4", cJSON_CreateNumber(dtu_do_read_pin(DTU_GPIO_DO_PIN4)));
    cJSON_AddItemToObject(data, "DO5", cJSON_CreateNumber(dtu_do_read_pin(DTU_GPIO_DO_PIN5)));
    
    cJSON_AddItemToObject(gdovr, "msgType", cJSON_CreateString("getDoValueRes"));
    cJSON_AddItemToObject(gdovr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdovr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdovr);
    gdovr = NULL;
}

/**
  * Function    : dtu_do_json_set_do_value
  * Description : 设置di配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_json_set_do_value(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gdov = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_DO_MSG_ID_SET_VALUE_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gdov =  cJSON_Parse(rcvdata);
    if(NULL != gdov)
    {
        data = cJSON_GetObjectItem(gdov, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "DO1");
            if(NULL != item)
            {
                if(item->valueint < 2)
                {
                    dtu_do_write_pin(DTU_GPIO_CHNNEL_1, item->valueint);
                    //正常设置为0
                    res = 0;
                    uprintf("DO1:%d", item->valueint);
                }
            }
            item = cJSON_GetObjectItem(data, "DO2");
            if(NULL != item)
            {
                if(item->valueint < 2)
                {
                    dtu_do_write_pin(DTU_GPIO_CHNNEL_2, item->valueint);
                    //正常设置为0
                    res = 0;
                    uprintf("DO2:%d", item->valueint);
                }
            }
            item = cJSON_GetObjectItem(data, "DO3");
            if(NULL != item)
            {
                if(item->valueint < 2)
                {
                    dtu_do_write_pin(DTU_GPIO_CHNNEL_3, item->valueint);
                    //正常设置为0
                    res = 0;
                    uprintf("DO3:%d", item->valueint);
                }
            }
            item = cJSON_GetObjectItem(data, "DO4");
            if(NULL != item)
            {
                if(item->valueint < 2)
                {
                    dtu_do_write_pin(DTU_GPIO_CHNNEL_4, item->valueint);
                    //正常设置为0
                    res = 0;
                    uprintf("DO4:%d", item->valueint);
                }
            }
            item = cJSON_GetObjectItem(data, "DO5");
            if(NULL != item)
            {
                if(item->valueint < 2)
                {
                    dtu_do_write_pin(DTU_GPIO_CHNNEL_5, item->valueint);
                    //正常设置为0
                    res = 0;
                    uprintf("DO5:%d", item->valueint);
                }
            }
        }
        //删除json对象
        cJSON_Delete(gdov);
        gdov = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_do_json_set_do_value_res
  * Description : 回复服务器设置di值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do_json_set_do_value_res(UINT8 res)
{
    cJSON *gdovr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdovr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gdovr, "msgType", cJSON_CreateString("setDoValueRes"));
    cJSON_AddItemToObject(gdovr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gdovr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdovr);
    gdovr = NULL;
}

/**
  * Function    : dtu_do_json_get_do_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_json_get_do_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_DO_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_do_json_get_do_config_res
  * Description : 回复服务器获取di配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do_json_get_do_config_res(void)
{
    cJSON *gdocr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gdocr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enRpt1", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_1].type));
    cJSON_AddItemToObject(data, "enRpt2", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_2].type));
    cJSON_AddItemToObject(data, "enRpt3", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_3].type));
    cJSON_AddItemToObject(data, "enRpt4", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_4].type));
    cJSON_AddItemToObject(data, "enRpt5", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_5].type));
    cJSON_AddItemToObject(data, "rptInterval1", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_1].interval));
    cJSON_AddItemToObject(data, "rptInterval2", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_2].interval));
    cJSON_AddItemToObject(data, "rptInterval3", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_3].interval));
    cJSON_AddItemToObject(data, "rptInterval4", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_4].interval));
    cJSON_AddItemToObject(data, "rptInterval5", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_5].interval));
    cJSON_AddItemToObject(data, "rstStatus1", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_1].status));
    cJSON_AddItemToObject(data, "rstStatus2", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_2].status));
    cJSON_AddItemToObject(data, "rstStatus3", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_3].status));
    cJSON_AddItemToObject(data, "rstStatus4", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_4].status));
    cJSON_AddItemToObject(data, "rstStatus5", cJSON_CreateNumber(dtu_file_ctx->doo.params[DTU_BUF_INDEX_5].status));
//    cJSON_AddItemToObject(data, "holdTime1", cJSON_CreateNumber(dtu_file_ctx->doo.do1.do_time));
//    cJSON_AddItemToObject(data, "holdTime2", cJSON_CreateNumber(dtu_file_ctx->doo.do2.do_time));
//    cJSON_AddItemToObject(data, "holdTime3", cJSON_CreateNumber(dtu_file_ctx->doo.do3.do_time));
//    cJSON_AddItemToObject(data, "holdTime4", cJSON_CreateNumber(dtu_file_ctx->doo.do4.do_time));
//    cJSON_AddItemToObject(data, "holdTime5", cJSON_CreateNumber(dtu_file_ctx->doo.do5.do_time));
//    cJSON_AddItemToObject(data, "turnTime1", cJSON_CreateNumber(dtu_file_ctx->doo.do1.do_flip));
//    cJSON_AddItemToObject(data, "turnTime2", cJSON_CreateNumber(dtu_file_ctx->doo.do2.do_flip));
//    cJSON_AddItemToObject(data, "turnTime3", cJSON_CreateNumber(dtu_file_ctx->doo.do3.do_flip));
//    cJSON_AddItemToObject(data, "turnTime4", cJSON_CreateNumber(dtu_file_ctx->doo.do4.do_flip));
//    cJSON_AddItemToObject(data, "turnTime5", cJSON_CreateNumber(dtu_file_ctx->doo.do5.do_flip));
    
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("getDoConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", data);
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

/**
  * Function    : dtu_do_json_set_do_config
  * Description : 设置do配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_do_json_set_do_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gdocg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 index = 0;
    UINT8 en = 0;
    UINT8 val = 0;
    UINT8 sta = 0;
    
    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_DO_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gdocg =  cJSON_Parse(rcvdata);
    if(NULL != gdocg)
    {
        data = cJSON_GetObjectItem(gdocg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "index");
            if(NULL != item)
            {
                index = item->valueint;
                item = cJSON_GetObjectItem(data, "enRpt");
                en = item->valueint;
                item = cJSON_GetObjectItem(data, "rptInterval");
                val = item->valueint;
                item = cJSON_GetObjectItem(data, "rstStatus");
                sta = item->valueint;
                if(index <= DTU_DO_MAX_NUM  && index > 0)
                {
                    if(DTU_DO_REPORT_INITIATIVE == en)
                    {
                        dtu_file_ctx->doo.params[index - 1].type = en;
                        if(val > 4 && sta < 3)
                        {
                            dtu_file_ctx->doo.params[index - 1].interval =  val;
                            dtu_file_ctx->doo.params[index - 1].status =  sta;

                            //开定时器
                            dtu_do_report_timer_start(index, dtu_file_ctx->doo.params[index - 1].interval);
                            //写入文件
                            dtu_trans_conf_file_write(dtu_file_ctx);
                            //正常设置为0
                            res = 0;
                        }
                        
                    }
                    else if(DTU_DO_REPORT_NONE == en)
                    {
                        dtu_file_ctx->doo.params[index - 1].type = en;
                        //关闭定时器
                        dtu_do_report_timer_stop(index);
                        //写入文件
                        dtu_trans_conf_file_write(dtu_file_ctx);
                        //正常设置为0
                        res = 0;
                    }
                }
            }

            uprintf("DO%d:type %d, interval %d, status %d\r\n",
                    index,
                    dtu_file_ctx->doo.params[index - 1].type,
                    dtu_file_ctx->doo.params[index - 1].interval,
                    dtu_file_ctx->doo.params[index - 1].status);
//                    dtu_file_ctx->doo.do1.do_time,
//                    dtu_file_ctx->doo.do1.do_flip);
        }
        //删除json对象
        cJSON_Delete(gdocg);
        gdocg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_do_json_set_do_config_res
  * Description : 回复服务器设置do
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do_json_set_do_config_res(UINT8 res)
{
    cJSON *gdocr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdocr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gdocr, "msgType", cJSON_CreateString("setDoConfigRes"));
    cJSON_AddItemToObject(gdocr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gdocr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdocr);
    gdocr = NULL;
}

/**
  * Function    : dtu_do_json_res
  * Description : DO主动上报
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_do_json_res(UINT8 channel, UINT8 level)
{
    cJSON *gdor = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gdor = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    if(DTU_GPIO_CHNNEL_1 == channel)
    {
        cJSON_AddItemToObject(data, "DO1", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_2 == channel)
    {
        cJSON_AddItemToObject(data, "DO2", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_3 == channel)
    {
        cJSON_AddItemToObject(data, "DO3", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_4 == channel)
    {
        cJSON_AddItemToObject(data, "DO4", cJSON_CreateNumber(level));
    }
    else if(DTU_GPIO_CHNNEL_5 == channel)
    {
        cJSON_AddItemToObject(data, "DO5", cJSON_CreateNumber(level));
    }

    cJSON_AddItemToObject(gdor, "msgType", cJSON_CreateString("doValueRpt"));
    cJSON_AddItemToObject(gdor, "data", data);
    
    //申请内存
    out = cJSON_Print(gdor);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gdor);
    gdor = NULL;
}

/**
  * Function    : dtu_ai_json_get_ai_value
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ai_json_get_ai_value(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_AI_MSG_ID_GET_VALUE_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_ai_json_get_ai_value_res
  * Description : 回复服务器获取di值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_ai_json_get_ai_value_res(void)
{
    cJSON *gaivr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    UINT16 ai_v = 0;
    float ai_i = 0.0;
    char ai_str[10] = {0};
    //创建json对象
    gaivr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    ai_v = dtu_ai_read();
    ai_i = (float)ai_v / DTU_AI_RES_VAL;
    snprintf(ai_str, 10, "%.02f", ai_i);
    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "AI1", cJSON_CreateString(ai_str));
    
    cJSON_AddItemToObject(gaivr, "msgType", cJSON_CreateString("getAiValueRes"));
    cJSON_AddItemToObject(gaivr, "data", data);
    
    //申请内存
    out = cJSON_Print(gaivr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gaivr);
    gaivr = NULL;
}

/**
  * Function    : dtu_ai_json_get_ai_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ai_json_get_ai_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_AI_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_ai_json_get_ai_config_res
  * Description : 回复服务器获取di配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_ai_json_get_ai_config_res(void)
{
    cJSON *gaicr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gaicr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enRpt1", cJSON_CreateNumber(dtu_file_ctx->ai.ai_type));
    cJSON_AddItemToObject(data, "cyc1", cJSON_CreateNumber(dtu_file_ctx->ai.ai_interval));
    cJSON_AddItemToObject(data, "rptRule1", cJSON_CreateNumber(dtu_file_ctx->ai.ai_res_rule));
    cJSON_AddItemToObject(data, "rptMin1", cJSON_CreateNumber(dtu_file_ctx->ai.ai_alarm_low));
    cJSON_AddItemToObject(data, "rptMax1", cJSON_CreateNumber(dtu_file_ctx->ai.ai_alarm_high));
    
    cJSON_AddItemToObject(gaicr, "msgType", cJSON_CreateString("getAiConfigRes"));
    cJSON_AddItemToObject(gaicr, "data", data);
    
    //申请内存
    out = cJSON_Print(gaicr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gaicr);
    gaicr = NULL;
}

/**
  * Function    : dtu_ai_json_set_ai_config
  * Description : 设置ai配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ai_json_set_ai_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gaicg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 en = 0;
    UINT8 cyc = 0;
    UINT8 rul = 0;
    UINT8 min = 0;
    UINT8 max = 0;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_AI_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gaicg =  cJSON_Parse(rcvdata);
    if(NULL != gaicg)
    {
        data = cJSON_GetObjectItem(gaicg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "enRpt1");
            en = item->valueint;
            item = cJSON_GetObjectItem(data, "cyc1");
            cyc = item->valueint;
            item = cJSON_GetObjectItem(data, "rptRule1");
            rul = item->valueint;
            item = cJSON_GetObjectItem(data, "rptMin1");
            min = item->valueint;
            item = cJSON_GetObjectItem(data, "rptMax1");
            max = item->valueint;

            //判断下发数据是否符合参数范围
            if(en < 2)
            {
                dtu_file_ctx->ai.ai_type = en;
                if(DTU_AI_REPORT_NONE != dtu_file_ctx->ai.ai_type)
                {
                    if(cyc >= 10 && rul < 3)
                    {
                        
                        dtu_file_ctx->ai.ai_interval = cyc;
                        dtu_file_ctx->ai.ai_res_rule = rul;
                        dtu_file_ctx->ai.ai_alarm_low = min;
                        dtu_file_ctx->ai.ai_alarm_high = max;

                        //开定时器
                        dtu_ai1_report_timer_start(dtu_file_ctx->ai.ai_interval);
                        //写入文件
                        dtu_trans_conf_file_write(dtu_file_ctx);

                        //正常设置为0
                        res = 0;
                    }
                }
                else
                {
                    dtu_ai1_report_timer_stop();
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }

                uprintf("AI1:type %d, interval %d, rule %d, min %d, max %d\r\n", 
                        dtu_file_ctx->ai.ai_type, 
                        dtu_file_ctx->ai.ai_interval, 
                        dtu_file_ctx->ai.ai_res_rule, 
                        dtu_file_ctx->ai.ai_alarm_low, 
                        dtu_file_ctx->ai.ai_alarm_high);
            }
        }
        //删除json对象
        cJSON_Delete(gaicg);
        gaicg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_ai_json_set_ai_config_res
  * Description : 回复服务器设置di值
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_ai_json_set_ai_config_res(UINT8 res)
{
    cJSON *gaivr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gaivr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gaivr, "msgType", cJSON_CreateString("setAiConfigRes"));
    cJSON_AddItemToObject(gaivr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gaivr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gaivr);
    gaivr = NULL;
}

/**
  * Function    : dtu_ai_do_flow
  * Description : do跟随ai
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_ai_do_flow(UINT8 index, UINT8 type)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();
    
    //IO跟随，阈值外高电平
    if(type == 1)
    {
        if(DTU_DO_FLOW_AI_XD_OPH == dtu_file_ctx->flow.do_flow[index])
        {
            dtu_do_write_pin(index, 1);
        }
        else if(DTU_DO_FLOW_AI_XD_OPL == dtu_file_ctx->flow.do_flow[index])
        {
            dtu_do_write_pin(index, 0);
        }
    }
    if(type == 0)
    {
        if(DTU_DO_FLOW_AI_DD_OPH == dtu_file_ctx->flow.do_flow[index])
        {
            dtu_do_write_pin(index, 1);
        }
        else if(DTU_DO_FLOW_AI_DD_OPL == dtu_file_ctx->flow.do_flow[index])
        {
            dtu_do_write_pin(index, 0);
        }
    }
}

/**
  * Function    : dtu_ai_json_res
  * Description : DI主动上报
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_ai_json_res(void)
{
    cJSON *gaivr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    UINT16 ai_v = 0;
    float ai_i = 0.0;
    char ai_str[10] = {0};
    
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gaivr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //获取adc值
    ai_v = dtu_ai_read();
    ai_i = (float)ai_v / DTU_AI_RES_VAL;
    snprintf(ai_str, 10, "%.02f", ai_i);

    //根据模式决定是否上报
    do 
    {
        //排除不用上报的情况
        if(DTU_AI_REPORT_TRIGGER == dtu_file_ctx->ai.ai_type)
        {
//            uprintf("AI Trigger rpt");
            //在阈值外上报，当电流位于阈值内，跳出
            if(DTU_AI_REPORT_OUT == dtu_file_ctx->ai.ai_res_rule)
            {
//                uprintf("AI rpt out");
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_1, 1);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_2, 1);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_3, 1);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_4, 1);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_5, 1);
                
                if(ai_i <= dtu_file_ctx->ai.ai_alarm_high && ai_i >= dtu_file_ctx->ai.ai_alarm_low)
                {
//                    uprintf("AI rpt out break");
                    break;
                }
            }
            //在阈值内上报，当电流位于阈值外，跳出
            else if(DTU_AI_REPORT_IN == dtu_file_ctx->ai.ai_res_rule)
            {
//                uprintf("AI rpt in");
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_1, 0);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_2, 0);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_3, 0);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_4, 0);
                dtu_ai_do_flow(DTU_GPIO_CHNNEL_5, 0);

                if(ai_i > dtu_file_ctx->ai.ai_alarm_high || ai_i < dtu_file_ctx->ai.ai_alarm_low)
                {
//                    uprintf("AI rpt in break");
                    break;
                }
            }
        }
        //按照协议，对象内加入成员
        cJSON_AddItemToObject(data, "AI1", cJSON_CreateString(ai_str));
        cJSON_AddItemToObject(gaivr, "msgType", cJSON_CreateString("aiValueRpt"));
        cJSON_AddItemToObject(gaivr, "data", data);

        //申请内存
        out = cJSON_Print(gaivr);
        if(NULL != out)
        {
            send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
            if(NULL != send)
            {
                uprintf("send:%s",send);

                //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
                dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
                dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
                cJSON_free(send);
                send = NULL;
            }
            cJSON_free(out);
            out = NULL;
        }
    }while(0);
    //删除json对象
    cJSON_Delete(gaivr);
    gaivr = NULL;
}

/**
  * Function    : dtu_flow_json_get_flow_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_flow_json_get_flow_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_FLOW_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_flow_json_get_flow_config_res
  * Description : 回复服务器获取flow配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_flow_json_get_flow_config_res(void)
{
    cJSON *gfcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gfcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "flow1", cJSON_CreateNumber(dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_1]));
    cJSON_AddItemToObject(data, "flow2", cJSON_CreateNumber(dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_2]));
    cJSON_AddItemToObject(data, "flow3", cJSON_CreateNumber(dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_3]));
    cJSON_AddItemToObject(data, "flow4", cJSON_CreateNumber(dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_4]));
    cJSON_AddItemToObject(data, "flow5", cJSON_CreateNumber(dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_5]));
    
    cJSON_AddItemToObject(gfcr, "msgType", cJSON_CreateString("getIOFlowConfigRes"));
    cJSON_AddItemToObject(gfcr, "data", data);
    
    //申请内存
    out = cJSON_Print(gfcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gfcr);
    gfcr = NULL;
}

/**
  * Function    : dtu_flow_json_set_flow_config
  * Description : 设置flow配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_flow_json_set_flow_config(int len , char *rcvdata)
{
    UINT8 res = 0;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * sfcg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 index = 0;
    UINT8 f = 0;
    
    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_FLOW_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    sfcg =  cJSON_Parse(rcvdata);
    if(NULL != sfcg)
    {
        data = cJSON_GetObjectItem(sfcg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "index");
            if(NULL != item)
            {
                if(index <= DTU_DO_MAX_NUM)
                {
                    if(f > 0 && f < 7)
                    {
                        dtu_file_ctx->flow.do_flow[index - 1] = f;
                        //写入文件
                        dtu_trans_conf_file_write(dtu_file_ctx);

                        //正常设置为0
                        res = 0;
                        uprintf("Flow%d,%d", index, dtu_file_ctx->flow.do_flow[index - 1]);
                    }
                }
            }
        }
        //删除json对象
        cJSON_Delete(sfcg);
        sfcg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_flow_json_set_flow_config_res
  * Description : 回复服务器设置flow
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_flow_json_set_flow_config_res(UINT8 res)
{
    cJSON *gfcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gfcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gfcr, "msgType", cJSON_CreateString("setIOFlowConfigRes"));
    cJSON_AddItemToObject(gfcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gfcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gfcr);
    gfcr = NULL;
}

/**
  * Function    : dtu_clk_json_clk_net_config
  * Description : 发送消息给主任务，按照协议回复相应内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk_json_get_clk_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_CLK_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_clk_json_get_clk_config_res
  * Description : 回复服务器获clk配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk_json_get_clk_config_res(void)
{
    cJSON *gclkcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gclkcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enClk1", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].sw));
    cJSON_AddItemToObject(data, "hour1", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].h));
    cJSON_AddItemToObject(data, "min1", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].m));
    cJSON_AddItemToObject(data, "sec1", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].s));
    cJSON_AddItemToObject(data, "channel1", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].num));
    cJSON_AddItemToObject(data, "level1", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].level));
    cJSON_AddItemToObject(data, "enClk2", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].sw));
    cJSON_AddItemToObject(data, "hour2", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].h));
    cJSON_AddItemToObject(data, "min2", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].m));
    cJSON_AddItemToObject(data, "sec2", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].s));
    cJSON_AddItemToObject(data, "channel2", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].num));
    cJSON_AddItemToObject(data, "level2", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].level));
    cJSON_AddItemToObject(data, "enClk3", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].sw));
    cJSON_AddItemToObject(data, "hour3", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].h));
    cJSON_AddItemToObject(data, "min3", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].m));
    cJSON_AddItemToObject(data, "sec3", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].s));
    cJSON_AddItemToObject(data, "channel3", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].num));
    cJSON_AddItemToObject(data, "level3", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].level));
    cJSON_AddItemToObject(data, "enClk4", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].sw));
    cJSON_AddItemToObject(data, "hour4", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].h));
    cJSON_AddItemToObject(data, "min4", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].m));
    cJSON_AddItemToObject(data, "sec4", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].s));
    cJSON_AddItemToObject(data, "channel4", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].num));
    cJSON_AddItemToObject(data, "level4", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].level));
    cJSON_AddItemToObject(data, "enClk5", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].sw));
    cJSON_AddItemToObject(data, "hour5", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].h));
    cJSON_AddItemToObject(data, "min5", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].m));
    cJSON_AddItemToObject(data, "sec5", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].s));
    cJSON_AddItemToObject(data, "channel5", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].num));
    cJSON_AddItemToObject(data, "level5", cJSON_CreateNumber(dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].level));
    
    cJSON_AddItemToObject(gclkcr, "msgType", cJSON_CreateString("getClockConfigRes"));
    cJSON_AddItemToObject(gclkcr, "data", data);
    
    //申请内存
    out = cJSON_Print(gclkcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gclkcr);
    gclkcr = NULL;
}

/**
  * Function    : dtu_clk_json_set_clk_config
  * Description : 设置网络配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_clk_json_set_clk_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gclkcg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 index = 0;
    UINT8 e = 0;
    UINT8 h = 0;
    UINT8 m = 0;
    UINT8 s = 0;
    UINT8 c = 0;
    UINT8 l = 0;
    
    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_CLK_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gclkcg =  cJSON_Parse(rcvdata);
    if(NULL != gclkcg)
    {
        data = cJSON_GetObjectItem(gclkcg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "index");
            if(NULL != item)
            {
                index =  item->valueint;
                item = cJSON_GetObjectItem(data, "enClk");
                e =  item->valueint;
                item = cJSON_GetObjectItem(data, "hour");
                h =  item->valueint;
                item = cJSON_GetObjectItem(data, "min");
                m =  item->valueint;
                item = cJSON_GetObjectItem(data, "sec");
                s =  item->valueint;
                item = cJSON_GetObjectItem(data, "channel");
                c =  item->valueint;
                item = cJSON_GetObjectItem(data, "level");
                l =  item->valueint;
                if(index <= DTU_CLK_INDEX_5)
                {
                    if(e == DTU_CLK_ON)
                    {
                        dtu_file_ctx->clk.params[index - 1].sw =  e;
                        if(h <= 24 && m <= 60 && s <= 60)
                        {
                            if(c < 6 && c > 0 && l < 2)
                            {
                                dtu_file_ctx->clk.params[index - 1].h =  h;
                                dtu_file_ctx->clk.params[index - 1].m =  m;
                                dtu_file_ctx->clk.params[index - 1].s =  s;
                                dtu_file_ctx->clk.params[index - 1].num =  c;
                                dtu_file_ctx->clk.params[index - 1].level =  l;
                                //闹钟开启
                                dtu_clk_timer_start(index);
                                //写入文件
                                dtu_trans_conf_file_write(dtu_file_ctx);

                                //正常设置为0
                                res = 0;
                            }
                        }
                    }
                    else if(e == DTU_CLK_OFF)
                    {
                        dtu_file_ctx->clk.params[index - 1].sw =  e;
                        //写入文件
                        dtu_trans_conf_file_write(dtu_file_ctx);

                        //正常设置为0
                        res = 0;
                        //闹钟结束
                        
                    }
                    uprintf("+CLK:%d,%d,%d,%d,%d\r\n" \
                                             "+CLK2:%d,%d,%d,%d,%d\r\n" \
                                             "+CLK3:%d,%d,%d,%d,%d\r\n" \
                                             "+CLK4:%d,%d,%d,%d,%d\r\n" \
                                             "+CLK5:%d,%d,%d,%d,%d",
//                                              dtu_file_ctx->clk.clk1.sw,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].h,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].m,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].s,
//                                              dtu_file_ctx->clk.clk1.type,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].num,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].level,
//                                              dtu_file_ctx->clk.clk2.sw,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].h,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].m,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].s,
//                                              dtu_file_ctx->clk.clk2.type,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].num,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].level,
//                                              dtu_file_ctx->clk.clk3.sw,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].h,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].m,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].s,
//                                              dtu_file_ctx->clk.clk3.type,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].num,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].level,
//                                              dtu_file_ctx->clk.clk4.sw,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].h,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].m,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].s,
//                                              dtu_file_ctx->clk.clk4.type,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].num,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].level,
//                                              dtu_file_ctx->clk.clk5.sw,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].h,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].m,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].s,
//                                              dtu_file_ctx->clk.clk5.type,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].num,
                                              dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].level);
                }
            }
        }
        //删除json对象
        cJSON_Delete(gclkcg;
        gclkcg = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_clk_json_set_clk_config_res
  * Description : 回复服务器设置clk
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_clk_json_set_clk_config_res(UINT8 res)
{
    cJSON *gclkcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gclkcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gclkcr, "msgType", cJSON_CreateString("setClockConfigRes"));
    cJSON_AddItemToObject(gclkcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gclkcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gclkcr);
    gclkcr = NULL;
}
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:36:55 by: zhaoning */

/**
  * Function    : dtu_fota_result_json_cbk
  * Description : json ota 回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_fota_result_json_cbk(UINT32 result)
{
//    uprintf("%s: result = %ld",__FUNCTION__, result);
    int res = 0;
    cJSON *gota = NULL;
    char* out = NULL;
    char* send = NULL;
        
    if(result != FOTA_SUCCESS)
    {
        res = 1;
        uprintf("Fota Error,Please try again!");
    }

    //创建json对象
    gota = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gota, "msgType", cJSON_CreateString("setOTAConfigRes"));
    cJSON_AddItemToObject(gota, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gota);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gota);
    gota = NULL;
    
    if(result == FOTA_SUCCESS)
    {
        uprintf("Down Bin Ok! Moudle Rebooting For Update...");
        dtu_sleep(1);
        PM812_SW_RESET();// restart will update
    }
}

/*****************************************************************
* Function: SDK_MYFOTA_UPDATE
*
* Description:
*     该函数用于请求下载并更新本地固件
* 
* Parameters:
*     url                  [In]     目标固件的URL。
*     username          [In]     http请求服务器用户名。
*     password          [In]     http请求服务器密码。
*     cb                  [In]     fotaRunCallback参数result描述。
*                             0: 文件下载校验成功
*                             1: 文件下载校验失败
*     resetTime         [In]     文件下载成功后，重启进入Fota升级的时间，单位秒；
*                             0: 文件下载成功后，不主动重启进入fota升级，任意时间点调用重启接口或者重新上电都会进入fota升级；
*                             >0: 文件下载成功，resetTime秒后重启进入fota升级。 
* Return:
*     NONE
*
*****************************************************************/
extern void SDK_MYFOTA_UPDATE(char *url, char *username, char *password, fotaRunCallback cb, UINT32 resetTime);

/**
  * Function    : dtu_ota_json_set_ota_config
  * Description : 设置ota参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_ota_json_set_ota_config(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * gclkcg = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_OTA_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    gclkcg =  cJSON_Parse(rcvdata);
    if(NULL != gclkcg)
    {
        data = cJSON_GetObjectItem(gclkcg, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "url");

            res = 0;
            uprintf("url:%s",item->valuestring);
            SDK_MYFOTA_UPDATE(item->valuestring, "", "", dtu_fota_result_json_cbk, 20);

        }
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

#ifdef DTU_TYPE_MODBUS_INCLUDE
/**
  * Function    : dtu_modbus_json_get_modbus_config
  * Description : 服务器下发获取modbus配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_get_modbus_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_get_modbus_config_res
  * Description : 回复服务器获取modbus配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_modbus_json_get_modbus_config_res(void)
{
    cJSON *gmbcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gmbcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enMb", cJSON_CreateNumber(dtu_file_ctx->modbus.config.type));
    cJSON_AddItemToObject(data, "wait", cJSON_CreateNumber(dtu_file_ctx->modbus.config.wait));
    cJSON_AddItemToObject(data, "interval", cJSON_CreateNumber(dtu_file_ctx->modbus.config.interval));
    cJSON_AddItemToObject(data, "delay", cJSON_CreateNumber(dtu_file_ctx->modbus.config.delay));
    
    cJSON_AddItemToObject(gmbcr, "msgType", cJSON_CreateString("getMbConfig"));
    cJSON_AddItemToObject(gmbcr, "data", data);
    
    //申请内存
    out = cJSON_Print(gmbcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gmbcr);
    gmbcr = NULL;
}

/**
  * Function    : dtu_modbus_json_set_modbus_config
  * Description : 服务器下发设置modbus配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_set_modbus_config(int len , char *rcvdata)
{
    UINT8 res = 0;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * smbcr = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 type = 0;
    UINT8 wait = 0;
    UINT8 interval = 0;
    UINT8 delay = 0;
    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    smbcr =  cJSON_Parse(rcvdata);
    if(NULL != smbcr)
    {
        data = cJSON_GetObjectItem(smbcr, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "enMb");
            type = item->valueint;
            item = cJSON_GetObjectItem(data, "wait");
            wait = item->valueint;
            item = cJSON_GetObjectItem(data, "interval");
            interval = item->valueint;
            item = cJSON_GetObjectItem(data, "delay");
            delay = item->valueint;

            if(DTU_MODBUS_TYPE_ENABLE == type)
            {
                dtu_file_ctx->modbus.config.type = type;

                //关闭心跳
                dtu_file_ctx->hb.heartflag = 0;
#ifdef DTU_TYPE_HTTP_INCLUDE
                //关闭http
                dtu_file_ctx->http.config.type = 0;
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-12 10:38:15 by: zhaoning */
                if(wait > 0 && wait <= 60 && interval > 0 && interval <= 60 && delay > 0 && delay <= 60)
                {
                    dtu_file_ctx->modbus.config.wait =  item->valueint;
                    dtu_file_ctx->modbus.config.interval =  item->valueint;
                    dtu_file_ctx->modbus.config.delay =  item->valueint;
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
            }
            else if(DTU_MODBUS_TYPE_DISENABLE == type)
            {
                dtu_file_ctx->modbus.config.type = type;
                //打开心跳
                dtu_file_ctx->hb.heartflag = 1;
                //写入文件
                dtu_trans_conf_file_write(dtu_file_ctx);

                //正常设置为0
                res = 0;
            }

            uprintf("enMb %d, wait %d, interval %d, delay %d", 
                    dtu_file_ctx->modbus.config.type,
                    dtu_file_ctx->modbus.config.wait,
                    dtu_file_ctx->modbus.config.interval,
                    dtu_file_ctx->modbus.config.delay);
        }
        //删除json对象
        cJSON_Delete(smbcr);
        smbcr = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_set_modbus_config_res
  * Description : 回复服务器设置modbus配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_modbus_json_set_modbus_config_res(UINT8 res)
{
    cJSON *smbcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    smbcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(smbcr, "msgType", cJSON_CreateString("setMbConfigRes"));
    cJSON_AddItemToObject(smbcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(smbcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(smbcr);
    smbcr = NULL;
}

/**
  * Function    : dtu_modbus_json_get_modbus_cmd
  * Description : 服务器获取modubs指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_get_modbus_cmd(int len , char *rcvdata)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_GET_CMD_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_get_modbus_cmd_res
  * Description : 回复服务器获取modubs指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_modbus_json_get_modbus_cmd_res(UINT8 id)
{
    cJSON *gmbcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gmbcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "active", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd[id].active));
    cJSON_AddItemToObject(data, "sAddr", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd[id].slave_addr));
    cJSON_AddItemToObject(data, "cmdType", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd[id].fn));
    cJSON_AddItemToObject(data, "regSaddr", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd[id].reg_addr));
    cJSON_AddItemToObject(data, "regNorD", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd[id].reg_n_d));
    
    cJSON_AddItemToObject(gmbcr, "msgType", cJSON_CreateString("getMdConfigRes"));
    cJSON_AddItemToObject(gmbcr, "data", data);
    
    //申请内存
    out = cJSON_Print(gmbcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gmbcr);
    gmbcr = NULL;
}

/**
  * Function    : dtu_modbus_json_get_modbus_cmdwn
  * Description : 服务器获取modubs指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_get_modbus_cmdwn(int len , char *rcvdata)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_GET_CMD_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_get_modbus_cmdwn_res
  * Description : 回复服务器获取modubs指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_modbus_json_get_modbus_cmdwn_res(UINT8 id)
{
    cJSON *gmbcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    gmbcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "active", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd_wn[id].active));
    cJSON_AddItemToObject(data, "sAddr", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd_wn[id].slave_addr));
    cJSON_AddItemToObject(data, "cmdType", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd_wn[id].fn));
    cJSON_AddItemToObject(data, "regSaddr", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd_wn[id].reg_addr));
    cJSON_AddItemToObject(data, "regN", cJSON_CreateNumber(dtu_file_ctx->modbus.cmd_wn[id].reg_n));
//    cJSON_AddItemToObject(data, "regData", cJSON_CreateString(dtu_file_ctx->modbus.cmd[id].reg_data));
    
    cJSON_AddItemToObject(gmbcr, "msgType", cJSON_CreateString("getMdCmdWnRes"));
    cJSON_AddItemToObject(gmbcr, "data", data);
    
    //申请内存
    out = cJSON_Print(gmbcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gmbcr);
    gmbcr = NULL;
}

/**
  * Function    : dtu_modbus_json_set_modbus_cmd
  * Description : 服务器下发设置modbus指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_set_modbus_cmd(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * smbc = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 index = 0;
    UINT8 active = 0;
    UINT8 saddr = 0;
    UINT8 cmdtype = 0;
    UINT16 regaddr = 0;
    UINT16 regnd = 0;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_SET_CMD_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    smbc =  cJSON_Parse(rcvdata);
    if(NULL != smbc)
    {
        data = cJSON_GetObjectItem(smbc, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "index");
            index = item->valueint;
            
            if(index <= DTU_MODBUS_CMD_NUM)
            {
                index--;
                item = cJSON_GetObjectItem(data, "active");
                active = item->valueint;
                if(DTU_MODBUS_ACTIVE == active)
                {
                    dtu_file_ctx->modbus.cmd[index].active = active;
                    item = cJSON_GetObjectItem(data, "sAddr");
                    dtu_file_ctx->modbus.cmd[index].slave_addr = item->valueint;
                    item = cJSON_GetObjectItem(data, "cmdType");
                    dtu_file_ctx->modbus.cmd[index].fn =  item->valueint;
                    item = cJSON_GetObjectItem(data, "regSaddr");
                    dtu_file_ctx->modbus.cmd[index].reg_addr = item->valueint;
                    item = cJSON_GetObjectItem(data, "regNorD");
                    dtu_file_ctx->modbus.cmd[index].reg_n_d = item->valueint;

                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                else if(DTU_MODBUS_INACTIVE == active)
                {
                    dtu_file_ctx->modbus.cmd[index].active = active;
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }

                uprintf("%d,0x%04X,0x%02X,0x%04X,%d", 
                            dtu_file_ctx->modbus.cmd[index].active,
                            dtu_file_ctx->modbus.cmd[index].slave_addr,
                            dtu_file_ctx->modbus.cmd[index].fn,
                            dtu_file_ctx->modbus.cmd[index].reg_addr,
                            dtu_file_ctx->modbus.cmd[index].reg_n_d);
            }
        }
        //删除json对象
        cJSON_Delete(smbc);
        smbc = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_set_modbus_cmd_res
  * Description : 回复服务器设置modbus指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_modbus_json_set_modbus_cmd_res(UINT8 res)
{
    cJSON *smbcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    smbcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(smbcr, "msgType", cJSON_CreateString("setMbCmdRes"));
    cJSON_AddItemToObject(smbcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(smbcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(smbcr);
    smbcr = NULL;
}

/**
  * Function    : dtu_modbus_json_set_modbus_cmdwn
  * Description : 服务器下发设置modbus指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_set_modbus_cmdwn(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * smbc = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 index = 0;
    UINT8 active = 0;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_SET_CMD_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    smbc =  cJSON_Parse(rcvdata);
    if(NULL != smbc)
    {
        data = cJSON_GetObjectItem(smbc, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "index");
            index = item->valueint;
            item = cJSON_GetObjectItem(data, "active");
            active = item->valueint;
            
            if(index <= DTU_MODBUS_CMD_WN_NUM)
            {
                index--;
                if(DTU_MODBUS_ACTIVE == active)
                {
                    dtu_file_ctx->modbus.cmd_wn[index].active = active;
                    item = cJSON_GetObjectItem(data, "sAddr");
                    dtu_file_ctx->modbus.cmd_wn[index].slave_addr = item->valueint;
                    item = cJSON_GetObjectItem(data, "cmdType");
                    dtu_file_ctx->modbus.cmd_wn[index].fn =  item->valueint;
                    item = cJSON_GetObjectItem(data, "regSaddr");
                    dtu_file_ctx->modbus.cmd_wn[index].reg_addr = item->valueint;
                    item = cJSON_GetObjectItem(data, "regNorD");
                    dtu_file_ctx->modbus.cmd_wn[index].reg_n = item->valueint;
                    item = cJSON_GetObjectItem(data, "regData");
                    memset(dtu_file_ctx->modbus.cmd_wn[index].reg_data, 0, sizeof(dtu_file_ctx->modbus.cmd_wn[index].reg_data));
                    memcpy(dtu_file_ctx->modbus.cmd_wn[index].reg_data, item->valuestring, strlen(item->valuestring));
                
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                else if(DTU_MODBUS_INACTIVE == active)
                {
                    dtu_file_ctx->modbus.cmd_wn[index].active = active;
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                uprintf("%d,0x%04X,0x%02X,0x%04X,%d", 
                                                dtu_file_ctx->modbus.cmd_wn[index].active,
                                                dtu_file_ctx->modbus.cmd_wn[index].slave_addr,
                                                dtu_file_ctx->modbus.cmd_wn[index].fn,
                                                dtu_file_ctx->modbus.cmd_wn[index].reg_addr,
                                                dtu_file_ctx->modbus.cmd_wn[index].reg_n);
            }
        }
        //删除json对象
        cJSON_Delete(smbc);
        smbc = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_set_modbus_cmdwn_res
  * Description : 回复服务器设置modbus指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_modbus_json_set_modbus_cmdwn_res(UINT8 res)
{
    cJSON *gmbcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    gmbcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(gmbcr, "msgType", cJSON_CreateString("setMbCmdWnRes"));
    cJSON_AddItemToObject(gmbcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(gmbcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(gmbcr);
    gmbcr = NULL;
}

/**
  * Function    : dtu_modbus_json_modbus_res
  * Description : modbus模式下，配置为modbus上报后，调用此函数把hex转json
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_modbus_res(UINT8* hex, UINT32 len)
{
    cJSON *s01hr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    char* asciistr = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    s01hr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(s01hr, "msgType", cJSON_CreateString("rs485Rpt"));
    if(len > DTU_MODBUS_RES_LEN / 2)
    {
        len = DTU_MODBUS_RES_LEN / 2;
    }
    //申请内存
    asciistr = malloc(len * 2 + 1);
    if(NULL != asciistr)
    {
        memset(asciistr, 0, len * 2 + 1);
        utils_hex2ascii_str(asciistr, hex, len);
        cJSON_AddItemToObject(s01hr, "data", cJSON_CreateString(asciistr));

        //申请内存
        out = cJSON_Print(s01hr);
        if(NULL != out)
        {
            send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
            if(NULL != send)
            {
                uprintf("send:%s",send);

                //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
                dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
                dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
                cJSON_free(send);
                send = NULL;
            }
            cJSON_free(out);
            out = NULL;
        }
    }
    //删除json对象
    cJSON_Delete(s01hr);
    s01hr = NULL;
}

/**
  * Function    : dtu_modbus_json_set_modbus_data
  * Description : 服务器下发一个完整modbus指令，或者485指令，下发有效一次
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_modbus_json_set_modbus_data(int len , char *rcvdata)
{
//    UINT8 res = 0;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * smbd = NULL;
    cJSON * data = NULL;
    char * hex = NULL;
    UINT8 id = 0;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_MODBUS_MSG_ID_SET_CMD_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    smbd =  cJSON_Parse(rcvdata);
    if(NULL != smbd)
    {
        data = cJSON_GetObjectItem(smbd, "data");
        if(NULL != data)
        {
            //申请内存
            hex = malloc(strlen(data->valuestring) / 2 );
            memset(hex, 0, strlen(data->valuestring) / 2);
            //转换为hex
            utils_ascii_str2hex(0, hex, data->valuestring, strlen(data->valuestring));
            //发送数据到串口
            dtu_send_to_uart(hex, strlen(data->valuestring) / 2);
        }
        //删除json对象
        cJSON_Delete(smbd);
        smbd = NULL;
    }
}
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:36:47 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
/**
  * Function    : dtu_http_json_get_http_config
  * Description : 服务器下发获取http配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_http_json_get_http_config(void)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_HTTP_MSG_ID_GET_CONFIG_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_http_json_get_http_config_res
  * Description : 回复服务器获取http配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_json_get_http_config_res(void)
{
    cJSON *ghtpcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    ghtpcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "enHttp", cJSON_CreateNumber(dtu_file_ctx->http.config.type));
    
    cJSON_AddItemToObject(ghtpcr, "msgType", cJSON_CreateString("getHttpConfig"));
    cJSON_AddItemToObject(ghtpcr, "data", data);
    
    //申请内存
    out = cJSON_Print(ghtpcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(ghtpcr);
    ghtpcr = NULL;
}

/**
  * Function    : dtu_http_json_set_http_config
  * Description : 服务器下发设置http配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_http_json_set_http_config(int len , char *rcvdata)
{
    UINT8 res = 0;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * shtpc = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 type = 0;
    
    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_HTTP_MSG_ID_SET_CONFIG_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    shtpc =  cJSON_Parse(rcvdata);
    if(NULL != shtpc)
    {
        data = cJSON_GetObjectItem(shtpc, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "enHttp");
            type = item->valueint;
            if(DTU_HTTP_TYPE_ENABLE == type)
            {
                dtu_file_ctx->http.config.type =  type;
                //关闭心跳
                dtu_file_ctx->hb.heartflag = 0;
#ifdef DTU_TYPE_MODBUS_INCLUDE
                //关闭modbus
                dtu_file_ctx->modbus.config.type = 0;
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-12 10:38:34 by: zhaoning */

                //写入文件
                dtu_trans_conf_file_write(dtu_file_ctx);

                //正常设置为0
                res = 0;
            }
            else if(DTU_HTTP_TYPE_DISENABLE == type)
            {
                dtu_file_ctx->http.config.type =  type;
                //打开心跳
                dtu_file_ctx->hb.heartflag = 1;
                //写入文件
                dtu_trans_conf_file_write(dtu_file_ctx);

                //正常设置为0
                res = 0;
            }
            uprintf("enHttp %d", dtu_file_ctx->http.config.type);
        }
        //删除json对象
        cJSON_Delete(shtpc);
        shtpc = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_http_json_set_http_config_res
  * Description : 回复服务器设置http配置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_json_set_http_config_res(UINT8 res)
{
    cJSON *shtpcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    shtpcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(shtpcr, "msgType", cJSON_CreateString("setHttpConfigRes"));
    cJSON_AddItemToObject(shtpcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(shtpcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(shtpcr);
    shtpcr = NULL;
}

/**
  * Function    : dtu_http_json_get_http_cmd
  * Description : 服务器下发获取http指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_http_json_get_http_cmd(int len , char *rcvdata)
{
    DTU_JSON_MSG_T p_smsg = {0};

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_HTTP_MSG_ID_GET_CMD_REPORT;

//    uprintf("%s PIN4 status: %d \n", __FUNCTION__, p_smsg.status);
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_modbus_json_get_modbus_cmd_res
  * Description : 回复服务器获取http指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_json_get_http_cmd_res(void)
{
    cJSON *ghtpcr = NULL;
    cJSON *data = NULL;
    char* out = NULL;
    char* send = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //创建json对象
    ghtpcr = cJSON_CreateObject();
    data = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(data, "type1", cJSON_CreateNumber(dtu_file_ctx->http.http1.type));
    if(strlen(dtu_file_ctx->http.http1.head1))
    {
        cJSON_AddItemToObject(data, "head11", cJSON_CreateString(dtu_file_ctx->http.http1.head1));
    }
    if(strlen(dtu_file_ctx->http.http1.head2))
    {
        cJSON_AddItemToObject(data, "head12", cJSON_CreateString(dtu_file_ctx->http.http1.head2));
    }
    if(strlen(dtu_file_ctx->http.http1.head3))
    {
        cJSON_AddItemToObject(data, "head13", cJSON_CreateString(dtu_file_ctx->http.http1.head3));
    }
    cJSON_AddItemToObject(data, "type2", cJSON_CreateNumber(dtu_file_ctx->http.http2.type));
    if(strlen(dtu_file_ctx->http.http2.head1))
    {
        cJSON_AddItemToObject(data, "head21", cJSON_CreateString(dtu_file_ctx->http.http2.head1));
    }
    if(strlen(dtu_file_ctx->http.http2.head2))
    {
        cJSON_AddItemToObject(data, "head22", cJSON_CreateString(dtu_file_ctx->http.http2.head2));
    }
    if(strlen(dtu_file_ctx->http.http2.head3))
    {
        cJSON_AddItemToObject(data, "head23", cJSON_CreateString(dtu_file_ctx->http.http2.head3));
    }
    cJSON_AddItemToObject(ghtpcr, "msgType", cJSON_CreateString("getHttpConfigRes"));
    cJSON_AddItemToObject(ghtpcr, "data", data);
    
    //申请内存
    out = cJSON_Print(ghtpcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(ghtpcr);
    ghtpcr = NULL;
}

/**
  * Function    : dtu_http_json_set_http_cmd
  * Description : 服务器下发设置http指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_http_json_set_http_cmd(int len , char *rcvdata)
{
    UINT8 res = 1;
    DTU_JSON_MSG_T p_smsg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    cJSON * shtpc = NULL;
    cJSON * data = NULL;
    cJSON * item = NULL;
    UINT8 type1 = 0;
    UINT8 type2 = 0;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    //消息ID设置为对应ID
    p_smsg.msgId = DTU_HTTP_MSG_ID_SET_CMD_REPORT;

//    uprintf("%s %s\n", __FUNCTION__, rcvdata);

    //解析数据
    shtpc =  cJSON_Parse(rcvdata);
    if(NULL != shtpc)
    {
        data = cJSON_GetObjectItem(shtpc, "data");
        if(NULL != data)
        {
            item = cJSON_GetObjectItem(data, "type1");
            if(NULL != item)
            {
                type1 = item->valueint;
                if(type1 != DTU_HTTP1_TYPE_DISENABLE)
                {
                    dtu_file_ctx->http.http1.type = type1;
                    item = cJSON_GetObjectItem(data, "head11");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http1.head1, 0, sizeof(dtu_file_ctx->http.http1.head1));
                        memcpy(dtu_file_ctx->http.http1.head1, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http1.head1, 0, sizeof(dtu_file_ctx->http.http1.head1));
                    }
                    item = cJSON_GetObjectItem(data, "head12");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http1.head2, 0, sizeof(dtu_file_ctx->http.http1.head2));
                        memcpy(dtu_file_ctx->http.http1.head2, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http1.head2, 0, sizeof(dtu_file_ctx->http.http1.head2));
                    }
                    item = cJSON_GetObjectItem(data, "head13");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http1.head3, 0, sizeof(dtu_file_ctx->http.http1.head3));
                        memcpy(dtu_file_ctx->http.http1.head3, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http1.head3, 0, sizeof(dtu_file_ctx->http.http1.head3));
                    }
                    item = cJSON_GetObjectItem(data, "url1");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http1.url, 0, sizeof(dtu_file_ctx->http.http1.url));
                        memcpy(dtu_file_ctx->http.http1.url, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http1.url, 0, sizeof(dtu_file_ctx->http.http1.url));
                    }
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                else
                {
                    dtu_file_ctx->http.http1.type = type1;
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                uprintf("type1: %d, head11: %s, head12: %s, head13: %s\r\nurl1: %s\r\n", 
                                              dtu_file_ctx->http.http1.type,
                                              dtu_file_ctx->http.http1.head1,
                                              dtu_file_ctx->http.http1.head2,
                                              dtu_file_ctx->http.http1.head3,
                                              dtu_file_ctx->http.http1.url);
            }

            item = cJSON_GetObjectItem(data, "type2");
            if(NULL != item)
            {
                type2 = item->valueint;
                if(type2 != DTU_HTTP1_TYPE_DISENABLE)
                {
                    dtu_file_ctx->http.http2.type = type2;
                    item = cJSON_GetObjectItem(data, "head21");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http2.head1, 0, sizeof(dtu_file_ctx->http.http2.head1));
                        memcpy(dtu_file_ctx->http.http2.head1, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http2.head1, 0, sizeof(dtu_file_ctx->http.http2.head1));
                    }
                    item = cJSON_GetObjectItem(data, "head22");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http2.head2, 0, sizeof(dtu_file_ctx->http.http2.head2));
                        memcpy(dtu_file_ctx->http.http2.head2, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http2.head2, 0, sizeof(dtu_file_ctx->http.http2.head2));
                    }
                    item = cJSON_GetObjectItem(data, "head23");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http2.head3, 0, sizeof(dtu_file_ctx->http.http2.head3));
                        memcpy(dtu_file_ctx->http.http2.head3, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http2.head3, 0, sizeof(dtu_file_ctx->http.http2.head3));
                    }
                    item = cJSON_GetObjectItem(data, "url2");
                    if(NULL != item)
                    {
                        memset(dtu_file_ctx->http.http2.url, 0, sizeof(dtu_file_ctx->http.http2.url));
                        memcpy(dtu_file_ctx->http.http2.url, item->valuestring, strlen(item->valuestring));
                    }
                    else
                    {
                        memset(dtu_file_ctx->http.http2.url, 0, sizeof(dtu_file_ctx->http.http2.url));
                    }
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                else
                {
                    dtu_file_ctx->http.http2.type = type2;
                    //写入文件
                    dtu_trans_conf_file_write(dtu_file_ctx);

                    //正常设置为0
                    res = 0;
                }
                uprintf("type2: %d, head21: %s, head22: %s, head23: %s\r\nurl2: %s\r\n", 
                                              dtu_file_ctx->http.http2.type,
                                              dtu_file_ctx->http.http2.head1,
                                              dtu_file_ctx->http.http2.head2,
                                              dtu_file_ctx->http.http2.head3,
                                              dtu_file_ctx->http.http2.url);
            }
        }
        //删除json对象
        cJSON_Delete(shtpc);
        shtpc = NULL;
    }

    p_smsg.status = res;
    
    dtu_json_task_send_msgq(&p_smsg);
}

/**
  * Function    : dtu_http_json_set_http_cmd_res
  * Description : 回复服务器设置http指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_http_json_set_http_cmd_res(UINT8 res)
{
    cJSON *shtpcr = NULL;
    char* out = NULL;
    char* send = NULL;

    //创建json对象
    shtpcr = cJSON_CreateObject();

    //按照协议，对象内加入成员
    cJSON_AddItemToObject(shtpcr, "msgType", cJSON_CreateString("setHttpCmdRes"));
    cJSON_AddItemToObject(shtpcr, "data", cJSON_CreateNumber(res));
    
    //申请内存
    out = cJSON_Print(shtpcr);
    if(NULL != out)
    {
        send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
        if(NULL != send)
        {
            uprintf("send:%s",send);

            //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
            dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
            cJSON_free(send);
            send = NULL;
        }
        cJSON_free(out);
        out = NULL;
    }
    //删除json对象
    cJSON_Delete(shtpcr);
    shtpcr = NULL;
}

#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:38:18 by: zhaoning */

/**
  * Function    : dtu_json_data_prase
  * Description : 服务器下行json消息分发
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_json_data_prase(int len , char *rcvdata)
{
    //SYS getSysConfig
    if(NULL != strstr(rcvdata, "getSysConfig"))
    {
        dtu_sys_json_get_sys_config();
    }
    //SYS setSysConfig
    else if(NULL != strstr(rcvdata, "setSysConfig"))
    {
        dtu_sys_json_set_sys_config(len, rcvdata);
    }
#ifdef DTU_BASED_ON_TCP
    //Net getMqttConfig
    else if(NULL != strstr(rcvdata, "getNetConfig"))
    {
        dtu_net_json_get_net_config();
    }
    //Net setMqttConfig
    else if(NULL != strstr(rcvdata, "setNetConfig"))
    {
        dtu_net_json_set_net_config(len, rcvdata);
    }
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-30 15:17:45 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    //Net getNetConfig
    else if(NULL != strstr(rcvdata, "getMqttConfig"))
    {
        dtu_net_json_get_mqtt_config();
    }
    //Net setNetConfig
    else if(NULL != strstr(rcvdata, "setMqttConfig"))
    {
        dtu_net_json_set_mqtt_config(len, rcvdata);
    }
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-31 11:11:17 by: zhaoning */
#ifdef DTU_TYPE_DODIAI_INCLUDE
    //DI getDiValue
    else if(NULL != strstr(rcvdata, "getDiValue"))
    {
        dtu_di_json_get_di_value();
    }
    //DI getDiConfig
    else if(NULL != strstr(rcvdata, "getDiConfig"))
    {
        dtu_di_json_get_di_config();
    }
    //DI setDiConfig
    else if(NULL != strstr(rcvdata, "setDiConfig"))
    {
        dtu_di_json_set_di_config(len, rcvdata);
    }
    //DO getDoValue
    else if(NULL != strstr(rcvdata, "getDoValue"))
    {
        dtu_do_json_get_do_value();
    }
    //DO setDoValue
    else if(NULL != strstr(rcvdata, "setDoValue"))
    {
        dtu_do_json_set_do_value(len, rcvdata);
    }
    //DO getDoConfig
    else if(NULL != strstr(rcvdata, "getDoConfig"))
    {
        dtu_do_json_get_do_config();
    }
    //DO setDoConfig
    else if(NULL != strstr(rcvdata, "setDoConfig"))
    {
        dtu_do_json_set_do_config(len, rcvdata);
    }
    //AI getAiValue
    else if(NULL != strstr(rcvdata, "getAiValue"))
    {
        dtu_ai_json_get_ai_value();
    }
    //AI getAiConfig
    else if(NULL != strstr(rcvdata, "getAiConfig"))
    {
        dtu_ai_json_get_ai_config();
    }
    //AI setAiConfig
    else if(NULL != strstr(rcvdata, "setAiConfig"))
    {
        dtu_ai_json_set_ai_config(len, rcvdata);
    }
    //FLOW getIOFlowConfig
    else if(NULL != strstr(rcvdata, "getIOFlowConfig"))
    {
        dtu_flow_json_get_flow_config();
    }
    //FLOW getIOFlowConfigRes
    else if(NULL != strstr(rcvdata, "getIOFlowConfigRes"))
    {
        dtu_flow_json_set_flow_config(len, rcvdata);
    }
    //FLOW setClkConfig
    else if(NULL != strstr(rcvdata, "setClkConfig"))
    {
        dtu_clk_json_get_clk_config();
    }
    //FLOW setClkConfigRes
    else if(NULL != strstr(rcvdata, "setClkConfigRes"))
    {
        dtu_clk_json_set_clk_config(len, rcvdata);
    }
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 12:04:46 by: zhaoning */
    //OTA setIOFlowConfigRes
    else if(NULL != strstr(rcvdata, "setOTAConfig"))
    {
        dtu_ota_json_set_ota_config(len, rcvdata);
    }
#ifdef DTU_TYPE_MODBUS_INCLUDE
    //MODBUS getMbConfig
    else if(NULL != strstr(rcvdata, "getMbConfig"))
    {
        dtu_modbus_json_get_modbus_config();
    }
    //MODBUS setMbConfig
    else if(NULL != strstr(rcvdata, "setMbConfig"))
    {
        dtu_modbus_json_set_modbus_config(len, rcvdata);
    }
    //MODBUS getMbCmd
    else if(NULL != strstr(rcvdata, "getMbCmd"))
    {
        dtu_modbus_json_get_modbus_cmd(len, rcvdata);
    }
    //MODBUS setMbCmd
    else if(NULL != strstr(rcvdata, "setMbCmd"))
    {
        dtu_modbus_json_set_modbus_cmd(len, rcvdata);
    }
    //MODBUS setRs485data
    else if(NULL != strstr(rcvdata, "setRs485data"))
    {
        dtu_modbus_json_set_modbus_data(len, rcvdata);
    }
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:39:21 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
    //HTTP getHttpConfig
    else if(NULL != strstr(rcvdata, "getHttpConfig"))
    {
        dtu_http_json_get_http_config();
    }
    //MODBUS setHttpConfig
    else if(NULL != strstr(rcvdata, "setHttpConfig"))
    {
        dtu_http_json_set_http_config(len, rcvdata);
    }
    //MODBUS getHttpCmd
    else if(NULL != strstr(rcvdata, "getHttpCmd"))
    {
        dtu_http_json_get_http_cmd(len, rcvdata);
    }
    //MODBUS setHttpCmd
    else if(NULL != strstr(rcvdata, "setHttpCmd"))
    {
        dtu_http_json_set_http_cmd(len, rcvdata);
    }

#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:39:35 by: zhaoning */
}

/**
  * Function    : dtu_json_task
  * Description : json主任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_json_task(void *param)
{
    OS_STATUS  status;
    DTU_JSON_MSG_T json_rcv_msg = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    //获取对应模块的上下文指针
    dtu_file_ctx = dtu_get_file_ctx();

    while(1)
    {
        status = OSAMsgQRecv(dtu_json_msgq, (void *)&json_rcv_msg, DTU_JSON_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            uprintf("%s: json_rcv_msg id: %d\n", __FUNCTION__, json_rcv_msg.msgId);
            uprintf("%s: json_rcv_msg status: %d\n", __FUNCTION__, json_rcv_msg.status);
            uprintf("%s: json_rcv_msg channel: %d\n", __FUNCTION__, json_rcv_msg.channel);
            uprintf("%s: json_rcv_msg result: %d\n", __FUNCTION__, json_rcv_msg.result);
            uprintf("%s: json_rcv_msg current: %d\n", __FUNCTION__, json_rcv_msg.current);
//            uprintf("%s: json_rcv_msg value: %d\n", __FUNCTION__, json_rcv_msg.us_value);
            
            switch(json_rcv_msg.msgId)
            {
                case DTU_SYS_MSG_ID_GET_SYS_CONFIG_REPORT:
                    dtu_sys_json_get_sys_config_res();
                    break;
                case DTU_SYS_MSG_ID_SET_SYS_CONFIG_REPORT:
                    dtu_sys_json_set_sys_config_res(json_rcv_msg.status);
                    break;
                
#ifdef DTU_BASED_ON_TCP
                case DTU_NET_MSG_ID_GET_TCP_CONFIG_REPORT:
                    dtu_net_json_get_net_config_res();
                    break;
                case DTU_NET_MSG_ID_SET_TCP_CONFIG_REPORT:
                    dtu_net_json_set_net_config_res(json_rcv_msg.status);
                    break;
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 18:14:38 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
                case DTU_NET_MSG_ID_GET_MQTT_CONFIG_REPORT:
                    dtu_net_json_get_mqtt_config_res();
                    break;
                case DTU_NET_MSG_ID_SET_MQTT_CONFIG_REPORT:
                    dtu_net_json_set_mqtt_config_res(json_rcv_msg.status);
                    break;
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-31 11:17:25 by: zhaoning */

#ifdef DTU_TYPE_DODIAI_INCLUDE
                case DTU_DI_MSG_ID_DI_PROACTIVE_REPORT:
                    dtu_di_json_res(DTU_DI_REPORT_INTER, json_rcv_msg.channel, json_rcv_msg.status);
                    break;
                case DTU_DI_MSG_ID_DI_TRIGGER_REPORT:
                    dtu_di_json_res(DTU_DI_REPORT_TRIG, json_rcv_msg.channel, json_rcv_msg.status);
                    break;
                case DTU_DI_MSG_ID_GET_VALUE_REPORT:
                    dtu_di_json_get_di_value_res();
                    break;
                case DTU_DI_MSG_ID_GET_CONFIG_REPORT:
                    dtu_di_json_get_di_config_res();
                    break;
                case DTU_DI_MSG_ID_SET_CONFIG_REPORT:
                    dtu_di_json_set_di_config_res(json_rcv_msg.status);
                    break;

                case DTU_DO_MSG_ID_DO_PROACTIVE_REPORT:
                    dtu_do_json_res(json_rcv_msg.channel, json_rcv_msg.status);
                    break;
                case DTU_DO_MSG_ID_GET_VALUE_REPORT:
                    dtu_do_json_get_do_value_res();
                    break;
                case DTU_DO_MSG_ID_SET_VALUE_REPORT:
                    dtu_do_json_set_do_value_res(json_rcv_msg.status);
                    break;
                case DTU_DO_MSG_ID_GET_CONFIG_REPORT:
                    dtu_do_json_get_do_config_res();
                    break;
                case DTU_DO_MSG_ID_SET_CONFIG_REPORT:
                    dtu_do_json_set_do_config_res(json_rcv_msg.status);
                    break;
                case DTU_AI_MSG_ID_AI_PROACTIVE_REPORT:
                    dtu_ai_json_res();
                    break;
                case DTU_AI_MSG_ID_GET_VALUE_REPORT:
                    dtu_ai_json_get_ai_value_res();
                    break;
                case DTU_AI_MSG_ID_GET_CONFIG_REPORT:
                    dtu_ai_json_get_ai_config_res();
                    break;
                case DTU_AI_MSG_ID_SET_CONFIG_REPORT:
                    dtu_ai_json_set_ai_config_res(json_rcv_msg.result);
                    break;

                case DTU_FLOW_MSG_ID_GET_CONFIG_REPORT:
                    dtu_flow_json_get_flow_config_res();
                    break;
                case DTU_FLOW_MSG_ID_SET_CONFIG_REPORT:
                    dtu_flow_json_set_flow_config_res(json_rcv_msg.status);
                    break;
                
                case DTU_CLK_MSG_ID_GET_CONFIG_REPORT:
                    dtu_clk_json_get_clk_config_res();
                    break;
                case DTU_CLK_MSG_ID_SET_CONFIG_REPORT:
                    dtu_clk_json_set_clk_config_res(json_rcv_msg.status);
                    break;
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:43:56 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
                case DTU_MODBUS_MSG_ID_GET_CONFIG_REPORT:
                    dtu_modbus_json_get_modbus_config_res();
                    break;
                case DTU_MODBUS_MSG_ID_SET_CONFIG_REPORT:
                    dtu_modbus_json_set_modbus_config_res(json_rcv_msg.status);
                    break;
                
                case DTU_MODBUS_MSG_ID_GET_CMD_REPORT:
                    dtu_modbus_json_get_modbus_cmd_res(json_rcv_msg.channel);
                    break;
                case DTU_MODBUS_MSG_ID_SET_CMD_REPORT:
                    dtu_modbus_json_set_modbus_cmd_res(json_rcv_msg.status);
                    break;
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 11:08:46 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
                case DTU_HTTP_MSG_ID_GET_CONFIG_REPORT:
                    dtu_http_json_get_http_config_res();
                    break;
                case DTU_HTTP_MSG_ID_SET_CONFIG_REPORT:
                    dtu_http_json_set_http_config_res(json_rcv_msg.status);
                    break;
                
                case DTU_HTTP_MSG_ID_GET_CMD_REPORT:
                    dtu_http_json_get_http_cmd_res();
                    break;
                case DTU_HTTP_MSG_ID_SET_CMD_REPORT:
                    dtu_http_json_set_http_cmd_res(json_rcv_msg.status);
                    break;

#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 11:09:21 by: zhaoning */
                default:
                    break;
            }
            
        }
        
//        if (json_rcv_msg.data)
//        {
//            free(json_rcv_msg.data);
//            json_rcv_msg.data = NULL;
//        }
    }

}

/**
  * Function    : dtu_json_task_init
  * Description : json主任务和消息队列创建
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_json_task_init(void)
{
    OSA_STATUS status = 0;

    /*creat message*/
    status = OSAMsgQCreate(&dtu_json_msgq, "dtu_json_msgq", DTU_JSON_TASK_MSGQ_MSG_SIZE, DTU_JSON_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
    ASSERT(status == OS_SUCCESS);
    //创建JSON任务
    status = OSATaskCreate(&dtu_json_task_ref, dtu_json_task_stack, DTU_JSON_TASK_STACK_SIZE, 151, "json_task", dtu_json_task, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_json.c 2023-9-18 14:32:08 by: zhaoning 

