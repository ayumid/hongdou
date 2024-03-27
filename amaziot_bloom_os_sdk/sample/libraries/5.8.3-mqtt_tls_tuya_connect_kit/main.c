//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2024-3-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-3-7
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
#include "teldef.h"
#include "UART.h"
#include "sys.h"
#include "sdk_api.h"
#include "tuya_config.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_mqtt_tls_tuya_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_mqtt_tls_tuya_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define tuya_sleep(x) OSATaskSleep((x)*200) // second
#define TUYA_TASK_STACK_SIZE 1024 * 16

// Private variables ------------------------------------------------------------

static void* _task_stack = NULL;
static OSTaskRef _task_ref = NULL;
static void* _send_task_stack = NULL;
static OSTaskRef _send_task_ref = NULL;

static void tuya_connectkit_task(void *ptr);
static void tuya_send_task(void *ptr);

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

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
    int ret;

    _task_stack = malloc(TUYA_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);

    ret = OSATaskCreate(&_task_ref, _task_stack, TUYA_TASK_STACK_SIZE, 100, "start_kit_task", tuya_connectkit_task, NULL);
    ASSERT(ret == OS_SUCCESS);
    
    _send_task_stack = malloc(TUYA_TASK_STACK_SIZE);
    ASSERT(_send_task_stack != NULL);

    ret = OSATaskCreate(&_send_task_ref, _send_task_stack, TUYA_TASK_STACK_SIZE, 100, "send_task", tuya_send_task, NULL);
    ASSERT(ret == OS_SUCCESS);

}

static void wait_network_ready(void)
{
    int count = 0;
    int ready = 0;

    while (!ready)
    {
        if (getCeregReady(isMasterSim0() ? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
        {
            ready = 1;
        }
        sample_mqtt_tls_tuya_catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();

        OSATaskSleep(200);
    }
}

static void tuya_connectkit_task(void *ptr)
{
    int ret = 0;
    
    wait_network_ready();
    sample_mqtt_tls_tuya_catstudio_printf("\n\n\nSuccess in the net 2\n\n\n");
    
//    cellular_demo();
    switch_demo_non_qrcode();
}

int tuya_get_csq(void)
{
    char atresbuf[16] = {0};
    int err = 0;
    char *p = NULL;
    int csqValue = 99;

    memset(atresbuf, 0, sizeof(atresbuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 2, "+CSQ", 1, NULL, atresbuf, sizeof(atresbuf));

    if(strlen(atresbuf) >= 10 )
    {
        p = strchr(atresbuf,':');
        csqValue = atoi(p + 1);
    }

    return csqValue;
}

void tuya_get_imei(char *buffer)
{
    char imei[24] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(imei, 0, sizeof(imei));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CGSN\r\n", 2, NULL, 1, NULL, imei, sizeof(imei));

    if(strlen(imei) >= 15 )
    {
        snprintf(buffer, 16, imei);
        return;
    }
}

void tuya_get_iccid(char *buffer)
{
    char iccid[36] = {0};
    int err = 0;
    char *p = NULL;
    int errorCnt = 0;//
    wait_reg:
    memset(iccid, 0, sizeof(iccid));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+ICCID\r\n", 2, "+ICCID", 1, NULL, iccid, sizeof(iccid));

    if(strstr(iccid, "+ICCID") != NULL )
    {
        p = strchr(iccid,':');
        snprintf(buffer, 21, p + 2);
        return;
    }
}

static void tuya_send_task(void *ptr)
{
    int csq = 0;
    char imei[16] = {0};
    char iccid[21] = {0};
    char dp_muti_buf[128] = {0};
    char dp_buf[16] = {0};

    //"{\"103\":\"866663060145165\",\"104\":\"898604A41921C0467222\"}"
    tuya_get_imei(imei);
    tuya_get_iccid(iccid);
    snprintf(dp_muti_buf, 128, "{\"103\":\"%s\",\"104\":\"%s\"}", imei, iccid);
    tuya_send_multiple_value_data(dp_muti_buf);

    while(1)
    {
        csq = tuya_get_csq();
        snprintf(dp_buf, 16, "{\"102\":%d}", csq);
        tuya_send_csq_data(dp_buf);
        printf("send csq:%d", csq);
        tuya_sleep(3);
    }
}

// End of file : main.c 2024-3-7 9:03:03 by: zhaoning 

