//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-15
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-15
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
#define sample_mutexes_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_mutexes_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_mutexes_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_MUTEX_STACK_1_SIZE            0x2000
#define SAMPLE_MUTEX_STACK_2_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_mutexes_task_1_stack = NULL;
static void* sample_mutexes_task_2_stack = NULL;

static OSTaskRef sample_mutexes_task_1_ref = NULL;
static OSTaskRef sample_mutexes_task_2_ref = NULL;

static OSMutexRef sample_mutexes_mutex_ref = NULL;

static OSATimerRef sample_mutexes_task_1_delete_timer_ref = NULL;

static char sample_mutexes_data_buf[1024] = {0};
static int sample_mutexes_data_buf_len = 0;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

void sample_mutexes_init(void);
void sample_mutexes_lock(void);
void sample_mutexes_unlock(void);

static void sample_mutexes_task_1(void *ptr);
static void sample_mutexes_task_2(void *ptr);

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
    OSA_STATUS status;

    sample_mutexes_task_1_stack = malloc(SAMPLE_MUTEX_STACK_1_SIZE);
    sample_mutexes_task_2_stack = malloc(SAMPLE_MUTEX_STACK_2_SIZE);
    
    sample_mutexes_init();

    status = OSATaskCreate(&sample_mutexes_task_1_ref, sample_mutexes_task_1_stack, SAMPLE_MUTEX_STACK_1_SIZE, 100, "sample_mutexes_task_1", sample_mutexes_task_1, NULL);
    ASSERT(status == OS_SUCCESS);
    
    status = OSATaskCreate(&sample_mutexes_task_2_ref, sample_mutexes_task_2_stack, SAMPLE_MUTEX_STACK_2_SIZE, 101, "sample_mutexes_task_2", sample_mutexes_task_2, NULL);
    ASSERT(status == OS_SUCCESS);    
}

int sample_mutex_write_to_buffer(char* buffer, int len)
{
    int i;
    int writeLen = 0;

    sample_mutexes_lock();
    if ((sample_mutexes_data_buf_len + len) > 1024)
    {
        sample_mutexes_uart_printf("Buffer overflow!\n");
        writeLen = 0;
    }
    else
    {
        writeLen = len;
    }
    
    for(i = 0; i < writeLen ; i++)
    {
        sample_mutexes_data_buf[sample_mutexes_data_buf_len++] = buffer[i];
    }    
    sample_mutexes_unlock();

    return writeLen;    
}

int sample_mutex_read_to_buffer(char* buffer, int len)
{
    int i;
    int readLen = 0;

    sample_mutexes_lock();
    readLen = (len >= sample_mutexes_data_buf_len) ? sample_mutexes_data_buf_len : len;

    for(i = 0; i < readLen ; i++)
    {
        buffer[i] = sample_mutexes_data_buf[i];
    }    
    
    sample_mutexes_data_buf_len = sample_mutexes_data_buf_len - readLen;

    if (sample_mutexes_data_buf_len > 0)
    {
        for(i = 0; i < sample_mutexes_data_buf_len ; i++){
            sample_mutexes_data_buf[i] = sample_mutexes_data_buf[readLen + i];
        }
    }
    sample_mutexes_unlock();

    return readLen;
}

static void sample_mutexes_task_1(void *ptr)
{
    int ret;

    char testBuffer[128] = {0};
    while(1)
    {
        memset(testBuffer, 0x41, sizeof(testBuffer));
        ret = sample_mutex_write_to_buffer(testBuffer, 128);
        sample_mutexes_uart_printf("sample_mutex_write_to_buffer ret %d!\n", ret);
        OSATaskSleep(50);        // 如果可能存在while空跑的case，一定需要加上个sleep，不然可能会造成系统调度异常
    }
}

static void sample_mutexes_task_2(void *ptr)
{
    int ret;
    char testBuffer[256] = {0};

    while(1)
    {
        memset(testBuffer, 0x00, sizeof(testBuffer));
        ret = sample_mutex_read_to_buffer(testBuffer, 256);
        sample_mutexes_uart_printf("sample_mutex_read_to_buffer ret %d, %s!\n", ret, testBuffer);
        OSATaskSleep(50);        // 如果可能存在while空跑的case，一定需要加上个sleep，不然可能会造成系统调度异常
    }
}

void sample_mutexes_init(void)
{
    OSA_STATUS status;
    status = OSAMutexCreate(&sample_mutexes_mutex_ref, OS_PRIORITY);
    ASSERT(status == OS_SUCCESS);
}

void sample_mutexes_lock(void)
{
    OSA_STATUS status;
    status = OSAMutexLock(sample_mutexes_mutex_ref, OSA_SUSPEND);
    ASSERT(status == OS_SUCCESS);
}

void sample_mutexes_unlock(void)
{
    OSA_STATUS status;
    status = OSAMutexUnlock(sample_mutexes_mutex_ref);
    ASSERT(status == OS_SUCCESS);
}
// End of file : main.c 2023-5-15 14:43:46 by: zhaoning 
