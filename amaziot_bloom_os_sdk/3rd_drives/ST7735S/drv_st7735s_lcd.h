//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_st7735s_lcd.h
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
#ifndef _DRV_ST7735S_LCD_H_
#define _DRV_ST7735S_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

//画笔颜色
#define WHITE              0xFFFF
#define BLACK              0x0000      
#define BLUE               0x001F  
#define BRED               0XF81F
#define GRED               0XFFE0
#define GBLUE              0X07FF
#define RED                0xF800
#define MAGENTA            0xF81F
#define GREEN              0x07E0
#define CYAN               0x7FFF
#define YELLOW             0xFFE0
#define BROWN              0XBC40 //棕色
#define BRRED              0XFC07 //棕红色
#define GRAY               0X8430 //灰色
#define DARKBLUE           0X01CF //深蓝色
#define LIGHTBLUE          0X7D7C //浅蓝色  
#define GRAYBLUE           0X5458 //灰蓝色
#define LIGHTGREEN         0X841F //浅绿色
#define LGRAY              0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE          0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE             0X2B12 //浅棕蓝色(选择条目的反色)

// Public functions prototypes --------------------------------------------------
    
void drv_st7735s_lcd_fill(UINT16 xsta,UINT16 ysta,UINT16 xend,UINT16 yend,UINT16 color);//指定区域填充颜色
void drv_st7735s_draw_point(UINT16 x,UINT16 y,UINT16 color);//在指定位置画一个点
void drv_st7735s_draw_line(UINT16 x1,UINT16 y1,UINT16 x2,UINT16 y2,UINT16 color);//在指定位置画一条线
void drv_st7735s_draw_rectangle(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2,UINT16 color);//在指定位置画一个矩形
void drv_st7735s_draw_circle(UINT16 x0,UINT16 y0,UINT8 r,UINT16 color);//在指定位置画一个圆

void drv_st7735s_show_chinese(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示汉字串
void drv_st7735s_show_chinese_12x12(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示单个12x12汉字
void drv_st7735s_show_chinese_16x16(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示单个16x16汉字
void drv_st7735s_show_chinese_24x24(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示单个24x24汉字
void drv_st7735s_show_chinese_32x32(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示单个32x32汉字

void drv_st7735s_show_char(UINT16 x,UINT16 y,UINT8 num,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示一个字符
void drv_st7735s_show_string(UINT16 x,UINT16 y,const UINT8 *p,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode);//显示字符串
UINT32 drv_st7735s_pow(UINT8 m,UINT8 n);//求幂
void drv_st7735s_show_int_num(UINT16 x,UINT16 y,UINT16 num,UINT8 len,UINT16 fc,UINT16 bc,UINT8 sizey);//显示整数变量
void drv_st7735s_show_float_num(UINT16 x,UINT16 y,float num,UINT8 len,UINT16 fc,UINT16 bc,UINT8 sizey);//显示两位小数变量

void drv_st7735s_show_pic(UINT16 x,UINT16 y,UINT16 length,UINT16 width,const UINT8 pic[]);//显示图片

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_ST7735S_LCD_H_.2024-6-5 11:55:09 by: zhaoning */
