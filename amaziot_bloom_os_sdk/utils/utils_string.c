//------------------------------------------------------------------------------
// Company     : Copyright (c) 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : utils.c
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

// Includes ---------------------------------------------------------------------

#include "utils_string.h"

//#include "math.h"

// Private macros / types / typedef ---------------------------------------------

#define                 UTILS_LOCAL_TIMEZONE                   (8)

#define                 UTILS_NMEA_MAX_LEN                     (100)

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// functions prototypes -------------------------------------------------

//static void reverse(char *s);

// Functions --------------------------------------------------------------------
#if 0
/**
  * Function    : utils_utc_2_localtime
  * Description : UTC时间转为东八区时间
  * Input       : p_buffer    utc时间
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void utils_utc_2_localtime(UINT8* p_buffer)
{
    UINT8 local_year = *p_buffer;
    UINT8 local_month = *(p_buffer + 1);
    UINT8 local_day = *(p_buffer + 2);
    UINT8 local_hour = *(p_buffer + 3) + UTILS_LOCAL_TIMEZONE;//东八区 utc + 8
    UINT8 local_min = *(p_buffer + 4);
    UINT8 local_sec = *(p_buffer + 5);
    
    if (local_hour > 23)
    {
        local_hour -= 24;
        local_day++;
        if(local_month == 2)
        {
            if ((0 == local_year % 4 && 0!=local_year % 100) || 0 == local_year % 400)
            {
                if (local_day > 29)
                {
                    local_day = 1;
                    local_month++;
                }
            }
            else
            {
                if (local_day > 28)
                {
                    local_day = 1;
                    local_month++;
                }
            }
        }
        else if(local_month == 1 || local_month == 3 || local_month == 5 || local_month == 7 || local_month == 8 || local_month == 10 || local_month == 12)
        {
            if (local_day > 31)
            {
                local_day = 1;
                local_month++;
                if(local_month > 12)
                {
                    local_month = 1;
                    local_year++;
                }
            }
        }
        else if(local_month == 4 || local_month == 6 || local_month == 9 || local_month == 11)
        {
            if (local_day > 30)
            {
                local_day = 1;
                local_month++;
            }
        }
    }

     *p_buffer = local_year;
     *(p_buffer + 1) = local_month;
     *(p_buffer + 2) = local_day;
     *(p_buffer + 3) = local_hour;
     *(p_buffer + 4) = local_min;
     *(p_buffer + 5) = local_sec;
}
#endif
/**
  * Function    : utils_hex2ascii
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int utils_hex2ascii(UINT8 uc_hex)
{
    if( uc_hex <= 9 )
        return ( uc_hex + '0' );
    else if( uc_hex >= 0xA && uc_hex <= 0xF )
        return ( uc_hex + '7' );
    else
        return '0';
}

/**
  * Function    : utils_hex2ascii_str
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT16 utils_hex2ascii_str(char* p_str, UINT8* p_hex, UINT16 ul_len)
{
    unsigned int i;
    
    for( i = 0; i < ul_len; i++ )
    {
        *p_str++ = utils_hex2ascii( p_hex[i] >> 4 );
        *p_str++ = utils_hex2ascii( p_hex[i] & 0xF );
    }

    *p_str = '\0';//输出字符串加个结束符
    
    return ( i << 1 );
}

/**
  * Function    : utils_ascii2hex
  * Description : hex转字符串
  * Input       : uc_ascii    hex值
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int utils_ascii2hex(UINT8 uc_ascii)
{
    if(uc_ascii <= '9')
        return (uc_ascii - '0');
    else if(uc_ascii >= 'A' && uc_ascii <= 'F')
        return (uc_ascii - '7' );
    else if(uc_ascii >= 'a' && uc_ascii <= 'f')
        return (uc_ascii - 'W');
    else
        return -1;
}

/**
  * Function    : utils_ascii_str2hex
  * Description : 字符串转hex
  * Input       : mode    模式
  *               p_hex   待转换的hex首地址
                  p_str   转换后存储字符串的首地址
                  ulLen   长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 utils_ascii_str2hex(UINT8 mode, char* p_hex, const char* p_str, UINT32 ulLen )
{
    unsigned int i = 0, len;
    UINT8 temp = 0;
    char* phex = NULL;
    len = ulLen;
    
//    if((ulLen & 0x01) && (mode == ALIGN_MODE_SUFFIX))
//    {
//        *p_hex++ = utils_ascii2hex( p_str[0]);
//        p_str++;
//        len++;
//        i++;
//    }
    phex = p_hex;
    len = len >> 1;

    if(ASCIISTR2HEX_MODE_LITTLE_ENDIAN == mode)
    {
        for( ; i < len; i++)
        {
            *phex++ = (utils_ascii2hex(p_str[0]) << 4) + utils_ascii2hex( p_str[1]);
            p_str += 2;
        }
    }
    else if(ASCIISTR2HEX_MODE_BIG_ENDIAN == mode)
    {
        for( ; i < len; i++)
        {
            *phex++ = (utils_ascii2hex(p_str[1]) << 4) + utils_ascii2hex( p_str[0]);
            p_str += 2;
        }
    }
    else if(ASCIISTR2HEX_MODE_BIG_ENDIAN_16 == mode)
    {
        for( ; i < len; i++)
        {
            *phex++ = (utils_ascii2hex(p_str[0]) << 4) + utils_ascii2hex( p_str[1]);
            p_str += 2;
        }
        for(i = 0 ; i < len; )
        {
            temp = p_hex[i];
            p_hex[i] = p_hex[i + 1];
            p_hex[i + 1] = temp;
//            printf("p_hex[%d] = 0x%02X p_hex[%d] = 0x%02X\r\n", i, *(p_hex + i), i + 1, *(p_hex + i + 1));
            i = i + 2;
        }
    }
//    if((ulLen & 0x01) && (mode == ALIGN_MODE_PREFIX))
//    {
//        *p_hex++ = utils_ascii2hex( p_str[0]);
//        i++;
//    }

    return (i);
}
#if 1
/**
  * Function    : reverse
  * Description : 字符串倒序
  * Input       : s    待处理的字符串地址
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void reverse(char *s)
{  
    char temp;
    char *p = s;//p指向s的头部
    char *q = s;//q指向s的尾部
    while(*q)
        ++q;
    q--;
    
    while(q > p)
    {  
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }   
}

/**
  * Function    : utils_itoa
  * Description : 整形转为字符串类型
  * Input       : n        待转换的数字
  *               p_str    转换后存储区
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void utils_itoa(int n, char* p_str)
{
    int i = 0;   
    int ab_n = 0;
    char str[100] = {0};
    
    if(n < 0)
        ab_n = 0 - n;
    else
        ab_n = n;
    do//从各位开始变为字符，直到最高位，最后应该反转
    {  
        str[i++] = ab_n % 10 + '0';  
        ab_n = ab_n / 10;  
    }while(ab_n > 0);  
       
    str[i] = '\0';//最后加上字符串结束符
    
    reverse(str);

    if(n < 0)
        strcat(p_str, "-");
    strcat(p_str, str);
}

/**
  * Function    : utils_ftoa
  * Description : 浮点型转为字符串
  * Input       : p_str    转换后数据存储区首地址
  *               f_num    待转换的浮点型数
                  n        小数点后保留n位
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int utils_ftoa(char *p_str, float f_num, int n)
{
    int     sumI;
    float   sumF;
    int     sign = 0;
    int     temp;
    int     count = 0;

    char *p;
    char *pp;

    if(p_str == NULL) return -1;
    p = p_str;

    /*Is less than 0*/
    if(f_num < 0)
    {
        sign = 1;
        f_num = 0 - f_num;
    }

    sumI = (int)f_num;//sumI is the part of int
    sumF = f_num - sumI;//sumF is the part of float

    /*Int ===> String*/
    do
    {
        temp = sumI % 10;
        *(p_str++) = temp + '0';
    }while((sumI = sumI /10) != 0);

    /*******End*******/

    if(sign == 1)
    {
        *(p_str++) = '-';
    }

    pp = p_str;
    
    pp--;
    while(p < pp)
    {
        *p = *p + *pp;
        *pp = *p - *pp;
        *p = *p -*pp;
        p++;
        pp--;
    }

    *(p_str++) = '.';     //point

    /*Float ===> String*/
    do
    {
        temp = (int)(sumF*10);
        *(p_str++) = temp + '0';

        if((++count) == n)
            break;
    
        sumF = sumF*10 - temp;

    }while(!(sumF > -0.000001f && sumF < 0.000001f));

    *p_str ='\0';

    return 0;
}

