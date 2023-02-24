//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_sc7a20.c
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

// Includes ---------------------------------------------------------------------

#include "drv_i2c_sc7a20.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
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
void drv_sc7a20_init(void)
{
	iicgpioinit();

	drv_sc7a20_write_byte(0x20,0x77);  //400Hz+正常模式xyz使能
 	drv_sc7a20_write_byte(0x21,0x88);  //高通滤波：正常模式,00+滤波使能
 	drv_sc7a20_write_byte(0x23,0x88);  //读取完成再更新、2g+正常模式
}
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
void drv_sc7a20_write_byte(unsigned char RAddr, unsigned char WData)
{
 	iicstart();
	iicsendbyte(SC7A20_I2C_WADDR);
	waitack();
	iicsendbyte(RAddr);
	waitack();
	iicsendbyte(WData);
	waitack();
	iicstop();
}
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
void drv_sc7a20_read_byte(unsigned char RAddr, unsigned char *RData)
{
	iicstart();
	iicsendbyte(SC7A20_I2C_WADDR);
	waitack();
	iicsendbyte(RAddr);
	waitack();
	iicstart();
	iicsendbyte(SC7A20_I2C_RADDR);
	waitack();
	*RData = iicreceivebyte();
	iicstop();
}
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
int drv_sc7a20_get_accd(unsigned char accd_lsb,unsigned char accd_msb)
{
	unsigned char accd_l,accd_m = 0;
	unsigned int temp;
	int accd;

	drv_sc7a20_read_byte(accd_lsb,&accd_l);
	drv_sc7a20_read_byte(accd_msb,&accd_m);
	temp &= 0x0000;
	temp |= accd_m;
	temp <<= 8;
	temp &= 0xff00;
	temp |= accd_l;	
	if(temp&0x8000)
	{
		temp >>= 4;
		temp |= 0xf000;
	}
	else
	{
		temp >>= 4;
		temp &= 0x0fff;
	}
	accd = temp*1;
	return accd;
}
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
void drv_sc7a20_get_info(int *x_accd,int *y_accd,int *z_accd)
{
	//获取三轴数据
	int x,y,z = 0;
	unsigned char i = 16;
	for(i=8;i>0;i--)
	{
		x += drv_sc7a20_get_accd(SC7A20_X_ACCD_L_ADDR,SC7A20_X_ACCD_H_ADDR);
		y += drv_sc7a20_get_accd(SC7A20_Y_ACCD_L_ADDR,SC7A20_Y_ACCD_H_ADDR);
		z += drv_sc7a20_get_accd(SC7A20_Z_ACCD_L_ADDR,SC7A20_Z_ACCD_H_ADDR);
	}
	x /= 8;
	y /= 8;
	z /= 8;	
	sdk_uart_printf("x:%hd mg, y:%hd mg, z:%hd mg \r\n",x,y,z);
	*x_accd = x;
	*y_accd = y;
	*z_accd = z;
}

// End of file : drv_i2c_sc7a20.h 2022-1-4 17:45:41 by: win 

