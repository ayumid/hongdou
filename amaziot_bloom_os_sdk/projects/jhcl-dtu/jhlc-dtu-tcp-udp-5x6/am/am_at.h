//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_at.h
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
#ifndef _AM_AT_H_
#define _AM_AT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdbool.h>

#include "sdk_api.h"

#include "am.h"

// Public defines / typedefs ----------------------------------------------------

#define DTU_CMD_LINE_MAX_LINE_SIZE 512
#define DTU_CMD_LINE_RES_MAX_LINE_SIZE 512

#define DTU_AT_CMD_NAME_MAX_LEN 50 // 16->50
#define DTU_AT_COMMAND_PARAM_MAX 16

#define DTU_TRANS_HEART_LEN 50
#define DTU_TRANS_LINK_LEN 50
#define DTU_TRANS_IP_LEN 60
#define DTU_TRANS_CMDPW_LEN 6

typedef enum 
{
    AT_CMDPARAM_EMPTY,
    AT_CMDPARAM_NUMBER,
    AT_CMDPARAM_STRING,
    AT_CMDPARAM_DSTRING,
    AT_CMDPARAM_RAWTEXT,
    AT_CMDPARAM_STRING_PARSED, // string parameter, and parsed
    AT_CMDPARAM_DTMF_PARSED,   // DTMF parameter, and parsed
    AT_CMDPARAM_HEXDATA_PARSED // hexstr parameter, and parsed
}DTU_AT_CMDPARAM_TYPE_E;

typedef enum 
{
    AT_CMD_SET,
    AT_CMD_TEST,
    AT_CMD_READ,
    AT_CMD_EXE
}DTU_AT_CMD_TYPE_E;

typedef enum
{
    DSAT_CMD_PREP_STATE_HUNT,
    DSAT_CMD_PREP_STATE_FOUND_A,
    DSAT_CMD_PREP_STATE_FOUND_AT,
}DTU_DSAT_CMD_PREP_STATE_TYPE_E;

typedef struct dtu_dsat_sio_info_s
{
    char cmd_line_buffer[DTU_CMD_LINE_MAX_LINE_SIZE];
    char *build_cmd_ptr;
    DTU_DSAT_CMD_PREP_STATE_TYPE_E  at_cmd_prep_state;
}DTU_DSAT_SIO_INFO_TYPE_E;

typedef enum
{
    DSAT_OK             = 0,
    DSAT_NO_CARRIER     = 3,
    DSAT_ERROR          = 4,
}DTU_DSAT_RESULT_TYPE_E;

typedef struct
{
    int istranscmd;
    int result;
    char *response;
}DTU_AT_CMD_RES_T;

typedef struct 
{
    uint8_t type;
    uint16_t length;
    uint8_t value[1];
}DTU_AT_COMMAND_PARAM_T;

typedef struct{
    UINT8 iType;//cmd type
    UINT8 paramCount;//param num
    DTU_AT_COMMAND_PARAM_T *params[DTU_AT_COMMAND_PARAM_MAX];
    
}DTU_AT_CMD_PARA_T;

typedef struct
{
    unsigned char at_cmd_name[DTU_AT_CMD_NAME_MAX_LEN + 1];
    DTU_DSAT_RESULT_TYPE_E (*proc_func)(char *, char *, DTU_AT_CMD_RES_T *);
}DTU_AT_CMD_TABLE_T;

// Public functions prototypes --------------------------------------------------

void dtu_handle_serial_data(DTU_MSG_UART_DATA_PARAM_T *uartData);
void dtu_uart_data_recv_thread(void);
void dtu_uart4_data_send_thread(void *param);

void dtu_uart_data_recv_cbk(UINT8 *data, UINT32 len);
void dtu_uart4_data_recv_cbk(UINT8 *data, UINT32 len);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_AT_H_.2023-8-28 10:35:43 by: zhaoning */