/**
  * Function    : utils_insert_sort
  * Description : 插入排序
  * Input       : a    待排序数组
  *               n    数组长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void utils_insert_sort(UINT32 a[], int n)
{
    int i = 0;
    
    for(i = 1; i < n; i++)
    {
        if(a[i] < a[i - 1])
        {
            int j = i - 1;
            UINT32 x = a[i];

            while(j > -1 && x < a[j])
            {  
                a[j + 1] = a[j];
                j--;
            }
            
            a[j + 1] = x;
        }
    }
}

/**
  * Function    : utils_U_bubble_sort
  * Description : 冒泡排序
  * Input       : array    无符号数组
  *               size     数组长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void utils_U_bubble_sort(UINT32 *array, int size)
{
    int i = 0,j = 0;
    UINT32 tem = 0;
    
    for(i = 1; i < size; i++)  
    {
        for(j = i; j > 0 && array[j - 1] > array[j]; j--) 
        {   
            tem = array[j];
            array[j] = array[j - 1]; 
            array[j-1] = tem; 
        } 
    }
}

/**
  * Function    : utils_S_bubble_sort
  * Description : 冒泡排序
  * Input       : array    有符号数组
  *               size     数组长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void utils_S_bubble_sort(INT32 *array, int size)
{
    int i = 0,j = 0;
    INT32 tem = 0;
    
    for(i = 1; i < size; i++)  
    {
        for(j = i; j > 0 && array[j - 1] > array[j]; j--) 
        {   
            tem = array[j];
            array[j] = array[j - 1]; 
            array[j-1] = tem; 
        } 
    }
}
#endif
/**
  * Function    : app_nmea_cpy
  * Description : 从src中拷贝nmea语句到dest
  * Input       : src     gnss原始数据
  *               dest    目的地址
  *               nmea    拷贝的nmea语句标识
  *               next_nmea    下一个nmea语句标识
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 注意越界问题，src缓冲区一定要大
  **/
