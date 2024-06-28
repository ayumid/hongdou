/* *********************************************************
 * 多级菜单v2.0
 * 作者:Adam
 *
 * 移植方法
 *
 * 1. 配置菜单宏定义, 兼容不同显示器及布局, 例如:
    #define MENU_WIDTH 128 // 菜单宽度
    #define MENU_HEIGHT 64 // 菜单高度
    #define MENU_LINE_H 20 // 行高

 * 2. 实现 menu_command_callback() 对应的指令功能以完成移植, 有些指令是有参数的, 参数已经提取好, 按需使用参数即可,
 *    详情可看CSDN博客:https://blog.csdn.net/AdminAdam/article/details/138232161

 * 使用方法
 *
 * 1. 创建选项列表,并直接初始化, 每个选项对应其名字和功能(功能为函数指针, 直接填写函数名), 例如:
    static MENU_OptionTypeDef MENU_OptionList[] = {
        {"<<<", NULL},                     // 固定格式, 用于退出
        {"Tools", MENU_RunToolsMenu},      // 工具
        {"Games", MENU_RunGamesMenu},      // 游戏
        {"Setting", MENU_RunSettingMenu},  // 设置
        {"Information", MENU_Information}, // 信息
        {"..", NULL},                      // 固定格式, 用于计算选项列表长度和退出
    };

 * 2. 创建菜单句柄 并把菜单句柄内的选项列表指针指向 第1 步创建的选项列表, 例如
    static MENU_HandleTypeDef MENU = {.OptionList = MENU_OptionList};

 * 3. 调用 MENU_RunMenu() 运行菜单, 参数为菜单句柄
    MENU_RunMenu(&MENU);
 *
 * 4. 为了实现多级菜单, 可使用一个函数来封装 第 1 2 3 步, 封装好的函数可作为功能被其他菜单调用, 以此实现不限层级多级菜单, 此文件底部提供了示例代码
 *    例如 void MENU_RunToolsMenu(void) 被选项 {"Tools", MENU_RunToolsMenu} 调用;
 *
 *
 * 视频教程:https://www.bilibili.com/video/BV1Y94y1g7mu?p=2
 * CSDN博客:https://blog.csdn.net/AdminAdam/article/details/138232161
 *
 * 下载链接
 * 百度网盘:https://pan.baidu.com/s/1bZPWCKaiNbb-l1gpAv6QNg?pwd=KYWS
 * Gitee: https://gitee.com/AdamLoong/Embedded_Menu_Simple
 * GitHub:https://github.com/AdamLoong/Embedded_Menu_Simple
 *
 * B站UP:加油哦大灰狼
 * 如果此程序对你有帮助记得给个一键三连哦! ( •̀ ω •́ )✧
 ********************************************************* */

#include "MENU.h"
#include "stdio.h"

/* 配置菜单 */
#define MENU_X 0       // 菜单位置X
#define MENU_Y 0       // 菜单位置Y
#define MENU_WIDTH 128 // 菜单宽度
#define MENU_HEIGHT 64 // 菜单高度

#define MENU_LINE_H 20 // 行高
#define MENU_PADDING 2 // 内边距
#define MENU_MARGIN 2  // 外边距

#define MENU_FONT_W 8  // 字体宽度
#define MENU_FONT_H 16 // 字体高度

#define MENU_BORDER 1         // 边框线条尺寸
#define IS_CENTERED 1         // 是否居中
#define IS_OVERSHOOT 1        // 是否过冲 (果冻效果)
#define OVERSHOOT 0.321       // 过冲量 0 < 范围 < 1;
#define ANIMATION_SPEED 0.321 // 动画速度 0 < 范围 <= 1;

#define CURSOR_CEILING (((MENU_HEIGHT - MENU_MARGIN - MENU_MARGIN) / MENU_LINE_H) - 1) // 光标限位

/** Port 移植接口 * **************************************************************/
/* 依赖头文件 */
#include "Key.h"
#include "OLED.h"

