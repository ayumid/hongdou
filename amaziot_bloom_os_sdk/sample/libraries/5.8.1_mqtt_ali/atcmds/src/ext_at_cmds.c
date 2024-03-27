/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ext_at_cmds.c
  Version       : Initial Draft
  Author        : litang
  Created       : 2021/3/30
  Last Modified :
  Description   : ext_at_cmds.c
  Function List :
              AtEXTCONFIG
              AtEXTONOFF
              AtEXTSWVER
  History       :
  1.Date        : 2021/3/30
    Author      : litang
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "teldef.h"
#include "telatparamdef.h"
#include "telatci.h"
#include "utlMalloc.h"
#include "download.h"
#include "FDI_TYPE.h"
#include "FDI_FILE.h"
#include "qspi_flash.h"
#include "sys_version.h"
#include "fattypes.h"
#include "I2C.h"
#include "diag_nvm.h"
#include "pmic_rtc.h"
#include "main.h"
#include "telutl.h"

/******************************************************************************
 *   at functions
 ******************************************************************************/


RETURNCODE_T  AtEXTONOFF(             const utlAtParameterOp_T         op,
        const char                        *command_name_p,
        const utlAtParameterValue_P2c    parameter_values_p,
        const size_t                    num_parameters,
        const char                        *info_text_p,
        unsigned int                    *xid_p,
        void                            *arg_p)
{
    UNUSEDPARAM(command_name_p);
    UNUSEDPARAM(info_text_p);

    TelAtParserID sAtpIndex = * (TelAtParserID *) arg_p;
    UINT32 atHandle = MAKE_AT_HANDLE( sAtpIndex );
    RETURNCODE_T ret = utlSUCCESS;
    
    static int onoff = 0;    
    
    char buf[MAX_STRING_LEN]= { 0 };
    UINT8 validCmd = TRUE;

    yuge_printf("%s: atHandle = %d, sAtpIndex = %d.\n", __FUNCTION__, atHandle, sAtpIndex);

    *xid_p = atHandle;
    switch(op)
    {
        case TEL_EXT_GET_CMD:        //AT+EXTONOFF?
        case TEL_EXT_ACTION_CMD:    //AT+EXTONOFF

            snprintf(buf, MAX_STRING_LEN, "+EXTONOFF: %d", onoff);
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, buf);
            break;

        case TEL_EXT_SET_CMD:    //AT+EXTONOFF=1
            if( getExtValue( parameter_values_p, 0, (int *)&onoff, 0, 1, 0) == FALSE )
                validCmd = FALSE;
            
            if (validCmd){
                ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
            }else{
                ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
            }            
            break;
        case TEL_EXT_TEST_CMD:    //AT+EXTONOFF=?
            snprintf(buf, MAX_STRING_LEN, "+EXTONOFF: (0-1)");            
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);                        
            break;

        default:
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
            break;
    }
        
    return ret;
}


RETURNCODE_T  AtEXTSWVER(             const utlAtParameterOp_T         op,
        const char                        *command_name_p,
        const utlAtParameterValue_P2c    parameter_values_p,
        const size_t                    num_parameters,
        const char                        *info_text_p,
        unsigned int                    *xid_p,
        void                            *arg_p)
{
    UNUSEDPARAM(command_name_p);
    UNUSEDPARAM(info_text_p);

    TelAtParserID sAtpIndex = * (TelAtParserID *) arg_p;
    UINT32 atHandle = MAKE_AT_HANDLE( sAtpIndex );
    RETURNCODE_T ret = utlSUCCESS;
    
    static char extswver[MAX_STRING_LEN] = "CLM920_CAT1_SDK";
    
    char extswver_str[MAX_STRING_LEN] = {0};
    UINT16 extswver_str_len = 0;
    
    char buf[MAX_STRING_LEN]= { 0 };
    UINT8 validCmd = TRUE;

    yuge_printf("%s: atHandle = %d, sAtpIndex = %d.\n", __FUNCTION__, atHandle, sAtpIndex);

    *xid_p = atHandle;
    switch(op)
    {
        case TEL_EXT_GET_CMD:    
        case TEL_EXT_ACTION_CMD:
            snprintf(buf, MAX_STRING_LEN, "BUILDTIME: [%s %s]", BUILD_DATE, BUILD_TIME);
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, buf);
            break;

        case TEL_EXT_SET_CMD:
            if (getExtString(parameter_values_p, 0, extswver_str, MAX_STRING_LEN, (INT16 *)&extswver_str_len, NULL) == FALSE ){
                
                validCmd = FALSE;
            }
            
            if ((validCmd) && (extswver_str_len <= MAX_STRING_LEN))    {
                memset(extswver, 0x0, sizeof(extswver));
                memcpy(extswver, extswver_str, extswver_str_len);
                ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
            }else{
                ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
            }            
            break;
        case TEL_EXT_TEST_CMD:
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);                        
            break;
        default:
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
            break;
    }
        
    return ret;
}

RETURNCODE_T  AtEXTCONFIG(             const utlAtParameterOp_T         op,
        const char                        *command_name_p,
        const utlAtParameterValue_P2c    parameter_values_p,
        const size_t                    num_parameters,
        const char                        *info_text_p,
        unsigned int                    *xid_p,
        void                            *arg_p)
{
    UNUSEDPARAM(command_name_p);
    UNUSEDPARAM(info_text_p);

    RETURNCODE_T ret = utlSUCCESS;    
    RETURNCODE_T rc = INITIAL_RETURN_CODE;
    TelAtParserID sAtpIndex = * (TelAtParserID *) arg_p;
    UINT32 atHandle = MAKE_AT_HANDLE( sAtpIndex );

    static int param_0    = 0;    
    static int param_1    = 0;    
    
    static char param_2[1024] = {0};    
    int param_2_len  = 0;    
    
    char buf[MAX_STRING_LEN]= { 0 };
    UINT8 validCmd = TRUE;

    *xid_p = atHandle;
    switch(op)
    {
        case TEL_EXT_GET_CMD:
        case TEL_EXT_ACTION_CMD:
            sprintf(buf,"+EXTCONFIG: %d,%d,%s", param_0, param_1, param_2);
            ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, buf);
            break;

        case TEL_EXT_SET_CMD:
            
            if( getExtValue( parameter_values_p, 0, (int *)&param_0, 0, 66535, 0) == FALSE )
                validCmd = FALSE;

            if( getExtValue( parameter_values_p, 1, (int *)&param_1, 0, 66535, 0) == FALSE )
                validCmd = FALSE;
            
            if( getExtString( parameter_values_p, 2, param_2, 1024, (INT16 *)&param_2_len, NULL ) == FALSE )
                validCmd = FALSE;        
                        
            if (validCmd)
            {                    
                ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
            }else{
                ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
            }
            break;
            
        case TEL_EXT_TEST_CMD:
        default:
            ret = ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
            break;
    }
    
    /* handle the return value */
    rc = HANDLE_RETURN_VALUE(ret);
    return rc;
}



