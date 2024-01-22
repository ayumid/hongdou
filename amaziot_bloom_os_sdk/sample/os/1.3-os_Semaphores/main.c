#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_sem_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_sem_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_sem_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_SEM_STACK_1_SIZE            0x2000
#define SAMPLE_SEM_STACK_2_SIZE            0x2000
static void* sample_sem_task_1_stack = NULL;
static void* sample_sem_task_2_stack = NULL;

static OSTaskRef sample_sem_task_1_ref = NULL;
static OSTaskRef sample_sem_task_2_ref = NULL;

static OSSemaRef sample_sem_ref = NULL;

static OSATimerRef sample_sem_task_1_delete_timer_ref = NULL;

static void sample_sem_task_1(void *ptr);
static void sample_sem_task_2(void *ptr);

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
    OSA_STATUS status;

    sample_sem_task_1_stack = malloc(SAMPLE_SEM_STACK_1_SIZE);
    sample_sem_task_2_stack = malloc(SAMPLE_SEM_STACK_2_SIZE);
    
    status = OSASemaphoreCreate (&sample_sem_ref, 1, OSA_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSATaskCreate(&sample_sem_task_1_ref, sample_sem_task_1_stack, SAMPLE_SEM_STACK_1_SIZE, 100, "sample_sem_task_1", sample_sem_task_1, NULL);
    ASSERT(status == OS_SUCCESS);
    
    status = OSATaskCreate(&sample_sem_task_2_ref, sample_sem_task_2_stack, SAMPLE_SEM_STACK_2_SIZE, 101, "sample_sem_task_2", sample_sem_task_2, NULL);
    ASSERT(status == OS_SUCCESS);    
}

void sample_sem_task_1_del_by_timer(UINT32 id)
{
    OSA_STATUS status;

    if(sample_sem_task_1_ref)
    {
        status = OSATaskDelete(sample_sem_task_1_ref);
        ASSERT(status == OS_SUCCESS);
        sample_sem_task_1_ref = NULL;
       
        if(sample_sem_task_1_stack)
        {
            free(sample_sem_task_1_stack);
            sample_sem_task_1_stack = NULL;
        }
    }

    OSATimerDelete (sample_sem_task_1_delete_timer_ref);
}


static void sample_sem_task_1(void *ptr)
{
    int count = 1;
    OSA_STATUS status;
    
    while(count++ < 100)
    {
    
        sample_sem_uart_printf("**this is a sample_sem_task_1 count %d\n", count);
        if (count == 20)
        {

            status = OSASemaphoreAcquire(sample_sem_ref, OS_SUSPEND);
            ASSERT(status == OS_SUCCESS);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            OSATaskSleep(500);
            sample_sem_uart_printf("**this is a sample_sem_task_1 count %d**\n", count);
            status = OSASemaphoreRelease(sample_sem_ref);
            ASSERT(status == OS_SUCCESS);            
        }
        OSATaskSleep(500);        // 如果可能存在while空跑的case，一定需要加上个sleep，不然可能会造成系统调度异常
    }
    
    OSATimerCreate(&sample_sem_task_1_delete_timer_ref);
    OSATimerStart(sample_sem_task_1_delete_timer_ref, 400, 0, sample_sem_task_1_del_by_timer, 0);
}


static void sample_sem_task_2(void *ptr)
{
    int count = 1;
    OSA_STATUS status;
    
    while(count++)
    {
        status = OSASemaphoreAcquire(sample_sem_ref, OS_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        sample_sem_uart_printf("##this is a sample_sem_task_2 count %d\n", count);
        OSATaskSleep(500);
        status = OSASemaphoreRelease(sample_sem_ref);
        ASSERT(status == OS_SUCCESS);
    }
}