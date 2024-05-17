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

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

#undef printf
/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
//#define printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define HEX2NUM(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) >= 'A' && (c) <= 'F') ? ((c) - ('A' - 0xa)) : ((c) - ('a' - 0xa)))//test

#define sleep(x) OSATaskSleep((x) * 200)//second
#define msleep(x) OSATaskSleep((x) * 20)//100*msecond

#define DTU_VERSION                 "1.0"

#define DTU_TYPE_EXSIM//使用外置卡
//#define DTU_TYPE_5X6//使用5x6卡
//#define DTU_TYPE_3IN1//使用三合一卡
//#define DTU_TYPE_EXSIM_GNSS//使用外置卡 带定位
//#define DTU_TYPE_5X6_GNSS//使用5x6卡 带定位
//#define DTU_TYPE_3IN1_GNSS//使用三合一卡 带定位


#define DTU_UART_9600_BAUD//使用9600波特率
//#define DTU_UART_38400_BAUD//使用38400波特率
//#define DTU_UART_115200_BAUD//使用115200波特率

#ifdef DTU_TYPE_EXSIM_GNSS

#ifdef DTU_UART_9600_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_Exsim-V1.0-9600-202310261155"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_Exsim-V1.0-38400-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_Exsim-V1.0-115200-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#endif /* ifdef DTU_TYPE_EXSIM_GNSS.2023-9-21 16:57:46 by: zhaoning */

#ifdef DTU_TYPE_5X6_GNSS

#ifdef DTU_UART_9600_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_5x6-V1.0-9600-202310261155"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_5x6-V1.0-38400-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_5x6-V1.0-115200-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#endif /* ifdef DTU_TYPE_5X6_GNSS.2023-9-21 16:57:21 by: zhaoning */

#ifdef DTU_TYPE_3IN1_GNSS

#ifdef DTU_UART_9600_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_3in1-V1.0-9600-202310261155"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_3in1-V1.0-38400-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define FWVERSION "AP4000MT_430EV5_GNSS_M-L_3in1-V1.0-115200-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#endif /* ifdef DTU_TYPE_3IN1_GNSS.2023-9-21 16:56:49 by: zhaoning */

#ifdef DTU_TYPE_EXSIM

#ifdef DTU_UART_9600_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_Exsim-V1.0-9600-202310261155"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_Exsim-V1.0-38400-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_Exsim-V1.0-115200-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#endif /* ifdef DTU_TYPE_EXSIM.2023-9-21 16:58:13 by: zhaoning */

#ifdef DTU_TYPE_5X6

#ifdef DTU_UART_9600_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_5x6-V1.0-9600-202310261155"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_5x6-V1.0-38400-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_5x6-V1.0-115200-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#endif /* ifdef DTU_TYPE_5X6.2023-9-21 16:58:45 by: zhaoning */

#ifdef DTU_TYPE_3IN1

#ifdef DTU_UART_9600_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_3in1-V1.0-9600-202310261155"
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_3in1-V1.0-38400-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
#define FWVERSION "AP4000MT_430EV5_M-L_3in1-V1.0-115200-202310261155"
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

#endif /* ifdef DTU_TYPE_3IN1.2023-9-21 16:59:27 by: zhaoning */

#define DTU_DATA_MODE   0
#define DTU_AT_MODE     1

#define DTU_IP_MAX_LEN      65
#define DTU_HB_MAX_LEN      52
#define DTU_REG_MAX_LEN     52
#define DTU_CMDPW_MAX_LEN   10
#define DTU_NMEA_MAX_LEN    10

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

typedef struct
{
    UINT8 type;//类型 1 TCP 0 UDP
    char ip[DTU_IP_MAX_LEN];//IP地址或者域名
    int port;//端口号
}DTU_SOCKET_PARAM;

typedef struct
{
    UINT8 heartflag;//心跳打开标志 1 打开 0 关闭
    int hearttime;//心跳间隔时间
    char heart[DTU_HB_MAX_LEN];//心跳包数据
}DTU_HB_PARAM;

typedef struct
{
    UINT8 linkflag;//注册包上报打开标志 1 打开 0 关闭
    UINT8 linktype;
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

typedef struct
{
    DTU_SOCKET_PARAM socket;//tcp相关参数
    DTU_HB_PARAM hb;//心跳包相关参数
    DTU_REG_PARAM reg;//注册包相关参数
    DTU_NET_ATCMD_PARAM net_at;//网络AT相关参数

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
    DTU_SIM_PARAM sim;//SIM卡相关参数
#endif

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
    DTU_GNSS_PARAM gnss;//定位相关参数
#endif

}DTU_FILE_PARAM_T;

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

typedef struct
{
    uint8_t uart_mode;    // 0:DTU_DATA_MODE 1:AT_MDOE
    OSMsgQRef dtu_msgq_uart;//at串口消息队列
    
#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
    OSMsgQRef dtu_msgq_uart4;//串口4消息队列
#endif
}DTU_UART_PARAM_T;

typedef struct
{
    OSATimerRef dtu_timer_ref;//心跳定时器
}DTU_TIME_PARAM_T;

// Public functions prototypes --------------------------------------------------

int is_begin_with(const char * str1,char *str2);

void dtu_send_serial_data_to_server(DTU_MSG_UART_DATA_PARAM_T * uartData);
int dtu_process_at_cmd_line(char *cmdName, char *cmdLine);

DTU_SOCKET_PARAM_T* dtu_get_socket_ctx(void);
DTU_FILE_PARAM_T* dtu_get_file_ctx(void);
DTU_UART_PARAM_T* dtu_get_uart_ctx(void);
DTU_TIME_PARAM_T* dtu_get_timer_ctx(void);

void dtu_hb_timer_callback(UINT32 tmrId);

int dtu_init_trans_conf(void);
int dtu_trans_conf_file_write(DTU_FILE_PARAM_T* st_dtu_file);
void dtu_trans_conf_file_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_H_.2023-8-28 10:35:38 by: zhaoning */

