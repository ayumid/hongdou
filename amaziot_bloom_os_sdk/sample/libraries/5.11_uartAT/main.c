//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "osa.h"
#include "UART.h"
#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "sys.h"
#include "sdk_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define sleep(x) OSATaskSleep((x) * 200)//second


#define DATA_MODE   0
#define AT_MODE     1

#define MAX_LINE_SIZE 1024

#define AT_CMD_NAME_MAX_LEN 64

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
    int result;
    char *response;
} AtCmdResponse;

#define _TASK_STACK_SIZE     1024*8

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

static dsat_sio_info_s_type g_sio_info;

static OSMsgQRef uartMsgQ = NULL;

static void uartdata_thread(void *param);

uint8_t serial_mode;    // 0:DATA_MODE 1:AT_MDOE

// Private functions prototypes -------------------------------------------------

static int dtu_atcmd_regen(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_regtp(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_regdt(char *atName, char *atLine, AtCmdResponse *resp);

typedef struct {
    unsigned char at_cmd_name[AT_CMD_NAME_MAX_LEN + 1];
    int (*proc_func)(char *, char *, AtCmdResponse *);
} ATCmdTable;

static ATCmdTable dtu_atcmd_table[] = {
    { "+REGEN", dtu_atcmd_regen },
    { "+REGTP", dtu_atcmd_regtp },
    { "+REGDT", dtu_atcmd_regdt }
    // add
};

static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;

typedef struct{
    int port;   // 0:uart, 1:uart3, 2:uart4
    int len;
    UINT8 *UArgs;
}uartParam;

// Public functions prototypes --------------------------------------------------

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

void Phase1Inits_enter(void)
{
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    OSA_STATUS status;

    catstudio_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);

    status = OSAMsgQCreate(&uartMsgQ, "uartMsgQ", sizeof(uartParam), 300, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);

    _task_stack = malloc(_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);

    status = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 83, "uartdata_thread", uartdata_thread, NULL);
    ASSERT(status == OS_SUCCESS);        
}

void mainUartRecvCallback(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    catstudio_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.UArgs = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    
    osa_status = OSAMsgQSend(uartMsgQ, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

/* process transparent data function */
static void send_serial_data(char *transData, int data_len)
{
    catstudio_printf("%s[%d]: DATA_MODE recvdata: %s\n", __FUNCTION__, __LINE__, transData);
}


/* This function send data to uart. */
static void send_to_uart(const char *toUart, int length)
{
    char *sendData = NULL;
    catstudio_printf("%s[%d]:toUart=%s, length=%d\n", __FUNCTION__, __LINE__, toUart, length);
    sendData = (char *)malloc(length+1);
    ASSERT(sendData != NULL);
    memset(sendData, 0, length+1);

    memcpy(sendData, toUart, length);
    sendData[length] = '\0';
    
    catstudio_printf("%s[%d]:send to uart data=%s, length=%d\n", __FUNCTION__, __LINE__, sendData, length);    
    //send_data_2uart(gATUARTIntPortNum, (UINT8 *)sendData, length);  //After 202010315 , discard this interface, use UART_SEND_DATA
    UART_SEND_DATA((UINT8 *)sendData, length);
    catstudio_printf("%s[%d]:sendData==NULL = %d\n", __FUNCTION__, __LINE__, (sendData==NULL));
    if (sendData)
        free(sendData);
}

static void check_serial_mode(uartParam *msgUartData)
{
    catstudio_printf("%s[%d]:check_serial_mode=%d\n", __FUNCTION__, __LINE__, serial_mode);
    if (serial_mode == DATA_MODE) {
        if(msgUartData->len == 3 && memcmp(msgUartData->UArgs, "+++", 3) == 0) {
            serial_mode = 0xFF;
            send_to_uart("a", 1);

            // T0D0: Exit 0xff mode after 3 seconds, change to DATA_MODE
        }
    } else if (serial_mode == 0xFF) {
        if(msgUartData->len == 1 && memcmp(msgUartData->UArgs, "a", 1) == 0) {
            serial_mode = AT_MODE;
            send_to_uart("+ok", 3);
        } else {
            serial_mode = DATA_MODE;
            if (msgUartData->len > 0)
                send_serial_data((char *)msgUartData->UArgs, msgUartData->len);
        }
    }
}

static void sendResponse(AtCmdResponse *response)
{
    static const char *MSG_OK="\r\nOK\r\n";
    static const char *MSG_ERROR="\r\nERROR\r\n";
    char resp_buffer[MAX_LINE_SIZE];
    const char *msg = NULL;
    int sz;


    if (response->result == DSAT_OK) {
        if (response->response && response->response[0]) {
            sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
            send_to_uart((char *)resp_buffer, sz);
        }
        msg = MSG_OK;
    } else {
        msg = MSG_ERROR;
    }

    send_to_uart(msg, strlen(msg));

}


static int process_at_cmd_line(char *cmdName, char *cmdLine)
{
    AtCmdResponse *response;
    int i;

    response = malloc(sizeof(AtCmdResponse));
    if ( response != NULL ) {
        memset(response, 0x0, sizeof(AtCmdResponse));
        response->response = malloc(MAX_LINE_SIZE);
        response->response[0] = 0;
        response->result = DSAT_ERROR;

        ATCmdTable *atcmd_ptr = dtu_atcmd_table;
        int nCommands = sizeof(dtu_atcmd_table) / sizeof(dtu_atcmd_table[0]);
        for (i = 0; i < nCommands; i++, atcmd_ptr++) {
            if (!strncasecmp((char *)atcmd_ptr->at_cmd_name, cmdName, strlen(cmdName))) {
                response->result = DSAT_OK;
                if (atcmd_ptr->proc_func != NULL) {
                    response->result = atcmd_ptr->proc_func(cmdName, cmdLine, response);
                }
                break;
            }
        }

        sendResponse(response);

        if (response->response)
            free(response->response);
        free(response);
    }
    return 0;
}


static void process_at_cmd_mode(const char *atCmdData, int data_len)
{
    dsat_sio_info_s_type *sio_info_ptr = &g_sio_info;
    const char *buf_ptr = NULL;
    char cc, atName[MAX_LINE_SIZE] = {0};
    int step = 0;

    buf_ptr = atCmdData;
    catstudio_printf("%s[%d]: AT_MODE recv ATCMD: %s\n", __FUNCTION__, __LINE__, buf_ptr);
    
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
                if (cc == '=') {
                    catstudio_printf("sio_info_ptr->cmd_line_buffer:%s\n", sio_info_ptr->cmd_line_buffer);
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step);
                    atName[step-1] = '\0';
                    catstudio_printf("atName is %s\n", atName);
                }
                *sio_info_ptr->build_cmd_ptr++ = cc;
            } else {
                /*  EOL found, terminate and parse */
                *sio_info_ptr->build_cmd_ptr = '\0';

                if (!strlen(atName)) {
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step-1);
                    atName[step-1] = '\0';
                    catstudio_printf("[%d]: atName is %s\n", __LINE__, atName);
                }

                catstudio_printf("%s[%d]: cmd_line_buffer=%s\n", __FUNCTION__, __LINE__, sio_info_ptr->cmd_line_buffer);

                // T0D0: here should add AT command parse

                process_at_cmd_line(atName, sio_info_ptr->cmd_line_buffer);
                
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;

        default:
            sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            break;
        }
    }
}


