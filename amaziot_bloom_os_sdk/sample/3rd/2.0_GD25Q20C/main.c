//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
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
#include "osa_mem.h"
#include "ql_spi_api.h"

#include "drv_gd25q20.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_gd25q20_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_gd25q20_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(1);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define sample_gd25q20_sleep(x) OSATaskSleep((x)*200) // second

#define SAMPLE_GD25Q20_STACK_SIZE           2048

// Private variables ------------------------------------------------------------

static void *sample_gd25q20_task_stack = NULL;

static OSTaskRef sample_gd25q20_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_gd25q20_task(void *ptr);

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

    sample_gd25q20_task_stack = malloc(SAMPLE_GD25Q20_STACK_SIZE);

    ret = OSATaskCreate(&sample_gd25q20_task_ref, sample_gd25q20_task_stack, SAMPLE_GD25Q20_STACK_SIZE, 88, "sample_gd25q20_task", sample_gd25q20_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

static void sample_gd25q20_task(void *ptr)
{
    int ret = 0;
    uint32_t identification = 0;
//    unsigned char writeBuf[30] = {0};
//    unsigned char readBuf[30] = {0};

//    ret = ql_spi_init(QL_SPI_PORT0, QL_SPI_MODE3, QL_SPI_CLK_812_5KHZ);
//    sample_gd25q20_catstudio_printf("ql_spi_init ret %d", ret);

    drv_gd25q20_init();

    identification = drv_gd25q20_read_identification();
    sample_gd25q20_uart_printf("identification is 0x%X, Device id is 0x%X, Manufacturer Device ID is 0x%X", 
                               drv_gd25q20_read_identification(), drv_gd25q20_read_device_id(), drv_gd25q20_read_manufacturer_id());
    
//    while(1)
//    {
//        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, 0);
//        sample_gd25q20_uart_printf("low");
//        OSATaskSleep(5*200);
//        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, 1);
//        sample_gd25q20_uart_printf("high");
//        OSATaskSleep(5*200);
//    }
    if(identification != RDID)
    {
        /* 读取错误处理 */
        sample_gd25q20_uart_printf("SPI read-write Error, please check the connection between MCU and SPI Flash\n");
    }
    else
    {
        //读取成功处理
        char tx_buff1[64] = "abcdefghigklmnopqrstuvwxyz0123456789";
        char rx_buff1[64] = {0};
        char tx_buff2[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ9876543210";
        char rx_buff2[64] = {0};
        int i = 0;
        //测试跨sector写，并且读出数据，写两次，第二次保留第一次部分数据，证明数据擦除，写入正常
        drv_gd25q20_write((UINT8*)tx_buff1, 8181, strlen(tx_buff1));//从8181地址开始写数据，需要写第二和第三个扇区
        drv_gd25q20_read((UINT8*)rx_buff1, 8181, strlen(tx_buff1));
        sample_gd25q20_uart_printf("read flash:%s", rx_buff1);

        if(!strncmp(tx_buff1, rx_buff1, strlen(tx_buff1)))
        {
            sample_gd25q20_uart_printf("SPI read-write succeed 1");
        }
        //验证驱动擦除扇区时，可以保留之前有效内容
        drv_gd25q20_write((UINT8*)tx_buff2, 8186, strlen(tx_buff2));//从8186地址开始写数据，需要写第二和第三个扇区，同时不能擦掉8181 - 8186的5字节数据
        drv_gd25q20_read((UINT8*)rx_buff2, 8181, strlen(tx_buff2) + 5);
        sample_gd25q20_uart_printf("read flash:%s", rx_buff2);

        if(!strncmp(rx_buff2, "abcdeABCDEFGHIJKLMNOPQRSTUVWXYZ9876543210", strlen("abcdeABCDEFGHIJKLMNOPQRSTUVWXYZ9876543210")))
        {
            sample_gd25q20_uart_printf("SPI read-write succeed 2");
        }
    }
//    memset(writeBuf, 0x00, sizeof(writeBuf));
//    memset(readBuf, 0x00, sizeof(readBuf));

//    writeBuf[0] = 0x9F;
    while (1)
    {
//        ret = ql_spi_write_read(QL_SPI_PORT0, readBuf, writeBuf, 1);
//        sample_gd25q20_catstudio_printf("ql_spi_write_read ret %d, readBuf %02X,%02X,%02X\n", ret, readBuf[0], readBuf[1], readBuf[2]);
//        ret = ql_spi_write(QL_SPI_PORT0, writeBuf, 1);
//        sample_gd25q20_catstudio_printf("ql_spi_write_read ret %d, readBuf %02X\n", ret, writeBuf[0]);
//        ret = ql_spi_read(QL_SPI_PORT0, readBuf, 3);
//        sample_gd25q20_catstudio_printf("ql_spi_write_read ret %d, readBuf %02X,%02X,%02X\n", ret, readBuf[0], readBuf[1], readBuf[2]);
        OSATaskSleep(5 * 200);
    }
}

// End of file : main.c 2023-5-17 9:22:29 by: zhaoning 

