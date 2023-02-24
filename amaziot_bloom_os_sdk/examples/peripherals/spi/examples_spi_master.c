//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : examples_spi_master.c
// Auther      : win
// Version     :
// Date : 2021-12-29
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-29
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "incl_config.h"
#include "utils_common.h"
#include "oled.h"
#include "bmp.h"

#include "wmri.h"

#ifdef INCL_EXAMPLES_SPI
// Private defines / typedefs ---------------------------------------------------
#define _TASK_STACK_SIZE     1024*100
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
static OSTaskRef _task_ref = NULL;

extern OSMsgQRef  MsgSpiQ;

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
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

	WMRI_Init();
	
	ret = OSAMsgQCreate(&MsgSpiQ, "MsgSpiQ", sizeof(MsgSpiParam), 500, OS_FIFO);
    ASSERT(ret == OS_SUCCESS);
	
    ret = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 120, "test-task", _task, NULL);
    ASSERT(ret == OS_SUCCESS);

}
/***********************************************************************
* Description : Wifi_Init
* Arguments   : 
* Returns     : 
* Author      : dmh
***********************************************************************/
static int Wifi_Init(void)
{
	int ret;
	int errorCnt = 0;
	//read for clean init flag
	SPIRxData();
	SPIRxData();
	SPIRxData();
	SPIRxData();
	SPIRxData();
	wait_spi:
	//1.spi is ok
	ret = RISpiApply();

	if(ret == 0){
		errorCnt++;
		if(errorCnt>10)
			return -1;
		goto wait_spi;
	}
	errorCnt = 0;
	wait_ap:
	//2.wifi is ap
	ret = RIGetWlanType();
	if(ret != 2){
		RISetWlanType(APP_WLAN_AP);
		OSATaskSleep(20);
		errorCnt++;
		if(errorCnt>10)
			return -1;
		goto wait_ap;
	}
	return 0;
}

static void _task(void *ptr)
{

#if 0
	u8 t;
	//初始化OLED  
	OLED_Init();			
	t=' ';
	while(1) 
	{		
		OLED_Clear();
		OLED_ShowCHinese(0,0,0);//北
		OLED_ShowCHinese(20,0,1);//京
		OLED_ShowCHinese(40,0,2);//奇
		OLED_ShowCHinese(60,0,3);//迹
		OLED_ShowCHinese(80,0,4);//物
		OLED_ShowCHinese(100,0,5);//联
		OLED_ShowString(0,2,"OLED TEST V1.0");
	 	OLED_ShowString(20,4,"2021/12/29");  
		OLED_ShowString(0,6,"ASCII:");  
		OLED_ShowString(63,6,"CODE:");  
		OLED_ShowChar(48,6,t);//显示ASCII字符	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,6,t,3,16);//显示ASCII字符的码值 	
			
		msleep(5);
		OLED_Clear();
		msleep(5);
		OLED_DrawBMP(0,0,128,8,BMP1);  //图片显示(图片显示慎用，生成的字表较大，会占用较多空间，FLASH空间8K以下慎用)
		msleep(5);
	}	
#else
	int ret;
	OSA_STATUS status;
	MsgSpiParam MsgSpi_temp;

	while(!IsAtCmdSrvReady())
	{
		sleep(1);
	}
	//load wifi moudle
	ret = Wifi_Init();
	if(ret == 0){
		sdk_uart_printf("Wifi Load Ok!\n");
	}else{
		sdk_uart_printf("Wifi Load Error!\n");
	}
	while (1){
		memset(&MsgSpi_temp, 0, sizeof(MsgSpiParam));  
        status = OSAMsgQRecv(MsgSpiQ, (UINT8 *)&MsgSpi_temp, sizeof(MsgSpiParam), OSA_SUSPEND);//recv data from spi
        
        if (status == OS_SUCCESS) {
            if (MsgSpi_temp.UArgs) {
               	printf("Spi Data:%d\r\n",MsgSpi_temp.len);
				free(MsgSpi_temp.UArgs);
            }
        }
        
    }
#endif /* if 0. 2021-12-31 17:16:29 by: win */
}
#endif /* ifdef INCL_EXAMPLES_SPI.2021-12-29 11:25:06 by: win */

// End of file : examples_spi_master.h 2021-12-29 11:22:51 by: win 

