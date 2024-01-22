//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main_2.c
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
#include "ql_adc_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_ADC_TASK_STACK_SIZE     1024*5

#define SAMPLE_ADC_TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------

static UINT8 _task_stack[SAMPLE_ADC_TASK_STACK_SIZE];

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

// Public variables -------------------------------------------------------------

static void _timer_callback(UINT32 tmrId);
static void _task_flag(void *ptr);

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
    
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, SAMPLE_ADC_TASK_STACK_SIZE, 120, "_task_flag", _task_flag, NULL);
    ASSERT(ret == OS_SUCCESS);

    catstudio_printf("Phase2Inits_exit OSATimerStart\n");
    
    OSATimerStart(_timer_ref, 10 * 200, 10 * 200, _timer_callback, 0); // 10 seconds timer
}

static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, SAMPLE_ADC_TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void _task_flag(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = SAMPLE_ADC_TASK_TIMER_CHANGE_FLAG_BIT;
    int count = 0;
    unsigned short adc0Value;
    unsigned short adc1Value;
    int ret;

    ret = ql_adc_init();        // add 4K task stack size for adc init
    catstudio_printf("ql_adc_init %d\n", ret);
    
    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        
        if (status == OS_SUCCESS){
            if (flag_value & SAMPLE_ADC_TASK_TIMER_CHANGE_FLAG_BIT) {        
            
                ret = ql_adc_read(ADC_CHANNEL_ADC0, &adc0Value);
//                ret = ql_adc_read(ADC_CHANNEL_ADC1, &adc1Value);
                
                catstudio_printf("_task: count: %d, ret %d, adcvaule %u,%u, source_vol: %u\n", count++, ret, adc0Value, adc1Value, ql_get_cur_source_vol()/* KV7 can read the source vol == adc1*/);                        
            }
        }
    }
}

// End of file : main_2.c 2023-5-17 9:22:45 by: zhaoning 

