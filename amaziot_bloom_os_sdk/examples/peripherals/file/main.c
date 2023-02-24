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

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

static OSATimerRef _timer_ref = NULL;
static OSTaskRef _task_ref = NULL;
static OSFlagRef _flag_ref = NULL;
static int count = 0;
static void _timer_callback(UINT32 param);
static void _task(void *ptr);

#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
#define TASK_TIMER_CHANGE_FLAG_BIT    0x01
#define TASK_TIMER_CHANGE2_FLAG_BIT    0x02

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
    sdk_uart_printf("debug> enter main\n");
    
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "filetest-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
    OSATimerStart(_timer_ref, 10 * 200, 10 * 200, _timer_callback, 0); // 3 seconds timer
    
}
static void _timer_callback(UINT32 param)
{
    sdk_uart_printf("debug> count = %d",++count);
    int temp;
    temp = count %2 +1;

    if (temp & 1) {
        OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
    } else {
        OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE2_FLAG_BIT, OSA_FLAG_OR);
    }
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT | TASK_TIMER_CHANGE2_FLAG_BIT;
    int fd = 0;
    char temp[30];

    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        sdk_uart_printf("debug> count = %d",count);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            fd = FDI_fopen("test_file", "wb");
            if (fd != 0){
                sdk_uart_printf("debug> open file for write OK\n");
                memset(temp, 0, 30);
                sprintf(temp,"Hello world! _%d",count);
                FDI_fwrite(temp, 20, 1, fd);
                FDI_fclose(fd);
            } else {
                sdk_uart_printf("debug> open file for write error\n");
            }
        } else if (flag_value & TASK_TIMER_CHANGE2_FLAG_BIT) {
            fd = FDI_fopen("test_file", "rb");
            if (fd) {
                sdk_uart_printf("debug> open file for read OK\n");
                memset(temp,0,30);
                FDI_fread(temp,30,1,fd);
                FDI_fclose(fd);
                sdk_uart_printf("debug> temp = %s", temp);
            } else {
                sdk_uart_printf("debug> open file for read error\n");
            }
        }
    }
}

// End of file : main.h 2023-2-23 14:31:52 by: zhaoning 

