#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"
#include "cJSON.h"
#include "sdk_api.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define _STACK_SIZE            0x2000
static void* _task_stack = NULL;

static OSTaskRef _task_ref = NULL;


OSATimerRef _task_delete_timer_ref = NULL;

static void _task(void *ptr);

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
    
    _task_stack = malloc(_STACK_SIZE);
    
    ret = OSATaskCreate(&_task_ref, _task_stack, _STACK_SIZE, 100, "_task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

void DeleteTaskByTimer(UINT32 id)
{
    OSA_STATUS status;

    if(_task_ref)
    {
        status = OSATaskDelete(_task_ref);
        ASSERT(status == OS_SUCCESS);
        _task_ref = NULL;
       
        if(_task_stack)
        {
            free(_task_stack);
            _task_stack = NULL;
        }
    }

    OSATimerDelete (_task_delete_timer_ref);
}


static void wait_network_ready(void)
{
    int count = 0;
    int ready=0;

    while (!ready){
        if(getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){
            ready = 1;
        }
        catstudio_printf("wait_network_ready: %d s", count++);
        if (count > 300)
            PM812_SW_RESET();
            
        OSATaskSleep(200);
    }
}


void lbsResultCallback(int result, char *jsondata)
{    
    cJSON *json_root = NULL, *jresult = NULL, *jtmp = NULL;
    double latitude, longitude;
    char desc[256] = {0};
    
    catstudio_printf("lbsResultCallback result %d, jsondata %s\n", result, jsondata==NULL?"NULL":jsondata);    
    
    if (result == 0){
        
        json_root = cJSON_Parse(jsondata);
        if (!json_root) {

            goto clean;
        }

        jresult = cJSON_GetObjectItemCaseSensitive(json_root, "result");
        if (!jresult || !cJSON_IsObject(jresult)) {

            goto clean;
        }
        jtmp = cJSON_GetObjectItemCaseSensitive(jresult, "location");
        if (!jtmp || !cJSON_IsString(jtmp)) {

            goto clean;
        }
        if (sscanf(jtmp->valuestring, "%lf,%lf", &longitude, &latitude) != 2) {

            goto clean;
        }

        jtmp = cJSON_GetObjectItemCaseSensitive(jresult, "desc");
        if (jtmp && cJSON_IsString(jtmp)) {
            strncpy(desc, jtmp->valuestring, sizeof(desc)-1);
        }

        catstudio_printf("+CLOCATION: %.6f,%.6f\r\n", latitude, longitude);    
        if (strlen(desc) > 0) {
            catstudio_printf("+CLOCDESC: \"%s\"\r\n", desc);
        }        
    }
    
clean:    
    if (json_root)
        cJSON_Delete(json_root);    
    
}

#define GAODE_LBS_KEY "1cf7b3a730fc0594be613ebd46a1cd71"  //需要客户自行申请，，该key仅限测试，，可能会偶现受限情况

/*****************************************************************
* Function: SDK_MYFOTA_UPDATE
*
* Description:
*     该函数用于请求高德LBS用于获取定位
* 
* Parameters:
*     mode              [In]     LBS mode。
*                             0: 单基站定位，速度响应更快
*                             1: 多基站定位，速度可能会慢，位置可能更精准
*     key                  [In]     高德 LBS 专用key,需要客户自行申请
*     cb                  [In]     lbsCallback参数result和jsondata描述。
*                             0: 获取高德LBS定位信息成功，获取到的json数据
*                             1: 获取高德LBS定位信息失败，NULL
* Return:
*     NONE
*****************************************************************/
extern void SDK_GAODE_LBS_REQUEST(int mode, char *key, lbsCallback cb);


static void _task(void *ptr)
{
    int count = 5;
    unsigned int current_os_ticks;

    wait_network_ready();
    
    while(count--){
        
        current_os_ticks = OSAGetTicks();    // 获取系统tick ,一个tick 5ms，即开机时间    
        catstudio_printf("this is a test task current_os_ticks %d\n", current_os_ticks);
        OSATaskSleep(200);        // 如果可能存在while空跑的case，一定需要加上个sleep，不然可能会造成系统调度异常
        
        SDK_GAODE_LBS_REQUEST(1, GAODE_LBS_KEY, lbsResultCallback);
        OSATaskSleep(200*5);    
        SDK_GAODE_LBS_REQUEST(2, GAODE_LBS_KEY, lbsResultCallback);
        OSATaskSleep(200*5);
    }
    
    OSATimerCreate(&_task_delete_timer_ref);
    OSATimerStart(_task_delete_timer_ref, 400, 0, DeleteTaskByTimer, 0);
}
