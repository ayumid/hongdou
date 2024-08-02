//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_at.c
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

// Includes ---------------------------------------------------------------------

#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "teldef.h"
#include "download.h"

#include "utils_string.h"

#include "am_at.h"
#include "am_gpio.h"
#ifdef DTU_TYPE_DODIAI_INCLUDE
#include "am_di.h"
#include "am_do.h"
#include "am_ai.h"
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-31 16:11:26 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
#include "am_utils_sha1.h"
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 17:34:15 by: zhaoning */

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static DTU_AT_CMD_PARA_T dtu_atcmd_param;
static unsigned int dtu_at_tmp_count = 0;
static char dtu_at_cmd_tmp[DTU_CMD_LINE_MAX_LINE_SIZE];
static DTU_DSAT_SIO_INFO_TYPE_E g_sio_info;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

#ifdef DTU_BASED_ON_TCP
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setip(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheartflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheart(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlinkflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlink(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setyzcsq(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:36:43 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setaliauth(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttconn(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttsub(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttpub(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttheart(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttlink(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:00:42 by: zhaoning */

static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_help(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_version(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reload(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reset(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto0(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcmdpw(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setati(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_ota(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmi(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmm(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);

#ifdef DTU_BASED_ON_TCP
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setip(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hbflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hb(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_linkflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_link(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:36:52 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setaliauth(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttconn(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttsub(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttpub(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttheart(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttlink(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:01:59 by: zhaoning */

static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_query(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reset(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reload(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_version(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_yzcsq(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);

static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_o(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_cmdpw(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setati(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_ota(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmi(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmm(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);

#ifdef DTU_TYPE_GNSS_INCLUDE
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setgps(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setgps(DTU_AT_CMD_PARA_T *pParam,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif

#ifdef DTU_TYPE_3IN1
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsim(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsimlock(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsim(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsimlock(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif

#ifdef DTU_TYPE_DODIAI_INCLUDE
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dicfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_docfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_doflow(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_aicfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_diwr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dowr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dird(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dord(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_aird(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_clock(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_flow(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dicfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_docfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_doflow(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_aicfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_diwr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dowr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dird(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dord(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_aird(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_clock(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_flow(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:45:00 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbcfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbrescfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbadd(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbaddwn(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbcfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbrescfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbadd(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbaddwn(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:27:32 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_httpcfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_httpadd(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_httpcfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_httpadd(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:09:43 by: zhaoning */

#ifdef DTU_TYPE_FORMAT_INCLUDE
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_formatcfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_formatcfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_formatht(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_formatht(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_TYPE_FORMAT_INCLUDE.2023-12-8 9:46:38 by: zhaoning */

#ifdef DTU_TYPE_AGNSS_INCLUDE
//基于中科微agnss
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_agnsscfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_agnsscfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_agnssparams(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_agnssparams(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_TYPE_AGNSS_INCLUDE.2024-1-3 10:14:33 by: zhaoning */

#ifdef DTU_TYPE_LORA_INCLUDE
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_lora_gateway_cfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_lora_gateway_cfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_lora_gateway_slave_parsms(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_lora_gateway_slave_parsms(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_lora_gateway_slave_del(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_lora_gateway_slave_del(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif /* ifdef DTU_TYPE_LORA_INCLUDE.2024-1-15 12:04:58 by: zhaoning */
static DTU_AT_CMD_TABLE_T dtu_atcmd_table[] = {
#ifdef DTU_BASED_ON_TCP
    { "+TRANSIP",dtu_atcmd_setip},
    { "+TRANSHEARTFLAG",dtu_atcmd_setheartflag},
    { "+TRANSHEART",dtu_atcmd_setheart},
    { "+TRANSLINKFLAG",dtu_atcmd_setlinkflag},
    { "+TRANSLINK",dtu_atcmd_setlink},
//    { "+TRANSYZCSQ",dtu_atcmd_setyzcsq},
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:37:04 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    { "+SETALIAUTH",dtu_mqtt_atcmd_setaliauth},
    { "+SETMQTTCONN",dtu_mqtt_atcmd_setmqttconn},
    { "+SETMQTTSUB",dtu_mqtt_atcmd_setmqttsub},
    { "+SETMQTTPUB",dtu_mqtt_atcmd_setmqttpub},
    { "+SETMQTTHEART",dtu_mqtt_atcmd_setmqttheart},
    { "+SETMQTTLINK",dtu_mqtt_atcmd_setmqttlink},
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:02:28 by: zhaoning */
    { "+HELP",dtu_atcmd_help},
    { "+VERSION",dtu_atcmd_version},
    { "+RELOAD",dtu_atcmd_reload},
    { "+RESET",dtu_atcmd_reset},
    { "O",dtu_atcmd_seto},
    { "O0",dtu_atcmd_seto0},
    { "+CMDPW",dtu_atcmd_setcmdpw},
#ifdef DTU_TYPE_3IN1
    { "+SIM",dtu_atcmd_setsim},
    { "+SIMLOCK",dtu_atcmd_setsimlock},
#endif
#ifdef DTU_TYPE_GNSS_INCLUDE
#ifdef DTU_BASED_ON_TCP
    { "+SETTCPGPS",dtu_atcmd_setgps},
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:37:23 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
    { "+SETMQTTGPS",dtu_atcmd_setgps},
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 17:34:57 by: zhaoning */
#endif
    { "I",dtu_atcmd_setati},
    { "+OTA",dtu_atcmd_ota},
    { "+CGMI",dtu_atcmd_setcgmi},
    { "+CGMM",dtu_atcmd_setcgmm},
    { "+CGMR",dtu_atcmd_setcgmr},

#ifdef DTU_TYPE_DODIAI_INCLUDE
    { "+DICFG",dtu_atcmd_dicfg},
//    { "+DIWR",dtu_atcmd_diwr},
    { "+DIRD",dtu_atcmd_dird},
    { "+DOCFG",dtu_atcmd_docfg},
    { "+DOFLOW",dtu_atcmd_doflow},
    { "+DOWR",dtu_atcmd_dowr},
    { "+DORD",dtu_atcmd_dord},
    { "+AICFG",dtu_atcmd_aicfg},
    { "+AIRD",dtu_atcmd_aird},
    { "+CLKCFG",dtu_atcmd_clock},
//    { "+IOFLOW",dtu_atcmd_flow},
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:48:32 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
    { "+MBCFG",dtu_atcmd_mbcfg},
    { "+MBRESCFG",dtu_atcmd_mbrescfg},
    { "+MBADD",dtu_atcmd_mbadd},
    { "+MBADDWN",dtu_atcmd_mbaddwn},
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:27:39 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
    { "+HTTPCFG",dtu_atcmd_httpcfg},
    { "+HTTPADD",dtu_atcmd_httpadd},
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:09:50 by: zhaoning */

#ifdef DTU_TYPE_FORMAT_INCLUDE
    { "+FMCFG",dtu_atcmd_formatcfg},
    { "+FMHT",dtu_atcmd_formatht},
#endif /* ifdef DTU_TYPE_FORMAT_INCLUDE.2023-12-8 9:46:06 by: zhaoning */

#ifdef DTU_TYPE_AGNSS_INCLUDE
    { "+AGNSSCFG",dtu_atcmd_agnsscfg},
    { "+AGNSSPARAMS",dtu_atcmd_agnssparams},
#endif /* ifdef DTU_TYPE_AGNSS_INCLUDE.2024-1-3 10:15:14 by: zhaoning */

#ifdef DTU_TYPE_LORA_INCLUDE
    { "+LORACFG",dtu_atcmd_lora_gateway_cfg},
    { "+LORASPARAMS",dtu_atcmd_lora_gateway_slave_parsms},
    { "+LORADEL",dtu_atcmd_lora_gateway_slave_del}
#endif /* ifdef DTU_TYPE_LORA_INCLUDE.2024-1-15 12:07:41 by: zhaoning */
};

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_send_to_uart
  * Description : 发送数据到at串口
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_send_to_uart(char *toUart, int length)
{
    uprintf("%s[%d] length:%d\n", __FUNCTION__, __LINE__, length);    

    UART_SEND_DATA((UINT8 *)toUart, length);

}

/**
  * Function    : is_begin_with
  * Description : 判断字符串str1是否是以str2开头
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int is_begin_with(const char * str1,char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    char *p = str2;
    int i = 0;
    while(*p != '\0')
    {
        if(*p != str1[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

/**
  * Function    : is_end_with
  * Description : 判断字符串str1是否是以str2结尾
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int is_end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
    return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

/**
  * Function    : at_ParamStrParse
  * Description : 解析at指令字符参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static const uint8_t *at_ParamStrParse(DTU_AT_COMMAND_PARAM_T *param)
{
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;

    // param->value must be started and ended with double-quote
    uint8_t *s = param->value + 1;
    uint8_t *d = param->value;
    uint16_t length = param->length - 2;
    param->length = 0;
    
    while (length > 0)
    {
        uint8_t c = *s++;
        length--;
        // It is more permissive than SPEC.
        if (c == '\\' && length >= 2 && isalnum(s[0]) && isalnum(s[1]))
        {
            *d++ = (HEX2NUM(s[0]) << 4) | HEX2NUM(s[1]);
            s += 2;
            length -= 2;
        }
        else
        {
            *d++ = c;
        }
        param->length++;
    }
    *d = '\0';
    param->type = AT_CMDPARAM_STRING_PARSED;
    
    return param->value;
}

/**
  * Function    : at_ParamStr
  * Description : 判断字符参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static uint8_t *at_ParamStr(DTU_AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;
    if (param->type == AT_CMDPARAM_STRING && param->length >= 2)
        return (uint8_t *)at_ParamStrParse(param);

failed:
    *paramok = false;
    return "";
}

/**
  * Function    : at_ParamUint
  * Description : 解析at指令整形参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static UINT32 at_ParamUint(DTU_AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;

    if (param->type == AT_CMDPARAM_NUMBER || param->type == AT_CMDPARAM_RAWTEXT)
    {
        char *endptr = NULL;
        unsigned long value = strtoul((const char *)param->value, &endptr, 10);
        if (*endptr != 0)
        {
            uprintf("endptr:%s ", endptr);//test
            goto failed;

        }

        return value;
    }

failed:
    *paramok = false;
    return 0;
}

/**
  * Function    : at_ParamUintInRange
  * Description : 判断整形参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static UINT32 at_ParamUintInRange(DTU_AT_COMMAND_PARAM_T *param, UINT32 minval,
                             uint32_t maxval, bool *paramok)
{
    uint32_t res = at_ParamUint(param, paramok);
    if (*paramok && res >= minval && res <= maxval)
        return res;

    *paramok = false;
    return minval;
}

/**
  * Function    : dtu_package_at_cmd
  * Description : at指令解析
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_package_at_cmd(char *atName, char *atLine,DTU_AT_CMD_PARA_T *dtu_atcmd_param)
{
    char strParam[DTU_CMD_LINE_MAX_LINE_SIZE] = {0};
    UINT8 index = 0;
    int step = 0;
    char cc = 0;
    char *save_buf = NULL;
    char *buf_ptr = NULL;
    
    int data_len = strlen(atLine) - strlen(atName) - 1;
    buf_ptr = atLine + strlen(atName) + 1;
    save_buf = buf_ptr;
    //指令中的参数个数设置为0
    dtu_atcmd_param->paramCount = 0;
    
    //指令后面没有问号，等于号，等于号加问号
    if(strlen(atLine) == strlen(atName))
    {
        uprintf("dtu_atcmd_param->iType = AT_CMD_EXE");
        dtu_atcmd_param->iType = AT_CMD_EXE;
        return 0;
    }
    //指令后是问号的情况
    if(*(atLine + strlen(atName)) == '?')
    {
        uprintf("dtu_atcmd_param->iType = AT_CMD_READ");
        if(strlen(atLine)- strlen(atName) == 1)
            dtu_atcmd_param->iType = AT_CMD_READ;
        return 0;
    }
    //指令后是等于号加问号的情况
    if( (*(atLine + strlen(atName)) == '=')&&( *(atLine + strlen(atName) + 1) == '?'))
    {
        uprintf("dtu_atcmd_param->iType = AT_CMD_TEST");
        if(strlen(atLine)- strlen(atName) == 2)
            dtu_atcmd_param->iType = AT_CMD_TEST;
        return 0;
    }
    //指令为设置指令的情况
    dtu_atcmd_param->iType = AT_CMD_SET;
    //开始解析
    while (data_len > 0)
    {
//        uprintf("str b:%s",buf_ptr);
        cc = *buf_ptr++;
        data_len--;
        step++;
//        uprintf("cc:%c",cc);
        if(cc == ',')
        {
//            uprintf("test00:%s","begin0");
            //0.get the strParam and judge
            if(step < 1)
            {
//                uprintf("step < 1");
                return -1;
            }
            //连续两个逗号中间无内容，例如订阅的topic里面只设置1 - 2个topic
            if(1 == step)
            {
                dtu_atcmd_param->paramCount++;
                dtu_atcmd_param->params[index] = (DTU_AT_COMMAND_PARAM_T*)malloc(sizeof(DTU_AT_COMMAND_PARAM_T) + step * sizeof(uint8_t));
                if(dtu_atcmd_param->params[index] == NULL)
                    return -1;
                dtu_atcmd_param->params[index++]->type = AT_CMDPARAM_EMPTY;
//                uprintf("empty:%d",index);
//                uprintf("buf_ptr:%s",buf_ptr);
                save_buf = buf_ptr;
                step = 0;
                continue;
            }
//            uprintf("param:%d",index);
            memset(strParam, 0, DTU_CMD_LINE_MAX_LINE_SIZE);
            memcpy(strParam, save_buf, step - 1);
            //uprintf("strParam:%s",strParam); test

            //wait for check 
            if(step > 3)
            {
                if(!(strParam[0] == '\"' && strParam[step - 2] == '\"'))
                {
                    //if begin "
                    if(strParam[0] == '\"')
                    {
                        continue;
                    }
                    //if not all num
                    if(strspn(strParam, "0123456789") != strlen(strParam))
                        return -1;
                }
            }
            else
            {
                //if not all num
                if(strspn(strParam, "0123456789") != strlen(strParam))
                        return -1;
            }
//            uprintf("test:step:%d",step);
//            uprintf("test:buf_ptr:%s",buf_ptr);
//            uprintf("test:save_buf:%s",save_buf);
//            1.set dtu_atcmd_param->paramCount
            dtu_atcmd_param->paramCount++;
            //2.malloc dtu_atcmd_param->param
            dtu_atcmd_param->params[index] = (DTU_AT_COMMAND_PARAM_T*)malloc(sizeof(DTU_AT_COMMAND_PARAM_T) + step*sizeof(uint8_t));
            if(dtu_atcmd_param->params[index] == NULL)
                return -1;
            
            //3.set param->length
            dtu_atcmd_param->params[index]->length = step - 1;
            //4.set param-type
            if(dtu_atcmd_param->params[index]->length > 2)
            {
                if(*save_buf == '\"' && *(save_buf+step-2) == '\"')
                    dtu_atcmd_param->params[index]->type = AT_CMDPARAM_STRING;
                else
                    dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
            }
            else
                dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
            //5.set params->value
            memcpy(dtu_atcmd_param->params[index++]->value, strParam, step);
            save_buf = buf_ptr;   
            step = 0;
        }
    }
//    uprintf("test00:%s","begin1");
    //1.set param
    dtu_atcmd_param->paramCount++;
    //2.malloc dtu_atcmd_param->param
    dtu_atcmd_param->params[index] = (DTU_AT_COMMAND_PARAM_T*)malloc(sizeof(DTU_AT_COMMAND_PARAM_T) + (step + 1)*sizeof(uint8_t));
    if(dtu_atcmd_param->params[index] == NULL)
        return -1;
    //3.get the strParam and judge
    if(step < 1)
        return -1;
    memset(strParam, 0, DTU_CMD_LINE_MAX_LINE_SIZE);
    memcpy(strParam, save_buf, step);

    //wait for check 
    if(step > 3)
    {
        if(!(strParam[0] == '\"' && strParam[step - 1] == '\"')){
            //if not all num
            if(strspn(strParam, "0123456789") != strlen(strParam))
                return -1;
        }
    }
    else
    {
        //if not all num
        if(strspn(strParam, "0123456789") != strlen(strParam))
                return -1;
    }

    //4.set param->length
    dtu_atcmd_param->params[index]->length = step;
    //5.set param-type
    if(dtu_atcmd_param->params[index]->length > 2)
    {
        if(*save_buf == '\"' && *(save_buf + step - 1)=='\"')
            dtu_atcmd_param->params[index]->type = AT_CMDPARAM_STRING;
        else
            dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
    }
    else
        dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
    //6.set params->value
    memcpy(dtu_atcmd_param->params[index++]->value, strParam, step + 1);
//    uprintf("test00:%s","end");
    
    return 0;
}

/**
  * Function    : dtu_free_param
  * Description : 释放申请的内存
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_free_param(DTU_AT_CMD_PARA_T *dtu_atcmd_param)
{
    UINT8 i = 0;

    //循环释放所有申请的neicun资源
    for(i = 0; i < dtu_atcmd_param->paramCount; i++)
    {
        if(dtu_atcmd_param->params[i] != NULL)
        {
            free(dtu_atcmd_param->params[i]);
            dtu_atcmd_param->params[i] = NULL;
        }
    }
}

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_atcmd_send_responnse
  * Description : 发送at指令回复
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_atcmd_send_responnse(DTU_AT_CMD_RES_T *response)
{
    static const char *MSG_OK="\r\nOK\r\n";
    static const char *MSG_ERROR="\r\nERROR\r\n";
    char resp_buffer[DTU_CMD_LINE_RES_MAX_LINE_SIZE]={0};
    const char *msg = NULL;
    int sz = 0;
    DTU_SOCKET_PARAM_T* dtu_socket_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_socket_ctx = dtu_get_socket_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    
    //判断是否是透传模式，发送数据到服务器
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE){
        if (response->result == DSAT_OK) {
            if (response->response && response->response[0]) {
                if(response->istranscmd == DSAT_OK)
                    sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                else
                    sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
            }
            msg = MSG_OK;
        } else {
            msg = MSG_ERROR;
        }
        sprintf(resp_buffer + strlen(resp_buffer), "%s", msg);
        //judge is ATO cmd
        if(strcmp(response->response,"ATO")==0 || strcmp(response->response,"ATO0")==0)
            dtu_send_to_uart(resp_buffer, strlen(resp_buffer));
        else
            dtu_socket_write(resp_buffer, strlen(resp_buffer));
    }
    //本地AT指令发送回复到串口
    else
    {
        if (response->result == DSAT_OK)
        {
            if (response->response && response->response[0])
            {
                if(response->istranscmd == DSAT_OK)
                    sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                else
                    sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
                dtu_send_to_uart((char *)resp_buffer, sz);
            }
            msg = MSG_OK;
        }
        else
        {
            msg = MSG_ERROR;
        }
        dtu_send_to_uart((char*)msg, strlen((const char*)msg));
    }
}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 17:11:45 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
/**
  * Function    : dtu_mqtt_send_response
  * Description : 发送at指令回复
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_atcmd_send_responnse(DTU_AT_CMD_RES_T *response)
{
    char *MSG_OK="\r\nOK\r\n";
    char *MSG_ERROR="\r\nERROR\r\n";
    char resp_buffer[DTU_CMD_LINE_RES_MAX_LINE_SIZE]={0};
    char *msg = NULL;
    int sz = 0;
    DTU_UART_PARAM_T* dtu_mqtt_uart_ctx = NULL;
    DTU_MQTT_PARAM_T* dtu_mqtt_ctx = NULL;
    
    dtu_mqtt_uart_ctx = dtu_get_uart_ctx();
    dtu_mqtt_ctx = dtu_get_mqtt_ctx();
    
    if(dtu_mqtt_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        if(dtu_mqtt_is_connect(dtu_mqtt_ctx->dtu_mqtt_client))
        {
            if (response->result == DSAT_OK)
            {
                if (response->response && response->response[0])
                {
                    if(response->istranscmd == DSAT_OK)
                        sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                    else
                        sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
                }
                msg = MSG_OK;
            }
            else
            {
                msg = MSG_ERROR;
            }
            sprintf(resp_buffer + strlen(resp_buffer), "%s", msg);
            //judge is ATO cmd
            if(strcmp(response->response,"ATO")==0 || strcmp(response->response,"ATO0")==0 || strcmp(response->response,"AT+CSQ")==0 )
                dtu_send_to_uart(resp_buffer, strlen(resp_buffer));
            else
                dtu_mqtt_send(resp_buffer, strlen(resp_buffer));
        }
    }
    else
    {
        if (response->result == DSAT_OK)
        {
            if (response->response && response->response[0])
            {
                if(response->istranscmd == DSAT_OK)
                    sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                else
                    sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
                dtu_send_to_uart((char *)resp_buffer, sz);
            }
            msg = (char*)MSG_OK;
        }
        else
        {
            msg = (char*)MSG_ERROR;
        }
        
        dtu_send_to_uart(msg, strlen(msg));
    }
}
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 17:11:59 by: zhaoning */

/**
  * Function    : dtu_process_at_cmd_line
  * Description : 查找AT指令相应函数，执行相关指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_process_at_cmd_line(char *cmdName, char *cmdLine)
{
    DTU_AT_CMD_RES_T *response = NULL;
    int i = 0;

    //申请回复信息内存
    response = malloc(sizeof(DTU_AT_CMD_RES_T));
    if ( response != NULL )
    {
        //清空申请的存储区
        memset(response, 0x0, sizeof(DTU_AT_CMD_RES_T));
        //申请内存
        response->response = malloc(DTU_CMD_LINE_RES_MAX_LINE_SIZE);
        response->response[0] = 0;
        response->result = DSAT_ERROR;
        response->istranscmd = DSAT_ERROR;
        
        DTU_AT_CMD_TABLE_T *atcmd_ptr = dtu_atcmd_table;
        int nCommands = sizeof(dtu_atcmd_table) / sizeof(dtu_atcmd_table[0]);
        //在列表里查找对应的函数执行
        for (i = 0; i < nCommands; i++, atcmd_ptr++) {
            if (strcasecmp((char *)atcmd_ptr->at_cmd_name, cmdName)==0)
            {
                response->result = DSAT_OK;
                response->istranscmd = DSAT_OK;
                if (atcmd_ptr->proc_func != NULL) {
                    response->result = atcmd_ptr->proc_func(cmdName, cmdLine, response);
                }
                break;
            }
        }

        uprintf("%s[%d] cmd:%d", __FUNCTION__, __LINE__, response->istranscmd);
        //透传模式打开后，at指令无法使用，只能在这里调用SendATCMDWaitResp来实现TEL_AT_CMD_ATP_10里面的at，包含大部分at
        if(response->istranscmd == DSAT_ERROR)
        {
            char at_str[128] = {'\0'};
            sprintf(at_str, "AT%s\r",cmdLine);
            char resp_str[128] = {'\0'};
            response->result = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, NULL, 1, NULL, resp_str, sizeof(resp_str));
            uprintf("%s[%d] resp_str:%s,%d", __FUNCTION__, __LINE__,resp_str,strlen(resp_str));
            snprintf(response->response, strlen(resp_str) + 1, "%s", resp_str);
        }
        //发送回复
        dtu_atcmd_send_responnse(response);
        //释放内存，并置空
        if(NULL != response->response)
        {
            free(response->response);
            response->response = NULL;
        }
        if(NULL != response)
        {
            free(response);
            response = NULL;
        }
    }
    
    return 0;
}

/**
  * Function    : dtu_process_at_cmd_mode
  * Description : 解析AT指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_process_at_cmd_mode(const char *atCmdData, int data_len)
{
    DTU_DSAT_SIO_INFO_TYPE_E *sio_info_ptr = &g_sio_info;
    char *buf_ptr = NULL;
    char cc, atName[DTU_CMD_LINE_RES_MAX_LINE_SIZE] = {0};
    int step = 0;
    int find061 = 0;

    buf_ptr = (char*)atCmdData;
    uprintf("%s[%d]: ATCMD: %s\n", __FUNCTION__, __LINE__, buf_ptr);
    //memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));//for 32bit error
    while (data_len > 0) {
        cc = *buf_ptr++;
        data_len--;
        
        switch (sio_info_ptr->at_cmd_prep_state) {
        case DSAT_CMD_PREP_STATE_HUNT:
            if ( UPCASE( cc ) == 'A' ) {
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_A;
            }
            break;

        case DSAT_CMD_PREP_STATE_FOUND_A:
            if ( UPCASE( cc ) == 'T' ) {
                sio_info_ptr->build_cmd_ptr = sio_info_ptr->cmd_line_buffer;
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_AT;
            } else if ( UPCASE( cc ) != 'A' ) {
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;

        case DSAT_CMD_PREP_STATE_FOUND_AT:
            step++;
            if (cc != '\r') {
                if (cc == '=' || cc == '?') {
                    if(0 == find061)
                    {
                        uprintf("sio_info_ptr->cmd_line_buffer:%s\n", sio_info_ptr->cmd_line_buffer);
                        strncpy(atName, sio_info_ptr->cmd_line_buffer, step);
                        atName[step-1] = '\0';
                        uprintf("atName is %s\n", atName);
                        find061 = 1;
                    }
                }
                *sio_info_ptr->build_cmd_ptr++ = cc;
            } else {
                /*  EOL found, terminate and parse */
                *sio_info_ptr->build_cmd_ptr = '\0';

                if (!strlen(atName)) {
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step-1);
                    atName[step-1] = '\0';
                    uprintf("[%d]: atName is %s\n", __LINE__, atName);
                }

                uprintf("%s[%d]: cmd_line_buffer=%s\n", __FUNCTION__, __LINE__, sio_info_ptr->cmd_line_buffer);

                // T0D0: here should add AT command parse

                dtu_process_at_cmd_line(atName, sio_info_ptr->cmd_line_buffer);
                
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;
            
        default:
            sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            break;
        }
    }
    memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));//for 32bit error
}

/**
  * Function    : check_serial_mode
  * Description : 检查是否是"+++"，是的话退出透传
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void check_serial_mode(DTU_MSG_UART_DATA_PARAM_T *msgUartData)
{
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    //获取串口上下文结构体指针
    dtu_uart_ctx = dtu_get_uart_ctx();

    //如果当前在透传模式
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        //比对字符如果是"+++"，退出透传，同时发送"\r\nOK\r\n"到串口
        if(msgUartData->len == 3 && memcmp(msgUartData->UArgs, "+++", 3) == 0)
        {
            dtu_uart_ctx->uart_mode = DTU_AT_MODE;
            dtu_send_to_uart("\r\nOK\r\n", 6);
        }
    } 
}

/**
  * Function    : dtu_handle_serial_data
  * Description : 区别当前是在at指令模式还是数据透传模式，分别做相应处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
#ifdef DTU_TYPE_MODBUS_INCLUDE
void dtu_handle_serial_data(DTU_MSG_UART_DATA_PARAM_T *uartData)
{
    DTU_MSG_UART_DATA_PARAM_T uart_send_temp = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    dtu_file_ctx = dtu_get_file_ctx();
    
    //检查是否收到"+++"，退出透传，进入AT模式
    check_serial_mode(uartData);

    //如果是透传模式，就把数据发送到服务器
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        uprintf("It time, dtu_serial_mode is DTU_DATA_MODE!\n");

        if(DTU_MODBUS_TYPE_ENABLE == dtu_file_ctx->modbus.config.type)
        {
            uart_send_temp.UArgs = malloc(uartData->len);
            memcpy(uart_send_temp.UArgs, uartData->UArgs, uartData->len);
            uart_send_temp.len = uartData->len;
            uart_send_temp.id = DTU_MODBUS_DATA_MSG;
#ifdef DTU_TYPE_485_INCLUDE
            dtu_485_task_send_msgq(&uart_send_temp);//发送串口收到的数据给Modbus任务进行处理
#else
            dtu_modbus_task_send_msgq(&uart_send_temp);//发送串口收到的数据给Modbus任务进行处理
#endif /* ifdef DTU_TYPE_485_INCLUDE.2024-7-30 14:23:03 by: zhaoning */
        }
        else
        {
#ifdef DTU_BASED_ON_TCP
            dtu_send_serial_data_to_server(uartData);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 18:04:36 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send_serial_data(uartData);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 18:11:00 by: zhaoning */
        }
    }
    //如果是at模式，
    else if (dtu_uart_ctx->uart_mode == DTU_AT_MODE)
    {
        uprintf("It time, dtu_serial_mode is DTU_AT_MODE!\n");
        //如果在at模式下，又收到了“+++”，不响应
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
        {
            memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //如果在at模式下，收到了"\r\n"，解析at
        else if(strstr((const char*)uartData->UArgs,"\r\n"))
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            uprintf("%s[%d] send AT command uargs:%s,len:%d\n", __FUNCTION__, __LINE__,dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp) );
            dtu_process_at_cmd_mode(dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //防止at指令过长，拼接，一般不会到这里
        else
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            dtu_at_tmp_count += uartData->len;
            if(dtu_at_tmp_count > DTU_CMD_LINE_MAX_LINE_SIZE - 1)
                dtu_at_tmp_count = 0;
            
        }
    }
}
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2024-7-30 14:11:56 by: zhaoning */
#ifdef DTU_TYPE_HTTP_INCLUDE
void dtu_handle_serial_data(DTU_MSG_UART_DATA_PARAM_T *uartData)
{
    DTU_MSG_UART_DATA_PARAM_T uart_send_temp = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    dtu_file_ctx = dtu_get_file_ctx();
    
    //检查是否收到"+++"，退出透传，进入AT模式
    check_serial_mode(uartData);

    //如果是透传模式，就把数据发送到服务器
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        uprintf("It time, dtu_serial_mode is DTU_DATA_MODE!\n");

        if(DTU_HTTP_TYPE_ENABLE == dtu_file_ctx->http.config.type)
        {
            uart_send_temp.UArgs = malloc(uartData->len);
            memcpy(uart_send_temp.UArgs, uartData->UArgs, uartData->len);
            uart_send_temp.len = uartData->len;
            dtu_http_s_task_send_msgq(&uart_send_temp);//发送串口收到的数据给HTTP任务进行处理
        }
        else
        {
#ifdef DTU_BASED_ON_TCP
            dtu_send_serial_data_to_server(uartData);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 18:04:36 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
            dtu_mqtt_send_serial_data(uartData);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 18:11:00 by: zhaoning */
        }
    }
    //如果是at模式，
    else if (dtu_uart_ctx->uart_mode == DTU_AT_MODE)
    {
        uprintf("It time, dtu_serial_mode is DTU_AT_MODE!\n");
        //如果在at模式下，又收到了“+++”，不响应
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
        {
            memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //如果在at模式下，收到了"\r\n"，解析at
        else if(strstr((const char*)uartData->UArgs,"\r\n"))
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            uprintf("%s[%d] send AT command uargs:%s,len:%d\n", __FUNCTION__, __LINE__,dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp) );
            dtu_process_at_cmd_mode(dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //防止at指令过长，拼接，一般不会到这里
        else
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            dtu_at_tmp_count += uartData->len;
            if(dtu_at_tmp_count > DTU_CMD_LINE_MAX_LINE_SIZE - 1)
                dtu_at_tmp_count = 0;
            
        }
    }
}
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2024-7-30 14:13:12 by: zhaoning */
#if !defined(DTU_TYPE_MODBUS_INCLUDE) && !defined(DTU_TYPE_HTTP_INCLUDE)
void dtu_handle_serial_data(DTU_MSG_UART_DATA_PARAM_T *uartData)
{
    DTU_MSG_UART_DATA_PARAM_T uart_send_temp = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    dtu_file_ctx = dtu_get_file_ctx();
    
    //检查是否收到"+++"，退出透传，进入AT模式
    check_serial_mode(uartData);

    //如果是透传模式，就把数据发送到服务器
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        uprintf("It time, dtu_serial_mode is DTU_DATA_MODE!\n");
#ifdef DTU_BASED_ON_TCP
        dtu_send_serial_data_to_server(uartData);
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 18:04:36 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
        dtu_mqtt_send_serial_data(uartData);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 18:11:00 by: zhaoning */
    }
    //如果是at模式，
    else if (dtu_uart_ctx->uart_mode == DTU_AT_MODE)
    {
        uprintf("It time, dtu_serial_mode is DTU_AT_MODE!\n");
        //如果在at模式下，又收到了“+++”，不响应
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
        {
            memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //如果在at模式下，收到了"\r\n"，解析at
        else if(strstr((const char*)uartData->UArgs,"\r\n"))
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            uprintf("%s[%d] send AT command uargs:%s,len:%d\n", __FUNCTION__, __LINE__,dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp) );
            dtu_process_at_cmd_mode(dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //防止at指令过长，拼接，一般不会到这里
        else
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            dtu_at_tmp_count += uartData->len;
            if(dtu_at_tmp_count > DTU_CMD_LINE_MAX_LINE_SIZE - 1)
                dtu_at_tmp_count = 0;
            
        }
    }
}
#endif

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_atcmd_setip
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setip(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setip(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
        
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setheartflag
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheartflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_hbflag(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setheart
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheart(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_hb(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setlinkflag
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlinkflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_linkflag(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setlink
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlink(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_link(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_help
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_help(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_query(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setyzcsq(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
//{
//    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
//    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};
//    
//    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
//    {
//        result = dtu_at_trans_cmd_func_set_yzcsq(&dtu_atcmd_param, atLine, resp);
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    else
//    {
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    
//    return result;
//}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:45:23 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setaliauth(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_setaliauth(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_setmqttconn
  * Description : 设置mqtt连接参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttconn(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_setmqttconn(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_setmqttsub
  * Description : 设置订阅topic
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttsub(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_setmqttsub(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_setmqttpub
  * Description : 设置发布topic
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttpub(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_setmqttpub(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_setmqttheart
  * Description : 设置心跳
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttheart(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_setmqttheart(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_setmqttlink
  * Description : 这是注册包
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_setmqttlink(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param ={0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_setmqttlink(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_version
  * Description : 获取版本
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_atcmd_version(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_mqtt_at_trans_cmd_func_version(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_atcmd_help
  * Description : 查询参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_help(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_mqtt_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine, &dtu_mqtt_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_query(&dtu_mqtt_at_cmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_mqtt_at_cmd_param);
    }
    
    return result;
}

#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:03:45 by: zhaoning */
/**
  * Function    : dtu_atcmd_version
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_version(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_version(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_reload
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reload(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_reload(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_reset
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reset(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_reset(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_seto
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_o(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_seto0
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto0(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_o(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setcmdpw
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcmdpw(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd( atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_cmdpw(&dtu_atcmd_param,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setati
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setati(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setati(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_ota
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_ota(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_ota(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setcgmi
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmi(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setcgmi(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    return result;
}

/**
  * Function    : dtu_atcmd_setcgmm
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmm(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setcgmm(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setcgmr
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setcgmr(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

#ifdef DTU_BASED_ON_TCP
/**
  * Function    : dtu_at_trans_cmd_func_setip
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setip(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;//类型 1 TCP 0 UDP
    UINT8 *mode = NULL;//模式
    UINT8 *ip = NULL;//IP地址或者域名
    UINT32 port = 0;//端口号
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    //解析设置指令
    if (dtu_atcmd_param->iType == AT_CMD_SET)
    {
        //判断是否是三个参数
        if (dtu_atcmd_param->paramCount == 3)
        {
            //第一个参数，调用字符串解析函数解析
             mode = at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
             if (!paramRet)
                 return result;
             if ((strcmp((const char*)mode, "TCP") == 0) || (strcmp((const char*)mode, "tcp") == 0))
             {
                 //set trans type
                 uprintf("TCP ");
                 type = 0;
             }
             else if ((strcmp((const char*)mode, "UDP") == 0) || (strcmp((const char*)mode, "udp") == 0))
             {
                 //set trans type
                 uprintf("UDP ");
                 type = 1;
             }
             else
             {
                 return result;
             }
             //第二个参数，调用字符串解析函数解析
             ip = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
             if ((!paramRet)||(strlen((const char*)ip) > DTU_TRANS_IP_LEN))
             {
                 return result;
             }
             //第三个参数，调用整形解析函数解析
             port = at_ParamUintInRange(dtu_atcmd_param->params[2], 0, 65535, &paramRet);
             if (!paramRet)
                 return result;
             //set type
             dtu_file_ctx->socket.type = type;
             //set trans ip
             sprintf(dtu_file_ctx->socket.ip,(const char*)ip);
             //set trans port
             dtu_file_ctx->socket.port = port;
             //return ok
             if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
        }
        else
        {
            return result;
        }
    }
    //组装读指令回复内容
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        //根据类型回复
        if(dtu_file_ctx->socket.type == 0)
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s,\"%s\",%d","+TRANSIP:\"TCP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        else 
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s,\"%s\",%d","+TRANSIP:\"UDP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_hbflag
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hbflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 flag = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            flag = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 1, &paramRet);
            if (!paramRet)
                return result;
            //set heartflag
            dtu_file_ctx->hb.heartflag = flag;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->hb.heartflag);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_hb
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hb(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 time = 0;
    UINT8 *heart = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 2)
        {
            time = at_ParamUintInRange(dtu_atcmd_param->params[0], 10, 3600, &paramRet);
            if (!paramRet)
                return result;
            dtu_file_ctx->hb.hearttime = time;
            heart = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
            if ((!paramRet)||(strlen((const char*)heart) > DTU_TRANS_HEART_LEN))
            {
                return result;
            }
            //set heart
            sprintf(dtu_file_ctx->hb.heart,(const char*)heart);
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d,\"%s\"", atLine, dtu_file_ctx->hb.hearttime, dtu_file_ctx->hb.heart);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_linkflag
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_linkflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 flag = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            flag = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 3, &paramRet);
            if (!paramRet)
                return result;
            //set linkfalg
            dtu_file_ctx->reg.linkflag = flag;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->reg.linkflag);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_link
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_link(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 *link = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            link = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
            if ((!paramRet)||(strlen((const char*)link) > DTU_TRANS_LINK_LEN))
            {
             return result;
            }
            //set link
            sprintf(dtu_file_ctx->reg.link,(const char*)link);
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%s", atLine, dtu_file_ctx->reg.link);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_query
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_query(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        char arrStr[500] ={0};
        if(dtu_file_ctx->socket.type == 0)
            sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"TCP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        else
            sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"UDP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSHEARTFLAG:", dtu_file_ctx->hb.heartflag);
        sprintf(arrStr + strlen(arrStr), "%s%d,\"%s\"","\r\n+TRANSHEART:", dtu_file_ctx->hb.hearttime, dtu_file_ctx->hb.heart);
        sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSLINKFLAG:", dtu_file_ctx->reg.linkflag);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+TRANSLINK:", dtu_file_ctx->reg.link);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+CMDPW:", dtu_file_ctx->net_at.cmdpw);

#ifdef DTU_TYPE_3IN1
        sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIM:", dtu_file_ctx->sim.sim);
        sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIMLOCK:", dtu_file_ctx->sim.simlock);
#endif
#ifdef DTU_TYPE_GNSS_INCLUDE
        sprintf(arrStr + strlen(arrStr), "%s%d,%d,\"%s\"","\r\n+SETTCPGPS:", dtu_file_ctx->gnss.gpsflag, dtu_file_ctx->gnss.gpstime, dtu_file_ctx->gnss.gpsnmea);
#endif

        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:45:57 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
/**
  * Function    : dtu_mqtt_at_trans_cmd_func_setaliauth
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setaliauth(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    char* productkey = NULL;
    char* devname = NULL;
    char* devsecret = NULL;
    char* region = NULL;

    UINT32 portInt = 0;
    UINT32 keeplive = 0;
    UINT32 cleansession = 0;

    UINT8 index = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_mqtt_at_cmd_param->iType)
    {
        if (dtu_mqtt_at_cmd_param->paramCount == 7)
        {                      
            
            productkey = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen((char*)productkey) >= 64))
            {
                return result;
            }
            devname = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen((char*)devname) >= 64))
            {
                return result;
            }
            devsecret = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen((char*)devsecret) >= 64))
            {
                return result;
            }
            region = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen((char*)region) >= 64))
            {
                return result;
            }
            portInt = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            keeplive = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            cleansession = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            memset(dtu_file_ctx->mqtt.ip, 0, sizeof(dtu_file_ctx->mqtt.ip));
            strcat(dtu_file_ctx->mqtt.ip, productkey);
            strcat(dtu_file_ctx->mqtt.ip, ".iot-as-mqtt.");
            strcat(dtu_file_ctx->mqtt.ip, region);
            strcat(dtu_file_ctx->mqtt.ip, ".aliyuncs.com");
            
            dtu_file_ctx->mqtt.port = portInt;
            
            memset(dtu_file_ctx->mqtt.clientid, 0, sizeof(dtu_file_ctx->mqtt.clientid));
            strcat(dtu_file_ctx->mqtt.clientid, productkey);
            strcat(dtu_file_ctx->mqtt.clientid, ".");
            strcat(dtu_file_ctx->mqtt.clientid, devname);
            strcat(dtu_file_ctx->mqtt.clientid, "|securemode=3,signmethod=hmacsha1|");
            
            dtu_file_ctx->mqtt.keeplive = keeplive;
            dtu_file_ctx->mqtt.cleansession = cleansession;
    
            memset(dtu_file_ctx->mqtt.username, 0, sizeof(dtu_file_ctx->mqtt.username));
            strcat(dtu_file_ctx->mqtt.username, devname);
            strcat(dtu_file_ctx->mqtt.username, "&");
            strcat(dtu_file_ctx->mqtt.username, productkey);
    
            char mqttMeta[200] = {0};
            strcat(mqttMeta, "clientId");
            strcat(mqttMeta, productkey);
            strcat(mqttMeta, ".");
            strcat(mqttMeta, devname);
            strcat(mqttMeta, "deviceName");
            strcat(mqttMeta, devname);
            strcat(mqttMeta, "productKey");
            strcat(mqttMeta, productkey);
            memset(dtu_file_ctx->mqtt.password, 0, sizeof(dtu_file_ctx->mqtt.password));
            utils_hmac_sha1(mqttMeta, strlen(mqttMeta), dtu_file_ctx->mqtt.password, devsecret, strlen(devsecret));

            if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
             
        }
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_at_trans_cmd_func_setmqttconn
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttconn(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8* cmdpw = NULL;
    char* ip = NULL;
    char port[10]={"\0"};
    int portInt = 0;
    char *tmp_clientid = NULL;
    UINT16 aliveSeconds = 0;
    UINT8 cleansession = 0;
    char *tmp_username = NULL;
    char *tmp_password = NULL;
    UINT8 index = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_mqtt_at_cmd_param->iType)
    {
        if (dtu_mqtt_at_cmd_param->paramCount == 7)
        {                      
            
            ip = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen((char*)ip) >= 64))
            {
                return result;
            }
            portInt = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 65535, &paramRet);
            if (!paramRet){
                return result;
            }
            sprintf(port,"%d",portInt);
//            uprintf("port:%s",port);
            tmp_clientid = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if (!paramRet || strlen((char*)tmp_clientid) > 500)
            {
                  return result;
            }
            aliveSeconds = (UINT16)at_ParamUint(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if (!paramRet)
            {
                return result;
            }

            cleansession = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            tmp_username = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if (!paramRet || strlen((char*)tmp_username) > 500)
            {
                return result;
            }
            tmp_password = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if (!paramRet || strlen((char*)tmp_password) > 500)
            {
                return result;
            }
            sprintf(dtu_file_ctx->mqtt.ip,ip);
            dtu_file_ctx->mqtt.port = portInt;
            sprintf(dtu_file_ctx->mqtt.clientid, tmp_clientid);
            dtu_file_ctx->mqtt.keeplive = aliveSeconds;
            dtu_file_ctx->mqtt.cleansession = cleansession;
            sprintf(dtu_file_ctx->mqtt.username, tmp_username);
            sprintf(dtu_file_ctx->mqtt.password, tmp_password);    
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
             
        }
    }
    else if(dtu_mqtt_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+SETMQTTCONN:\"%s\",%d,\"%s\",%d,%d,\"%s\",\"%s\"", dtu_file_ctx->mqtt.ip,dtu_file_ctx->mqtt.port,dtu_file_ctx->mqtt.clientid,dtu_file_ctx->mqtt.keeplive,dtu_file_ctx->mqtt.cleansession,dtu_file_ctx->mqtt.username,dtu_file_ctx->mqtt.password);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_at_trans_cmd_func_setmqttsub
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttsub(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    char *subtopic = NULL;
    char *subtopic1 = NULL;
    char *subtopic2 = NULL;
    UINT8 subflag = 0;
    UINT8 subqos = 0;
    UINT8 index = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    //判断是否是设置指令
    if (AT_CMD_SET == dtu_mqtt_at_cmd_param->iType)
    {
        //判断新来的参数是否是有5个参数
        if (dtu_mqtt_at_cmd_param->paramCount == 5)
        {
            //解析第一个topic
            subtopic = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen(subtopic) >= 100))
            {
                return result;
            }
            //解析的topic写入文件缓冲区
            sprintf(dtu_file_ctx->mqtt.subtopic, subtopic);
            //判断指令是否为空
            if(AT_CMDPARAM_EMPTY != dtu_mqtt_at_cmd_param->params[index]->type)
            {
                //解析第二个topic
                subtopic1 = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
                if ((!paramRet)||(strlen(subtopic1) >= 100))
                {
                    return result;
                }
                //解析的topic写入文件缓冲区
                sprintf(dtu_file_ctx->mqtt.subtopic1, subtopic1);
            }
            else
            {
                index++;
            }
            //判断指令是否为空
            if(AT_CMDPARAM_EMPTY != dtu_mqtt_at_cmd_param->params[index]->type)
            {
                //解析第三个topic
                subtopic2 = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
                if ((!paramRet)||(strlen(subtopic2) >= 100))
                {
                    return result;
                }
                //解析的topic写入文件缓冲区
                sprintf(dtu_file_ctx->mqtt.subtopic2, subtopic2);
            }
            else
            {
                index++;
            }

            //解析第4个参数
            subflag = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet){
                return result;
            }
            //解析第5个参数
            subqos = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 2, &paramRet);
            if (!paramRet)
            {
               return result;
            }
            
            dtu_file_ctx->mqtt.subflag = subflag;
            dtu_file_ctx->mqtt.subqos = subqos;
            //写入文件
            if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    //判断是否是读指令
    else if(dtu_mqtt_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+SETMQTTSUB:\"%s\",\"%s\",\"%s\",%d,%d", dtu_file_ctx->mqtt.subtopic,dtu_file_ctx->mqtt.subtopic1,dtu_file_ctx->mqtt.subtopic2,dtu_file_ctx->mqtt.subflag,dtu_file_ctx->mqtt.subqos);
        result = DSAT_OK;
    }
    
    return result;
}

static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttpub(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    char *pubtopic = NULL;
    UINT8 pubqos = 0;
    UINT8 duplicate = 0;
    UINT8 retain = 0;
    UINT8 index = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_mqtt_at_cmd_param->iType)
    {
        if (dtu_mqtt_at_cmd_param->paramCount == 4)
        {

            pubtopic = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen((char*)pubtopic) >= 100))
            {
                return result;
            }
            pubqos = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            duplicate = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet)
            {
               return result;
            }
            retain = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            sprintf(dtu_file_ctx->mqtt.pubtopic,pubtopic);
            dtu_file_ctx->mqtt.pubqos = pubqos;
            dtu_file_ctx->mqtt.duplicate = duplicate;
            dtu_file_ctx->mqtt.retain = retain;
            
            if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        
    }
    else if(dtu_mqtt_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+SETMQTTPUB:\"%s\",%d,%d,%d", dtu_file_ctx->mqtt.pubtopic,dtu_file_ctx->mqtt.pubqos,dtu_file_ctx->mqtt.duplicate,dtu_file_ctx->mqtt.retain);
        result = DSAT_OK;
    }
    
    return result;
}

static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttheart(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 heartflag = 0;
    UINT8 hearttime = 0;
    char *heart = NULL;
    UINT8 index = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_mqtt_at_cmd_param->iType)
    {
        if(dtu_mqtt_at_cmd_param->paramCount == 1)
        {
            heartflag = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            dtu_file_ctx->hb.heartflag = heartflag;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", atLine);
                result = DSAT_OK;
            }
        }
        else if (dtu_mqtt_at_cmd_param->paramCount == 3)
        {

            
            heartflag = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            hearttime = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 10, 3600, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            heart = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen(heart) >= 50))
            {
                return result;
            }
            
            
            dtu_file_ctx->hb.heartflag = heartflag;
            dtu_file_ctx->hb.hearttime = hearttime;
            sprintf(dtu_file_ctx->hb.heart, heart);
            
            if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        
    }
    else if(dtu_mqtt_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+SETMQTTHEART:%d,%d,\"%s\"", dtu_file_ctx->hb.heartflag,dtu_file_ctx->hb.hearttime,dtu_file_ctx->hb.heart);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_mqtt_at_trans_cmd_func_setmqttlink
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_mqtt_at_trans_cmd_func_setmqttlink(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 linkflag = 0;
    char *link = NULL;
    UINT8 index = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_mqtt_at_cmd_param->iType)
    {
        if(dtu_mqtt_at_cmd_param->paramCount == 1)
        {
            linkflag = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 3, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            dtu_file_ctx->reg.linkflag = linkflag;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        else if (dtu_mqtt_at_cmd_param->paramCount == 2)
        {
            linkflag = at_ParamUintInRange(dtu_mqtt_at_cmd_param->params[index++], 0, 3, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            
            link = (char *)at_ParamStr(dtu_mqtt_at_cmd_param->params[index++], &paramRet);
            if ((!paramRet)||(strlen(link) >= 50))
            {
                return result;
            }

            dtu_file_ctx->reg.linkflag = linkflag;
            sprintf(dtu_file_ctx->reg.link,link);
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_mqtt_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+SETMQTTLINK:%d,\"%s\"", dtu_file_ctx->reg.linkflag,dtu_file_ctx->reg.link);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_query
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_query(DTU_AT_CMD_PARA_T *dtu_mqtt_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (dtu_mqtt_at_cmd_param->iType == AT_CMD_EXE)
    {
        char arrStr[500] = {0};
        sprintf(arrStr + strlen(arrStr), "%s\"%s\",%d,\"%s\",%d,%d,\"%s\",\"%s\"","+SETMQTTCONN:", dtu_file_ctx->mqtt.ip,dtu_file_ctx->mqtt.port,dtu_file_ctx->mqtt.clientid,dtu_file_ctx->mqtt.keeplive,dtu_file_ctx->mqtt.cleansession,dtu_file_ctx->mqtt.username,dtu_file_ctx->mqtt.password);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\",\"%s\",\"%s\",%d,%d","\r\n+SETMQTTSUB:",  dtu_file_ctx->mqtt.subtopic,dtu_file_ctx->mqtt.subtopic1,dtu_file_ctx->mqtt.subtopic2,dtu_file_ctx->mqtt.subflag,dtu_file_ctx->mqtt.subqos);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\",%d,%d,%d","\r\n+SETMQTTPUB:", dtu_file_ctx->mqtt.pubtopic,dtu_file_ctx->mqtt.pubqos,dtu_file_ctx->mqtt.duplicate,dtu_file_ctx->mqtt.retain);
        sprintf(arrStr + strlen(arrStr), "%s%d,%d,\"%s\"","\r\n+SETMQTTHEART:", dtu_file_ctx->hb.heartflag,dtu_file_ctx->hb.hearttime,dtu_file_ctx->hb.heart);
        sprintf(arrStr + strlen(arrStr), "%s%d,\"%s\"","\r\n+SETMQTTLINK:", dtu_file_ctx->reg.linkflag,dtu_file_ctx->reg.link);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+CMDPW:", dtu_file_ctx->net_at.cmdpw);

#ifdef DTU_TYPE_3IN1
        sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIM:", dtu_file_ctx->sim.sim);
        sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIMLOCK:", dtu_file_ctx->sim.simlock);
#endif
#ifdef DTU_TYPE_GNSS_INCLUDE
        sprintf(arrStr + strlen(arrStr), "%s%d,%d,\"%s\"","\r\n+SETMQTTGPS:", dtu_file_ctx->gnss.gpsflag, dtu_file_ctx->gnss.gpstime, dtu_file_ctx->gnss.gpsnmea);
#endif

        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}

#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:06:10 by: zhaoning */
/**
  * Function    : dtu_at_trans_cmd_func_reload
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reload(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        //init write tans_file
        dtu_init_trans_conf();
        if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
        {
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
            result = DSAT_OK;
        }
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_reset
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reset(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        PM812_SW_RESET();
        result = DSAT_OK;
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_version
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_version(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s_%s_%s_%s_%s_%s_%s-%s", DTU_TYPE, DTU_NET_MODE, DTU_CGMR, FWVERSION, DTU_VERSION, BAUDRATE, BUILD_DATE, BUILD_TIME);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_o
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_o(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
        dtu_uart_ctx->uart_mode = DTU_DATA_MODE;
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_cmdpw
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_cmdpw(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8* cmdpw = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {                      
             cmdpw = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
             if ((!paramRet)||(strlen((const char*)cmdpw) > DTU_TRANS_CMDPW_LEN))
             {
                 return result;
             }
             //set cmdpw
             sprintf(dtu_file_ctx->net_at.cmdpw,(const char*)cmdpw);
             if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
             
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%s", atLine, dtu_file_ctx->net_at.cmdpw);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setati
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setati(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "Manufacturer: %s\r\nModel: %s\r\nRevision: %s-%s\r\nCorp: %s\r\nBaud: %s", DTU_CGMI, DTU_CGMM, DTU_CGMR, DTU_VERSION, DTU_CO, BAUDRATE);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_fota_result_cbk
  * Description : dtu升级回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_fota_result_cbk(UINT32 result)
{
//    uprintf("%s: result = %ld",__FUNCTION__, result);
    if(result == FOTA_SUCCESS)
    {
        uprintf("Down Bin Ok! Moudle Rebooting For Update...");
        dtu_sleep(1);
        PM812_SW_RESET();// restart will update
    }
    else
    {
        uprintf("Fota Error,Please try again!");
    }
}

/*****************************************************************
* Function: SDK_MYFOTA_UPDATE
*
* Description:
*     该函数用于请求下载并更新本地固件
* 
* Parameters:
*     url                  [In]     目标固件的URL。
*     username          [In]     http请求服务器用户名。
*     password          [In]     http请求服务器密码。
*     cb                  [In]     fotaRunCallback参数result描述。
*                             0: 文件下载校验成功
*                             1: 文件下载校验失败
*     resetTime         [In]     文件下载成功后，重启进入Fota升级的时间，单位秒；
*                             0: 文件下载成功后，不主动重启进入fota升级，任意时间点调用重启接口或者重新上电都会进入fota升级；
*                             >0: 文件下载成功，resetTime秒后重启进入fota升级。 
* Return:
*     NONE
*
*****************************************************************/
extern void SDK_MYFOTA_UPDATE(char *url, char *username, char *password, fotaRunCallback cb, UINT32 resetTime);

static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_ota(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    char* url = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {                      
            url = (char *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
            uprintf("url:%s",url);
            SDK_MYFOTA_UPDATE(url, "", "", dtu_fota_result_cbk, 20);
            
            result = DSAT_OK;
        }
    }
    
    return result;

}

/**
  * Function    : dtu_at_trans_cmd_func_setcgmi
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmi(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", DTU_CGMI);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setcgmm
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmm(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", DTU_CGMM);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setcgmr
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s-%s", DTU_CGMR, DTU_VERSION);
        result = DSAT_OK;
    }
    
    return result;
}

#ifdef DTU_TYPE_GNSS_INCLUDE
/**
  * Function    : dtu_atcmd_setgps
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setgps(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T pParam;

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine,&pParam)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setgps(&pParam,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&pParam);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&pParam);
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setgps
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setgps(DTU_AT_CMD_PARA_T *pParam,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 gpsflag;
    UINT16 gpstime;
    char* gpsnmea;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
        
    dtu_file_ctx = dtu_get_file_ctx();

    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
            gpsflag = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
            if (!paramRet)
                return result;
            //set gpsflag
            dtu_file_ctx->gnss.gpsflag = gpsflag;
            if(0 == gpsflag)
            {
                //上报定时器停止
                dtu_gnss_timer_stop();
                dtu_gnss_pw_init();
            }
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        else if(pParam->paramCount == 3)
        {
            gpsflag = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
            if (!paramRet)
                return result;
            gpstime = at_ParamUintInRange(pParam->params[1], 0, 3600, &paramRet);
            if (!paramRet)
                return result;
            gpsnmea = (char*)at_ParamStr(pParam->params[2], &paramRet);
            if ((!paramRet)||(strlen(gpsnmea) > 3))
                return result;
            if(1 == gpsflag && gpstime >= 5)
            {
                //set gpsflag
                dtu_file_ctx->gnss.gpsflag = gpsflag;
                dtu_file_ctx->gnss.gpstime = gpstime;
                sprintf(dtu_file_ctx->gnss.gpsnmea, gpsnmea);
                //gnss模块上电
                dtu_gnss_pw_init();
                //上报定时器启动
                dtu_gnss_timer_start();
                if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
                {
                    snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                    result = DSAT_OK;
                }
            }
        }
        dtu_gnss_pw_init();
    }
    else if(pParam->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d,%d,%s", atLine, dtu_file_ctx->gnss.gpsflag, dtu_file_ctx->gnss.gpstime, dtu_file_ctx->gnss.gpsnmea);
        result = DSAT_OK;
    }
    return result;
}
#endif

#ifdef DTU_TYPE_3IN1
/**
  * Function    : dtu_atcmd_setsim
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsim(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine,&dtu_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setsim(&dtu_at_cmd_param,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    return result;
}

/**
  * Function    : dtu_atcmd_setsimlock
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsimlock(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine,&dtu_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setsimlock(&dtu_at_cmd_param,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setsim
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsim(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 sim = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_at_cmd_param->iType)
    {
        if (dtu_at_cmd_param->paramCount == 1)
        {
             sim = at_ParamUintInRange(dtu_at_cmd_param->params[0], 1, 3, &paramRet);
             if (!paramRet)
                return result;
             //set sim
             dtu_file_ctx->sim.sim = sim;
             if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
        }
    }
    else if(dtu_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->sim.sim);
        result = DSAT_OK;
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setsimlock
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsimlock(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 simlock = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_at_cmd_param->iType)
    {
        if (dtu_at_cmd_param->paramCount == 1)
        {
             simlock = at_ParamUintInRange(dtu_at_cmd_param->params[0], 0, 1, &paramRet);
             if (!paramRet)
                return result;
             //set linkfalg
             dtu_file_ctx->sim.simlock = simlock;
             if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
        }
    }
    else if(dtu_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->sim.simlock);
        result = DSAT_OK;
    }
    return result;
}
#endif

#ifdef DTU_TYPE_DODIAI_INCLUDE
/**
  * Function    : dtu_atcmd_dicfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dicfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_dicfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_docfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_docfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_docfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_doflow
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_doflow(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_doflow(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_aicfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_aicfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_aicfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_diwr
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_diwr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
//{
//    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
//    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

//    //判断AT指令是否正确
//    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
//    {
//        //调用对应的AT指令处理函数
//        result = dtu_at_trans_cmd_func_diwr(&dtu_atcmd_param, atLine, resp);
//        //释放AT指令资源
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    else
//    {
//        //释放AT指令资源
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    
//    return result;
//}

/**
  * Function    : dtu_atcmd_dowr
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dowr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_dowr(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_dird
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dird(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_dird(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_dord
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_dord(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_dord(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_aird
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_aird(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_aird(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_clock
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_clock(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_clock(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_flow
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_flow(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
//{
//    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
//    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

//    //判断AT指令是否正确
//    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
//    {
//        //调用对应的AT指令处理函数
//        result = dtu_at_trans_cmd_func_flow(&dtu_atcmd_param, atLine, resp);
//        //释放AT指令资源
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    else
//    {
//        //释放AT指令资源
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    
//    return result;
//}

/**
  * Function    : dtu_at_trans_cmd_func_dicfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dicfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 channel = 0;//通道
    UINT8 type = 0;//主动上报 or 触发上报
    UINT32 interval = 0;//主动上报间隔
    UINT8 edge = 0;//主动上报间隔
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 4)
        {
            //第一个参数，通道，第几路Di
            channel = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            type = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            interval = at_ParamUintInRange(dtu_atcmd_param->params[2], 10, 3600, &paramRet);
            if (!paramRet)
            {
                return result;
            }//第三个参数，调用整形解析函数解析
            edge = at_ParamUintInRange(dtu_atcmd_param->params[3], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }

            //根据通道来判断配置哪一路DI，根据type配置上了还是下拉
            dtu_file_ctx->di.params[channel - 1].type = type;
            dtu_file_ctx->di.params[channel - 1].interval = interval;
            dtu_file_ctx->di.params[channel - 1].edge = edge;
            //触发上报
            //do what?
            //定时上报
            if(DTU_DI_REPORT_INITIATIVE == type)
            {
                //开定时器
                dtu_di_report_timer_start(channel, interval);
            }
            else
            {
                dtu_di_report_timer_stop(channel);
            }

            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DI1CFG:%d,%d,%d\r\n+DI2CFG:%d,%d,%d\r\n+DI3CFG:%d,%d,%d\r\n+DI4CFG:%d,%d,%d\r\n+DI5CFG:%d,%d,%d", 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_1].type, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_1].interval, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_1].edge, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_2].type, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_2].interval, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_2].edge, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_3].type, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_3].interval, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_3].edge, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_4].type, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_4].interval, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_4].edge, 
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_5].type,
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_5].interval,
                                          dtu_file_ctx->di.params[DTU_BUF_INDEX_5].edge);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_docfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_docfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 channel = 0;//通道
    UINT8 type = 0;//主动上报
    UINT32 interval;//主动上报间隔
    UINT8 status = 0;//上电状态
//    UINT32 time = 0;//输出保持时间
//    UINT32 flip = 0;//定时翻转时间
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 4)
        {
            //第一个参数，通道，第几路DO
            channel = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            type = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            interval = at_ParamUintInRange(dtu_atcmd_param->params[2], 10, 3600, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第四个参数，调用整形解析函数解析
            status = at_ParamUintInRange(dtu_atcmd_param->params[3], 0, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
//            //第五个参数，调用整形解析函数解析
//            time = at_ParamUintInRange(dtu_atcmd_param->params[4], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                return result;
//            }
//            //第六个参数，调用整形解析函数解析
//            flip = at_ParamUintInRange(dtu_atcmd_param->params[5], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                return result;
//            }
            uprintf("%d %d %d %d", channel, type, interval, status);

            //根据通道来判断配置哪一路DI，根据type配置上了还是下拉
            dtu_file_ctx->doo.params[channel - 1].type = type;
            dtu_file_ctx->doo.params[channel - 1].interval = interval;
            dtu_file_ctx->doo.params[channel - 1].status = status;
//            dtu_file_ctx->doo.do1.do_time = time;
//            dtu_file_ctx->doo.do1.do_flip = flip;
            dtu_do_init_do(channel, &dtu_file_ctx->doo.params[channel - 1]);
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DO1CFG:%d,%d,%d\r\n" \
                                         "+DO2CFG:%d,%d,%d\r\n" \
                                         "+DO3CFG:%d,%d,%d\r\n" \
                                         "+DO4CFG:%d,%d,%d\r\n" \
                                         "+DO5CFG:%d,%d,%d", 
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_1].type,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_1].interval,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_1].status,
//                                          dtu_file_ctx->doo.do1.do_time,
//                                          dtu_file_ctx->doo.do1.do_flip, 
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_2].type,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_2].interval,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_2].status,
//                                          dtu_file_ctx->doo.do2.do_time,
//                                          dtu_file_ctx->doo.do2.do_flip, 
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_3].type,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_3].interval,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_3].status,
//                                          dtu_file_ctx->doo.do3.do_time,
//                                          dtu_file_ctx->doo.do3.do_flip, 
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_4].type,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_4].interval,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_4].status,
//                                          dtu_file_ctx->doo.do4.do_time,
//                                          dtu_file_ctx->doo.do4.do_flip, 
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_5].type,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_5].interval,
                                          dtu_file_ctx->doo.params[DTU_BUF_INDEX_5].status);
//                                          dtu_file_ctx->doo.do5.do_time,
//                                          dtu_file_ctx->doo.do5.do_flip);
        result = DSAT_OK;
    }
    
    return result;

}

/**
  * Function    : dtu_at_trans_cmd_func_doflow
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_doflow(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 channel = 0;//通道
    UINT8 flow = 0;//DO跟随状态
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 4)
        {
            //第一个参数，通道，第几路DO
            channel = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            flow = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 6, &paramRet);
            if (!paramRet)
            {
                return result;
            }

            //根据通道来判断配置哪一路DO，根据type配置上了还是下拉
            dtu_file_ctx->flow.do_flow[channel - 1] = flow;
            
            uprintf("%d %d", channel, flow);
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DOFLOW:%d,%d,%d,%d,%d", 
                                          dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_1],
                                          dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_2],
                                          dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_3],
                                          dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_4],
                                          dtu_file_ctx->flow.do_flow[DTU_BUF_INDEX_5]);
        result = DSAT_OK;
    }
    
    return result;

}

/**
  * Function    : dtu_at_trans_cmd_func_aicfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_aicfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;//打开关闭AI功能
    UINT8 rule = 0;//上报规则
    UINT8 alarm_high = 0;//AI 高限制
    UINT8 alarm_low = 0;//AI 低限制
    UINT32 interval = 0;//上报间隔
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 5)
        {
            //第一个参数，打开关闭AI功能
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            rule = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            alarm_high = at_ParamUintInRange(dtu_atcmd_param->params[2], 4, 20, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第四个参数，调用整形解析函数解析
            alarm_low = at_ParamUintInRange(dtu_atcmd_param->params[3], 4, 20, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第五个参数，调用整形解析函数解析
            interval = at_ParamUintInRange(dtu_atcmd_param->params[4], 10, 3600, &paramRet);
            if (!paramRet)
            {
                return result;
            }

            dtu_file_ctx->ai.ai_type = type;
            dtu_file_ctx->ai.ai_res_rule = rule;
            dtu_file_ctx->ai.ai_alarm_high = alarm_high;
            dtu_file_ctx->ai.ai_alarm_low = alarm_low;
            dtu_file_ctx->ai.ai_interval = interval;
            if(DTU_AI_REPORT_NONE != type)
            {
                if(interval >= 10)
                {
                    dtu_file_ctx->ai.ai_interval = interval;
                    //打开周期上报定时器
                    dtu_ai1_report_timer_start(interval);
                }
            }
            else
            {
                //停止定时器
                dtu_ai1_report_timer_stop();
            }
            
            //打开定时上报
//            if(DTU_AI_REPORT_INITIATIVE == type)
//            {
//                dtu_ai1_report_timer_start(dtu_file_ctx->ai.ai_interval);
//            }

            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+AICFG:%d,%d,%d,%d,%d", dtu_file_ctx->ai.ai_type, dtu_file_ctx->ai.ai_res_rule, dtu_file_ctx->ai.ai_alarm_high, dtu_file_ctx->ai.ai_alarm_low, dtu_file_ctx->ai.ai_interval);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_diwr
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_diwr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
//{
//    UINT8 *channel = NULL;//通道
//    UINT8 edge = 0;//上拉 or 下拉
//    bool paramRet = TRUE;
//    //init ruturn error
//    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
//    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

//    dtu_file_ctx = dtu_get_file_ctx();
//    if (AT_CMD_SET == dtu_atcmd_param->iType)
//    {
//        if (dtu_atcmd_param->paramCount == 2)
//        {
//            //第一个参数，通道，第几路Di
//            channel = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
//            if (!paramRet)
//            {
//                return result;
//            }
//            //第二个参数，调用整形解析函数解析
//            pull = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                return result;
//            }
//            //根据通道来判断配置哪一路DI，根据type配置上了还是下拉
//            if(strcmp((const char*)channel, "DI1") == 0)
//            {
//                dtu_file_ctx->di.di1.di_edge = edge;
//                dtu_di_int_init1();
//            }
//            if(strcmp((const char*)channel, "DI2") == 0)
//            {
//                dtu_file_ctx->di.di2.di_edge = edge;
//                dtu_di_int_init2();
//            }
//            if(strcmp((const char*)channel, "DI3") == 0)
//            {
//                dtu_file_ctx->di.di3.di_edge = edge;
//                dtu_di_int_init3();
//            }
//            if(strcmp((const char*)channel, "DI4") == 0)
//            {
//                dtu_file_ctx->di.di4.di_edge = edge;
//                dtu_di_int_init4();
//            }
//            if(strcmp((const char*)channel, "DI5") == 0)
//            {
//                dtu_file_ctx->di.di5.di_edge = edge;
//                dtu_di_int_init5();
//            }

//            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
//            {
//                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
//                result = DSAT_OK;
//            }
//        }
//    }
//    else if(dtu_atcmd_param->iType == AT_CMD_READ)
//    {
//        UINT8 di1 = 0;
//        UINT8 di2 = 0;
//        UINT8 di3 = 0;
//        UINT8 di4 = 0;
//        UINT8 di5 = 0;

//        di1 = GpioGetLevel(DTU_GPIO_DI_PIN1);
//        di2 = GpioGetLevel(DTU_GPIO_DI_PIN2);
//        di3 = GpioGetLevel(DTU_GPIO_DI_PIN3);
//        di4 = GpioGetLevel(DTU_GPIO_DI_PIN4);
//        di5 = GpioGetLevel(DTU_GPIO_DI_PIN5);
//        
//        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE,"+DIWR:%d,%d,%d,%d,%d", di1, di2, di3, di4, di5);
//        result = DSAT_OK;
//    }
//    
//    return result;
//}

/**
  * Function    : dtu_at_trans_cmd_func_dowr
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dowr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 channel = 0;//通道
    UINT8 type = 0;//上拉 or 下拉
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 2)
        {
            //第一个参数，通道，第几路Di
            channel = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            type = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //根据通道来判断配置哪一路DI，根据type配置上了还是下拉
            dtu_do_write_pin(channel, type);
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        UINT8 do1 = 0;
        UINT8 do2 = 0;
        UINT8 do3 = 0;
        UINT8 do4 = 0;
        UINT8 do5 = 0;

        do1 = GpioGetLevel(DTU_GPIO_DO_PIN1);
        do2 = GpioGetLevel(DTU_GPIO_DO_PIN2);
        do3 = GpioGetLevel(DTU_GPIO_DO_PIN3);
        do4 = GpioGetLevel(DTU_GPIO_DO_PIN4);
        do5 = GpioGetLevel(DTU_GPIO_DO_PIN5);
        
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DOWR:%d,%d,%d,%d,%d", do1, do2, do3, do4, do5);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_dird
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dird(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 channel = 0;//通道
    UINT8 di1 = 0;
    UINT8 di2 = 0;
    UINT8 di3 = 0;
    UINT8 di4 = 0;
    UINT8 di5 = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，通道，第几路Di
            channel = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }

            //根据通道来判断配置哪一路DI，根据type配置上了还是下拉
            if(DTU_GPIO_CHNNEL_0 == channel)
            {
                di1 = GpioGetLevel(DTU_GPIO_DI_PIN1);
                di2 = GpioGetLevel(DTU_GPIO_DI_PIN2);
                di3 = GpioGetLevel(DTU_GPIO_DI_PIN3);
                di4 = GpioGetLevel(DTU_GPIO_DI_PIN4);
                di5 = GpioGetLevel(DTU_GPIO_DI_PIN5);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DIRD:%d,%d,%d,%d,%d", di1, di2, di3, di4, di5);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_1 == channel)
            {
                di1 = GpioGetLevel(DTU_GPIO_DI_PIN1);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DIRD:%d", di1);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_2 == channel)
            {
                di2 = GpioGetLevel(DTU_GPIO_DI_PIN2);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DIRD:%d", di2);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_3 == channel)
            {
                di3 = GpioGetLevel(DTU_GPIO_DI_PIN3);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DIRD:%d", di3);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_4 == channel)
            {
                di4 = GpioGetLevel(DTU_GPIO_DI_PIN4);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DIRD:%d", di4);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_5 == channel)
            {
                di5 = GpioGetLevel(DTU_GPIO_DI_PIN5);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DIRD:%d", di5);
                result = DSAT_OK;
            }
        }
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_dord
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_dord(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 channel = 0;//通道
    UINT8 do1 = 0;
    UINT8 do2 = 0;
    UINT8 do3 = 0;
    UINT8 do4 = 0;
    UINT8 do5 = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，通道，第几路Di
            channel = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //根据通道来判断配置哪一路DI，根据type配置上了还是下拉
            if(DTU_GPIO_CHNNEL_0 == channel)
            {
                do1 = GpioGetLevel(DTU_GPIO_DO_PIN1);
                do2 = GpioGetLevel(DTU_GPIO_DO_PIN2);
                do3 = GpioGetLevel(DTU_GPIO_DO_PIN3);
                do4 = GpioGetLevel(DTU_GPIO_DO_PIN4);
                do5 = GpioGetLevel(DTU_GPIO_DO_PIN5);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DORD:%d,%d,%d,%d,%d", do1, do2, do3, do4, do5);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_1 == channel)
            {
                do1 = GpioGetLevel(DTU_GPIO_DO_PIN1);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DORD:%d", do1);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_2 == channel)
            {
                do2 = GpioGetLevel(DTU_GPIO_DO_PIN2);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DORD:%d", do2);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_3 == channel)
            {
                do3 = GpioGetLevel(DTU_GPIO_DO_PIN3);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DORD:%d", do3);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_4 == channel)
            {
                do4 = GpioGetLevel(DTU_GPIO_DO_PIN4);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DORD:%d", do4);
                result = DSAT_OK;
            }
            else if(DTU_GPIO_CHNNEL_5 == channel)
            {
                do5 = GpioGetLevel(DTU_GPIO_DO_PIN5);
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+DORD:%d", do5);
                result = DSAT_OK;
            }
        }
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_aird
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_aird(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 *link = NULL;
    UINT16 ai_v = 0;
    float ai_i = 0.0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    
    if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        ai_v = dtu_ai_read();
        ai_i = (float)ai_v / DTU_AI_RES_VAL;
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+AIRD:%.02f", ai_i);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_flow
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_flow(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
//{
//    UINT8 flow1 = 0;//通道1跟随状态
//    UINT8 flow2 = 0;//通道2跟随状态
//    UINT8 flow3 = 0;//通道3跟随状态
//    UINT8 flow4 = 0;//通道4跟随状态
//    UINT8 flow5 = 0;//通道5跟随状态
//    
//    bool paramRet = TRUE;
//    //init ruturn error
//    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
//    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

//    dtu_file_ctx = dtu_get_file_ctx();
//    if (AT_CMD_SET == dtu_atcmd_param->iType)
//    {
//        if (dtu_atcmd_param->paramCount == 6)
//        {
//            //第一个参数，通道，第几个定时器
//            flow1 = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                dtu_file_ctx->flow.do1_flow = flow1;
//                return result;
//            }
//            //第二个参数，调用整形解析函数解析
//            flow2 = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                dtu_file_ctx->flow.do2_flow = flow2;
//                return result;
//            }
//            //第三个参数，调用整形解析函数解析
//            flow3 = at_ParamUintInRange(dtu_atcmd_param->params[2], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                dtu_file_ctx->flow.do3_flow = flow3;
//                return result;
//            }
//            //第四个参数，调用整形解析函数解析
//            flow4 = at_ParamUintInRange(dtu_atcmd_param->params[3], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                dtu_file_ctx->flow.do4_flow = flow4;
//                return result;
//            }
//            //第五个参数，调用整形解析函数解析
//            flow5 = at_ParamUintInRange(dtu_atcmd_param->params[4], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                dtu_file_ctx->flow.do5_flow = flow5;
//                return result;
//            }

//            uprintf("%d %d %d %d %d\n", flow1, flow2, flow3, flow4, flow5);

//            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
//            {
//                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
//                result = DSAT_OK;
//            }
//        }
//    }
//    else if(dtu_atcmd_param->iType == AT_CMD_READ)
//    {
//        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+FLOW: %d, %d, %d, %d, %d", flow1, flow2, flow3, flow4, flow5);
//        result = DSAT_OK;
//    }
//    
//    return result;
//}

/**
  * Function    : dtu_at_trans_cmd_func_clock
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_clock(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
   UINT8 index = 0;//通道
    UINT8 sw = 0;//1 打开定时器，0 关闭定时器
    UINT8 h;//时
    UINT8 m = 0;//分
    UINT8 s = 0;//秒
//    UINT8 type = 0;//类型
    UINT8 channel = 0;//动作DOx
    UINT8 level = 0;//动作电平 1断开 2闭合
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 6)
        {
            //第一个参数，通道，第几个定时器
            index = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            sw = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            h = at_ParamUintInRange(dtu_atcmd_param->params[2], 0, 24, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第四个参数，调用整形解析函数解析
            m = at_ParamUintInRange(dtu_atcmd_param->params[4], 0, 60, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第五个参数，调用整形解析函数解析
            s = at_ParamUintInRange(dtu_atcmd_param->params[5], 0, 60, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第六个参数，调用整形解析函数解析
//            type = at_ParamUintInRange(dtu_atcmd_param->params[5], 0, 65535, &paramRet);
//            if (!paramRet)
//            {
//                return result;
//            }
            //第七个参数，调用整形解析函数解析
            channel = at_ParamUintInRange(dtu_atcmd_param->params[6], 1, 5, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第八个参数，调用整形解析函数解析
            level = at_ParamUintInRange(dtu_atcmd_param->params[7], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            uprintf("%d %d %d %d %d", h, m, s, channel, level);
            //根据通道来判断配置哪个CLK
            dtu_file_ctx->clk.params[index - 1].sw = sw;
            if(DTU_CLK_ON == sw)
            {
                dtu_file_ctx->clk.params[index - 1].h = h;
                dtu_file_ctx->clk.params[index - 1].m = m;
                dtu_file_ctx->clk.params[index - 1].s = s;
//                dtu_file_ctx->clk.clk1.type = type;
                dtu_file_ctx->clk.params[index - 1].num = channel;
                dtu_file_ctx->clk.params[index - 1].level = level;
                dtu_clk_timer_start(index);
            }
            else if(DTU_CLK_OFF == sw)
            {
                dtu_clk_timer_stop(index);
            }

            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, 
                                         "+CLK1:%d,%d,%d,%d,%d\r\n" \
                                         "+CLK2:%d,%d,%d,%d,%d\r\n" \
                                         "+CLK3:%d,%d,%d,%d,%d\r\n" \
                                         "+CLK4:%d,%d,%d,%d,%d\r\n" \
                                         "+CLK5:%d,%d,%d,%d,%d", 
//                                          dtu_file_ctx->clk.clk1.sw,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].h,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].m,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].s,
//                                          dtu_file_ctx->clk.clk1.type,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].num,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_1].level,
//                                          dtu_file_ctx->clk.clk2.sw,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].h,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].m,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].s,
//                                          dtu_file_ctx->clk.clk2.type,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].num,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_2].level,
//                                          dtu_file_ctx->clk.clk3.sw,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].h,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].m,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].s,
//                                          dtu_file_ctx->clk.clk3.type,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].num,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_3].level,
//                                          dtu_file_ctx->clk.clk4.sw,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].h,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].m,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].s,
//                                          dtu_file_ctx->clk.clk4.type,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].num,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_4].level,
//                                          dtu_file_ctx->clk.clk5.sw,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].h,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].m,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].s,
//                                          dtu_file_ctx->clk.clk5.type,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].num,
                                          dtu_file_ctx->clk.params[DTU_BUF_INDEX_5].level);
        result = DSAT_OK;
    }
    
    return result;
}
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:53:17 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
/**
  * Function    : dtu_atcmd_mbcfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbcfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_mbcfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_mbrescfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbrescfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_mbrescfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_mbadd
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbadd(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_mbadd(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_mbaddwn
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_mbaddwn(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_mbaddwn(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_mbcfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbcfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    UINT8 wait = 0;
    UINT8 interval = 0;
    UINT16 delay = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 4)
        {
            //第一个参数，是否打开modbus
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            wait = at_ParamUintInRange(dtu_atcmd_param->params[1], 1, 60, &paramRet);
            if (!paramRet || wait < 1)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            interval = at_ParamUintInRange(dtu_atcmd_param->params[2], 1, 60, &paramRet);
            if (!paramRet || interval < 1)
            {
                return result;
            }
            //第四个参数，调用整形解析函数解析
            delay = at_ParamUintInRange(dtu_atcmd_param->params[3], 1, 60, &paramRet);
            if (!paramRet || delay < 5)
            {
                return result;
            }
            
            uprintf("%d %d %d %d\n", type, wait, interval, delay);
            
            dtu_file_ctx->modbus.config.type = type;
            dtu_file_ctx->modbus.config.wait = wait;
            dtu_file_ctx->modbus.config.interval = interval;
            dtu_file_ctx->modbus.config.delay = delay;

            if(0 == dtu_file_ctx->modbus.config.type)
            {
                //打开心跳
                dtu_file_ctx->hb.heartflag = 1;
                //打开心跳定时器
                dtu_hb_timer_start();
                //关闭odbus指令定时器
                dtu_modbus_interval_timer_stop();
            }
            else
            {
                //关闭心跳
                dtu_file_ctx->hb.heartflag = 0;
                //关闭心跳定时器
                dtu_hb_timer_stop();
#ifdef DTU_TYPE_HTTP_INCLUDE
                //关闭http
                dtu_file_ctx->http.config.type = 0;
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-12 10:37:11 by: zhaoning */
                
                //开启modbus指令定时器
                dtu_modbus_interval_timer_start();
            }
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, 
                                          "+MBCFG:%d,%d,%d,%d", 
                                          dtu_file_ctx->modbus.config.type,
                                          dtu_file_ctx->modbus.config.wait,
                                          dtu_file_ctx->modbus.config.interval,
                                          dtu_file_ctx->modbus.config.delay);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_mbrescfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbrescfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，回复类型
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            uprintf("%d\n", type);
            
            dtu_file_ctx->modbus.config.res_type = type;
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+MBRESCFG:%d", dtu_file_ctx->modbus.config.res_type);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_mbadd
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbadd(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 id = 0;
    UINT8 active = 0;
    UINT8 slave_addr = 0;
    UINT8 fn = 0;
    UINT16 reg_addr_start = 0;
    UINT16 reg_n_d = 0;
    UINT8* reg_data = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 6)
        {
            //第一个参数，第几个指令
            id = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, DTU_MODBUS_CMD_NUM, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            active = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            slave_addr = at_ParamUintInRange(dtu_atcmd_param->params[2], 1, 247, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第四个参数，调用整形解析函数解析
            fn = at_ParamUintInRange(dtu_atcmd_param->params[3], 1, 255, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第五个参数，调用整形解析函数解析
            reg_addr_start = at_ParamUintInRange(dtu_atcmd_param->params[4], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第六个参数，调用整形解析函数解析
            reg_n_d = at_ParamUintInRange(dtu_atcmd_param->params[5], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            uprintf("%d %d %d %d %d %d", id, active, slave_addr, fn, reg_addr_start, reg_n_d);

            id--;
            dtu_file_ctx->modbus.cmd[id].active = active;
            dtu_file_ctx->modbus.cmd[id].slave_addr = slave_addr;
            dtu_file_ctx->modbus.cmd[id].fn = fn;
            dtu_file_ctx->modbus.cmd[id].reg_addr = reg_addr_start;
            dtu_file_ctx->modbus.cmd[id].reg_n_d = reg_n_d;

            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，第几个指令
            id = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, DTU_MODBUS_CMD_NUM, &paramRet);
            if (!paramRet || id < 1)
            {
                return result;
            }
            id = id - 1;

            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+MBADD:%d,0x%04X,0x%02X,0x%04X,%d", 
                                            dtu_file_ctx->modbus.cmd[id].active,
                                            dtu_file_ctx->modbus.cmd[id].slave_addr,
                                            dtu_file_ctx->modbus.cmd[id].fn,
                                            dtu_file_ctx->modbus.cmd[id].reg_addr,
                                            dtu_file_ctx->modbus.cmd[id].reg_n_d);
            result = DSAT_OK;
        }
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_mbaddwn
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_mbaddwn(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 id = 0;
    UINT8 active = 0;
    UINT8 slave_addr = 0;
    UINT8 fn = 0;
    UINT16 reg_addr_start = 0;
    UINT16 reg_n_d = 0;
    UINT8* reg_data = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 7)
        {
            //第一个参数，第几个指令
            id = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, DTU_MODBUS_CMD_WN_NUM, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数，调用整形解析函数解析
            active = at_ParamUintInRange(dtu_atcmd_param->params[1], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数，调用整形解析函数解析
            slave_addr = at_ParamUintInRange(dtu_atcmd_param->params[2], 1, 247, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第四个参数，调用整形解析函数解析
            fn = at_ParamUintInRange(dtu_atcmd_param->params[3], 1, 255, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第五个参数，调用整形解析函数解析
            reg_addr_start = at_ParamUintInRange(dtu_atcmd_param->params[4], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第六个参数，调用整形解析函数解析
            reg_n_d = at_ParamUintInRange(dtu_atcmd_param->params[5], 0, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第七个参数，调用整形解析函数解析
            reg_data = at_ParamStr(dtu_atcmd_param->params[6], &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            uprintf("%d %d %d %d %d %d %s", id, active, slave_addr, fn, reg_addr_start, reg_n_d, reg_data);

            id--;
            dtu_file_ctx->modbus.cmd_wn[id].active = active;
            dtu_file_ctx->modbus.cmd_wn[id].slave_addr = slave_addr;
            dtu_file_ctx->modbus.cmd_wn[id].fn = fn;
            dtu_file_ctx->modbus.cmd_wn[id].reg_addr = reg_addr_start;
            dtu_file_ctx->modbus.cmd_wn[id].reg_n = reg_n_d;

            //modbus是Big-endian
            utils_ascii_str2hex(ASCIISTR2HEX_MODE_BIG_ENDIAN_16, (char*)&dtu_file_ctx->modbus.cmd_wn[id].reg_data, (char*)reg_data, strlen((char*)reg_data));
//            int i = 0;
//            for(i = 0; i < reg_n_d; i++)
//            {
//                uprintf("reg data[%d]: %X\r\n", i, dtu_file_ctx->modbus.cmd_wn[id].reg_data[i]);
//            }
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，第几个指令
            id = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, DTU_MODBUS_CMD_WN_NUM, &paramRet);
            if (!paramRet || id < 1)
            {
                return result;
            }
            id--;

            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, 
                                            "+MBADDWN:%d,0x%04X,0x%02X,0x%04X,%d,%s", 
                                            dtu_file_ctx->modbus.cmd_wn[id].active,
                                            dtu_file_ctx->modbus.cmd_wn[id].slave_addr,
                                            dtu_file_ctx->modbus.cmd_wn[id].fn,
                                            dtu_file_ctx->modbus.cmd_wn[id].reg_addr,
                                            dtu_file_ctx->modbus.cmd_wn[id].reg_n,
                                            (char*)dtu_file_ctx->modbus.cmd_wn[id].reg_data);
            result = DSAT_OK;
        }
    }
    
    return result;
}
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:35:39 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
/**
  * Function    : dtu_atcmd_httpcfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_httpcfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_httpcfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_httpadd
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_httpadd(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_httpadd(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_httpcfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_httpcfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，类型，是否开启http功能
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 1, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            uprintf("%d\n", type);

            if(0 == type)
            {
                //打开心跳
                dtu_file_ctx->hb.heartflag = 1;
                //打开心跳定时器
                dtu_hb_timer_start();
            }
            else
            {
                //关闭心跳
                dtu_file_ctx->hb.heartflag = 0;
                //关闭心跳定时器
                dtu_hb_timer_stop();
#ifdef DTU_TYPE_MODBUS_INCLUDE
                //关闭modbus
                dtu_file_ctx->modbus.config.type = 0;
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-12 10:37:28 by: zhaoning */
            }
            dtu_file_ctx->http.config.type = type;

            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+HTTPCFG:%d", dtu_file_ctx->http.config.type);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_httpadd
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_httpadd(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 id = 0;
    UINT8 type1 = 0;
    UINT8 type2 = 0;
    UINT8* url1 = NULL;
    UINT8* url2 = NULL;
    UINT8* head11 = NULL;
    UINT8* head12 = NULL;
    UINT8* head13 = NULL;
    UINT8* head21 = NULL;
    UINT8* head22 = NULL;
    UINT8* head23 = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 6)
        {
            //第一个参数，第几个http实例
            id = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            if(1 == id)
            {
                type1 = at_ParamUintInRange(dtu_atcmd_param->params[1], 1, 2, &paramRet);
                if (!paramRet)
                {
                    return result;
                }
                dtu_file_ctx->http.http1.type = type1;
                if(AT_CMDPARAM_EMPTY != dtu_atcmd_param->params[2]->type)
                {
                    //第三个参数，调用整形解析函数解析
                    head11 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[2], &paramRet);
                    if ((!paramRet)||(strlen((const char*)head11) > DTU_TRANS_IP_LEN))
                    {
                        return result;
                    }
                    snprintf(dtu_file_ctx->http.http1.head1, DTU_HTTP_HEAD_MAX_LEN, "%s", head11);
                }
                else
                {
                    memset(dtu_file_ctx->http.http1.head1, 0, DTU_HTTP_HEAD_MAX_LEN);
                }
                if(AT_CMDPARAM_EMPTY != dtu_atcmd_param->params[3]->type)
                {
                    //第四个参数，调用整形解析函数解析
                    head12 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[3], &paramRet);
                    if ((!paramRet)||(strlen((const char*)head12) > DTU_TRANS_IP_LEN))
                    {
                        return result;
                    }
                    snprintf(dtu_file_ctx->http.http1.head2, DTU_HTTP_HEAD_MAX_LEN, "%s", head12);
                }
                else
                {
                    memset(dtu_file_ctx->http.http1.head2, 0, DTU_HTTP_HEAD_MAX_LEN);
                }
                if(AT_CMDPARAM_EMPTY != dtu_atcmd_param->params[4]->type)
                {
                    //第五个参数，调用整形解析函数解析
                    head13 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[4], &paramRet);
                    if ((!paramRet)||(strlen((const char*)head13) > DTU_TRANS_IP_LEN))
                    {
                        return result;
                    }
                    snprintf(dtu_file_ctx->http.http1.head3, DTU_HTTP_HEAD_MAX_LEN, "%s", head13);
                }
                else
                {
                    memset(dtu_file_ctx->http.http1.head3, 0, DTU_HTTP_HEAD_MAX_LEN);
                }
                //第二个参数，调用整形解析函数解析
                url1 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[5], &paramRet);
                if ((!paramRet)||(strlen((const char*)url1) > DTU_TRANS_IP_LEN))
                {
                    return result;
                }
                snprintf(dtu_file_ctx->http.http1.url, DTU_HTTP_URL_MAX_LEN, "%s", url1);
                uprintf("%d\r\n%s\r\n%s\r\n%s\r\n%s", id, url1, head11, head12, head13);
            }
            else if(2 == id)
            {
                type2 = at_ParamUintInRange(dtu_atcmd_param->params[1], 1, 2, &paramRet);
                if (!paramRet)
                {
                    return result;
                }
                dtu_file_ctx->http.http2.type = type2;
                if(AT_CMDPARAM_EMPTY != dtu_atcmd_param->params[2]->type)
                {
                    //第三个参数，调用整形解析函数解析
                    head21 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[2], &paramRet);
                    if ((!paramRet)||(strlen((const char*)head21) > DTU_TRANS_IP_LEN))
                    {
                        return result;
                    }
                    snprintf(dtu_file_ctx->http.http2.head1, DTU_HTTP_HEAD_MAX_LEN, "%s", head21);
                }
                else
                {
                    memset(dtu_file_ctx->http.http2.head1, 0, DTU_HTTP_HEAD_MAX_LEN);
                }
                if(AT_CMDPARAM_EMPTY != dtu_atcmd_param->params[3]->type)
                {
                    //第四个参数，调用整形解析函数解析
                    head22 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[3], &paramRet);
                    if ((!paramRet)||(strlen((const char*)head22) > DTU_TRANS_IP_LEN))
                    {
                        return result;
                    }
                    snprintf(dtu_file_ctx->http.http2.head2, DTU_HTTP_HEAD_MAX_LEN, "%s", head22);
                }
                else
                {
                    memset(dtu_file_ctx->http.http2.head2, 0, DTU_HTTP_HEAD_MAX_LEN);
                }
                if(AT_CMDPARAM_EMPTY != dtu_atcmd_param->params[4]->type)
                {
                    //第五个参数，调用整形解析函数解析
                    head23 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[4], &paramRet);
                    if ((!paramRet)||(strlen((const char*)head23) > DTU_TRANS_IP_LEN))
                    {
                        return result;
                    }
                    snprintf(dtu_file_ctx->http.http2.head3, DTU_HTTP_HEAD_MAX_LEN, "%s", head23);
                }
                else
                {
                    memset(dtu_file_ctx->http.http2.head3, 0, DTU_HTTP_HEAD_MAX_LEN);
                }
                //第二个参数，调用整形解析函数解析
                url2 = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[5], &paramRet);
                if ((!paramRet)||(strlen((const char*)url2) > DTU_TRANS_IP_LEN))
                {
                    return result;
                }
                snprintf(dtu_file_ctx->http.http2.url, DTU_HTTP_URL_MAX_LEN, "%s", url2);
                uprintf("%d\r\n%s\r\n%s\r\n%s\r\n%s", id, url2, head21, head22, head23);
            }
            
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, 
                                          "+HTTP1ADD:%d,%s,%s,%s,%s\r\n" \
                                          "+HTTP2ADD:%d,%s,%s,%s,%s", 
                                          dtu_file_ctx->http.http1.type,
                                          dtu_file_ctx->http.http1.head1,
                                          dtu_file_ctx->http.http1.head2,
                                          dtu_file_ctx->http.http1.head3,
                                          dtu_file_ctx->http.http1.url,
                                          dtu_file_ctx->http.http2.type,
                                          dtu_file_ctx->http.http2.head1,
                                          dtu_file_ctx->http.http2.head2,
                                          dtu_file_ctx->http.http2.head3,
                                          dtu_file_ctx->http.http2.url);
        result = DSAT_OK;
    }
    
    return result;
}
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:10:11 by: zhaoning */

#ifdef DTU_TYPE_FORMAT_INCLUDE
/**
  * Function    : dtu_atcmd_formatcfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_formatcfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_formatcfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_formatcfg
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_formatcfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，类型，是否开启http功能
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }

            //打开格式化功能
            dtu_file_ctx->format.type = type;
            
            uprintf("type:%d", type);
            
            //参数保存文件
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+FMCFG:%d", dtu_file_ctx->format.type);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_formatht
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_formatht(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_formatht(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_formatht
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_formatht(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 *header = NULL;//
    UINT8 *tailer = NULL;//IP地址或者域名
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 2)
        {
            //第一个参数，调用字符串解析函数解析
            header = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
            if ((!paramRet)||(strlen((const char*)header) > DTU_FORMAT_LEN))
            {
                return result;
            }
            //第二个参数，调用字符串解析函数解析
            tailer = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
            if ((!paramRet)||(strlen((const char*)tailer) > DTU_FORMAT_LEN))
            {
                return result;
            }

            //字符转十六进制
            utils_ascii_str2hex(0, (char*)&dtu_file_ctx->format.header, (char*)header, strlen((char*)header));
            utils_ascii_str2hex(0, (char*)&dtu_file_ctx->format.tailer, (char*)tailer, strlen((char*)tailer));
            
            uprintf("header:0x%X tailer:0x%X", dtu_file_ctx->format.header, dtu_file_ctx->format.tailer);
            
            //参数保存文件
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+FMHT:0x%X,0x%X", dtu_file_ctx->format.header, dtu_file_ctx->format.tailer);
        result = DSAT_OK;
    }
    
    return result;
}

#endif /* ifdef DTU_TYPE_FORMAT_INCLUDE.2023-12-7 15:43:38 by: zhaoning */

#ifdef DTU_TYPE_AGNSS_INCLUDE

/**
  * Function    : dtu_atcmd_agnsscfg
  * Description : 配置agnss参数，此功能必须包含gnss功能为前提
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_agnsscfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_agnsscfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_agnsscfg
  * Description : 设置agnss的打开关闭，和获取星历时间
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_agnsscfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    UINT8 val = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数，类型，是否开启agnss功能
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }

            //第二个参数，类型，agnss功能
            val = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 256, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            //打开agnss功能
            dtu_file_ctx->agnss.type = type;
            //配置获取星历时间，单位小时
            dtu_file_ctx->agnss.interval = val;
            
            uprintf("type:%d val:%d", type, val);
            
            //参数保存文件
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+AGNSSCFG:%d,%d", dtu_file_ctx->agnss.type, dtu_file_ctx->agnss.interval);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_agnssparams
  * Description : 配置agnss登录参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_agnssparams(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_agnssparams(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_agnssparams
  * Description : 设置中科微登录用户名 密钥
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_agnssparams(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 *uname = NULL;//
    UINT8 *pwd = NULL;//IP地址或者域名
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 2)
        {
            //第一个参数，调用字符串解析函数解析
            uname = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
            if ((!paramRet)||(strlen((const char*)uname) > DTU_AGNSS_USERNEME_LEN))
            {
                return result;
            }
            //第二个参数，调用字符串解析函数解析
            pwd = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
            if ((!paramRet)||(strlen((const char*)pwd) > DTU_AGNSS_PASSWORD_LEN))
            {
                return result;
            }

            //字符转十六进制
            sprintf(dtu_file_ctx->agnss.uname, uname);
            sprintf(dtu_file_ctx->agnss.pwd, pwd);    
            uprintf("username:%s password:%s", dtu_file_ctx->agnss.uname, dtu_file_ctx->agnss.pwd);
            
            //参数保存文件
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+AGNSSPARAMS:%s,%s", dtu_file_ctx->agnss.uname, dtu_file_ctx->agnss.pwd);
        result = DSAT_OK;
    }
    
    return result;
}

#endif /* ifdef DTU_TYPE_AGNSS_INCLUDE.2024-1-3 10:08:44 by: zhaoning */

#ifdef DTU_TYPE_LORA_INCLUDE

extern DTU_FORM_FILE_PARAM_T st_dtu_form_file_t[256];
/**
  * Function    : dtu_atcmd_lora_gateway_cfg
  * Description : 配置agnss参数，此功能必须包含gnss功能为前提
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_lora_gateway_cfg(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_lora_gateway_cfg(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_lora_gateway_cfg
  * Description : 设置agnss的打开关闭，和获取星历时间
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_lora_gateway_cfg(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    UINT8 wait = 0;
    UINT8 interval = 0;
    UINT16 delay = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 4)
        {
            //第一个参数
            type = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 2, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数
            wait = at_ParamUintInRange(dtu_atcmd_param->params[1], 1, 10, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第三个参数
            interval = at_ParamUintInRange(dtu_atcmd_param->params[2], 1, 10, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第四个参数
            delay = at_ParamUintInRange(dtu_atcmd_param->params[3], 1, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            //lora网关模式，1: 巡检模式 2: 定时模式（支持低功耗）
            dtu_file_ctx->lora.type = type;

            if(DTU_LORA_CALL == type)
            {
                dtu_file_ctx->lora.wait = wait;
                dtu_file_ctx->lora.interval = interval;
                dtu_file_ctx->lora.delay = delay;
                uprintf("type:%d wait:%d interval:%d delay:%d", type, wait, interval, delay);
            }
            else if(DTU_LORA_TIME == type)
            {
                uprintf("type:%d", type);
            }
            
            //参数保存文件
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+LORACFG:%d,%d,%d,%d", dtu_file_ctx->lora.type, dtu_file_ctx->lora.wait, dtu_file_ctx->lora.interval, dtu_file_ctx->lora.delay);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_lora_gateway_slave_parsms
  * Description : 配置agnss登录参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_lora_gateway_slave_parsms(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_lora_gateway_slave_parsms(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_lora_gateway_slave_parsms
  * Description : 设置中科微登录用户名 密钥
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_lora_gateway_slave_parsms(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT16 index = 0;//
    UINT8 h = 0;//小时
    UINT8 m = 0;//小时
    UINT8 s = 0;//小时
    UINT8* devid = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 5)
        {
            //第一个参数
            index = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第一个参数，调用字符串解析函数解析
            devid = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
            if ((!paramRet)||(strlen((const char*)devid) > DTU_LORA_ID_MAX_LEN))
            {
                return result;
            }
            //第二个参数
            h = at_ParamUintInRange(dtu_atcmd_param->params[2], 0, 23, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数
            m = at_ParamUintInRange(dtu_atcmd_param->params[3], 1, 60, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //第二个参数
            s = at_ParamUintInRange(dtu_atcmd_param->params[4], 1, 60, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            
            //赋值
            st_dtu_form_file_t[index - 1].day_timestamp = h * 3600 + m * 60 + s;
            sprintf(st_dtu_form_file_t[index - 1].devid, (char*)devid); 
            
            uprintf("index:%d dtime:%ld devid:%s", index, st_dtu_form_file_t[index - 1].day_timestamp, st_dtu_form_file_t[index - 1].devid);
            
            //参数保存文件
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0 && dtu_form_conf_file_write(st_dtu_form_file_t) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
            
        }
        else if(dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数
            index = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+LORASPARAMS:%d,%ld,%s", index, st_dtu_form_file_t[index - 1].day_timestamp, st_dtu_form_file_t[index - 1].devid);
            result = DSAT_OK;
        }
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_lora_gateway_slave_del
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_lora_gateway_slave_del(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_lora_gateway_slave_del(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_lora_gateway_slave_del
  * Description : 删除
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_lora_gateway_slave_del(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT16 index = 0;//

    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if(dtu_atcmd_param->paramCount == 1)
        {
            //第一个参数
            index = at_ParamUintInRange(dtu_atcmd_param->params[0], 1, 65535, &paramRet);
            if (!paramRet)
            {
                return result;
            }
            //删除参数
            memset(st_dtu_form_file_t[index - 1].devid, 0, DTU_LORA_ID_MAX_LEN); 
            
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "+LORADEL:%d", index);
            result = DSAT_OK;
        }
    }
    
    return result;
}

#endif /* ifdef DTU_TYPE_LORA_INCLUDE.2024-1-3 10:08:44 by: zhaoning */

// End of file : am_at.c 2023-8-28 10:35:41 by: zhaoning 

