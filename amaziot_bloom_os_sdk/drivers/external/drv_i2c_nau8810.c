//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_nau8810.c
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

// Includes ---------------------------------------------------------------------

#include "drv_i2c_nau8810.h"
// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
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
void drv_nau8810_init(void)
{
	drv_i2c_init();
}
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
I2C_ReturnCode drv_nau8810_read_data(unsigned char addr, unsigned short *value)
{
	I2C_ReturnCode status = I2C_RC_NOT_OK;
	unsigned char temp_buf[2];
	unsigned char retry_count = 5;
	unsigned char nau_reg = (addr << 1) & 0xFE;

	do {		
		status = drv_i2c_read_data(&nau_reg,
                                    1 /*cmd lenght*/,
                                    NAU8810_SLAVE_WRITE_ADDR,
                                    FALSE /*not protected*/,
                                    temp_buf,
                                    2,
                                    NAU8810_SLAVE_READ_ADDR);

		
		if (status != I2C_RC_OK)
		{
			sdk_uart_printf("status: 0x%lx", status);
		}
		else
		{
			*value = (((unsigned short)temp_buf[0]) << 8) | temp_buf[1];
			break;
		}
	} while (--retry_count);
	
	return status;
}
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
I2C_ReturnCode drv_nau8810_write_data(unsigned char addr, unsigned short data)
{

	I2C_ReturnCode status = I2C_RC_NOT_OK;
	unsigned char param_data[3] = {0x00};
	unsigned char retry_count = 5;

	param_data[0] = ((addr<<1) & 0xFE) | ((data >> 8) & 0x01);
	param_data[1] = (unsigned char)(data & 0xFF);

	do {
		status = drv_i2c_write_data(param_data,2,NAU8810_SLAVE_WRITE_ADDR,FALSE,0);
		if (status != I2C_RC_OK)
		{			 
			sdk_uart_printf("status:0x%lx, addr:0x%lx, data:0x%lx, retry_count:0x%lx",status,addr,data, retry_count);

		}
	   else
			break;
	} while (--retry_count);

	return status;
}

// End of file : drv_i2c_nau8810.h 2021-12-2 10:41:47 by: win 

