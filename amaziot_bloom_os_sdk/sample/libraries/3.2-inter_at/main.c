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

#include "inter_at.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_interat_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_interat_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_iat_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_IAT_STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_iat_task_stack = NULL;

static OSTaskRef sample_iat_task_ref = NULL;

OSATimerRef sample_iat_task_delete_timer_ref = NULL;

static void sample_iat_task(void *ptr);

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
    
    sample_iat_task_stack = malloc(SAMPLE_IAT_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_iat_task_ref, sample_iat_task_stack, SAMPLE_IAT_STACK_SIZE, 100, "sample_iat_task", sample_iat_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void sample_sim_task_del_by_timer(UINT32 id)
{
    OSA_STATUS status;

    if(sample_iat_task_ref)
    {
        status = OSATaskDelete(sample_iat_task_ref);
        ASSERT(status == OS_SUCCESS);
        sample_iat_task_ref = NULL;
       
        if(sample_iat_task_stack)
        {
            free(sample_iat_task_stack);
            sample_iat_task_stack = NULL;
        }
    }

    OSATimerDelete (sample_iat_task_delete_timer_ref);
}

//detect net status, enable or disable
static void wait_dev_reg_net(void)
{
    int count = 0;

    while(!getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){

        OSATaskSleep(200);
        count++;
        if (count > 150){
            PM812_SW_RESET();
        }
    }
}

static void sample_iat_task(void *ptr)
{
    char sim_imei[32] = {0};
    char sim_imsi[32] = {0};
    char sim_iccid[32] = {0};
    char sim_cell[512] = {0};
    int rssi = 0;
    LIB_CELL_INFO_T cell = {0};
    
    wait_dev_reg_net();
    
    while(1)
    {
        lib_get_imei(sim_imei);
        sample_interat_uart_printf("%s[%d] imei:%s", __FUNCTION__, __LINE__, sim_imei);
        lib_get_imsi(sim_imsi);
        sample_interat_uart_printf("%s[%d] imsi:%s", __FUNCTION__, __LINE__, sim_imsi);
        lib_get_iccid(sim_iccid);
        sample_interat_uart_printf("%s[%d] iccid:%s", __FUNCTION__, __LINE__, sim_iccid);
        rssi = lib_get_rssi();
        sample_interat_uart_printf("%s[%d] rssi:%d", __FUNCTION__, __LINE__, rssi);
        //获取rsrp rsrq sinr
        lib_get_cell_info(&cell);
        sample_interat_uart_printf("rsrp:%d, rsrq:%d, sinr:%d", 
                        cell.rsrp,
                        cell.rsrq,
                        cell.sinr);
                        
        break;
    }
    
    OSATimerCreate(&sample_iat_task_delete_timer_ref);
    OSATimerStart(sample_iat_task_delete_timer_ref, 400, 0, sample_sim_task_del_by_timer, 0);
}

// End of file : main.c 2023-5-17 11:27:12 by: zhaoning 

