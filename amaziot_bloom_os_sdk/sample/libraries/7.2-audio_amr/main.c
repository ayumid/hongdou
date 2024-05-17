#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "osa.h"
#include "UART.h"
#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "sys.h"
#include "UART.h"
#include "teldef.h"
#include "sdk_api.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define sleep(x) OSATaskSleep((x) * 200)//second

static OSMsgQRef commonIndMsgQ = NULL;
static OSFlagRef amrRecordFlagRef = NULL;

typedef struct{    
    int len;
    UINT8 *UArgs;
}commonIndParam;


static OSTaskRef commonInd_task_ref = NULL;
#define COMMONIND_TASK_STACK_SIZE     1024*4
static void* commonInd_task_stack = NULL;


static OSTaskRef voice_sms_task_ref = NULL;
#define VOICE_SMS_TASK_STACK_SIZE     1024*4
static void* voice_sms_task_stack = NULL;

static void commonInd_thread(void * argv);
static void voice_sms_thread(void * argv);

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

extern BOOL IsAtCmdSrvReady(void);
extern BOOL isMasterSim0(void);

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

    status = OSAFlagCreate(&amrRecordFlagRef);
    ASSERT(status == OS_SUCCESS);

    catstudio_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&commonIndMsgQ, "commonIndMsgQ", sizeof(commonIndParam), 300, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);

    commonInd_task_stack = malloc(COMMONIND_TASK_STACK_SIZE);
    ASSERT(commonInd_task_stack != NULL);

	status = OSATaskCreate(&commonInd_task_ref, commonInd_task_stack, COMMONIND_TASK_STACK_SIZE, 150, "commonInd_thread", commonInd_thread, NULL);
    ASSERT(status == OS_SUCCESS);	

    voice_sms_task_stack = malloc(VOICE_SMS_TASK_STACK_SIZE);
    ASSERT(voice_sms_task_stack != NULL);
	
	status = OSATaskCreate(&voice_sms_task_ref, voice_sms_task_stack, VOICE_SMS_TASK_STACK_SIZE, 151, "voice_sms_thread", voice_sms_thread, NULL);
    ASSERT(status == OS_SUCCESS);		
}

