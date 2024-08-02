//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_utils.c
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

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "osa.h"
#include "am_utils.h"
#include "am_common.h"
#include "utils_common.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

#ifdef CRON_SUPPORT

//OSFlagRef  ClockFlgRef = NULL;
//OSFlagRef  ClockFlgRefUpdateTime = NULL;

char *ParseField(char *user, char *ary, int modvalue, int off,
                        const char *const *names, char *ptr)
{
    char *base = ptr;
    int n1 = -1;
    int n2 = -1;

    if (base == NULL) {
        return (NULL);
    }

    while (*ptr != ' ' && *ptr != '\t' && *ptr != '\n') {
        int skip = 0;

        /* Handle numeric digit or symbol or '*' */

        if (*ptr == '*') {
            n1 = 0;        /* everything will be filled */
            n2 = modvalue - 1;
            skip = 1;
            ++ptr;
        } else if (*ptr >= '0' && *ptr <= '9') {
            if (n1 < 0) {
                n1 = strtol(ptr, &ptr, 10) + off;
            } else {
                n2 = strtol(ptr, &ptr, 10) + off;
            }
            skip = 1;
        } else if (names) {
            int i;

            for (i = 0; names[i]; ++i) {
                if (strncmp(ptr, names[i], strlen(names[i])) == 0) {
                    break;
                }
            }
            if (names[i]) {
                ptr += strlen(names[i]);
                if (n1 < 0) {
                    n1 = i;
                } else {
                    n2 = i;
                }
                skip = 1;
            }
        }

        /* handle optional range '-' */

        if (skip == 0) {
            cprintf("  failed user %s parsing %s", user, base);
            cprintf("FUN:%s LINE:%d TIME:%s", __FUNCTION__, __LINE__, __TIME__);
            return (NULL);
        }
        if (*ptr == '-' && n2 < 0) {
            ++ptr;
            continue;
        }

        /*
         * collapse single-value ranges, handle skipmark, and fill
         * in the character array appropriately.
         */

        if (n2 < 0) {
            n2 = n1;
        }
        if (*ptr == '/') {
            skip = strtol(ptr + 1, &ptr, 10);
        }
        /*
         * fill array, using a failsafe is the easiest way to prevent
         * an endless loop
         */

        {
            int s0 = 1;
            int failsafe = 1024;

            --n1;
            do {
                n1 = (n1 + 1) % modvalue;

                if (--s0 == 0) {
                    ary[n1 % modvalue] = 1;
                    s0 = skip;
                }
            }
            while (n1 != n2 && --failsafe);

            if (failsafe == 0) {
                cprintf(" failed user %s parsing %s", user, base);
                cprintf("FUN:%s LINE:%d TIME:%s", __FUNCTION__, __LINE__, __TIME__);
                return (NULL);
            }
        }
        if (*ptr != ',') {
            break;
        }
        ++ptr;
        n1 = -1;
        n2 = -1;
    }

    if (*ptr != ' ' && *ptr != '\t' && *ptr != '\n') {
        cprintf(" failed user %s parsing %s", user, base);
        cprintf("FUN:%s LINE:%d TIME:%s", __FUNCTION__, __LINE__, __TIME__);
        return (NULL);
    }

    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
        ++ptr;
    }

    if (1) 
    {
        int i;
        cprintf("%s:",user);
        for (i = 0; i < modvalue; ++i) {
            cprintf(" %d ", ary[i]);
        }
//        cprintf("\r\n");
    }

    return (ptr);
}

uint8_t  GetMaxday(struct data_time_str *p_data)
{
    uint8_t maxday = 0;
    uint16_t year = p_data->year;
    switch(p_data->month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            maxday = 31;  
            break;
        case 2:
            if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            {
                maxday = 29;  
            }
            else
            {
                maxday = 28;  
            }             
            break;
        default :
             maxday = 30; 
             break;
    }
    return maxday;
}


uint8_t RTCGetWeek(uint32_t y,uint8_t m,uint8_t d)
{
    uint8_t week = 0;
    if(m==1||m==2) 
    {
        m+=12;
        y--;
    }
    week=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400+1)%7;
    return week;
}

void UpdateTimeBuf(struct data_time_str *p_data) 
{
    uint8_t maxday = 0;
    if(p_data->sec == 59)
    {
        p_data->sec = 0;
        if(p_data->min == 59)
        {
//            OSAFlagSet(ClockFlgRef, 0x01, OSA_FLAG_OR);//check timer 1s //20211129
            app_msgq_msg updateMsg = {0};
            updateMsg.msgId = SEND_TASK_MSG_CLOCKUPDATA_MSG;
            send_task_send_msgq(&updateMsg);
            p_data->min = 0;
            if(p_data->hour == 23)
            {    
                
                p_data->hour = 0;
                maxday = GetMaxday(p_data);
                if(p_data->day ==  maxday)
                {
                    p_data->day = 1;
                    if(p_data->month == 12)
                    {
                        p_data->month = 1;
                        p_data->year++;
                    }
                    else
                    {
                        p_data->month++;    
                    }
                }
                else
                {
                    p_data->day++;    
                }
            }
            else
            {
                p_data->hour++;
            }
        }
        else
        {
            p_data->min++;    
        }    
    }
    else
    {
        p_data->sec++;    
    }
        
        
    p_data->week = RTCGetWeek(p_data->year,p_data->month,p_data->day);
}

#endif

//add by dmh begin
#ifdef UTILSUPPORT
void HexToStr(char *pbDest, char *pbSrc, int nLen)
{
    char ddl = 0;
    char ddh = 0;
    int i = 0;

    for (i=0; i<nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 39;
        if (ddl > 57) ddl = ddl + 39;
        pbDest[i*2] = ddh;
        pbDest[i*2+1] = ddl;
    }

    pbDest[nLen*2] = '\0';
}

/*function for utils*/
int is_begin_with(const char * str1,char *str2)
{
  if(str1 == NULL || str2 == NULL)
    return -1;
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  if((len1 < len2) || (len1 == 0 || len2 == 0))
    return -1;
  char *p = str2;
  int i = 0;
  while(*p != '\0')
  {
    if(*p != str1[i])
      return 0;
    p++;
    i++;
  }
  return 1;
}

/* is_end_with */
int is_end_with(const char *str1, char *str2)
{
  if(str1 == NULL || str2 == NULL)
    return -1;
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  if((len1 < len2) || (len1 == 0 || len2 == 0))
    return -1;
  while(len2 >= 1)
  {
    if(str2[len2 - 1] != str1[len1 - 1])
      return 0;
    len2--;
    len1--;
  }
  return 1;
}
#endif

// End of file : am_utils.h 2023-3-18 17:15:31 by: zhaoning 

