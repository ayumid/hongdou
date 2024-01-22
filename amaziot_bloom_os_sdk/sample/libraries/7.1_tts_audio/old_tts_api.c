#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "ci_sim.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "telutl.h"
#include "ci_mm.h"
#include "acm_comm.h"

#include "herottspcm.h"
#include "herotime.h"
#include "herotts.h"


/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { RTI_LOG("[sdk]"fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { CPUartLogPrintf("[sdk]"fmt, ##args); } while(0)


extern UINT32 AudioHAL_AifGetVolume(void);
extern void AudioHAL_AifSetVolume(UINT32 vol);
extern int hero_tts_stop(void);


/*****************************************************************
* Function: AUDIO_SET_VOLUME_VALUE
*
* Description:
*     该函数用于设置音量大小。
* 
* Parameters:
*     volume          [In]     音量等级0-5;
* Return:
*     NONE
*
*****************************************************************/
void AUDIO_SET_VOLUME_VALUE(int volume)
{
    int volumeValue = 0;
    printf("AUDIO_SET_VOLUME_VALUE micGainValue=%d", volume);    

    switch(volume)
    {
        case 0:
            volumeValue = 0;
            break;
        case 1:
            volumeValue = 3;
            break;
        case 2:
            volumeValue = 5;
            break;
        case 3:
            volumeValue = 7;
            break;
        case 4:
            volumeValue = 9;
            break;
        case 5:
            volumeValue = 11;
            break;                        
        default:
            volumeValue = 5;
            break;
    }
    
    printf("AUDIO_SET_VOLUME_VALUE volumeValue=%d", volumeValue);    

    AudioHAL_AifSetVolume(volumeValue);
}

/*****************************************************************
* Function: AUDIO_SET_VOLUME_VALUE
*
* Description:
*     该函数用于获取音量大小。
* 
* Parameters:
*     NONE
* Return:
*     volume          音量等级0-5;
*
*****************************************************************/
int AUDIO_GET_VOLUME_VALUE(void)
{
    int clvlValue = 0;
    
    clvlValue = AudioHAL_AifGetVolume();
    printf("AUDIO_GET_VOLUME_VALUE clvlValue=%d", clvlValue);    
    
    if (clvlValue == 0){
        clvlValue = 0;
    }else if ((clvlValue >= 1) && (clvlValue <= 3)){
        clvlValue = 1;
    }else if ((clvlValue >= 4) && (clvlValue <= 5)){
        clvlValue = 2;
    }else if ((clvlValue >= 6) && (clvlValue <= 7)){
        clvlValue = 3;
    }else if ((clvlValue >= 8) && (clvlValue <= 9)){
        clvlValue = 4;
    }else if ((clvlValue >= 10) && (clvlValue <= 11)){
        clvlValue = 5;
    }
    
    return clvlValue;
}

/*****************************************************************
* Function: AUDIO_SET_VOLUME_VALUE_TINY
*
* Description:
*     该函数用于设置音量大小。功能同AUDIO_SET_VOLUME_VALUE，音量等级调整跨度更小
* 
* Parameters:
*     volume          [In]     音量等级0-11;
* Return:
*     NONE
*
*****************************************************************/
void AUDIO_SET_VOLUME_VALUE_TINY(int volume)
{    
    printf("AUDIO_SET_VOLUME_VALUE volumeValue=%d", volume);    

    AudioHAL_AifSetVolume(volume);
}

/*****************************************************************
* Function: AUDIO_GET_VOLUME_VALUE_TINY
*
* Description:
*     该函数用于获取音量大小。
* 
* Parameters:
*     NONE
* Return:
*     volume          音量等级0-11;
*
*****************************************************************/
int AUDIO_GET_VOLUME_VALUE_TINY(void)
{    
    return AudioHAL_AifGetVolume();
}


/*****************************************************************
* Function: AUDIO_TTS_SPEED
*
* Description:
*     该函数用于设置TTS语速。
* 
* Parameters:
*     speed          [In]     -32768~+32767， 0 是正常语速;
* Return:
*     NONE
*
*****************************************************************/
void AUDIO_TTS_SPEED(int speed)
{
    hero_tts_set_speed(speed);
}

/*****************************************************************
* Function: AUDIO_TTS_RUN
*
* Description:
*     该函数用于设置TTS语速。
* 
* Parameters:
*     text          [In] 小端unicode文本;
*     textSize      [In] 小端unicode文本SIZE，最大128个汉字(512 char);
*     callback      [In] tts播放回调函数，详见hero_tts_play_callback;
*     userData    [In] userData;
* Return:
*     NONE
*
*****************************************************************/
void AUDIO_TTS_RUN(char *text, int textSize, hero_tts_play_callback callback, int userData)
{
     static int tts_have_run = 0;

    if (!tts_have_run){
        hero_timer_init_task();
        tts_have_run = 1;
    }

    hero_tts_start(text, textSize, callback, userData);
}

/*****************************************************************
* Function: AUDIO_TTS_STOP
*
* Description:
*     该函数用于停止TTS播放。
* 
* Parameters:
*     speed          [In]     -32768~+32767， 0 是正常语速;
* Return:
*     NONE
*
*****************************************************************/
void AUDIO_TTS_STOP(void)
{
    hero_tts_stop();
}