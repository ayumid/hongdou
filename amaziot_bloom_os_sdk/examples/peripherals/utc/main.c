//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-2-23
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-2-23
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

extern BOOL IsAtCmdSrvReady(void);

extern int SendATCMDWaitResp(int sATPInd,char *in_str, int timeout, char *ok_fmt, int ok_flag,
                            char *err_fmt, char *out_str, int resplen);

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define _TASK_STACK_SIZE     1024*4
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

static void _timer_callback(UINT32 tmrId);
static void _task(void *ptr);

extern void update_the_cp_ver(char *cp_ver);    // max length 128
// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

extern void SDK_DISABLE_UART_DEBUG_INFO(int disable);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
    SDK_DISABLE_UART_DEBUG_INFO(1);
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    int ret;
    
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    catstudio_printf("Phase2Inits_exit OSATimerStart\n");

    OSATimerStart(_timer_ref, 50 * 200, 60 * 60 * 200, _timer_callback, 0); // 10 seconds timer
}




static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}


static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;
    UINT32 timeTick;
    
    int result;
    char at_str[128]={'\0'};
    char resp_str[128]={'\0'};
    int ready=0;
    int ret;
    t_rtc utcTime;
    int timeZone;
    
    while(!IsAtCmdSrvReady())
    {
        OSATaskSleep(100);
    }

    sprintf(at_str, "AT^SYSINFO\r");
    while (!ready){
        OSATaskSleep(100);
        ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, "^SYSINFO:",1,NULL, resp_str, sizeof(resp_str));
        catstudio_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
        if(strstr(resp_str, "^SYSINFO: 2,3") != NULL || strstr(resp_str, "^SYSINFO: 2,2") != NULL){
            ready = 1;
        }
    }
    
    
    OSATaskSleep(200*10);        //开机系统会同步一次时间
    
    timeTick = OSAGetTicks();    // 获取开机时间，单位ticks(5ms)
    catstudio_printf("timeTicks: %ld\n", timeTick);
    
    
    SDK_GET_BEIJING_TIME(&utcTime);    // 获取UTC时间，单位ticks(5ms)
    catstudio_printf("BJ TIME: \"%02d/%02d/%02d,%02d:%02d:%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec);

    SDK_GET_UTC_TIME(&utcTime, &timeZone);
    catstudio_printf("UTC TIME: \"%02d/%02d/%02d,%02d:%02d:%02d%c%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec, timeZone > 0? '+' : '-', ABS(timeZone)/15);

    utcTime.tm_year = 2020;
    utcTime.tm_mon = 5;
    utcTime.tm_mday = 8;
    utcTime.tm_hour = 12;
    utcTime.tm_min = 12;
    utcTime.tm_sec = 12;
    
    timeZone = 32;  // +8  //8*4=32
    SDK_SET_UTC_TIME(utcTime, timeZone);
    
    SDK_GET_UTC_TIME(&utcTime, &timeZone);
    catstudio_printf("UTC TIME: \"%02d/%02d/%02d,%02d:%02d:%02d%c%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec, timeZone > 0? '+' : '-', ABS(timeZone)/15);
    
    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            static int count = 0;
            count++;
            catstudio_printf("_task: count: %d\n", count);
            
            switch(count)
            {                
                case 1:
                    SDK_GET_BEIJING_TIME(&utcTime);
                    catstudio_printf("BJ TIME: \"%02d/%02d/%02d,%02d:%02d:%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec);

                    SDK_GET_UTC_TIME(&utcTime, &timeZone);
                    catstudio_printf("UTC TIME: \"%02d/%02d/%02d,%02d:%02d:%02d%c%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec, timeZone > 0? '+' : '-', ABS(timeZone)/15);
                    
                    result = SDK_NTP_UPDATE_TIME_WAIT_RESULT("ntp.aliyun.com", 500);    /* ntp校准时间，设置500ms超时，如果超时将不更新本地时间，长时间运营状态下，可通过ntp来校验时间 */
                    catstudio_printf("ntp %s: result %d\n", "ntp.aliyun.com", result);
                    
                    SDK_GET_BEIJING_TIME(&utcTime);
                    catstudio_printf("BJ TIME: \"%02d/%02d/%02d,%02d:%02d:%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec);

                    SDK_GET_UTC_TIME(&utcTime, &timeZone);
                    catstudio_printf("UTC TIME: \"%02d/%02d/%02d,%02d:%02d:%02d%c%02d\"", (utcTime.tm_year)%100, utcTime.tm_mon, utcTime.tm_mday, utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec, timeZone > 0? '+' : '-', ABS(timeZone)/15);
                                        
                    break;

                default:
                    count = 0;
                    break;
            }            
        }
    }
}

// End of file : main.h 2023-2-23 10:57:53 by: zhaoning 

