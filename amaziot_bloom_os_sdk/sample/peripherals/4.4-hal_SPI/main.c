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

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sdk_uart_printf(fmt, args...) \
	do                                \
	{                                 \
		sdklogConfig(1);              \
		sdkLogPrintf(fmt, ##args);    \
	} while (0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) \
	do                                 \
	{                                  \
		sdklogConfig(0);               \
		sdkLogPrintf(fmt, ##args);     \
	} while (0)

#define sleep(x) OSATaskSleep((x)*200) // second

#define SPI_STACK_SIZE 0x2000
static void *spi_task_stack = NULL;

static OSTaskRef spi_task_ref = NULL;

static void spi_task(void *ptr);

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

	spi_task_stack = malloc(SPI_STACK_SIZE);

	ret = OSATaskCreate(&spi_task_ref, spi_task_stack, SPI_STACK_SIZE, 88, "spi_task", spi_task, NULL);
	ASSERT(ret == OS_SUCCESS);
}

static void spi_task(void *ptr)
{
	int ret = 0;
	unsigned char writeBuf[30] = {0};
	unsigned char readBuf[30] = {0};

	ret = ql_spi_init(QL_SPI_PORT0, QL_SPI_MODE3, QL_SPI_CLK_3_25MHZ);
	catstudio_printf("ql_spi_init ret %d", ret);

	while (1)
	{

		memset(writeBuf, 0x00, sizeof(writeBuf));
		memset(readBuf, 0x00, sizeof(readBuf));

		writeBuf[0] = 0xA1;
		writeBuf[1] = 0xA2;
		writeBuf[2] = 0xA3;
		writeBuf[3] = 0xA4;
		writeBuf[4] = 0xA5;
		writeBuf[5] = 0xA6;
		writeBuf[6] = 0xA7;
		writeBuf[7] = 0xA8;
		writeBuf[8] = 0xA9;
		writeBuf[9] = 0xAA;

		ret = ql_spi_write_read(QL_SPI_PORT0, readBuf, writeBuf, strlen((char *)writeBuf));
		catstudio_printf("ql_spi_write_read ret %d, readBuf %02X,%02X,%02X\n", ret, readBuf[0], readBuf[1], readBuf[2]);

		OSATaskSleep(5 * 200);
	}
}
