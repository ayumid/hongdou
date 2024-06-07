//------------------------------------------------------------------------------
// Copyright , 2017-2021 濂囪抗鐗╄仈锛堝寳浜級绉戞妧鏈夐檺鍏徃
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

#include "drv_st7735s_lcd.h"
#include "drv_st7735s_lcd_init.h"
//#include "drv_st7735s_lcdfont.h"
#include "drv_st7735s_pic.h"

// Private macros / types / typedef ---------------------------------------------

/*Log澶鐨勬椂鍊欎笉寤鸿浣跨敤UART log锛屼細鍑虹幇寰堝寮傚父锛屽缓璁娇鐢–ATStudio 鏌ョ湅log*/
// debug uart log
#define sample_st7735s_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_st7735s_catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_st7735s_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_ISR_PIN_NUM        126

#define SAMPLE_ST7735S_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

OSATaskRef     sample_st7735s_task_ref = NULL;
char         sample_st7735s_stack_ptr[SAMPLE_ST7735S_STACK_SIZE] = {0};
static UINT32  event_ticks = 0;
static UINT8   int_status = 0;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

void sample_st7735s_task(void *param);

// Public functions prototypes --------------------------------------------------

// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    int ret = 0;
    GPIOConfiguration config = {0};

    //鍒涘缓瀹氭椂鍣?    OSATimerCreate(&sample_xl9535_int_detect_timer_ref);
    //鍒涘缓涓柇澶勭悊浠诲姟
    OSATaskCreate(&sample_st7735s_task_ref, sample_st7735s_stack_ptr, SAMPLE_ST7735S_STACK_SIZE, 100, "st7735s_task", sample_st7735s_task, NULL);

}

/**
  * Function    : sample_st7735s_task
  * Description : 涓柇澶勭悊浠诲姟锛岄�氳繃浜嬩欢椹卞姩锛屽綋瀹氭椂鍣ㄨ秴鏃跺悗锛屼細浜х敓涓�涓簨浠讹紝杩欎釜浠诲姟寮�濮嬫墽琛岋紝閫氳繃瀵勫瓨鍣ㄥ垽鏂璱o鎵╁睍鑺墖鍝釜io浜х敓鍙樺寲
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_st7735s_task(void *param)
{
    GPIO_ReturnCode ret = 0;
    OSA_STATUS status = OS_SUCCESS;
    float t=0;

    drv_st7735s_init();//

    while(1)
    {
        //屏幕显示白色
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //显示图片，图片宽36，高24，具体要看自己的图片使用桌面软件生成的实际参数，这里的图片参数为93 * 26
        drv_st7735s_show_pic(20,25,123,34,gImage_1);
        sample_st7735s_sleep(1);
        //屏幕显示白色
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //显示汉字，大小16
        drv_st7735s_show_chinese(50,5,"奇迹物联",RED,WHITE,12,0);
        //显示字符
        drv_st7735s_show_string(40,20,"LCD_W:",RED,WHITE,16,0);
        //显示数字
        drv_st7735s_show_int_num(88,20,LCD_W,3,RED,WHITE,16);
        //显示字符
        drv_st7735s_show_string(40,40,"LCD_H:",RED,WHITE,16,0);
        //显示数字
        drv_st7735s_show_int_num(88,40,LCD_H,3,RED,WHITE,16);
        //显示浮点数
        drv_st7735s_show_float_num(40,60,t,4,RED,WHITE,16);
        t += 0.11;
        
        sample_st7735s_sleep(1);
        //屏幕显示白色
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //显示圆
        drv_st7735s_draw_circle(45,35,20,BLUE);
        //显示汉字，大小24
        drv_st7735s_show_chinese(85,20,"奇迹物联",RED,WHITE,16,0);
        sample_st7735s_sleep(1);
        //屏幕显示白色
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //显示矩形
        drv_st7735s_draw_rectangle(120,20,140,60,GREEN);
        //显示汉字，大小32
        drv_st7735s_show_chinese(10,30,"奇迹物联",RED,WHITE,24,0);
        sample_st7735s_sleep(1);
        //屏幕显示白色
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //画线
        drv_st7735s_draw_line(10,20,130,40,LIGHTBLUE);
        drv_st7735s_draw_line(50,25,150,10,CYAN);
        //显示汉字，大小12
        drv_st7735s_show_chinese(30,40,"奇迹物联",RED,WHITE,32,0);
        sample_st7735s_sleep(1);
//        LCD_SCLK_Clr();
//        LCD_MOSI_Clr();
//        LCD_RES_Clr();
//        LCD_DC_Clr();
//        LCD_CS_Clr();
//        LCD_BLK_Clr();
//        sample_st7735s_sleep(1);
//        LCD_SCLK_Set();
//        LCD_MOSI_Set();
//        LCD_RES_Set();
//        LCD_DC_Set();
//        LCD_CS_Set();
//        LCD_BLK_Set();
    }
}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

