//------------------------------------------------------------------------------
// Copyright , 2017-2023 濂囪抗鐗╄仈锛堝寳浜級绉戞妧鏈夐檺鍏�?
// Filename    : drv_st7735s_lcd_init.h
// Auther      : zhaoning
// Version     :
// Date : 2024-6-5
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-5
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_ST7735S_LCD_INIT_H_
#define _DRV_ST7735S_LCD_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Public defines / typedefs ----------------------------------------------------

#define DRV_ST7735S_USE_HORIZONTAL 2  //设置横屏或者竖屏显�?0�?为竖�?2�?为横�?

#if DRV_ST7735S_USE_HORIZONTAL == 0 || DRV_ST7735S_USE_HORIZONTAL == 1
#define LCD_W 80
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 80
#endif

#define DRV_ST7735S_I2C_CS                  17//PIN 31 PCM_SYNC
#define DRV_ST7735S_I2C_SDA                 50//PIN 65 SDA
#define DRV_ST7735S_I2C_SCL                 49//PIN 48 SCL
#define DRV_ST7735S_I2C_DC                  19//PIN 32 PCM_IN
#define DRV_ST7735S_I2C_RES                 18//PIN 33 PCM_OUT
#define DRV_ST7735S_I2C_BLK                 16//PIN 30 PCM_CLK

#define DRV_ST7735S_GPIO_HIGH               1
#define DRV_ST7735S_GPIO_LOW                0

//-----------------LCD端口定义---------------- 

#define DRV_ST7735S_SCLK_CLR() GpioSetLevel(DRV_ST7735S_I2C_SCL, DRV_ST7735S_GPIO_LOW)//SCL=SCLK
#define DRV_ST7735S_SCLK_SET() GpioSetLevel(DRV_ST7735S_I2C_SCL, DRV_ST7735S_GPIO_HIGH)

#define DRV_ST7735S_MOSI_CLR() GpioSetLevel(DRV_ST7735S_I2C_SDA, DRV_ST7735S_GPIO_LOW)//SDA=MOSI
#define DRV_ST7735S_MOSI_SET() GpioSetLevel(DRV_ST7735S_I2C_SDA, DRV_ST7735S_GPIO_HIGH)

#define DRV_ST7735S_RES_CLR()  GpioSetLevel(DRV_ST7735S_I2C_RES, DRV_ST7735S_GPIO_LOW)//RES
#define DRV_ST7735S_RES_SET()  GpioSetLevel(DRV_ST7735S_I2C_RES, DRV_ST7735S_GPIO_HIGH)

#define DRV_ST7735S_DC_CLR()   GpioSetLevel(DRV_ST7735S_I2C_DC, DRV_ST7735S_GPIO_LOW)//DC
#define DRV_ST7735S_DC_SET()   GpioSetLevel(DRV_ST7735S_I2C_DC, DRV_ST7735S_GPIO_HIGH)

#define DRV_ST7735S_CS_CLR()   GpioSetLevel(DRV_ST7735S_I2C_CS, DRV_ST7735S_GPIO_LOW)//CS
#define DRV_ST7735S_CS_SET()   GpioSetLevel(DRV_ST7735S_I2C_CS, DRV_ST7735S_GPIO_HIGH)

#define DRV_ST7735S_BLK_CLR()  GpioSetLevel(DRV_ST7735S_I2C_BLK, DRV_ST7735S_GPIO_LOW)//BLK
#define DRV_ST7735S_BLK_SET()  GpioSetLevel(DRV_ST7735S_I2C_BLK, DRV_ST7735S_GPIO_HIGH)

// Public functions prototypes --------------------------------------------------

void drv_st7735s_delay_ms(unsigned int ms);

void drv_st7735s_gpio_init(void);//初始化GPIO
void LCD_Writ_Bus(UINT8 dat);//模拟SPI时序
void LCD_WR_DATA8(UINT8 dat);//写入一个字�?
void LCD_WR_DATA(UINT16 dat);//写入两个字节
void LCD_WR_REG(UINT8 dat);//写入一个指�?
void LCD_Address_Set(UINT16 x1,UINT16 y1,UINT16 x2,UINT16 y2);//设置坐标函数
void drv_st7735s_init(void);//LCD初始�?

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_ST7735S_LCD_INIT_H_.2024-6-5 11:39:19 by: zhaoning */
