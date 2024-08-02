//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_at_cmds.c
// Auther      : zhaoning
// Version     :
// Date : 2023-3-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-3-18
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
#include <stdbool.h>

#include "sys.h"
#include "sdk_api.h"
#include "teldef.h"
#include "cJSON.h"
#include "sha256.h"
#include "am_at_cmds.h"
#include "am_file.h"
#include "utils_common.h"
#include "am_common.h"
#include "am_utils.h"

// Private defines / typedefs ---------------------------------------------------

typedef struct {
    unsigned char at_cmd_name[AT_CMD_NAME_MAX_LEN + 1];
    dsat_result_enum_type (*proc_func)(char *, char *, AtCmdResponse *);
} ATCmdTable;

// Private variables ------------------------------------------------------------

static bool atDataTimeOut = false;
static int tmp_count = 0;
static char tmp[256] = {0};
static dsat_sio_info_s_type g_sio_info;
static OSMsgQRef MsgUartData_sdk = NULL;

// Public variables -------------------------------------------------------------

OSATimerRef _atdatatimer_ref = NULL;//20200708
char at_version[10];    //define for at query romVersion
uint8_t serial_mode;    // 0:DATA_MODE 1:AT_MDOE

extern trans_conf transconf;//weihu yi ge save in flash

// Private functions prototypes -------------------------------------------------

