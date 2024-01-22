//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_mqtt.h
// Auther      : zhaoning
// Version     :
// Date : 2023-8-28
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-28
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_MQTT_H_
#define _AM_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdbool.h>

#include "sdk_api.h"

#include "am.h"

#ifdef DTU_BASED_ON_MQTT
// Public defines / typedefs ----------------------------------------------------

// Public functions prototypes --------------------------------------------------

void dtu_mqtt_recv_cbk(UINT8 *data, UINT32 len);
void dtu_mqtt_subscribe_topic(void);
void dtu_mqtt_wait_init_mqtt(void);
void dtu_mqtt_wait_connect_mqtt(void);
void dtu_mqtt_send_serial_data(DTU_MSG_UART_DATA_PARAM_T * uartData);

#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:36:36 by: zhaoning */

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_MQTT_H_.2023-8-28 14:57:24 by: zhaoning */

