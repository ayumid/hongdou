//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_sht20.h
// Auther      : win
// Version     :
// Date : 2021-12-28
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-28
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_I2C_SHT20_H_
#define _DRV_I2C_SHT20_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "utils_common.h"

// Public defines / typedef -----------------------------------------------------
#define write_sht20_addr   0x80
#define read_sht20_addr    0x81
#define read_temp_cmd      0xE3
#define read_hum_cmd   	   0xE5

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_sht20_init
  * Description : sht20初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_sht20_init(void);

/**
  * Function    : drv_sht20_write_data
  * Description : 设置sht20寄存器
  * Input       : addr：寄存器地址
  *               dat： 写入的数据
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_sht20_write_data(unsigned char addr, unsigned char dat);
/**
  * Function    : drv_sht20_read_data
  * Description : 读取sht20寄存器
  * Input       : addr：读取地址
  *               
  * Output      : 
  * Return      : 读取到的16位数据
  * Auther      : win
  * Others      : 
  **/
unsigned short drv_sht20_read_data(unsigned char addr);
/**
  * Function    : drv_sht20_read_temp
  * Description : 读取温度值
  * Input       : 
  *               
  * Output      : 
  * Return      : 温度值
  * Auther      : win
  * Others      : 
  **/
float drv_sht20_read_temp(void);
/**
  * Function    : drv_sht20_read_hum
  * Description : 读取湿度值
  * Input       : 
  *               
  * Output      : 
  * Return      : 湿度值
  * Auther      : win
  * Others      : 
  **/
float drv_sht20_read_hum(void);


#endif /* ifndef _DRV_I2C_SHT20_H_.2021-12-28 10:11:59 by: win */

