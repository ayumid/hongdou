//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_oled.c
// Auther      : zhaoning
// Version     :
// Date : 2024-6-26
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-26
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_oled.h"
#include "menu.h"
#include "menu_options.h"
#include "am_sw_input.h"
#include "am_485.h"

#include "drv_ssd1315_oled.h"
#include "drv_ssd1315_bmp.h"

// Private defines / typedefs ---------------------------------------------------

#define sample_ssd1315_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_GPIO_ISR_PIN_NUM        126

#define SAMPLE_SSD1315_STACK_SIZE            2048

// Private variables ------------------------------------------------------------

OSATaskRef     sample_ssd1315_task_ref = NULL;
char         sample_ssd1315_stack_ptr[SAMPLE_SSD1315_STACK_SIZE] = {0};
static UINT32  event_ticks = 0;
static UINT8   int_status = 0;

// Public variables -------------------------------------------------------------

extern SW_DTU_ALIVE sw_dtu_alive[SW_DTU_NUM_MAX];

// Private functions prototypes -------------------------------------------------

void air_sw_task(void *param);
void menu_run_main_menu(void);
void menu_run_status_menu(void);
void menu_run_info_menu(void);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

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
//                                                   {OLED_TOOLS, menu_run_tools_menu},      // 工具
                                                   {OLED_SETTING, NULL},                 // 设置
                                                   {OLED_STATUS, menu_run_status_menu},                 // 485状态
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

static MENU_OPTION_TYPE_T MENU_OptionList_on[] = {
                                                   {"", NULL},
                                                   {"ON", NULL},
                                                   {".."}};
static MENU_OPTION_TYPE_T MENU_OptionList_off[] = {
                                                   {"", NULL},
                                                   {"OFF", NULL},
                                                   {".."}};

void menu_run_status_menu1(void)
{
    if(sw_dtu_alive[0].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }

}
void menu_run_status_menu2(void)
{
    if(sw_dtu_alive[1].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu3(void)
{
    if(sw_dtu_alive[2].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu4(void)
{
    if(sw_dtu_alive[3].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu5(void)
{
    if(sw_dtu_alive[4].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu6(void)
{
    if(sw_dtu_alive[5].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu7(void)
{
    if(sw_dtu_alive[6].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu8(void)
{
    if(sw_dtu_alive[7].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu9(void)
{
    if(sw_dtu_alive[8].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu10(void)
{
    if(sw_dtu_alive[9].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu11(void)
{
    if(sw_dtu_alive[10].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }

}
void menu_run_status_menu12(void)
{
    if(sw_dtu_alive[11].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu13(void)
{
    if(sw_dtu_alive[12].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu14(void)
{
    if(sw_dtu_alive[13].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu15(void)
{
    if(sw_dtu_alive[14].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu16(void)
{
    if(sw_dtu_alive[15].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu17(void)
{
    if(sw_dtu_alive[16].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu18(void)
{
    if(sw_dtu_alive[17].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu19(void)
{
    if(sw_dtu_alive[18].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu20(void)
{
    if(sw_dtu_alive[19].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu21(void)
{
    if(sw_dtu_alive[20].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }

}
void menu_run_status_menu22(void)
{
    if(sw_dtu_alive[21].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu23(void)
{
    if(sw_dtu_alive[22].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu24(void)
{
    if(sw_dtu_alive[23].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu25(void)
{
    if(sw_dtu_alive[24].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu26(void)
{
    if(sw_dtu_alive[25].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu27(void)
{
    if(sw_dtu_alive[26].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu28(void)
{
    if(sw_dtu_alive[27].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu29(void)
{
    if(sw_dtu_alive[28].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu30(void)
{
    if(sw_dtu_alive[29].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}
void menu_run_status_menu31(void)
{
    if(sw_dtu_alive[30].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }

}
void menu_run_status_menu32(void)
{
    if(sw_dtu_alive[31].alive == 1)
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_on};
        menu_run_menu(&MENU);
    }
    else
    {
        static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList_off};
        menu_run_menu(&MENU);
    }
}

/**
  * Function    : menu_run_status_menu
  * Description : 根据按键选择运行工具界面
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void menu_run_status_menu(void)
{
    static MENU_OPTION_TYPE_T MENU_OptionList[] = {
                                                   {"Device01", menu_run_status_menu1},
                                                   {"Device02", menu_run_status_menu2},
                                                   {"Device03", menu_run_status_menu3},
                                                   {"Device04", menu_run_status_menu4},
                                                   {"Device05", menu_run_status_menu5},
                                                   {"Device06", menu_run_status_menu6},
                                                   {"Device07", menu_run_status_menu7},
                                                   {"Device08", menu_run_status_menu8},
                                                   {"Device09", menu_run_status_menu9},
                                                   {"Device10", menu_run_status_menu10},
                                                   {"Device11", menu_run_status_menu11},
                                                   {"Device12", menu_run_status_menu12},
                                                   {"Device13", menu_run_status_menu13},
                                                   {"Device14", menu_run_status_menu14},
                                                   {"Device15", menu_run_status_menu15},
                                                   {"Device16", menu_run_status_menu16},
                                                   {"Device17", menu_run_status_menu17},
                                                   {"Device18", menu_run_status_menu18},
                                                   {"Device19", menu_run_status_menu19},
                                                   {"Device20", menu_run_status_menu20},
                                                   {"Device21", menu_run_status_menu21},
                                                   {"Device22", menu_run_status_menu22},
                                                   {"Device23", menu_run_status_menu23},
                                                   {"Device24", menu_run_status_menu24},
                                                   {"Device25", menu_run_status_menu25},
                                                   {"Device26", menu_run_status_menu26},
                                                   {"Device27", menu_run_status_menu27},
                                                   {"Device28", menu_run_status_menu28},
                                                   {"Device29", menu_run_status_menu29},
                                                   {"Device30", menu_run_status_menu30},
                                                   {"Device31", menu_run_status_menu31},
                                                   {"Device32", menu_run_status_menu32},
                                                   {".."}};

    static MENU_HANDLE_TYPE_T MENU = {.OptionList = MENU_OptionList};

    menu_run_menu(&MENU);
//    OSATaskSleep(199);
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

            OSATaskSleep(1);
    }
}

void air_sw_task_init(void)
{
    int ret = 0;

    //创建定时器,定时刷新设备状态
//    OSATimerCreate(&sample_xl9535_int_detect_timer_ref);
    //创建中断处理任务
    OSATaskCreate(&sample_ssd1315_task_ref, sample_ssd1315_stack_ptr, SAMPLE_SSD1315_STACK_SIZE, 100, "air_sw_task", air_sw_task, NULL);

}

/**
  * Function    : sample_ssd1315_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void air_sw_task(void *param)
{
    OSA_STATUS status = OS_SUCCESS;
    float t = 0;

    drv_ssd1315_init();//初始化OLED

    //初始化按键
    input_key_init();
    
    while(1)
    {
        menu_run_main_menu();

//        OSATaskSleep(100);
    }

}

// End of file : am_oled.c 2024-6-26 10:18:43 by: zhaoning 

