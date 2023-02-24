//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_i2c.c
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

#include "drv_i2c.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : drv_i2c_init
  * Description : I2C初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void drv_i2c_init(void)
{
	Ci2C_PINMUX_INIT();
}
/**
  * Function    : drv_i2c_write_data
  * Description : I2C写数据
  * Input       : data 数据指针
  *				  dataSize 数据大小
  *               writeSlaveAddress 从机写地址
  *               protected 是否写保护
  *               userId 用户ID
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
I2C_ReturnCode drv_i2c_write_data(UINT8 *data ,UINT16 dataSize,UINT8 writeSlaveAddress,BOOL protected,UINT16 userId)
{
	return CI2CMasterSendDataDirect(data, dataSize,writeSlaveAddress,protected,userId );
}
/**
  * Function    : drv_i2c_read_data
  * Description : I2C读数据
  * Input       : cmd 读命令指针
  *				  cmdLength 读命令长度
  *               writeSlaveAddress 从机写地址
  *               protected 是否写保护
  *               designatedRxBufferPtr 读取数据接收缓存首地址
  *				  dataSize 读取到的数据长度
  *               readSlaveAddress 从机读地址              
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
I2C_ReturnCode drv_i2c_read_data(UINT8      *cmd,UINT16 cmdLength,UINT8 writeSlaveAddress,BOOL protected,UINT8 *designatedRxBufferPtr,UINT16 dataSize,UINT8 readSlaveAddress)
{
	return CI2CMasterReceiveDataDirect (cmd,cmdLength,writeSlaveAddress,protected,designatedRxBufferPtr,dataSize,readSlaveAddress );
}


// End of file : drv_i2c.h 2021-11-23 17:49:18 by: win 

