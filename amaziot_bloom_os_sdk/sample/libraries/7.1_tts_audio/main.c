//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2024-5-21
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-21
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

#include "UART.h"
#include "osa.h"
#include "cgpio.h"
#include "synthtext.h"
#include "old_tts_api.h"
#include "AudioHAL.h"

// Private defines / typedefs ---------------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)

#define _TASK_STACK_SIZE     2048


#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

// Private variables ------------------------------------------------------------

static void* _task_stack;

static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;
static OSFlagRef _flag_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void _timer_callback(UINT32 tmrId);
static void _task(void *ptr);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

extern void update_the_cp_ver(char *cp_ver);    // max length 128
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

extern AUDIOHAL_ERR_T AudioHAL_AifOpen(AUDIOHAL_ITF_T itf, AUDIOHAL_AIF_DEVICE_CFG_T *config);

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

    ret = OSAFlagCreate(&_flag_ref);
    ASSERT(ret == OS_SUCCESS);

    ret = OSATimerCreate(&_timer_ref);
    ASSERT(ret == OS_SUCCESS);

    _task_stack = malloc(_TASK_STACK_SIZE);
    ASSERT(_task_stack != NULL);

    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 88, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

    catstudio_printf("Phase2Inits_exit OSATimerStart\n");

    OSATimerStart(_timer_ref, 1 * 200, 15 * 200, _timer_callback, 0); // 10 seconds timer
}


static void _timer_callback(UINT32 tmrId)
{
    OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
}


//tts输入为unicode小端数据
const char tts_data[] = {0xed,0x8b,0xf3,0x97,0x6c,0x8f,0x62,0x63,0x4b,0x6d,0xd5,0x8b,0xed,0x8b,0xf3,0x97,0x6c,0x8f,0x62,0x63,0x4b,0x6d,0xd5,0x8b,0xed,0x8b,0xf3,0x97,0x6c,0x8f,0x62,0x63,0x4b,0x6d,0xd5,0x8b};
int tts_data_len = sizeof(tts_data);


static OSFlagRef tts_flag_ref = NULL;


static void tts_play_callback(int ret, int userData)
{
    catstudio_printf("tts done \n");
    if (tts_flag_ref)
        OSAFlagSet(tts_flag_ref, 0x01, OSA_FLAG_OR);
}

static void _task(void *ptr)
{
    OSA_STATUS status;
    UINT32 flag_value;
    UINT32 flag_mask = TASK_TIMER_CHANGE_FLAG_BIT;
    
    // TTS使用内部codec
    AudioHAL_AifUseInternalCodec();
    // TTS使用pcm输出
    //AudioHAL_AifUseSSPA();
    
    status = OSAFlagCreate(&tts_flag_ref);
    ASSERT(status == OS_SUCCESS);
    
    while(1) {
        status = OSAFlagWait(_flag_ref, flag_mask, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
            static int count = 0;
            count++;
            catstudio_printf("_task: 11count: %d\n", count);
            AUDIO_SET_VOLUME_VALUE(5);
            if(count % 3 == 0){
                
                AUDIO_TTS_SPEED(TIHO_TTS_SPEED_MIN);    
                AUDIO_TTS_RUN((char *)tts_data, tts_data_len, tts_play_callback, NULL);                        
            }
            else if(count % 3 == 1){
                            
                AUDIO_TTS_SPEED(TIHO_TTS_SPEED_NORMAL);    
                // 阻塞播放    
                do{
                    UINT32 tts_flag_value;
                    AUDIO_TTS_RUN((char *)tts_data, tts_data_len, tts_play_callback, NULL);    
                    OSAFlagWait(tts_flag_ref, 0x01, OSA_FLAG_OR_CLEAR, &tts_flag_value, 200*10);    // 等待播放结束回调setflag，或者10s超时（这个时间根据实际情况设置，防止卡死）
                }while(0);
                // 阻塞播放    
                do{
                    UINT32 tts_flag_value;
                    AUDIO_TTS_RUN((char *)tts_data, tts_data_len, tts_play_callback, NULL);    
                    OSAFlagWait(tts_flag_ref, 0x01, OSA_FLAG_OR_CLEAR, &tts_flag_value, 200*10);    // 等待播放结束回调setflag，或者10s超时（这个时间根据实际情况设置，防止卡死）
                }while(0);
                // 阻塞播放    
                do{
                    UINT32 tts_flag_value;
                    AUDIO_TTS_RUN((char *)tts_data, tts_data_len, tts_play_callback, NULL);    
                    OSAFlagWait(tts_flag_ref, 0x01, OSA_FLAG_OR_CLEAR, &tts_flag_value, 200*10);    // 等待播放结束回调setflag，或者10s超时（这个时间根据实际情况设置，防止卡死）
                }while(0);
            }
            else if(count % 3 == 2){
                
                AUDIO_TTS_SPEED(TIHO_TTS_SPEED_MAX);    
                AUDIO_TTS_RUN((char *)tts_data, tts_data_len, tts_play_callback, NULL);
            }


        }
    }
}

// End of file : main.c 2024-5-21 10:16:53 by: zhaoning 