void commonIndRecvCallback(UINT8 * recv_data, UINT32 recv_len)
{	
	commonIndParam commonInd_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);

    commonInd_data.UArgs = (UINT8 *)tempbuf;
    commonInd_data.len = recv_len;
	
    osa_status = OSAMsgQSend(commonIndMsgQ, sizeof(commonIndParam), (UINT8*)&commonInd_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

#define AMR_RECORD_START    0x01
#define AMR_RECORD_STOP	    0x02

void amr_record_event_send(int event)
{
	if (amrRecordFlagRef != NULL){		
	    OSAFlagSet(amrRecordFlagRef, event, OSA_FLAG_OR);	
	}
}

static void commonInd_thread(void * argv)
{
    commonIndParam commonInd_temp;	
    OSA_STATUS status;
    int rcv = 0;    

	int ret;
		
    while (1) {
        memset(&commonInd_temp, 0, sizeof(commonIndParam));
        
        status = OSAMsgQRecv(commonIndMsgQ, (UINT8 *)&commonInd_temp, sizeof(commonIndParam), OSA_SUSPEND);
        
        if (status == OS_SUCCESS) {
            if (commonInd_temp.UArgs) {
                catstudio_printf("%s[%d]: commonInd_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, commonInd_temp.len, (char *)(commonInd_temp.UArgs));
								
				// read voice call and sms URC
				
				if (strstr((char *)commonInd_temp.UArgs, "^CONN:")){
					
					amr_record_event_send(AMR_RECORD_START);
				}
				
				if (strstr((char *)commonInd_temp.UArgs, "^CEND:")){
					
					amr_record_event_send(AMR_RECORD_STOP);
				}
				
				free(commonInd_temp.UArgs);
            }
        }
    }
}

#include "amr_vocoder_api.h"
#include "acm_audio_record.h"

int amr_record_start(void) 
{
	int rc = 0;
	AmrEncConfigInfo config = { 0 };
	config.mode = AUDIO_RECORD_MODE_MIXED; // record tx pcm to amr
	config.callback = NULL;

	rc = amrEncStart("test.amr", &config, 0);
	if (rc) {
		catstudio_printf("%s, %d, rc:%d", rc, __FUNCTION__, __LINE__);
	}
	else {
		catstudio_printf("%s, %d, rc:%d", rc, __FUNCTION__, __LINE__);
	}
	return 0;
}

int amr_record_stop(void) 
{
	amrEncStopWithName("test.amr");
	return 0;
}

int amr_paly_start(void) 
{
	int rc = 0;
	AmrFileConfigInfo config = { 0 };
	AUDIO_PLAY_OPTION play_option = { 0 };
	play_option.dest_end = 0x0;	//dest play end,0->near,1->far,2->both
	config.option = play_option.option;
	rc = amrPlayStart("test.amr", &config, 0);
	if (rc) {
		catstudio_printf("%s, %d, rc:%d", rc, __FUNCTION__, __LINE__);
	}
	else {
		catstudio_printf("%s, %d, rc:%d", rc, __FUNCTION__, __LINE__);
	}
	return 0;
}



static void voice_sms_thread(void * argv)
{
	int ret;
	OSA_STATUS status;
	UINT32 flag_value;
    UINT32 flag_mask = AMR_RECORD_START|AMR_RECORD_STOP;
	char at_str[128]={'\0'};
	char resp_str[128]={'\0'};
    int ready=0;
    UINT32 osaTick = 0;
	UINT32 freeSize = 0;
	osaTick = OSAGetTicks();	// 获取系统tick
    catstudio_printf("%s: resp_str = %s, osaTick = %ld\n",__FUNCTION__,resp_str,osaTick);
		
	while(!IsAtCmdSrvReady())
	{
		OSATaskSleep(100);
	}
	
    while (!ready){

		memset(at_str, 0x00, sizeof(at_str));
		memset(resp_str, 0x00, sizeof(resp_str));
		
		sprintf(at_str, "AT^SYSINFO\r");	
		if (isMasterSim0()){
			ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12, at_str, 3, "^SYSINFO:",1,NULL, resp_str, sizeof(resp_str));
		}else{
			ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12+36, at_str, 3, "^SYSINFO:",1,NULL, resp_str, sizeof(resp_str));
		}
        catstudio_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
		if(strstr(resp_str, "^SYSINFO: 2,3") != NULL || strstr(resp_str, "^SYSINFO: 2,2") != NULL){
			ready = 1;
		}
		OSATaskSleep(100);
    }
		
	osaTick = OSAGetTicks();
    catstudio_printf("%s: resp_str = %s, osaTick = %ld\n",__FUNCTION__,resp_str,osaTick);

	ready = 0;		
	while (!ready){

		memset(at_str, 0x00, sizeof(at_str));
		memset(resp_str, 0x00, sizeof(resp_str));
		
		sprintf(at_str, "AT+CIREG?\r");		//查询IMS注册状态，，语音相关AT，建议使用TEL_AT_CMD_ATP_12 通道
		if (isMasterSim0()){
			ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12, at_str, 3, "+CIREG:",1,NULL, resp_str, sizeof(resp_str));
		}else{
			ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12+36, at_str, 3, "+CIREG:",1,NULL, resp_str, sizeof(resp_str));
		}
		catstudio_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
		if(strstr(resp_str, "+CIREG: 1,1") != NULL ){			
			ready = 1;
		}
		OSATaskSleep(100);
	}

	sdk_register_common_ind_cb(commonIndRecvCallback);
		
	memset(at_str, 0x00, sizeof(at_str));
	memset(resp_str, 0x00, sizeof(resp_str));	
	sprintf(at_str, "ATD10086;\r");	
	if (isMasterSim0()){
		ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
	}else{
		ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12+36, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
	}
	catstudio_printf("%s: #1# resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
	
	extern unsigned int FDI_GetFreeSpaceSize(void);
	extern void AudioHAL_AifUseInternalCodec(void);
	
	AudioHAL_AifUseInternalCodec();			// 模拟语音
	//AudioHAL_AifUseSSPA();					// PCM语音
	while(1){
		status = OSAFlagWait(amrRecordFlagRef, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		
		if (status == OS_SUCCESS){
			if (flag_value & AMR_RECORD_START) {				
				freeSize = FDI_GetFreeSpaceSize();
				
				catstudio_printf("_task: freeSize: %d\n", freeSize);
				catstudio_printf("AMR_RECORD_START\n");
				amr_record_start();
			}else if (flag_value & AMR_RECORD_STOP) {				
				
				catstudio_printf("AMR_RECORD_STOP\n");
				amr_record_stop();
				OSATaskSleep(100);
				amr_paly_start();
			}
		}
	}
}



