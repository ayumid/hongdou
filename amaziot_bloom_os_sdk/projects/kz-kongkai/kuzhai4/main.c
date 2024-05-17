//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : main.c
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

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#include "sdk_api.h"
#include "teldef.h"
#include "websocket.h"
#include "download.h"
#include "cJSON.h"
#include "am_at_cmds.h"
#include "am_file.h"
#include "am_utils.h"
#include "am_gpio.h"
#include "utils_common.h"
#include "am_common.h"
#include "core_http.h"

// Private macros / types / typedef ---------------------------------------------

#define _MAIN_TASK_STACK_SIZE     1024 * 24
#define _SEND_TASK_STACK_SIZE     1024 * 16
#define _CLOCK_TASK_STACK_SIZE    1024 * 8
#define _PLUS_TASK_STACK_SIZE     1024 * 3

#define SEND_MSGQ_TASK_MSGQ_MSG_SIZE              (sizeof(app_msgq_msg))
#define SEND_MSGQ_TASK_MSGQ_QUEUE_SIZE            (200)
#define CLOCK_MSGQ_TASK_MSGQ_MSG_SIZE                (sizeof(app_msgq_msg))
#define CLOCK_MSGQ_TASK_MSGQ_QUEUE_SIZE              (200)

typedef struct
{
    char ip[32];
    int port;
    int hearttime;
    int resendtime;
    char user_apikey[64];
    char romVersion[16];
    char downloadUrl[256];
    char sequence[30];
    int rssitime;
    int lastrssi;
}trans_conf0;

typedef enum 
{
    TCP_INITIAL,
    TCP_LS_CONNECT,
    TCP_LS_GETAREA,
    TCP_ZS_CONNECT,
    TCP_ZS_REGISTER,
    TCP_ZS_NORMAL
    
}TCP_TYPE;

typedef struct AlarmClock 
{
    UINT8 type;
    UINT8 enable;
    CronLine line;
    struct data_time_str atTime;
    UINT8 switchStatus;
    UINT8 switchNum;
} AlarmClock;

// Private variables ------------------------------------------------------------

/***********************************************************************
*    MAIN TASK DEFINE
***********************************************************************/    
//static void* _task_stack = NULL;
static UINT8 _plus0_task_stack[_PLUS_TASK_STACK_SIZE];
static OSTaskRef _plus0_task_ref = NULL;
static UINT8 _plus1_task_stack[_PLUS_TASK_STACK_SIZE];
static OSTaskRef _plus1_task_ref = NULL;
static UINT8 _plus2_task_stack[_PLUS_TASK_STACK_SIZE];
static OSTaskRef _plus2_task_ref = NULL;
static UINT8 _plus3_task_stack[_PLUS_TASK_STACK_SIZE];
static OSTaskRef _plus3_task_ref = NULL;

static UINT8 _task_stack[_MAIN_TASK_STACK_SIZE];
static OSTaskRef _task_ref = NULL;
static UINT8 _send_task_stack[_SEND_TASK_STACK_SIZE];
static OSTaskRef _send_task_ref = NULL;
static UINT8 _clock_task_stack[_CLOCK_TASK_STACK_SIZE];
static OSTaskRef _clock_task_ref = NULL;
/***********************************************************************
*    MSG QUEUE DEFINE
***********************************************************************/
static OSMsgQRef    send_msgq;
static OSMsgQRef    clock_msgq;
/***********************************************************************
*    HEART TASK DEFINE
***********************************************************************/
//static OSFlagRef  HeartFlgRef = NULL;
static OSATimerRef _hearttimer_ref = NULL;
/***********************************************************************
*    RSSI TASK DEFINE
***********************************************************************/
//static OSFlagRef  RssiFlgRef = NULL;
static OSATimerRef _rssitimer_ref = NULL;
/***********************************************************************
*    RESEND TASK DEFINE
***********************************************************************/
//static OSFlagRef ResendFlgRef = NULL;
static OSATimerRef _resendtimer_ref = NULL;
/***********************************************************************
*    CLOCK RELATED DEFINE
***********************************************************************/
static OSATimerRef _timer_ref = NULL;//local clock timer 1s
/***********************************************************************
*    COEN RELATED DEFINE
***********************************************************************/
#ifdef CRON_SUPPORT
static struct data_time_str CalcDataTime;
static const char *const DowAry[] = {
    "sun",
    "mon",
    "tue",
    "wed",
    "thu",
    "fri",
    "sat",

    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    NULL
};

static const char *const MonAry[] = {
    "jan",
    "feb",
    "mar",
    "apr",
    "may",
    "jun",
    "jul",
    "aug",
    "sep",
    "oct",
    "nov",
    "dec",

    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
    NULL
};

static AlarmClock* alarms = NULL;//init no yiyi
static int alarmsCnt = 0;
#endif

static trans_conf0 transconf0;//weihu yi ge 
static char jsonStr[1024]={0};
static TCP_TYPE tcpType;//connect status
static UINT8 registerStatus;//device regster status
static int fd;//websocket handle

// Public variables -------------------------------------------------------------

/***********************************************************************
*    GPIO RELATED DEFINE
***********************************************************************/
//extern OSFlagRef  GpioFlgRef_alarm;//alarm status up flag

//extern OSFlagRef  GpioFlgRef;        //gpio status up flag
extern OSATimerRef _pulsetimer_ref0; //diandong cnt timer
extern OSATimerRef _pulsetimer_ref1; //diandong cnt timer
extern OSATimerRef _pulsetimer_ref2; //diandong cnt timer
extern OSATimerRef _pulsetimer_ref3; //diandong cnt timer

extern OSFlagRef  PluseFlgRef0;        //diandong falg
extern OSFlagRef  PluseFlgRef1;        //diandong falg
extern OSFlagRef  PluseFlgRef2;        //diandong falg
extern OSFlagRef  PluseFlgRef3;        //diandong falg

extern UINT8 MoudleLedStatus;        //moudle led satus
/***********************************************************************
*    SYSTEM RELATED DEFINE
***********************************************************************/
extern BOOL IsAtCmdSrvReady(void);//at cmd ready
/***********************************************************************
*    FLASH RELATED DEFINE
***********************************************************************/
extern trans_conf transconf;//weihu yi ge save in flash
/***********************************************************************
*    AT RELATED DEFINE
***********************************************************************/
extern uint8_t serial_mode;    // 0:DATA_MODE 1:AT_MDOE
extern char at_version[10];   ////define for at query romVersion
/***********************************************************************
*    CLOCK RELATED DEFINE
***********************************************************************/
//extern OSFlagRef  ClockFlgRef;         //chexk timer flag  1min
//extern OSFlagRef  ClockFlgRefUpdateTime;//chexk timer flag  1day

//20230414 zhaoning 按照5.3_http_new修改新的https
/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;
// Private functions prototypes -------------------------------------------------

static void main_task(void * argv);
static void send_task(void * argv);
static void clock_task(void * argv);

static void _hearttimer_callback(UINT32 tmrId);
static void heart_thread(void);
static void _rssitimer_callback(UINT32 tmrId);
static void rssi_thread(void);
static void _resendtimer_callback(UINT32 tmrId);
static void resend_thread(void);
static void _timer_callback(UINT32 tmrId);
static void clock_thread(void);
static void clockupdate_thread(void);
static void _pulsetimer_callback0(UINT32 tmrId);
static void _pulsetimer_callback1(UINT32 tmrId);
static void _pulsetimer_callback2(UINT32 tmrId);
static void _pulsetimer_callback3(UINT32 tmrId);
static void pluse_thread0(void * argv);
static void pluse_thread1(void * argv);
static void pluse_thread2(void * argv);
static void pluse_thread3(void * argv);
static void gpio_thread(void);
static void alarm_thread(void);
static void sw_fotaresultCallback(UINT32 result);
static void fota_start(void);
static uint8_t GetWakeUpTimePoint2(AlarmClock *alarm,struct data_time_str *p_data);
static uint8_t print_time_test(struct data_time_str *p_data);
static void init_trans_conf0(void);

static void wait_dev_active(void);
static void wait_dev_reg_net(void);
static void sw_http_recv_handler(void *handle, const http_recv_t *packet, void *userdata);
static void wait_dev_getDomain(void);
static int wait_dev_connectDomain(void);

// Public functions prototypes --------------------------------------------------

//关机：OnKeyPoweroff_Start()，重启：PM812_SW_RESET();
//extern BOOL isMasterSim0(void);
//extern UINT8 getCeregReady(UINT32 atHandle);

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

//extern void UART_INIT_VOLT(int voltType);
extern void UART_SET_RDY_REPORT_FLAG(int flag);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
//    s_uart_cfg uartCfg = {0};//1178版本sdk可以在这里初始化串口，1191后不可以在这里初始化，初始化转移到Phase2Inits_exit 20230413
    char ver_data[128] = {0};
    // 设置UART初始化电平，只能在Phase1Inits_enter调用
    /*
    0: 1.8v -- default
    1: 2.8V
    2: 3.3V
    */
//    UART_INIT_VOLT(2);
    UART_SET_RDY_REPORT_FLAG(0);
//    UART_OPEN(RecvCallback); 
//    UART_GET_CONFIG(&uartCfg);
//    uartCfg.baudrate = 9600;
//    UART_SET_CONFIG(&uartCfg);

    snprintf(ver_data, 128, "SWVER: [%s] BUILDTIME: [%s %s]", FWVERSION, BUILD_DATE, BUILD_TIME);
    update_the_cp_ver(ver_data);    //可以使用AT+CPVER读取，部分客户需要提供版本给原厂生产，便于原厂识别版本号
}

void Phase1Inits_exit(void)
{

}

void Phase2Inits_enter(void)
{

}

void Phase2Inits_exit(void)
{
    OSA_STATUS status = 0;
    s_uart_cfg uartCfg = {0};
    char arrStr[30] = {0};
    
    UART_OPEN(RecvCallback); 
    UART_GET_CONFIG(&uartCfg);
    uartCfg.baudrate = 9600;
    UART_SET_CONFIG(&uartCfg);

    printf("Phase2Inits_exit");
    
    //init transfile
    trans_conf_file_init();
    
    //init transfile0
    init_trans_conf0();
    
    //init led sattus
    init_led_status();

    //init uart config
    init_at_uart_conf();

    if(transfile_is_null())
    {
        serial_mode = AT_MODE;
        MoudleLedStatus = FACTORY;

        sprintf(arrStr, "%s\r\n", FWVERSION);
        send_to_uart(arrStr, strlen(arrStr));
        send_to_uart("factory mode\r\n", strlen("factory mode\r\n"));
    }
    else
    {
//        printf("normal mode\r\n");
        MoudleLedStatus = NOTLIVE;

        sprintf(arrStr + strlen(arrStr), "%s-V%s\r\n", transconf.device_model, FWVERSION);
        send_to_uart(arrStr, strlen(arrStr));
        send_to_uart("normal mode\r\n", strlen("normal mode\r\n"));

        //init gpio
        init_gpio_conf();
                    
        //begin add by 
        status = OSATimerCreate(&_hearttimer_ref);
        ASSERT(status == OS_SUCCESS);
//        status = OSAFlagCreate(&HeartFlgRef);
//        ASSERT(status == OS_SUCCESS);
//        
        status = OSATimerCreate(&_rssitimer_ref);
        ASSERT(status == OS_SUCCESS);
//        status = OSAFlagCreate(&RssiFlgRef);
//        ASSERT(status == OS_SUCCESS);

        status = OSATimerCreate(&_resendtimer_ref);
        ASSERT(status == OS_SUCCESS);
//        status = OSAFlagCreate(&ResendFlgRef);
//        ASSERT(status == OS_SUCCESS);
        
        status = OSATimerCreate(&_timer_ref);
        ASSERT(status == OS_SUCCESS);
//        status = OSAFlagCreate(&ClockFlgRef);
//        ASSERT(status == OS_SUCCESS);
//        status = OSAFlagCreate(&ClockFlgRefUpdateTime);
//        ASSERT(status == OS_SUCCESS);
//        //status = OSAFlagCreate(&FotaStatusRef);
//        //ASSERT(status == OS_SUCCESS);
        
        status = OSATimerCreate(&_pulsetimer_ref0);
        ASSERT(status == OS_SUCCESS);
        status = OSATimerCreate(&_pulsetimer_ref1);
        ASSERT(status == OS_SUCCESS);
        status = OSATimerCreate(&_pulsetimer_ref2);
        ASSERT(status == OS_SUCCESS);
        status = OSATimerCreate(&_pulsetimer_ref3);
        ASSERT(status == OS_SUCCESS);
        status = OSAFlagCreate(&PluseFlgRef0);
        ASSERT(status == OS_SUCCESS);
        status = OSAFlagCreate(&PluseFlgRef1);
        ASSERT(status == OS_SUCCESS);
        status = OSAFlagCreate(&PluseFlgRef2);
        ASSERT(status == OS_SUCCESS);
        status = OSAFlagCreate(&PluseFlgRef3);
        ASSERT(status == OS_SUCCESS);

        //20230404 zhaoning 
//        extern UINT32 OsaGetDefaultMemPoolFreeSize(void);
//        UINT32 pool_size = 0;
//        pool_size = OsaGetDefaultMemPoolFreeSize();
//        printf("pool size: %d calloc size: %d", pool_size, _MAIN_TASK_STACK_SIZE);//pool size: 405120 calloc size: 65536(这个pool 上电后可能会变化，可能是360000+)

//        _task_stack = calloc(_MAIN_TASK_STACK_SIZE, sizeof(UINT8));
//        _task_stack = malloc(_MAIN_TASK_STACK_SIZE);
//        memset(_task_stack, 0, _MAIN_TASK_STACK_SIZE);
//        if(NULL == _task_stack)
//        {
//            send_to_uart("main task calloc err\r\n", strlen("main task calloc err\r\n"));
//        }

//        sys_thread_new("heart_thread", (lwip_thread_fn)heart_thread, NULL, SYS_THREAD_STACK_SIZE * 8, 161);

//        sys_thread_new("rssi_thread", (lwip_thread_fn)rssi_thread, NULL, SYS_THREAD_STACK_SIZE * 16, 161);

//        sys_thread_new("resend_thread", (lwip_thread_fn)resend_thread, NULL, SYS_THREAD_STACK_SIZE * 8, 161);

//        sys_thread_new("clock_thread", (lwip_thread_fn)clock_thread, NULL, SYS_THREAD_STACK_SIZE * 16, 161);//20210222 100->50

//        sys_thread_new("clockupdate_thread", (lwip_thread_fn)clockupdate_thread, NULL, SYS_THREAD_STACK_SIZE * 8, 161);// for update time

//        sys_thread_new("gpio_thread", (lwip_thread_fn)gpio_thread, NULL, SYS_THREAD_STACK_SIZE * 8, 160);

//        sys_thread_new("pluse_thread0", (lwip_thread_fn)pluse_thread0, NULL, SYS_THREAD_STACK_SIZE * 4, 160);//20210222
//        sys_thread_new("pluse_thread1", (lwip_thread_fn)pluse_thread1, NULL, SYS_THREAD_STACK_SIZE * 4, 160);//20210222
//        sys_thread_new("pluse_thread2", (lwip_thread_fn)pluse_thread2, NULL, SYS_THREAD_STACK_SIZE * 4, 160);//20210222
//        sys_thread_new("pluse_thread3", (lwip_thread_fn)pluse_thread3, NULL, SYS_THREAD_STACK_SIZE * 4, 160);//20210222

//        sys_thread_new("alarm_thread", (lwip_thread_fn)alarm_thread, NULL, SYS_THREAD_STACK_SIZE * 8, 160);

        /*creat message*/
        status = OSAMsgQCreate(&send_msgq, "send_msgq", SEND_MSGQ_TASK_MSGQ_MSG_SIZE, SEND_MSGQ_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
        ASSERT(status == OS_SUCCESS);
        /*creat message*/
        status = OSAMsgQCreate(&clock_msgq, "clock_msgq", CLOCK_MSGQ_TASK_MSGQ_MSG_SIZE, CLOCK_MSGQ_TASK_MSGQ_QUEUE_SIZE, OS_FIFO);
        ASSERT(status == OS_SUCCESS);
        
        status = OSATaskCreate(&_plus0_task_ref, _plus0_task_stack, _PLUS_TASK_STACK_SIZE, 150, "pluse0_task", pluse_thread0, NULL);
        ASSERT(status == OS_SUCCESS);
        status = OSATaskCreate(&_plus1_task_ref, _plus1_task_stack, _PLUS_TASK_STACK_SIZE, 150, "pluse1_task", pluse_thread1, NULL);
        ASSERT(status == OS_SUCCESS);
        status = OSATaskCreate(&_plus2_task_ref, _plus2_task_stack, _PLUS_TASK_STACK_SIZE, 150, "pluse2_task", pluse_thread2, NULL);
        ASSERT(status == OS_SUCCESS);
        status = OSATaskCreate(&_plus3_task_ref, _plus3_task_stack, _PLUS_TASK_STACK_SIZE, 150, "pluse3_task", pluse_thread3, NULL);
        ASSERT(status == OS_SUCCESS);
        
        status = OSATaskCreate(&_task_ref, _task_stack, _MAIN_TASK_STACK_SIZE, 151, "main_task", main_task, NULL);
        ASSERT(status == OS_SUCCESS);
        status = OSATaskCreate(&_send_task_ref, _send_task_stack, _SEND_TASK_STACK_SIZE, 151, "send_task", send_task, NULL);
        ASSERT(status == OS_SUCCESS);
        status = OSATaskCreate(&_clock_task_ref, _clock_task_stack, _CLOCK_TASK_STACK_SIZE, 151, "clock_task", clock_task, NULL);
        ASSERT(status == OS_SUCCESS);
    }
}

/***********************************************************************
*    LOCAL JSON FUNCTIONS
***********************************************************************/
static void package_json_post_data()
{
    //{\"accept\":\"ws\",\"deviceid\":\"10008f721c\",\"nonce\":15949675,\"ts\":0,\"version\":3,\"userAgent\":\"device\",\"apikey\":\"bfbe8d39-c127-4911-84ee-90005fa1c1ae\",\"model\":\"GSB-D04-GL\",\"romVersion\":\"AM430EV1.0\"}"
    cJSON *jsonData = NULL;
    char* out = NULL;
    
    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "accept", cJSON_CreateString("ws"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "nonce", cJSON_CreateNumber(15949675));
    cJSON_AddItemToObject(jsonData, "ts",  cJSON_CreateNumber(5));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf.factory_apikey));
    cJSON_AddItemToObject(jsonData, "model", cJSON_CreateString(transconf.device_model));
    cJSON_AddItemToObject(jsonData, "romVersion", cJSON_CreateString(transconf0.romVersion));
    cJSON_AddItemToObject(jsonData, "version", cJSON_CreateNumber(3));

    out = cJSON_Print(jsonData);

    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
