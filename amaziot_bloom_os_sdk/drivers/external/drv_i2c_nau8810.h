//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_nau8810.h
// Auther      : win
// Version     :
// Date : 2021-12-2
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-2
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_I2C_NAU8810_H_
#define _DRV_I2C_NAU8810_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "drv_i2c.h"
#include "utils_common.h"

// Public defines / typedef -----------------------------------------------------
#define NAU8810_I2C_SLAVE_ADDR               0x34    //7-MSB bits: 0011_010
#define NAU8810_SLAVE_WRITE_ADDR             0x34
#define NAU8810_SLAVE_READ_ADDR              0x35

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_nau8810_init
  * Description : nau8810初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_nau8810_init(void);
/**
  * Function    : drv_nau8810_read_data
  * Description : 读nau8810数据
  * Input       : addr  读地址
  *               value 读取数据接收缓存
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
I2C_ReturnCode drv_nau8810_read_data(unsigned char addr, unsigned short *value);
/**
  * Function    : drv_nau8810_write_data
  * Description : 写nau8810数据
  * Input       : addr 写地址
  *               data 写入数据
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
I2C_ReturnCode drv_nau8810_write_data(unsigned char addr, unsigned short data);

#endif /* ifndef _DRV_I2C_NAU8810_H_.2021-12-2 10:56:00 by: win */

