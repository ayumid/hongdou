//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : drv_wtn6x.h
// Auther      : zhaoning
// Version     :
// Date : 2022-3-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2022-3-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_WTN6X_H_
#define _DRV_WTN6X_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Public defines / typedef -----------------------------------------------------

#define                 COM_SCL                                 49
#define                 COM_SDA                                 50
#define                 COM_BUSY                                16

#define DRV_WTN6X_GPIO_HIGH               1
#define DRV_WTN6X_GPIO_LOW                0

#define                 DRIVER_I2C_SDA_OUTPUT_H()                   GpioSetLevel(COM_SDA, DRV_WTN6X_GPIO_HIGH)
#define                 DRIVER_I2C_SDA_OUTPUT_L()                   GpioSetLevel(COM_SDA, DRV_WTN6X_GPIO_LOW)

#define                 DRIVER_I2C_SCL_OUTPUT_H()                   GpioSetLevel(COM_SCL, DRV_WTN6X_GPIO_HIGH)
#define                 DRIVER_I2C_SCL_OUTPUT_L()                   GpioSetLevel(COM_SCL, DRV_WTN6X_GPIO_LOW)

#define                 DRIVER_I2C_BUSY_INPUT_R()                   GpioGetLevel(COM_BUSY)

// Public functions prototypes --------------------------------------------------

void drv_wtn60x0_busy_pin_init(void);

void drv_virtual_i2c_scl_out_dir(void);
void drv_virtual_i2c_sda_out_dir(void);
void drv_wtn6x_1_line_write(UINT8 DDATA);
void drv_wtn6x_2_line_write(UINT8 DDATA);
UINT32 drv_wtn60x0_write(UINT8 data);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_WTN6X_H_.2022-3-7 14:36:00 by: zhaoning */

