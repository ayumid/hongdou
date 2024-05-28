//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : inter_at.c
// Auther      : zhaoning
// Version     :
// Date : 2024-5-27
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-27
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "inter_at.h"
#include "lib_common.h"
#include "teldef.h"
#include "ci_dev.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static LIB_CELL_INFO_T st_lib_cell_info;
static OSFlagRef lib_cell_flag_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : lib_get_rssi
  * Description : 获取rssi
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int lib_get_rssi(void)
{
    char mmRspBuf[16] = {0};
    int err = 0;
    char *p = NULL;
    int csqValue = 0;
    int errorCnt = 0;//
wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 2, "+CSQ", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    lib_uart_printf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strlen(mmRspBuf) >= 10 )
    {
        p = strchr(mmRspBuf,':');
        csqValue = atoi(p + 1);
        
        return csqValue;
    }
    else
    {
        lib_sleep(2);
        errorCnt++;
        if(errorCnt > 1)
        {
            csqValue = 99;
            
            return csqValue;
        }    
        goto wait_reg;
    } 
}

/**
  * Function    : lib_get_imei
  * Description : 获取imei
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_get_imei(char *buffer)
{
    char mmRspBuf[24] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CGSN\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    lib_uart_printf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strlen(mmRspBuf) >= 15 )
    {
        snprintf(buffer,16,mmRspBuf);
        return;
    }
    else
    {
        lib_sleep(2);
        errorCnt++;
        if(errorCnt > 1)
        {
            memset(mmRspBuf, 0, sizeof(mmRspBuf));
            snprintf(buffer,16,mmRspBuf);
            
            return;
        }
        goto wait_reg;
    } 
}

/**
  * Function    : lib_get_imsi
  * Description : 获取imsi
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_get_imsi(char *buffer)
{
    char mmRspBuf[24] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CIMI\r\n", 2, NULL, 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    lib_uart_printf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strlen(mmRspBuf) >= 15 )
    {
        snprintf(buffer, 16, mmRspBuf);
        return;
    }
    else
    {
        lib_sleep(2);
        errorCnt++;
        if(errorCnt>1)
        {
            memset(mmRspBuf, 0, sizeof(mmRspBuf));
            snprintf(buffer,16,mmRspBuf);
            
            return;
        }
        goto wait_reg;
    } 
}

/**
  * Function    : lib_get_iccid
  * Description : 获取iccid
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_get_iccid(char *buffer)
{
    char mmRspBuf[36] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+ICCID\r\n", 2, "+ICCID", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
//    lib_uart_printf("%s[%d]mmRspBuf==>%s %d", __FUNCTION__, __LINE__, mmRspBuf, strlen(mmRspBuf));

    if(strstr(mmRspBuf, "+ICCID") != NULL )
    {
        p = strchr(mmRspBuf,':');
        snprintf(buffer,21,p+2);
        return;
    }
    else
    {
        lib_sleep(2);
        errorCnt++;
        if(errorCnt>1)
        {
            memset(mmRspBuf, 0, sizeof(mmRspBuf));
            snprintf(buffer,21,mmRspBuf);
            
            return;
        }
        goto wait_reg;
    } 
}

void cellScanReslutCallback(int celltype, void *scan_data)
{
    int index=0;
    char plmn[8] = {0};
    
    if (celltype == 0){
//        OSAFlagSet(lib_cell_flag_ref, 0x01, OSA_FLAG_OR);
    }else if (celltype == 1){
        LIB_CELL_INFO_T *sInfo = &st_lib_cell_info;
        CiDevPrimLteEngmodeScellInfoInd * pLteEngmodeScellInfoInd= (CiDevPrimLteEngmodeScellInfoInd *)scan_data;
        CiDevLteEngScellInfo *pLteEngScellInfo = &(pLteEngmodeScellInfoInd->info);
//        lib_uart_printf("%s[%d]: decode scellinfo...\n", __FUNCTION__, __LINE__);
        
        if(pLteEngmodeScellInfoInd && pLteEngmodeScellInfoInd->sCellPresent!=0){            
            if (pLteEngScellInfo->params.lenOfMnc == 3){
                sprintf(plmn, "%X%03X", pLteEngScellInfo->params.mcc, pLteEngScellInfo->params.mnc);
            }else{
                sprintf(plmn, "%X%02X", pLteEngScellInfo->params.mcc, pLteEngScellInfo->params.mnc);
            }

//            lib_uart_printf("servingcell: %s,%d,%d,%d,%d,%d,%d,%d,%d", 
//                        plmn,
//                        pLteEngScellInfo->params.tac,
//                        pLteEngScellInfo->params.cellId,
//                        pLteEngScellInfo->params.dlEuArfcn,
//                        pLteEngScellInfo->params.phyCellId,
//                        pLteEngScellInfo->meas.srxlev,
//                        pLteEngScellInfo->meas.rsrp,
//                        pLteEngScellInfo->meas.rsrq,
//                        pLteEngScellInfo->meas.sinr);
                        
            st_lib_cell_info.mcc = pLteEngScellInfo->params.mcc;                        
            st_lib_cell_info.lenOfMnc = pLteEngScellInfo->params.lenOfMnc;
            st_lib_cell_info.mnc = pLteEngScellInfo->params.mnc;            
            st_lib_cell_info.tac = pLteEngScellInfo->params.tac;
            st_lib_cell_info.cellId = pLteEngScellInfo->params.cellId;
            st_lib_cell_info.euArfcn = pLteEngScellInfo->params.dlEuArfcn;
            st_lib_cell_info.phyCellId = pLteEngScellInfo->params.phyCellId;
            st_lib_cell_info.rsrp = pLteEngScellInfo->meas.rsrp;
            st_lib_cell_info.rsrq = pLteEngScellInfo->meas.rsrq;
            st_lib_cell_info.sinr = pLteEngScellInfo->meas.sinr;

            OSAFlagSet(lib_cell_flag_ref, 0x01, OSA_FLAG_OR);
        }
    }else if (celltype == 2){
        CiDevPrimLteEngmodeIntraFreqInfoInd * pLteEngmodeIntraFreqInfoInd = (CiDevPrimLteEngmodeIntraFreqInfoInd *)scan_data;
//        lib_uart_printf("%s[%d]: decode IntraFreq ncellinfo...\n", __FUNCTION__, __LINE__);
        
        if(pLteEngmodeIntraFreqInfoInd && pLteEngmodeIntraFreqInfoInd->numIntraFreq < CI_DEV_LTE_MAX_CELL_INTRA){    
            for (index = 0; index < pLteEngmodeIntraFreqInfoInd->numIntraFreq; index++)    
            {    
                if (pLteEngmodeIntraFreqInfoInd->intraFreq[index].mcc <= 0)
                    continue;
        
                if (pLteEngmodeIntraFreqInfoInd->intraFreq[index].lenOfMnc == 3){
                    sprintf(plmn, "%X%03X", pLteEngmodeIntraFreqInfoInd->intraFreq[index].mcc, pLteEngmodeIntraFreqInfoInd->intraFreq[index].mnc);
                }else{
                    sprintf(plmn, "%X%02X", pLteEngmodeIntraFreqInfoInd->intraFreq[index].mcc, pLteEngmodeIntraFreqInfoInd->intraFreq[index].mnc);
                }

//                lib_uart_printf("\r\nIntraFreq ncellinfo: %s,%d,%d,%d,%d,%d,%d,%d,%d\r\n", 
//                            plmn,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].tac,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].cellId,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].euArfcn,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].phyCellId,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].srxlev,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].rsrp,
//                            pLteEngmodeIntraFreqInfoInd->intraFreq[index].rsrq,    
//                            -1);    
            }
        }
    }else if (celltype == 3){        
        CiDevPrimLteEngmodeInterFreqInfoInd * pLteEngmodeInterFreqInfoInd = (CiDevPrimLteEngmodeInterFreqInfoInd *)scan_data;
//        lib_uart_printf("%s[%d]: decode InterFreq ncellinfo...\n", __FUNCTION__, __LINE__);
        
        if(pLteEngmodeInterFreqInfoInd && pLteEngmodeInterFreqInfoInd->numInterFreq < CI_DEV_LTE_MAX_CELL_INTRA){    
            for (index = 0; index < pLteEngmodeInterFreqInfoInd->numInterFreq; index++)    
            {    
                if (pLteEngmodeInterFreqInfoInd->interFreq[index].mcc <= 0)
                    continue;
                if (pLteEngmodeInterFreqInfoInd->interFreq[index].lenOfMnc == 3){
                    sprintf(plmn, "%X%03X", pLteEngmodeInterFreqInfoInd->interFreq[index].mcc, pLteEngmodeInterFreqInfoInd->interFreq[index].mnc);
                }else{
                    sprintf(plmn, "%X%02X", pLteEngmodeInterFreqInfoInd->interFreq[index].mcc, pLteEngmodeInterFreqInfoInd->interFreq[index].mnc);
                }

//                lib_uart_printf("\r\nInterFreq ncellinfo: %s,%d,%d,%d,%d,%d,%d,%d,%d\r\n", 
//                            plmn,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].tac,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].cellId,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].euArfcn,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].phyCellId,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].srxlev,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].rsrp,
//                            pLteEngmodeInterFreqInfoInd->interFreq[index].rsrq,    
//                            -1);    
            }
        }    
    }    
}

/*****************************************************************
* Function: CELL_ALL_SCAN_START
*
* Description:
*     该函数用于扫描主小区和邻小区参数,扫描数据cb函数返回。
* 
* Parameters:
    
*     mode      [In]     小区信息扫描选项
*                             1: 只扫描主小区
*                             2: 扫描主小区和邻区
*     cb          [In]     cellScanExCallback,邻小区扫描数据cb函数返回。
*                     cellScanExCallback 参数描述: //扫描结束时cb参数: 0,NULL
*                         celltype: 0:扫描结束，1:主小区信息，2:同频邻区信息，3:异频邻区信息
*                         scan_data: 详见sample\8.1-cellinfo
* Return:
*     NONE
*
*
*部分参数换算说明:
*    0 rsrp < -140 dBm 
*    1 -140 dBm ≤ rsrp < -139 dBm 
*    2 -139 dBm ≤ rsrp < -138 dBm 
*    : : : :
*    95 -46 dBm ≤ rsrp < -45 dBm 
*    96 -45 dBm ≤ rsrp < -44 dBm 
*    97 -44 dBm ≤ rsrp 
*    255 not known or not detectable
*    
*    0 rsrq < -19.5 dB 
*    1 -19.5 dB ≤ rsrq < -19 dB 
*    2 -19 dB ≤ rsrq < -18.5 dB 
*    : : : : 
*    32 -4 dB ≤ rsrq < -3.5 dB 
*    33 -3.5 dB ≤ rsrq < -3 dB 
*    34 -3 dB ≤ rsrq 
*    255 not known or not detectable
*****************************************************************/
extern void CELL_SCAN_START_EX(int mode, cellScanExCallback cb);

