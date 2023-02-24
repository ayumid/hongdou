/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

  File Name     : am_at_cmds.h
  Version       : V1.0.0
  Author        : dmh
  Created       : 2021/04/09
******************************************************************************/


#ifndef _AM_AT_CMDS_H_
#define _AM_AT_CMDS_H_
#include "incl_config.h"

#ifdef INCL_MOUDULES_MT_MQTT
typedef enum 
{
	DATA_MODE   =0,
	AT_MODE,    
	AT_DATA_MODE 
}SERIAL_MODE;

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )

#define HEX2NUM(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) >= 'A' && (c) <= 'F') ? ((c) - ('A' - 0xa)) : ((c) - ('a' - 0xa)))//test


#define MAX_LINE_SIZE 512 

#define AT_CMD_NAME_MAX_LEN 50 //dmh 16->50

#define AT_COMMAND_PARAM_MAX (16)

typedef enum {
    DSAT_CMD_PREP_STATE_HUNT,
    DSAT_CMD_PREP_STATE_FOUND_A,
    DSAT_CMD_PREP_STATE_FOUND_AT,
} dsat_cmd_prep_state_enum_type;

typedef struct dsat_sio_info_s {
    char cmd_line_buffer[MAX_LINE_SIZE];
    char *build_cmd_ptr;
    dsat_cmd_prep_state_enum_type  at_cmd_prep_state;
} dsat_sio_info_s_type;


typedef enum {
    DSAT_OK             = 0,
    DSAT_NO_CARRIER     = 3,
    DSAT_ERROR          = 4,
} dsat_result_enum_type;


typedef struct {
	int istranscmd;
    int result;
    char *response;
} AtCmdResponse;


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
}AT_CMDPARAM_TYPE;

typedef enum 
{
    AT_CMD_SET,
    AT_CMD_TEST,
    AT_CMD_READ,
    AT_CMD_EXE
}AT_CMD_TYPE;

typedef struct 
{
    uint8_t type;
    uint16_t length;
    uint8_t value[1];
} AT_COMMAND_PARAM_T;

typedef struct{
	UINT8 iType;//cmd type
	UINT8 paramCount;//param num
	AT_COMMAND_PARAM_T *params[AT_COMMAND_PARAM_MAX];
	
}AT_CMD_PARA;

void process_at_cmd_mode(const char *atCmdData, int data_len);
void init_at_uart_conf(void);


int get_RSSI(void);
int get_RSSI_grade(int csq);
void get_IMEI(char *buffer);
void get_IMSI(char *buffer);
void get_ICCID(char *buffer);
#endif /* ifdef INCL_MOUDULES_MT_MQTT.2022-1-25 17:36:55 by: win */



#endif
