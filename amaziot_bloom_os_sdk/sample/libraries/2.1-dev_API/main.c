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

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_dev_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_dev_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_dev_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_DEV_STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_dev_task_stack = NULL;

static OSTaskRef sample_dev_task_ref = NULL;


OSATimerRef sample_dev_task_delete_timer_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_dev_task(void *ptr);

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

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
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
    
    sample_dev_task_stack = malloc(SAMPLE_DEV_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_dev_task_ref, sample_dev_task_stack, SAMPLE_DEV_STACK_SIZE, 100, "sample_dev_task", sample_dev_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void sample_task_del_by_timer(UINT32 id)
{
    OSA_STATUS status;

    if(sample_dev_task_ref)
    {
        status = OSATaskDelete(sample_dev_task_ref);
        ASSERT(status == OS_SUCCESS);
        sample_dev_task_ref = NULL;
       
        if(sample_dev_task_stack)
        {
            free(sample_dev_task_stack);
            sample_dev_task_stack = NULL;
        }
    }

    OSATimerDelete (sample_dev_task_delete_timer_ref);
}


static void sample_dev_task(void *ptr)
{
    int count = 5;
    unsigned int current_os_ticks;
    char imei_str[32] = {0};
    char sn_str[32] = {0};
    BOOL ret;
    
    while(count--)
    {
        
        current_os_ticks = OSAGetTicks();    // 获取系统tick ,一个tick 5ms，即开机时间    
        sample_dev_uart_printf("this is a test task current_os_ticks %d\n", current_os_ticks);
        OSATaskSleep(200);        // 如果可能存在while空跑的case，一定需要加上个sleep，不然可能会造成系统调度异常
        
        memset(imei_str, 0x00, sizeof(imei_str));
        ret = SDK_GetDevImei(imei_str, sizeof(imei_str));
        sample_dev_uart_printf("SDK_GetDevImei ret %d, imei: %s\n", ret, imei_str);
        
        memset(sn_str, 0x00, sizeof(sn_str));
        ret = SDK_GetDevSn(sn_str, sizeof(sn_str));
        sample_dev_uart_printf("SDK_GetDevSn ret %d, SN: %s\n", ret, sn_str);
    }
    
    OSATimerCreate(&sample_dev_task_delete_timer_ref);
    OSATimerStart(sample_dev_task_delete_timer_ref, 400, 0, sample_task_del_by_timer, 0);
}

// End of file : main.c 2023-5-17 11:27:07 by: zhaoning 

