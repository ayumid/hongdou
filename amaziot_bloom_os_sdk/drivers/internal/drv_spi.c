//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_spi.c
// Auther      : win
// Version     :
// Date : 2021-11-23
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-23
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_spi.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : drv_spi_init
  * Description : SPI初始化
  * Input       : spimode SPI模式
  *               spiclk  SPI时钟
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_spi_init(SPI_MODE_T spimode, SPI_CLK_T spiclk)
{
	SPI_Initialize(spimode,spiclk);
}
/**
  * Function    : drv_spi_readwrite_data
  * Description : SPI读写数据
  * Input       : inbuf  写入数据
  *               outbuf 读取数据
  *				  len 数据长度
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
unsigned long drv_spi_readwrite_data(unsigned char *inbuf, unsigned char *outbuf, unsigned int len)
{
	return SPI_DataTransfer(inbuf,outbuf,len);
}
/**
  * Function    : drv_spi_write_data
  * Description : SPI写数据
  * Input       : buf 写入数据
  *               len 数据长度
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
unsigned long drv_spi_write_data(unsigned char *buf, unsigned int len)
{
	return SPI_WriteData(buf,len);
}

// End of file : drv_spi.c 2021-11-23 18:08:57 by: win 

