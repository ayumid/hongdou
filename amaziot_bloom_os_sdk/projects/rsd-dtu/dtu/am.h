//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am.h
// Auther      : zhaoning
// Version     :
// Date : 2023-8-28
// Description :
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_H_
#define _AM_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdlib.h>

#include "sdk_api.h"

#ifdef DTU_BASED_ON_MQTT
#include "mqttclient.h"
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 17:36:56 by: zhaoning */
#ifdef DTU_TYPE_JSON_INCLUDE
#include "cJSON.h"
#endif /* ifdef DTU_TYPE_CJSON_INCLUDE.2023-10-30 18:35:01 by: zhaoning */

// Public defines / typedefs ----------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define uprintf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
//#define cprintf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define HEX2NUM(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) >= 'A' && (c) <= 'F') ? ((c) - ('A' - 0xa)) : ((c) - ('a' - 0xa)))//test

#define dtu_sleep(x) OSATaskSleep((x) * 200)//second
#define dtu_ms_sleep(x) OSATaskSleep((x) * 20)//100*msecond
#define dtu_10ms_sleep(x) OSATaskSleep((x) * 2)//10*msecond

#define DTU_VERSION                 "V1.0"
#define DTU_CGMI                    "Beijing Amaziot Co.,Ltd."
#define DTU_CGMM                    "Amaziot AM430EV5"
#define DTU_CGMR                    "AM430EV5"
#define DTU_CO                      "RSD"

//#define DTU_UART_REGULAR_BAUD

#ifdef DTU_TYPE_DODIAI_INCLUDE
#define DTU_TYPE                    "AP5000MT_R"
#else
#define DTU_TYPE                    "AP4000MT_D"
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-31 12:16:13 by: zhaoning */

#ifdef DTU_BASED_ON_TCP
#define DTU_NET_MODE                "TCP"
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:47:48 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
#define DTU_NET_MODE                "MQTT"
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 15:48:48 by: zhaoning */
#ifdef DTU_BASED_ON_HTTP
#define DTU_NET_MODE                "HTTP"
#endif /* ifdef DTU_BASED_ON_HTTP.2023-10-27 15:49:51 by: zhaoning */

#ifdef DTU_UART_9600_BAUD
#define BAUDRATE "9600"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define BAUDRATE "38400"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define BAUDRATE "115200"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#ifdef DTU_TYPE_GNSS_INCLUDE

#ifdef DTU_TYPE_EXSIM
#define FWVERSION "GNSS_M-L_Exsim"
#endif /* ifdef DTU_TYPE_EXSIM.2023-9-21 16:57:46 by: zhaoning */

#ifdef DTU_TYPE_5X6
#define FWVERSION "GNSS_M-L_5x6"
#endif /* ifdef DTU_TYPE_5X6.2023-9-21 16:57:21 by: zhaoning */

#ifdef DTU_TYPE_3IN1
#define FWVERSION "GNSS_M-L_3in1"
#endif /* ifdef DTU_TYPE_3IN1.2023-9-21 16:56:49 by: zhaoning */

#else
#ifdef DTU_TYPE_EXSIM
#define FWVERSION "M-L_Exsim"
#endif /* ifdef DTU_TYPE_EXSIM.2023-9-21 16:58:13 by: zhaoning */

#ifdef DTU_TYPE_5X6
#define FWVERSION "M-L_5x6"
#endif /* ifdef DTU_TYPE_5X6.2023-9-21 16:58:45 by: zhaoning */

#ifdef DTU_TYPE_3IN1
#define FWVERSION "M-L_3in1"
#endif /* ifdef DTU_TYPE_3IN1.2023-9-21 16:59:27 by: zhaoning */

#endif /* ifdef DTU_TYPE_GNSS_INCLUDE.2023-10-31 14:20:45 by: zhaoning */


#define DTU_DATA_MODE   0
#define DTU_AT_MODE     1

#define DTU_DEFAULT_THREAD_STACKSIZE 1024

