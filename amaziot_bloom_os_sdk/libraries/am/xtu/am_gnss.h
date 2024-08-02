//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_gnss.h
// Auther      : zhaoning
// Version     :
// Date : 2023-9-11
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-9-11
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_GNSS_H_
#define _AM_GNSS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

//#define                 GNSS_ANALYSIS

#define                 MODULES_GNSS_NMEA_RES_LEN                     (1024)//上报服务器缓冲区长度
#define                 MODULES_GNSS_NMEA_LEN                         (768)//串口接收最大语句长度
#define                 MODULES_GNSS_SINGLE_NMEA_LEN                  (100)//单个nmea长度
#define                 MODULES_GNSS_SINGLE_NMEA_RES_LONG_LEN         (500)//单个nmea

typedef struct DTU_GNSS_NMEA_S
{
    char a_nmea[MODULES_GNSS_NMEA_LEN];//串口接收最大语句缓冲区
    char a_nmea_gngga[MODULES_GNSS_SINGLE_NMEA_LEN];//gngga语句缓冲区
    char a_nmea_gngll[MODULES_GNSS_SINGLE_NMEA_LEN];//gngll语句缓冲区
    char a_nmea_gngsa[MODULES_GNSS_SINGLE_NMEA_LEN];//gngsa语句缓冲区
    char a_nmea_gpgsv[MODULES_GNSS_SINGLE_NMEA_RES_LONG_LEN];//gpgsv语句缓冲区
    char a_nmea_bdgsv[MODULES_GNSS_SINGLE_NMEA_RES_LONG_LEN];//bdgsv语句缓冲区
    char a_nmea_gnrmc[MODULES_GNSS_SINGLE_NMEA_LEN];//gnrmc语句缓冲区
    char a_nmea_gnvtg[MODULES_GNSS_SINGLE_NMEA_LEN];//gnvtg语句缓冲区
    char a_nmea_gnzda[MODULES_GNSS_SINGLE_NMEA_LEN];//gnzda语句缓冲区
    char a_nmea_gptxt[MODULES_GNSS_SINGLE_NMEA_LEN];//gptxt语句缓冲区

}DTU_GNSS_NMEA_T;

// Public functions prototypes --------------------------------------------------

void dtu_gnss_data_prase(char* gnss_data, UINT32 len);
void dtu_send_gnss_data_to_server(void);
void dtu_gnss_timer_init(void);
void dtu_gnss_timer_start(void);
void dtu_gnss_timer_stop(void);

void dtu_uart4_data_send_thread(void *param);
void dtu_uart4_data_recv_cbk(UINT8 *data, UINT32 len);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_GNSS_H_.2023-9-11 17:09:47 by: zhaoning */

