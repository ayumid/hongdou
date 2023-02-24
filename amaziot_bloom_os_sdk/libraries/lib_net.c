//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : lib_net.c
// Auther      : win
// Version     :
// Date : 2021-11-24
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-24
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "lib_net.h"
#include "utils_common.h"
// Private defines / typedefs ---------------------------------------------------
extern BOOL IsAtCmdSrvReady(void);
extern int SendATCMDWaitResp(int sATPInd,char *in_str, int timeout, char *ok_fmt, int ok_flag,
                            char *err_fmt, char *out_str, int resplen);
// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : lib_net_tcpip_ok
  * Description : 等待网络就绪
  * Input       : timeout     超时时间，单位s
  *               
  * Output      : 
  * Return      : 成功: 0   失败： -1
  * Auther      : win
  * Others      : 
  **/

int lib_net_tcpip_ok(int timeout)
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
    
    if(strstr(mmRspBuf, "^SYSINFO: 2,3") != NULL || strstr(mmRspBuf, "^SYSINFO: 2,2") != NULL){
		errorCnt = 0;
		return 0;
    }else{
        sleep(1);
		memset(mmRspBuf, 0, sizeof(mmRspBuf));
	    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10+36, "AT^SYSINFO\r\n", 3, "^SYSINFO", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
	    
	    if(strstr(mmRspBuf, "^SYSINFO: 2,3") != NULL || strstr(mmRspBuf, "^SYSINFO: 2,2") != NULL){
			errorCnt = 0;
			return 0;
	    }else{
			errorCnt++;
			if(errorCnt>timeout){
				errorCnt = 0;
				return -1;
			}	
	        goto wait_reg;
		}
		
    } 
}

// End of file : lib_net.h 2021-11-24 10:19:23 by: win 

