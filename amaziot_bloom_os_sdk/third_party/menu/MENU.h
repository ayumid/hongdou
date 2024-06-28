#ifndef __MENU_H
#define __MENU_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#include <stdint.h>

enum _MENU_StrVarType // 选项字符串附带的变量的数据类型枚举
{
    INT8 = 0,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    STRING,
    CHAR,
    FLOAT,
};

enum _menu_command
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

typedef struct _MENU_OptionTypeDef // 选项结构体
{
    char *String;                     // 选项字符串
    void (*func)(void);               // 函数指针
    void *StrVarPointer;              // 附带变量 的指针
    enum _MENU_StrVarType StrVarType; // 附带变量 的类型
    uint8_t StrLen;                   // 字符串长度
} MENU_OptionTypeDef;

typedef struct _MENU_HandleTypeDef // 选项结构体
{
    MENU_OptionTypeDef *OptionList; // 选项列表指针

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

} MENU_HandleTypeDef;

/**********************************************************/
/* 宏函数 */

#define COORD_CHANGE_SIZE(sta, end) (((end) - (sta)) + 1)   // 坐标转换成尺寸 COORD_CHANGE_SIZE
#define SIZE_CHANGE_COORD(sta, size) (((sta) + (size)) - 1) // 尺寸转换成坐标 SIZE_CHANGE_COORD

// 逐步接近目标, actual当前, target目标, step_size步长
#define STEPWISE_TO_TARGET(actual, target, step_size)                                                                          \
    ((((target) - (actual)) > (0.0625))    ? ((actual) + (0.0625) + (((target) - (actual)) * (step_size)))                     \
     : (((target) - (actual)) < -(0.0625)) ? ((actual) - (0.0625) + (((target) - (actual)) * (step_size)))                     \
                                           : ((target)))

/**********************************************************/
/* driver */

int menu_command_callback(enum _menu_command command, ...);
void MENU_RunMenu(MENU_HandleTypeDef *hMENU);
void MENU_HandleInit(MENU_HandleTypeDef *hMENU);
void MENU_Event_and_Action(MENU_HandleTypeDef *hMENU);
void MENU_UpdateIndex(MENU_HandleTypeDef *hMENU);
void MENU_ShowOptionList(MENU_HandleTypeDef *hMENU);
uint8_t MENU_ShowOption(int16_t X, int16_t Y, MENU_OptionTypeDef *Option);
void MENU_ShowCursor(MENU_HandleTypeDef *hMENU);
void MENU_ShowBorder(MENU_HandleTypeDef *hMENU);

/**********************************************************/
/* use */

void MENU_RunMainMenu(void);
void MENU_RunToolsMenu(void);
void MENU_RunGamesMenu(void);
void MENU_Information(void);

/**********************************************************/
#endif
