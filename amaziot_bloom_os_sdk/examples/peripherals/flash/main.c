#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "sdk_api.h"
#include "Errors.h"


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
    catstudio_printf("debug> enter main\n");
    
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 200, "flash-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
    OSATimerStart(_timer_ref, 10 * 200, 10 * 200, _timer_callback, 0); // 10 seconds timer
    
}
static void _timer_callback(UINT32 param)
{
    catstudio_printf("debug> count = %d\n",++count);
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
    UINT32 startAddr = 0;
    UINT32 endAddr = 0;
    UINT32 readAddr = 0;
    UINT32 writeAddr = 0;
    char tempBuf[128] = {0};
    const char Buffer[] = "####################################################################################################";
    unsigned int ret = 0;
    
    startAddr = yuge_nor_custem_start_address();
    endAddr = yuge_nor_custem_end_address();
    
    catstudio_printf(" startAddr(%lX), endAddr(%lX)...\n", startAddr, endAddr);
    while(1) {
        memset(tempBuf, 0x0, sizeof(tempBuf));
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        catstudio_printf("debug> count = %d\n",count);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
                        
            ret = yuge_nor_do_write(startAddr, (unsigned char *)Buffer, 100);
            if (ret != NoError) {
                catstudio_printf(" Write flash failed(%d)...\n", ret);
                continue;
            }
            ret = yuge_nor_do_read(startAddr, (unsigned char *)tempBuf, 100);
            if (ret != NoError) {
                catstudio_printf(" Read flash failed(%d)...\n", ret);
                continue;
            }
            catstudio_printf("tempBuf[0] %X, tempBuf[99] %X\n", tempBuf[0], tempBuf[99]);

        } else if (flag_value & TASK_TIMER_CHANGE2_FLAG_BIT) {
            //ret = yuge_nor_do_erase(startAddr, YUGE_4K_SIZE);
            ret = yuge_nor_do_erase(startAddr, YUGE_64K_SIZE);
            if (ret != NoError) {
                catstudio_printf(" Erase flash failed..\n");
                continue;
            }
            ret = yuge_nor_do_read(startAddr, (unsigned char *)tempBuf, 100);
            if (ret != NoError) {
                catstudio_printf(" Read flash failed(%d)...\n", ret);
                continue;
            }
            catstudio_printf("tempBuf[0] %X, tempBuf[99] %X\n", tempBuf[0], tempBuf[99]);
        }
    }
}

