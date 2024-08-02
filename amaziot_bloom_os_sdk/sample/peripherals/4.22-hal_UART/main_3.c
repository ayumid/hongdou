#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"
#include "ql_uart_api.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sleep(x) OSATaskSleep((x) * 200)//second

#define UART_STACK_SIZE			0x8000
static void* uart_task_stack = NULL;

static OSTaskRef uart_task_ref = NULL;


static OSMsgQRef uartMsgQ = NULL;

typedef struct{
    int len;
    UINT8 *data;
}uartParam;

static void uart_task(void *ptr);

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
	
    ret = OSAMsgQCreate(&uartMsgQ, "uartMsgQ", sizeof(uartParam), 300, OS_FIFO);
    ASSERT(ret == OS_SUCCESS);	
	
	uart_task_stack = malloc(UART_STACK_SIZE);
	
    ret = OSATaskCreate(&uart_task_ref, uart_task_stack, UART_STACK_SIZE, 100, "uart_task", uart_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

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
		catstudio_printf("r_data malloc failed\r\n");
		return;
	}

	memset(r_data, 0x00, max_read_len);		
	read_len = ql_uart_read(port, (unsigned char *)r_data, max_read_len);
	catstudio_printf("port %d, read_len = %d\r\n", port, read_len);			
	if (read_len > 0) {
		
		msg_data = (char *)malloc(read_len+1);
		memset(msg_data, 0x0, read_len+1);
		memcpy(msg_data, (char *)r_data, read_len);

		uart_data.data = (UINT8 *)msg_data;	// will free the buf in recv task
		uart_data.len = read_len;
		
		osa_status = OSAMsgQSend(uartMsgQ, sizeof(uartParam), (UINT8*)&uart_data, OSA_NO_SUSPEND);
		ASSERT(osa_status == OS_SUCCESS);	
	} 
	
	if (r_data){
		free(r_data);
	}
	
	catstudio_printf("ql_uart_callback end\r\n");	
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
			catstudio_printf("debug: %s", buffer);
			length += 128;						
		}else{
			memcpy(buffer, data+length, len-length);
			catstudio_printf("debug: %s", buffer);
			length = len;
		}
	}		
}

static void uart_task(void *ptr)
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

	ret = ql_uart_open(current_test_pot, QL_UART_BAUD_115200, QL_FC_NONE);
	if (ret) {
		catstudio_printf("open uart[%d] failed! \n", current_test_pot);
		return;
	}

	ql_uart_get_dcbconfig(current_test_pot, &dcb);
	catstudio_printf("[%s][%d]baudrate:%d, data_bit:%d, stop_bit:%d, parity_bit:%d, flow_ctrl:%d \r\n", __func__, __LINE__, dcb.baudrate, dcb.data_bit, dcb.stop_bit, dcb.parity_bit, dcb.flow_ctrl);
	dcb.baudrate = QL_UART_BAUD_115200;
	ql_uart_set_dcbconfig(current_test_pot, &dcb);
	
#ifdef SDK_UART_CB_RECV_DATA
	ql_uart_register_cb(current_test_pot, ql_uart_callback);
#else
	ql_uart_register_cb(current_test_pot, NULL);
#endif
	
	while(1){
		
#ifdef SDK_UART_CB_RECV_DATA
        memset(&uart_temp, 0, sizeof(uartParam));   
		catstudio_printf("wait uart data\n");
        status = OSAMsgQRecv(uartMsgQ, (UINT8 *)&uart_temp, sizeof(uartParam), OSA_SUSPEND);//recv data from callback
        
        if (status == OS_SUCCESS) {
            if (uart_temp.data) {

				write_bytes = ql_uart_write(current_test_pot, uart_temp.data, uart_temp.len);
				catstudio_printf("uart write data, write_bytes:%d,%d, data:%s\n", write_bytes, uart_temp.len, (char *)(uart_temp.data));			
				debugx(uart_temp.len, uart_temp.data);
				free(uart_temp.data);
            }
        }
#else	
		memset(r_data, 0, sizeof(r_data));
	 	read_bytes = ql_uart_read(current_test_pot, (unsigned char *)r_data, sizeof(r_data));
		catstudio_printf("uart read data, read_bytes:%d, data:%s\n", read_bytes, r_data);
		
		if (read_bytes > 0){
			write_bytes = ql_uart_write(current_test_pot, (unsigned char *)r_data, read_bytes);
			catstudio_printf("uart write data, write_bytes:%d, data:%s\n", write_bytes, r_data);			
		}else{
			OSATaskSleep(10);						
		}		
#endif	

	}
}