enum 
{
    DTU_UART4_MSG_ID_INIT = 0,
    DTU_UART4_MSG_ID_RECV,
    DTU_UART4_MSG_ID_SEND,
};

typedef struct
{
    void *UArgs;//串口数据指针
    UINT32 id;//消息id
    UINT32 len;//数据长度
}DTU_MSG_UART_DATA_PARAM_T;

#define DTU_IP_MAX_LEN      65
#define DTU_HB_MAX_LEN      52
#define DTU_REG_MAX_LEN     52
#define DTU_CMDPW_MAX_LEN   10
#define DTU_NMEA_MAX_LEN    10

#ifdef DTU_BASED_ON_TCP
typedef struct
{
    UINT8 type;//类型 1 TCP 0 UDP
    char ip[DTU_IP_MAX_LEN];//IP地址或者域名
    int port;//端口号
}DTU_SOCKET_PARAM;
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:11:00 by: zhaoning */

#define DTU_MQTT_IP_MAX_LEN      65
#define DTU_MQTT_HB_MAX_LEN      52
#define DTU_MQTT_REG_MAX_LEN     52
#define DTU_MQTT_CMDPW_MAX_LEN   10
#define DTU_MQTT_NMEA_MAX_LEN    10

#define DTU_MQTT_CLIENTID_MAX_LEN    512
#define DTU_MQTT_USERNAME_MAX_LEN    512
#define DTU_MQTT_PASSWORD_MAX_LEN    512
#define DTU_MQTT_TOPIC_MAX_LEN       130

#ifdef DTU_BASED_ON_MQTT
typedef struct
{
    char ip[DTU_MQTT_IP_MAX_LEN];
    UINT32 port;

    char clientid[DTU_MQTT_CLIENTID_MAX_LEN];
    UINT16 keeplive;
    UINT8 cleansession;
    char username[DTU_MQTT_USERNAME_MAX_LEN];
    char password[DTU_MQTT_PASSWORD_MAX_LEN];
    char subtopic[DTU_MQTT_TOPIC_MAX_LEN];
    char subtopic1[DTU_MQTT_TOPIC_MAX_LEN];
    char subtopic2[DTU_MQTT_TOPIC_MAX_LEN];
    UINT8 subflag;
    UINT8 subqos;
    char pubtopic[DTU_MQTT_TOPIC_MAX_LEN];
    UINT8 pubqos;
    UINT8 duplicate;
    UINT8 retain;
}DTU_MQTT_PARAM;
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:11:23 by: zhaoning */

typedef struct
{
    UINT8 heartflag;//心跳打开标志 1 打开 0 关闭
    int hearttime;//心跳间隔时间
    char heart[DTU_HB_MAX_LEN];//心跳包数据
}DTU_HB_PARAM;

typedef struct
{
    UINT8 linkflag;//注册包上报打开标志 1 打开 0 关闭
    char link[DTU_REG_MAX_LEN];//注册包数据
}DTU_REG_PARAM;

typedef struct
{
    char cmdpw[DTU_CMDPW_MAX_LEN];//网络AT指令前缀
}DTU_NET_ATCMD_PARAM;

typedef struct
{
    UINT8 sim;//sim卡编号 仅三合一卡有效
    UINT8 simlock;//锁定sim卡
}DTU_SIM_PARAM;

typedef struct
{
    UINT8 gpsflag;//定位上报打开标志 1 打开 0 关闭
    int gpstime;//上报时间间隔
    char gpsnmea[DTU_NMEA_MAX_LEN];//nmea上报标志
}DTU_GNSS_PARAM;

#ifdef DTU_TYPE_DODIAI_INCLUDE
#define             DTU_GPIO_CHNNEL_0                    0//GPIO所有通道
#define             DTU_GPIO_CHNNEL_1                    1//GPIO通道1
#define             DTU_GPIO_CHNNEL_2                    2//GPIO通道2
#define             DTU_GPIO_CHNNEL_3                    3//GPIO通道3
#define             DTU_GPIO_CHNNEL_4                    4//GPIO通道4
#define             DTU_GPIO_CHNNEL_5                    5//GPIO通道5