void utils_nmea_cpy(char* src, char* dest, char* nmea, char* next_nmea)
{
    char* p_src = NULL;
    char* p_nmea = NULL;
    char* p_end = NULL;
//    char buf[500] = {0};
    
    p_src = src;
    
    p_nmea = strstr(p_src, nmea);
    if(NULL != p_nmea)
    {
        if(NULL != next_nmea)
        {
            p_end = strstr(p_src, next_nmea);
            if(p_end != NULL)
                memcpy(dest, p_nmea, p_end - p_nmea);
//            utils_printf("[UTILS] len %d max %d ", p_end - p_nmea, strlen(dest));
        }
        else
        {
            p_end = memchr(p_nmea, '*', UTILS_NMEA_MAX_LEN);
            if(p_end != NULL)
                memcpy(dest, p_nmea, p_end + 3 - p_nmea);
//            utils_printf("[UTILS] len %d max %d ", p_end + 3 - p_nmea, sizeof(buf));
        }
    }
}

/**
  * Function    : utils_mktime
  * Description : 实现mktime
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 utils_mktime(t_rtc* time)
{
    if (0 >= (int) (time->tm_mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
        time->tm_mon += 12;      /* Puts Feb last since it has leap day */
        time->tm_year -= 1;
    }
    
    return (((
        (unsigned long) (time->tm_year/4 - time->tm_year/100 + time->tm_year/400 + 367*time->tm_mon/12 + time->tm_mday) +
        time->tm_year*365 - 719499
        )*24 + time->tm_hour /* now have hours */
        )*60 + time->tm_min /* now have minutes */
        )*60 + time->tm_sec; /* finally seconds */
}

