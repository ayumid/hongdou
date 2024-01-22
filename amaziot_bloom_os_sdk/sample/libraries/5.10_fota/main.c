//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
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
#include "osa_mem.h"
#include "sdk_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_fota_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_fota_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_fota_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_FOTA_STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_fota_task_stack = NULL;

static OSTaskRef sample_fota_task_ref = NULL;


OSATimerRef sample_fota_task_delete_timer_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_fota_task(void *ptr);

static char sample_fota_oem_sw_ver[32] = "OEM_VERSION_V1.0";

// Public functions prototypes --------------------------------------------------

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

extern void update_the_cp_ver(char *cp_ver);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
    //update_the_cp_ver(sample_fota_oem_sw_ver);    //可以使用AT+CPVER读取，部分客户需要提供版本给原厂生产，便于原厂识别版本号
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
    
    sample_fota_task_stack = malloc(SAMPLE_FOTA_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_fota_task_ref, sample_fota_task_stack, SAMPLE_FOTA_STACK_SIZE, 100, "sample_fota_task", sample_fota_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void DeleteTaskByTimer(UINT32 id)
{
    OSA_STATUS status;

    if(sample_fota_task_ref)
    {
        status = OSATaskDelete(sample_fota_task_ref);
        ASSERT(status == OS_SUCCESS);
        sample_fota_task_ref = NULL;
       
        if(sample_fota_task_stack)
        {
            free(sample_fota_task_stack);
            sample_fota_task_stack = NULL;
        }
    }

    OSATimerDelete (sample_fota_task_delete_timer_ref);
}


static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){
            ready = 1;
        }
        sample_fota_catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}

void sample_fota_result_cbk(UINT32 result)
{
    sample_fota_catstudio_printf("sample_fota_result_cbk result: %d", result);    
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

static void sample_fota_task(void *ptr)
{    
    wait_network_ready();
    
    sample_fota_catstudio_printf("sample_fota_oem_sw_ver: %s", sample_fota_oem_sw_ver);
    
    if(strncasecmp((char *)sample_fota_oem_sw_ver, "OEM_VERSION_V1.0", strlen("OEM_VERSION_V1.0")) == 0){
        SDK_MYFOTA_UPDATE("http://110.42.233.58:9999/http/oem_sw_v1.1.bin", "", "", sample_fota_result_cbk, 20);
    }
    
    OSATimerCreate(&sample_fota_task_delete_timer_ref);
    OSATimerStart(sample_fota_task_delete_timer_ref, 400, 0, DeleteTaskByTimer, 0);
}

// End of file : main.c 2023-5-17 14:22:01 by: zhaoning 

