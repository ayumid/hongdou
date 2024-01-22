/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: sdk_api.h
 *
 *  Description: AT interface prototypes for yuge AT commands
 *
 *  History:
 *  Jan 9, 2020- Li Tang Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifndef _OLD_TTS_API_H
#define _OLD_TTS_API_H

#include "osa.h"
#include "herottspcm.h"
#include "herotime.h"
#include "herotts.h"


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
void AUDIO_SET_VOLUME_VALUE(int volume);


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
int AUDIO_GET_VOLUME_VALUE(void);


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
void AUDIO_SET_VOLUME_VALUE_TINY(int volume);


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
int AUDIO_GET_VOLUME_VALUE_TINY(void);



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
void AUDIO_TTS_SPEED(int speed);



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
void AUDIO_TTS_RUN(char *text, int textSize, hero_tts_play_callback callback, int userData);


#endif

