//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_scck33112h6a.h
// Auther      : zhaoning
// Version     :
// Date : 2024-5-10
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-10
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_SCCK33112H6A_H_
#define _DRV_SCCK33112H6A_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "ql_i2c_api.h"

// Public defines / typedefs ----------------------------------------------------

//参考ql_i2c_api.h，这里使用num 0 ，即48引脚，gpio49 scl；65引脚，gpio50 sda
#define             DRV_SCCK33112H6A_I2C_NUM                        0
#define             DRV_SCCK33112H6A_I2C_SLAVE_ADDR                 0x48// 从机设备地址

#define             DRV_SCCK33112H6A_REGISTER0_ADDRESS              0x00// 温度寄存器地址
#define             DRV_SCCK33112H6A_REGISTER1_ADDRESS              0x01// 配置寄存器地址
#define             DRV_SCCK33112H6A_REGISTER2_ADDRESS              0x02// T low寄存器地址
#define             DRV_SCCK33112H6A_REGISTER3_ADDRESS              0x03// T high寄存器地址

#define             DRV_SCCK33112H6A_TEMP_SIGNED                    0x800
#define             DRV_SCCK33112H6A_TEMP_MSK                       0x7ff

#define             DRV_SCCK33112H6A_TEMP_1LSB                      0.0625f

// Public functions prototypes --------------------------------------------------

int drv_scck33112h6a_i2c_init(void);
int drv_scck33112h6a_i2c_read(unsigned char *addr, unsigned char *data);
int drv_scck33112h6a_i2c_write(unsigned char *addr, unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_XL9535_H_.2024-5-10 17:11:19 by: zhaoning */

