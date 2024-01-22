//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-16
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-16
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

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define _STACK_SIZE            0x2000

// Private variables ------------------------------------------------------------

static void* _task_stack = NULL;

static OSTaskRef _task_ref = NULL;


OSATimerRef _task_delete_timer_ref = NULL;

static void _task(void *ptr);

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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
    
    _task_stack = malloc(_STACK_SIZE);
    
    ret = OSATaskCreate(&_task_ref, _task_stack, _STACK_SIZE, 100, "_task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void DeleteTaskByTimer(UINT32 id)
{
    OSA_STATUS status;

    if(_task_ref)
    {
        status = OSATaskDelete(_task_ref);
        ASSERT(status == OS_SUCCESS);
        _task_ref = NULL;
       
        if(_task_stack)
        {
            free(_task_stack);
            _task_stack = NULL;
        }
    }

    OSATimerDelete (_task_delete_timer_ref);
}

static void apn_config(void);

static void _task(void *ptr)
{
    apn_config();
    
    OSATimerCreate(&_task_delete_timer_ref);
    OSATimerStart(_task_delete_timer_ref, 400, 0, DeleteTaskByTimer, 0);
}

extern BOOL IsAtCmdSrvReady(void);

extern BOOL isMasterSim0(void);

extern int SendATCMDWaitResp(int sATPInd, char *inStr, int timeOut, char *okFmt, int okFlag, char *errFmt, char *outStr, int outLen);

int SDKSendATCMD(int sATPInd, char *inStr, int timeOut, char *okFmt, int okFlag, char *errFmt, char *outStr, int outLen)
{
    if (isMasterSim0()){
        return SendATCMDWaitResp(sATPInd, inStr, timeOut, okFmt, okFlag, errFmt, outStr, outLen);
    }else{
        return SendATCMDWaitResp(sATPInd+36, inStr, timeOut, okFmt, okFlag, errFmt, outStr, outLen);
    }
}

static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){
            ready = 1;
        }
        sdk_uart_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}

