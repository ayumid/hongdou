//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2024-5-21
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-21
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
#include "sdk_api.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define _STACK_SIZE            0x2000
static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;

#define COM_STACK_SIZE            0x2000

typedef struct{    
    int len;
    UINT8 *UArgs;
}commonIndParam;

// Private variables ------------------------------------------------------------

static void* com_task_stack = NULL;
static OSTaskRef com_task_ref = NULL;
static OSMsgQRef commonIndMsgQ = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void _task(void *ptr);
static void com_task(void *ptr);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

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

extern int SendATCMDWaitResp(int sATPInd, char *inStr, int timeOut, char *okFmt, int okFlag, char *errFmt, char *outStr, int outLen);

extern void sdk_register_common_ind_cb(commonCallback cb);

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
    
    _task_stack = malloc(_STACK_SIZE);
    ASSERT(_task_stack != NULL);
    
    ret = OSATaskCreate(&_task_ref, _task_stack, _STACK_SIZE, 100, "_task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
            
    ret = OSAMsgQCreate(&commonIndMsgQ, "commonIndMsgQ", sizeof(commonIndParam), 300, OS_FIFO);
    ASSERT(ret == OS_SUCCESS);    
    
    com_task_stack = malloc(COM_STACK_SIZE);
    ASSERT(com_task_stack != NULL);
    
    ret = OSATaskCreate(&com_task_ref, com_task_stack, COM_STACK_SIZE, 100, "com_task", com_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}


static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){
            ready = 1;
        }
        catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}

int sdk_send_at_wiat_resp(char *inStr, int timeOut, char *okFmt, int okFlag, char *errFmt, char *outStr, int outLen)
{
    int ret;
    memset(outStr, 0x00, sizeof(outLen));
    
    if (isMasterSim0()){
        ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12, inStr, timeOut, okFmt, okFlag, errFmt, outStr, outLen);
    }else{
        ret = SendATCMDWaitResp(TEL_AT_CMD_ATP_12+36, inStr, timeOut, okFmt, okFlag, errFmt, outStr, outLen);
    }    
    
    return ret;
}

static void _task(void *ptr)
{    
    int ret;
    char resp_str[128]={'\0'};

    int ims_ready = 0;
    
    wait_network_ready();

    
    // +CIREG?
    while (!ims_ready){
        ret = sdk_send_at_wiat_resp("AT+CIREG?\r\n", 3, "+CIREG:", 1, NULL, resp_str, sizeof(resp_str));
        catstudio_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
        if(strstr(resp_str, "+CIREG: 1,1") != NULL ){
            ims_ready = 1;
        }
        OSATaskSleep(200);
    }
    
    // ATD
    ret = sdk_send_at_wiat_resp("ATD15810331216;\r\n", 3, NULL, 1, NULL, resp_str, sizeof(resp_str));
    catstudio_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);
    
    OSATaskSleep(200*10);
    
    // AT+CHUP
    ret = sdk_send_at_wiat_resp("AT+CHUP\r\n", 3, NULL, 1, NULL, resp_str, sizeof(resp_str));
    catstudio_printf("%s: resp_str = %s, ret = %u\n",__FUNCTION__,resp_str,ret);        
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

static void com_task(void *ptr)
{
    int ret;
    int rcv = 0;    
    OSA_STATUS status;
    commonIndParam commonInd_temp;    
            
    sdk_register_common_ind_cb(commonIndRecvCallback);
    
    while (1) {
        memset(&commonInd_temp, 0, sizeof(commonIndParam));        
        status = OSAMsgQRecv(commonIndMsgQ, (UINT8 *)&commonInd_temp, sizeof(commonIndParam), OSA_SUSPEND);
        
        if (status == OS_SUCCESS) {
            if (commonInd_temp.UArgs) {
                catstudio_printf("%s[%d]: commonInd_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, commonInd_temp.len, (char *)(commonInd_temp.UArgs));
                                
                //volte URC: RING、^ORIG、^CONN、^CEND、+CLCC、+COLP
                
                if (strstr((char *)commonInd_temp.UArgs, "RING")
                    ||strstr((char *)commonInd_temp.UArgs, "^ORIG")
                    ||strstr((char *)commonInd_temp.UArgs, "^CONN")
                    ||strstr((char *)commonInd_temp.UArgs, "^CEND")
                    ||strstr((char *)commonInd_temp.UArgs, "+CLCC")
                    ||strstr((char *)commonInd_temp.UArgs, "+COLP")){
                        
                    catstudio_printf("volte URC: len:%d, data:%s\n", commonInd_temp.len, (char *)(commonInd_temp.UArgs));    
                }
                
                free(commonInd_temp.UArgs);
            }
        }
    }
}

// End of file : main.c 2024-5-21 10:15:22 by: zhaoning 
