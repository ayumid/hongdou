//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_utils.h
// Auther      : zhaoning
// Version     :
// Date : 2023-3-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-3-18
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_UTILS_H_
#define _AM_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

// Public defines / typedef -----------------------------------------------------

#define CRON_SUPPORT 1

#define UTILSUPPORT 1

struct data_time_str
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t week;
};
typedef struct CronLine 
{
    char cl_Mins[60];    /* 0-59                                 */
    char cl_Hrs[24];    /* 0-23                                 */
    char cl_Days[32];    /* 0-31                                 */
    char cl_Mons[13];    /* 0-12                                 */
    char cl_Dow[7];        /* 0-6, beginning sunday                */
} CronLine;

// Public functions prototypes --------------------------------------------------

char *ParseField(char *user, char *ary, int modvalue, int off,const char *const *names, char *ptr);
uint8_t  GetMaxday(struct data_time_str *p_data);
uint8_t RTCGetWeek(uint32_t y,uint8_t m,uint8_t d);

void HexToStr(char *pbDest, char *pbSrc, int nLen);
int is_begin_with(const char * str1,char *str2);
int is_end_with(const char *str1, char *str2);
void UpdateTimeBuf(struct data_time_str *p_data);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_UTILS_H_.2023-3-18 17:22:10 by: zhaoning */