/// @brief 菜单指令回调函数
/// @param  command 指令
/// @param  ... 可变参数列表根据指令定义
/// @return  返回值根据指令定义
int menu_command_callback(enum _menu_command command, ...)
{
    int retval = 0;

    switch (command)
    {
        /* Output */
    case BUFFER_DISPLAY: // 无参无返
    {
        OLED_Update();
    }
    break;

    case BUFFER_CLEAR: // 无参无返
    {
        OLED_Clear();
    }
    break;

    case SHOW_STRING: // 参数:( int16_t x, int16_t y, char *str ); 返回: uint8_t 字符串长度;
    {
        /* 提取参数列表 */
        int *arg_list = ((int *)&command) + 1; // 指针偏移4字节, 指向第一个参数
        int show_x = arg_list[0];
        int show_y = arg_list[1];
        char *show_string = (char *)arg_list[2];

        /* 按需使用参数 */
        retval = OLED_Printf(show_x, show_y, MENU_FONT_W, show_string); // 显示字符串
        // retval = strlen(show_string); // 如果你的显示字符串函数没有返回值, 也可以使用strlen()函数获取字符串长度
    }
    break;

    case SHOW_CURSOR: // 参数:( int xsta, int ysta, int xend, int yend ); 返回: 无;
    {
        /* 提取参数列表 */
        int *arg_list = ((int *)&command) + 1;
        int cursor_xsta = arg_list[0];
        int cursor_ysta = arg_list[1];
        int cursor_xend = arg_list[2];
        int cursor_yend = arg_list[3];

        /* 按需使用参数 */
        OLED_ReverseArea(cursor_xsta, cursor_ysta, COORD_CHANGE_SIZE(cursor_xsta, cursor_xend),
                         COORD_CHANGE_SIZE(cursor_ysta, cursor_yend));
        // OLED_DrawRectangle(cursor_xsta, cursor_ysta, COORD_CHANGE_SIZE(cursor_xsta, cursor_xend),
        //                    COORD_CHANGE_SIZE(cursor_ysta, cursor_yend), 0);
    }
    break;

    case DRAW_FRAME: // 参数:( int xsta, int ysta, int wide, int high ); 返回: 无;
    {
        /* 提取参数列表 */
        int *arg_list = ((int *)&command) + 1;
        int frame_x = arg_list[0];
        int frame_y = arg_list[1];
        int frame_width = arg_list[2];
        int frame_height = arg_list[3];

        /* 按需使用参数 */
        OLED_DrawRectangle(frame_x, frame_y, frame_width, frame_height, 0);
    }
    break;

    /* Input */
    case GET_EVENT_ENTER: // 参数: 无; 返回: 布尔;
    {
        retval = Key_GetEvent_Enter() || Key_GetEvent_Right(); // 这代表两个按键都可以触发这个事件
    }
    break;

    case GET_EVENT_BACK: // 参数: 无; 返回: 布尔;
    {
        retval = Key_GetEvent_Back();
    }
    break;

    case GET_EVENT_WHEEL: // 参数: 无; 返回: int16_t 滚动量;
    {
        if (Key_GetEvent_Up())
            retval = 1;
        else if (Key_GetEvent_Down())
            retval = -1;
        else
            retval = Key_Encoder_Take(&Encoder1); // 这代表上下按键及编码器旋钮都可以触发这个事件, 注意返回值是有符号的
    }
    break;

    default:
        break;
    }

    return retval;
}

/* ***************************************************** Port 移植接口 ** */

/* ******************************************************** */

/// @brief 菜单运行函数
/// @param hMENU 菜单句柄
void MENU_RunMenu(MENU_HandleTypeDef *hMENU)
{
    MENU_HandleInit(hMENU); // 初始化

    while (hMENU->isRun)
    {
        menu_command_callback(BUFFER_CLEAR); // 擦除缓冲区

        MENU_ShowOptionList(hMENU); /* 显示选项列表 */
        MENU_ShowCursor(hMENU);     /* 显示光标 */
        MENU_ShowBorder(hMENU);     // 显示边框

        menu_command_callback(BUFFER_DISPLAY); // 缓冲区更新至显示器

        MENU_Event_and_Action(hMENU); // 检查事件及作相应操作
    }
}

