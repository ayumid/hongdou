//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_ssd1315_oled.h
// Auther      : zhaoning
// Version     :
// Date : 2024-6-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_SSD1315_OLED_H_
#define _DRV_SSD1315_OLED_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"
#include "stdlib.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Public defines / typedefs ----------------------------------------------------

//-----------------OLED端口定义---------------- 
#define DRV_SSD1315_SPI_CS                  17//PIN 31 PCM_SYNC
#define DRV_SSD1315_SPI_SDA                 50//PIN 65 SDA
#define DRV_SSD1315_SPI_SCL                 49//PIN 48 SCL
#define DRV_SSD1315_SPI_DC                  19//PIN 32 PCM_IN
#define DRV_SSD1315_SPI_RES                 18//PIN 33 PCM_OUT

#define DRV_ST7735S_GPIO_HIGH               1
#define DRV_ST7735S_GPIO_LOW                0

#define DRV_SSD1315_SCL_CLR() GpioSetLevel(DRV_SSD1315_SPI_SCL, DRV_ST7735S_GPIO_LOW)//SCL
#define DRV_SSD1315_SCL_SET() GpioSetLevel(DRV_SSD1315_SPI_SCL, DRV_ST7735S_GPIO_HIGH)

#define DRV_SSD1315_SDA_CLR() GpioSetLevel(DRV_SSD1315_SPI_SDA, DRV_ST7735S_GPIO_LOW)//SDA
#define DRV_SSD1315_SDA_SET() GpioSetLevel(DRV_SSD1315_SPI_SDA, DRV_ST7735S_GPIO_HIGH)

#define DRV_SSD1315_RES_CLR() GpioSetLevel(DRV_SSD1315_SPI_RES, DRV_ST7735S_GPIO_LOW)//RES
#define DRV_SSD1315_RES_SET() GpioSetLevel(DRV_SSD1315_SPI_RES, DRV_ST7735S_GPIO_HIGH)

#define DRV_SSD1315_DC_CLR()  GpioSetLevel(DRV_SSD1315_SPI_DC, DRV_ST7735S_GPIO_LOW)//DC
#define DRV_SSD1315_DC_SET()  GpioSetLevel(DRV_SSD1315_SPI_DC, DRV_ST7735S_GPIO_HIGH)

#define DRV_SSD1315_CS_CLR()  GpioSetLevel(DRV_SSD1315_SPI_CS, DRV_ST7735S_GPIO_LOW)//CS
#define DRV_SSD1315_CS_SET()  GpioSetLevel(DRV_SSD1315_SPI_CS, DRV_ST7735S_GPIO_HIGH)

#define DRV_SSD1315_OLED_CMD  0//写命令
#define DRV_SSD1315_OLED_DATA 1//写数据

// Public functions prototypes --------------------------------------------------

void drv_ssd1315_clear_point(UINT8 x,UINT8 y);
void drv_ssd1315_color_turn(UINT8 i);
void drv_ssd1315_display_turn(UINT8 i);
void drv_ssd1315_wr_byte(UINT8 dat,UINT8 mode);
void drv_ssd1315_display_on(void);
void drv_ssd1315_display_off(void);
void drv_ssd1315_refresh(void);
void drv_ssd1315_clear(void);
void drv_ssd1315_draw_point(UINT8 x,UINT8 y,UINT8 t);
void drv_ssd1315_draw_line(UINT8 x1,UINT8 y1,UINT8 x2,UINT8 y2,UINT8 mode);
void drv_ssd1315_draw_circle(UINT8 x,UINT8 y,UINT8 r);
void drv_ssd1315_show_char(UINT8 x,UINT8 y,UINT8 chr,UINT8 size1,UINT8 mode);
void drv_ssd1315_show_char_6x8(UINT8 x,UINT8 y,UINT8 chr,UINT8 mode);
void drv_ssd1315_show_string(UINT8 x,UINT8 y,UINT8 *chr,UINT8 size1,UINT8 mode);
void drv_ssd1315_show_num(UINT8 x,UINT8 y,UINT32 num,UINT8 len,UINT8 size1,UINT8 mode);
void drv_ssd1315_show_chinese(UINT8 x,UINT8 y,UINT8 num,UINT8 size1,UINT8 mode);
void drv_ssd1315_scroll_display(UINT8 num,UINT8 space,UINT8 mode);
void drv_ssd1315_show_picture(UINT8 x,UINT8 y,UINT8 sizex,UINT8 sizey,UINT8 BMP[],UINT8 mode);
void drv_ssd1315_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_SSD1315_OLED_H_.2024-6-7 9:15:58 by: zhaoning */
