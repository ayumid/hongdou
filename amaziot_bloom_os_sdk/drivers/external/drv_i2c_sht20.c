//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_sht20.c
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

// Includes ---------------------------------------------------------------------

#include "drv_i2c_sht20.h"
#include "drv_simulator_i2c.h"
// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
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
void drv_sht20_init(void)
{
	iicgpioinit();
}

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
void drv_sht20_write_data(unsigned char addr, unsigned char dat)
{
	  iicstart();
      delay_iic_150us();
      iicsendbyte(write_sht20_addr);
      delay_iic_150us();
      waitack();
      delay_iic_150us();
      iicsendbyte(addr);
      delay_iic_150us();
      waitack();
      iicsendbyte(dat);
      delay_iic_150us();
      waitack();
      iicstop();
      delay_iic_150us(); 
}

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
unsigned short drv_sht20_read_data(unsigned char addr)
{
	 unsigned short data = 0;
	 unsigned char checksum = 0;
     iicstart();
     delay_iic_150us();
     iicsendbyte(write_sht20_addr);
	 delay_iic_150us();
     waitack();
     delay_iic_150us();
     iicsendbyte(addr);
     delay_iic_150us();
     waitack();
     delay_iic_150us();
     iicstart();
     delay_iic_150us();
     iicsendbyte(read_sht20_addr);
     delay_iic_150us();
     waitack();
     delay_iic_150us();

	 //等待传感器内部处理完成
	 delay_iic_ms(100);
	//开始读温度数据的高八位
	 data = ((unsigned short)iicreceivebyte()) << 8;
	 sendack();
	 //读温度数据的低八位
	 data |= (unsigned short)iicreceivebyte();
	 sendack();
	 //读校验和
	 checksum = iicreceivebyte();
	 delay_iic_150us();
     sendnotack();
     delay_iic_150us();
     iicstop();
     delay_iic_150us();
     return data;
}
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
float drv_sht20_read_temp(void)
{
	unsigned short tempVlaue;
	float temprature;
	
	tempVlaue = drv_sht20_read_data(read_temp_cmd);
	
	temprature=-46.85+175.72*((float)tempVlaue/65536);

	return temprature;
}

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
float drv_sht20_read_hum(void)
{
	unsigned short humVlaue;
	float humidity;

	humVlaue = drv_sht20_read_data(read_hum_cmd);
	
	humidity =-6+125*((float)humVlaue/65536);
	return humidity;
}
// End of file : drv_i2c_sht20.h 2021-12-28 10:11:54 by: win 

