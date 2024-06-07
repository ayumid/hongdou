//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_scck33112h6a.c
// Auther      : zhaoning
// Version     :
// Date : 2024-5-10
// Description : 本驱动适用于XL9535
//          
//          
// History     :
//     
//    1. Time         :  2024-5-10
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_scck33112h6a.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_scck33112h6a_i2c_init
  * Description : 初始化使用的第0组 i2c 总线
  * Input       : 
  *               
  * Output      : 
  * Return      : 0 成功，-1 失败
  * Auther      : zhaoning
  * Others      : 
  **/
int drv_scck33112h6a_i2c_init(void)
{
    return ql_i2c_init(DRV_SCCK33112H6A_I2C_NUM, STANDARD_MODE);
}

/**
  * Function    : drv_scck33112h6a_i2c_read
  * Description : 读scck33112h6a的寄存器内容
  * Input       : addr   寄存器地址
  *               data   读取的寄存器值
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int drv_scck33112h6a_i2c_read(unsigned char *addr, unsigned char *data)
{
    return ql_i2c_read_ext(DRV_SCCK33112H6A_I2C_NUM, DRV_SCCK33112H6A_I2C_SLAVE_ADDR, addr, 1, data, 1);
}

/**
  * Function    : drv_scck33112h6a_i2c_write
  * Description : 写xl9535的寄存器
  * Input       : addr   寄存器地址
  *               data   写入寄存器值
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int drv_scck33112h6a_i2c_write(unsigned char *addr, unsigned char *data)
{
    return ql_i2c_write_ext(DRV_SCCK33112H6A_I2C_NUM, DRV_SCCK33112H6A_I2C_SLAVE_ADDR, addr, 1, data, 1);
}

/**
  * Function    : drv_scck33112h6a_set_sleep
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void drv_scck33112h6a_set_sleep(void)
//{
//    uint8_t reg_data[2] = {0};
//    
//    reg_data[0] = 0x61;
//    reg_data[1] = 0xA0;
//    lib_i2c_write(EEPROM_DEVICE_ADDRESS << 1, EEPROM_REGISTER1_ADDRESS, reg_data, 2);
//}

/**
  * Function    : drv_scck33112h6a_set_wakeup
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void drv_scck33112h6a_set_wakeup(void)
//{
//    uint8_t reg_data[2] = {0};
//    
//    reg_data[0] = 0x60;
//    reg_data[1] = 0xA0;
//    lib_i2c_write(EEPROM_DEVICE_ADDRESS << 1, EEPROM_REGISTER1_ADDRESS, reg_data, 2);
//}

// End of file : drv_scck33112h6a.c 2024-5-10 17:11:17 by: zhaoning 

