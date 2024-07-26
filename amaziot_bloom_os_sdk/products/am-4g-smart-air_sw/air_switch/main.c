//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-7-2
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-7-2
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "sockets.h"
#include "teldef.h"

#include "am_at.h"
#include "am_gpio.h"
#include "am_socket.h"

#include "am.h"

#include "utils_string.h"

// Private defines / typedefs ---------------------------------------------------

//关机：OnKeyPoweroff_Start()，重启：PM812_SW_RESET();

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

#ifndef DTU_TYPE_EXSIM
extern UINT32 SDK_INIT_SIM_SELECT_VALUE;
#endif

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

extern BOOL IsAtCmdSrvReady(void);
extern void PM812_SW_RESET(void);
extern int SendATCMDWaitResp(int sATPInd,char *in_str, int timeout, char *ok_fmt, int ok_flag,
                            char *err_fmt, char *out_str, int resplen);

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

/**
  * Function    : dtu_version_to_at_uart
  * Description : 输出型号到AT串口
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_version_to_at_uart(void)
{
    //串口输出版本
    char arrStr[64] = {0};
    
    snprintf(arrStr, 64, "%s", "AMAZIOT AP4000MT_430E\r\n");
    
    UART_SEND_DATA((UINT8 *)arrStr, strlen(arrStr));
}

void Phase1Inits_enter(void)
{
//    s_uart_cfg uartCfg = {0};
    char ver_data[128] = {0};
    // 设置UART初始化电平，只能在Phase1Inits_enter调用
    /*
    0: 1.8v -- default
    1: 2.8V
    2: 3.3V
    */
//    UART_INIT_VOLT(2);
//    UART_SET_RDY_REPORT_FLAG(0);
    snprintf(ver_data, 128, "SWVER: [%s] BUILDTIME: [%s %s]", DTU_VERSION, BUILD_DATE, BUILD_TIME);
    update_the_cp_ver(ver_data);    //可以使用AT+CPVER读取，部分客户需要提供版本给原厂生产，便于原厂识别版本号

#ifndef DTU_TYPE_EXSIM
    //使用SIM2
    SDK_INIT_SIM_SELECT_VALUE = 2;
#endif

}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    OSA_STATUS status = 0;

    //初始化信息存储文件，包括激活码，各种标志位等
    dtu_trans_conf_file_init();
#ifdef DTU_TYPE_3IN1
    //初始化sim卡
    dtu_trans_sim_init();
#endif
    //初始化OLED任务
    air_sw_task_init();
    //初始化led灯控制io
    dtu_trans_net_led_init();  
    //初始化AT 透传串口
    dtu_trans_uart_init();
    //初始化心跳定时器
    dtu_hb_timer_init();
    //初始化透传任务，需要先初始化串口，否则后面串口输出失败
    dtu_trans_task_init();
    //输出型号到AT串口
    dtu_version_to_at_uart();

#ifdef DTU_BASED_ON_TCP
    //初始化TCP任务
    dtu_sokcet_task_init();
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 16:44:19 by: zhaoning */

#ifdef DTU_TYPE_JSON_INCLUDE
    //初始化和服务器之间json通信任务
    dtu_json_task_init();
#endif /* ifdef DTU_TYPE_CJSON_INCLUDE.2023-10-30 18:33:25 by: zhaoning */
    
#ifdef DTU_TYPE_485_INCLUDE
    //初始化modbus任务
    dtu_485_task_init();
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:30:49 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
    //初始化http任务
    dtu_http_s_task_init();
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:07:28 by: zhaoning */

    //网络保持任务初始化
    dtu_checknet_task_init();
    //输出个日志
    uprintf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);

}

// End of file : main.c 2023-7-2 17:24:37 by: zhaoning 

