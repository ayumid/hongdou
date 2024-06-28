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

// Private defines / typedefs ---------------------------------------------------

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
  * Function    : sample_ssd1315_task
  * Description : 中断处理任务，通过事件驱动，当定时器超时后，会产生一个事件，这个任务开始执行，通过寄存器判断io扩展芯片哪个io产生变化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void sample_ssd1315_task(void *param)
{
    OSA_STATUS status = OS_SUCCESS;
    float t=0;

    drv_ssd1315_init();//初始化OLED
    drv_ssd1315_color_turn(0);//0正常显示，1 反色显示
    drv_ssd1315_display_turn(0);//0正常显示 1 屏幕翻转显示
    
    while(1)
    {
        //显示图片，注意参数和实际点阵大小一致
        drv_ssd1315_show_picture(0,12,132,37,BMP1,1);
        drv_ssd1315_refresh();
        sample_ssd1315_sleep(1);
        drv_ssd1315_clear();
        //显示汉字
        drv_ssd1315_show_chinese(30,0,0,16,1);//
        drv_ssd1315_show_chinese(48,0,1,16,1);//
        drv_ssd1315_show_chinese(64,0,2,16,1);//
        drv_ssd1315_show_chinese(82,0,3,16,1);//
        //显示字符，数字
        drv_ssd1315_show_string(36,16,"AMAZIOT",16,1);
        drv_ssd1315_show_string(26,32,"2024/06/01",16,1);
        drv_ssd1315_show_string(0,48,"ASCII:",16,1);  
        drv_ssd1315_show_string(63,48,"CODE:",16,1);
        drv_ssd1315_show_char(48,48,t,16,1);//显示ASCII字符    
        t++;
        if(t>'~')t=' ';
        drv_ssd1315_show_num(103,48,t,3,16,1);
        drv_ssd1315_refresh();
        sample_ssd1315_sleep(1);
        drv_ssd1315_clear();
        //显示不同大小的汉字
        drv_ssd1315_show_chinese(0,0,0,16,1);  //16*16 
        drv_ssd1315_show_chinese(16,0,0,24,1); //24*24 
        drv_ssd1315_show_chinese(24,25,0,32,1);//32*32 
        drv_ssd1315_show_chinese(64,0,0,64,1); //64*64 
        drv_ssd1315_refresh();
        sample_ssd1315_sleep(1);
        drv_ssd1315_clear();
        //显示不同大小的字符
        drv_ssd1315_show_string(0,0,"AMAZIOT",8,1);//6*8 "AMAZIOT"
        drv_ssd1315_show_string(0,8,"AMAZIOT",12,1);//6*12 "AMAZIOT"
        drv_ssd1315_show_string(0,20,"AMAZIOT",16,1);//8*16 "AMAZIOT"
        drv_ssd1315_show_string(0,36,"AMAZIOT",24,1);//12*24 "AMAZIOT"
        drv_ssd1315_refresh();
        sample_ssd1315_sleep(1);
        //滚动显示汉字
        drv_ssd1315_scroll_display(14,4,1);
    }

}

// End of file : am_oled.c 2024-6-26 10:18:43 by: zhaoning 