/**
  * Function    : lib_get_cell_info
  * Description : 获取rsrp,rsrq,sinr
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void lib_get_cell_info(LIB_CELL_INFO_T* cell_info)
{   
    OSA_STATUS status = 0;
    UINT32 flag_value = 0;

    status= OSAFlagCreate(&lib_cell_flag_ref);
    ASSERT(status == OS_SUCCESS);
    
    CELL_SCAN_START_EX(2, cellScanReslutCallback);
    status = OSAFlagWait(lib_cell_flag_ref, 0xFF, OSA_FLAG_OR_CLEAR, &flag_value, 8 * 200/*OSA_SUSPEND*/);	//设置timeout秒后超时，防止task异常卡死

    if (status == OS_SUCCESS)
    {
        if (flag_value & 0x01)
        {
            memcpy(cell_info, &st_lib_cell_info, sizeof(LIB_CELL_INFO_T));
        }
    }
    else
    {
        lib_uart_printf("%s[%d]: ger cell info err", __FUNCTION__, __LINE__);
    }

//    lib_uart_printf("cell: %d,%d,%d\r\n", 
//                            st_lib_cell_info.rsrp,
//                            st_lib_cell_info.rsrq,
//                            st_lib_cell_info.sinr);

    OSAFlagDelete(lib_cell_flag_ref);
}
// End of file : inter_at.c 2024-5-27 15:16:10 by: zhaoning 

