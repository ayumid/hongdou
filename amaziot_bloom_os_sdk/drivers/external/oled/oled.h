//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : oled.h
// Auther      : win
// Version     :
// Date : 2021-12-29
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-29
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _OLED_H_
#define _OLED_H_

// Includes ---------------------------------------------------------------------
#include "timer.h"
#include "osa.h"
#include "cgpio.h"
#include "cgpio_HW.h"
// Public defines / typedef -----------------------------------------------------
#define  u8 unsigned char 
#define  u32 unsigned int 
//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define OLED_MODE 0

/*
*OLED对应的GPIO定义
*/
#define OLED_SCL   16//OLED 的 D0 脚 在 SPI 和 IIC 通信中为时钟管脚//27
#define OLED_SDIN  17//OLED 的 D1 脚 在 SPI 和 IIC 通信中为数据管脚//26
#define OLED_DC    18//OLED 的 DC脚 数据和命令控制管脚//25
#define OLED_CS    19//OLED 的 CS脚 也就是片选管脚//24

#define OLED_RST   2//OLED的RES脚 用来复位(低电平复位)//2

#define OLED_SCLK_Clr() {GpioSetDirection(OLED_SCL, GPIO_OUT_PIN);GpioSetLevel(OLED_SCL,0);}
#define OLED_SCLK_Set() {GpioSetDirection(OLED_SCL, GPIO_OUT_PIN);GpioSetLevel(OLED_SCL,1);}

#define OLED_SDIN_Clr() {GpioSetDirection(OLED_SDIN, GPIO_OUT_PIN);GpioSetLevel(OLED_SDIN,0);}
#define OLED_SDIN_Set() {GpioSetDirection(OLED_SDIN, GPIO_OUT_PIN);GpioSetLevel(OLED_SDIN,1);}

#define OLED_DC_Clr() {GpioSetDirection(OLED_DC, GPIO_OUT_PIN);GpioSetLevel(OLED_DC,0);}
#define OLED_DC_Set() {GpioSetDirection(OLED_DC, GPIO_OUT_PIN);GpioSetLevel(OLED_DC,1);}

#define OLED_CS_Clr()  {GpioSetDirection(OLED_CS, GPIO_OUT_PIN);GpioSetLevel(OLED_CS,0);}
#define OLED_CS_Set()  {GpioSetDirection(OLED_CS, GPIO_OUT_PIN);GpioSetLevel(OLED_CS,1);}

#define OLED_RST_Clr() {GpioSetDirection(OLED_RST, GPIO_OUT_PIN);GpioSetLevel(OLED_RST,0);}
#define OLED_RST_Set() {GpioSetDirection(OLED_RST, GPIO_OUT_PIN);GpioSetLevel(OLED_RST,1);}

// Public functions prototypes --------------------------------------------------
/**
  * Function    : OLED_Gpio_Init
  * Description : OLED对应的GPIO初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Gpio_Init(void);
/**
  * Function    : OLED_WR_Byte
  * Description : 向SSD1106写入一个字节
  * Input       : dat:要写入的数据/命令
  *               cmd:数据/命令标志 0,表示命令;1,表示数据
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_WR_Byte(u8 dat,u8 cmd);
/**
  * Function    : OLED_Set_Pos
  * Description : 指定显示位置
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Set_Pos(unsigned char x, unsigned char y);
/**
  * Function    : OLED_Display_On
  * Description : 开启OLED显示 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Display_On(void);
/**
  * Function    : OLED_Display_Off
  * Description : 关闭OLED显示  
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Display_Off(void);
/**
  * Function    : OLED_Clear
  * Description : 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Clear(void);
/**
  * Function    : OLED_ShowChar
  * Description : 在指定位置显示一个字符,包括部分字符
  * Input       : x:0~127
  *               y:0~63
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : //mode:0,反白显示;1,正常显示				 
  *  			  //size:选择字体 16/12 
  **/
void OLED_ShowChar(u8 x,u8 y,u8 chr);
/**
  * Function    : OLED_ShowNum
  * Description : 显示2个数字
  * Input       : x,y :起点坐标
  *				  num:数值(0~4294967295)
  *               len :数字的位数
  *               size:字体大小
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : //mode:模式	0,填充模式;1,叠加模式
  **/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
/**
  * Function    : OLED_ShowString
  * Description : 显示一个字符串
  * Input       : x,y :起点坐标
  *               chr :字符串首地址
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_ShowString(u8 x,u8 y,u8 *chr);
/**
  * Function    : OLED_ShowCHinese
  * Description : 显示汉字
  * Input       : x,y :起点坐标
  *               no  :文字编号
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
/**
  * Function    : OLED_DrawBMP
  * Description : 显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
/**
  * Function    : OLED_Init
  * Description : 初始化SSD1306	
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Init(void);

#endif /* ifndef _OLED_H_.2021-12-29 10:37:38 by: win */
	 