#define             DTU_BUF_INDEX_1                    0//
#define             DTU_BUF_INDEX_2                    1//
#define             DTU_BUF_INDEX_3                    2//
#define             DTU_BUF_INDEX_4                    3//
#define             DTU_BUF_INDEX_5                    4//

#define             DTU_TIME_INDEX_1                          1//定时器1
#define             DTU_TIME_INDEX_2                          2//定时器2
#define             DTU_TIME_INDEX_3                          3//定时器3
#define             DTU_TIME_INDEX_4                          4//定时器4
#define             DTU_TIME_INDEX_5                          5//定时器5

#define             DTU_GPIO_DO_PIN1                    14//28 pin
#define             DTU_GPIO_DO_PIN2                    15//29 pin
#define             DTU_GPIO_DO_PIN3                    20//64 pin
#define             DTU_GPIO_DO_PIN4                    22//21 pin
#define             DTU_GPIO_DO_PIN5                    17//44 pin

#define             DTU_GPIO_DI_PIN1                    11//58 pin
#define             DTU_GPIO_DI_PIN2                    26//49 pin
#define             DTU_GPIO_DI_PIN3                    25//66 pin
#define             DTU_GPIO_DI_PIN4                    78//57 pin
#define             DTU_GPIO_DI_PIN5                    53//22 pin

#define DTU_GPIO_LOW                             0//GPIO低电平
#define DTU_GPIO_HIGH                            1//GPIO高电平

#define DTU_DO_MAX_NUM                           5//DO通道个数

#define DTU_DO_REPORT_NONE                       0//DO不上报
#define DTU_DO_REPORT_INITIATIVE                 1//DO间隔上报

#define DTU_DO_OUT_STATUS_LOW                    0//DO上电低电平
#define DTU_DO_OUT_STATUS_HIGH                   1//DO上电高电平
#define DTU_DO_OUT_STATUS_LAST                   2//DO上电上次断电前电平

#define DTU_DO_TIME                              1//DO定时输出
#define DTU_DO_FLIP                              2//DO定时翻转

#define DTU_DI_MAX_NUM                           5//DI通道个数

#define DTU_DI_EDGE_FALLING                      0//DI检测下降沿
#define DTU_DI_PULL_RISING                       1//DI检测上升沿

#define DTU_DI_REPORT_NONE                       0//DI不上报
#define DTU_DI_REPORT_TRIGGER                    2//DI触发上报
#define DTU_DI_REPORT_INITIATIVE                 1//DI间隔上报

#define DTU_DI_REPORT_INTER                      1
#define DTU_DI_REPORT_TRIG                       2

#define DTU_AI_REPORT_NONE                       0//AI不上报
#define DTU_AI_REPORT_TRIGGER                    2//AI触发上报
#define DTU_AI_REPORT_INITIATIVE                 1//AI周期上报

#define DTU_AI_REPORT_OUT                        1//AI阈值内上报
#define DTU_AI_REPORT_IN                         2//AI阈值外上报

#define DTU_AI_RES_VAL                  51.0

#define DTU_DO_FLOW_DI_FORWARD                   1//正向跟随DI
#define DTU_DO_FLOW_DI_BACKWARD                  2//反向跟随DI
#define DTU_DO_FLOW_AI_DD_OPH                    3//AI阈值内输出高
#define DTU_DO_FLOW_AI_DD_OPL                    4//AI阈值内输出低
#define DTU_DO_FLOW_AI_XD_OPH                    5//AI阈值外输出高
#define DTU_DO_FLOW_AI_XD_OPL                    6//AI阈值外输出低

#define DTU_CLK_MAX_NUM                          5//闹钟个数

#define DTU_CLK_OFF                              0//闹钟关闭
#define DTU_CLK_ON                               1//闹钟打开

