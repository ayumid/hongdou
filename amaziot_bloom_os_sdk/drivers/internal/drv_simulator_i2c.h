//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_simulator_i2c.h
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
#ifndef _DRV_SIMULATOR_I2C_H_
#define _DRV_SIMULATOR_I2C_H_

// Includes ---------------------------------------------------------------------
#include "timer.h"
#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"

// Public defines / typedef -----------------------------------------------------
#define I2CSDA        26
#define I2CSCL        25	

// Public functions prototypes --------------------------------------------------
/**
  * Function    : delay_iic_us
  * Description : us级延时
  * Input       : us 延时us数值
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void delay_iic_us(int us);
/**
  * Function    : delay_iic_150us
  * Description : 延时150us
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void delay_iic_150us(void);
/**
  * Function    : delay_iic_ms
  * Description : ms级延时
  * Input       : ms 延时ms数值
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void delay_iic_ms(int ms);
/**
  * Function    : iicstart
  * Description : IIC发送起始信号
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void iicstart(void);
/**
  * Function    : iicstop
  * Description : IIC发送结束信号
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void iicstop(void);
/**
  * Function    : waitack
  * Description : 等待应答
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void waitack(void);
/**
  * Function    : sendack
  * Description : 发送应答信号
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void sendack(void);
/**
  * Function    : sendnotack
  * Description : 无应答
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void sendnotack(void);
/**
  * Function    : iicsendbyte
  * Description : IIC发送一个8位数据
  * Input       : ch：发送的数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void iicsendbyte(unsigned char ch);
/**
  * Function    : iicreceivebyte
  * Description : IIC接收一个8位数据
  * Input       : None
  *               
  * Output      : None
  * Return      : 读取到的数据
  * Auther      : win
  * Others      : 
  **/
unsigned char iicreceivebyte(void);
/**
  * Function    : iicgpioinit
  * Description : IIC对应GPIO初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void iicgpioinit(void);

#endif /* ifndef _DRV_SIMULATOR_I2C_H_.2021-12-28 9:48:07 by: win */

