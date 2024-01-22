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
#include "FDI_TYPE.h"
#include "FDI_FILE.h"

#include "am.h"
#include "teldef.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_simsw_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_simsw_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

static OSATimerRef sample_file_timer_ref = NULL;
static OSTaskRef sample_file_task_ref = NULL;
static OSFlagRef sample_file_flag_ref = NULL;
static int sample_file_count = 0;
static void sample_file_timer_callback(UINT32 param);
static void sample_file_task(void *ptr);

#define SAMPLE_FILE_TASK_STACK_SIZE     2048
static UINT32 sample_file_task_stack[SAMPLE_FILE_TASK_STACK_SIZE/sizeof(UINT32)];
#define SAMPLE_FILE_TASK_TIMER_CHANGE_FLAG_BIT    0x01
#define SAMPLE_FILE_TASK_TIMER_CHANGE2_FLAG_BIT    0x02

#ifndef DTU_TYPE_EXSIM
extern UINT32 SDK_INIT_SIM_SELECT_VALUE;
#endif

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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
#ifndef DTU_TYPE_EXSIM
    //使用SIM2
    SDK_INIT_SIM_SELECT_VALUE = 2;
#endif
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
    sample_simsw_uart_printf("debug> enter main\n");
    
    ret = OSAFlagCreate(&sample_file_flag_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&sample_file_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATaskCreate(&sample_file_task_ref, sample_file_task_stack, SAMPLE_FILE_TASK_STACK_SIZE, 200, "filetest-task", sample_file_task, NULL);
    ASSERT(ret == OS_SUCCESS);
    OSATimerStart(sample_file_timer_ref, 10 * 200, 10 * 200, sample_file_timer_callback, 0); // 3 seconds timer

    //调用位于am_file.c文件中的初始化文件API，这里会将默认值写入文件，文件不存在，会首先创建文件
    dtu_trans_conf_file_init();
    //初始化sim卡
    dtu_trans_sim_init();
}

static void sample_file_timer_callback(UINT32 param)
{
    OSAFlagSet(sample_file_flag_ref, SAMPLE_FILE_TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);

}

static void sample_file_task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value = 0;
    UINT32 flag_mask = SAMPLE_FILE_TASK_TIMER_CHANGE_FLAG_BIT | SAMPLE_FILE_TASK_TIMER_CHANGE2_FLAG_BIT;

    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();

    while(1) {
        status = OSAFlagWait(sample_file_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);

        //定时器超时，模拟无法驻网，切卡后复位，下次上电，会使用另一张卡
        dtu_trans_sim_switch();
        //获取当前卡号
        char mmRspBuf[36] = {0};
        int err = 0;
        char *p = NULL;
        int errorCnt = 0;//
        wait_reg:
        memset(mmRspBuf, 0, sizeof(mmRspBuf));
        err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+ICCID\r\n", 2, "+ICCID", 1, NULL, mmRspBuf, sizeof(mmRspBuf));

        if(NULL != mmRspBuf)
        {
            sample_simsw_uart_printf("%s", mmRspBuf);
            //模拟重启使用新卡驻网
            PM812_SW_RESET();
        }
    }
}

// End of file : main.h 2023-2-23 14:31:52 by: zhaoning 

