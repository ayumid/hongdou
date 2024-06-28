//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : drv_wtn6x.c
// Auther      : zhaoning
// Version     :
// Date : 2022-3-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2022-3-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_wtn6x.h"
#include "sdk_api.h"
#include "drv_3rd.h"

// Private defines / typedefs ---------------------------------------------------

#define                 DRV_WTN604X_DELAY_3                 600
#define                 DRV_WTN604X_DELAY_1                 200

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_wtn60x0_busy_pin_init
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_wtn60x0_busy_pin_init(void)
{
    GPIOConfiguration config = {0};

    //初始化int引脚
    config.pinDir = GPIO_IN_PIN;
    config.pinPull = GPIO_PULLUP_ENABLE;
    GpioInitConfiguration(COM_BUSY, config);

}

/**
  * Function    : drv_virtual_i2c_scl_out_dir
  * Description : 配置scl引脚
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void drv_virtual_i2c_scl_out_dir(void)
//{
//    GPIOReturnCode status = GPIORC_OK;

//    //初始化 CS CLK MOSI
//    status = GpioSetDirection(COM_SCL, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        drv_3rd_uart_printf("status: 0x%lx", status);
//    }

//    //设置为高电平
//    GpioSetLevel(COM_SCL, DRV_WTN6X_GPIO_HIGH);

//}

/**
  * Function    : drv_virtual_i2c_sda_out_dir
  * Description : 配置sda引脚
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_virtual_i2c_sda_out_dir(void)
{
    GPIOReturnCode status = GPIORC_OK;

    //初始化 CS CLK MOSI
    status = GpioSetDirection(COM_SDA, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }

    //设置为高电平
    GpioSetLevel(COM_SDA, DRV_WTN6X_GPIO_HIGH);

}

/**
  * Function    : drv_wtn6x_1_line_write
  * Description : 实现一线串口通信函数
  * Input       : DDATA     为发送数据
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_wtn6x_1_line_write(UINT8 DDATA)
{
    UINT8 S_DATA = 0;
    BOOL B_DATA = 0;
    int i = 0;

    S_DATA = DDATA; 
    
    DRIVER_I2C_SDA_OUTPUT_L();
    OSATaskSleep(1);	//延时 5ms B_DATA = S_DATA&0X01;
    B_DATA = S_DATA & 0X01;

//    osCoreEnterCritical();
    for(i = 0; i < 8; i++)
    {
        B_DATA = S_DATA & 0X01;
//        DRIVER_I2C_SDA_OUTPUT_H();
        
        if(B_DATA == 1)
        {
            DRIVER_I2C_SDA_OUTPUT_H();
            drv_3rd_delay_us(DRV_WTN604X_DELAY_3);	//延时 600us P_DATA = 0;
            DRIVER_I2C_SDA_OUTPUT_L();
            drv_3rd_delay_us(DRV_WTN604X_DELAY_1);	//延时 200us
        }
        else
        {
            DRIVER_I2C_SDA_OUTPUT_H();
            drv_3rd_delay_us(DRV_WTN604X_DELAY_1);	//延时 200us P_DATA = 0;
            DRIVER_I2C_SDA_OUTPUT_L();
            drv_3rd_delay_us(DRV_WTN604X_DELAY_3);	//延时 600us
        }
        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0X01;
    }
    
    DRIVER_I2C_SDA_OUTPUT_H();
    OSATaskSleep(1);
//    osCoreExitCritical();
    
}

/**
  * Function    : drv_wtn6x_2_line_write
  * Description : 实现二线串口通信函数
  * Input       : DDATA     为发送数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_wtn6x_2_line_write(UINT8 DDATA)
{
    UINT8 S_DATA = 0;
    BOOL B_DATA;
    int i = 0;
    
    DRIVER_I2C_SCL_OUTPUT_H();//时钟线
    DRIVER_I2C_SDA_OUTPUT_H();//数据线

    S_DATA = DDATA;
    DRIVER_I2C_SCL_OUTPUT_L();
    OSATaskSleep(1);//延时 5ms
    B_DATA = S_DATA & 0X01;

    for(i = 0; i < 8; i++)
    {
        DRIVER_I2C_SCL_OUTPUT_L();// 拉低
        
        if(TRUE == B_DATA)
            DRIVER_I2C_SDA_OUTPUT_H();
        else
            DRIVER_I2C_SDA_OUTPUT_L();
            
        drv_3rd_delay_us(300);//延时 300us
        DRIVER_I2C_SCL_OUTPUT_H();//拉高
        drv_3rd_delay_us(300);//延时 300us

        S_DATA = S_DATA >> 1;
        B_DATA = S_DATA & 0X01;
    }
    DRIVER_I2C_SDA_OUTPUT_H();
    DRIVER_I2C_SCL_OUTPUT_H();
    OSATaskSleep(1);
}

/**
  * Function    : drv_wtn60x0_write
  * Description : 写数据给芯片，控制芯片播放语音
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 drv_wtn60x0_write(UINT8 data)
{
    int i = 0;

//    drv_virtual_i2c_sda_out_dir();
//    drv_wtn60x0_busy_pin_init();
    
//    osDelay(20);

//    while(!DRIVER_I2C_BUSY_INPUT_R() && i < 1000 )
//    {
//        osDelay(10);
//        i++;

//        if(i >= 999)
//        {
//            return 101;
//        }
//    }

//    drv_wtn6x_1_line_write(data);
    OSATaskSleep(4);

    while(!DRIVER_I2C_BUSY_INPUT_R() && i < 10 )
    {
        OSATaskSleep(100);
        i++;

        if(i >= 9)
        {
            return 101;
        }
    }
    
    drv_wtn6x_1_line_write(data);
    
    return 0;
}

// End of file : drv_wtn6x.h 2022-3-7 14:35:57 by: zhaoning 

