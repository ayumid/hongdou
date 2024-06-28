#ifndef tm_lwm2m_h
#define tm_lwm2m_h

#include "plat_time.h"
#include "plat_osl.h"
#include "iot_api.h"
#include "log.h"
#include "nbiot_m2m.h"
#include "utils.h"

extern int tm_lwm2m_step(int timeout_ms);
extern int tm_lwm2m_login();
extern int32_t tm_lwm2m_send_packet(uint8_t *topic,uint8_t *payload,int payload_len,uint32_t timeout_ms);
extern int tm_lwm2m_deinit();
extern int tm_lwm2m_logout(uint32_t timeout_ms);
extern int tm_lwm2m_init(int32_t (*callback)(const uint8_t * /** data_name*/, uint8_t * /** data*/, uint32_t /** data_len*/));

#endif