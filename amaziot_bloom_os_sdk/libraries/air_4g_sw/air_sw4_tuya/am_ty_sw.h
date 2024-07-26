//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_ty_sw.h
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
#ifndef _AM_TY_SW_H_
#define _AM_TY_SW_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

// Public defines / typedef -----------------------------------------------------

#define GPIO_KEY_PIN0  21//16 pin
#define GPIO_KEY_PIN1  24//19 pin
#define GPIO_KEY_PIN2  22//21 pin
#define GPIO_KEY_PIN3  13//26 pin

#define GPIO_CLR_PIN0  16//30 pin
#define GPIO_CLR_PIN1  17//31 pin
#define GPIO_CLR_PIN2  19//32 pin
#define GPIO_CLR_PIN3  18//33 pin

#define GPIO_LED_STATUS  53//22 pin

#define GPIO_DD_ALARM  54//23 pin
#define SEND_MSGQ_TASK_MSGQ_MSG_SIZE              (sizeof(app_msgq_msg))
#define SEND_MSGQ_TASK_MSGQ_QUEUE_SIZE            (200)

typedef enum
{
    NORMAL = 0,
    NOGPRS,
    NOSERVER,
    UNREGISTERED,
    UPGRADE,
    NOTLIVE,
    FACTORY
}GSMStatus;
typedef struct
{
    char deviceId[16];
    char factory_apikey[64];
    char sta_mac[32];
    char sap_mac[32];
    char device_model[16];
    
    char linshi[64];
    char zhengshi[64];
    
    UINT8 initstatus0;
    UINT8 initstatus1;
    UINT8 initstatus2;
    UINT8 initstatus3;
    UINT8 activeFlag;

    UINT8 sdstatus0;
    UINT8 sdstatus1;
    UINT8 sdstatus2;
    UINT8 sdstatus3;
    
    char license[256];
    UINT16 licenseLen;

    int pulsetime0;
    int pulsetime1;
    int pulsetime2;
    int pulsetime3;

    UINT8 sledstatus;
    UINT8 lockstatus;
    
}trans_conf;
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

void gpio_open(UINT32 portHandle, OSFlagRef OsFlagRef);
void gpio_close(UINT32 portHandle, OSATimerRef OsaTimeRef);
void init_led_status(void);
void init_gpio_conf(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TY_SW_H_.2023-3-18 17:20:53 by: zhaoning */

