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

#include "menu.h"
#include "menu_options.h"

#include "drv_ssd1315_oled.h"
#include "drv_ssd1315_bmp.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_ssd1315_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_ssd1315_catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_ssd1315_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_ISR_PIN_NUM        126

#define SAMPLE_SSD1315_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

OSATaskRef     sample_ssd1315_task_ref = NULL;
char         sample_ssd1315_stack_ptr[SAMPLE_SSD1315_STACK_SIZE] = {0};
static UINT32  event_ticks = 0;
static UINT8   int_status = 0;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

void sample_ssd1315_task(void *param);

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
    OSATaskCreate(&sample_ssd1315_task_ref, sample_ssd1315_stack_ptr, SAMPLE_SSD1315_STACK_SIZE, 100, "ssd1315_task", sample_ssd1315_task, NULL);

}

/**
  * Function    : menu_run_main_menu
  * Description : 显示主界面，用户任务中调用这个函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void menu_run_main_menu(void)
{
    static MENU_OPTION_TYPE_T MENU_OptionList[] = {
                                                   {OLED_TOOLS, menu_run_tools_menu},      // 工具
                                                   {OLED_SETTING, NULL},                 // 设置
                                                   {OLED_ABOUT, menu_run_info_menu}, // 关于
                                                   {".."}};

    static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList};
    
    menu_run_menu(&MENU);
}

/**
  * Function    : menu_run_tools_menu
  * Description : 根据按键选择运行工具界面
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void menu_run_tools_menu(void)
{
    static MENU_OPTION_TYPE_T MENU_OptionList[] = {
                                                   {"TOOLS1", NULL},
                                                   {"TOOLS2", NULL},
                                                   {"TOOLS3", NULL},
                                                   {"TOOLS4", NULL},
                                                   {"TOOLS5", NULL},
                                                   {".."}};

    static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList};

    menu_run_menu(&MENU);
}

/**
  * Function    : menu_run_info_menu
  * Description : 根据按键选择运行信息界面
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void menu_run_info_menu(void)
{
    menu_command_callback(BUFFER_CLEAR);
    menu_command_callback(SHOW_STRING, 5, 0, OLED_VER_CO);
    menu_command_callback(BUFFER_DISPLAY);

    while (1)
    {

        if (menu_command_callback(GET_EVENT_ENTER))
            return;

        if (menu_command_callback(GET_EVENT_BACK))
            return;
    }
}

/**
  * Function    : sample_ssd1315_task
  * Description : 菜单主任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_ssd1315_task(void *param)
{
    drv_ssd1315_init();//初始化OLED

    //初始化按键
    input_key_init();
    while(1)
    {
        menu_run_main_menu();

    }

}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