#define DTU_CLK_INDEX_1                          1//闹钟1
#define DTU_CLK_INDEX_2                          2//闹钟2
#define DTU_CLK_INDEX_3                          3//闹钟3
#define DTU_CLK_INDEX_4                          4//闹钟4
#define DTU_CLK_INDEX_5                          5//闹钟5

typedef struct
{
    UINT8 edge;//DI 1 拉高 0 拉低
    UINT8 type;//2 定时上报 1 触发上报 0 不上报
    UINT32 interval;//上报间隔
}DTU_DI;

typedef struct
{
    DTU_DI params[DTU_DI_MAX_NUM];//DI参数
}DTU_DI_PARAM;

typedef struct
{
    UINT8 out;//掉电状态保存
    UINT8 type;//1 定时上报 0 不上报
    UINT32 interval;//上报间隔
    UINT8 status;//DO上电状态 2 上一次掉电前状态 1 上电高电平 0 上电低电平
//    UINT32 do_time;//上报定时器间隔
//    UINT32 do_flip;//翻转功能
}DTU_DO;

typedef struct
{
    DTU_DO params[DTU_DO_MAX_NUM];//DO参数
}DTU_DO_PARAM;

typedef struct
{
    UINT8 ai_type;//AI 是否使用
    UINT8 ai_res_rule;//AI 报警上报规则
    UINT8 ai_alarm_high;//AI 高限制
    UINT8 ai_alarm_low;//AI 低限制
    UINT32 ai_interval;//AI 上报间隔
}DTU_AI_PARAM;

typedef struct
{
    UINT8 do_flow[DTU_DO_MAX_NUM];//DOx 跟随状态
}DTU_FLOW_PARAM;

typedef struct
{
    UINT8 sw;//1 打开定时器，0 关闭定时器
    UINT8 h;//时
    UINT8 m;//分
    UINT8 s;//秒
    UINT8 type;//类型
    UINT8 num;//动作DOx
    UINT8 level;//动作电平 1断开 2闭合
}DTU_CLK;

typedef struct
{
    DTU_CLK params[DTU_CLK_MAX_NUM];//定时器参数
}DTU_CLK_PARAM;
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:38:38 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
#define DTU_MODBUS_POOLLING_STATE                0//modbus 普通指令轮训状态 01H 02H 03H 04H 05H 06H
#define DTU_MODBUS_POOLLING_WN_STATE             1//modbus 写多个寄存器指令轮训状态 0FH 10H

#define DTU_MODBUS_CMD_NUM                       50//modbus最大支持的 01H 02H 03H 04H 05H 06H指令数量
#define DTU_MODBUS_CMD_WN_NUM                    2//modbus最大支持的指令数量

#define DTU_MODBUS_TYPE_ENABLE                   1//modbus功能打开
#define DTU_MODBUS_TYPE_DISENABLE                0//modbus功能关闭

#define DTU_MODBUS_ACTIVE                        1//指令激活
#define DTU_MODBUS_INACTIVE                      0//指令禁止

#define DTU_MODBUS_REG_LEN                       123//下发寄存器内容缓冲区长度 (256 - 9) / 2

#define DTU_MODBUS_TIMER_INTERVAL_MSG            1//modbus定时器消息
#define DTU_MODBUS_DATA_MSG                      2//modbus定时器消息


#define DTU_MODBUS_RESPONSE_RTU_TYPE                        0//Modbus RTU协议上报
#define DTU_MODBUS_RESPONSE_TCP_TYPE                        1//Modbus TCP协议上报
#define DTU_MODBUS_RESPONSE_JSON_TYPE                       2// JSON 协议上报

#define DTU_MODBUS_PROTOCOL_SUCCESS                        0//modbus帧校验成功
#define DTU_MODBUS_PROTOCOL_ERROR                          1//modbus帧校验失败

#define DTU_MODBUS_RES_LEN                       1024//上报是把rtu转为字符串

