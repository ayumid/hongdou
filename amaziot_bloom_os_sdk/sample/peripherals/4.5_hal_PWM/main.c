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
#include "ql_pwm_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_pwm_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_pwm_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_pwm_sleep(x) OSATaskSleep((x) * 200)//second

#define PWM_STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* sample_pwm_task_stack = NULL;

static OSTaskRef sample_pwm_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_pwm_task(void *ptr);

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
    
    sample_pwm_task_stack = malloc(PWM_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_pwm_task_ref, sample_pwm_task_stack, PWM_STACK_SIZE, 100, "sample_pwm_task", sample_pwm_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

#define PWM_SLAVE_ADDR      0x13  

static void sample_pwm_task(void *ptr)
{
    
    OSATaskSleep(10*200);
    
    sample_pwm_uart_printf("sample_pwm_task");

    int gpio_num = 126;//PWM1
    //80ms/100ms
    int high_one_cycle_duration = 80; 
    int total_one_cycle_duration = 100;  
    ql_pwm_config(gpio_num,high_one_cycle_duration,total_one_cycle_duration);    //只能产生500hz以内 pwm
    ql_pwm_enable(gpio_num);

    int gpio_num1 = 9;//PWM2
    //70ms/100ms
    int high_one_cycle_duration1 = 70; 
    int total_one_cycle_duration1 = 100;  
    ql_pwm_config_plus(gpio_num1,QL_PWM_CYCLE_ABOVE_1MS,high_one_cycle_duration1,total_one_cycle_duration1);//产生1khz以内 pwm
    ql_pwm_enable(gpio_num1);

//    int gpio_num2 = 10;//PWM3
//    //300us/1000us
//    int high_one_cycle_duration2 = 30; 
//    int total_one_cycle_duration2 = 100;  
//    //ql_pwm_config_plus(gpio_num2,QL_PWM_CYCLE_ABOVE_10US,high_one_cycle_duration2,total_one_cycle_duration2);//产生1k-20khz pwm
//    ql_pwm_config_plus(gpio_num2,QL_PWM_CYCLE_ABOVE_10US,high_one_cycle_duration2,total_one_cycle_duration2);//产生1k-20khz pwm
//    ql_pwm_enable(gpio_num2);
//    
//    int i;
//    int gpio_num3 = 11;//PWM4
//    //50us/100us
//    int high_one_cycle_duration3 = 50; 
//    int total_one_cycle_duration3 = 100;     
//    ql_pwm_config_plus(gpio_num3,QL_PWM_CYCLE_ABOVE_1US,high_one_cycle_duration3,total_one_cycle_duration3);//产生10k-200khz pwm
//    ql_pwm_enable(gpio_num3);
    
    while(1) 
    {
        sample_pwm_uart_printf("pwm test is running!!!\n");
        OSATaskSleep(5*200);
        
//        for(i=0;i<=100;i++)
//        {
//            high_one_cycle_duration3=i;    
//            ql_pwm_config_plus(gpio_num3,QL_PWM_CYCLE_ABOVE_1US,high_one_cycle_duration3,total_one_cycle_duration3);
//            sample_pwm_uart_printf("high_one_cycle_duration3=%d\n",high_one_cycle_duration3);
//            
//            OSATaskSleep(100);
//        }
    }
        
}

// End of file : main.c 2023-5-17 9:22:35 by: zhaoning 