static void handle_serial_data(uartParam *uartData)
{
    catstudio_printf("%s[%d]:check_serial_mode=%s\n", __FUNCTION__, __LINE__, uartData->UArgs);
    check_serial_mode(uartData);

    if (serial_mode == DATA_MODE) {
        catstudio_printf("It time, serial_mode is DATA_MODE!\n");
        send_serial_data((char *)uartData->UArgs, uartData->len);
    } else if (serial_mode == AT_MODE) {
        catstudio_printf("It time, serial_mode is AT_MODE!\n");
        process_at_cmd_mode((char *)uartData->UArgs, uartData->len);
    }
}

static void uartdata_thread(void *param)
{
    uartParam uart_temp;
    OSA_STATUS status;

    serial_mode = DATA_MODE;
    
    UART_OPEN(mainUartRecvCallback); 
    
    while (1) {
        memset(&uart_temp, 0x00, sizeof(uartParam));
        
        status = OSAMsgQRecv(uartMsgQ, (UINT8 *)&uart_temp, sizeof(uart_temp), OSA_SUSPEND);
        
        if (status == OS_SUCCESS) {
            if (uart_temp.UArgs) {
                catstudio_printf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                handle_serial_data(&uart_temp);
                
                free(uart_temp.UArgs);
            }
        }
    }
}

static int dtu_atcmd_regen(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    if (atName) {
        catstudio_printf("[atcmd_test]: recv AT command then exec:AT%s\n", atLine);
        snprintf(resp->response, strlen(atLine)+1, "%s", atLine);
        result = DSAT_OK;
    }

    return result;
}
static int dtu_atcmd_regtp(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    if (atName) {
        catstudio_printf("[atcmd_test]: recv AT command then exec:AT%s\n", atLine);
        snprintf(resp->response, strlen(atLine)+1, "%s", atLine);
        result = DSAT_OK;
    }

    return result;
}
static int dtu_atcmd_regdt(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    if (atName) {
        catstudio_printf("[atcmd_test]: recv AT command then exec:AT%s\n", atLine);
        snprintf(resp->response, strlen(atLine)+1, "%s", atLine);
        result = DSAT_OK;
    }

    return result;
}

// End of file : main.c 2023-5-17 14:24:06 by: zhaoning 

