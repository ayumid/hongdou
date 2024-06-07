//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
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

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
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

    //创建定时�?    OSATimerCreate(&sample_xl9535_int_detect_timer_ref);
    //创建中断处理任务
    OSATaskCreate(&sample_st7735s_task_ref, sample_st7735s_stack_ptr, SAMPLE_ST7735S_STACK_SIZE, 100, "st7735s_task", sample_st7735s_task, NULL);

}

/**
  * Function    : sample_st7735s_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
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
        //��Ļ��ʾ��ɫ
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //��ʾͼƬ��ͼƬ��36����24������Ҫ���Լ���ͼƬʹ������������ɵ�ʵ�ʲ����������ͼƬ����Ϊ93 * 26
        drv_st7735s_show_pic(20,25,123,34,gImage_1);
        sample_st7735s_sleep(1);
        //��Ļ��ʾ��ɫ
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //��ʾ���֣���С16
        drv_st7735s_show_chinese(50,5,"�漣����",RED,WHITE,12,0);
        //��ʾ�ַ�
        drv_st7735s_show_string(40,20,"LCD_W:",RED,WHITE,16,0);
        //��ʾ����
        drv_st7735s_show_int_num(88,20,LCD_W,3,RED,WHITE,16);
        //��ʾ�ַ�
        drv_st7735s_show_string(40,40,"LCD_H:",RED,WHITE,16,0);
        //��ʾ����
        drv_st7735s_show_int_num(88,40,LCD_H,3,RED,WHITE,16);
        //��ʾ������
        drv_st7735s_show_float_num(40,60,t,4,RED,WHITE,16);
        t += 0.11;
        
        sample_st7735s_sleep(1);
        //��Ļ��ʾ��ɫ
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //��ʾԲ
        drv_st7735s_draw_circle(45,35,20,BLUE);
        //��ʾ���֣���С24
        drv_st7735s_show_chinese(85,20,"�漣����",RED,WHITE,16,0);
        sample_st7735s_sleep(1);
        //��Ļ��ʾ��ɫ
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //��ʾ����
        drv_st7735s_draw_rectangle(120,20,140,60,GREEN);
        //��ʾ���֣���С32
        drv_st7735s_show_chinese(10,30,"�漣����",RED,WHITE,24,0);
        sample_st7735s_sleep(1);
        //��Ļ��ʾ��ɫ
        drv_st7735s_lcd_fill(0,0,LCD_W,LCD_H,WHITE);
        //����
        drv_st7735s_draw_line(10,20,130,40,LIGHTBLUE);
        drv_st7735s_draw_line(50,25,150,10,CYAN);
        //��ʾ���֣���С12
        drv_st7735s_show_chinese(30,40,"�漣����",RED,WHITE,32,0);
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

