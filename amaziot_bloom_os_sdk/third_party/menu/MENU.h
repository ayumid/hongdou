//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : MENU.h
// Auther      : zhaoning
// Version     :
// Date : 2024-7-3
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-7-3
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _MENU_H_
#define _MENU_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdint.h>
#include"sdk_api.h"

// Public defines / typedefs ----------------------------------------------------

enum _MENU_STR_VAR_S // 选项字符串附带的变量的数据类型枚举
{
    OLED_INT8 = 0,
    OLED_UINT8,
    OLED_INT16,
    OLED_UINT16,
    OLED_INT32,
    OLED_UINT32,
    OLED_STRING,
    OLED_CHAR,
    OLED_FLOAT,
};

enum _MENU_CMD_S
{
    BUFFER_DISPLAY, // 无参无返
    BUFFER_CLEAR,   // 无参无返
    SHOW_STRING,    // 可变参数列表对应顺序: x, y, string
    SHOW_CURSOR,    // 可变参数列表对应顺序: x, y, width, height;
    DRAW_FRAME,     // 可变参数列表对应顺序: x, y, width, height;

    GET_EVENT_ENTER, // 返回布尔
    GET_EVENT_BACK,  // 返回布尔
    GET_EVENT_WHEEL, // 返回有符号整型
};

typedef struct _MENU_OPTION_TYPE_S // 选项结构体
{
    char *String;                     // 选项字符串
    void (*func)(void);               // 函数指针
    void *StrVarPointer;              // 附带变量 的指针
    enum _MENU_STR_VAR_S StrVarType; // 附带变量 的类型
    uint8_t StrLen;                   // 字符串长度
} MENU_OPTION_TYPE_T;

typedef struct _MENU_HANDLE_TYPE_S // 选项结构体
{
    MENU_OPTION_TYPE_T *OptionList; // 选项列表指针

    int16_t Catch_i;              // 选中下标
    int16_t Cursor_i;             // 光标下标
    int16_t Show_i;               // 显示(遍历)起始下标
    int16_t Option_Max_i;         // 选项列表长度
    int16_t Show_i_Previous;      // 上一次的显示下标
    int16_t Wheel_Event;          // 菜单滚动事件
    uint8_t AnimationUpdateEvent; // 动画更新事件
    uint8_t isRun;                // 运行标志
    uint8_t isInitialized;        // 已初始化标志
    // uint8_t isPlayingAnimation; // 正在播放动画标志

} MENU_HANDLE_TYPE_T;

/* 宏函数 */

#define COORD_CHANGE_SIZE(sta, end) (((end) - (sta)) + 1)   // 坐标转换成尺寸 COORD_CHANGE_SIZE
#define SIZE_CHANGE_COORD(sta, size) (((sta) + (size)) - 1) // 尺寸转换成坐标 SIZE_CHANGE_COORD

// 逐步接近目标, actual当前, target目标, step_size步长
#define STEPWISE_TO_TARGET(actual, target, step_size)                                                                          \
    ((((target) - (actual)) > (0.0625))    ? ((actual) + (0.0625) + (((target) - (actual)) * (step_size)))                     \
     : (((target) - (actual)) < -(0.0625)) ? ((actual) - (0.0625) + (((target) - (actual)) * (step_size)))                     \
                                           : ((target)))

// Public functions prototypes --------------------------------------------------

#ifdef __cplusplus
}
#endif

int menu_command_callback(enum _MENU_CMD_S command, ...);
void menu_run_menu(MENU_HANDLE_TYPE_T *hMENU);
void menu_handle_init(MENU_HANDLE_TYPE_T *hMENU);
void menu_event_and_action(MENU_HANDLE_TYPE_T *hMENU);
void menu_updata_idx(MENU_HANDLE_TYPE_T *hMENU);
void menu_show_option_list(MENU_HANDLE_TYPE_T *hMENU);
uint8_t menu_show_option(int16_t X, int16_t Y, MENU_OPTION_TYPE_T *Option);
void menu_show_cursor(MENU_HANDLE_TYPE_T *hMENU);
void menu_show_border(MENU_HANDLE_TYPE_T *hMENU);
void menu_run_main_menu(void);
void menu_run_tools_menu(void);
void menu_run_games_menu(void);
void menu_run_info_menu(void);
void menu_display_menu(MENU_HANDLE_TYPE_T *hMENU);

#endif /* ifndef _MENU_H_.2024-7-3 12:03:08 by: zhaoning */