typedef struct
{
    UINT8 type;//是否打开modbus
    UINT8 wait;//指令发出后，等待从机上报超时时间
    UINT8 interval;//指令之间间隔时间
    UINT16 delay;//指令循环完毕后等待时间
    UINT8 res_type;//modbus从机数据上报类型，0 JSON 协议上报 1 Modbus TCP协议上报 2 Modbus RTU协议上报
}DTU_MODBUS_CONFIG_PARAM;

typedef struct
{
    UINT8 active;//指令激活 1: 激活 0: 未激活
    UINT8 slave_addr;//从机地址
    UINT8 fn;//功能码
    UINT16 reg_addr;//01H 02H 03H 04H 寄存器首地址 05H 06H 写入寄存器地址
    UINT16 reg_n_d;//01H 02H 03H 04H 寄存器个数 05H 06H 写单个寄存器的数据
}DTU_MODBUS_CMD_PARAM;

typedef struct
{
    UINT8 active;//指令激活 1: 激活 0: 未激活
    UINT8 slave_addr;//从机地址
    UINT8 fn;//功能码
    UINT16 reg_addr;// 0FH 10H 寄存器首地址
    UINT16 reg_n;//10H 寄存器个数
    UINT16 reg_data[DTU_MODBUS_REG_LEN];//写入寄存器的内容缓冲区
}DTU_MODBUS_CMD_WN_PARAM;

typedef struct
{
    DTU_MODBUS_CONFIG_PARAM config;//modbus配置参数
    DTU_MODBUS_CMD_PARAM cmd[DTU_MODBUS_CMD_NUM];//modbus指令数组
    DTU_MODBUS_CMD_WN_PARAM cmd_wn[DTU_MODBUS_CMD_WN_NUM];
}DTU_MODBUS_PARAM;

typedef struct
{
    UINT32 id;//mod指令列表中的id
    UINT32 state;//modbus有两种指令，DTU_MODBUS_CMD_PARAM DTU_MODBUS_CMD_WN_PARAM，分两种状态轮训
    OSATimerRef md_timer_ref;//指令发送间隔定时器
}DTU_MODBUS_T;

#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:30:22 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
#define DTU_HTTP_TYPE_ENABLE                     1
#define DTU_HTTP_TYPE_DISENABLE                  0

#define DTU_HTTP1_TYPE_POST                      2
#define DTU_HTTP1_TYPE_GET                       1
#define DTU_HTTP1_TYPE_DISENABLE                 0

#define DTU_HTTP2_TYPE_POST                      2
#define DTU_HTTP2_TYPE_GET                       1
#define DTU_HTTP2_TYPE_DISENABLE                 0

#define DTU_HTTP_URL_MAX_LEN                     256
#define DTU_HTTP_HEAD_MAX_LEN                    64
#define DTU_HTTP_QUERY_MAX_LEN                   64

#define DTU_HTTP_TYPE_POST                       1
#define DTU_HTTP_TYPE_GET                        2
#define DTU_HTTPS_TYPE_POST                      3
#define DTU_HTTPS_TYPE_GER                       4

typedef struct
{
    UINT8 type;//0 不使用    1 开启
}DTU_HTTP_CONFIG_PARAM;

typedef struct
{
    UINT8 type;//1 http post  2 http get  //暂不支持https 3 https post 4 https get
    char url[DTU_HTTP_URL_MAX_LEN];//http实例url
    char head1[DTU_HTTP_HEAD_MAX_LEN];//http实例请求头
    char head2[DTU_HTTP_HEAD_MAX_LEN];//http实例请求头
    char head3[DTU_HTTP_HEAD_MAX_LEN];//http实例请求头
//    char query1[DTU_HTTP_QUERY_MAX_LEN];
//    char query2[DTU_HTTP_QUERY_MAX_LEN];
//    char query3[DTU_HTTP_QUERY_MAX_LEN];
}DTU_HTTP_CMD_PARAM;

