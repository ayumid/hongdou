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
#include "sdk_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_sim_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_sim_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_sim_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_SIM_STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_sim_task_stack = NULL;

static OSTaskRef sample_sim_task_ref = NULL;


OSATimerRef sample_sim_task_delete_timer_ref = NULL;

static void sample_sim_task(void *ptr);

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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
    SDK_SET_HOSCFG_CONFIG(1,1);    // 在这个接口中初始化热插拔配置，启用热插拔，高电平检测
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
    
    sample_sim_task_stack = malloc(SAMPLE_SIM_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_sim_task_ref, sample_sim_task_stack, SAMPLE_SIM_STACK_SIZE, 100, "sample_sim_task", sample_sim_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void sample_sim_task_del_by_timer(UINT32 id)
{
    OSA_STATUS status;

    if(sample_sim_task_ref)
    {
        status = OSATaskDelete(sample_sim_task_ref);
        ASSERT(status == OS_SUCCESS);
        sample_sim_task_ref = NULL;
       
        if(sample_sim_task_stack)
        {
            free(sample_sim_task_stack);
            sample_sim_task_stack = NULL;
        }
    }

    OSATimerDelete (sample_sim_task_delete_timer_ref);
}


static void sample_sim_task(void *ptr)
{
    int count = 500;
    unsigned int current_os_ticks;
    
    BOOL ret;
    CiSimPinState simStatus;
    char sim_imsi[32] = {0};
    char sim_iccid[32] = {0};

    
    while(count--){
        
        current_os_ticks = OSAGetTicks();    // 获取系统tick ,一个tick 5ms，即开机时间    
        sample_sim_uart_printf("this is a test task current_os_ticks %d\n", current_os_ticks);
        OSATaskSleep(200*3);        // 如果可能存在while空跑的case，一定需要加上个sleep，不然可能会造成系统调度异常
        
        simStatus = SDK_GetSimStatus();
        sample_sim_uart_printf("SDK_GetSimStatus simStatus %d\n", simStatus);
        if (simStatus == CI_SIM_PIN_ST_READY){
            
            memset(sim_imsi, 0x00, sizeof(sim_imsi));
            ret = SDK_GetSimImsi(0, (uint8_t *)sim_imsi, sizeof(sim_imsi));
            sample_sim_uart_printf("SDK_GetSimImsi ret %d, sim_imsi: %s\n", ret, sim_imsi);
            
            memset(sim_iccid, 0x00, sizeof(sim_iccid));
            ret = SDK_GetSimIccid(0, (uint8_t *)sim_iccid, sizeof(sim_iccid));
            sample_sim_uart_printf("SDK_GetSimIccid ret %d, sim_iccid: %s\n", ret, sim_iccid);            
        }
    }
    
    OSATimerCreate(&sample_sim_task_delete_timer_ref);
    OSATimerStart(sample_sim_task_delete_timer_ref, 400, 0, sample_sim_task_del_by_timer, 0);
}

// End of file : main.c 2023-5-17 11:27:12 by: zhaoning 