//        printf("jsonData:%s",out + 70);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
    }
    cJSON_Delete(jsonData);
    jsonData = NULL;
} 

static UINT32 parse_json_post_data(char *str)
{
    int ret = -1;
    cJSON * hiDevice = NULL;
    
    hiDevice = cJSON_Parse(str);  
    if(hiDevice != NULL)
    {
        cJSON* pItemErr = NULL;
        cJSON* pItemPort = NULL;
        cJSON* pItemIP = NULL;

        pItemErr = cJSON_GetObjectItem(hiDevice, "error");
        if(pItemErr != NULL)
        {
            switch (pItemErr->valueint)
            {
                case 0:
                    //success
                    ret = 0;
                    pItemPort = cJSON_GetObjectItem(hiDevice, "port");
                    if (pItemPort != NULL)
                    {
                        transconf0.port = pItemPort->valueint;
                    }
                    pItemIP = cJSON_GetObjectItem(hiDevice, "IP");
                    if (pItemIP != NULL)
                    {
                        sprintf(transconf0.ip, pItemIP->valuestring);
                    }
                    break;
                case 400:
                    //resend
                    ret = 400;
                    break;
                case 401:
                    //resend
                    ret = 403;
                    break;
                case 500:
                    //resend
                    ret = 500;
                    break;
                default:
                break;
            }
        }
    }
    else
    {
        printf("parse_json_post_data error");
    }
    if(NULL != hiDevice)
    {
        cJSON_Delete(hiDevice);
        hiDevice = NULL;
    }
    
    return ret;
}

static void package_json_ls_getarea_data()
{
    //{"action":"tmpReg","version":7,"deviceid":"10008f721c","model":"GSB-D04-GL","romVersion":"AM430EV1.0","userAgent":"device","apikey":"bfbe8d39-c127-4911-84ee-90005fa1c1ae","type":0}
    cJSON *jsonData = NULL;
    char* out = NULL;
    
    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("tmpReg"));
    cJSON_AddItemToObject(jsonData, "version", cJSON_CreateNumber(7));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "type", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf.factory_apikey));
    cJSON_AddItemToObject(jsonData, "model", cJSON_CreateString(transconf.device_model));
    cJSON_AddItemToObject(jsonData, "romVersion", cJSON_CreateString(transconf0.romVersion));

    out = cJSON_Print(jsonData);

    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s", out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static UINT32 parse_json_ls_getarea_data(char * str)
{
    int ret = -1;
    cJSON * hiDevice = NULL;

    hiDevice = cJSON_Parse(str);  
    if(hiDevice != NULL)
    {
        cJSON * pItemErr = NULL;
        cJSON * pItemCfg = NULL;
        cJSON * pItemhbI = NULL;
        cJSON * pItemRg = NULL;

        pItemErr = cJSON_GetObjectItem(hiDevice, "error");
        if (pItemErr != NULL)
        {
            //sprintf(transconf.zhengshi,"https://%s-dispd.coolkit.cn/dispatch/device",pItem->valuestring);
            switch (pItemErr->valueint)
            {
                case 0:
                    //heart
                    //{"error":0,"deviceid":"10008f721c","config":{"hb":1,"hbInterval":145}}
                    ret = 0;
                    pItemCfg = cJSON_GetObjectItem(hiDevice, "config");
                    if (pItemCfg != NULL)
                    {
                        pItemhbI = cJSON_GetObjectItem(pItemCfg, "hbInterval");
                        if(pItemhbI != NULL)
                        {
                            transconf0.hearttime = pItemhbI->valueint;
                        }
                    }
                    break;
                case 400:
                    //resend
                    ret = 400;
                    break;
                case 403:
                    //disconnect
                    ret = 403;
                    break;
                case 416:
                    //disconnect goto zhengshi
                    ret = 416;
                    pItemRg = cJSON_GetObjectItem(hiDevice, "region");
                    if (pItemRg != NULL)
                    {
                        if(strcmp("cn", pItemRg->valuestring) == 0)//20211230
                            sprintf(transconf.zhengshi,"https://%s-dispd.coolkit.cn/dispatch/device",pItemRg->valuestring);
                        else
                            sprintf(transconf.zhengshi,"https://%s-dispd.coolkit.cc/dispatch/device",pItemRg->valuestring);
                        trans_conf_file_write(transconf);//save
                    }
                    break;
                case 500:
                    //resend
                    ret = 500;
                    break;
                default:
                break;
            }
        }
        
    }
    else
    {
        printf("parse_json_ls_getarea_data error");
    }
    if(NULL != hiDevice)
    {
        cJSON_Delete(hiDevice);
        hiDevice = NULL;
    }
    return ret;
}