typedef struct
{
    DTU_HTTP_CONFIG_PARAM config;//http配置参数
    DTU_HTTP_CMD_PARAM http1;//http1实例参数
    DTU_HTTP_CMD_PARAM http2;//http2实例参数
}DTU_HTTP_PARAM;
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:30:03 by: zhaoning */

typedef struct
{
#ifdef DTU_BASED_ON_TCP
    DTU_SOCKET_PARAM socket;//tcp相关参数
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:58:10 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
    DTU_MQTT_PARAM mqtt;
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:09:13 by: zhaoning */
    DTU_HB_PARAM hb;//心跳包相关参数
    DTU_REG_PARAM reg;//注册包相关参数
    DTU_NET_ATCMD_PARAM net_at;//网络AT相关参数

#ifdef DTU_TYPE_3IN1
    DTU_SIM_PARAM sim;//SIM卡相关参数
#endif

#ifdef DTU_TYPE_GNSS_INCLUDE
    DTU_GNSS_PARAM gnss;//定位相关参数
#endif

#ifdef DTU_TYPE_DODIAI_INCLUDE
    DTU_DI_PARAM di;//DI相关参数
    DTU_DO_PARAM doo;//DO相关参数
    DTU_AI_PARAM ai;//AI相关参数
    DTU_FLOW_PARAM flow;//IO跟随相关参数
    DTU_CLK_PARAM clk;//时钟相关参数
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:38:52 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
    DTU_MODBUS_PARAM modbus;//modbus相关参数
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:29:42 by: zhaoning */
    
#ifdef DTU_TYPE_HTTP_INCLUDE
    DTU_HTTP_PARAM http;//http相关参数
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:09:09 by: zhaoning */
}DTU_FILE_PARAM_T;

#ifdef DTU_BASED_ON_TCP
typedef struct
{
    int fd;//tcp下行描述符
    char remoteIp[256];//ip
    int remotePort;//端口
    int socketType; //0:TCP  1:UDP
    OSMsgQRef dtu_msgq_socket_recv;//下行卡消息队列
}DTU_SOCKET_PARAM_T;

typedef struct
{
    int fd;//描述符
    DTU_SOCKET_PARAM_T *sock;//下行socket参数
}DTU_SOCKET_RECV_TYPE_T;
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:14:08 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
typedef struct
{
    mqtt_client_t *dtu_mqtt_client;
    UINT8 dtu_mqtt_subflag;
}DTU_MQTT_PARAM_T;
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:14:15 by: zhaoning */

typedef struct
{
    uint8_t uart_mode;    // 0:DTU_DATA_MODE 1:AT_MDOE
    OSMsgQRef dtu_msgq_uart;//at串口消息队列
    
#ifdef DTU_TYPE_GNSS_INCLUDE
    OSMsgQRef dtu_msgq_uart4;//串口4消息队列
#endif
}DTU_UART_PARAM_T;

typedef struct
{
    OSATimerRef dtu_hb_timer_ref;//心跳定时器
}DTU_HB_TIME_PARAM_T;

