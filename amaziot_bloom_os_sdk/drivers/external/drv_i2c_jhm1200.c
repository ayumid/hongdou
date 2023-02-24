//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c_jhm1200.c
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

#include "drv_i2c_jhm1200.h"
#include "drv_simulator_i2c.h"
#include "utils_common.h"
// Private defines / typedefs ---------------------------------------------------
#define I2CSDA_HIGH   {GpioSetDirection(I2CSDA, GPIO_OUT_PIN);GpioSetLevel(I2CSDA,1);}
#define I2CSDA_LOW    {GpioSetDirection(I2CSDA, GPIO_OUT_PIN);GpioSetLevel(I2CSDA,0);}
#define I2CSCL_HIGH   {GpioSetDirection(I2CSCL, GPIO_OUT_PIN);GpioSetLevel(I2CSCL,1);}
#define I2CSCL_LOW    {GpioSetDirection(I2CSCL, GPIO_OUT_PIN);GpioSetLevel(I2CSCL,0);}
// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
void Set_SDA_OUTPUT(void)
{
	GpioSetDirection(I2CSDA, GPIO_OUT_PIN);
}
void Set_SDA_INPUT(void)
{
	GpioSetDirection(I2CSDA, GPIO_IN_PIN);
}
//Start signal
void Start(void)
{
	I2CSDA_HIGH;
	delay_iic_us(2);
	I2CSDA_LOW;
	delay_iic_us(2);
	I2CSDA_LOW;
	delay_iic_us(2);
	I2CSCL_LOW;
}
//Stop signal
void Stop(void)
{
	I2CSDA_LOW;
	delay_iic_us(2);
	I2CSCL_HIGH;
	delay_iic_us(2);
	I2CSDA_HIGH;
	delay_iic_us(2);
}

//Read ACK signal
unsigned char Check_ACK(void)
{
	unsigned char ack;
	Set_SDA_INPUT();
	I2CSCL_HIGH;
	delay_iic_us(2);
	ack = GpioGetLevel(I2CSDA);
	I2CSCL_LOW;
	delay_iic_us(2);
	return ack;
}

//Send ACK signal
void Send_ACK(void)
{
	I2CSDA_LOW;
	delay_iic_us(2);
	I2CSCL_HIGH;
	delay_iic_us(2);
	I2CSCL_LOW;
	delay_iic_us(2);
}

//Send one byte
void SendByte(unsigned char byte)
{
	unsigned char i = 0;
	do
	{
		if (byte & 0x80)
		{
			I2CSDA_HIGH;
		}
		else
		{
			I2CSDA_LOW;
		}
		delay_iic_us(2);
		I2CSCL_HIGH;
		delay_iic_us(2);
		byte <<= 1;
		i++;
		I2CSCL_LOW;
	} while (i < 8);
	I2CSCL_LOW;
}


//Receive one byte
unsigned char ReceiveByte(void)
{
	unsigned char i = 0, tmp = 0;
	Set_SDA_INPUT();
	do
	{
		tmp <<= 1;
		I2CSCL_HIGH;
		delay_iic_us(2);
		if (GpioGetLevel(I2CSDA))
		{
			tmp |= 1;
		}
		I2CSCL_LOW;
		delay_iic_us(2);
		i++;
	} while (i < 8);
	return tmp;
}
// Functions --------------------------------------------------------------------

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
unsigned char drv_jhm1200_write_data(unsigned char address, unsigned char *buf, unsigned char count)
{
	unsigned char timeout, ack;
	address &= 0xFE;
	Start();
	delay_iic_us(2);
	SendByte(address);
	Set_SDA_INPUT();
	delay_iic_us(2);
	timeout = 0;
	do
	{
		ack = Check_ACK();
		timeout++;
		if (timeout == 10)
		{
			Stop();
			return 1;
		}
	} while (ack);
	while (count)
	{
		SendByte(*buf);
		Set_SDA_INPUT();
		delay_iic_us(2);
		timeout = 0;
		do
		{
			ack = Check_ACK();
			timeout++;
			if (timeout == 10)
			{
				sdk_uart_printf("dmhcollect:waterPressure Check_ACK error!");//dmh test
				return 2; 
			}
		} while (0);
		buf++;
		count--;
	}
	Stop();
	return 0;
}

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
unsigned char drv_jhm1200_read_data(unsigned char address, unsigned char *buf, unsigned char count)
{
	unsigned char timeout, ack;
	address |= 0x01;
	Start();
	SendByte(address);
	Set_SDA_INPUT();
	delay_iic_us(2);
	timeout = 0;
	do
	{
		ack = Check_ACK();
		timeout++;
		if (timeout == 4)
		{
			Stop();
			sdk_uart_printf("dmhcollect:waterPressure Check_ACK error!");//dmh test
			return 1; 
		}
	} while (ack);
	delay_iic_us(2);
	while (count)
	{
		*buf = ReceiveByte();
		if (count != 1)
			Send_ACK();
		buf++;
		count--;
	}
	Stop();
	return 0;
}
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
void drv_jhm1200_init(void)
{
	iicgpioinit();
}

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
unsigned char drv_jhm1200_isBusy(void)
{
	unsigned char status;
	drv_jhm1200_read_data(Device_Address, &status, 1);
	
	if(status == 0)
		return 1;
	
	status = (status >> 5) & 0x01;
	return status;
}
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
double drv_jhm1200_get_pressure(void)
{
	unsigned char buffer[6] = {0};
	unsigned int  Dtest = 0;
	unsigned short int temp_raw = 0;
	double pressure = 0.0, temp = 0.0;

	
	buffer[0] = 0xAC;
	drv_jhm1200_write_data(Device_Address, buffer, 1);
	delay_iic_ms(5);
	while (1)
	{
		if (drv_jhm1200_isBusy())
		{
			delay_iic_ms(1);
		}
		else
			break;
	}
	if(drv_jhm1200_read_data(Device_Address, buffer, 6)>0)
	{
		sdk_uart_printf("dmhcollect:waterPressure error!");
	}

	sdk_uart_printf("dmhcollect:waterPressure,buffer:%2X,%2X,%2X,%2X,%2X,%2X",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);
	
	//The returned pressure and temperature values are converted into actual values according to the calibration range
	Dtest = ((unsigned int)buffer[1] << 16) | ((unsigned short int)buffer[2] << 8) | buffer[3];
	temp_raw = ((unsigned short int)buffer[4] << 8) | (buffer[5] << 0);
	pressure =(double)(PMAX-PMIN)/(DMAX-DMIN)*(Dtest-DMIN)+PMIN;
	temp = (double)temp_raw / 65536;
	temp = temp * 19000 - 4000;

	return pressure;
	
}

// End of file : drv_i2c_jhm1200.h 2021-12-28 12:32:48 by: win 

