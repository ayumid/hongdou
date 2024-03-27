/**
 * @file tuya_config.h
 * @brief IoT specific configuration file
 */

#ifndef TUYA_CONFIG_H_
#define TUYA_CONFIG_H_

#include "utils_common.h"

#define TUYA_PRODUCT_KEY      "fccbvoo8ppsibxif" // for test
#define TUYA_DEVICE_UUID      "uuidb870b71bdf8316bb"
#define TUYA_DEVICE_AUTHKEY   "jmfTflF3uxlQWALwPWA9MAPZZXOLyCii"

int switch_demo_non_qrcode(void);
int tuya_send_multiple_value_data(char* buf);
int tuya_send_csq_data(char* buf);

#endif