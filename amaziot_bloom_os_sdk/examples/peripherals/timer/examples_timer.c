//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_timer.c
// Auther      : win
// Version     :
// Date : 2021-11-25
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-25
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "drv_timer.h"
#include "drv_gpio.h"

#ifdef INCL_EXAMPLES_TIMER
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

#define GPIO_TEST_PIN        2	//PIN 2

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void _timer_callback(UINT32 tmrId);
static void _task(void *ptr);
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
	GPIOReturnCode status = GPIORC_OK;
    
 	status = drv_gpio_set_direction(GPIO_TEST_PIN, GPIO_OUT_PIN);
	if (status != GPIORC_OK){
		sdk_uart_printf("status: 0x%lx", status);
	}	
	
    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = drv_timer_init(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    drv_timer_start(_timer_ref, 2 * 200, 2 * 200, _timer_callback, 0); // 2 seconds timer
}
static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;

    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            static int count = 0;
            count++;
            sdk_uart_printf("_task: timer\n");
            if (count & 0x01) {
                drv_gpio_write_pin(GPIO_TEST_PIN, 1);
            } else {
				drv_gpio_write_pin(GPIO_TEST_PIN, 0);
            }
        }
    }
}

#endif /* ifdef INCL_EXAMPLES_TIMER.2021-11-25 12:04:10 by: win */

// End of file : examples_timer.h 2021-11-25 12:03:41 by: win 

