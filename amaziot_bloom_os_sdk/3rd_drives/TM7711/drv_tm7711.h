//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_tm7711.h
// Auther      : zhaoning
// Version     :
// Date : 2022-1-12
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2022-1-12
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_TM7711_H_
#define _DRV_TM7711_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Public defines / typedef -----------------------------------------------------

#define                 DRV_TM7711_CH1_10HZ                     0x01
#define                 DRV_TM7711_CH1_40HZ                     0x02
#define                 DRV_TM7711_CH2_TEMP                     0x03
#define                 DRV_TM7711_CH1_10HZ_CLK                 25
#define                 DRV_TM7711_CH1_40HZ_CLK                 27
#define                 DRV_TM7711_CH2_TEMP_CLK                 26

// Public functions prototypes --------------------------------------------------

void drv_tm7711_dout_pin_init(void);
void drv_tm7711_pdsck_pin_init(void);
UINT32 drv_tm7711_get_val(UINT8 uc_next, INT32* data);
float drv_tm7711_get_weight_value(int k, int b);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_TM7711_H_.2022-1-12 14:51:35 by: zhaoning */

