/**
  ******************************************************************************
  * @file    mb_port.c
  * @author  Derrick Wang
  * @brief   modebus移植接口
  ******************************************************************************
  * @note
  * 该文件为modbus移植接口的实现，根据不同的MCU平台进行移植
  ******************************************************************************
  */


#include "mb_include.h"
#include "UART.h"
#include "sdk_api.h"

extern MBHost mbHost;

extern OSMsgQRef	MsgUartData_sdk;
OSATimerRef _rxcachetimer_ref = NULL;
OSATimerRef _txtimerout_ref = NULL;


static void _txtimerout_callback(UINT32 tmrId);
static void _rxcachetimer_callback(UINT32 tmrId);
static void handle_serial_data(MsgUartDataParam_sdk *uartData);
static void uartdata_thread(void);


/*function for init mb_port_uartInit*/
void mb_port_uartInit(void)
{
	 int ret;
	 ret = OSATimerCreate(&_txtimerout_ref);
     ASSERT(ret == OS_SUCCESS);
	 ret = OSAMsgQCreate(&MsgUartData_sdk, "MsgUartData_sdk", sizeof(MsgUartDataParam_sdk), 500, OS_FIFO);
	 ASSERT(ret == OS_SUCCESS);
	 ret = OSATimerCreate(&_rxcachetimer_ref);
     ASSERT(ret == OS_SUCCESS);
	 sys_thread_new("uartdata_thread", uartdata_thread, NULL, 4096*2, 80);
}

/*function for tx timer out*/
static void _txtimerout_callback(UINT32 tmrId)
{
	printf("asr_test  _txtimerout_callback");
	switch(mbHost.state)
	{
		/*发送完但没有接收到数据*/
		case MBH_STATE_TX_END:
			mbHost.rxTimeOut++;
			if(mbHost.rxTimeOut>=MBH_REC_TIMEOUT) //接收超时
			{
				mbHost.rxTimeOut=0;
				mbHost.state=MBH_STATE_REC_ERR;
				OSATimerStop(_txtimerout_ref);	//关闭定时器
			}
			break;
		case MBH_STATE_RX:     	//3.5T到,接收一帧完成			
			OSATimerStop(_txtimerout_ref);		//关闭定时器
			break;
	}
}
void mbh_uartTxIsr(void)
{
	switch (mbHost.state)
	{
		case MBH_STATE_TX:
			//send data
			UART_SEND_DATA((UINT8 *)mbHost.txBuf, mbHost.txLen);
			mbHost.state=MBH_STATE_TX_END;
			//open timer
			OSATimerStart(_txtimerout_ref, 40, 40, _txtimerout_callback, 0); // 200 ms timer
			break;
		case MBH_STATE_TX_END:
			break;
	}	
}
/*function for rx cache timer*/
static void _rxcachetimer_callback(UINT32 tmrId)
{
	printf("asr_test  _rxcachetimer_callback");
	mbHost.state=MBH_STATE_RX_CHECK;
	OSATimerStop(_rxcachetimer_ref);
}
/*function for handle_serial_data*/
static void handle_serial_data(MsgUartDataParam_sdk *uartData)
{
	mbHost.state=MBH_STATE_RX;
    //close huancun timer
	OSATimerStop(_rxcachetimer_ref);
	//huan cun
	memcpy(&(mbHost.rxBuf[mbHost.rxCounter]),(char *)uartData->UArgs,uartData->len);//dmh 20200709
	mbHost.rxCounter += uartData->len;
	//open huancun timer
	OSATimerStart(_rxcachetimer_ref, 40, 40, _rxcachetimer_callback, 0); // 200 ms timer
	
}
/*function for at uart rx thread*/
static void uartdata_thread(void)
{
    MsgUartDataParam_sdk uart_temp;
    OSA_STATUS status;
    
    set_UartDataSwitch_sdk(1);  // open pass-through
   
    while (1) {
        memset(&uart_temp, 0, sizeof(MsgUartDataParam_sdk));
        
        status = OSAMsgQRecv(MsgUartData_sdk, (UINT8 *)&uart_temp, sizeof(MsgUartDataParam_sdk), OSA_SUSPEND);
        
        if (status == OS_SUCCESS) {
            if (uart_temp.UArgs) {
                printf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                handle_serial_data(&uart_temp);
            }
        }
    }
}