enum{
    DTU_SYS_MSG_ID_GET_SYS_CONFIG_REPORT = 1,
    DTU_SYS_MSG_ID_SET_SYS_CONFIG_REPORT,
    DTU_NET_MSG_ID_GET_SYS_CONFIG_REPORT,
    DTU_NET_MSG_ID_SET_SYS_CONFIG_REPORT,
#ifdef DTU_BASED_ON_TCP
    DTU_SYS_MSG_ID_GET_TCP_CONFIG_REPORT,
    DTU_SYS_MSG_ID_SET_TCP_CONFIG_REPORT,
    DTU_NET_MSG_ID_GET_TCP_CONFIG_REPORT,
    DTU_NET_MSG_ID_SET_TCP_CONFIG_REPORT,
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-31 11:14:52 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    DTU_SYS_MSG_ID_GET_MQTT_CONFIG_REPORT,
    DTU_SYS_MSG_ID_SET_MQTT_CONFIG_REPORT,
    DTU_NET_MSG_ID_GET_MQTT_CONFIG_REPORT,
    DTU_NET_MSG_ID_SET_MQTT_CONFIG_REPORT,
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-31 11:15:03 by: zhaoning */
#ifdef DTU_TYPE_DODIAI_INCLUDE
    DTU_DI_MSG_ID_DI_PROACTIVE_REPORT,
    DTU_DI_MSG_ID_DI_TRIGGER_REPORT,
    DTU_DI_MSG_ID_GET_VALUE_REPORT,
    DTU_DI_MSG_ID_GET_CONFIG_REPORT,
    DTU_DI_MSG_ID_SET_CONFIG_REPORT,
    DTU_DO_MSG_ID_DO_PROACTIVE_REPORT,
    DTU_DO_MSG_ID_GET_VALUE_REPORT,
    DTU_DO_MSG_ID_SET_VALUE_REPORT,
    DTU_DO_MSG_ID_GET_CONFIG_REPORT,
    DTU_DO_MSG_ID_SET_CONFIG_REPORT,
    DTU_AI_MSG_ID_AI_PROACTIVE_REPORT,
    DTU_AI_MSG_ID_GET_VALUE_REPORT,
    DTU_AI_MSG_ID_GET_CONFIG_REPORT,
    DTU_AI_MSG_ID_SET_CONFIG_REPORT,
    DTU_FLOW_MSG_ID_GET_CONFIG_REPORT,
    DTU_FLOW_MSG_ID_SET_CONFIG_REPORT,
    DTU_CLK_MSG_ID_GET_CONFIG_REPORT,
    DTU_CLK_MSG_ID_SET_CONFIG_REPORT,
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:43:39 by: zhaoning */
    DTU_OTA_MSG_ID_SET_CONFIG_REPORT,
#ifdef DTU_TYPE_MODBUS_INCLUDE
    DTU_MODBUS_MSG_ID_GET_CONFIG_REPORT,
    DTU_MODBUS_MSG_ID_SET_CONFIG_REPORT,
    DTU_MODBUS_MSG_ID_GET_CMD_REPORT,
    DTU_MODBUS_MSG_ID_SET_CMD_REPORT,
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 11:08:58 by: zhaoning */
#ifdef DTU_TYPE_HTTP_INCLUDE
    DTU_HTTP_MSG_ID_GET_CONFIG_REPORT,
    DTU_HTTP_MSG_ID_SET_CONFIG_REPORT,
    DTU_HTTP_MSG_ID_GET_CMD_REPORT,
    DTU_HTTP_MSG_ID_SET_CMD_REPORT,
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 11:09:11 by: zhaoning */
};

typedef struct _DTU_JSON_MSG_S
{
    UINT8        msgId;//消息ID
    UINT8        status;//当前消息状态，可作为回复服务器json中的成功 失败显示
    UINT8        channel;//回复服务器有通道的话
    UINT8        result;//结果
    UINT16        current;//ai消息中电流
//    UINT16       us_value;
}DTU_JSON_MSG_T, *P_DTU_JSON_MSG_T;

// Public functions prototypes --------------------------------------------------

int is_begin_with(const char * str1,char *str2);

void dtu_send_serial_data_to_server(DTU_MSG_UART_DATA_PARAM_T * uartData);
int dtu_process_at_cmd_line(char *cmdName, char *cmdLine);
void dtu_send_to_uart(char *toUart, int length);
void dtu_handle_serial_data(DTU_MSG_UART_DATA_PARAM_T *uartData);
void dtu_trans_task_init(void);

void dtu_checknet_task_init(void);
void dtu_gnss_task_init(void);