void MENU_HandleInit(MENU_HandleTypeDef *hMENU)
{
    hMENU->isRun = 1;                // 运行标志
    hMENU->AnimationUpdateEvent = 1; // 动画更新事件
    hMENU->Catch_i = 1;              // 选中下标默认为1,(因为hMENU->OptionList[0]为"<<<")
    hMENU->Cursor_i = 0;             // 光标下标默认为0
    hMENU->Show_i = 0;               // 显示(遍历)起始下标
    hMENU->Show_i_Previous = 1;      // CURSOR_CEILING + 1; // 上一次循环的显示下标
    hMENU->Option_Max_i = 0;         // 选项列表长度

    for (hMENU->Option_Max_i = 0; hMENU->OptionList[hMENU->Option_Max_i].String[0] != '.';
         hMENU->Option_Max_i++) // 计算选项列表长度
    {
        hMENU->OptionList[hMENU->Option_Max_i].StrLen =
            MENU_ShowOption(0, 0, &hMENU->OptionList[hMENU->Option_Max_i]); // 获取字符串长度
    }
    hMENU->Option_Max_i--; // 不显示".."
}

void MENU_Event_and_Action(MENU_HandleTypeDef *hMENU)
{
    if (menu_command_callback(GET_EVENT_ENTER)) /* 确定事件 如果选中的选项功能不为空则执行功能, 否则退出运行 */
    {
        if (hMENU->OptionList[hMENU->Catch_i].func != NULL)
        {
            hMENU->OptionList[hMENU->Catch_i].func();
        }
        else
        {
            hMENU->isRun = 0;
        }
        hMENU->AnimationUpdateEvent = 1;
    }
    else if (menu_command_callback(GET_EVENT_BACK)) /* 返回事件 退出运行 */
    {
        hMENU->isRun = 0;
    }
    else
    {
        hMENU->Wheel_Event = -menu_command_callback(GET_EVENT_WHEEL);

        if (hMENU->Wheel_Event) /* 滚动事件 更新选中下标和光标下标 */
        {
            MENU_UpdateIndex(hMENU);
            hMENU->AnimationUpdateEvent = 1;
        }
    }
}

void MENU_UpdateIndex(MENU_HandleTypeDef *hMENU)
{
    /* 更新下标 */
    hMENU->Cursor_i += hMENU->Wheel_Event;
    hMENU->Catch_i += hMENU->Wheel_Event;

    /* 限制选中下标 */
    if (hMENU->Catch_i > hMENU->Option_Max_i)
        hMENU->Catch_i = hMENU->Option_Max_i;

    if (hMENU->Catch_i < 0)
        hMENU->Catch_i = 0;

    /* 限制光标下标 */
    if (hMENU->Cursor_i > CURSOR_CEILING)
        hMENU->Cursor_i = CURSOR_CEILING;

    if (hMENU->Cursor_i > hMENU->Option_Max_i)
        hMENU->Cursor_i = hMENU->Option_Max_i;

    if (hMENU->Cursor_i > hMENU->Catch_i)
        hMENU->Cursor_i = hMENU->Catch_i;

    if (hMENU->Cursor_i < 0)
        hMENU->Cursor_i = 0;
}

void MENU_ShowOptionList(MENU_HandleTypeDef *hMENU)
{
    static float VerticalOffsetBuffer; // 垂直偏移缓冲

    /* 计算显示起始下标 */
    hMENU->Show_i = hMENU->Catch_i - hMENU->Cursor_i; // 详解 https://www.bilibili.com/read/cv32114635/?jump_opus=1

    if (hMENU->Show_i_Previous != hMENU->Show_i) // 如果显示下标有变化
    {
        VerticalOffsetBuffer = ((hMENU->Show_i - hMENU->Show_i_Previous) * MENU_LINE_H); // 计算垂直偏移缓冲量
        hMENU->Show_i_Previous = hMENU->Show_i;
    }

    if (VerticalOffsetBuffer)
    {
        VerticalOffsetBuffer = STEPWISE_TO_TARGET(VerticalOffsetBuffer, 0, ANIMATION_SPEED); // 行显示偏移量逐渐归零
    }

    for (int16_t i = -1; i <= CURSOR_CEILING + 1; i++) // 遍历显示 选项
    {
        if (hMENU->Show_i + i < 0)
            continue;

        if (hMENU->Show_i + i > hMENU->Option_Max_i)
            break;

#if (IS_CENTERED != 0)
        int16_t x = MENU_X + ((MENU_WIDTH - (hMENU->OptionList[hMENU->Show_i + i].StrLen * MENU_FONT_W)) / 2); // 水平居中
#else
        int16_t x = MENU_X + MENU_MARGIN + MENU_PADDING; // 左对齐加边距
#endif

        int16_t y = MENU_Y + MENU_MARGIN + (i * MENU_LINE_H) + ((MENU_LINE_H - MENU_FONT_H) / 2) + (int)VerticalOffsetBuffer;

        /* 显示选项, 并记录长度 */
        hMENU->OptionList[hMENU->Show_i + i].StrLen = MENU_ShowOption(x, y, &hMENU->OptionList[hMENU->Show_i + i]);
    }
}

