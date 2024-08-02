//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-16
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-16
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
#define sample_uarts_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_uarts_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

//#define SAMPLE_NOMAL_UART
//#define SAMPLE_DEBUG_UART
#define SAMPLE_QLUART_UART

#ifdef SAMPLE_NOMAL_UART
typedef struct{
    int port;   // 0:uart, 1:uart3, 2:uart4
    int len;
    UINT8 *data;
}uartParam;
#endif /* ifdef SAMPLE_NOMAL_UART.2024-8-1 14:21:45 by: zhaoning */

#define SAMPLE_UARTS_TASK_STACK_SIZE     1024 * 8


#ifdef SAMPLE_QLUART_UART
typedef struct{
    int len;
    UINT8 *data;
}uartParam;
#endif /* ifdef SAMPLE_QLUART_UART.2024-8-1 14:18:50 by: zhaoning */

// Private variables ------------------------------------------------------------

static OSMsgQRef sample_uarts_msgq = NULL;

static void* sample_uarts_task_stack = NULL;
static OSTaskRef sample_uarts_task_ref = NULL;

static void sample_uarts_uart_thread(void *param);

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

/*
0: 1.8v -- default
1: 2.8V
2: 3.3V
*/
extern void UART_INIT_VOLT(int voltType);

// Functions --------------------------------------------------------------------

