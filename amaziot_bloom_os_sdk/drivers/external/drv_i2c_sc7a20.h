//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_sc7a20.h
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
#ifndef _DRV_I2C_SC7A20_H_
#define _DRV_I2C_SC7A20_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "utils_common.h"
#include "drv_simulator_i2c.h"

// Public defines / typedef -----------------------------------------------------
#define SC7A20_I2C_WADDR 0x30	//I2C写地址：7位I2C地址+一位写
#define SC7A20_I2C_RADDR 0x31	//I2C写地址：7位I2C地址+一位读


#define SC7A20_X_ACCD_L_ADDR 0x28	
#define SC7A20_X_ACCD_H_ADDR 0x29	
#define SC7A20_Y_ACCD_L_ADDR 0x2A	
#define SC7A20_Y_ACCD_H_ADDR 0x2B	
#define SC7A20_Z_ACCD_L_ADDR 0x2C	
#define SC7A20_Z_ACCD_H_ADDR 0x2D	

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_sc7a20_init
  * Description : SC7A20初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_sc7a20_init(void);
/**
  * Function    : drv_sc7a20_write_byte
  * Description : SC7A20向寄存器写一个字节
  * Input       : RAddr：寄存器地址
  *  			  WData：数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_sc7a20_write_byte(unsigned char RAddr, unsigned char WData);
/**
  * Function    : drv_sc7a20_read_byte
  * Description : SC7A20从寄存器读一个字节
  * Input       : RAddr：寄存器地址
  *               RData：接收数据地址
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_sc7a20_read_byte(unsigned char RAddr, unsigned char *RData);
/**
  * Function    : drv_sc7a20_get_accd
  * Description : SC7A20获取加速度值
  * Input       : accd_lsb：加速度值低寄存器地址
  *               accd_msb：加速度值高寄存器地址
  *               
  * Output      : 
  * Return      : 返回值accd_f：加速度值，单位mg
  * Auther      : win
  * Others      : 
  **/
int drv_sc7a20_get_accd(unsigned char accd_lsb,unsigned char accd_msb);
/**
  * Function    : drv_sc7a20_get_info
  * Description : SC7A20获取xyz轴加速度值
  * Input       : x_accd：接收x轴加速度值地址
  *               y_accd：接收y轴加速度值地址
  *               2_accd：接收z轴加速度值地址
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_sc7a20_get_info(int *x_accd,int *y_accd,int *z_accd);

#endif /* ifndef _DRV_I2C_SC7A20_H_.2022-1-4 17:46:46 by: win */