uint8_t MENU_ShowOption(int16_t X, int16_t Y, MENU_OptionTypeDef *Option)
{
    char String[64]; // 定义字符数组

    switch (Option->StrVarType)
    {
    case INT8:
        sprintf(String, Option->String, *(int8_t *)Option->StrVarPointer);
        break;

    case UINT8:
        sprintf(String, Option->String, *(uint8_t *)Option->StrVarPointer);
        break;

    case INT16:
        sprintf(String, Option->String, *(int16_t *)Option->StrVarPointer);
        break;

    case UINT16:
        sprintf(String, Option->String, *(uint16_t *)Option->StrVarPointer);
        break;

    case INT32:
        sprintf(String, Option->String, *(int32_t *)Option->StrVarPointer);
        break;

    case UINT32:
        sprintf(String, Option->String, *(uint32_t *)Option->StrVarPointer);
        break;

    case CHAR:
        sprintf(String, Option->String, *(char *)Option->StrVarPointer);
        break;

    case STRING:
        sprintf(String, Option->String, (char *)Option->StrVarPointer);
        break;

    case FLOAT:
        sprintf(String, Option->String, *(float *)Option->StrVarPointer);
        break;

    default:
        sprintf(String, Option->String, (void *)Option->StrVarPointer);
        break;
    }

    return menu_command_callback(SHOW_STRING, X, Y, String); // 显示字符数组（字符串）
}

void MENU_ShowCursor(MENU_HandleTypeDef *hMENU)
{
    static float actual_xsta, actual_ysta, actual_xend, actual_yend; // actual
    static float target_xsta, target_ysta, target_xend, target_yend; // target

#if (IS_OVERSHOOT != 0)
    static float bounce_xsta, bounce_ysta, bounce_xend, bounce_yend;                   // bounce
    static uint8_t bounce_cnt_xsta, bounce_cnt_ysta, bounce_cnt_xend, bounce_cnt_yend; // bounce

#endif

    // static uint16_t Catch_i_Previous = 1, Cursor_i_Previous = 1; // 上一循环的状态

    if (hMENU->AnimationUpdateEvent)
    {
        hMENU->AnimationUpdateEvent = 0;

        uint16_t cursor_width = (MENU_PADDING + (hMENU->OptionList[hMENU->Catch_i].StrLen * MENU_FONT_W) + MENU_PADDING);
        uint16_t cursor_height = MENU_LINE_H;

#if (IS_CENTERED != 0)
        target_xsta = MENU_X + ((MENU_WIDTH - cursor_width) / 2);
#else
        target_xsta = MENU_X + MENU_MARGIN;
#endif
        target_ysta = MENU_Y + MENU_MARGIN + (hMENU->Cursor_i * MENU_LINE_H);
        target_xend = SIZE_CHANGE_COORD(target_xsta, cursor_width);
        target_yend = SIZE_CHANGE_COORD(target_ysta, cursor_height);

#if (IS_OVERSHOOT != 0)
        bounce_xsta = target_xsta + (target_xsta - actual_xsta) * OVERSHOOT;
        bounce_ysta = target_ysta + (target_ysta - actual_ysta) * OVERSHOOT;
        bounce_xend = target_xend + (target_xend - actual_xend) * OVERSHOOT;
        bounce_yend = target_yend + (target_yend - actual_yend) * OVERSHOOT;

        bounce_cnt_xsta = 2; // 反弹次数
        bounce_cnt_ysta = 2;
        bounce_cnt_xend = 2;
        bounce_cnt_yend = 2;
#endif
    }

#if (IS_OVERSHOOT != 0)

    if (bounce_xsta == actual_xsta)
    {
        if (bounce_cnt_xsta--)
            bounce_xsta = target_xsta + (target_xsta - actual_xsta) * OVERSHOOT;
        else
            bounce_xsta = target_xsta;
    }
    if (bounce_ysta == actual_ysta)
    {
        if (bounce_cnt_ysta--)
            bounce_ysta = target_ysta + (target_ysta - actual_ysta) * OVERSHOOT;
        else
            bounce_ysta = target_ysta;
    }
    if (bounce_xend == actual_xend)
    {
        if (bounce_cnt_xend--)
            bounce_xend = target_xend + (target_xend - actual_xend) * OVERSHOOT;
        else
            bounce_xend = target_xend;
    }
    if (bounce_yend == actual_yend)
    {
        if (bounce_cnt_yend--)
            bounce_yend = target_yend + (target_yend - actual_yend) * OVERSHOOT;
        else
            bounce_yend = target_yend;
    }

    actual_xsta = STEPWISE_TO_TARGET(actual_xsta, bounce_xsta, ANIMATION_SPEED);
    actual_ysta = STEPWISE_TO_TARGET(actual_ysta, bounce_ysta, ANIMATION_SPEED);
    actual_xend = STEPWISE_TO_TARGET(actual_xend, bounce_xend, ANIMATION_SPEED);
    actual_yend = STEPWISE_TO_TARGET(actual_yend, bounce_yend, ANIMATION_SPEED);

#else

    actual_xsta = STEPWISE_TO_TARGET(actual_xsta, target_xsta, ANIMATION_SPEED);
    actual_ysta = STEPWISE_TO_TARGET(actual_ysta, target_ysta, ANIMATION_SPEED);
    actual_xend = STEPWISE_TO_TARGET(actual_xend, target_xend, ANIMATION_SPEED);
    actual_yend = STEPWISE_TO_TARGET(actual_yend, target_yend, ANIMATION_SPEED);

#endif

    menu_command_callback(SHOW_CURSOR, (int)(actual_xsta + 0.5), (int)(actual_ysta + 0.5), (int)(actual_xend + 0.5),
                          (int)(actual_yend + 0.5));
}

