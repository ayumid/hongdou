//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_ty_sw_ele.h
// Auther      : zhaoning
// Version     :
// Date : 2024-7-23
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-23
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_TY_SW_ELE_H_
#define _AM_TY_SW_ELE_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

typedef struct CSE7759B_DATA_S
{
    unsigned short  voltage;//当前电压值，单位为0.1V
    unsigned short  electricity;//当前电流值,单位为0.01A
    unsigned short  power;//当前电流值,单位为0.01A
    
    unsigned long   energy;//当前消耗电能值对应的脉冲个数
    unsigned short  energyCurrent;//电能脉冲当前统计值
    unsigned short  energyLast;//电能脉冲上次统计值
    unsigned char   energyFlowFlag;//电能脉冲溢出标致
    
    unsigned long   energyUnit;//0.001度点对应的脉冲个数 
    float quantity;
}CSE7759B_DATA_T;

// Public functions prototypes --------------------------------------------------

int am_ty_sw_deal_uart_data(unsigned char *inData, int recvlen);
CSE7759B_DATA_T* lib_get_cse7759b_data(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TY_SW_ELE_H_.2024-7-23 9:19:54 by: zhaoning */

