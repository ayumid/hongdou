/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : am_at_cmds.c
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include "osa.h"
#include "UART.h"
#include "sys.h"
#include "sockets.h"
#include "sdk_api.h"
#include "teldef.h"

#include "libhttpclient.h"
#include "cJSON.h"
#include "am_at_cmds.h"
#include "am_file.h"

#include "utils_common.h"
#include "am_tcptrans.h"

#ifdef INCL_MOUDULES_MT_TCP
extern trans_conf transconf;//weihu yi ge save in flash
extern OSMsgQRef	MsgUartData_sdk;
extern void set_UartDataSwitch_sdk(BOOL flag);

//socket 
extern socketParam sockGroup;

/***********************************************************************
*	LOCAL FUNCTION PROTOTYPES
***********************************************************************/
static int dtu_atcmd_setip(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setheartflag(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setheart(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setlinkflag(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setlink(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setyzcsq(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_help(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_version(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_reload(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_reset(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_seto(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_seto0(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcmdpw(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setsim(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setsimlock(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setati(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcgmi(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcgmm(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcgmr(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_ota(char *atName, char *atLine, AtCmdResponse *resp);


static int AT_TRANS_CmdFunc_SETIP(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);//dmh
static int AT_TRANS_CmdFunc_SETHEARTFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETHEART(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETLINKFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETLINK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Query(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Reset(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Reload(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Version(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETYZCSQ(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_O(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_CMDPW(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETSIM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETSIMLOCK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETATI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETCGMI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETCGMM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETCGMR(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_OTA(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);

typedef struct {
    unsigned char at_cmd_name[AT_CMD_NAME_MAX_LEN + 1];
    int (*proc_func)(char *, char *, AtCmdResponse *);
} ATCmdTable;

static ATCmdTable dtu_atcmd_table[] = {
    { "+TRANSIP",dtu_atcmd_setip},
	{ "+TRANSHEARTFLAG",dtu_atcmd_setheartflag},
	{ "+TRANSHEART",dtu_atcmd_setheart},
	{ "+TRANSLINKFLAG",dtu_atcmd_setlinkflag},
	{ "+TRANSLINK",dtu_atcmd_setlink},
	{ "+TRANSYZCSQ",dtu_atcmd_setyzcsq},
	{ "+HELP",dtu_atcmd_help},
	{ "+VERSION",dtu_atcmd_version},
	{ "+RELOAD",dtu_atcmd_reload},
	{ "+RESET",dtu_atcmd_reset},
	{ "O",dtu_atcmd_seto},
	{ "O0",dtu_atcmd_seto0},
	{ "+CMDPW",dtu_atcmd_setcmdpw},
	{ "+SIM",dtu_atcmd_setsim},
	{ "+SIMLOCK",dtu_atcmd_setsimlock},
	{ "I",dtu_atcmd_setati},
	{ "+CGMI",dtu_atcmd_setcgmi},
	{ "+CGMM",dtu_atcmd_setcgmm},
	{ "+CGMR",dtu_atcmd_setcgmr},
	{ "+OTA",dtu_atcmd_ota},
	// add by dmh end
};
/***********************************************************************
*	DEAL UART FOR AT FUNCTION PROTOTYPES
***********************************************************************/	
uint8_t serial_mode;	// 0:DATA_MODE 1:AT_MDOE
char at_version[100];	//define for at query romVersion

//begin for 32bit error
int tmp_count = 0;
char tmp[600] = {0};
//end for 32bit error

OSATimerRef _cachetimer_ref = NULL;
OSFlagRef _sendflag_ref = NULL;
#define SEND_MAX_BUFF 1024 //socket sendBuf length
unsigned int send_count = 0;
char sendBuf[4096]={0};



///dmh 20201125/////////////
struct http_data_s {
    unsigned data_sz;
    UINT8 data[100*1024];
};
static void wait_mcu_ota(char* url);
///dmh 20201125/////////////

/***********************************************************************
*	LOCAL FUNCTION PROTOTYPES
***********************************************************************/
static const uint8_t *at_ParamStrParse(AT_COMMAND_PARAM_T *param);
static const uint8_t *at_ParamStr(AT_COMMAND_PARAM_T *param, bool *paramok);
static uint32_t at_ParamUint(AT_COMMAND_PARAM_T *param, bool *paramok);
static uint32_t at_ParamUintInRange(AT_COMMAND_PARAM_T *param, uint32_t minval,uint32_t maxval, bool *paramok);
static int package_at_cmd(char *atName, char *atLine,AT_CMD_PARA *pParam);
static void free_pParam(AT_CMD_PARA *pParam);
static int process_at_cmd_line(char *cmdName, char *cmdLine);
//static void process_at_cmd_mode(const char *atCmdData, int data_len);
static void sendResponse(AtCmdResponse *response);
static void send_serial_data(MsgUartDataParam_sdk * uartData);
static void check_serial_mode(MsgUartDataParam_sdk *msgUartData);
static void handle_serial_data(MsgUartDataParam_sdk *uartData);
static void uartdata_thread(void);
static void _cachetimer_callback(UINT32 tmrId);
static int uarthandle(MsgUartDataParam_sdk * uartData);

/* This function send data to uart. */
void send_to_uart(char *toUart, int length)
{
    char *sendData = NULL;
    printf("%s[%d]:toUart=%s, length=%d\n", __FUNCTION__, __LINE__, toUart, length);
    sendData = (char *)malloc(length+1);
    ASSERT(sendData != NULL);
    memset(sendData, 0, length+1);

    memcpy(sendData, toUart, length);
    sendData[length] = '\0';
    
    printf("%s[%d]:send to uart data=%s, length=%d\n", __FUNCTION__, __LINE__, sendData, length);    
	UART_SEND_DATA((UINT8 *)sendData, length);
	printf("%s[%d]:sendData==NULL = %d\n", __FUNCTION__, __LINE__, (sendData==NULL));
    if (sendData)
        free(sendData);
}
/*function for init at uart*/
void init_at_uart_conf(void)
{
	 int ret;
	 ret = OSAMsgQCreate(&MsgUartData_sdk, "MsgUartData_sdk", sizeof(MsgUartDataParam_sdk), 500, OS_FIFO);
	 ASSERT(ret == OS_SUCCESS);
	 ret = OSATimerCreate(&_cachetimer_ref);
     ASSERT(ret == OS_SUCCESS);
	 ret = OSAFlagCreate(&_sendflag_ref);
     ASSERT(ret == OS_SUCCESS);
	 sys_thread_new("uartdata_thread", uartdata_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 80);
	
	
}
/*function for at uart thread*/
static void uartdata_thread(void)
{
    MsgUartDataParam_sdk uart_temp;
    OSA_STATUS status;
    
    set_UartDataSwitch_sdk(1);  // open pass-through
   
    while (1) {
        memset(&uart_temp, 0, sizeof(MsgUartDataParam_sdk));
        
        status = OSAMsgQRecv(MsgUartData_sdk, (UINT8 *)&uart_temp, sizeof(MsgUartDataParam_sdk), OSA_SUSPEND);
        
        if (status == OS_SUCCESS) {
            if (uart_temp.UArgs) {
                printf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                handle_serial_data(&uart_temp);
            }
        }
    }
}
/*function for cache timer*/
static void _cachetimer_callback(UINT32 tmrId)
{
	printf("asr_test  _cachetimer_callback");
	OSAFlagSet(_sendflag_ref, 0x01, OSA_FLAG_OR);
	OSATimerStop(_cachetimer_ref);
}
static int uarthandle(MsgUartDataParam_sdk * uartData)
{
    int result = 0;
    printf("uartData->UArgs = %s, uartData->len=%d",uartData->UArgs,uartData->len);

	if(send_count >= SEND_MAX_BUFF)
	{
		if(sockGroup.fd){

			///////////////////////////////////////////////////////////////////
			uint16_t send_size = SEND_MAX_BUFF;
			//add link len
			if(transconf.linkflag==2 || transconf.linkflag==3)
				send_size += strlen(transconf.link);
			char *data = malloc(send_size);
	        if (data == NULL)
	        {
	        	printf("dmhtest:malloc(send_size) error ,send_size:%d",send_size);
	            return;
	        }
	        memset(data, 0, send_size);
			//add link data
			if(send_size>SEND_MAX_BUFF){
				memcpy(data, transconf.link, strlen(transconf.link));
				memcpy(data+strlen(transconf.link), sendBuf, SEND_MAX_BUFF);
			}else
	        	memcpy(data, sendBuf, SEND_MAX_BUFF);
			send_count-=SEND_MAX_BUFF;
			result = socket_write(sockGroup.fd,(void *)data,send_size);	
			free(data);
			//////////////////////////////////////////////////////////////////
			//if(transconf.linkflag==2 || transconf.linkflag==3)//dmh test
			//	socket_write(sockGroup.fd,(void *)transconf.link,strlen(transconf.link));
			//result = socket_write(sockGroup.fd,(void *)sendBuf,SEND_MAX_BUFF);
			//send_count-=SEND_MAX_BUFF;
			
   		}
		else
        	result = -1;
	}
	else{
		//open huancun timer function
		OSATimerStart(_cachetimer_ref, 40, 40, _cachetimer_callback, 0); // 200 ms timer
	}
	
    printf("%s --- %d",__FUNCTION__,__LINE__);
    if(uartData->UArgs)
        free(uartData->UArgs);
    printf("%s --- %d",__FUNCTION__,__LINE__);
    return result;
}
/*function for data mode send data*/
static void send_serial_data(MsgUartDataParam_sdk * uartData)
{
	printf("%s[%d]: DATA_MODE recvdata: %s\n", __FUNCTION__, __LINE__, uartData->UArgs);
	if(sockGroup.fd){
		//close huancun timer
		OSATimerStop(_cachetimer_ref);
		//huan cun
		memcpy(&(sendBuf[send_count]),(char *)uartData->UArgs,uartData->len);//dmh 20200709
		send_count += uartData->len;
		if (0 != uarthandle(uartData))
		{
			if(sockGroup.fd){
				close(sockGroup.fd);
				sockGroup.fd = 0;
			}
		  
		}
		
	}
}
/*function for check serial moded*/
static void check_serial_mode(MsgUartDataParam_sdk *msgUartData)
{
    if (serial_mode == DATA_MODE) {
        if(msgUartData->len == 3 && memcmp(msgUartData->UArgs, "+++", 3) == 0) {
            serial_mode = AT_MODE;
			send_to_uart("\r\nOK\r\n", 6);
        }
    } 
	
}
/*function for handle_serial_data*/
static void handle_serial_data(MsgUartDataParam_sdk *uartData)
{
    check_serial_mode(uartData);//close judge serial mode

    if (serial_mode == DATA_MODE){
       send_serial_data(uartData);
	
    } 
	else if (serial_mode == AT_MODE) {
        printf("It time, serial_mode is AT_MODE!\n");
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
		{
			memset(tmp, 0, sizeof(tmp));
			tmp_count = 0;
		}
		else if(strstr(uartData->UArgs,"\r\n"))
		{
			sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
			printf("[atcmd_test]: send AT command uargs:%s,len:%d\n",tmp, strlen(tmp) );
			process_at_cmd_mode(tmp, strlen(tmp));
			tmp_count = 0;
		}
		else
		{
			sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
			tmp_count += uartData->len;//modify by dmh 2020-06-22
			if(tmp_count>512)
				tmp_count =0;
			
		}
    }
	
}

// =============================================================================
// at_ParamStrParse (param is already checked)
// =============================================================================
static const uint8_t *at_ParamStrParse(AT_COMMAND_PARAM_T *param)
{
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;

    // param->value must be started and ended with double-quote
    uint8_t *s = param->value + 1;
    uint8_t *d = param->value;
    uint16_t length = param->length - 2;
    param->length = 0;
    while (length > 0)
    {
        uint8_t c = *s++;
        length--;
        // It is more permissive than SPEC.
        if (c == '\\' && length >= 2 && isalnum(s[0]) && isalnum(s[1]))
        {
            *d++ = (HEX2NUM(s[0]) << 4) | HEX2NUM(s[1]);
            s += 2;
            length -= 2;
        }
        else
        {
            *d++ = c;
        }
        param->length++;
    }
    *d = '\0';
    param->type = AT_CMDPARAM_STRING_PARSED;
    return param->value;
}
// =============================================================================
// at_ParamStr
// =============================================================================
static const uint8_t *at_ParamStr(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;
    if (param->type == AT_CMDPARAM_STRING && param->length >= 2)
        return at_ParamStrParse(param);

failed:
    *paramok = false;
    return "";
}
// =============================================================================
// at_ParamUint
// =============================================================================
static uint32_t at_ParamUint(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;

    if (param->type == AT_CMDPARAM_NUMBER || param->type == AT_CMDPARAM_RAWTEXT)
    {
        char *endptr;
        unsigned long value = strtoul((const char *)param->value, &endptr, 10);
		if (*endptr != 0){
			printf("dmhtest00:endptr:%s ",endptr);//test
			goto failed;

		}

        return value;
    }

failed:
    *paramok = false;
    return 0;
}
// =============================================================================
// at_ParamUintInRange
// =============================================================================
static uint32_t at_ParamUintInRange(AT_COMMAND_PARAM_T *param, uint32_t minval,
                             uint32_t maxval, bool *paramok)
{
    uint32_t res = at_ParamUint(param, paramok);
	if (*paramok && res >= minval && res <= maxval)
        return res;

    *paramok = false;
    return minval;
}
// =============================================================================
// package_at_cmd
// =============================================================================
static int package_at_cmd(char *atName, char *atLine,AT_CMD_PARA *pParam)
{
	
	pParam->paramCount = 0;
	if(strlen(atLine) == strlen(atName)){
		printf("pParam->iType = AT_CMD_EXE");
		pParam->iType = AT_CMD_EXE;
		return 0;
	}
	if(*(atLine+strlen(atName))=='?'){
		printf("pParam->iType = AT_CMD_READ");
		if(strlen(atLine)-strlen(atName) == 1)
			pParam->iType = AT_CMD_READ;
		return 0;
	}
	if( (*(atLine+strlen(atName))=='=')&&( *(atLine+strlen(atName)+1)=='?')){
		printf("pParam->iType = AT_CMD_TEST");
		if(strlen(atLine)-strlen(atName) == 2)
			pParam->iType = AT_CMD_TEST;
		return 0;
	}
	printf("pParam->iType = AT_CMD_SET");
	pParam->iType = AT_CMD_SET;
	 char strParam[500]={0};
    UINT8 index=0;
    int step = 0;
	char cc;
	char *save_buf;
	char *buf_ptr;
	int data_len=strlen(atLine)-strlen(atName)-1;
	buf_ptr = atLine+strlen(atName)+1;
    save_buf = buf_ptr;
    while (data_len > 0) {
        cc = *buf_ptr++;
        data_len--;
        step++;
        if(cc == ','){
			printf("dmhtest00:%s","begin0");
 
 
			 
			 //0.get the strParam and judge
			 if(step<=1)
				 return -1;
			 memset(strParam,0,500);//dmh add 2020-12-08
			 memcpy(strParam, save_buf, step-1);
			 //printf("dmhtest:strParam:%s",strParam);//dmh test
 
			 //wait for check 
			 if(step > 3){
				 if(!(strParam[0] == '\"' && strParam[step-2] == '\"')){
					 //if begin "
					 if(strParam[0] == '\"'){
						 continue;
					 }
					 //if not all num
					 if(strspn(strParam, "0123456789")!=strlen(strParam))
						 return -1;
				 }
			 }else{
				 //if not all num
				 if(strspn(strParam, "0123456789")!=strlen(strParam))
						 return -1;
			 }
			 //printf("dmhtest:step:%d",step);//dmh test
			 //printf("dmhtest:buf_ptr:%s",buf_ptr);//dmh test
			 //printf("dmhtest:save_buf:%s",save_buf);//dmh test
			 //1.set pParam->paramCount
			 pParam->paramCount++;
			 //2.malloc pParam->param
			 pParam->params[index]=malloc(sizeof(AT_COMMAND_PARAM_T)+step*sizeof(uint8_t));
			 if(pParam->params[index] == NULL)
				 return -1;
			 
			 //3.set param->length
			 pParam->params[index]->length=step-1;
			 //4.set param-type
			 if(pParam->params[index]->length>2){
				 if(*save_buf == '\"' && *(save_buf+step-2)=='\"')
					 pParam->params[index]->type=AT_CMDPARAM_STRING;
				 else
					 pParam->params[index]->type=AT_CMDPARAM_NUMBER;
			 }else
				 pParam->params[index]->type=AT_CMDPARAM_NUMBER;
			 //5.set params->value
			 memcpy(pParam->params[index++]->value, strParam, step);
			 save_buf = buf_ptr;   
			 step = 0;
		 }
					
	 }
	 printf("dmhtest00:%s","begin1");
	 //1.set param
	 pParam->paramCount++;
	 //2.malloc pParam->param
	 pParam->params[index]=malloc(sizeof(AT_COMMAND_PARAM_T)+(step+1)*sizeof(uint8_t));
	 if(pParam->params[index] == NULL)
		 return -1;
	 //3.get the strParam and judge
	 if(step<1)
		 return -1;
	 memset(strParam,0,500);
	 memcpy(strParam, save_buf, step);
 
	 //wait for check 
	 if(step > 3){
		 if(!(strParam[0] == '\"' && strParam[step-1] == '\"')){
			 //if not all num
			 if(strspn(strParam, "0123456789")!=strlen(strParam))
				 return -1;
		 }
	 }else{
		 //if not all num
		 if(strspn(strParam, "0123456789")!=strlen(strParam))
				 return -1;
	 }
 
	 //4.set param->length
	 pParam->params[index]->length=step;
	 //5.set param-type
	 if(pParam->params[index]->length>2){
		 if(*save_buf == '\"' && *(save_buf+step-1)=='\"')
			 pParam->params[index]->type=AT_CMDPARAM_STRING;
		 else
			 pParam->params[index]->type=AT_CMDPARAM_NUMBER;
	 }else
		 pParam->params[index]->type=AT_CMDPARAM_NUMBER;
	 //6.set params->value
	 memcpy(pParam->params[index++]->value, strParam, step+1);
	 printf("dmhtest00:%s","end");
	 return 0;
 }

// =============================================================================
// free_pParam
// =============================================================================
static void free_pParam(AT_CMD_PARA *pParam)
{
	UINT8 i=0;
	for(i=0;i<pParam->paramCount;i++){
		if(pParam->params[i]!=NULL)
			free(pParam->params[i]);
	}
}
// =============================================================================
// sendResponse
// =============================================================================
static void sendResponse(AtCmdResponse *response)
{
    static const char *MSG_OK="\r\nOK\r\n";
    static const char *MSG_ERROR="\r\nERROR\r\n";
    char resp_buffer[MAX_LINE_SIZE]={0};
    const char *msg = NULL;
    int sz;
	if(serial_mode == DATA_MODE){
		if(sockGroup.fd){
			if (response->result == DSAT_NO_CARRIER)return;
			if (response->result == DSAT_OK) {
		        if (response->response && response->response[0]) {
					sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
		        }
		        msg = MSG_OK;
		    } else {
		        msg = MSG_ERROR;
		    }
			sprintf(resp_buffer + strlen(resp_buffer), "%s", msg);
			//judge is ATO cmd
			if(strcmp(response->response,"ATO")==0 || strcmp(response->response,"ATO0")==0)
				send_to_uart(resp_buffer, strlen(resp_buffer));
			else
				socket_write(sockGroup.fd,resp_buffer, strlen(resp_buffer));
		}
	}else{
	    if (response->result == DSAT_OK) {
	        if (response->response && response->response[0]) {
				sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
				send_to_uart((char *)resp_buffer, sz);
	        }
	        msg = MSG_OK;
	    } else {
	        msg = MSG_ERROR;
	    }
		send_to_uart(msg, strlen(msg));
	}
	

}
// =============================================================================
// process_at_cmd_line
// =============================================================================
static int process_at_cmd_line(char *cmdName, char *cmdLine)
{
    AtCmdResponse *response;
    int i;
    printf("dmhtest00:%s",cmdName);
	printf("dmhtest00:%s",cmdLine);
    response = malloc(sizeof(AtCmdResponse));
    if ( response != NULL ) {
        memset(response, 0x0, sizeof(AtCmdResponse));
        response->response = malloc(MAX_LINE_SIZE);
        response->response[0] = 0;
        response->result = DSAT_ERROR;
		response->istranscmd = DSAT_ERROR;

        ATCmdTable *atcmd_ptr = dtu_atcmd_table;
        int nCommands = sizeof(dtu_atcmd_table) / sizeof(dtu_atcmd_table[0]);
        for (i = 0; i < nCommands; i++, atcmd_ptr++) {
            if (strcasecmp((char *)atcmd_ptr->at_cmd_name, cmdName)==0) {
                response->result = DSAT_OK;
				response->istranscmd = DSAT_OK;
                if (atcmd_ptr->proc_func != NULL) {
                    response->result = atcmd_ptr->proc_func(cmdName, cmdLine, response);
                }
                break;
            }
        }
		//begin test by dmh
		
		if(response->istranscmd == DSAT_ERROR){
			char at_str[128]={'\0'};
			sprintf(at_str, "AT%s\r",cmdLine);
			char resp_str[512]={'\0'};
			response->result = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, NULL,1,NULL, resp_str, sizeof(resp_str));
			printf("dmhtest:resp_str:%s,%d",resp_str,strlen(resp_str));
			snprintf(response->response, strlen(resp_str)+1, "%s", resp_str);
		}
		
		//end test by dmh 
        sendResponse(response);

        if (response->response)
            free(response->response);
        free(response);
    }
    return 0;
}
// =============================================================================
// process_at_cmd_mode
// =============================================================================
static dsat_sio_info_s_type g_sio_info;
void process_at_cmd_mode(const char *atCmdData, int data_len)
{
    dsat_sio_info_s_type *sio_info_ptr = &g_sio_info;
    char *buf_ptr = NULL;
    char cc, atName[MAX_LINE_SIZE] = {0};
    int step = 0;

    buf_ptr = atCmdData;
    printf("%s[%d]: AT_MODE recv ATCMD: %s\n", __FUNCTION__, __LINE__, buf_ptr);
    //memset(tmp, 0, sizeof(tmp));//for 32bit error
    while (data_len > 0) {
        cc = *buf_ptr++;
        data_len--;
        
        switch (sio_info_ptr->at_cmd_prep_state) {
        case DSAT_CMD_PREP_STATE_HUNT:
            if ( UPCASE( cc ) == 'A' ) {
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_A;
            }
            break;

        case DSAT_CMD_PREP_STATE_FOUND_A:
            if ( UPCASE( cc ) == 'T' ) {
                sio_info_ptr->build_cmd_ptr = sio_info_ptr->cmd_line_buffer;
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_AT;
            } else if ( UPCASE( cc ) != 'A' ) {
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;

        case DSAT_CMD_PREP_STATE_FOUND_AT:
            step++;
            if (cc != '\r') {
                if (cc == '=' || cc == '?') {
                    printf("sio_info_ptr->cmd_line_buffer:%s\n", sio_info_ptr->cmd_line_buffer);
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step);
                    atName[step-1] = '\0';
                    printf("atName is %s\n", atName);
                }
                *sio_info_ptr->build_cmd_ptr++ = cc;
            } else {
                /*  EOL found, terminate and parse */
                *sio_info_ptr->build_cmd_ptr = '\0';

                if (!strlen(atName)) {
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step-1);
                    atName[step-1] = '\0';
                    printf("[%d]: atName is %s\n", __LINE__, atName);
                }

                printf("%s[%d]: cmd_line_buffer=%s\n", __FUNCTION__, __LINE__, sio_info_ptr->cmd_line_buffer);

                // T0D0: here should add AT command parse

                process_at_cmd_line(atName, sio_info_ptr->cmd_line_buffer);
                
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;			 

        default:
            sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            break;
        }
    }
	memset(tmp, 0, sizeof(tmp));//for 32bit error
}

////////////////////dmh 20201125//////////////////
static int _response_cb(char *buffer, int size, int nitems, void *private_data)
{
    struct http_data_s *client_data = private_data;
    if ((client_data->data_sz + size) < sizeof(client_data->data)) {
        memcpy(client_data->data + client_data->data_sz, buffer, size);
        client_data->data_sz += size;
        return 0;
    }
    return -1;
}
static void wait_mcu_ota(char* url)
{
    printf("ota task!");	
    //wait_dev_reg_net();
	struct http_client *client = NULL;
    struct http_data_s *client_data = NULL;
    int response_code = 0;

	char otaLength[50];
    
PRO_START:
    client_data = malloc(sizeof(*client_data));
    if (!client_data){
		printf("malloc client_data error!");
		send_to_uart("\r\nERROR\r\n", 9);
		goto clean;
	}
       
   
    client = http_client_init();
    if (!client){
		printf("http_client_init error!");
		send_to_uart("\r\nERROR\r\n", 9);
		goto clean;
	}
	
	memset(client_data, 0, sizeof(*client_data));//dmh 20201023
	
    http_client_setopt(client, HTTPCLIENT_OPT_URL, url);
    http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, _response_cb);
    http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, client_data);
    http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);
    http_client_perform(client);
    http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
    printf("[http_client_test]Get tcp state %d\n", response_code);

    if (response_code >= 200 && response_code < 300)
     {
         if(client_data->data_sz)
         {
            printf("\r\n ota_data_sz=%u", client_data->data_sz);
			
			sprintf(otaLength,"Content-Length: %d\r\n",client_data->data_sz);
			send_to_uart(otaLength, strlen(otaLength));
			
			if(client_data->data_sz>51200){
				send_to_uart(client_data->data, 51200);
				send_to_uart(client_data->data+51200, client_data->data_sz-51200);
			}else{
				send_to_uart(client_data->data, client_data->data_sz);
			}
         }
    }else if (response_code == 404) {
        printf("ota_response_code == %d\r\n%s",response_code ,client_data->data);
		send_to_uart("\r\nERROR\r\n", 9);
    }else{
		printf("ota_response_code == %d\r\n",response_code);
		send_to_uart("\r\nERROR\r\n", 9);
	}
        
   
clean:
	printf("clean test!");
    if (client)
        http_client_shutdown(client);
    if (client_data)
        free(client_data);
}
////////////////////dmh 20201125//////////////////

/******************************************************************************
 *   at functions
 ******************************************************************************/
/*function for ATCMD*/
static int dtu_atcmd_setip(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;

	//begin dmh test
	AT_CMD_PARA pParam;
	//UINT8 i=0;
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		/*
		*bianli AT_CMD_PARA
		printf("dmhtest-paramiType:%d",pParam.iType);
		printf("dmhtest-paramCount:%d",pParam.paramCount);
		for(i=0;i<pParam.paramCount;i++){
			printf("dmhtest-paramType:%d",pParam.params[i]->type);
			send_to_uart(pParam.params[i]->value,pParam.params[i]->length);
			send_to_uart("\r\n",2);
		}
		*/
		result = AT_TRANS_CmdFunc_SETIP(&pParam,atLine,resp);
		free_pParam(&pParam);
		
	}
	else{
		free_pParam(&pParam);
	}
	//end dmh test
	
	return result;
}
static int dtu_atcmd_setheartflag(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETHEARTFLAG(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setheart(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETHEART(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setlinkflag(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETLINKFLAG(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setlink(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETLINK(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_help(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Query(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setyzcsq(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETYZCSQ(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_version(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Version(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_reload(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Reload(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_reset(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Reset(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_seto(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_O(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_seto0(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_O(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcmdpw(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_CMDPW(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setsim(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETSIM(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setsimlock(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETSIMLOCK(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setati(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETATI(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcgmi(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETCGMI(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcgmm(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETCGMM(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcgmr(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETCGMR(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_ota(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_OTA(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
//end add by dmh
//begin add by dmh
/*function for ATCMD */
static int AT_TRANS_CmdFunc_SETIP(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 type;
	UINT8 *mode;
	UINT8 *ip;
	UINT32 port;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_SET)
	{
		CPUartLogPrintf("dmhtest00:begin 0021 ");
		if (pParam->paramCount == 3)
		{
			 CPUartLogPrintf("dmhtest00:begin 002 ");
			 mode = at_ParamStr(pParam->params[0], &paramRet);
			 if (!paramRet)
				return result;
			 if ((strcmp(mode, "TCP") == 0) || (strcmp(mode, "tcp") == 0))
			 {
				//set trans type
				CPUartLogPrintf("dmhtest00:TCP ");
				type = 0;
			 }
			 else if ((strcmp(mode, "UDP") == 0) || (strcmp(mode, "udp") == 0))
			 {
				//set trans type
				CPUartLogPrintf("dmhtest00:UDP ");
				type = 1;
			 }
			 else
			 {
				return result;
			 }
			 ip = (UINT8 *)at_ParamStr(pParam->params[1], &paramRet);
			 if ((!paramRet)||(strlen(ip) > TRANS_IP_LEN))
					 return result;
			 
			 port = at_ParamUintInRange(pParam->params[2], 0, 65535, &paramRet);
			 if (!paramRet)
				return result;
			 //set type
			 transconf.type = type;
			 //set trans ip
			 sprintf(transconf.ip,ip);
			 //set trans port
			 transconf.port = port;
			 //return ok
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
		else
		{
			return result;
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		//snprintf(resp->response, strlen(transconf->ip)+1, "%s", transconf->ip);
		CPUartLogPrintf("dmhtest00:read ip ");
		CPUartLogPrintf("dmhtest00:ip:%s",transconf.ip);
		CPUartLogPrintf("dmhtest00:port:%d",transconf.port);
		char arrStr[100]={0};
		if(transconf.type == 0)
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"TCP\"", transconf.ip, transconf.port);
		else 
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"UDP\"", transconf.ip, transconf.port);
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETHEARTFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	
	UINT8 flag;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{
					 
			 flag = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
			 if (!paramRet)
				return result;
			 //set heartflag
			 transconf.heartflag = flag;
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
		
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSHEARTFLAG:";
		sprintf(arrStr + strlen(arrStr), "%d", transconf.heartflag);
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETHEART(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 time;
	UINT8 *heart;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 2)
		{
			 time = at_ParamUintInRange(pParam->params[0], 10, 3600, &paramRet);
			 if (!paramRet)
				return result;
			 transconf.hearttime = time;
			 heart = (UINT8 *)at_ParamStr(pParam->params[1], &paramRet);
			 if ((!paramRet)||(strlen(heart) > TRANS_HEART_LEN))
			 {
				return result;
			 }
			 //set heart
			 sprintf(transconf.heart,heart);
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSHEART:";
		sprintf(arrStr + strlen(arrStr), "%d,\"%s\"", transconf.hearttime,transconf.heart);//dmh 2019-11-05
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETLINKFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 flag;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{
					 
			 flag = at_ParamUintInRange(pParam->params[0], 0, 3, &paramRet);
			 if (!paramRet)
				return result;
			 //set linkfalg
			 transconf.linkflag = flag;
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		
		UINT8 arrStr[] = "+TRANSLINKFLAG:";
		sprintf(arrStr + strlen(arrStr), "%d", transconf.linkflag);//dmh 2019-11-05
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETLINK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 *link;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{
			 link = (UINT8 *)at_ParamStr(pParam->params[0], &paramRet);
			 if ((!paramRet)||(strlen(link) > TRANS_LINK_LEN))
			 {
				return result;
			 }
			 //set link
			 sprintf(transconf.link,link);
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSLINK:";
		sprintf(arrStr + strlen(arrStr), "%s", transconf.link);
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_Query(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
				
		char arrStr[500] ={0};
		if(transconf.type == 0)
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"TCP\"", transconf.ip, transconf.port);
		else
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"UDP\"", transconf.ip, transconf.port);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSHEARTFLAG:", transconf.heartflag);
		sprintf(arrStr + strlen(arrStr), "%s%d,\"%s\"","\r\n+TRANSHEART:", transconf.hearttime,transconf.heart);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSLINKFLAG:", transconf.linkflag);
		sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+TRANSLINK:", transconf.link);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSYZCSQ:", transconf.yzcsq);
		sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+CMDPW:", transconf.cmdpw);
		sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIM:", transconf.sim);
		sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIMLOCK:", transconf.simlock);
		
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
	
	
}
static int AT_TRANS_CmdFunc_Reload(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	//init write tans_file
	transconf = init_trans_conf(transconf); 
	if (pParam->iType == AT_CMD_EXE){
		if(trans_conf_file_write(transconf)==0){
			char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
			result = DSAT_OK;
		}
	}
	return result;
}
static int AT_TRANS_CmdFunc_Reset(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		PM812_SW_RESET();
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_Version(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="AP4000MT_430E_V1.0-20210506";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETYZCSQ(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 yzcsq;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{					 
			 yzcsq = at_ParamUintInRange(pParam->params[0], 0, 255, &paramRet);
			 if (!paramRet)
				return result;
			 //set yzcsq
			 transconf.yzcsq = yzcsq;
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
			 
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		char arrStr[] = "+TRANSYZCSQ:";
		sprintf(arrStr + strlen(arrStr), "%d", transconf.yzcsq);//dmh 2019-11-05
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_O(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[100]="AT";
		sprintf(arrStr + strlen(arrStr), "%s",atLine);
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		serial_mode = DATA_MODE;
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_CMDPW(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8* cmdpw;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{					 
			 cmdpw = (UINT8 *)at_ParamStr(pParam->params[0], &paramRet);
			 if ((!paramRet)||(strlen(cmdpw) > TRANS_CMDPW_LEN))
			 {
				return result;
			 }
			 //set cmdpw
			 sprintf(transconf.cmdpw,cmdpw);
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
			 
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		char arrStr[] = "+CMDPW:";
		sprintf(arrStr + strlen(arrStr), "%s", transconf.cmdpw);//dmh 2019-11-05
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETSIM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 sim;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{
					 
			 sim = at_ParamUintInRange(pParam->params[0], 1, 3, &paramRet);
			 if (!paramRet)
				return result;
			 //set sim
			 transconf.sim = sim;
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		
		UINT8 arrStr[] = "+SIM:";
		sprintf(arrStr + strlen(arrStr), "%d", transconf.sim);//dmh 2019-11-05
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETSIMLOCK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 simlock;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
	{
		if (pParam->paramCount == 1)
		{
					 
			 simlock = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
			 if (!paramRet)
				return result;
			 //set linkfalg
			 transconf.simlock = simlock;
			 if(trans_conf_file_write(transconf)==0){
				char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
				snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
		}
	}
	else if(pParam->iType == AT_CMD_READ)
	{
		
		UINT8 arrStr[] = "+SIMLOCK:";
		sprintf(arrStr + strlen(arrStr), "%d", transconf.simlock);//dmh 2019-11-05
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETATI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="Manufacturer: Beijing Amaziot Co.,Ltd.\r\nModel: Amaziot AM430E\r\nRevision: AM430E V1.0";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETCGMI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="Beijing Amaziot Co.,Ltd.";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETCGMM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="Amaziot AM430E";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETCGMR(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="AM430E V1.0";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_OTA(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8* url;
	bool paramRet = TRUE;
	//init ruturn no
	dsat_result_enum_type result = DSAT_NO_CARRIER;
	
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 		 
			 url = (UINT8 *)at_ParamStr(pParam->params[0], &paramRet);
			 if (!paramRet){
			 	send_to_uart("\r\nERROR\r\n", 9);
				return result;
			 }
                
			 wait_mcu_ota(url);
        }else{
			send_to_uart("\r\nERROR\r\n", 9);
		}
    }else{
		send_to_uart("\r\nERROR\r\n", 9);
	}
	char arrStr[100]="AT";
	sprintf(arrStr + strlen(arrStr), "%s",atLine);
 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
	serial_mode = DATA_MODE;//in teans
	return result;
}





int get_RSSI(void)
{
	char mmRspBuf[100] = {0};
    int  err;
	char *p;
	int csqValue;
	int errorCnt=0;//dmh
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 2, "+CSQ", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==>%s", mmRspBuf);

	if(strlen(mmRspBuf) >=10 ){
		p = strchr(mmRspBuf,':');
		csqValue = atoi(p+1);
	    return csqValue;
    }
    else{
        sleep(2);
		errorCnt++;
		if(errorCnt>1){
			/*
			//reset
			UART_SEND_DATA("\r\nGet Csq Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
			*/
			csqValue = 99;
			return csqValue;
		}	
        goto wait_reg;
    } 
}
int get_RSSI_grade(int csq)
{
	
	if(csq<=4 || csq==99){
		return 1;
	}
	else if(csq>=28){
		return 6;
	}else if(csq>=22){
		return 5;
	}else if(csq>=16){
		return 4;
	}else if(csq>=10){
		return 3;
	}else if(csq>=4){
		return 2;
	}
	return 0;
}


void get_IMEI(char *buffer)
{
	char mmRspBuf[100] = {0};
    int  err;
	char *p;
	int errorCnt=0;//dmh
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CGSN\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==>%s", mmRspBuf);

	if(strlen(mmRspBuf) >=15 ){
		snprintf(buffer,16,mmRspBuf);
		return 0;
    }
    else{
        sleep(2);
		errorCnt++;
		if(errorCnt>1){
			memset(mmRspBuf, 0, sizeof(mmRspBuf));
			snprintf(buffer,16,mmRspBuf);
			return 0;
			/*
			//reset
			UART_SEND_DATA("\r\nGet IMEI Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
			*/
			
		}	
        goto wait_reg;
    } 
}
void get_IMSI(char *buffer)
{
	char mmRspBuf[100] = {0};
    int  err;
	char *p;
	int errorCnt=0;//dmh
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CIMI\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==>%s", mmRspBuf);

	if(strlen(mmRspBuf) >=15 ){
		snprintf(buffer,16,mmRspBuf);
		return 0;
    }
    else{
        sleep(2);
		errorCnt++;
		if(errorCnt>1){
			memset(mmRspBuf, 0, sizeof(mmRspBuf));
			snprintf(buffer,16,mmRspBuf);
			return 0;
			/*
			//reset
			UART_SEND_DATA("\r\nGet IMEI Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
			*/
			
		}	
        goto wait_reg;
    } 
}
void get_ICCID(char *buffer)
{
	char mmRspBuf[100] = {0};
    int  err;
	char *p;
	int errorCnt=0;//dmh
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+ICCID\r\n", 2, "+ICCID", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==>%s", mmRspBuf);

	if(strstr(mmRspBuf, "+ICCID") != NULL ){
		p = strchr(mmRspBuf,':');
		snprintf(buffer,21,p+2);
		return 0;
    }
    else{
        sleep(2);
		errorCnt++;
		if(errorCnt>1){
			memset(mmRspBuf, 0, sizeof(mmRspBuf));
			snprintf(buffer,21,mmRspBuf);
			return 0;
			/*
			//reset
			UART_SEND_DATA("\r\nGet ICCID Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
			*/
		}	
        goto wait_reg;
    } 
}

#endif /* ifdef INCL_MOUDULES_MT_TCP.2022-1-25 17:30:58 by: win */



