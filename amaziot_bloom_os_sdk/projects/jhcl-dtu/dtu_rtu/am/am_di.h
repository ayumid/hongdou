//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_di.h
// Auther      : zhaoning
// Version     :
// Date : 2023-8-29
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-29
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_DI_H_
#define _AM_DI_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_di_task_init(void);

void dtu_di_int_init1(void);
void dtu_di_int_init2(void);
void dtu_di_int_init3(void);
void dtu_di_int_init4(void);
void dtu_di_int_init5(void);
void dtu_di_int_init_all(void);

UINT8 dtu_di_read_pin(UINT8 channel);

void dtu_di1_report_timer_stop(void);
void dtu_di2_report_timer_stop(void);
void dtu_di3_report_timer_stop(void);
void dtu_di4_report_timer_stop(void);
void dtu_di5_report_timer_stop(void);

void dtu_di1_report_timer_start(UINT32 time);
void dtu_di2_report_timer_start(UINT32 time);
void dtu_di3_report_timer_start(UINT32 time);
void dtu_di4_report_timer_start(UINT32 time);
void dtu_di5_report_timer_start(UINT32 time);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_DI_H_.2023-8-29 10:15:37 by: zhaoning */

