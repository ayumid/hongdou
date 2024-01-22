#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"

//#define api_printf(fmt, args...) do { RTI_LOG("[libapi]"fmt, ##args); } while(0)
#define sdk_uart_printf(fmt, args...) do { RTI_LOG("[libapi]"fmt, ##args); } while(0)
    
void test_api(void )
{
    sdk_uart_printf("test api lib printf\n");
}
