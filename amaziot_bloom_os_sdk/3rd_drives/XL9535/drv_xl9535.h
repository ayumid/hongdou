//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_xl9535.h
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
#ifndef _DRV_XL9535_H_
#define _DRV_XL9535_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

#include "ql_i2c_api.h"

// Public defines / typedefs ----------------------------------------------------

//参考ql_i2c_api.h，这里使用num 0 ，即48引脚，gpio49 scl；65引脚，gpio50 sda
#define             DRV_XL9535_I2C_NUM              0
#define             DRV_XL9535_I2C_SLAVE_ADDR       0x20

#define             DRV_XL9535_REG0_ADDR             0x00
#define             DRV_XL9535_REG1_ADDR             0x01
#define             DRV_XL9535_REG2_ADDR             0x02
#define             DRV_XL9535_REG3_ADDR             0x03
#define             DRV_XL9535_REG4_ADDR             0x04
#define             DRV_XL9535_REG5_ADDR             0x05
#define             DRV_XL9535_REG6_ADDR             0x06
#define             DRV_XL9535_REG7_ADDR             0x07

#define             DRV_XL9535_GPIO_ISR_PIN_NUM        126

//注意，下面的宏定义是基于全网通信号测试仪来定义的，寄存器值对应的IO如下，如果修改了硬件连接，下面定义也会改变
// DOWN_EN UP_EN MUL_EN 这3个IO状态改变，对应寄存器1 的值发生改变，默认0x07
#define             DRV_XL9535_BUTTON_DOWN_EN        0x03
#define             DRV_XL9535_BUTTON_UP_EN          0x05
#define             DRV_XL9535_BUTTON_SIG_EN         0x06

//SIG_EN READ_EN DE_EN CK_EN SIM_EN GNSS_EN NB_EN 4G_EN 这8个IO状态改变，对应寄存器0 的值发生改变，默认0xff
#define             DRV_XL9535_BUTTON_MUL_EN         0x7f
#define             DRV_XL9535_BUTTON_READ_EN        0xbf
#define             DRV_XL9535_BUTTON_DE_EN          0xdf
#define             DRV_XL9535_BUTTON_CK_EN          0xef
#define             DRV_XL9535_BUTTON_SIM_EN         0xf7
#define             DRV_XL9535_BUTTON_GNSS_EN        0xfb
#define             DRV_XL9535_BUTTON_NB_EN          0xfd
#define             DRV_XL9535_BUTTON_4G_EN          0xfe

// Public functions prototypes --------------------------------------------------

int drv_xl9535_i2c_init(void);
int drv_xl9535_i2c_read(unsigned char *addr, unsigned char *data);
int drv_xl9535_i2c_write(unsigned char *addr, unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_XL9535_H_.2024-5-10 17:11:19 by: zhaoning */

