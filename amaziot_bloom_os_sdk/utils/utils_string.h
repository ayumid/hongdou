//------------------------------------------------------------------------------
// Company     : Copyright (c) 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : utils_string.h
// Auther      : zhaoning
// Version     :
// Date        : 2021-7-19
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-7-19
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _UTILS_STRING_H_
#define _UTILS_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public macros / types / typedef ----------------------------------------------

#define                 ASCIISTR2HEX_MODE_LITTLE_ENDIAN         0
#define                 ASCIISTR2HEX_MODE_BIG_ENDIAN            1
#define                 ASCIISTR2HEX_MODE_LITTLE_ENDIAN_16         2
#define                 ASCIISTR2HEX_MODE_BIG_ENDIAN_16            3

#define             INCL_ERR_SUCCESS                         (0)

#define             INCL_ERR_GNSS_INVALID_ERR                (300)

typedef struct UTILS_GNSS_STANDARD_S_
{    
    char latitude[15];
    char longtitude[15];
    char speed[8];
    char angle[8];
    UINT32 ul_times;
    UINT32 ul_delay;
//#ifdef APP_DEV_GNSS_NUM_USED
//    char gsv[512];
//#endif /* ifdef APP_DEV_GNSS_NUM_USED.2022-9-15 10:50:06 by: zhaoning */
//    u32 ul_agps;
}UTILS_GNSS_STANDARD_T;

// Public functions prototypes --------------------------------------------------

//void utils_utc_2_localtime(UINT8* p_buffer);
//int utils_hex2ascii(UINT8 uc_hex);
//UINT16 utils_hex2ascii_str(char* p_str, UINT8* p_hex, UINT16 ul_len);
int utils_ascii2hex(UINT8 uc_ascii);
UINT32 utils_ascii_str2hex(UINT8 mode, char* p_hex, const char* p_str, UINT32 ulLen );
//void utils_itoa(int n,char s[]);
//int utils_ftoa(char *str, float num, int n);//n是转换的精度，即是字符串'.'后有几位小数
//void utils_insert_sort(UINT32 a[], int n);
//void utils_U_bubble_sort(UINT32 *array, int size);
//void utils_S_bubble_sort(INT32 *array, int size);
void utils_nmea_cpy(char* src, char* dest, char* nmea, char* next_nmea);
//double utils_get_distance_between_A2B(double lata, double lnga, double latb, double lngb);
//double utils_get_distance_between_A2B_by_haversine(double lata,double loga, double latb,double logb);

#ifdef __cplusplus
}
#endif

#endif /* ifndef UTILS_STRING_H_.2021-7-19 9:58:53 by: zhaoning */

