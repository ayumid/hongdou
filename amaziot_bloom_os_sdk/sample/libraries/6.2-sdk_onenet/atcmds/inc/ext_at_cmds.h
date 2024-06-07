/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ext_at_cmds.h
  Version       : Initial Draft
  Author        : litang
  Created       : 2021/3/30
  Last Modified :
  Description   : ext_at_cmds.h
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

#ifndef _EXT_AT_CMDS_H_
#define _EXT_AT_CMDS_H_

RETURNCODE_T  AtEXTONOFF(			 const utlAtParameterOp_T		 op,
		const char						*command_name_p,
		const utlAtParameterValue_P2c	parameter_values_p,
		const size_t					num_parameters,
		const char						*info_text_p,
		unsigned int					*xid_p,
		void							*arg_p);


RETURNCODE_T  AtEXTSWVER(			 const utlAtParameterOp_T		 op,
		const char						*command_name_p,
		const utlAtParameterValue_P2c	parameter_values_p,
		const size_t					num_parameters,
		const char						*info_text_p,
		unsigned int					*xid_p,
		void							*arg_p);

RETURNCODE_T  AtEXTCONFIG(			 const utlAtParameterOp_T		 op,
		const char						*command_name_p,
		const utlAtParameterValue_P2c	parameter_values_p,
		const size_t					num_parameters,
		const char						*info_text_p,
		unsigned int					*xid_p,
		void							*arg_p);

#endif