#ifdef DTU_BASED_ON_TCP
DTU_SOCKET_PARAM_T* dtu_get_socket_ctx(void);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:04:30 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
DTU_MQTT_PARAM_T* dtu_get_mqtt_ctx(void);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 17:04:49 by: zhaoning */
DTU_FILE_PARAM_T* dtu_get_file_ctx(void);
DTU_UART_PARAM_T* dtu_get_uart_ctx(void);
DTU_HB_TIME_PARAM_T* dtu_get_hb_timer_ctx(void);

void dtu_hb_timer_callback(UINT32 tmrId);
void dtu_hb_timer_init(void);

int dtu_init_trans_conf(void);
int dtu_trans_conf_file_write(DTU_FILE_PARAM_T* st_dtu_file);
void dtu_trans_conf_file_init(void);

int dtu_json_task_send_msgq(DTU_JSON_MSG_T * msg);

void dtu_sys_json_get_sys_config(void);
void dtu_sys_json_set_sys_config(int len , char *rcvdata);
void dtu_net_json_get_net_config(void);
void dtu_net_json_set_net_config(int len , char *rcvdata);

#ifdef DTU_TYPE_DODIAI_INCLUDE
void dtu_di_json_get_di_value(void);
void dtu_di_json_get_di_config(void);
void dtu_di_json_set_di_config(int len , char *rcvdata);
void dtu_di_times_init(void);

void dtu_ai_json_get_ai_value(void);
void dtu_ai_json_get_ai_config(void);
void dtu_ai_json_set_ai_config(int len , char *rcvdata);
void dtu_ai_times_init(void);
void dtu_ai1_report_timer_start(UINT32 time);
void dtu_ai1_report_timer_stop(void);

void dtu_do_json_get_do_value(void);
void dtu_do_json_set_do_value(int len , char *rcvdata);
void dtu_do_json_get_do_config(void);
void dtu_do_json_set_do_config(int len , char *rcvdata);
void dtu_do_report_timer_start(UINT8 index, UINT32 time);
void dtu_do_report_timer_stop(UINT8 index);
//void dtu_do1_timer_stop(void);
//void dtu_do2_timer_stop(void);
//void dtu_do3_timer_stop(void);
//void dtu_do4_timer_stop(void);
//void dtu_do5_timer_stop(void);
void dtu_do_times_init(void);
void dtu_do_clk_timeout_write_pin(UINT8 channel, UINT8 status);

void dtu_clk_times_init(void);
void dtu_clk_timer_start(UINT8 index);

void dtu_flow_json_get_flow_config(void);
void dtu_flow_json_set_flow_config(int len , char *rcvdata);
void dtu_clk_json_get_clk_config(void);
void dtu_clk_json_set_clk_config(int len , char *rcvdata);
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 14:18:38 by: zhaoning */

void dtu_ota_json_set_ota_config(int len , char *rcvdata);

#ifdef DTU_TYPE_MODBUS_INCLUDE
void dtu_modbus_send_flag(void);
void dtu_modbus_task_init(void);
int dtu_modbus_task_send_msgq(DTU_MSG_UART_DATA_PARAM_T * msg);
void dtu_modbus_interval_timer_start(void);
void dtu_modbus_interval_timer_stop(void);

void dtu_modbus_json_get_modbus_config(void);
void dtu_modbus_json_set_modbus_config(int len , char *rcvdata);
void dtu_modbus_json_get_modbus_cmd(int len , char *rcvdata);
void dtu_modbus_json_get_modbus_cmdwn(int len , char *rcvdata);
void dtu_modbus_json_set_modbus_cmd(int len , char *rcvdata);
void dtu_modbus_json_set_modbus_cmdwn(int len , char *rcvdata);
void dtu_modbus_json_modbus_res(UINT8* data, UINT32 len);
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-27 14:19:03 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
int dtu_http_s_task_send_msgq(DTU_MSG_UART_DATA_PARAM_T * msg);
void dtu_http_s_task_init(void);
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-27 14:19:16 by: zhaoning */

void dtu_json_data_prase(int len , char *rcvdata);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_H_.2023-8-28 10:35:38 by: zhaoning */