/**
  * Function    : utils_utc8_2_timestamp
  * Description : 北京时间转换为时间戳
  * Input       : 
  *               
  * Output      : 时间戳
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 utils_utc8_2_timestamp(void)
{
    UINT32 time = 0;
    
    t_rtc st_tm = {0};
    
    SDK_GET_BEIJING_TIME(&st_tm);
    
    time = utils_mktime(&st_tm) - 3600 * 8;
    
//    utils_printf("[APP] timestamp:%ld %d-%d-%d %d:%d:%d", time, st.tm_year, st.tm_mon, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec);

    return time;
}

/**
  * Function    : utils_utc8_2_day_timestamp
  * Description : 获取当天时间戳
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 utils_utc8_2_day_timestamp(void)
{
    UINT32 time = 0;
    
    t_rtc st_tm = {0};
    
    SDK_GET_BEIJING_TIME(&st_tm);
    
    time = st_tm.tm_hour * 3600 + st_tm.tm_min * 60 + st_tm.tm_sec;
    
//    utils_printf("[APP] timestamp:%ld %d-%d-%d %d:%d:%d", time, st.tm_year, st.tm_mon, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec);

    return time;
}

#if 0
/**
  * Function    : utils_get_distance_between_A2B
  * Description : 根据a,b亮点的经纬度坐标获取a和b之间的距离
  * Input       : lata a点纬度
  *               loga a点经度
  *               latb b点纬度
  *               logb b点经度
  * Output      : 
  * Return      : distance 距离
  * Auther      : zhaoning
  * Others      : 
  **/
double utils_get_distance_between_A2B(double lata,double loga, double latb,double logb)
{
    double EARTH_RADIUS = 6371.0;
    double PI = 3.14;
    double distance = 0.0;
    double lat_a = 0.0;
    double lat_b = 0.0;
    double log_a = 0.0;
    double log_b = 0.0;
    
    //转弧度
    lat_a = lata  * PI / 180;
    lat_b = latb  * PI / 180;
    log_a = loga  * PI / 180;
    log_b = logb  * PI / 180;
 
    double dis = cos(lat_b) * cos(lat_a) * cos(log_b -log_a) + sin(lat_a) * sin(lat_b);
    
    distance = EARTH_RADIUS * acos(dis);
    
    return distance;
}

/**
  * Function    : utils_haversin
  * Description : haversine公式
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static double utils_haversine(double theta)
{
    double v = sin(theta / 2);
    return v * v;
}

/**
  * Function    : utils_get_distance_between_A2B_by_haversine
  * Description : haversine公式,根据a,b亮点的经纬度坐标获取a和b之间的距离
  * Input       : lata a点纬度
  *               loga a点经度
  *               latb b点纬度
  *               logb b点经度
  * Output      : 
  * Return      : distance 距离
  * Auther      : zhaoning
  * Others      : 声明，这种办法在距离超过几千公里后开始不准，一万公里测试大概误差几百公里（visual studio c#准确）
  **/
double utils_get_distance_between_A2B_by_haversine(double lata,double loga, double latb,double logb)
{
    double EARTH_RADIUS = 6371.0;
    double PI = 3.14;
    double distance = 0;
    double lat_a = 0.0;
    double lat_b = 0.0;
    double log_a = 0.0;
    double log_b = 0.0;
    //用haversine公式计算球面两点间的距离。
    //转弧度
    lat_a = lata  * PI / 180;
    lat_b = latb  * PI / 180;
    log_a = loga  * PI / 180;
    log_b = logb  * PI / 180;
 
    //差值
    double vLon = fabs(log_a - log_b);
    double vLat = fabs(lat_a - lat_b);
 
    //h is the great circle distance in radians, great circle就是一个球体上的切面，它的圆心即是球心的一个周长最大的圆。
    double h = utils_haversine(vLat) + cos(lat_a) * cos(lat_b) * utils_haversine(vLon);
 
    distance = 2 * EARTH_RADIUS * sin(sqrt(h));
 
    return distance;
}
#endif
// End of file : utils.c 2021-7-19 9:58:50 by: zhaoning 


