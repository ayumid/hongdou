//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_gpio.h
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
#ifndef _AM_GPIO_H_
#define _AM_GPIO_H_

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

// Public functions prototypes --------------------------------------------------

void gpio_open(UINT32 portHandle, OSFlagRef OsFlagRef);
void gpio_close(UINT32 portHandle, OSATimerRef OsaTimeRef);
void init_led_status(void);
void init_gpio_conf(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_GPIO_H_.2023-3-18 17:20:53 by: zhaoning */