static void apn_config(void)
{
    int ret;
    int index = 0;
    char at_str[64]={'\0'};
    char resp_str[128]={'\0'};
    
    int tempValue = 0;
    char *tempPtr = NULL;
    char tempIPstr[32]={'\0'};
    char tempAPNstr[64]={'\0'};    
    char tempUSRstr[64]={'\0'};
    char tempPASSWDstr[64]={'\0'};    

    while(!IsAtCmdSrvReady())
    {
        OSATaskSleep(100);
    }    

    //设置APN： AT*CGDFLT=1,"IP","APN",,,,,,,,,,,,,,,,,,1    //重启生效
        // 参数1：0 不存储, 1 存储, 建议使用1重启生效。
        // 参数2：IP 网络类型ipv4, IPV6 网络类型ipv6, IPV4V6 网络类型ipv4v6。
        // 参数3：APN。
        // 最后一位参数：APN不为空时设置成1, 为空设置成0。
    //查询默认APN：AT*CGDFLT=1                                // *CGDFLT: "IP","",0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0
        // 返回参数1：IP 网络类型ipv4, IPV6 网络类型ipv6, IPV4V6 网络类型ipv4v6。
        // 返回参数2：APN, 空表示使用网络下发APN。
    //设置用户名密码: AT*CGDFAUTH=1,2,usr,passwrd            // 重启生效, 大部分的卡使用不到,  //第一个0立即生效不存储，1立即生效且存储；第二个参数是鉴权方式，0-none；1-pap；2-chap
        // 参数1：0 不存储, 1 存储, 建议使用1重启生效。
        // 参数2：鉴权方式，0-none；1-pap；2-chap。
        // 参数3：用户名。
        // 参数4：密码。                
    //查询用户名密码：AT*CGDFAUTH=1                            // *CGDFAUTH:0,"",""
        // 返回参数1：鉴权方式，0-none；1-pap；2-chap。
        // 返回参数2：用户名。
        // 返回参数3：密码。
    //查询当前APN和IP：AT+CGDCONT?                            // +CGDCONT: 1,"IP","CMNET","10.24.212.128",0,0,,,,
        // 参数1：cid，PDP 上下文标识，用于指定特定 PDP 上下文的定义，默认cid=1。
        // 参数2：IP 网络类型ipv4, IPV6 网络类型ipv6, IPV4V6 网络类型ipv4v6。
        // 参数3：APN。
        // 参数4：IP地址。

    memset(at_str, 0x00, sizeof(at_str));
    memset(resp_str, 0x00, sizeof(resp_str));    
    sprintf(at_str, "AT*CGDFLT=1\r\n");        // 查询预设的APN
    ret = SDKSendATCMD(TEL_AT_CMD_ATP_12, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
    sdk_uart_printf("%s: resp_str = %s, ret = %u\n", at_str, resp_str, ret);
    
    // *CGDFLT: "IP","APN"    APN后面的参数不需要管    
    tempPtr = strstr(resp_str, "*CGDFLT");
    if (tempPtr){
        memset(tempIPstr, 0x0, sizeof(tempIPstr));
        memset(tempAPNstr, 0x0, sizeof(tempAPNstr));    
        //*CGDFLT: "IP","cmnet",0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1
        sscanf(tempPtr, "%*[^ ] \"%[^\"]\",\"%[^\"]\",%*[^]", tempIPstr, tempAPNstr);        

        sdk_uart_printf("%s: tempIPstr = %s, tempAPNstr = %s\n", __FUNCTION__, tempIPstr, tempAPNstr);
    }    
    
    if (strncmp(tempAPNstr, "CMWAP", strlen("CMWAP")) != 0){        
        memset(at_str, 0x00, sizeof(at_str));
        memset(resp_str, 0x00, sizeof(resp_str));    
        sprintf(at_str, "AT*CGDFLT=1,\"IP\",\"%s\",,,,,,,,,,,,,,,,,,1\r\n", "CMWAP");        
        ret = SDKSendATCMD(TEL_AT_CMD_ATP_12, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
        sdk_uart_printf("%s: resp_str = %s, ret = %u\n",at_str,resp_str,ret);                

        OSATaskSleep(200);        
        //重启
        PM812_SW_RESET();        
    }    
    
#if 0
    memset(at_str, 0x00, sizeof(at_str));
    memset(resp_str, 0x00, sizeof(resp_str));    
    sprintf(at_str, "AT*CGDFAUTH=1\r");        // 查询预设的用户名密码
    ret = SDKSendATCMD(TEL_AT_CMD_ATP_12, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
    sdk_uart_printf("%s: resp_str = %s, ret = %u\n", at_str, resp_str, ret);
    
    // *CGDFAUTH:auth,"usr","passwrd"
    tempPtr = strstr(resp_str, "*CGDFAUTH");
    if (tempPtr){
        memset(tempUSRstr, 0x0, sizeof(tempUSRstr));
        memset(tempPASSWDstr, 0x0, sizeof(tempPASSWDstr));    
        //*CGDFAUTH:2,"usr","passwrd"        
        sscanf(tempPtr, "*CGDFAUTH:%d,\"%[^\"]\",\"%[^\"]\"", &tempValue, tempUSRstr, tempPASSWDstr);
        sdk_uart_printf("%s: tempValue = %d, tempUSRstr = %s, tempPASSWDstr = %s\n", __FUNCTION__, tempValue, tempUSRstr, tempPASSWDstr);
    }    
    
    if ((strncmp(tempUSRstr, "usr", strlen("usr")) != 0)
        ||(strncmp(tempPASSWDstr, "passwrd", strlen("passwrd")) != 0)){        

        memset(at_str, 0x00, sizeof(at_str));
        memset(resp_str, 0x00, sizeof(resp_str));    
        sprintf(at_str, "AT*CGDFAUTH=1,2,usr,passwrd\r");        
        ret = SDKSendATCMD(TEL_AT_CMD_ATP_12, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
        sdk_uart_printf("%s: resp_str = %s, ret = %u\n",at_str,resp_str,ret);                
        
        OSATaskSleep(200);        
        //重启
        PM812_SW_RESET();                    
    }    
#endif    
    
    wait_network_ready();
        
    memset(at_str, 0x00, sizeof(at_str));
    memset(resp_str, 0x00, sizeof(resp_str));    
    sprintf(at_str, "AT+CGDCONT?\r\n");
    ret = SDKSendATCMD(TEL_AT_CMD_ATP_12, at_str, 5, NULL,1,NULL, resp_str, sizeof(resp_str));
    sdk_uart_printf("%s: resp_str = %s, ret = %u\n",at_str,resp_str,ret);            

}

// End of file : main.c 2023-5-16 17:56:03 by: zhaoning 

