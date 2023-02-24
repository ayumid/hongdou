//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : modules_mt_mqtt.c
// Auther      : win
// Version     :
// Date : 2022-1-25
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2022-1-25
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "lib_mqtt.h"
#include "drv_timer.h"
#include "lib_net.h"

#include "am_at_cmds.h"
#include "am_file.h"
#include "am_gpio.h"
#include "am_tcptrans.h"

#ifdef INCL_MOUDULES_MT_MQTT

// Private defines / typedefs ---------------------------------------------------
extern trans_conf transconf;//weihu yi ge save in flash
extern uint8_t serial_mode;
extern OSMsgQRef MsgUartData_sdk;
extern UINT8 subFlag;
extern mqtt_client_t *client;

// Private variables ------------------------------------------------------------
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void wait_dev_reg_net(void);
static void main_thread(void * argv);
static void _timer_callback(UINT32 tmrId);

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

	//init transfile
	trans_conf_file_init();

	//init gpio
	init_gpio_conf();;  

	//init at uart
	init_at_uart_conf();
	
	//init mqtt
	init_mqtt_conf();
	
	send_string_2uart("AMAZIOT AP4000MT_430E_MQTT\r\n");
	
	//heart timer
    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

	//main task 	
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 150, "test-task", main_thread, NULL);
    ASSERT(ret == OS_SUCCESS);	
}

static void wait_dev_reg_net(void)
{
    char mmRspBuf[100] = {0};
    int  err;
	int errorCnt=0;//dmh

	while(!IsAtCmdSrvReady())
	{
		OSATaskSleep(100);
	}

	
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT^SYSINFO\r\n", 3, "^SYSINFO", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==> %s", mmRspBuf);

    if(strstr(mmRspBuf, "^SYSINFO: 2,3") != NULL || strstr(mmRspBuf, "^SYSINFO: 2,2") != NULL){
		errorCnt = 0;
        return 0;
    }else{
        sleep(3);
		errorCnt++;
		if(errorCnt>20){
			//reset
			send_to_uart("\r\nCheck Net Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
		}	
        goto wait_reg;
    } 
}
static void main_thread(void *ptr)
{


    int rc = 0,count=0;
    
	wait_dev_reg_net();
PRO_START:

	wait_init_mqtt();

	wait_connect_mqtt();

	send_to_uart("\r\nCONNECT OK\r\n", 14);

	GpioSetLevel(GPIO_LINK_PIN, 1);

	OSATaskSleep(100);//delay for sub error//dmh20210329
	
	mqtt_subscribe_topic();

	//heart timer
	if(transconf.heartflag)
		OSATimerStart(_timer_ref, transconf.hearttime * 200, transconf.hearttime * 200 , _timer_callback, 0);
	
	//link data
	if(transconf.linkflag == 1 || transconf.linkflag == 3){
		 mqtt_send((void *)transconf.link,strlen(transconf.link));
	}

    while (1)
    {
       if(MQTTIsConnected(client))
       {
       		OSATaskSleep(200);
			
	   }else{
			break;
	   }
    }
	subFlag=0;
	send_to_uart("\r\nMQTT RESTART\r\n", 16);
	GpioSetLevel(GPIO_LINK_PIN, 0);
	goto PRO_START;
	
    
}

/*function for timer heart*/
static void _timer_callback(UINT32 tmrId)
{
	printf("asr_test  _timer_callback");
    if(serial_mode == DATA_MODE)
    {
    	if(MQTTIsConnected(client) && transconf.heartflag==1){
	        printf("%s --- %d",__FUNCTION__,__LINE__);
	        
	        MsgUartDataParam_sdk heart = {0};
		    char *heartbuf = (char *)malloc(100);
			memset(heartbuf, 0x0, 100);
			sprintf(heartbuf, "%s",transconf.heart);
	        heart.len = strlen(heartbuf);
			heart.UArgs = (UINT8 *)heartbuf;
			OSAMsgQSend(MsgUartData_sdk, sizeof(MsgUartDataParam_sdk), (UINT8*)&heart, OSA_NO_SUSPEND);
			
    	}
		
    }
	
}

#endif /* ifdef INCL_MOUDULES_MT_MQTT.2022-1-25 17:07:56 by: win */

// End of file : modules_mt_mqtt.h 2022-1-25 17:07:19 by: win 

