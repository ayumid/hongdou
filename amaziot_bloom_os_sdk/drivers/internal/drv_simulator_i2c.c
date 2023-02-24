//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_simulator_i2c.c
// Auther      : win
// Version     :
// Date : 2021-12-28
// Description :仿IIC协议   
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

// Functions --------------------------------------------------------------------
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
void delay_iic_us(int us)
{
	int cnt = 26;
	volatile int i = 0;
	while (us--) {
		i = cnt;
		while (i--);
	}
}
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
void delay_iic_150us(void)
{
	delay_iic_us(150);
}

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
void delay_iic_ms(int ms)
{
	int target_cnt = (ms << 5 ) + ms;
	int old_cnt= 0;
	if (target_cnt < 0)
		while(1);
	if (ms) {
		old_cnt = timerCountRead(TCR_2);		
		while (1) {
			if (timerCountRead(TCR_2) - old_cnt > target_cnt)
				break;
		}
	}
}
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
void iicstart(void)
{
    I2CSDA_HIGH;
    delay_iic_150us();
    I2CSCL_HIGH;
    delay_iic_150us();
    I2CSDA_LOW;              //START:when CLK is high,DATA change form high to low 
    delay_iic_150us();       //保持时间>4us
	//缺少  钳住IIC总线，准备发送或接收数据  ，是否必须
} 
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
void iicstop(void)
{
     //缺少  拉低SCL  ，是否必须
	 I2CSDA_LOW;            //SCL在低电平期间，SDA变化无效
     delay_iic_150us();
     I2CSCL_HIGH;
     delay_iic_150us();
     I2CSDA_HIGH;          //STOP:when CLK is high DATA change form low to high
     delay_iic_150us();    //保持时间>4.7us
     
}
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
void waitack(void)
{   //将SDA改为输入
	GpioSetDirection(I2CSDA, GPIO_IN_PIN);
    I2CSCL_HIGH;
    delay_iic_150us();
    while(GpioGetLevel(I2CSDA)!=0);
    I2CSCL_LOW;
    delay_iic_150us();
    I2CSDA_HIGH;
    delay_iic_150us();

}
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
void sendack(void)
{
     I2CSDA_LOW;
     delay_iic_150us();
     I2CSCL_HIGH;
     delay_iic_150us();
     I2CSCL_LOW;
     delay_iic_150us();   
      
}
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
void sendnotack(void)
{
    I2CSDA_HIGH;
    delay_iic_150us();
    I2CSCL_HIGH;
    delay_iic_150us();
    I2CSCL_LOW;
    delay_iic_150us();
    I2CSDA_LOW;
    delay_iic_150us();
}
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
void iicsendbyte(unsigned char ch)
{
     unsigned char i=8;
     while(i--)
     {
          I2CSCL_LOW;
          delay_iic_150us();
          if((ch&0x80)==0)
          {
              I2CSDA_LOW; 
          } 
          else
          {
              I2CSDA_HIGH;
          }
          ch=ch<<1;
          delay_iic_150us();
          I2CSCL_HIGH;
          delay_iic_150us();
          
     } 
     I2CSCL_LOW;
     delay_iic_150us();
     I2CSDA_HIGH;
     delay_iic_150us();
}
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
unsigned char iicreceivebyte(void)
{
    unsigned char i=8;
    unsigned char ddata=0;
    I2CSDA_HIGH;
    //将数据线改为输入
    delay_iic_150us();
    GpioSetDirection(I2CSDA, GPIO_IN_PIN);
    while(i--)
    {
         ddata<<=1;
         I2CSCL_LOW;
         delay_iic_150us();
         I2CSCL_HIGH;
         delay_iic_150us();         
         if(GpioGetLevel(I2CSDA)==0)
         {
            ddata|=0;
         }
         else
         {
            ddata|=1;
         }          
    }
    delay_iic_150us();
    I2CSCL_LOW;
    delay_iic_150us();
    I2CSDA_HIGH;
    delay_iic_150us();
    return ddata;
}
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
void iicgpioinit(void)
{
	unsigned long tempaddr = 0;
	volatile unsigned int value;
	
	tempaddr = GPIO_MFPR_ADDR(I2CSCL);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("I2CSCL value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//多功能pin脚，大部分GPIO功能都是默认最低位为0
	}	
	*((volatile unsigned long *)tempaddr) = value;
	sdk_uart_printf("I2CSCL value %lx\n", *((volatile unsigned long *)tempaddr));

	tempaddr = GPIO_MFPR_ADDR(I2CSDA);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("I2CSDA value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//多功能pin脚，大部分GPIO功能都是默认最低位为0
	}	
	*((volatile unsigned long *)tempaddr) = value;
	sdk_uart_printf("I2CSDA value %lx\n", *((volatile unsigned long *)tempaddr));	
}
// End of file : drv_simulator_i2c.h 2021-12-28 9:48:01 by: win 