void MENU_ShowBorder(MENU_HandleTypeDef *hMENU) // 显示边框
{
    for (int16_t i = 0; i < MENU_BORDER; i++)
    {
        menu_command_callback(DRAW_FRAME, MENU_X + i, MENU_Y + i, MENU_WIDTH - i - i, MENU_HEIGHT - i - i);
    }
}

/* ******************************************************** */

/* ******************************************************** */
/* 应用示例 */

void MENU_RunMainMenu(void)
{
    static MENU_OptionTypeDef MENU_OptionList[] = {{"<<<"},
                                                   {"Tools", MENU_RunToolsMenu},      // 工具
                                                   {"Games", MENU_RunGamesMenu},      // 游戏
                                                   {"Setting", NULL},                 // 设置
                                                   {"Information", MENU_Information}, // 信息
                                                   {".."}};

    static MENU_HandleTypeDef MENU = {.OptionList = MENU_OptionList};

    MENU_RunMenu(&MENU);
}

void MENU_RunToolsMenu(void)
{
    static MENU_OptionTypeDef MENU_OptionList[] = {{"<<<"},
                                                   {"Seria", NULL},        // 串口
                                                   {"Oscilloscope", NULL}, // 示波器
                                                   {"PWM Output", NULL},   // PWM 输出
                                                   {"PWM Input", NULL},    // PWM 输入
                                                   {"ADC Input", NULL},    // ADC 输入
                                                   {".."}};

    static MENU_HandleTypeDef MENU = {.OptionList = MENU_OptionList};

    MENU_RunMenu(&MENU);
}

void MENU_RunGamesMenu(void)
{
    static MENU_OptionTypeDef MENU_OptionList[] = {{"<<<"},
                                                   {"Snake", NULL},        // 贪吃蛇
                                                   {"Snake II", NULL},     // 贪吃蛇2
                                                   {"Snake III", NULL},    // 贪吃蛇3
                                                   {"Game of Life", NULL}, // 康威生命游戏
                                                   {".."}};

    static MENU_HandleTypeDef MENU = {.OptionList = MENU_OptionList};

    MENU_RunMenu(&MENU);
}

void MENU_Information(void)
{
    menu_command_callback(BUFFER_CLEAR);
    menu_command_callback(SHOW_STRING, 5, 0, "menu v2.0\nBy:Adam\nbilibili\nUP:加油哦大灰狼");
    menu_command_callback(BUFFER_DISPLAY);

    while (1)
    {

        if (menu_command_callback(GET_EVENT_ENTER))
            return;

        if (menu_command_callback(GET_EVENT_BACK))
            return;
    }
}

/**********************************************************/
