//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_modbus.c
// Auther      : win
// Version     :
// Date : 2021-12-22
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-22
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "UART.h"
#include "sys.h"
#include "sdk_api.h"
#include "incl_config.h"
#include "utils_common.h"
#include "mb_host.h"

#ifdef INCL_EXAMPLES_MODBUS
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1280
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void modbus_task(void);
static void send_thread(void);

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
	
    lib_mbh_init();
	
	ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "modbus_task", modbus_task, NULL);
	ASSERT(ret == OS_SUCCESS);

	sys_thread_new("send_thread", send_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 80);
}
static void mbh_exec(unsigned char *data,unsigned char len)
{
	unsigned char datalen=len-2;
	switch(data[1])//cmd
	{
		case 1:
			mbh_hook_rec01(data[0],(data+2),datalen);
			break;
		case 2:
			mbh_hook_rec02(data[0],(data+2),datalen);
			break;
		case 3:
			mbh_hook_rec03(data[0],(data+2),datalen);
			break;
		case 4:
			mbh_hook_rec04(data[0],(data+2),datalen);
			break;
		case 5:
			mbh_hook_rec05(data[0],(data+2),datalen);
			break;
		case 6:
			mbh_hook_rec06(data[0],(data+2),datalen);
			break;
		case 15:
			mbh_hook_rec15(data[0],(data+2),datalen);
			break;
		case 16:
			mbh_hook_rec16(data[0],(data+2),datalen);
			break;
	}
}
void mbh_hook_rec01(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec01");
}
void mbh_hook_rec02(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec02");
}
void mbh_hook_rec03(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec03");
}
void mbh_hook_rec04(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec04");
}
void mbh_hook_rec05(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec05");
}
void mbh_hook_rec06(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec06");
}
void mbh_hook_rec15(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec15");
}
void mbh_hook_rec16(uint8_t add,uint8_t *data,uint8_t datalen)
{
	sdk_uart_printf("asr_test  mbh_hook_rec16");
}

static void modbus_task(void)
{
    UART_BaudRates baud;
	baud = UART_GET_BAUD();
	sdk_uart_printf("%s: baud %d\n", __FUNCTION__, baud);
	
    while (1) {
       lib_mbh_poll(mbh_exec);
	   msleep(2);
    }
}
static void send_thread(void)
{
	UINT8 mb_data[4]={0x00,0x01,0x00,0x01};
   
    while (1) {
	    sleep(3);
        lib_mbh_send(0x01,0x03,mb_data,4);
	  
    }
}
#endif /* ifdef INCL_EXAMPLES_MODBUS.2021-12-22 11:59:25 by: win */

// End of file : examples_modbus.h 2021-12-22 11:49:47 by: win 

