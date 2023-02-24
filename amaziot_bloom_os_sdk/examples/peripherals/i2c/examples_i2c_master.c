//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_i2c_master.c
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
#include "drv_i2c_sht20.h"
#include "drv_i2c_jhm1200.h"


#ifdef INCL_EXAMPLES_I2C
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
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
	
	
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

}
static void _task(void *ptr)
{

#if 0
    float temprature;
	float humidity;
	drv_sht20_init();
	while(1) {
		temprature = drv_sht20_read_temp();
		humidity = drv_sht20_read_hum();
		sdk_uart_printf("temprature:%f,humidity:%f\n",temprature,humidity);
		sleep(5);
    }
#else
	double pressure;
	drv_jhm1200_init();
	while(1) {
		pressure = drv_jhm1200_get_pressure();;
		sdk_uart_printf("pressure:%f\n",pressure);
		sleep(5);
    }
#endif 
}

#endif /* ifdef INCL_EXAMPLES_I2C.2021-11-25 11:58:53 by: win */

// End of file : examples_i2c_master.h 2021-11-25 11:58:03 by: win 