void Phase1Inits_enter(void)
{
    // 设置UART初始化电平，只能在Phase1Inits_enter调用
    //UART_INIT_VOLT(2);
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

    sample_uarts_uart_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&sample_uarts_msgq, "sample_uarts_msgq", sizeof(uartParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    sample_uarts_task_stack = malloc(SAMPLE_UARTS_TASK_STACK_SIZE);
    ASSERT(sample_uarts_task_stack != NULL);

    status = OSATaskCreate(&sample_uarts_task_ref, sample_uarts_task_stack, SAMPLE_UARTS_TASK_STACK_SIZE, 82, "sample_uarts_uart_thread", sample_uarts_uart_thread, NULL);
    ASSERT(status == OS_SUCCESS);    
}

#ifdef SAMPLE_NOMAL_UART
void sample_uarts_main_uart_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    sample_uarts_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 0;
    
    osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

void sample_uarts_uart3_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
    sample_uarts_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 1;

    osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

void sample_uarts_uart4_recv_cbk(UINT8 * recv_data, UINT32 recv_len)
{    
    uartParam uart_data = {0};
    OSA_STATUS osa_status;
    
    char *tempbuf = (char *)malloc(recv_len+10);
    memset(tempbuf, 0x0, recv_len+10);
    memcpy(tempbuf, (char *)recv_data, recv_len);
    
//    sample_uarts_uart_printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, recv_len, (char *)(recv_data)); 
    uart_data.data = (UINT8 *)tempbuf;
    uart_data.len = recv_len;
    uart_data.port = 2;

    osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

static void sample_uarts_uart_thread(void *param)
{
    OSA_STATUS status;
    uartParam uart_data = {0};
    s_uart_cfg uartCfg = {0};
    s_uart_cfg uart3Cfg = {0};
    s_uart_cfg uart4Cfg = {0};
    
    // uart uart3 uart4 默认波特率都是115200
    UART_OPEN(sample_uarts_main_uart_recv_cbk); 
    UART_GET_CONFIG(&uartCfg);
    uartCfg.baudrate = 115200;
    UART_SET_CONFIG(&uartCfg);

    UART3_OPEN(sample_uarts_uart3_recv_cbk); 
    UART3_GET_CONFIG(&uart3Cfg);
    uart3Cfg.baudrate = 4800;
    UART3_SET_CONFIG(&uart3Cfg);    

    UART4_OPEN(sample_uarts_uart4_recv_cbk); 
    UART4_GET_CONFIG(&uart4Cfg);
    uart4Cfg.baudrate = 4800;
    UART4_SET_CONFIG(&uart4Cfg);
    
    while (1) {
        memset(&uart_data, 0x00, sizeof(uartParam));        
        status = OSAMsgQRecv(sample_uarts_msgq, (UINT8 *)&uart_data, sizeof(uartParam), OSA_SUSPEND);    //recv data from uart
        if (status == OS_SUCCESS) {
            if (uart_data.data) {
//                sample_uarts_uart_printf("%s: port %d len:%d, data:%s\n", __FUNCTION__, uart_data.port, uart_data.len, (char *)(uart_data.data));
                sample_uarts_uart_printf("%s: port %d len:%d\n", __FUNCTION__, uart_data.port, uart_data.len);
//                if (uart_data.port == 0)
//                    UART_SEND_DATA(uart_data.data,uart_data.len);
//                else if (uart_data.port == 1)
//                    UART3_SEND_DATA(uart_data.data,uart_data.len);
//                else if (uart_data.port == 2)
//                    UART4_SEND_DATA(uart_data.data,uart_data.len);
//                
                free(uart_data.data);
            }
        }
    }
}
#endif /* ifdef SAMPLE_NOMAL_UART.2024-8-1 14:16:41 by: zhaoning */

#ifdef SAMPLE_QLUART_UART
#include "ql_uart_api.h"

void ql_uart_callback(QL_UART_PORT_NUMBER_E port, void *para)
{
	uartParam uart_data = {0};
    OSA_STATUS osa_status;
	
	int read_len = 0;
	const int max_read_len = 2048;
	char *r_data = NULL;		//char r_data[2048] = {0};
	char *msg_data = NULL;
	
	r_data = (char *)malloc(max_read_len);
	if (!r_data){
		sample_uarts_uart_printf("r_data malloc failed\r\n");
		return;
	}

	memset(r_data, 0x00, max_read_len);		
	read_len = ql_uart_read(port, (unsigned char *)r_data, max_read_len);
	sample_uarts_uart_printf("port %d, read_len = %d\r\n", port, read_len);			
	if (read_len > 0) {
		
		msg_data = (char *)malloc(read_len+1);
		memset(msg_data, 0x0, read_len+1);
		memcpy(msg_data, (char *)r_data, read_len);

		uart_data.data = (UINT8 *)msg_data;	// will free the buf in recv task
		uart_data.len = read_len;
		
		osa_status = OSAMsgQSend(sample_uarts_msgq, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
		ASSERT(osa_status == OS_SUCCESS);	
	} 
	
	if (r_data){
		free(r_data);
	}
	
	sample_uarts_uart_printf("ql_uart_callback end\r\n");	
}

//#define SDK_UART_CB_RECV_DATA

void debugx(int len, UINT8 *data)
{
	char buffer[256] = {0};
	int length = 0;
	
	while(len-length > 0){
		memset(buffer, 0x00, sizeof(buffer));
		if (len-length > 128){
			memcpy(buffer, data+length, 128);
			sample_uarts_uart_printf("debug: %s", buffer);
			length += 128;						
		}else{
			memcpy(buffer, data+length, len-length);
			sample_uarts_uart_printf("debug: %s", buffer);
			length = len;
		}
	}		
}

static void sample_uarts_uart_thread(void *ptr)
{
	int ret = -1;
    uartParam uart_temp;	
    OSA_STATUS status;
	
	int read_bytes = 0, write_bytes = 0;
	char r_data[2048] = {0};

	//QL_UART_PORT_NUMBER_E current_test_pot = QL_DEBUG_UART_PORT;	
	QL_UART_PORT_NUMBER_E current_test_pot = QL_MAIN_UART_PORT;
	//QL_UART_PORT_NUMBER_E current_test_pot = QL_BT_UART_PORT;	
	//QL_UART_PORT_NUMBER_E current_test_pot = QL_UART4_PORT;
	//QL_UART_PORT_NUMBER_E current_test_pot = QL_USB_AT_PORT;
	//QL_UART_PORT_NUMBER_E current_test_pot = QL_USB_MODEM_PORT;

	ql_uart_config_t dcb;

	ret = ql_uart_open(current_test_pot, QL_UART_BAUD_4800, QL_FC_NONE);
	if (ret) {
		sample_uarts_uart_printf("open uart[%d] failed! \n", current_test_pot);
		return;
	}

	ql_uart_get_dcbconfig(current_test_pot, &dcb);
	sample_uarts_uart_printf("[%s][%d]baudrate:%d, data_bit:%d, stop_bit:%d, parity_bit:%d, flow_ctrl:%d \r\n", __func__, __LINE__, dcb.baudrate, dcb.data_bit, dcb.stop_bit, dcb.parity_bit, dcb.flow_ctrl);
	dcb.baudrate = QL_UART_BAUD_4800;
	ql_uart_set_dcbconfig(current_test_pot, &dcb);
	
#ifdef SDK_UART_CB_RECV_DATA
	ql_uart_register_cb(current_test_pot, ql_uart_callback);
#else
	ql_uart_register_cb(current_test_pot, NULL);
#endif
	
	while(1){
		
#ifdef SDK_UART_CB_RECV_DATA
        memset(&uart_temp, 0, sizeof(uartParam));   
		sample_uarts_uart_printf("wait uart data\n");
        status = OSAMsgQRecv(sample_uarts_msgq, (UINT8 *)&uart_temp, sizeof(uartParam), OSA_SUSPEND);//recv data from callback
        
        if (status == OS_SUCCESS) {
            if (uart_temp.data) {

				write_bytes = ql_uart_write(current_test_pot, uart_temp.data, uart_temp.len);
				sample_uarts_uart_printf("uart write data, write_bytes:%d,%d, data:%s\n", write_bytes, uart_temp.len, (char *)(uart_temp.data));			
				debugx(uart_temp.len, uart_temp.data);
				free(uart_temp.data);
            }
        }
#else	
		memset(r_data, 0, sizeof(r_data));
	 	read_bytes = ql_uart_read(current_test_pot, (unsigned char *)r_data, sizeof(r_data));
		sample_uarts_uart_printf("uart read data, read_bytes:%d, data:%s\n", read_bytes, r_data);
		
		if (read_bytes > 0){
			write_bytes = ql_uart_write(current_test_pot, (unsigned char *)r_data, read_bytes);
			sample_uarts_uart_printf("uart write data, write_bytes:%d, data:%s\n", write_bytes, r_data);			
		}else{
			OSATaskSleep(10);						
		}		
#endif	

	}
}
#endif /* ifdef SAMPLE_QLUART_UART.2024-8-1 14:17:29 by: zhaoning */

// End of file : main.c 2023-5-16 9:39:37 by: zhaoning 

