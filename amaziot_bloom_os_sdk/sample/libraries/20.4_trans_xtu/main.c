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

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_trans_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_trans_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

static OSATimerRef sample_file_timer_ref = NULL;
static OSTaskRef sample_file_task_ref = NULL;
static OSFlagRef sample_file_flag_ref = NULL;
static int sample_file_count = 0;
static void sample_file_timer_callback(UINT32 param);
static void sample_file_task(void *ptr);

#define SAMPLE_FILE_TASK_STACK_SIZE     1280
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
    sample_trans_uart_printf("debug> enter main\n");

    //初始化串口
    dtu_trans_uart_init();
    //调用位于am_file.c文件中的初始化文件API，这里会将默认值写入文件，文件不存在，会首先创建文件
    dtu_trans_task_init();
}

// End of file : main.h 2023-2-23 14:31:52 by: zhaoning 

