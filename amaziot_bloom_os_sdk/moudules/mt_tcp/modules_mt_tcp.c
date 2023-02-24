//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : modules_mt_tcp.c
// Auther      : win
// Version     :
// Date : 2021-12-9
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-9
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "lib_socket.h"
#include "drv_timer.h"
#include "lib_net.h"

#include "am_at_cmds.h"
#include "am_file.h"
#include "am_gpio.h"
#include "am_tcptrans.h"


#ifdef INCL_MOUDULES_MT_TCP
// Private defines / typedefs ---------------------------------------------------
extern trans_conf transconf;//weihu yi ge save in flash
extern uint8_t serial_mode;
extern OSMsgQRef MsgUartData_sdk;
extern socketParam sockGroup;

// Private variables ------------------------------------------------------------
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

static OSATimerRef _timer_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
static void wait_dev_reg_net(void);
static void wait_dev_check_csq(void);
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
    OSA_STATUS status;

	//init transfile
	trans_conf_file_init();

	//init gpio
	init_gpio_conf();   

	//init at uart
    init_at_uart_conf();

	//init socket 
	init_socket_conf();

	send_string_2uart("AMAZIOT AP4000MT_430E\r\n");

	//heart timer
	status = OSATimerCreate(&_timer_ref);
    ASSERT(status == OS_SUCCESS);

	//main thread
	status = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 150, "test-task", main_thread, NULL);
    ASSERT(status == OS_SUCCESS);

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
static void wait_dev_check_csq(void)
{
    char mmRspBuf[100] = {0};
    int  err;
	char *p;
	int csqValue;
	int errorCnt=0;//dmh
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 3, "+CSQ", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==> %s", mmRspBuf);

	if(strlen(mmRspBuf) >=10 ){
		p = strchr(mmRspBuf,':');
		csqValue = atoi(p+1);
	    printf("dmhtest:csq=%d",csqValue);
        if((csqValue>=transconf.yzcsq) && (csqValue != 99))
		{
		  errorCnt = 0;
		  return 0;
		}
    }
    else{
        sleep(2);
		errorCnt++;
		if(errorCnt>20){
			//reset
			send_to_uart("\r\nCheck Csq Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
		}	
        goto wait_reg;
    } 
}
/*thread for main*/
static void main_thread(void * argv)
{

	wait_dev_reg_net();
   	wait_dev_check_csq();

PRO_START:
    
    setSockCfn();
	//Open LinkA
	GpioSetLevel(GPIO_LINK_PIN, 1);
	//heart timer
	if(transconf.heartflag)
		OSATimerStart(_timer_ref, transconf.hearttime * 200, transconf.hearttime * 200 , _timer_callback, 0);
	//link data
	if(transconf.linkflag == 1 || transconf.linkflag == 3){
		if(0 != socket_write(sockGroup.fd,(void *)transconf.link,strlen(transconf.link)))
			goto PRO_START;
	}        
	while (1) {

		if(sockGroup.fd){
	        sleep(1);
		}else
	    {
			printf(" %s -- %u, restart socket", __FUNCTION__,__LINE__); 
			//Close LinkA
			GpioSetLevel(GPIO_LINK_PIN, 0);
			send_to_uart("\r\nSOCKET RESTART\r\n", 18);
            break;
		}
    }
    goto PRO_START;
}
static void _timer_callback(UINT32 tmrId)
{
	printf("asr_test  _timer_callback");
    if(serial_mode == DATA_MODE)
    {
    	if(sockGroup.fd && transconf.heartflag == 1){
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
#endif /* ifdef INCL_MOUDULES_MT_TCP.2022-1-25 16:08:48 by: win */

// End of file : modules_mt_tcp.h 2021-12-9 10:35:00 by: win 

