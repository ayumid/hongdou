/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ext_at_controller.c
  Version       : Initial Draft
  Author        : litang
  Created       : 2021/3/30
  Last Modified :
  Description   : ext_at_controller.c
  Function List :
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


/******************************************************************************
 *   include files
 ******************************************************************************/ 
#include <stdio.h>
#include <osa.h>
#include "stdlib.h"
#include "string.h"
#include "telcontroller.h"
#include "telatci.h"
#include "telconfig.h"
#include "telatparamdef.h"
#include "teldbg.h"
#include "telcc.h"
#include "utlMalloc.h"
#include "utlTrace.h"
#include "diag.h"
#include "diag_API.h"
#include "cimodem.h"
#include "telutl.h"
#include "mat.h"
#include "ext_at_cmds.h"
typedef enum
{
	EXT_MAT_EXTAT_MIN =  MAT_EXTAT_MIN,
	EXT_MAT_EXTONOFF,
	EXT_MAT_EXTSWVER,
	EXT_MAT_EXTCONFIG,
	
	EXT_MAT_EXTAT_MAX =  MAT_EXTAT_MAX,
}ext_mat_id_e;


/******************************************************************************
 *   at cmds params type
 ******************************************************************************/

utlAtParameter_T ext_one_decimal_params[] = {
	utlDEFINE_DECIMAL_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
};

utlAtParameter_T ext_one_string_params[] = { 
	utlDEFINE_STRING_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
};

utlAtParameter_T ext_plusEXTCONFIG_params[] = { 
	utlDEFINE_DECIMAL_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_DECIMAL_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),
	utlDEFINE_STRING_AT_PARAMETER( utlAT_PARAMETER_ACCESS_READ_WRITE, utlAT_PARAMETER_PRESENCE_OPTIONAL),	
};


/******************************************************************************
 *   register at cmds
 *	
 ******************************************************************************/

utlAtCommand_T ext_shell_commands[] = {
	utlDEFINE_EXTENDED_EXACTION_VSYNTAX_AT_COMMAND((MATCmdType)EXT_MAT_EXTONOFF, "+EXTONOFF", ext_one_decimal_params, AtEXTONOFF, AtEXTONOFF, AtEXTONOFF),	
	utlDEFINE_EXTENDED_EXACTION_VSYNTAX_AT_COMMAND((MATCmdType)EXT_MAT_EXTSWVER, "+EXTSWVER", ext_one_string_params, AtEXTSWVER, AtEXTSWVER, AtEXTSWVER),	
	utlDEFINE_EXTENDED_EXACTION_VSYNTAX_AT_COMMAND((MATCmdType)EXT_MAT_EXTCONFIG, "+EXTCONFIG", ext_plusEXTCONFIG_params, AtEXTCONFIG, AtEXTCONFIG, AtEXTCONFIG),	
};


unsigned int ext_shell_commands_num = utlNumberOf(ext_shell_commands);;

