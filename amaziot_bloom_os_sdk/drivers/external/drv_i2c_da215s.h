//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_da215s.h
// Auther      : win
// Version     :
// Date : 2022-1-4
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2022-1-4
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_I2C_DA215S_H_
#define _DRV_I2C_DA215S_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "utils_common.h"
#include "drv_simulator_i2c.h"

// Public defines / typedef -----------------------------------------------------
#define DA215S_I2C_WADDR 0x4E	//I2C写地址：7位I2C地址+一位写
#define DA215S_I2C_RADDR 0x4F	//I2C写地址：7位I2C地址+一位读


#define DA215S_X_ACCD_L_ADDR 0x02	
#define DA215S_X_ACCD_H_ADDR 0x03	
#define DA215S_Y_ACCD_L_ADDR 0x04	
#define DA215S_Y_ACCD_H_ADDR 0x05	
#define DA215S_Z_ACCD_L_ADDR 0x06	
#define DA215S_Z_ACCD_H_ADDR 0x07	

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_da215s_init
  * Description : DA215S初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_da215s_init(void);
/**
  * Function    : drv_da215s_write_byte
  * Description : DA215S向寄存器写一个字节
  * Input       : RAddr：寄存器地址
  *  			  WData：数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_da215s_write_byte(unsigned char RAddr, unsigned char WData);
/**
  * Function    : drv_da215s_read_byte
  * Description : DA215S从寄存器读一个字节
  * Input       : RAddr：寄存器地址
  *               RData：接收数据地址
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_da215s_read_byte(unsigned char RAddr, unsigned char *RData);
/**
  * Function    : drv_da215s_get_accd
  * Description : DA215S获取加速度值
  * Input       : accd_lsb：加速度值低寄存器地址
  *               accd_msb：加速度值高寄存器地址
  *               
  * Output      : 
  * Return      : 返回值accd_f：加速度值，单位mg
  * Auther      : win
  * Others      : 
  **/
int drv_da215s_get_accd(unsigned char accd_lsb,unsigned char accd_msb);
/**
  * Function    : drv_da215s_get_info
  * Description : DA215S获取xyz轴加速度值
  * Input       : x_accd：接收x轴加速度值地址
  *               y_accd：接收y轴加速度值地址
  *               2_accd：接收z轴加速度值地址
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_da215s_get_info(int *x_accd,int *y_accd,int *z_accd);

#endif /* ifndef _DRV_I2C_DA215S_H_.2022-1-4 18:19:33 by: win */

