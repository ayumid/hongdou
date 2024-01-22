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

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

#include "lib_llcc68_hal.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_lora_rtx_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_lora_rtx_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_gpio_sleep(x) OSATaskSleep((x) * 200)//second

#define LORA_RTX_TASK_STACK_SIZE     1024 * 32

typedef struct _LORA_DEMO_DATA_MSG_S
{
    UINT32 len;//消息长度
    UINT8* data;
}LORA_DEMO_DATA_MSG_T, *P_LORA_DEMO_DATA_MSG_T;

// Private variables ------------------------------------------------------------

static UINT8 lora_rtx_task_stack[LORA_RTX_TASK_STACK_SIZE];

static OSTaskRef lora_rtx_task_ref = NULL;
static void lora_rtx_task(void *ptr);

// Public variables -------------------------------------------------------------

extern OSMsgQRef lora_data_msgq;

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

extern UINT32 GPIO_MFPR_ADDR(UINT32 n);

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

    ret = OSATaskCreate(&lora_rtx_task_ref, lora_rtx_task_stack, LORA_RTX_TASK_STACK_SIZE, 151, "lora_rtx_task", lora_rtx_task, NULL);
    ASSERT(ret == OS_SUCCESS);

    sample_lora_rtx_uart_printf("Phase2Inits_exit\n");
}

//static int sample_gpio_set(UINT32 GPIO_NUM, int val)
//{
//    GPIOReturnCode status = GPIORC_OK;
//    
//    status = GpioSetLevel(GPIO_NUM, val ? 1 : 0);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//        return -1;
//    }
//    
//    return 0;
//}

static void lora_rtx_task(void *ptr)
{        
    int highlow = 0;
    GPIOReturnCode status = GPIORC_OK;
    LORA_DEMO_DATA_MSG_T rcv_data = {0};
//    GPIOConfiguration config;
    
    //VCXO_OUT需要重映射为gpio126,寄存器地址仔细看域格给的excel文档
    *((volatile unsigned long*)0xD401E0D8) = 0xd0c2;
    sample_lora_rtx_uart_printf("VCXO_OUT value %lx\n", *((volatile UINT32 *)0xD401E0D8));

/**********GPIO输出***********/        
//    status = GpioSetDirection(NSS_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    status = GpioSetDirection(SCK_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    status = GpioSetDirection(MOSI_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    status = GpioSetDirection(MISO_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    status = GpioSetDirection(POR_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    status = GpioSetDirection(BUSY_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    status = GpioSetDirection(DIO1_PIN, GPIO_OUT_PIN);
//    if (status != GPIORC_OK)
//    {
//        sample_lora_rtx_uart_printf("status: 0x%lx", status);
//    }
//    ql_spi_init(QL_SPI_PORT0, QL_SPI_MODE3, QL_SPI_CLK_812_5KHZ);
    //初始化lora模块，一次最多接收200字节，发送端需要设置不能超200，超过分包发送
    dev_lora_module_init();
    //初始化lora dio1中断，初始化资源，lora接收发送任务
    dev_lora_dio1_task_init();
    sample_lora_rtx_uart_printf("lora init ok\n");

    while(1)
    {
//        SX1262Hal_SPIWrite(0x55);
        //这里实现了发送任意长度的字节，可以使用另一个模块来接收
        sample_gpio_sleep(3);
        dev_lora_module_packet_init(strlen("1234567980"));
        dev_lora_send("1234567980", strlen("1234567980"));
        sample_lora_rtx_uart_printf("send a\n");
        sample_gpio_sleep(3);
        dev_lora_module_packet_init(strlen("abcdefghijklmnopqrstuvwxyz"));
        dev_lora_send("abcdefghijklmnopqrstuvwxyz", strlen("abcdefghijklmnopqrstuvwxyz"));
        sample_lora_rtx_uart_printf("send b\n");
        
        //这里实现了接收其它模块发来的数据
//        status = OSAMsgQRecv(lora_data_msgq, (void *)&rcv_data, sizeof(LORA_RCV_DATA_MSG_T), OSA_SUSPEND);

//        if (status == OS_SUCCESS)
//        {
//            if(NULL != rcv_data.data)
//            {
//                sample_lora_rtx_uart_printf("lora data len:%d %s\n", rcv_data.len, rcv_data.data);
//                free(rcv_data.data);
//                rcv_data.data = NULL;
//            }
//        }
    }

}

// End of file : main.c 2023-5-16 9:01:12 by: zhaoning 