static void package_json_zs_register_data()
{
    //sprintf(buff, "{\"action\":\"tmpReg\",\"version\":7,\"deviceid\":\"10008f721c\",\"model\":\"GSB-D04-GL\",\"romVersion\":\"AM430EV1.0\",\"userAgent\":\"device\",\"apikey\":\"bfbe8d39-c127-4911-84ee-90005fa1c1ae\",\"type\":0}");
    //sprintf(buff, "{\"action\":\"register\",\"version\":7,\"deviceid\":\"10008f721c\",\"romVersion\":\"AM430EV1.0\",\"userAgent\":\"device\",\"apikey\":\"bfbe8d39-c127-4911-84ee-90005fa1c1ae\",\"ts\":\"1595318801\"}");
    cJSON *jsonData = NULL;
    char* out = NULL;
    
    jsonData= cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("register"));
    cJSON_AddItemToObject(jsonData, "version", cJSON_CreateNumber(7));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "ts", cJSON_CreateString("1595318801"));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf.factory_apikey));
    cJSON_AddItemToObject(jsonData, "model", cJSON_CreateString(transconf.device_model));
    cJSON_AddItemToObject(jsonData, "romVersion", cJSON_CreateString(transconf0.romVersion));

    out = cJSON_Print(jsonData);

    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static UINT32 parse_json_zs_register_data(char * str)
{
    int ret = -1;
    int year = 0;
    int mon = 0;
    int mday = 0;
    int mhour = 0;
    int min = 0;
    int sec = 0;
    int msec = 0;
    int i = 0;
    int clocks = 0;
    cJSON *clock = NULL;
    cJSON *type = NULL;
    cJSON *at = NULL;
    cJSON *enabled = NULL;
    cJSON *doItem = NULL;
    cJSON *switchStatus = NULL;
    cJSON *switchNum = NULL;
    cJSON *pulse0 = NULL;
    cJSON *pulse1 = NULL;
    cJSON *pulse2 = NULL;
    cJSON *pulse3 = NULL;
    cJSON *pulse = NULL;
    cJSON *width = NULL;
    
    cJSON *hiDevice = NULL;
    
    hiDevice = cJSON_Parse(str);  
    if(hiDevice != NULL)
    {
        cJSON * pItemErr = NULL;
        cJSON * pItemAk = NULL;
        cJSON * pItemCfg = NULL;
        cJSON * pItemhb = NULL;
        cJSON * pItemdt = NULL;
        cJSON * pItemPm = NULL;
        cJSON * pItemPmtm = NULL;
        cJSON * pItemPl = NULL;
        cJSON * pItemlk = NULL;
        cJSON * pIteCfg2 = NULL;
        cJSON * pIteRi = NULL;

        pItemErr = cJSON_GetObjectItem(hiDevice, "error");
        if(pItemErr != NULL)
        {
            switch (pItemErr->valueint)
            {
                case 0:
                    //no error
                    //{"error":0,"deviceid":"10008f721c","apikey":"61b6291d-4517-487c-be5f-b35c7865b047","config":{"checkInterval":120,"diffValue":10,"hb":1,"hbInterval":145},"datapoints":[]}
                    ret = 0;
                    //get apikey
                    pItemAk = cJSON_GetObjectItem(hiDevice, "apikey");
                    if(pItemAk!=NULL)
                    {
                        sprintf(transconf0.user_apikey,pItemAk->valuestring);
                    }
                    //get heart time
                    pItemCfg = cJSON_GetObjectItem(hiDevice, "config");
                    if (pItemCfg != NULL)
                    {
                        pItemhb = cJSON_GetObjectItem(pItemCfg, "hbInterval");
                        if(pItemhb != NULL)
                        {
                            transconf0.hearttime = pItemhb->valueint;
                        }
                    }
                    //get time data
                    pItemdt = cJSON_GetObjectItem(hiDevice, "date");
                    if(pItemdt != NULL)
                    {
//                        printf("pItem->valuestring:%s",cJSON_Print(pItemdt));
                        //GTM jiexi//2020-07-31T08:50:00.272Z
                        if(sscanf(pItemdt->valuestring,"%d-%d-%dT%d:%d:%d.%dZ",&year,&mon,&mday,&mhour,&min,&sec,&msec)>=6)
                        {
                            printf("jiexiOK!!!year:%d,mon:%d,day:%d,hour:%d,min:%d,sec:%d",year,mon,mday,mhour,min,sec);
                            CalcDataTime.year = year;
                            CalcDataTime.month = mon;
                            CalcDataTime.day = mday;
                            CalcDataTime.hour = mhour;
                            CalcDataTime.min = min;
                            CalcDataTime.sec = sec;
                            //start local timer
                            OSATimerStop(_timer_ref);
//                            OSAFlagSet(ClockFlgRef, 0x01, OSA_FLAG_OR);//check timer 1s //20211129
                            app_msgq_msg clockMsg = {0};

                            clockMsg.msgId = CLOCK_TASK_MSG_CLOCK_MSG;
                            send_task_send_msgq(&clockMsg);
                            OSATimerStart(_timer_ref, 200, 200 , _timer_callback, 0);//1s
                        }
                    }
                    //get params
                    pItemPm = cJSON_GetObjectItem(hiDevice, "params");
                    if(pItemPm != NULL)
                    {
                        //get timers
                        pItemPmtm = cJSON_GetObjectItem(pItemPm, "timers");
                        if(pItemPmtm == NULL)
                        {
                            //"params":0
                            if(alarms != NULL)
                            {
                                free(alarms);
                                alarms = NULL;
                            }
                            alarmsCnt = 0;
                        }
                        if(pItemPmtm != NULL)
                        {
                            clocks = cJSON_GetArraySize(pItemPmtm);
                            printf("clocks:%d",clocks);
                            if(alarms != NULL)
                            {
                                free(alarms);
                                alarms = NULL;
                            }
                            alarmsCnt = clocks;
                            
                            if(clocks > 0)
                            {
                                alarms = (AlarmClock*)calloc(clocks,sizeof(AlarmClock));
                                for(i = 0; i < clocks; i++){
                                    clock = cJSON_GetArrayItem(pItemPmtm, i);
                                    if(clock != NULL)
                                    {
//                                        printf("clock:%s",cJSON_Print(clock));
                                        type = cJSON_GetObjectItem(clock, "type");
                                        if(type != NULL)
                                        {
                                            if(strcmp("once",type->valuestring)==0)
                                                alarms[i].type =0;
                                            else
                                                alarms[i].type =1;
                                        }
                                        at = cJSON_GetObjectItem(clock, "at");
                                        if(at != NULL)
                                        {
                                            printf("at:%s",at->valuestring);
                                            if(alarms[i].type == 1)
                                            {
                                                char *ptr = NULL;
                                                char buf[50] = {0};
                                                sprintf(buf,"%s ",at->valuestring);//add kongge
                                                //at->valuestring ->CronTable
                                                ptr = ParseField("Mins", alarms[i].line.cl_Mins, 60, 0, NULL, buf);
                                                ptr = ParseField("Hrs", alarms[i].line.cl_Hrs, 24, 0, NULL, ptr);
                                                ptr = ParseField("Days", alarms[i].line.cl_Days, 32, 0, NULL, ptr);
                                                ptr = ParseField("Mons", alarms[i].line.cl_Mons, 13, 0, MonAry, ptr);//20211207
                                                ptr = ParseField("Week", alarms[i].line.cl_Dow, 7, 0, DowAry, ptr);
                                                if (ptr == NULL) 
                                                {
                                                    printf("It's over");
                                                }
                                            }
                                            else
                                            {
                                                //GTM jiexi//2020-07-31T08:50:00.272Z
                                                if(sscanf(at->valuestring,"%d-%d-%dT%d:%d:%d.%dZ",&year,&mon,&mday,&mhour,&min,&sec,&msec)>=6)
                                                {
                                                    printf("jiexiOK!!!year:%d,mon:%d,day:%d,hour:%d,min:%d,sec:%d\n",year,mon,mday,mhour,min,sec);
                                                    alarms[i].atTime.year = year;
                                                    alarms[i].atTime.month = mon;
                                                    alarms[i].atTime.day = mday;
                                                    alarms[i].atTime.hour = mhour;
                                                    alarms[i].atTime.min = min;
                                                    alarms[i].atTime.sec = sec;//20211129
                                                }
                                            }
                                        }
                                        enabled = cJSON_GetObjectItem(clock, "enabled");
                                        if(enabled != NULL)
                                        {
                                            alarms[i].enable = enabled->valueint;
                                        }
                                        doItem = cJSON_GetObjectItem(clock, "do");
                                        if(doItem != NULL)
                                        {
                                            switchStatus = cJSON_GetObjectItem(doItem, "switch");
                                            if(switchStatus != NULL)
                                            {
                                                if(strcmp("on",switchStatus->valuestring) == 0)
                                                {
                                                    alarms[i].switchStatus = 1;
                                                }
                                                else
                                                {
                                                    alarms[i].switchStatus = 0;
                                                }
                                            }
                                            switchNum = cJSON_GetObjectItem(doItem, "outlet");
                                            if(switchNum != NULL)
                                            {
                                                alarms[i].switchNum = switchNum->valueint;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        //get pulses
//                        pItem = cJSON_GetObjectItem(hiDevice, "params");
                        pItemPl = cJSON_GetObjectItem(pItemPm, "pulses");
                        if(pItemPl == NULL)
                        {
                            //"params":0
                            transconf.pulsetime0 = 0;
                            transconf.pulsetime1 = 0;
                            transconf.pulsetime2 = 0;
                            transconf.pulsetime3 = 0;
                            trans_conf_file_write(transconf);//save
                        }
                        if(pItemPl != NULL)
                        {
//                            printf("pItem->valuestring:%s",cJSON_Print(pItemPl));
                            pulse0 = cJSON_GetArrayItem(pItemPl, 0);
                            if(pulse0 != NULL)
                            {
//                                printf("pulse0->valuestring:%s",cJSON_Print(pulse0));
                                pulse = cJSON_GetObjectItem(pulse0, "pulse");
                                if(pulse != NULL)
                                {
                                    printf("pulse->valuestring:%s",pulse->valuestring);
                                    if(0 == strcmp(pulse->valuestring,"on"))
                                    {
                                        width = cJSON_GetObjectItem(pulse0, "width");
                                        if(width != NULL)
                                        {
                                            transconf.pulsetime0 = width->valueint;
                                        }
                                    }
                                    else
                                    {
                                        transconf.pulsetime0 = 0;
                                    }
                                }
                            }
                            pulse1 = cJSON_GetArrayItem(pItemPl, 1);
                            if(pulse1 != NULL)
                            {
//                                printf("pulse1->valuestring:%s",cJSON_Print(pulse1));
                                pulse = cJSON_GetObjectItem(pulse1, "pulse");
                                if(pulse != NULL)
                                {
                                    printf("pulse->valuestring:%s",pulse->valuestring);
                                    if(0 == strcmp(pulse->valuestring,"on"))
                                    {
                                        width = cJSON_GetObjectItem(pulse1, "width");
                                        if(width != NULL)
                                        {
                                            transconf.pulsetime1 = width->valueint;
                                        }
                                    }
                                    else
                                    {
                                        transconf.pulsetime1 = 0;
                                    }
                                }
                            }
                            pulse2 = cJSON_GetArrayItem(pItemPl, 2);
                            if(pulse2 != NULL)
                            {
//                                printf("pulse2->valuestring:%s",cJSON_Print(pulse2));
                                pulse = cJSON_GetObjectItem(pulse2, "pulse");
                                if(pulse != NULL)
                                {
                                    printf("pulse->valuestring:%s",pulse->valuestring);
                                    if(0 == strcmp(pulse->valuestring,"on"))
                                    {
                                        width = cJSON_GetObjectItem(pulse2, "width");
                                        if(width != NULL)
                                        {
                                            transconf.pulsetime2 = width->valueint;
                                        }
                                    }
                                    else
                                    {
                                        transconf.pulsetime2 = 0;
                                    }
                                }
                            }
                            pulse3 = cJSON_GetArrayItem(pItemPl, 3);
                            if(pulse3 != NULL)
                            {
//                                printf("pulse3->valuestring:%s",cJSON_Print(pulse3));
                                pulse = cJSON_GetObjectItem(pulse3, "pulse");
                                if(pulse != NULL)
                                {
                                    printf("pulse->valuestring:%s",pulse->valuestring);
                                    if(0 == strcmp(pulse->valuestring,"on"))
                                    {
                                        width = cJSON_GetObjectItem(pulse3, "width");
                                        if(width != NULL)
                                        {
                                            transconf.pulsetime3 = width->valueint;
                                        }
                                    }
                                    else
                                    {
                                        transconf.pulsetime3 = 0;
                                    }
                                }
                            }
                            trans_conf_file_write(transconf);//save
                        }
                        //get lock
//                        pItem = cJSON_GetObjectItem(hiDevice, "params");
                        pItemlk = cJSON_GetObjectItem(pItemPm, "lock");
                        if(pItemlk != NULL)
                        {
//                            printf("pItem->valuestring:%s",cJSON_Print(pItemlk));
                            if(1 == pItemlk->valueint)
                            {
                                //delete timers
                                if(alarms != NULL)
                                {
                                    free(alarms);
                                    alarms = NULL;
                                }
                                alarmsCnt = 0;
                                //delete pulse
                                transconf.pulsetime0 = 0;
                                transconf.pulsetime1 = 0;
                                transconf.pulsetime2 = 0;
                                transconf.pulsetime3 = 0;
                                //delete sdstatus
                                transconf.sdstatus0 = 0;
                                transconf.sdstatus1 = 0;
                                transconf.sdstatus2 = 0;
                                transconf.sdstatus3 = 0;
                                //delete sledstaus
                                transconf.sledstatus = 1;
                            }
                            transconf.lockstatus = pItemlk->valueint;
                            //save sdstatus
                            trans_conf_file_write(transconf);
                        }
                    }
                    break;
                case 400:
                    ret = 400;
                    break;
                case 403:
                    ret = 403;
                    break;
                case 404:
                    ret = 404;
                    //{"error":404,"reason":"Not Found","deviceid":"10008f721c","apikey":"bfbe8d39-c127-4911-84ee-90005fa1c1ae","config":{"regInterval":15},"actionName":"register"}
                    pIteCfg2 = cJSON_GetObjectItem(hiDevice, "config");
                    if (pIteCfg2 != NULL)
                    {
                        pIteRi = cJSON_GetObjectItem(pIteCfg2, "regInterval");
                        if(pIteRi != NULL)
                        {
                            transconf0.resendtime = pIteRi->valueint;
                        }
                    }
                    break;
                case 415:
                    ret = 415;
                    //{"error":415,"deviceid":"10008f721c","apikey":"bfbe8d39-c127-4911-84ee-90005fa1c1ae","config":{"regInterval":15},"actionName":"register"}
                    pIteCfg2 = cJSON_GetObjectItem(hiDevice, "config");
                    if (pIteCfg2 != NULL)
                    {
                        pIteRi = cJSON_GetObjectItem(pIteCfg2, "regInterval");
                        if(pIteRi != NULL)
                        {
                            transconf0.resendtime = pIteRi->valueint;
                        }
                    }
                    break;
                case 500:
                    ret = 500;
                    break;
                default:
                break;
            }
        }
    }
    else
    {
        printf("parse_json_zs_register_data error");
    }
    if(NULL != hiDevice)
    {
        cJSON_Delete(hiDevice);
        hiDevice = NULL;
    }
    return ret;
}

//static UINT32 ack_json_zs_updata_data(char * str)
//{
//    int ret = -1;
//    cJSON* hiDevice = cJSON_Parse(str);  
//    if(hiDevice != NULL)
//    {
//        ret = 0;
//        cJSON_DeleteItemFromObject(hiDevice, "userAgent");
//        cJSON_DeleteItemFromObject(hiDevice, "from");
//        cJSON_DeleteItemFromObject(hiDevice, "ts");
//        cJSON_DeleteItemFromObject(hiDevice, "action");
//        cJSON_DeleteItemFromObject(hiDevice, "params");
//        cJSON_AddItemToObject(hiDevice, "error", cJSON_CreateNumber(0));
//        //printf("dmtest:hiDevice:%s",cJSON_Print(hiDevice));
//        memset(jsonStr,0,sizeof(jsonStr));
//        sprintf(jsonStr, cJSON_Print(hiDevice));
//        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
//    }
//    else
//    {
//        printf("ack_json_zs_updata_data error");
//    }
//    cJSON_Delete(hiDevice);
//    return ret;
//}

static UINT32 parse_json_zs_updata_data(char * str)
{
    int ret = -1;
    uint8_t sendSdStatus = 0;
    
    cJSON *clock = NULL;
    cJSON *type = NULL;
    cJSON *at = NULL;
    cJSON *enabled = NULL;
    cJSON *doItem = NULL;
    cJSON *switchStatus = NULL;
    cJSON *switchNum = NULL;
    int year = 0;
    int mon = 0;
    int mday = 0;
    int mhour = 0;
    int min = 0;
    int sec = 0;
    int msec = 0;
    int i = 0;
    int clocks = 0;
    cJSON *pulse0 = NULL;
    cJSON *pulse1 = NULL;
    cJSON *pulse2 = NULL;
    cJSON *pulse3 = NULL;
    cJSON *pulse = NULL;
    cJSON *width = NULL;
    cJSON *switch0 = NULL;
    cJSON *switchOn = NULL;
    cJSON *outlet = NULL;
    int switchs = 0;
    cJSON *configure0 = NULL;
    cJSON *configure1 = NULL;
    cJSON *configure2 = NULL;
    cJSON *configure3 = NULL;
    cJSON *startup = NULL;
    
    cJSON *hiDevice = NULL;
    
    hiDevice = cJSON_Parse(str);
    if(hiDevice != NULL)
    {
        ret = 0;
        //open or close
        cJSON * pItemPm = NULL;
        cJSON * pItemSw = NULL;
        cJSON * pItemTm = NULL;
        cJSON * pItemPl = NULL;
        cJSON * pItemCfg = NULL;
        cJSON * pItemSo = NULL;
        cJSON * pItemLk = NULL;
        
        pItemPm = cJSON_GetObjectItem(hiDevice, "params");
        if(pItemPm != NULL)
        {
            //get switches
            //printf("pItem->valuestring:%s",cJSON_Print(pItem));
            pItemSw = cJSON_GetObjectItem(pItemPm, "switches");
            if(pItemSw != NULL)
            {
//                printf("pItem->valuestring:%s",cJSON_Print(pItemSw));
                switchs = cJSON_GetArraySize(pItemSw);
                for(i = 0; i < switchs; i++)
                {
                    switch0 = cJSON_GetArrayItem(pItemSw, i);
                    if(switch0 != NULL)
                    {
//                        printf("switch0->valuestring:%s",cJSON_Print(switch0));
                        switchOn = cJSON_GetObjectItem(switch0, "switch");
                        outlet = cJSON_GetObjectItem(switch0, "outlet");
                        if(switchOn != NULL && outlet != NULL)
                        {
                            if(0 == strcmp(switchOn->valuestring,"on"))
                            {
                                switch (outlet->valueint)
                                {
                                    case 0:
                                        gpio_open(GPIO_CLR_PIN0,PluseFlgRef0);
                                        break;
                                    case 1:
                                        gpio_open(GPIO_CLR_PIN1,PluseFlgRef1);
                                        break;
                                    case 2:
                                        gpio_open(GPIO_CLR_PIN2,PluseFlgRef2);
                                        break;
                                    case 3:
                                        gpio_open(GPIO_CLR_PIN3,PluseFlgRef3);
                                        break;
                                    default:
                                    break;
                                }
                            }
                            else
                            {
                                switch (outlet->valueint)
                                {
                                    case 0:
                                        gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                                        break;
                                    case 1:
                                        gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                                        break;
                                    case 2:
                                        gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                                        break;
                                    case 3:
                                        gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                                        break;
                                    default:
                                    break;
                                }
                            }
                        }
                    }
                }
                //save initstatus
                transconf.initstatus0=GpioGetLevel(GPIO_CLR_PIN0);
                transconf.initstatus1=GpioGetLevel(GPIO_CLR_PIN1);
                transconf.initstatus2=GpioGetLevel(GPIO_CLR_PIN2);
                transconf.initstatus3=GpioGetLevel(GPIO_CLR_PIN3);
                trans_conf_file_write(transconf);
            }
            //get timers
//            pItem = cJSON_GetObjectItem(hiDevice, "params");
            pItemTm = cJSON_GetObjectItem(pItemPm, "timers");
            if(pItemTm != NULL)
            {
                clocks = cJSON_GetArraySize(pItemTm);
                printf("clocks:%d",clocks);
                if(alarms != NULL)
                {
                    free(alarms);
                    alarms = NULL;
                }
                alarmsCnt = clocks;
                
                if(clocks > 0)
                {
                    alarms = (AlarmClock*)calloc(clocks,sizeof(AlarmClock));
                    for(i = 0; i < clocks; i++)
                    {
                        clock = cJSON_GetArrayItem(pItemTm, i);
                        if(clock != NULL)
                        {
//                            printf("clock:%s",cJSON_Print(clock));
                            type = cJSON_GetObjectItem(clock, "type");
                            if(type != NULL)
                            {
                                if(strcmp("once",type->valuestring)==0)
                                    alarms[i].type =0;
                                else
                                    alarms[i].type =1;
                            }
                            at = cJSON_GetObjectItem(clock, "at");
                            if(at != NULL)
                            {
                                printf("at:%s",at->valuestring);
                                if(alarms[i].type == 1)
                                {
                                    char *ptr = NULL;
                                    char buf[50]={0};
                                    sprintf(buf,"%s ",at->valuestring);//add kongge
                                    //at->valuestring
                                    ptr = ParseField("Mins", alarms[i].line.cl_Mins, 60, 0, NULL, buf);
                                    ptr = ParseField("Hrs", alarms[i].line.cl_Hrs, 24, 0, NULL, ptr);
                                    ptr = ParseField("Days", alarms[i].line.cl_Days, 32, 0, NULL, ptr);
                                    ptr = ParseField("Mons", alarms[i].line.cl_Mons, 13, 0, MonAry, ptr);//20211207
                                    ptr = ParseField("Week", alarms[i].line.cl_Dow, 7, 0, DowAry, ptr);
                                    if (ptr == NULL) 
                                    {
                                        printf("It's over");
                                    }
                                }
                                else
                                {
                                    //GTM jiexi//2020-07-31T08:50:00.272Z
                                    if(sscanf(at->valuestring,"%d-%d-%dT%d:%d:%d.%dZ",&year,&mon,&mday,&mhour,&min,&sec,&msec)>=6)
                                    {
                                        printf("jiexiOK!!!year:%d,mon:%d,day:%d,hour:%d,min:%d,sec:%d",year,mon,mday,mhour,min,sec);
                                        alarms[i].atTime.year = year;
                                        alarms[i].atTime.month = mon;
                                        alarms[i].atTime.day = mday;
                                        alarms[i].atTime.hour = mhour;
                                        alarms[i].atTime.min = min;
                                        alarms[i].atTime.sec = sec;//20211129
                                    }
                                }
                            }
                            enabled = cJSON_GetObjectItem(clock, "enabled");
                            if(enabled != NULL)
                            {
                                alarms[i].enable = enabled->valueint;
                            }
                            doItem = cJSON_GetObjectItem(clock, "do");
                            if(doItem != NULL)
                            {
                                switchStatus = cJSON_GetObjectItem(doItem, "switch");
                                if(switchStatus != NULL)
                                {
                                    if(strcmp("on",switchStatus->valuestring) == 0)
                                    {
                                        alarms[i].switchStatus = 1;
                                    }
                                    else
                                    {
                                        alarms[i].switchStatus = 0;
                                    }
                                }
                                switchNum = cJSON_GetObjectItem(doItem, "outlet");
                                if(switchNum != NULL)
                                {
                                    alarms[i].switchNum = switchNum->valueint;
                                }
                            }
                        }
                    }
                }
            }
            //get pulses
//            pItem = cJSON_GetObjectItem(hiDevice, "params");
            pItemPl = cJSON_GetObjectItem(pItemPm, "pulses");
            if(pItemPl != NULL)
            {
//                printf("pItem->valuestring:%s",cJSON_Print(pItemPl));
                pulse0 = cJSON_GetArrayItem(pItemPl, 0);
                if(pulse0 != NULL)
                {
//                    printf("pulse0->valuestring:%s",cJSON_Print(pulse0));
                    pulse = cJSON_GetObjectItem(pulse0, "pulse");
                    if(pulse != NULL)
                    {
                        printf("pulse->valuestring:%s",pulse->valuestring);
                        if(0 == strcmp(pulse->valuestring,"on"))
                        {
                            width = cJSON_GetObjectItem(pulse0, "width");
                            if(width != NULL)
                            {
                                transconf.pulsetime0 = width->valueint;
                                transconf.sdstatus0 = 0;
                                sendSdStatus = 1;
                                //close led//20201022
                                gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                            }
                            
                        }
                        else
                        {
                            if(transconf.pulsetime0 > 0)
                                gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                            transconf.pulsetime0 = 0;
                        }
                        
                    }
                }
                pulse1 = cJSON_GetArrayItem(pItemPl, 1);
                if(pulse1 != NULL)
                {
//                    printf("pulse1->valuestring:%s",cJSON_Print(pulse1));
                    pulse = cJSON_GetObjectItem(pulse1, "pulse");
                    if(pulse != NULL)
                    {
                        printf("pulse->valuestring:%s",pulse->valuestring);
                        if(0 == strcmp(pulse->valuestring,"on"))
                        {
                            width = cJSON_GetObjectItem(pulse1, "width");
                            if(width != NULL)
                            {
                                transconf.pulsetime1 = width->valueint;
                                
                                transconf.sdstatus1 = 0;
                                sendSdStatus = 1;
                                //close led//20201022
                                gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                            }
                        }
                        else
                        {
                            if(transconf.pulsetime1 > 0)
                                gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                            transconf.pulsetime1 = 0;
                        }
                    }
                }
                pulse2 = cJSON_GetArrayItem(pItemPl, 2);
                if(pulse2 != NULL)
                {
//                    printf("pulse2->valuestring:%s",cJSON_Print(pulse2));
                    pulse = cJSON_GetObjectItem(pulse2, "pulse");
                    if(pulse != NULL)
                    {
                        printf("pulse->valuestring:%s",pulse->valuestring);
                        if(0 == strcmp(pulse->valuestring,"on"))
                        {
                            width = cJSON_GetObjectItem(pulse2, "width");
                            if(width != NULL)
                            {
                                transconf.pulsetime2 = width->valueint;
                                
                                transconf.sdstatus2 = 0;
                                sendSdStatus = 1;
                                //close led//20201022
                                gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                            }
                            
                        }
                        else
                        {
                            if(transconf.pulsetime2 > 0)
                                gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                            transconf.pulsetime2 = 0;
                        }
                        
                    }
                }
                pulse3 = cJSON_GetArrayItem(pItemPl, 3);
                if(pulse3 != NULL)
                {
//                    printf("pulse3->valuestring:%s",cJSON_Print(pulse3));
                    pulse = cJSON_GetObjectItem(pulse3, "pulse");
                    if(pulse != NULL)
                    {
                        printf("pulse->valuestring:%s",pulse->valuestring);
                        if(0 == strcmp(pulse->valuestring,"on"))
                        {
                            width = cJSON_GetObjectItem(pulse3, "width");
                            if(width != NULL)
                            {
                                transconf.pulsetime3 = width->valueint;
                                
                                transconf.sdstatus3 = 0;
                                sendSdStatus = 1;
                                //close led//20201022
                                gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                            }
                            
                        }
                        else
                        {
                            if(transconf.pulsetime3 > 0)
                                gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                            transconf.pulsetime3 = 0;
                        }
                        
                    }
                }
                //up data
//                OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                app_msgq_msg gpioMsg = {0};

                gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                send_task_send_msgq(&gpioMsg);
                trans_conf_file_write(transconf);//save
            }
            //get configure
//            pItem = cJSON_GetObjectItem(hiDevice, "params");
            pItemCfg = cJSON_GetObjectItem(pItemPm, "configure");
            if(pItemCfg != NULL)
            {
//                printf("pItem->valuestring:%s",cJSON_Print(pItemCfg));
                configure0 = cJSON_GetArrayItem(pItemCfg, 0);
                if(configure0 != NULL)
                {
//                    printf("configure0->valuestring:%s",cJSON_Print(configure0));
                    startup = cJSON_GetObjectItem(configure0, "startup");
                    if(startup != NULL)
                    {
                        printf("startup->valuestring:%s",startup->valuestring);
                        if(0 == strcmp(startup->valuestring,"on"))
                            transconf.sdstatus0 = 1;
                        else if(0 == strcmp(startup->valuestring,"off"))
                            transconf.sdstatus0 = 0;
                        else
                            transconf.sdstatus0 = 2;
                        //judge plusetime //20201015
                        if(transconf.pulsetime0 > 0)
                            transconf.sdstatus0 = 0;
                        
                    }
                }
                configure1 = cJSON_GetArrayItem(pItemCfg, 1);
                if(configure1 != NULL)
                {
//                    printf("configure1->valuestring:%s",cJSON_Print(configure1));
                    startup = cJSON_GetObjectItem(configure1, "startup");
                    if(startup != NULL)
                    {
                        printf("startup->valuestring:%s",startup->valuestring);
                        if(0 == strcmp(startup->valuestring,"on"))
                            transconf.sdstatus1 = 1;
                        else if(0 == strcmp(startup->valuestring,"off"))
                            transconf.sdstatus1 = 0;
                        else
                            transconf.sdstatus1 = 2;
                        //judge plusetime //20201015
                        if(transconf.pulsetime1 > 0)
                            transconf.sdstatus1 = 0;
                    }
                }
                configure2 = cJSON_GetArrayItem(pItemCfg, 2);
                if(configure2 != NULL)
                {
//                    printf("configure2->valuestring:%s",cJSON_Print(configure2));
                    startup = cJSON_GetObjectItem(configure2, "startup");
                    if(startup != NULL)
                    {
                        printf("startup->valuestring:%s",startup->valuestring);
                        if(0 == strcmp(startup->valuestring,"on"))
                            transconf.sdstatus2 = 1;
                        else if(0 == strcmp(startup->valuestring,"off"))
                            transconf.sdstatus2 = 0;
                        else
                            transconf.sdstatus2 = 2;
                        //judge plusetime //20201015
                        if(transconf.pulsetime2 > 0)
                            transconf.sdstatus2 = 0;
                    }
                }
                configure3 = cJSON_GetArrayItem(pItemCfg, 3);
                if(configure3 != NULL)
                {
//                    printf("configure3->valuestring:%s",cJSON_Print(configure3));
                    startup = cJSON_GetObjectItem(configure3, "startup");
                    if(startup != NULL)
                    {
                        printf("startup->valuestring:%s",startup->valuestring);
                        if(0 == strcmp(startup->valuestring,"on"))
                            transconf.sdstatus3 = 1;
                        else if(0 == strcmp(startup->valuestring,"off"))
                            transconf.sdstatus3 = 0;
                        else
                            transconf.sdstatus3 = 2;
                        //judge plusetime //20201015
                        if(transconf.pulsetime3 > 0)
                            transconf.sdstatus3 = 0;
                    }
                }
                //save sdstatus
                trans_conf_file_write(transconf);
            }
            //get sledOnline
//            pItem = cJSON_GetObjectItem(hiDevice, "params");
            pItemSo = cJSON_GetObjectItem(pItemPm, "sledOnline");
            if(pItemSo != NULL)
            {
//                printf("pItem->valuestring:%s",cJSON_Print(pItemSo));
                if(0 == strcmp(pItemSo->valuestring,"on"))
                    transconf.sledstatus = 1;
                else
                    transconf.sledstatus = 0;
                //save sdstatus
                trans_conf_file_write(transconf);
            }
            //get lock
//            pItem = cJSON_GetObjectItem(hiDevice, "params");
            pItemLk = cJSON_GetObjectItem(pItemPm, "lock");
            if(pItemLk != NULL)
            {
//                printf("pItem->valuestring:%s",cJSON_Print(pItemLk));
                if(1 == pItemLk->valueint)
                {
                    //delete timers
                    if(alarms != NULL)
                    {
                        free(alarms);
                        alarms = NULL;
                    }
                    alarmsCnt = 0;
                    //delete pulse
                    transconf.pulsetime0 = 0;
                    transconf.pulsetime1 = 0;
                    transconf.pulsetime2 = 0;
                    transconf.pulsetime3 = 0;
                    //delete sdstatus
                    transconf.sdstatus0 = 0;
                    transconf.sdstatus1 = 0;
                    transconf.sdstatus2 = 0;
                    transconf.sdstatus3 = 0;
                    //delete sledstaus
                    transconf.sledstatus = 1;
                }
                //close all gpio
                gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
//                OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                app_msgq_msg gpioMsg = {0};

                gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                send_task_send_msgq(&gpioMsg);
                transconf.lockstatus = pItemLk->valueint;
                //save sdstatus
                trans_conf_file_write(transconf);
            }
        }
        cJSON_DeleteItemFromObject(hiDevice, "userAgent");
        cJSON_DeleteItemFromObject(hiDevice, "from");
        cJSON_DeleteItemFromObject(hiDevice, "ts");
        cJSON_DeleteItemFromObject(hiDevice, "action");
        cJSON_DeleteItemFromObject(hiDevice, "params");
        cJSON_AddItemToObject(hiDevice, "error", cJSON_CreateNumber(0));

        if(sendSdStatus)
        {
            printf("sendSdStatus is Ok");
            //add sdstatus up
            cJSON *params = NULL;
            
            cJSON *configure = NULL;
            cJSON *sd1 = NULL,*sd2 = NULL,*sd3 = NULL,*sd4 = NULL;

            //////////////pack configure////////////////////////////////////////////
            sd1 = cJSON_CreateObject();
            sd2 = cJSON_CreateObject();
            sd3 = cJSON_CreateObject();
            sd4 = cJSON_CreateObject();
            cJSON_AddItemToObject(sd1, "outlet", cJSON_CreateNumber(0));
            //judge sdstatus0
            if(transconf.sdstatus0 == 0)
                cJSON_AddItemToObject(sd1, "startup", cJSON_CreateString("off"));
            else if(transconf.sdstatus0 == 1)
                cJSON_AddItemToObject(sd1, "startup", cJSON_CreateString("on"));
            else
                cJSON_AddItemToObject(sd1, "startup", cJSON_CreateString("stay"));
            cJSON_AddItemToObject(sd2, "outlet", cJSON_CreateNumber(1));
            //judge sdstatus1
            if(transconf.sdstatus1 == 0)
                cJSON_AddItemToObject(sd2, "startup", cJSON_CreateString("off"));
            else if(transconf.sdstatus1 == 1)
                cJSON_AddItemToObject(sd2, "startup", cJSON_CreateString("on"));
            else
                cJSON_AddItemToObject(sd2, "startup", cJSON_CreateString("stay"));
            cJSON_AddItemToObject(sd3, "outlet", cJSON_CreateNumber(2));
            //judge sdstatus2
            if(transconf.sdstatus2 == 0)
                cJSON_AddItemToObject(sd3, "startup", cJSON_CreateString("off"));
            else if(transconf.sdstatus2 == 1)
                cJSON_AddItemToObject(sd3, "startup", cJSON_CreateString("on"));
            else
                cJSON_AddItemToObject(sd3, "startup", cJSON_CreateString("stay"));
            cJSON_AddItemToObject(sd4, "outlet", cJSON_CreateNumber(3));
            //judge sdstatus3
            if(transconf.sdstatus3 == 0)
                cJSON_AddItemToObject(sd4, "startup", cJSON_CreateString("off"));
            else if(transconf.sdstatus3 == 1)
                cJSON_AddItemToObject(sd4, "startup", cJSON_CreateString("on"));
            else
                cJSON_AddItemToObject(sd4, "startup", cJSON_CreateString("stay"));
            configure = cJSON_CreateArray();
            cJSON_AddItemToArray(configure, sd1);
            cJSON_AddItemToArray(configure, sd2);
            cJSON_AddItemToArray(configure, sd3);
            cJSON_AddItemToArray(configure, sd4);
            //////////////pack configure////////////////////////////////////////////
        
            //////////////packe params//////////////////////////////////////////////
            params = cJSON_CreateObject();
            cJSON_AddItemToObject(params, "configure", configure);    
            //////////////packe params//////////////////////////////////////////////
            cJSON_AddItemToObject(hiDevice, "params", params);
        }

        char* out = NULL;

        out = cJSON_Print(hiDevice);
        
        memset(jsonStr,0,sizeof(jsonStr));
        if(NULL != out)
        {
            printf("jsonData:%s",out);
            sprintf(jsonStr, out);
            cJSON_free(out);
            out = NULL;
            webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
        }
    }
    else
    {
        printf("parse_json_zs_updata_data error");
    }
    if(NULL != hiDevice)
    {
        cJSON_Delete(hiDevice);
    }
    
    return ret;
}

static void up_json_zs_updata_data()
{
    //{"action":"update","deviceid":"10008f721c","apikey":"61b6291d-4517-487c-be5f-b35c7865b047","userAgent":"device","params":{"switches":[{"outlet":0,"switch":"off"},{"outlet":1,"switch":"off"},{"outlet":2,"switch":"off"},{"outlet":3,"switch":"off"}]}}
    cJSON *jsonData = NULL;
    cJSON *params = NULL;
    cJSON *switches = NULL;
    cJSON *switch1 = NULL;
    cJSON *switch2 = NULL;
    cJSON *switch3 = NULL;
    cJSON *switch4 = NULL;
    
    cJSON *configure = NULL;
    cJSON *sd1 = NULL;
    cJSON *sd2 = NULL;
    cJSON *sd3 = NULL;
    cJSON *sd4 = NULL;

    int csq = 0;
    int rssi = 0;
    char rssiStr[10] = {0};
    char imeiBuf[20] = {0};
    char imsiBuf[20] = {0};
    char iccidBuf[30] = {0};
    char* out = NULL;

    ///////////////pack switches/////////////////////////////////////////////
    switch1 = cJSON_CreateObject();
    switch2 = cJSON_CreateObject();
    switch3 = cJSON_CreateObject();
    switch4 = cJSON_CreateObject();
    if(NULL == switch1 || NULL == switch2 || NULL == switch3 || NULL == switch4)
    {
        return;
    }
    cJSON_AddItemToObject(switch1, "outlet", cJSON_CreateNumber(0));
    //judge gpio0 status
    if(GpioGetLevel(GPIO_CLR_PIN0) == 0)
        cJSON_AddItemToObject(switch1, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch1, "switch", cJSON_CreateString("on"));
    cJSON_AddItemToObject(switch2, "outlet", cJSON_CreateNumber(1));
    //judge gpio1 status
    if(GpioGetLevel(GPIO_CLR_PIN1) == 0)
        cJSON_AddItemToObject(switch2, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch2, "switch", cJSON_CreateString("on"));
    cJSON_AddItemToObject(switch3, "outlet", cJSON_CreateNumber(2));
    //judge gpio1 status
    if(GpioGetLevel(GPIO_CLR_PIN2) == 0)
        cJSON_AddItemToObject(switch3, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch3, "switch", cJSON_CreateString("on"));
    cJSON_AddItemToObject(switch4, "outlet", cJSON_CreateNumber(3));
    //judge gpio1 status
    if(GpioGetLevel(GPIO_CLR_PIN3) == 0)
        cJSON_AddItemToObject(switch4, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch4, "switch", cJSON_CreateString("on"));

    switches = cJSON_CreateArray();
    if(NULL == switches)
    {
        return;
    }
    cJSON_AddItemToArray(switches, switch1);
    cJSON_AddItemToArray(switches, switch2);
    cJSON_AddItemToArray(switches, switch3);
    cJSON_AddItemToArray(switches, switch4);
    ///////////////pack switches/////////////////////////////////////////////

    //////////////pack configure////////////////////////////////////////////
    sd1 = cJSON_CreateObject();
    sd2 = cJSON_CreateObject();
    sd3 = cJSON_CreateObject();
    sd4 = cJSON_CreateObject();
    if(NULL == sd1 || NULL == sd2 || NULL == sd3 || NULL == sd4)
    {
        return;
    }
    cJSON_AddItemToObject(sd1, "outlet", cJSON_CreateNumber(0));
    //judge sdstatus0
    if(transconf.sdstatus0 == 0)
        cJSON_AddItemToObject(sd1, "startup", cJSON_CreateString("off"));
    else if(transconf.sdstatus0 == 1)
        cJSON_AddItemToObject(sd1, "startup", cJSON_CreateString("on"));
    else
        cJSON_AddItemToObject(sd1, "startup", cJSON_CreateString("stay"));
    cJSON_AddItemToObject(sd2, "outlet", cJSON_CreateNumber(1));
    //judge sdstatus1
    if(transconf.sdstatus1 == 0)
        cJSON_AddItemToObject(sd2, "startup", cJSON_CreateString("off"));
    else if(transconf.sdstatus1 == 1)
        cJSON_AddItemToObject(sd2, "startup", cJSON_CreateString("on"));
    else
        cJSON_AddItemToObject(sd2, "startup", cJSON_CreateString("stay"));
    cJSON_AddItemToObject(sd3, "outlet", cJSON_CreateNumber(2));
    //judge sdstatus2
    if(transconf.sdstatus2 == 0)
        cJSON_AddItemToObject(sd3, "startup", cJSON_CreateString("off"));
    else if(transconf.sdstatus2 == 1)
        cJSON_AddItemToObject(sd3, "startup", cJSON_CreateString("on"));
    else
        cJSON_AddItemToObject(sd3, "startup", cJSON_CreateString("stay"));
    cJSON_AddItemToObject(sd4, "outlet", cJSON_CreateNumber(3));
    //judge sdstatus3
    if(transconf.sdstatus3 == 0)
        cJSON_AddItemToObject(sd4, "startup", cJSON_CreateString("off"));
    else if(transconf.sdstatus3 == 1)
        cJSON_AddItemToObject(sd4, "startup", cJSON_CreateString("on"));
    else
        cJSON_AddItemToObject(sd4, "startup", cJSON_CreateString("stay"));
        
    configure = cJSON_CreateArray();
    if(NULL == configure)
    {
        return;
    }
    cJSON_AddItemToArray(configure, sd1);
    cJSON_AddItemToArray(configure, sd2);
    cJSON_AddItemToArray(configure, sd3);
    cJSON_AddItemToArray(configure, sd4);
    //////////////pack configure////////////////////////////////////////////

    //////////////packe params//////////////////////////////////////////////
    params = cJSON_CreateObject();
    if(NULL == params)
    {
        return;
    }
    cJSON_AddItemToObject(params, "switches", switches);
    cJSON_AddItemToObject(params, "configure", configure);
    //////////////packe sledOnline//////////////////////////////////////////////
    if(transconf.sledstatus == 1)
        cJSON_AddItemToObject(params, "sledOnline", cJSON_CreateString("on"));
    else
        cJSON_AddItemToObject(params, "sledOnline", cJSON_CreateString("off"));
    //////////////packe sledOnline//////////////////////////////////////////////
    cJSON_AddItemToObject(params, "fwVersion", cJSON_CreateString(transconf0.romVersion));
    
    //get RSSI
    csq = get_RSSI();
    printf("csq:%d",csq);
    if(csq == 99)
        csq = 0;
    rssi = csq * 2 - 113;
    transconf0.lastrssi = rssi;
    sprintf(rssiStr,"%d",rssi);
    printf("rssiStr:%s",rssiStr);
    //get IMEI
    get_IMEI(imeiBuf);
    printf("imeiBuf:%s",imeiBuf);
    //get IMSI
    get_IMSI(imsiBuf);
    printf("imsiBuf:%s",imsiBuf);
    //get ICCID
    get_ICCID(iccidBuf);
    printf("iccidBuf:%s",iccidBuf);

    cJSON_AddItemToObject(params, "rssi", cJSON_CreateString(rssiStr));
    cJSON_AddItemToObject(params, "imei", cJSON_CreateString(imeiBuf));
    cJSON_AddItemToObject(params, "imsi", cJSON_CreateString(imsiBuf));
    cJSON_AddItemToObject(params, "iccid", cJSON_CreateString(iccidBuf));
    //////////////packe params//////////////////////////////////////////////
    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("update"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));
    cJSON_AddItemToObject(jsonData, "params", params);

    out = cJSON_Print(jsonData);

    memset(jsonStr, 0, sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static void up_json_zs_updata_switches()
{
    int ret = 0;
    char* out = NULL;
    //{"action":"update","deviceid":"10008f721c","apikey":"61b6291d-4517-487c-be5f-b35c7865b047","userAgent":"device","params":{"switches":[{"outlet":0,"switch":"off"},{"outlet":1,"switch":"off"},{"outlet":2,"switch":"off"},{"outlet":3,"switch":"off"}]}}
    cJSON *jsonData = NULL;
    cJSON *params = NULL;
    cJSON *switches = NULL;
    cJSON *switch1 = NULL;
    cJSON *switch2 = NULL;
    cJSON *switch3 = NULL;
    cJSON *switch4 = NULL;
    
    ///////////////pack switches/////////////////////////////////////////////
    switch1 = cJSON_CreateObject();
    switch2 = cJSON_CreateObject();
    switch3 = cJSON_CreateObject();
    switch4 = cJSON_CreateObject();
    if(NULL == switch1 || NULL == switch2 || NULL == switch3 || NULL == switch4)
    {
        return;
    }
    cJSON_AddItemToObject(switch1, "outlet", cJSON_CreateNumber(0));
    //judge gpio0 status
    if(GpioGetLevel(GPIO_CLR_PIN0) == 0)
        cJSON_AddItemToObject(switch1, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch1, "switch", cJSON_CreateString("on"));
    cJSON_AddItemToObject(switch2, "outlet", cJSON_CreateNumber(1));
    //judge gpio1 status
    if(GpioGetLevel(GPIO_CLR_PIN1) == 0)
        cJSON_AddItemToObject(switch2, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch2, "switch", cJSON_CreateString("on"));
    cJSON_AddItemToObject(switch3, "outlet", cJSON_CreateNumber(2));
    //judge gpio1 status
    if(GpioGetLevel(GPIO_CLR_PIN2) == 0)
        cJSON_AddItemToObject(switch3, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch3, "switch", cJSON_CreateString("on"));
    cJSON_AddItemToObject(switch4, "outlet", cJSON_CreateNumber(3));
    //judge gpio1 status
    if(GpioGetLevel(GPIO_CLR_PIN3) == 0)
        cJSON_AddItemToObject(switch4, "switch", cJSON_CreateString("off"));
    else
        cJSON_AddItemToObject(switch4, "switch", cJSON_CreateString("on"));
    switches = cJSON_CreateArray();
    if(NULL == switches)
    {
        return;
    }
    cJSON_AddItemToArray(switches, switch1);
    cJSON_AddItemToArray(switches, switch2);
    cJSON_AddItemToArray(switches, switch3);
    cJSON_AddItemToArray(switches, switch4);
    ///////////////pack switches/////////////////////////////////////////////

    //////////////packe params//////////////////////////////////////////////
    params = cJSON_CreateObject();
    if(NULL == params)
    {
        return;
    }
    cJSON_AddItemToObject(params, "switches", switches);
    //////////////packe params//////////////////////////////////////////////

    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("update"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));
    cJSON_AddItemToObject(jsonData, "params", params);

    out = cJSON_Print(jsonData);
    
    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        ret = webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
        if(ret <= 0)
        {
            printf("websocket send switches err");
        } 
        else
            printf("websocket send switches ok");
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static void up_json_zs_updata_alarm()
{
    int ret = 0;
    //{"action":"update","deviceid":"10008f721c","apikey":"61b6291d-4517-487c-be5f-b35c7865b047","userAgent":"device","params":{"alarmType":1}}
    cJSON *jsonData = NULL;
    cJSON *params = NULL;
    char* out = NULL;
    
    //////////////packe params//////////////////////////////////////////////
    params = cJSON_CreateObject();
    if(NULL == params)
    {
        return;
    }
    cJSON_AddItemToObject(params, "alarmType", cJSON_CreateNumber(1));
    //////////////packe params//////////////////////////////////////////////

    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("update"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));
    cJSON_AddItemToObject(jsonData, "params", params);

    out = cJSON_Print(jsonData);

    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        ret = webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
        if(ret <= 0)
        {
            printf("websocket send alarm err");
        } 
        else
            printf("websocket send alarm ok");
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static void get_json_zs_time_data()
{
    cJSON *jsonData = NULL;
    char* out = NULL;
    
    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("date"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));
    
    out = cJSON_Print(jsonData);

    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static void get_json_zs_timers_data()
{
    //{"action":"query","deviceid":"10008f721c","apikey":"61b6291d-4517-487c-be5f-b35c7865b047","userAgent":"device","params":["timers"]}
    cJSON *jsonData = NULL;
    cJSON *params = NULL;
    char* out = NULL;
    
    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("query"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));

    params = cJSON_CreateArray();
    cJSON_AddItemToArray(params, cJSON_CreateString("timers"));
    cJSON_AddItemToArray(params, cJSON_CreateString("pulses"));
    cJSON_AddItemToArray(params, cJSON_CreateString("lock"));
    
    cJSON_AddItemToObject(jsonData, "params", params);

    out = cJSON_Print(jsonData);

    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static UINT32 parse_json_zs_notify_data(char * str)
{
    int ret = -1;
    cJSON *hiDevice = NULL;
    
    hiDevice = cJSON_Parse(str);
    if(hiDevice != NULL)
    {
        ret = 0;
        //open or close
        cJSON * pItem = NULL;
        cJSON * pItemPm = NULL;
        cJSON * pItemRg = NULL;
        
        pItem = cJSON_GetObjectItem(hiDevice, "cmd");
        if(pItem != NULL)
        {
//            printf("pItem->valuestring:%s",cJSON_Print(pItem));    
            if(strcmp("deleteNotify",pItem->valuestring) == 0)
            {
                ret = 2;
                //delete timers
                if(alarms != NULL)
                {
                    free(alarms);
                    alarms = NULL;
                }
                alarmsCnt = 0;
                //delete pulse
                transconf.pulsetime0 = 0;
                transconf.pulsetime1 = 0;
                transconf.pulsetime2 = 0;
                transconf.pulsetime3 = 0;
                //delete sdstatus
                transconf.sdstatus0 = 0;
                transconf.sdstatus1 = 0;
                transconf.sdstatus2 = 0;
                transconf.sdstatus3 = 0;
                //delete sledstaus
                transconf.sledstatus = 1;

                //delete lockstatus
                transconf.lockstatus = 0;// 20210601
                trans_conf_file_write(transconf);//save

                //add close 
                gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                
            }
            else if(strcmp("addNotify",pItem->valuestring)==0)
            {
                ret = 3;
                pItemPm = cJSON_GetObjectItem(hiDevice, "params");
                if(pItemPm != NULL)
                {
//                    printf("pItem->valuestring:%s",cJSON_Print(pItem));
                    pItemRg = cJSON_GetObjectItem(pItemPm, "region");
                    if(pItemRg != NULL)
                    {
                        if(strcmp("cn",pItemRg->valuestring)==0)//20211230
                            sprintf(transconf.zhengshi,"https://%s-dispd.coolkit.cn/dispatch/device",pItemRg->valuestring);
                        else
                            sprintf(transconf.zhengshi,"https://%s-dispd.coolkit.cc/dispatch/device",pItemRg->valuestring);
                        trans_conf_file_write(transconf);//save
                    }
                }
            }
        }
        cJSON_DeleteItemFromObject(hiDevice, "userAgent");
        cJSON_DeleteItemFromObject(hiDevice, "cmd");
        cJSON_DeleteItemFromObject(hiDevice, "action");
        cJSON_DeleteItemFromObject(hiDevice, "params");
        
        cJSON_AddItemToObject(hiDevice, "error", cJSON_CreateNumber(0));

        char* out = NULL;

        out = cJSON_Print(hiDevice);
        memset(jsonStr,0,sizeof(jsonStr));
        if(NULL != out)
        {
            printf("jsonData:%s",out);
            sprintf(jsonStr, out);
            cJSON_free(out);
            out = NULL;
            webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
        }
    }
    else
    {
        printf("parse_json_zs_notify_data error");
    }
    if(NULL != hiDevice)
    {
        cJSON_Delete(hiDevice);
    }
    return ret;
}

//static UINT32 ack_json_zs_notify_data(char * str)
//{
//    int ret = -1;
//    
//    cJSON *hiDevice = cJSON_Parse(str);  
//    if(hiDevice != NULL)
//    {
//        ret = 0;
//        cJSON_DeleteItemFromObject(hiDevice, "userAgent");
//        cJSON_DeleteItemFromObject(hiDevice, "cmd");
//        cJSON_DeleteItemFromObject(hiDevice, "action");
//        cJSON_DeleteItemFromObject(hiDevice, "params");
//        
//        cJSON_AddItemToObject(hiDevice, "error", cJSON_CreateNumber(0));
//        //printf("dmtest:hiDevice:%s",cJSON_Print(hiDevice));
//        memset(jsonStr,0,sizeof(jsonStr));
//        sprintf(jsonStr, cJSON_Print(hiDevice));
//        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
//    }
//    else
//    {
//        printf("ack_json_zs_notify_data error");
//    }
//    cJSON_Delete(hiDevice);
//    return ret;
//}

static UINT32 parse_json_zs_upgrade_data(char * str)
{
    int ret = -1;
    //int i;
    cJSON *hiDevice = NULL;
    
    hiDevice = cJSON_Parse(str);  
    if(hiDevice != NULL)
    {
        //open or close
        cJSON * pItem = NULL;
        cJSON * params = NULL;
        cJSON * pItemMd = NULL;
        cJSON * bin = NULL;
        cJSON * arr = NULL;
        cJSON * url = NULL;
        
        pItem = cJSON_GetObjectItem(hiDevice, "sequence");
        if(pItem != NULL)
        {
//            printf("pItem->valuestring:%s",cJSON_Print(pItem));
            sprintf(transconf0.sequence,pItem->valuestring);
        }
        params = cJSON_GetObjectItem(hiDevice, "params");
        if(params != NULL)
        {
//            printf("pItem->valuestring:%s",cJSON_Print(params));
            pItemMd = cJSON_GetObjectItem(params, "model");
            if(pItemMd != NULL)
            {
//                printf("pItem->valuestring:%s",cJSON_Print(pItem));
                if(strcmp(pItemMd->valuestring,transconf.device_model) == 0)
                {
                    bin = cJSON_GetObjectItem(params, "binList");
                    if(bin != NULL)
                    {
                        /*
                        printf("pItem->valuestring:%s",cJSON_Print(pItem));    
                        int bins= cJSON_GetArraySize(pItem);
                        
                        for(i=0;i<bins;i++){
                            bin = cJSON_GetArrayItem(pItem, i);
                            printf("pItem->valuestring:%s",cJSON_Print(bin));
                            url = cJSON_GetObjectItem(bin, "downloadUrl");
                            if(url!=NULL)
                                printf("pItem->valuestring:%s",cJSON_Print(url));
                            name = cJSON_GetObjectItem(bin, "name");
                            if(name!=NULL)
                                printf("pItem->valuestring:%s",cJSON_Print(name));
                            digest = cJSON_GetObjectItem(bin, "digest");
                            if(digest!=NULL)
                                printf("pItem->valuestring:%s",cJSON_Print(digest));
                        }
                        */
                        
                        arr = cJSON_GetArrayItem(bin, 0);
                        if(arr != NULL)
                        {
                            url = cJSON_GetObjectItem(arr, "downloadUrl");
                            if(url != NULL)
                            {
//                                printf("pItem->valuestring:%s",cJSON_Print(url));
                                sprintf(transconf0.downloadUrl,url->valuestring);
                                ret = 0;
                            }
                        }
                    }
                }
                else
                {
                    ret = 1;
                }
            }
        }
    }
    else
    {
        printf("parse_json_zs_upgrade_data error");
    }
    if(NULL != hiDevice)
    {
        cJSON_Delete(hiDevice);
    }
    return ret;
}

static UINT32 ack_json_zs_upgrade_data(int error)
{
    cJSON *jsonData = NULL;
    char* out = NULL;
    
    jsonData = cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return 1;
    }
    cJSON_AddItemToObject(jsonData, "error", cJSON_CreateNumber(error));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));
    cJSON_AddItemToObject(jsonData, "sequence", cJSON_CreateString(transconf0.sequence));
    
    out = cJSON_Print(jsonData);
    
    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
    
    return 0;
}

static void up_json_zs_updata_rssi(int rssi)
{
    //{"action":"update","deviceid":"10008f721c","apikey":"61b6291d-4517-487c-be5f-b35c7865b047","userAgent":"device","params":{"RSSI":20}}
    cJSON *jsonData = NULL;
    cJSON *params = NULL;
    char rssiStr[10] = {0};
    char* out = NULL;
    
    sprintf(rssiStr,"%d",rssi);
    //////////////packe params//////////////////////////////////////////////
    params = cJSON_CreateObject();
    if(NULL == params)
    {
        return;
    }
    cJSON_AddItemToObject(params, "rssi", cJSON_CreateString(rssiStr));
            
    //////////////packe params//////////////////////////////////////////////
    jsonData= cJSON_CreateObject();
    if(NULL == jsonData)
    {
        return;
    }
    cJSON_AddItemToObject(jsonData, "action", cJSON_CreateString("update"));
    cJSON_AddItemToObject(jsonData, "deviceid", cJSON_CreateString(transconf.deviceId));
    cJSON_AddItemToObject(jsonData, "userAgent", cJSON_CreateString("device"));
    cJSON_AddItemToObject(jsonData, "apikey", cJSON_CreateString(transconf0.user_apikey));
    cJSON_AddItemToObject(jsonData, "params", params);

    out = cJSON_Print(jsonData);
    
    memset(jsonStr,0,sizeof(jsonStr));
    if(NULL != out)
    {
        printf("jsonData:%s",out);
        sprintf(jsonStr, out);
        cJSON_free(out);
        out = NULL;
        webSocket_send(fd, jsonStr, strlen(jsonStr), true, WDT_TXTDATA);
    }
    
    cJSON_Delete(jsonData);
    jsonData = NULL;
}

static void wait_dev_active(void)
{
    wait_reg:
    if(transconf.activeFlag == 1)
    {
        trans_conf_file_write(transconf);//save
    }
    else
    {
        msleep(2);
        goto wait_reg;
    }
}

static void wait_dev_reg_net(void)
{
    char mmRspBuf[30] = {0};
    int err = 0;
    int errorCnt = 0;
    
    MoudleLedStatus = NOGPRS;
    //因为出错后有发at+cfun重新驻网问题，所以这里按照之前方式查询是否驻网，使用屏蔽代码方式，注意extern函数 20230619
//    while(!getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1))
//    {
//        printf("wait net ...\n");
//        sleep(3);
//        errorCnt++;
//        if(errorCnt>20)
//        {
//            //20210607
//            errorCnt = 0;
//            //AT+CFUN=0
//            set_CFUN_0();
//            //delay
//            sleep(3);
//            //AT+CFUN=1
//            set_CFUN_1();
//        }
//    }
//    errorCnt = 0;
//    MoudleLedStatus = NOSERVER;

    while(!IsAtCmdSrvReady())
    {
        OSATaskSleep(100);
    }
wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_12, "AT^SYSINFO\r\n", 3, "^SYSINFO", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==> %s", mmRspBuf);

    if(strstr(mmRspBuf, "^SYSINFO: 2,3") != NULL || strstr(mmRspBuf, "^SYSINFO: 2,2") != NULL)
    {
        errorCnt = 0;
        MoudleLedStatus = NOSERVER;
    }
    else
    {
        sleep(3);
        errorCnt++;
        if(errorCnt>20)
        {
            //20210607
            errorCnt = 0;
            //AT+CFUN=0
            set_CFUN_0();
            //delay
            sleep(3);
            //AT+CFUN=1
            set_CFUN_1();
        }
        goto wait_reg;
    } 
}

/* HTTP事件回调函数, 当SDK读取到网络报文时被触发, 报文描述类型 */
static void sw_http_recv_handler(void *handle, const http_recv_t *packet, void *userdata)
{
    char *body = NULL;
    
    switch (packet->type) {
        case HTTPRECV_STATUS_CODE:
            //SDK收到HTTP报文时, 会通过这个用户回调打印HTTP状态码, 如404, 200, 302等
            printf("status code: %d", packet->data.status_code.code);
            
        break;

        case HTTPRECV_HEADER:
            //SDK收到HTTP报文时, 会通过这个用户回调打印HTTP首部, 如Content-Length等
            //printf("key: %s, value: %s", packet->data.header.key, packet->data.header.value);
            
        break;

        case HTTPRECV_BODY:
            //处理云平台的HTTP回应报文
            //printf("len = %d, %s", packet->data.body.len,packet->data.body.buffer);
            //20230414 zhaoning 不清楚为什么packet->data.body.buffer 在len长度后面有乱码，保险起见，这里多拷贝一次
            body = malloc(packet->data.body.len + 1);
            if(NULL != body)
            {
                memset(body, 0, packet->data.body.len + 1);
                memcpy(body, (const char*)packet->data.body.buffer, packet->data.body.len);
                printf("len = %d, body: %s", packet->data.body.len, body);
//            if(NULL != packet->data.body.buffer)
//                parse_json_post_data((char*)packet->data.body.buffer);
                parse_json_post_data((char*)body);
                free(body);
                body = NULL;
            }
        break;

        default:
        
        break;

    }
}

static void wait_dev_getDomain(void)
{
    int32_t res = -1;
    core_http_handle_t *http_handle = NULL;

    core_http_request_t request = {0};
    core_http_response_t response = {0};
    uint64_t timenow_ms = 0;
    aiot_sysdep_network_cred_t cred = {0};
    uint16_t host_port = 443;
    uint16_t connect_timeout = CORE_HTTP_DEFAULT_CONNECT_TIMEOUT_MS*2;
    uint16_t send_timeout = CORE_HTTP_DEFAULT_SEND_TIMEOUT_MS*2;
    uint16_t recv_timeout = CORE_HTTP_DEFAULT_RECV_TIMEOUT_MS*2;
    char ls[32] = {0};
    char zs[32] = {0};
    char path[32] = {0};
    char temp[32] = {0};
    UINT32 errorCnt;
    
    /* 配置SDK的底层依赖 */
    aiot_sysdep_set_portfile(&g_aiot_sysdep_portfile, 1);    
    /* 配置SDK的日志输出 */
    //aiot_state_set_logcb(demo_state_logcb);
PRO_START:
    wait_dev_reg_net();

    http_handle = core_http_init();
    if (http_handle == NULL)
    {
        printf("%s[%d], res: %d", __FUNCTION__, __LINE__, res);
        goto clean;
    }

    if(strlen(transconf.zhengshi)== 0)
    {
        printf("linshi:%s",transconf.linshi);
        //linshi:https://disp.coolkit.cn/dispatch/device
        //默认都是https://
        strncpy(temp, transconf.linshi + strlen("https://"), strlen(transconf.linshi) - strlen("https://"));
        strncpy(ls, temp, strstr(temp, "/") - temp);
        strcpy(path, strstr(temp, "/"));
        printf("ls: %s path: %s", ls, path);
        res = core_http_setopt(http_handle, CORE_HTTPOPT_HOST, ls);
    }
    else
    {
        printf("zhengshi:%s",transconf.zhengshi);
        //linshi:https://cn-disp.coolkit.cn/dispatch/device
        //默认都是https://
        strncpy(temp, transconf.linshi + strlen("https://"), strlen(transconf.linshi) - strlen("https://"));
        strncpy(zs, temp, strstr(temp, "/") - temp);
        strcpy(path, strstr(temp, "/"));
        printf("zs: %s path: %s", zs, path);
        res = core_http_setopt(http_handle, CORE_HTTPOPT_HOST, zs);
    }
    
    res = core_http_setopt(http_handle, CORE_HTTPOPT_PORT, &host_port);
    catstudio_printf("core_http_setopt %d, res: %d", CORE_HTTPOPT_PORT, res);

    package_json_post_data();

    /* 创建SDK的安全凭据, 用于建立TLS连接 */
    memset(&cred, 0, sizeof(aiot_sysdep_network_cred_t));
    cred.ssl_verify_type = MBEDTLS_SSL_VERIFY_NONE;

    res = core_http_setopt(http_handle, CORE_HTTPOPT_NETWORK_CRED, &cred);
    res = core_http_setopt(http_handle, CORE_HTTPOPT_CONNECT_TIMEOUT_MS, &connect_timeout);
    res = core_http_setopt(http_handle, CORE_HTTPOPT_SEND_TIMEOUT_MS, &send_timeout);
    res = core_http_setopt(http_handle, CORE_HTTPOPT_RECV_TIMEOUT_MS, &recv_timeout);
    res = core_http_setopt(http_handle, CORE_HTTPOPT_RECV_HANDLER, sw_http_recv_handler);
    res = core_http_setopt(http_handle, CORE_HTTPOPT_USERDATA, (void *)&response);

    if ((res = core_http_connect(http_handle)) < STATE_SUCCESS) {
        printf("%s[%d], res: %d", __FUNCTION__, __LINE__, res);
        goto clean;
    }

    memset(&request, 0, sizeof(core_http_request_t));

    request.method = "POST";
    request.path = "/dispatch/device";
    request.content = (uint8_t *)jsonStr;
    request.content_len = (uint32_t)strlen(jsonStr);

    res = core_http_send(http_handle, &request);
    printf("%s[%d], res: %d", __FUNCTION__, __LINE__, res);

    timenow_ms = http_handle->sysdep->core_sysdep_time();
    while (1)
    {
        if (timenow_ms >= http_handle->sysdep->core_sysdep_time())
        {
            timenow_ms = http_handle->sysdep->core_sysdep_time();
        }
        if (http_handle->sysdep->core_sysdep_time() - timenow_ms >= http_handle->recv_timeout_ms) {
            break;
        }

        res = core_http_recv(http_handle);
        if (res < STATE_SUCCESS)
        {
            break;
        }
    }

    if (res == STATE_HTTP_READ_BODY_FINISHED)
    {
#if 1
        printf("http finished: %d", res);
        errorCnt = 0;
        goto exit;
#endif /* if 0. 2023-4-21 14:08:58 by: zhaoning */
#if 0
        printf("http finished: %d need test err: %d", res, errorCnt + 1);
#endif /* if 0. 2023-4-21 14:08:45 by: zhaoning */
    }
    else
    {
        //这里会继续往下执行，直到goto PRO_START
        printf("http err: %s", res);
    }
    //出错才会来这里
    errorCnt++; 
    if(errorCnt > 9)
    {
        errorCnt = 0;
    }

clean:
    /* 销毁HTTP实例 */
    if(NULL != http_handle)
        core_http_deinit((void **)&http_handle);

    sleep(pow(2,errorCnt));
    printf("http clean the %d power of 2", errorCnt);

    goto PRO_START;

exit:
    /* 销毁HTTP实例 */
    if(NULL != http_handle)
        core_http_deinit((void **)&http_handle);
    printf("http exit");

}

static int wait_dev_connectDomain(void)
{
    printf("transconf0.ip:%s",transconf0.ip);
    printf("transconf0.port:%d",transconf0.port);
    int errorCnt = 0;
wait_reg:
    if(((fd = webSocket_clientLinkToServer(transconf0.ip, transconf0.port, "api/ws", 1)) <= 0))
    {
        printf("client link to server failed !fd:%d",fd);
        errorCnt++;
        if(errorCnt > 3)
        {
            return -1;
        }
        sleep(pow(2,errorCnt));
        goto wait_reg;
    }
    
    printf("fd:%d",fd);
    printf("websocket connect success!");
    return 0;
}

static void wait_connect_server(void)
{
    if(fd > 0)
        websocket_close(fd);//20210222
wait_reg:
    wait_dev_getDomain();
    if(wait_dev_connectDomain() < 0)
    {
        goto wait_reg;
    }
}

/*function for checknet and init socket*/
static void main_task(void * argv)
{
    int ret = 0;
    int error = 0;
    //8个延时同时下发，websocket打印：dataLen:1719,packageLen:1715,headLen:4,dpRet:-19
    //8个定时器同时下发，websocket打印：dataLen:1486,packageLen:1482,headLen:4,dpRet:-19
    //所以暂时定位2*1024(dataLen可能有变化，未深入测试)
    char buf[2*1024] = {0};
    
    wait_dev_active();
PRO_START:
    OSATimerStop(_resendtimer_ref);
    OSATimerStop(_hearttimer_ref);
    OSATimerStop(_rssitimer_ref);
    tcpType = TCP_INITIAL;

    wait_connect_server();
    //Open LinkA
    MoudleLedStatus = UNREGISTERED;
            
    printf("%s-%d",__FUNCTION__,__LINE__);

    //linshi Server
    if(strlen(transconf.zhengshi) == 0)
    {
        tcpType = TCP_LS_CONNECT;
        package_json_ls_getarea_data();
    }
    else
    {
        //zhengshi Server
        tcpType = TCP_ZS_CONNECT;
        package_json_zs_register_data();
        registerStatus = 0;
    }

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = webSocket_recv(fd, buf, sizeof(buf) - 1, NULL);
        printf("ssl_read buf: %s ret:%d fd:%d",buf, ret, fd);
        
        if (ret <= 0)
        {
            if(fd > 0)
                websocket_close(fd);
            goto PRO_START;
           
        }
        else if(ret > 0)
        {
            if(strstr(buf,"pong")==NULL)
            {
                //linshi Server
                if(strlen(transconf.zhengshi) == 0)
                {
                    if(strstr(buf,"\"action\":\"notify\"")!=NULL)
                    {
                        error = parse_json_zs_notify_data(buf);
                        if(error == 2 || error == 3)
                            goto PRO_START;
                    }
                    else if(strstr(buf,"error") != NULL)
                    {
                        //jiexi json
                        error = parse_json_ls_getarea_data(buf);
                        if(error == 416 || error == 403)
                            goto PRO_START;
                        else
                        {
                            tcpType = TCP_LS_GETAREA;
                            OSATimerStop(_hearttimer_ref);
                            OSATimerStart(_hearttimer_ref, transconf0.hearttime*200, transconf0.hearttime*200, _hearttimer_callback, 0); 
                        }
                    }
                }
                else
                {
                    if(strstr(buf,"\"action\":\"update\"") != NULL)
                    {
                        parse_json_zs_updata_data(buf);
                    }
                    else if(strstr(buf,"\"action\":\"notify\"") != NULL)
                    {
                        error = parse_json_zs_notify_data(buf);
                        if(error == 2 || error == 3)
                            goto PRO_START;
                    }
                    else if(strstr(buf,"\"action\":\"upgrade\"") != NULL)
                    {
                        error = parse_json_zs_upgrade_data(buf);
                        if(error<0)
                        {
                            //upgrade error
                            ack_json_zs_upgrade_data(400);
                        }
                        else if(error == 1)
                        {
                            //upgrade error
                            ack_json_zs_upgrade_data(406);
                        }
                        else
                        {
                            MoudleLedStatus = UPGRADE;
                            fota_start();
                        }
                    }
                    else if(strstr(buf,"error")!=NULL)
                    {
                        error = parse_json_zs_register_data(buf);
                        if(error == 0)
                        {
                            if(registerStatus == 0)
                            {
                                registerStatus = 1;
                                MoudleLedStatus = NORMAL;
                                //heart start
                                OSATimerStop(_hearttimer_ref);
                                OSATimerStart(_hearttimer_ref, transconf0.hearttime*200, transconf0.hearttime*200, _hearttimer_callback, 0); 
                                
                                //updata for tongbu
                                up_json_zs_updata_data();
                                //get time
                                get_json_zs_time_data();
                                //get timer and pulses and lock
                                get_json_zs_timers_data();

                                //check rssi start
                                OSATimerStop(_rssitimer_ref);
                                OSATimerStart(_rssitimer_ref, transconf0.rssitime*200, transconf0.rssitime*200, _rssitimer_callback, 0); 
                            }
                            tcpType = TCP_ZS_CONNECT;
                        }
                        else
                        {
                            if(registerStatus == 0)
                            {
                                //init sd status
                                if(transconf.sdstatus0 > 0 || transconf.sdstatus1 > 0 ||transconf.sdstatus2 > 0 ||transconf.sdstatus3 > 0)
                                {
                                    transconf.sdstatus0 = 0;
                                    transconf.sdstatus1 = 0;
                                    transconf.sdstatus2 = 0;
                                    transconf.sdstatus3 = 0;
                                    trans_conf_file_write(transconf);//save    
                                    gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                                    gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                                    gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                                    gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                                }
                                //init sled status
                                if(transconf.sledstatus == 0)
                                {
                                    transconf.sledstatus = 1;
                                    trans_conf_file_write(transconf);//save    
                                }
                                //init pluse time
                                /*
                                if(transconf.pulsetime0 > 0 || transconf.pulsetime1 > 0 || transconf.pulsetime2 > 0 || transconf.pulsetime3 > 0)
                                {
                                    transconf.pulsetime0 = 0;
                                    transconf.pulsetime1 = 0;
                                    transconf.pulsetime2 = 0;
                                    transconf.pulsetime3 = 0;
                                    trans_conf_file_write(transconf);//save    
                                    if(GpioGetLevel(GPIO_CLR_PIN0) == 1)
                                    {
                                        //close led//20201022
                                        gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                                    }
                                    if(GpioGetLevel(GPIO_CLR_PIN1) == 1)
                                    {
                                        //close led//20201022
                                        gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                                    }
                                    if(GpioGetLevel(GPIO_CLR_PIN2) == 1)
                                    {
                                        //close led//20201022
                                        gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                                    }
                                    if(GpioGetLevel(GPIO_CLR_PIN3) == 1)
                                    {
                                        //close led//20201022
                                        gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                                    }
                                }
                                */
                                if(error == 415 || error == 404)
                                {
                                    sprintf(transconf.zhengshi, "");//del zhengshi server//20220222
                                    trans_conf_file_write(transconf);//save
                                    goto PRO_START;
                                }
                                else
                                {
                                    tcpType = TCP_ZS_REGISTER;
                                    OSATimerStop(_resendtimer_ref);
                                    OSATimerStart(_resendtimer_ref, transconf0.resendtime*200, 0, _resendtimer_callback, 0); 
                                }
                            }
                            else
                            {
                                tcpType = TCP_ZS_NORMAL;
                                OSATimerStop(_resendtimer_ref);
                                OSATimerStart(_resendtimer_ref, transconf0.resendtime*200, 0, _resendtimer_callback, 0); 
                            }
                        }
                    }
                }
            }
        }
    }
}

static void send_task(void * argv)
{
    OS_STATUS  status;
    app_msgq_msg sdkMsg = {0};

    while(1)
    {
        status = OSAMsgQRecv(send_msgq, (void *)&sdkMsg, SEND_MSGQ_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
//            printf("%s, msgid=%d", __FUNCTION__, sdkMsg.msgId);
            switch(sdkMsg.msgId)
            {
                case SEND_TASK_MSG_HB_MSG:
                    heart_thread();
                break;
                case SEND_TASK_MSG_RSSI_MSG:
                    rssi_thread();
                break;
                case SEND_TASK_MSG_RESEND_MSG:
                    resend_thread();
                break;
                case SEND_TASK_MSG_CLOCKUPDATA_MSG:
                    clockupdate_thread();
                break;
                case SEND_TASK_MSG_GPIO_MSG:
                    gpio_thread();
                break;
                case SEND_TASK_MSG_ALARM_MSG:
                    alarm_thread();
                break;
                default:
                break;
            }
        }
        
//        if (sdkMsg.data)
//            free(sdkMsg.data);
    }
}

int send_task_send_msgq(app_msgq_msg * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(send_msgq, SEND_MSGQ_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        printf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgId, status);
        ret = -1;
    }    
    
    return ret;
}

int clock_task_send_msgq(app_msgq_msg * msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(clock_msgq, CLOCK_MSGQ_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS)
    {
        printf("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgId, status);
        ret = -1;
    }    
    
    return ret;
}

/*function for timer heart*/
static void _hearttimer_callback(UINT32 tmrId)
{
//    printf("asr_test  _haerttimer_callback");

//    OSAFlagSet(HeartFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg hbMsg = {0};

    hbMsg.msgId = SEND_TASK_MSG_HB_MSG;

    send_task_send_msgq(&hbMsg);

}

/*function for heart thread*/
static void heart_thread(void)
{
    int ret = 0;
    char buf[6] = {0};
    sprintf(buf, "ping");
    
//    OSA_STATUS status = 0;
//    UINT32 flag_value = 0;
//    while(1)
//    {
//        status = OSAFlagWait(HeartFlgRef, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
//        if(flag_value & 0x01)
//        {
            if(tcpType > TCP_INITIAL)
            {
                printf("heart data");
                ret = webSocket_send(fd, buf, strlen(buf), true, WDT_TXTDATA);
                if(ret <= 0) 
                    printf("websocket send err");
                else
                    printf("websocket send buf:%s",buf);
            }
//            if(tcpType > TCP_INITIAL)
//            {
//                printf("heart data");
//                ret = webSocket_send(fd, buf, strlen(buf), true, WDT_TXTDATA);
//                char memremain[32] = {0};
//                sprintf(memremain, "heap:%ld", OsaGetDefaultMemPoolFreeSize());
//                ret = webSocket_send(fd, memremain, strlen(memremain), true, WDT_TXTDATA);
//                if(ret <= 0) 
//                    printf("websocket send err");
//                else
//                    printf("websocket send buf:%s %s",buf, memremain);
//            }
//        }
//    }
}

/*function for timer check rssi*/
static void _rssitimer_callback(UINT32 tmrId)
{
//    printf("_rssitimer_callback");

//    OSAFlagSet(RssiFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg rssiMsg = {0};

    rssiMsg.msgId = SEND_TASK_MSG_RSSI_MSG;

    send_task_send_msgq(&rssiMsg);
}

/*function for check rssi thread*/
static void rssi_thread(void)
{    
//    OSA_STATUS status = 0;
//    UINT32 flag_value = 0;
    int csq = 0;
    int rssi = 0;
    int csqGrade = 0;
    
//    while(1)
//    {
//        status = OSAFlagWait(RssiFlgRef, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
//        if(flag_value & 0x01)
//        {
            csq = get_RSSI();
            if(csq == 99)
                csq = 0;
            rssi = csq * 2 - 113;
//            csqGrade = get_RSSI_grade(rssi);
            csqGrade = ABS(rssi - transconf0.lastrssi);
//            printf("rssi:%d abs:%d",rssi, csqGrade);
            if(csqGrade > 10)
            {
                if(tcpType > TCP_LS_GETAREA)
                {
                    //up rssi
                    up_json_zs_updata_rssi(rssi);
                }
            }
            transconf0.lastrssi = rssi;
//        }
//    }
}

/*function for resend timer*/
static void _resendtimer_callback(UINT32 tmrId)
{
//    printf("_resendtimer_callback");        
//    OSAFlagSet(ResendFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg resendMsg = {0};

    resendMsg.msgId = SEND_TASK_MSG_RESEND_MSG;

    send_task_send_msgq(&resendMsg);
}

/*function for resend thread*/
static void resend_thread(void)
{
//    OSA_STATUS status = 0;
//    UINT32 flag_value = 0;
      
//    while(1)
//    {
//        status = OSAFlagWait(ResendFlgRef, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
//        if(flag_value & 0x01)
//        {
            printf("resend");
            if(tcpType == TCP_LS_GETAREA )
            {
                tcpType = TCP_LS_CONNECT;
                package_json_ls_getarea_data();
            }
            else if(tcpType == TCP_ZS_REGISTER)
            {
                tcpType = TCP_ZS_CONNECT;
                package_json_zs_register_data();
                registerStatus = 0;
            }
            else if(tcpType == TCP_ZS_NORMAL)
            {
                tcpType = TCP_ZS_CONNECT;
                up_json_zs_updata_data();
            }
//        }
//    }
}

/*function for timer local clock 1s */
static void _timer_callback(UINT32 tmrId)
{
    //local clock timer
    UpdateTimeBuf(&CalcDataTime);    
//    OSAFlagSet(ClockFlgRef, 0x01, OSA_FLAG_OR);//check timer 1s //20211129
    app_msgq_msg clockMsg = {0};

    clockMsg.msgId = CLOCK_TASK_MSG_CLOCK_MSG;

    clock_task_send_msgq(&clockMsg);

}

/*function for thread local clock 1min */
static void clock_task(void * argv)
{
    OSA_STATUS status = 0;
    UINT32 flag_value = 0;
    UINT32 i = 0;
    bool upFlag = false;
    app_msgq_msg sdkMsg = {0};

    while(1)
    {
        status = OSAMsgQRecv(clock_msgq, (void *)&sdkMsg, CLOCK_MSGQ_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            switch(sdkMsg.msgId)
            {
                case CLOCK_TASK_MSG_CLOCK_MSG:
                    if(alarmsCnt > 0)
                    {
//                        print_time_test(&CalcDataTime);//20211129
                        for(i = 0; i < alarmsCnt; i++)
                        {
                            if(GetWakeUpTimePoint2(alarms + i, &CalcDataTime) == 1)
                                upFlag = true;
                        }
                        if(upFlag)
                        {
                            upFlag = false;
                            //updata
//                            OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
                            app_msgq_msg gpioMsg = {0};
     
                            gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
                            send_task_send_msgq(&gpioMsg);
                        }
                    }
//                    else
//                    {
//                        printf("no alarm");
//                    }
                break;
                default:
                break;
            }
        }
        
    }
}

/*function for thread local clock 1day */
static void clockupdate_thread(void)
{
//    OSA_STATUS status = 0;
//    UINT32 flag_value = 0;
//    UINT32 i = 0;
//      
//    while(1)
//    {
//        status = OSAFlagWait(ClockFlgRefUpdateTime, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
//        if(flag_value & 0x01)
//        {
            printf("clockUpdate");
            if(tcpType == TCP_ZS_CONNECT)
            {
                //get time
                get_json_zs_time_data();
            }
//        }
//    }
}

/*function for timer diandong */
static void _pulsetimer_callback0(UINT32 tmrId)
{
    printf("_pulsetimer_callback0");    
    //close gpio
    gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
    //up data
//    OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg gpioMsg = {0};

    gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
    send_task_send_msgq(&gpioMsg);
}

static void _pulsetimer_callback1(UINT32 tmrId)
{
    printf("_pulsetimer_callback1");    
    //close gpio
    gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
    //up data
//    OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg gpioMsg = {0};
    
    gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
    send_task_send_msgq(&gpioMsg);

}

static void _pulsetimer_callback2(UINT32 tmrId)
{
    printf("_pulsetimer_callback2");    
    //close gpio
    gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
    //up data
//    OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg gpioMsg = {0};

    gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
    send_task_send_msgq(&gpioMsg);
}

static void _pulsetimer_callback3(UINT32 tmrId)
{
    printf("_pulsetimer_callback3");    
    //close gpio
    gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
    //up data
//    OSAFlagSet(GpioFlgRef, 0x01, OSA_FLAG_OR);
    app_msgq_msg gpioMsg = {0};

    gpioMsg.msgId = SEND_TASK_MSG_GPIO_MSG;
    send_task_send_msgq(&gpioMsg);
}

/*function for check is diandong open or close */
static void pluse_thread0(void * argv)
{
    OSA_STATUS status = 0;
    UINT32 flag_value = 0;
      
    while(1)
    {
        status = OSAFlagWait(PluseFlgRef0, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        if(flag_value & 0x01)
        {
            printf("transconf.pulsetime0:%d",transconf.pulsetime0);
            if(transconf.pulsetime0 > 0)
            {
                OSATimerStart(_pulsetimer_ref0, transconf.pulsetime0*0.2, 0, _pulsetimer_callback0, 0); 
            }
        }
    }
}

static void pluse_thread1(void * argv)
{
    OSA_STATUS status = 0;
    UINT32 flag_value = 0;
      
    while(1)
    {
        status = OSAFlagWait(PluseFlgRef1, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        if(flag_value & 0x01)
        {
            printf("transconf.pulsetime1:%d",transconf.pulsetime1);
            if(transconf.pulsetime1 > 0)
            {
                OSATimerStart(_pulsetimer_ref1, transconf.pulsetime1*0.2, 0, _pulsetimer_callback1, 0); 
            }
        }
    }
}

static void pluse_thread2(void * argv)
{
    OSA_STATUS status = 0;
    UINT32 flag_value = 0;
      
    while(1)
    {
        status = OSAFlagWait(PluseFlgRef2, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        if(flag_value & 0x01)
        {
            printf("transconf.pulsetime2:%d",transconf.pulsetime2);
            if(transconf.pulsetime2 > 0)
            {
                OSATimerStart(_pulsetimer_ref2, transconf.pulsetime2*0.2, 0, _pulsetimer_callback2, 0); 
            }
        }
    }
}

static void pluse_thread3(void * argv)
{
    OSA_STATUS status = 0;
    UINT32 flag_value = 0;
      
    while(1)
    {
        status = OSAFlagWait(PluseFlgRef3, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        if(flag_value & 0x01)
        {
            printf("transconf.pulsetime3:%d",transconf.pulsetime3);
            if(transconf.pulsetime3 > 0)
            {
                OSATimerStart(_pulsetimer_ref3, transconf.pulsetime3*0.2, 0, _pulsetimer_callback3, 0); 
            }
        }
    }
}

/*function for up gpio status */
static void gpio_thread(void)
{
//    OSA_STATUS status = 0;
//    UINT32 flag_value = 0;
//      
//    while(1)
//    {
//        status = OSAFlagWait(GpioFlgRef, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
//        if(flag_value & 0x01)
//        {
            printf("up data");
            //save initstatus
            transconf.initstatus0 = GpioGetLevel(GPIO_CLR_PIN0);
            transconf.initstatus1 = GpioGetLevel(GPIO_CLR_PIN1);
            transconf.initstatus2 = GpioGetLevel(GPIO_CLR_PIN2);
            transconf.initstatus3 = GpioGetLevel(GPIO_CLR_PIN3);
            trans_conf_file_write(transconf);
            
            if(tcpType > TCP_LS_GETAREA)
            {
               //up data
               up_json_zs_updata_switches();
            }
//        }
//    }
}

/*function for up alarm status */
static void alarm_thread(void)
{
//    OSA_STATUS status = 0;
//    UINT32 flag_value = 0;
//    
//    while(1)
//    {
//        status = OSAFlagWait(GpioFlgRef_alarm, 0x01, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
//        if(flag_value & 0x01)
//        {
            printf("up alarm data");
            if(tcpType == TCP_ZS_CONNECT)
            {
               //up alarm data
               up_json_zs_updata_alarm();
            }
//        }
//    }
}

static void sw_fotaresultCallback(UINT32 result)
{
    printf("%s: result = %ld",__FUNCTION__, result);
    if(result == FOTA_SUCCESS)
    {
        printf("Down Bin Ok! Moudle Rebooting For Update...");
        sleep(1);
        PM812_SW_RESET();// restart will update
    }
    else
    {
        printf("Fota Error,Please try again!");
        MoudleLedStatus = NORMAL;
        ack_json_zs_upgrade_data(409);
    }
}

/*function for fota jsonstr parse */
static void fota_start(void)
{
    printf("fota_start");
    printf("downloadUrl:%s",transconf0.downloadUrl);
    unsigned char str_sha256[32] = {0};
    char sha256str[65]={0};
    char jiaoyan[128] = {0};
    char url[512] = {0};
    char ts[16] = {0};

    snprintf(ts,11,"%s",transconf0.sequence);
    
    sprintf(jiaoyan,"%s%s%s",transconf.deviceId,ts,transconf.factory_apikey);
    printf("jiaoyan:%s", jiaoyan);
    //jiaoyan
    mbedtls_sha256((const unsigned char*)jiaoyan,strlen(jiaoyan),str_sha256,0);
    HexToStr(sha256str, (char*)str_sha256, 32);

    printf("sequence:%s,ts:%s,sha256str:%s",transconf0.sequence,ts,sha256str);
    
    sprintf(url,"%s?deviceid=%s&ts=%s&sign=%s",transconf0.downloadUrl,transconf.deviceId,ts,sha256str);

    printf("url:%s",url);
//    printf("url:%s",url + 150);

    SDK_MYFOTA_UPDATE(url, "", "", sw_fotaresultCallback, 20);
}

/*function for init_trans_conf0*/
static void init_trans_conf0(void)
{
    sprintf(transconf0.ip,"52.80.19.131");
    transconf0.port = 443;
    
    transconf0.hearttime = 30;
    transconf0.resendtime = 30;
    
    sprintf(transconf0.user_apikey,"");
    
    sprintf(transconf0.romVersion,FWVERSION);

    sprintf(transconf0.downloadUrl,"");
    
    sprintf(transconf0.sequence,"");

    transconf0.rssitime = 2;
    transconf0.lastrssi = 6;

    sprintf(at_version,FWVERSION);

}

#ifdef CRON_SUPPORT
/*function for get alarm clock*/
static uint8_t GetWakeUpTimePoint2(AlarmClock *alarm,struct data_time_str *p_data)
{
    if(alarm->type == 1)
    {
        if((alarm->line.cl_Mins[p_data->min] == 1)
        && (alarm->line.cl_Hrs[p_data->hour] == 1)
        && (alarm->line.cl_Days[p_data->day] == 1)
        && (alarm->line.cl_Mons[p_data->month] == 1)
        && (alarm->line.cl_Dow[p_data->week] == 1)
        && (p_data->sec == 0))
        {
            printf("%04d-%02d-%02d  %02d:%02d:%02d   week:%d",
                    p_data->year,p_data->month,p_data->day,
                    p_data->hour,p_data->min,p_data->sec,p_data->week);

            //set gpio
            if(alarm->switchStatus == 1)
            {
                switch (alarm->switchNum)
                {
                    case 0:
                        gpio_open(GPIO_CLR_PIN0,PluseFlgRef0);
                        break;
                    case 1:
                        gpio_open(GPIO_CLR_PIN1,PluseFlgRef1);
                        break;
                    case 2:
                        gpio_open(GPIO_CLR_PIN2,PluseFlgRef2);
                        break;
                    case 3:
                        gpio_open(GPIO_CLR_PIN3,PluseFlgRef3);
                        break;
                    default:
                    break;
                }
            }
            else
            {
                switch (alarm->switchNum)
                {
                    case 0:
                        gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                        break;
                    case 1:
                        gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                        break;
                    case 2:
                        gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                        break;
                    case 3:
                        gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                        break;
                    default:
                    break;
                }
            }            
            return 1;
        }    
    }
    else if(alarm->type == 0 && alarm->enable== 1)
    {
        if((alarm->atTime.year == p_data->year)&&
            (alarm->atTime.month == p_data->month)&&
            (alarm->atTime.day == p_data->day)&&
            (alarm->atTime.hour == p_data->hour)&&
            (alarm->atTime.min == p_data->min))
            {
                if(p_data->sec-alarm->atTime.sec>=0 && p_data->sec-alarm->atTime.sec<=5)
                {
                    //jiaoshi 5s wucha //20211208
                    printf("%04d-%02d-%02d  %02d:%02d:%02d",
                            p_data->year,p_data->month,p_data->day,
                            p_data->hour,p_data->min,p_data->sec);
                    //set gpio
                    if(alarm->switchStatus == 1)
                    {
                        switch (alarm->switchNum)
                        {
                            case 0:
                                gpio_open(GPIO_CLR_PIN0,PluseFlgRef0);
                                break;
                            case 1:
                                gpio_open(GPIO_CLR_PIN1,PluseFlgRef1);
                                break;
                            case 2:
                                gpio_open(GPIO_CLR_PIN2,PluseFlgRef2);
                                break;
                            case 3:
                                gpio_open(GPIO_CLR_PIN3,PluseFlgRef3);
                                break;
                            default:
                            break;
                        }
                    }
                    else
                    {
                        switch (alarm->switchNum)
                        {
                            case 0:
                                gpio_close(GPIO_CLR_PIN0,_pulsetimer_ref0);
                                break;
                            case 1:
                                gpio_close(GPIO_CLR_PIN1,_pulsetimer_ref1);
                                break;
                            case 2:
                                gpio_close(GPIO_CLR_PIN2,_pulsetimer_ref2);
                                break;
                            case 3:
                                gpio_close(GPIO_CLR_PIN3,_pulsetimer_ref3);
                                break;
                            default:
                            break;
                        }
                    }
                    alarm->enable=0;
                    return 1;
                }
        }
    }
    
    return 0;
}

/*function for printf time*/
static uint8_t print_time_test(struct data_time_str *p_data)
{
    printf("%s[%d]%04d-%02d-%02d  %02d:%02d:%02d  week:%d", __FUNCTION__, __LINE__,
            p_data->year,p_data->month,p_data->day,
            p_data->hour,p_data->min,p_data->sec,p_data->week);
    
    return 0;
}
#endif

// End of file : main.h 2023-3-18 17:24:44 by: zhaoning 