static dsat_result_enum_type dtu_atcmd_seto(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_seto0(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_setlicense(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_dellicense(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_devid(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_ver(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_chipid(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_wifi(char *atName, char *atLine, AtCmdResponse *resp);
static dsat_result_enum_type dtu_atcmd_sha(char *atName, char *atLine, AtCmdResponse *resp);

static dsat_result_enum_type AT_TRANS_CmdFunc_O(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_SETLICENSE(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_DELLICENSE(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_DEVID(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_VER(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_CHIPID(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_WIFI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static dsat_result_enum_type AT_TRANS_CmdFunc_SHA(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);

static ATCmdTable dtu_atcmd_table[] = {
    { "O",dtu_atcmd_seto},
    { "O0",dtu_atcmd_seto0},
    { "+SEND",dtu_atcmd_setlicense},
    { "+DELETE_DATA",dtu_atcmd_dellicense},
    { "+DEVID",dtu_atcmd_devid},
    { "+VER",dtu_atcmd_ver},
    { "+CHIPID",dtu_atcmd_chipid},
    { "+WIFI",dtu_atcmd_wifi},
    { "+SHA256",dtu_atcmd_sha},
    // add by  end
};

static const uint8_t *at_ParamStrParse(AT_COMMAND_PARAM_T *param);
static const uint8_t *at_ParamStr(AT_COMMAND_PARAM_T *param, bool *paramok);
static uint32_t at_ParamUint(AT_COMMAND_PARAM_T *param, bool *paramok);
static uint32_t at_ParamUintInRange(AT_COMMAND_PARAM_T *param, uint32_t minval,uint32_t maxval, bool *paramok);
static int package_at_cmd(char *atName, char *atLine,AT_CMD_PARA *pParam);
static void free_pParam(AT_CMD_PARA *pParam);
static int process_at_cmd_line(char *cmdName, char *cmdLine);
static void process_at_cmd_mode(const char *atCmdData, int data_len);
static void sendResponse(AtCmdResponse *response);
static void license_sha256(const char *license, int licenseLen);
static void send_serial_data(MsgUartDataParam_sdk * uartData);
static void check_serial_mode(MsgUartDataParam_sdk *msgUartData);
static void handle_serial_data(MsgUartDataParam_sdk *uartData);
static void _atdatatimer_callback(UINT32 tmrId);
static void uartdata_thread(void);

// Public functions prototypes --------------------------------------------------

extern void set_UartDataSwitch_sdk(BOOL flag);

// Functions --------------------------------------------------------------------

void RecvCallback(UINT8 *data, UINT32 len)
{
    MsgUartDataParam_sdk uart_temp = {0};
    OSA_STATUS status = 0;
    
    char *tempbuf = (char *)malloc(len+10);
    memset(tempbuf, 0x0, len+10);
    memcpy(tempbuf, (char *)data, len);
    
    uart_temp.UArgs = (UINT8 *)tempbuf;
    uart_temp.len =len;
    
    status = OSAMsgQSend(MsgUartData_sdk, sizeof(MsgUartDataParam_sdk), (UINT8*)&uart_temp, OSA_NO_SUSPEND);
    ASSERT(status == OS_SUCCESS);
        
}

/* This function send data to uart. */
void send_to_uart(char *toUart, int length)
{
    char *sendData = NULL;
    cprintf("%s[%d]:toUart=%s, length=%d", __FUNCTION__, __LINE__, toUart, length);
    sendData = (char *)malloc(length+1);
    ASSERT(sendData != NULL);
    memset(sendData, 0, length+1);

    memcpy(sendData, toUart, length);
    sendData[length] = '\0';
    
    cprintf("%s[%d]:send to uart data=%s, length=%d", __FUNCTION__, __LINE__, sendData, length);    
    UART_SEND_DATA((UINT8 *)sendData, length);
    cprintf("%s[%d]:sendData==NULL = %d", __FUNCTION__, __LINE__, (sendData==NULL));
    if (sendData)
        free(sendData);
}

/*function for init at uart*/
void init_at_uart_conf(void)
{
     int ret = 0;
     ret = OSAMsgQCreate(&MsgUartData_sdk, "MsgUartData_sdk", sizeof(MsgUartDataParam_sdk), 256, OS_FIFO);
     ASSERT(ret == OS_SUCCESS);
     ret = OSATimerCreate(&_atdatatimer_ref);

     ASSERT(ret == OS_SUCCESS);
     sys_thread_new("uartdata_thread", (lwip_thread_fn)uartdata_thread, NULL, SYS_THREAD_STACK_SIZE * 4, 80);
}

/*function for at uart thread*/
static void uartdata_thread(void)
{
    MsgUartDataParam_sdk uart_temp = {0};
    OSA_STATUS status = 0;
    
    while (1)
    {
        memset(&uart_temp, 0, sizeof(MsgUartDataParam_sdk));
        
        status = OSAMsgQRecv(MsgUartData_sdk, (UINT8 *)&uart_temp, sizeof(MsgUartDataParam_sdk), OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            if (uart_temp.UArgs)
            {
                cprintf("%s[%d]: uart_temp len:%d, data:%s", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                handle_serial_data(&uart_temp);
            }
        }
    }
}

/*function for sha256 jiami*/
static void license_sha256(const char *license, int licenseLen)
{
    unsigned char str_sha256[32] = {0};
    char sha256str[64] = {0};
    char jiaoyan[128] = {0};
    
    //jiexi json
    cJSON * hiDevice = cJSON_Parse(license);  
    if(hiDevice!=NULL){
        cJSON * pItem;

        pItem = cJSON_GetObjectItem(hiDevice, "deviceId");
        if (pItem == NULL)
            goto error;
        sprintf(transconf.deviceId,pItem->valuestring);
        
        pItem = cJSON_GetObjectItem(hiDevice, "factory_apikey");
        if (pItem == NULL)
            goto error;
        sprintf(transconf.factory_apikey,pItem->valuestring);
        
        pItem = cJSON_GetObjectItem(hiDevice, "sta_mac");
        if (pItem == NULL)
            goto error;
        sprintf(transconf.sta_mac,pItem->valuestring);
        
        pItem = cJSON_GetObjectItem(hiDevice, "sap_mac");
        if (pItem == NULL)
            goto error;
        sprintf(transconf.sap_mac,pItem->valuestring);
        
        pItem = cJSON_GetObjectItem(hiDevice, "device_model");
        if (pItem == NULL)
            goto error;
        sprintf(transconf.device_model,pItem->valuestring);

        sprintf(jiaoyan,"%s%s%s%s%s",transconf.deviceId,transconf.factory_apikey,transconf.sta_mac,transconf.sap_mac,transconf.device_model);

        cprintf("jiaoyan:%s",jiaoyan);//test
        
        //jiaoyan
        mbedtls_sha256((const unsigned char*)jiaoyan,strlen(jiaoyan),str_sha256,0);
        HexToStr(sha256str, (char*)str_sha256, 32);
        
        //send
        send_to_uart((char *)sha256str, 64);
        send_to_uart("\r\n", 2);
        send_to_uart((char *)sha256str, 64);
        send_to_uart("\r\n", 2);
        send_to_uart((char *)sha256str, 64);
        send_to_uart("\r\n", 2);
        send_to_uart("OK\r\n", 4);
        
        //save
        snprintf(transconf.license,licenseLen + 1,license);
        if(trans_conf_file_write(transconf) < 0)
            goto error;
        
        
        return;
    }
    else
    {
        cprintf("json error");
        goto error;
    }
    error:
       cJSON_Delete(hiDevice);
       transconf.licenseLen = strlen(transconf.license);
       char arrStr[]="DATA ERROR: FORMAT ERROR\r\nERROR\r\n";
       send_to_uart(arrStr, strlen(arrStr));
       return;
    
}

/*function for data mode send data*/
static void send_serial_data(MsgUartDataParam_sdk * uartData)
{
    cprintf("%s[%d]: DATA_MODE recvdata: %s", __FUNCTION__, __LINE__, uartData->UArgs);
}

/*function for check serial moded*/
static void check_serial_mode(MsgUartDataParam_sdk *msgUartData)
{
    if (serial_mode == DATA_MODE)
    {
        if(msgUartData->len == 3 && memcmp(msgUartData->UArgs, "+++", 3) == 0)
        {
            serial_mode = AT_MODE;
            send_to_uart("\r\nOK\r\n", 6);
        }
    } 
    
}

/*function for handle_serial_data*/
static void handle_serial_data(MsgUartDataParam_sdk *uartData)
{
    check_serial_mode(uartData);//close judge serial mode

    if (serial_mode == DATA_MODE)
    {
       send_serial_data(uartData);
    
    } 
    else if(serial_mode == AT_MODE)
    {
        cprintf("It time, serial_mode is AT_MODE!");
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
        {
            memset(tmp, 0, sizeof(tmp));
            tmp_count = 0;
        }
        else if(strstr((const char*)uartData->UArgs,"\r\n"))
        {
            sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
            cprintf("[atcmd_test]: send AT command uargs:%s,len:%d",tmp, strlen(tmp) );
            process_at_cmd_mode(tmp, strlen(tmp));
            tmp_count = 0;
        }
        else
        {
            sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
            tmp_count += uartData->len;//modify by 2020-06-22
            if(tmp_count>256)
                tmp_count =0;
            
        }
    }
    else if(serial_mode == AT_DATA_MODE)
    {
        sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
        tmp_count += uartData->len;//modify by 2020-06-22
        if(atDataTimeOut)
        {
            atDataTimeOut = false;
            transconf.licenseLen = strlen(transconf.license);
            char arrStr[]="DATA ERROR: NO DATA\r\nERROR\r\n";
            send_to_uart(arrStr, strlen(arrStr));
            //mode switch
            serial_mode = AT_MODE;
            //qing ling
            memset(tmp, 0, sizeof(tmp));//for 32bit error
            tmp_count = 0;
        }
        if(tmp_count >= transconf.licenseLen)
        {
            //stop timer
            OSATimerStop(_atdatatimer_ref);
            //judge has license
            if(strlen(transconf.license)>0&&strlen(transconf.license)>0 &&strlen(transconf.deviceId)>0 &&strlen(transconf.factory_apikey)>0&&strlen(transconf.sta_mac)>0&&strlen(transconf.sap_mac)>0&&strlen(transconf.device_model)>0 ){
                transconf.licenseLen = strlen(transconf.license);
                //error//FLASH FAILED: FLASHED ALREADY\r\nERROR\r\n
                char arrStr[]="FLASH FAILED: FLASHED ALREADY\r\nERROR\r\n";
                send_to_uart(arrStr, strlen(arrStr));
                
            }
            else
            {
                //jiaoyan
                license_sha256(tmp,transconf.licenseLen);    
            }
            //qing ling
            memset(tmp, 0, sizeof(tmp));//for 32bit error
            tmp_count = 0;
            //mode switch
            serial_mode = AT_MODE;
                
            
        }
        if(tmp_count > 256)
            tmp_count = 0;
    }
    
}

/*function for atdata timer*/
static void _atdatatimer_callback(UINT32 tmrId)
{
    cprintf("_atdatatimer_callback");
        
    atDataTimeOut = true;
    //send null message
    MsgUartDataParam_sdk heart = {0};    
    char *heartbuf = (char *)malloc(10);
    memset(heartbuf, 0x0, 10);
    sprintf(heartbuf, "0");
    heart.len = strlen(heartbuf);
    heart.UArgs = (UINT8 *)heartbuf;
    OSAMsgQSend(MsgUartData_sdk, sizeof(MsgUartDataParam_sdk), (UINT8*)&heart, OSA_NO_SUSPEND);
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
        if (*endptr != 0)
        {
            cprintf("endptr:%s ",endptr);//test
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
        cprintf("pParam->iType = AT_CMD_EXE");
        pParam->iType = AT_CMD_EXE;
        return 0;
    }
    if(*(atLine+strlen(atName))=='?'){
        cprintf("pParam->iType = AT_CMD_READ");
        if(strlen(atLine)-strlen(atName) == 1)
            pParam->iType = AT_CMD_READ;
        return 0;
    }
    if( (*(atLine+strlen(atName))=='=')&&( *(atLine+strlen(atName)+1)=='?')){
        cprintf("pParam->iType = AT_CMD_TEST");
        if(strlen(atLine)-strlen(atName) == 2)
            pParam->iType = AT_CMD_TEST;
        return 0;
    }
    cprintf("pParam->iType = AT_CMD_SET");
    pParam->iType = AT_CMD_SET;
    char strParam[100] = {0};
    UINT8 index = 0;
    int step = 0;
    char cc = 0;
    char *save_buf = NULL;
    char *buf_ptr = NULL;
    int data_len = strlen(atLine) - strlen(atName)-1;
    buf_ptr = atLine + strlen(atName) + 1;
    save_buf = buf_ptr;
    while (data_len > 0)
    {
        cc = *buf_ptr++;
        data_len--;
        step++;
        if(cc == ','){
            cprintf("%s","begin0");
            //1.set pParam->paramCount
            pParam->paramCount++;
            //2.malloc pParam->param
            pParam->params[index]=malloc(sizeof(AT_COMMAND_PARAM_T)+step*sizeof(uint8_t));
            if(pParam->params[index] == NULL)
                return -1;

            //3.get the strParam and judge
            if(step <= 1)
                return -1;
            memset(strParam,0,100);//add by 2020-06-22
            memcpy(strParam, save_buf, step-1);

            //wait for check 
            if(step > 3)
            {
                if(!(strParam[0] == '\"' && strParam[step-2] == '\"')){
                    //if not all num
                    if(strspn(strParam, "0123456789")!=strlen(strParam))
                        return -1;
                }
            }
            else
            {
                //if not all num
                if(strspn(strParam, "0123456789")!=strlen(strParam))
                        return -1;
            }
            
            //4.set param->length
            pParam->params[index]->length=step-1;
            //5.set param-type
            if(pParam->params[index]->length>2){
                if(*save_buf == '\"' && *(save_buf+step-2)=='\"')
                    pParam->params[index]->type=AT_CMDPARAM_STRING;
                else
                    pParam->params[index]->type=AT_CMDPARAM_NUMBER;
            }
            else
                pParam->params[index]->type=AT_CMDPARAM_NUMBER;
            //6.set params->value
            memcpy(pParam->params[index++]->value, strParam, step);
            save_buf = buf_ptr;   
            step = 0;
        }
                   
    }
    cprintf("%s","begin1");
    //1.set param
    pParam->paramCount++;
    //2.malloc pParam->param
    pParam->params[index]=malloc(sizeof(AT_COMMAND_PARAM_T)+(step+1)*sizeof(uint8_t));
    if(pParam->params[index] == NULL)
        return -1;
    //3.get the strParam and judge
    if(step<1)
        return -1;
    memset(strParam,0,100);
    memcpy(strParam, save_buf, step);

    //wait for check 
    if(step > 3)
    {
        if(!(strParam[0] == '\"' && strParam[step-1] == '\"'))
        {
            //if not all num
            if(strspn(strParam, "0123456789")!=strlen(strParam))
                return -1;
        }
    }
    else
    {
        //if not all num
        if(strspn(strParam, "0123456789")!=strlen(strParam))
                return -1;
    }

    //4.set param->length
    pParam->params[index]->length=step;
    //5.set param-type
    if(pParam->params[index]->length>2)
    {
        if(*save_buf == '\"' && *(save_buf+step-1)=='\"')
            pParam->params[index]->type=AT_CMDPARAM_STRING;
        else
            pParam->params[index]->type=AT_CMDPARAM_NUMBER;
    }
    else
        pParam->params[index]->type=AT_CMDPARAM_NUMBER;
    //6.set params->value
    memcpy(pParam->params[index++]->value, strParam, step+1);
    cprintf("%s","end");
    return 0;
}

// =============================================================================
// free_pParam
// =============================================================================
static void free_pParam(AT_CMD_PARA *pParam)
{
    UINT8 i = 0;
    for(i=0;i<pParam->paramCount;i++)
    {
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
    char resp_buffer[MAX_LINE_SIZE] = {0};
    const char *msg = NULL;
    int sz = 0;
    if(serial_mode == AT_MODE)
    {
        if (response->result == DSAT_OK)
        {
            if (response->response && response->response[0])
            {
                sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                UART_SEND_DATA((UINT8 *)resp_buffer, sz);
            }
            msg = MSG_OK;
        }
        else
        {
            msg = MSG_ERROR;
        }
        //UART_SEND_DATA(msg, strlen(msg));
    }
    else if(serial_mode == AT_DATA_MODE)
    {
        sz = sprintf((char *)resp_buffer, "%s\r\n", response->response);
        UART_SEND_DATA((UINT8 *)resp_buffer, sz);
    }
}

// =============================================================================
// process_at_cmd_line
// =============================================================================
static int process_at_cmd_line(char *cmdName, char *cmdLine)
{
    AtCmdResponse *response = NULL;
    int i = 0;
    cprintf("%s",cmdName);
    cprintf("%s",cmdLine);
    response = malloc(sizeof(AtCmdResponse));
    if ( response != NULL )
    {
        memset(response, 0x0, sizeof(AtCmdResponse));
        response->response = malloc(MAX_LINE_SIZE);
        response->response[0] = 0;
        response->result = DSAT_ERROR;
        response->istranscmd = DSAT_ERROR;

        ATCmdTable *atcmd_ptr = dtu_atcmd_table;
        int nCommands = sizeof(dtu_atcmd_table) / sizeof(dtu_atcmd_table[0]);
        for (i = 0; i < nCommands; i++, atcmd_ptr++)
        {
            if (strcmp((char *)atcmd_ptr->at_cmd_name, cmdName)==0)
            {
                response->result = DSAT_OK;
                response->istranscmd = DSAT_OK;
                if (atcmd_ptr->proc_func != NULL)
                {
                    response->result = atcmd_ptr->proc_func(cmdName, cmdLine, response);
                }
                break;
            }
        }
        //begin test by 
        
        if(response->istranscmd == DSAT_ERROR)
        {
            char at_str[128]={'\0'};
            sprintf(at_str, "AT%s\r",cmdLine);
            char resp_str[512]={'\0'};
            response->result = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, NULL,1,NULL, resp_str, sizeof(resp_str));
            cprintf("dmhtest:resp_str:%s,%d",resp_str,strlen(resp_str));
            snprintf(response->response, strlen(resp_str)+1, "%s", resp_str);
        }
        
        //end test by
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
static void process_at_cmd_mode(const char *atCmdData, int data_len)
{
    dsat_sio_info_s_type *sio_info_ptr = &g_sio_info;
    const char *buf_ptr = NULL;
    char cc, atName[MAX_LINE_SIZE] = {0};
    int step = 0;

    buf_ptr = atCmdData;
    cprintf("%s[%d]: AT_MODE recv ATCMD: %s", __FUNCTION__, __LINE__, buf_ptr);
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
                    cprintf("sio_info_ptr->cmd_line_buffer:%s", sio_info_ptr->cmd_line_buffer);
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step);
                    atName[step-1] = '\0';
                    cprintf("atName is %s", atName);
                }
                *sio_info_ptr->build_cmd_ptr++ = cc;
            } else {
                /*  EOL found, terminate and parse */
                *sio_info_ptr->build_cmd_ptr = '\0';

                if (!strlen(atName)) {
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step-1);
                    atName[step-1] = '\0';
                    cprintf("[%d]: atName is %s", __LINE__, atName);
                }

                cprintf("%s[%d]: cmd_line_buffer=%s", __FUNCTION__, __LINE__, sio_info_ptr->cmd_line_buffer);

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

/******************************************************************************
 *   at functions
 ******************************************************************************/
/*function for ATCMD*/
static dsat_result_enum_type dtu_atcmd_seto(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;
    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_O(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_seto0(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;
    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_O(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_setlicense(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_SETLICENSE(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_dellicense(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_DELLICENSE(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_devid(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_DEVID(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_ver(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_VER(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_chipid(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_CHIPID(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_wifi(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = AT_TRANS_CmdFunc_WIFI(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type dtu_atcmd_sha(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    AT_CMD_PARA pParam = {0};
    
    if(package_at_cmd(atName,atLine,&pParam)==0)
    {
        result = (dsat_result_enum_type)AT_TRANS_CmdFunc_SHA(&pParam,atLine,resp);
        free_pParam(&pParam);
    }
    else
    {
        free_pParam(&pParam);
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_O(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    if (pParam->iType == AT_CMD_EXE)
    {
        char arrStr[8]="AT";
        sprintf(arrStr + strlen(arrStr), "%s",atLine);
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
        serial_mode = DATA_MODE;
        result = DSAT_OK;
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_SETLICENSE(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT16 licenseLen = 0;
    bool paramRet = TRUE;
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
             licenseLen = at_ParamUintInRange(pParam->params[0], 0, 500, &paramRet);
             if (!paramRet)
                return result;
             //set licenseLen
             transconf.licenseLen = licenseLen;
             if(trans_conf_file_write(transconf)==0){
                snprintf(resp->response, 2, "%s", ">\0");
                result = DSAT_OK;
                serial_mode = AT_DATA_MODE;
                atDataTimeOut = false;
                //start timer
                OSATimerStart(_atdatatimer_ref, 5*200, 0, _atdatatimer_callback, 0); //20200708  5s
             }
        }
    }
    else if(pParam->iType == AT_CMD_READ)
    {
        char arrStr[200] = {0};
        
        sprintf(arrStr, "+SEND:%s", transconf.license);//2019-11-05
        snprintf(resp->response, strlen(arrStr) + 1, "%s", arrStr);
        result = DSAT_OK;
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_DELLICENSE(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT16 licenseLen = 0;
    bool paramRet = TRUE;
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_EXE == pParam->iType)
    {
        transconf.licenseLen = 0;
        sprintf(transconf.license,"");
        sprintf(transconf.deviceId,"");
        sprintf(transconf.factory_apikey,"");
        sprintf(transconf.sta_mac,"");
        sprintf(transconf.sap_mac,"");
        sprintf(transconf.device_model,"");
        if(trans_conf_file_write(transconf)==0){
            char arrStr[24]="AT";
            sprintf(arrStr + strlen(arrStr), "%s=%s",atLine,"OK");
//            cprintf("%s[%d]arrStr==>%s %d", __FUNCTION__, __LINE__, arrStr, strlen(arrStr));
            snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
            result = DSAT_OK;
         }
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_DEVID(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT16 licenseLen = 0;
    bool paramRet = TRUE;
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_READ == pParam->iType)
    {
        char arrStr[24] = {0};
        if(strlen(transconf.deviceId) > 0)
            sprintf(arrStr + strlen(arrStr), "AT+DEVID=%s", transconf.deviceId);//2019-11-05
        else
            sprintf(arrStr + strlen(arrStr), "AT+DEVID=%s", "ERROR");
//        cprintf("%s[%d]arrStr==>%s %d", __FUNCTION__, __LINE__, arrStr, strlen(arrStr));
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
        result = DSAT_OK;
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_VER(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT16 licenseLen = 0;
    bool paramRet = TRUE;
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_READ == pParam->iType)
    {
        char arrStr[30] = {0};
        if(strlen(transconf.device_model) > 0)
            sprintf(arrStr + strlen(arrStr), "AT+VER=%s-V%s", transconf.device_model, FWVERSION);
        else
            sprintf(arrStr + strlen(arrStr), "AT+VER=%s", "ERROR");
//        cprintf("%s[%d]arrStr==>%s %d", __FUNCTION__, __LINE__, arrStr, strlen(arrStr));
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
        result = DSAT_OK;
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_CHIPID(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT16 licenseLen = 0;
    bool paramRet = TRUE;
    char chipid[20] = {0};
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_READ == pParam->iType)
    {
        char arrStr[30] = {0};
        get_IMEI(chipid);
        sprintf(arrStr + strlen(arrStr), "AT+CHIPID=%s", chipid);//2019-11-05
//        cprintf("%s[%d]arrStr==>%s %d", __FUNCTION__, __LINE__, arrStr, strlen(arrStr));
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
        result = DSAT_OK;
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_WIFI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT16 licenseLen = 0;
    bool paramRet = TRUE;
    int csq = 0;
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_READ == pParam->iType)
    {
        char arrStr[16] = {0};
        csq = get_RSSI();
        sprintf(arrStr + strlen(arrStr), "AT+SIGNAL=%d", csq);//2019-11-05
//        cprintf("%s[%d]arrStr==>%s %d", __FUNCTION__, __LINE__, arrStr, strlen(arrStr));
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
        result = DSAT_OK;
    }
    return result;
}

static dsat_result_enum_type AT_TRANS_CmdFunc_SHA(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    unsigned char str_sha256[32] = {0};
    char sha256str[65] = {0};
    char jiaoyan[100] = {0};
    //init ruturn error
    dsat_result_enum_type result = DSAT_ERROR;
    
    if (AT_CMD_READ == pParam->iType)
    {
        char arrStr[70] = {0};
        if(transfile_is_null())
            sprintf(arrStr + strlen(arrStr), "AT+SHA256=ERROR");//2019-11-05
        else{
            
            sprintf(jiaoyan,"%s%s%s%s%s",transconf.deviceId,transconf.factory_apikey,transconf.sta_mac,transconf.sap_mac,(const unsigned char*)transconf.device_model);
            //jiaoyan
            mbedtls_sha256((const unsigned char*)jiaoyan,strlen(jiaoyan),str_sha256,0);
            HexToStr(sha256str, (char*)str_sha256, 32);
            sprintf(arrStr + strlen(arrStr), "AT+SHA256=%s\r\n",sha256str);//2019-11-05
        }
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
        result = DSAT_OK;
    }
    return result;
}

int get_RSSI(void)
{
    char mmRspBuf[16] = {0};
    int err = 0;
    char *p = NULL;
    int csqValue = 0;
    int errorCnt = 0;//
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 2, "+CSQ", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    cprintf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strlen(mmRspBuf) >= 10 )
    {
        p = strchr(mmRspBuf,':');
        csqValue = atoi(p + 1);
        return csqValue;
    }
    else
    {
        sleep(2);
        errorCnt++;
        if(errorCnt > 1)
        {
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

/*
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
*/

//int get_RSSI_grade(int rssi)
//{
//    
//    if(rssi<-92)
//    {
//        return 1;
//    }
//    else if(rssi < -77)
//    {
//        return 2;
//    }
//    else if(rssi < -62)
//    {
//        return 3;
//    }
//    else 
//        return 4;
//}

void get_IMEI(char *buffer)
{
    char mmRspBuf[24] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CGSN\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    cprintf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strlen(mmRspBuf) >= 15 )
    {
        snprintf(buffer,16,mmRspBuf);
        return;
    }
    else
    {
        sleep(2);
        errorCnt++;
        if(errorCnt > 1)
        {
            memset(mmRspBuf, 0, sizeof(mmRspBuf));
            snprintf(buffer,16,mmRspBuf);
            return;
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
    char mmRspBuf[24] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CIMI\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    cprintf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strlen(mmRspBuf) >= 15 )
    {
        snprintf(buffer, 16, mmRspBuf);
        return;
    }
    else
    {
        sleep(2);
        errorCnt++;
        if(errorCnt>1)
        {
            memset(mmRspBuf, 0, sizeof(mmRspBuf));
            snprintf(buffer,16,mmRspBuf);
            return;
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
    char mmRspBuf[36] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+ICCID\r\n", 2, "+ICCID", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    cprintf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strstr(mmRspBuf, "+ICCID") != NULL )
    {
        p = strchr(mmRspBuf,':');
        snprintf(buffer,21,p+2);
        return;
    }
    else
    {
        sleep(2);
        errorCnt++;
        if(errorCnt>1)
        {
            memset(mmRspBuf, 0, sizeof(mmRspBuf));
            snprintf(buffer,21,mmRspBuf);
            return;
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

void set_CFUN_0(void)
{
    char mmRspBuf[8] = {0};
    int err = 0;
    char *p = NULL;
    int csqValue = 0;
    int errorCnt = 0;//
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CFUN=0\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    cprintf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strstr(mmRspBuf, "OK") != NULL )
    {
        return;
    }
    else
    {
        sleep(2);
        errorCnt++;
        if(errorCnt > 2)
        {
            return;
        }    
        goto wait_reg;
    } 
}

void set_CFUN_1(void)
{
    char mmRspBuf[8] = {0};
    int err = 0;
    char *p = NULL;
    int csqValue = 0;
    int errorCnt = 0;//
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CFUN=1\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    cprintf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strstr(mmRspBuf, "OK") != NULL )
    {
        return;
    }
    else
    {
        sleep(2);
        errorCnt++;
        if(errorCnt > 2)
        {
            return;
        }    
        goto wait_reg;
    } 
}

// End of file : am_at_cmds.h 2023-3-18 17:07:22 by: zhaoning 

