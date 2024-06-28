//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_virtual_i2c.c
// Auther      : zhaoning
// Version     :
// Date : 2022-1-13
// Description : 标准IIC协议传输数据时为MSB方式，即高位在前低位在后，但有些器件为LSB方式，
//          
//          
// History     :
//     
//    1. Time         :  2022-1-13
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <math.h>

#include "drv_sc7a20.h"
#include "drv_3rd.h"

// Private defines / typedefs ---------------------------------------------------

#define PI 3.1415926535898

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_sc7a20_i2c_init
  * Description : 初始化使用的第0组 i2c 总线
  * Input       : 
  *               
  * Output      : 
  * Return      : 0 成功，-1 失败
  * Auther      : zhaoning
  * Others      : 
  **/
int drv_sc7a20_i2c_init(void)
{
    return ql_i2c_init(DRV_SC7A20_I2C_NUM, STANDARD_MODE);
}

/**
  * Function    : drv_sc7a20_i2c_read
  * Description : 读sc7a20的寄存器内容
  * Input       : addr   寄存器地址
  *               data   读取的寄存器值
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int drv_sc7a20_i2c_read(unsigned char *addr, unsigned char *data)
{
    return ql_i2c_read_ext(DRV_SC7A20_I2C_NUM, DRV_SC7A20_I2C_SLAVE_ADDR, addr, 1, data, 1);
}

/**
  * Function    : drv_sc7a20_i2c_write
  * Description : 写sc7a20的寄存器
  * Input       : addr   寄存器地址
  *               data   写入寄存器值
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int drv_sc7a20_i2c_write(unsigned char *addr, unsigned char *data)
{
    return ql_i2c_write_ext(DRV_SC7A20_I2C_NUM, DRV_SC7A20_I2C_SLAVE_ADDR, addr, 1, data, 1);
}


/**
  * Function    : drv_sc7a20_get_acc
  * Description : 获取三轴加速度数值，精度12bit
  * Input       : pXa    X轴加速度存储区指针
  *               pYa    Y轴加速度存储区指针
  *               pZa    Z轴加速度存储区指针
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
OSA_STATUS drv_sc7a20_get_acc(INT16* pXa, INT16* pYa, INT16* pZa)
{
    UINT8 buff[6] = {0};
    UINT8 i = 0;
    unsigned char RegAddr = 0;

    for(i = 0; i < 6; i++)
    {
        RegAddr = 0X28 + i;
        drv_sc7a20_i2c_read(&RegAddr, &buff[i]);
    }
    //X轴
    *pXa = buff[1];
    *pXa <<= 8;
    *pXa |= buff[0];
    *pXa >>= 4;//取12bit精度

    //Y轴
    *pYa = buff[3];
    *pYa <<= 8;
    *pYa |= buff[2];
    *pYa >>= 4;//取12bit精度

    //Z轴
    *pZa = buff[5];
    *pZa <<= 8;
    *pZa |= buff[4];
    *pZa >>= 4;//取12bit精度

    return TRUE;

}

/**
  * Function    : drv_sc7a20_get_pitch
  * Description : 获取俯仰角
  * Input       : x    X轴加速度
  *               y    Y轴加速度
  *               z    Z轴加速度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
float drv_sc7a20_get_pitch(INT16 x, INT16 y, INT16 z)
{
    float pitch = 0;
    UINT16 maxG = 0; 

    pitch = atan2((float)(0 - y), z) * 180 / PI;     //转换为度数

//    drv_3rd_uart_printf("pitch:%3d", pitch);
    return pitch;
}

/**
  * Function    : drv_sc7a20_get_roll
  * Description : 获取横滚角
  * Input       : x    X轴加速度
  *               y    Y轴加速度
  *               z    Z轴加速度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
float drv_sc7a20_get_roll(INT16 x, INT16 y, INT16 z)
{
    float roll = 0;
    UINT16 maxG = 0; 

    roll = atan2((float)(x), z) * 180 / PI;        //转换为度数

//    drv_3rd_uart_printf("roll:%3d", roll);

    return roll;
}

/**
  * Function    : drv_sc7a20_get_angle
  * Description : 获取Z轴倾角，即和水平面的角度
  * Input       : pAngleZ    角度存储区指针
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
OSA_STATUS drv_sc7a20_get_angle(float* pAngleZ)
{
    double Angle = 0.0;
    INT16 Xa, Ya, Za;

    //ADXL362 读取加速度数据
    if (drv_sc7a20_get_acc(&Xa, &Ya, &Za) == FALSE) 
        return FALSE;
//    drv_3rd_uart_printf("Xa:%d \tYa:%d \tZa:%d",Xa,Ya,Za);

    Angle = atan(sqrt(Xa * Xa + Ya * Ya)/Za) * 180 / PI;

    *pAngleZ = Angle;
//    drv_3rd_uart_printf("Angle: %.04f", *pAngleZ);

    return TRUE;
}

// End of file : drv_virtual_i2c.h 2022-1-13 11:57:53 by: zhaoning 

