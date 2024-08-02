//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_common.h
// Auther      : zhaoning
// Version     :
// Date : 2023-3-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-3-18
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_COMMON_H_
#define _AM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

// Public defines / typedef -----------------------------------------------------

#define                 FWVERSION               "5.0.4"

#define                 SYS_THREAD_STACK_SIZE                   (1024)

enum 
{
    SEND_TASK_MSG_HB_MSG = 0,
    SEND_TASK_MSG_RSSI_MSG,
    SEND_TASK_MSG_RESEND_MSG,
    SEND_TASK_MSG_CLOCKUPDATA_MSG,
    SEND_TASK_MSG_GPIO_MSG,
    SEND_TASK_MSG_ALARM_MSG
};

enum 
{
    CLOCK_TASK_MSG_CLOCK_MSG = 0,
};

typedef struct _app_msgq
{
    UINT32        msgId;
}app_msgq_msg, *p_app_msgq_msg;

// Public functions prototypes --------------------------------------------------

int send_task_send_msgq(app_msgq_msg * msg);
int clock_task_send_msgq(app_msgq_msg * msg);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_COMMON_H_.2023-3-18 17:19:28 by: zhaoning */

