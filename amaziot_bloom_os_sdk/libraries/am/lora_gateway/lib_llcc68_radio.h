//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : lib_llcc68_radio.h
// Auther      : zhaoning
// Version     :
// Date : 2023-7-21
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-7-21
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _LIB_LLCC68_RADIO_H_
#define _LIB_LLCC68_RADIO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

#include "lib_llcc68.h"

// Public defines / typedefs ----------------------------------------------------

/*!
 * \brief Structure describing the radio status
 */

/*!
 * \brief Structure describing the error codes for callback functions
 */
typedef enum
{
    IRQ_HEADER_ERROR_CODE                   = 0x01,
    IRQ_SYNCWORD_ERROR_CODE                 = 0x02,
    IRQ_CRC_ERROR_CODE                      = 0x04,
}IrqErrorCode_t;

enum IrqPblSyncHeaderCode_t
{
    IRQ_PBL_DETECT_CODE                     = 0x01,
    IRQ_SYNCWORD_VALID_CODE                 = 0x02,
    IRQ_HEADER_VALID_CODE                   = 0x04,
};

enum IrqTimeoutCode_t
{
    IRQ_RX_TIMEOUT                          = 0x00,
    IRQ_TX_TIMEOUT                          = 0x01,
    IRQ_XYZ                                 = 0xFF,
};

// Public functions prototypes --------------------------------------------------

    void txDone( void )  __attribute__((weak));                     
    void rxDone( void )  __attribute__((weak));            
    void rxPreambleDetect( void )  __attribute__((weak)); 
    void rxSyncWordDone( void )  __attribute__((weak));  
    void rxHeaderDone( void )  __attribute__((weak)); 
    void txTimeout( void )  __attribute__((weak)); 
    void rxTimeout( void )  __attribute__((weak));
    void rxError( IrqErrorCode_t errCode )  __attribute__((weak));   
    void cadDone( bool cadFlag )  __attribute__((weak));

#ifdef __cplusplus
}
#endif

#endif /* ifndef _LIB_LLCC68_RADIO_H_.2023-7-21 16:37:52 by: zhaoning */

