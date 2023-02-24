//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_jhm1200.h
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
#ifndef _DRV_I2C_JHM1200_H_
#define _DRV_I2C_JHM1200_H_

// Includes ---------------------------------------------------------------------

// Public defines / typedef -----------------------------------------------------
// Define the upper and lower limits of the calibration pressure
#define PMIN 0.0  //Full range pressure for example 0Kpa
#define PMAX 2500.0 //Zero Point Pressure Value, for example 2500Kpa
#define DMIN 1677722.0 //AD value corresponding to pressure zero, for example 10%AD
#define DMAX 15099494.0 //AD Value Corresponding to Full Pressure Range, for example 90%AD
#define Device_Address  0x78 << 1

// Public functions prototypes --------------------------------------------------
/**
  * Function    : drv_jhm1200_write_data
  * Description : 设置jhm1200寄存器
  * Input       : address 地址
  *               buf	  数据
  *               count   数据长度
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
unsigned char drv_jhm1200_write_data(unsigned char address, unsigned char *buf, unsigned char count);
/**
  * Function    : drv_jhm1200_read_data
  * Description : 读取jhm1200寄存器
  * Input       : address 地址
  *               buf	  数据
  *               count   数据长度
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
unsigned char drv_jhm1200_read_data(unsigned char address, unsigned char *buf, unsigned char count);
/**
  * Function    : drv_jhm1200_init
  * Description : jhm1200初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_jhm1200_init(void);

/**
  * Function    : drv_jhm1200_isBusy
  * Description : 读取一个字节的状态字，检测状态字的busy 位是否为1
  * Input       : 
  *               
  * Output      : 
  * Return      : 状态字的busy位
  * Auther      : win
  * Others      : 
  **/
unsigned char drv_jhm1200_isBusy(void);
/**
  * Function    : drv_jhm1200_get_pressure
  * Description : 获取jhm1200的压力值
  * Input       : 
  *               
  * Output      : 
  * Return      : 压力值
  * Auther      : win
  * Others      : 
  **/
double drv_jhm1200_get_pressure(void);

#endif /* ifndef _DRV_I2C_JHM1200_H_.2021-12-28 12:32:54 by: win */

