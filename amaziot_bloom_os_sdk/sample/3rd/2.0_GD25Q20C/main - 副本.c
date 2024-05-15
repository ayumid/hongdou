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
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_spi_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_spi_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(1);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define sample_spi_sleep(x) OSATaskSleep((x)*200) // second

#define SAMPLE_SPI_STACK_SIZE 0x2000

#define SAMPLE_SPI_CS 17
#define SAMPLE_SPI_CLK 16
#define SAMPLE_SPI_MOSI 18
#define SAMPLE_SPI_MISO 19

#define W25Q_DUMMY_BYTE 0xFF

#define W25Q_WRITE_ENABLE		0x06
#define W25Q_WRITE_DISABLE		0x04
#define W25Q_STATUS_REG1		0x05
#define W25Q_STATUS_REG2		0x35
#define W25Q_PAGE_PROGRAM		0x02
#define W25Q_SECTOR_ERASE		0x20
#define W25Q_BLOCK32_ERASE		0x52
#define W25Q_BLOCK64_ERASE		0xD8
#define W25Q_POWER_DOWN			0xB9
#define W25Q_READ_DATA			0x03
#define W25Q_FAST_READ			0x0B
#define W25Q_RELEASE_POWER_DOWN	0xAB
#define W25Q_DEVICE_ID			0xAB
#define W25Q_MANUFACTURER_ID	0x90
#define W25Q_JEDEC_ID			0x9F
#define W25Q_UNIQUE_ID_NUM		0x4B

#define W25Q_CHIP_ERASE			0xC7

// Private variables ------------------------------------------------------------

static void *sample_spi_task_stack = NULL;

static OSTaskRef sample_spi_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_spi_task(void *ptr);

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

/* 是否启用模拟 SPI ,支持 mode 0 and mode 3*/
#define USE_SIMULATE_SPI		0

#define MAX_TIME_OUT	((uint32_t)0x1000)

static uint32_t W25Q_TimeOut = MAX_TIME_OUT;

static int sample_gpio_set(UINT32 GPIO_NUM, int val)
{
    GPIOReturnCode status = GPIORC_OK;
    
    status = GpioSetLevel(GPIO_NUM, val ? 1 : 0);
    if (status != GPIORC_OK)
    {
        sample_spi_uart_printf("status: 0x%lx", status);
        return -1;
    }
    
    return 0;
}

/************************************************
函数名称 ： Write_SPI_Byte
功    能 ： SPI写读一个字节
参    数 ： Byte ---- 数据
返 回 值 ： Byte ---- 数据
*************************************************/
uint8_t Write_SPI_Byte( uint8_t Byte )
{
	uint8_t i;
 
//	SPI_SCK(HIGH);
	sample_gpio_set(SAMPLE_SPI_CLK, 1);
	
	for(i = 0;i < 8;i++)
	{
//		SPI_SCK(LOW);
		sample_gpio_set(SAMPLE_SPI_CLK, 0);
		OSATaskSleep(1);       // 空等待
		
	#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
		
	#else
		if(Byte & 0x80)
		{
//			SPI_MOSI(HIGH);
			sample_gpio_set(SAMPLE_SPI_MOSI, 1);
			sample_gpio_set(16, 1);
		}
		else
		{
//			SPI_MOSI(LOW);
			sample_gpio_set(SAMPLE_SPI_MOSI, 0);
		}
		
	#endif
		Byte <<= 1;
		OSATaskSleep(1);       // 空等待
//		SPI_SCK(HIGH);
		sample_gpio_set(SAMPLE_SPI_CLK, 1);
		OSATaskSleep(1);       // 空等待
		Byte |= GpioGetLevel(SAMPLE_SPI_MISO);
	}
	
	return Byte;
}
 
/************************************************
函数名称 ： Read_SPI_Byte
功    能 ： SPI只读一个字节
参    数 ： 无
返 回 值 ： temp ---- 数据
*************************************************/
uint8_t Read_SPI_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;
 
//	SPI_SCK(HIGH);
	sample_gpio_set(SAMPLE_SPI_CLK, 1);
	
	for(i = 0;i < 8;i++)
	{
//		SPI_SCK(LOW);
		sample_gpio_set(SAMPLE_SPI_CLK, 0);
		OSATaskSleep(1);       // 空等待
		temp <<= 1;
		
	#if 1
		temp |= GpioGetLevel(SAMPLE_SPI_MISO);
		
	#else
		if(SPI_MISO)
		{
			temp++;
		}
		
	#endif
//		SPI_SCK(HIGH);
		sample_gpio_set(SAMPLE_SPI_CLK, 1);
		OSATaskSleep(1);       // 空等待
	}
	
	return temp;
}

/************************************************
函数名称 ： TimeOut_Callback
功    能 ： 等待超时回调函数
参    数 ： ErrorCode ---- 错误代号
返 回 值 ： 错误值 0
*************************************************/

#if (0 == USE_SIMULATE_SPI)
static uint8_t TimeOut_Callback( char ErrorCode )
{
	/* 等待超时后的处理,输出错误信息 */
	sample_spi_catstudio_printf("SPI 等待超时!	  EerrorCode = %d\n",ErrorCode);

	return 0;
}

#endif /* USE_SIMULATE_SPI */

/************************************************
函数名称 ： SPI_Flash_SendByte
功    能 ： 使用SPI发送/ 返回一个字节的数据
参    数 ： wData ---- 写数据
返 回 值 ： rData ---- 读数据
*************************************************/
static uint8_t SPI_Flash_SendByte( uint8_t wData )
{
	
//#if USE_SIMULATE_SPI
	return Write_SPI_Byte(wData);
	
//#else
//	W25Q_TimeOut = MAX_TIME_OUT;
//	
//	/* Wait for W25Q_SPIx Tx buffer empty */
//	while(SPI_I2S_GetFlagStatus(W25Q_SPIx, SPI_I2S_FLAG_TXE) == RESET)
//	{
//		if(0 == (W25Q_TimeOut--))
//			return TimeOut_Callback(0);
//	}

//	/* Send byte through the W25Q_SPIx peripheral */
//	SPI_I2S_SendData(W25Q_SPIx, wData);
//	
//	W25Q_TimeOut = MAX_TIME_OUT;

//	/* Wait for W25Q_SPIx data reception */
//	while(SPI_I2S_GetFlagStatus(W25Q_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
//	{
//		if(0 == (W25Q_TimeOut--))
//			return TimeOut_Callback(1);
//	}
//	
//	/* Return the byte read from the W25Q_SPIx bus */
//	return SPI_I2S_ReceiveData(W25Q_SPIx);
//		
//#endif /* USE_SIMULATE_SPI */
}

/************************************************
函数名称 ： W25Qxx_Busy_Wait
功    能 ： W25Qxx忙等待
参    数 ： 无
返 回 值 ： 无
*************************************************/
//static void W25Qxx_Busy_Wait(void)
//{
//	uint8_t flash_status = 0;
//	
//    W25Q_CS(LOW);
//	
//	SPI_Flash_SendByte(W25Q_STATUS_REG1);
//	
//	do
//	{
//		flash_status = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
//	}while(flash_status & BIT_BUSY);
//	
//    W25Q_CS(HIGH);
//}

/************************************************
函数名称 ： W25Qxx_Read_JEDECID
功    能 ： 读 W25QxxJEDEC_ID（制造商、类型、容量）
参    数 ： 无
返 回 值 ： temp[0] ---- JEDEC_ID
*************************************************/
uint32_t W25Qxx_Read_JEDECID(void)
{
	uint32_t temp[4] = {0};

//    W25Q_CS(LOW);
    sample_gpio_set(SAMPLE_SPI_CS, 0);
	
	SPI_Flash_SendByte(W25Q_JEDEC_ID);
	temp[1] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// 制造商
	temp[2] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// 类型
	temp[3] = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);		// 容量
	temp[0] = (temp[1] << 16) | (temp[2] << 8) | temp[3];
	
//    W25Q_CS(HIGH);
    sample_gpio_set(SAMPLE_SPI_CS, 1);
	
	return temp[0];
}

/************************************************
函数名称 ： W25Qxx_Read_Manufacturer_ID
功    能 ： 读 W25Qxx制造商 ID
参    数 ： 无
返 回 值 ： id_num ---- 制造商 ID
*************************************************/
uint16_t W25Qxx_Read_Manufacturer_ID(void)
{
	uint16_t id_num = 0;

//    W25Q_CS(LOW);
    sample_gpio_set(SAMPLE_SPI_CS, 0);
	
	SPI_Flash_SendByte(W25Q_MANUFACTURER_ID);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(0x00);
	id_num |= SPI_Flash_SendByte(W25Q_DUMMY_BYTE) << 8;
	id_num |= SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	
//    W25Q_CS(HIGH);
    sample_gpio_set(SAMPLE_SPI_CS, 1);
	
	return id_num;
}

/************************************************
函数名称 ： W25Qxx_Read_DeviceID
功    能 ： 读 W25Qxx设备 ID
参    数 ： 无
返 回 值 ： id_num ---- 设备 ID
*************************************************/
uint8_t W25Qxx_Read_DeviceID(void)
{
	uint8_t id_num = 0;

//    W25Q_CS(LOW);
    sample_gpio_set(SAMPLE_SPI_CS, 0);
	
	SPI_Flash_SendByte(W25Q_DEVICE_ID);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	id_num = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
	
//    W25Q_CS(HIGH);
    sample_gpio_set(SAMPLE_SPI_CS, 1);
	
	return id_num;
}

/************************************************
函数名称 ： W25Qxx_Page_Program
功    能 ： W25Qxx页编程（调用本函数写入数据前需要先擦除扇区）
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
//void W25Qxx_Page_Program( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
//{
//	W25Qxx_Write_Enable();
//	
//    W25Q_CS(LOW);
//	
//	SPI_Flash_SendByte(W25Q_PAGE_PROGRAM);
//	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
//	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
//	SPI_Flash_SendByte(Address & 0xFF);
//	
//	if(Len > W25Q_PAGE_SIZE)
//	{
//		Len = W25Q_PAGE_SIZE;
//		W25Q_DEBUG_PRINTF("W25Qxx Page Program data too large!\n"); 
//	}
//	while(Len--)
//	{
//		SPI_Flash_SendByte(*pBuffer);
//		pBuffer++;
//	}
//	
//    W25Q_CS(HIGH);
//	
//	W25Qxx_Busy_Wait();
//}

/************************************************
函数名称 ： W25Qxx_Write_Flash
功    能 ： 写 W25Qxx闪存数据（调用本函数写入数据前需要先擦除扇区）
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
//void W25Qxx_Write_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
//{
//	uint8_t NumOfPage = 0, NumOfSingle = 0;
//	uint8_t Addr = 0, count = 0, temp = 0;
//	
//	/* mod运算求余，若 Address是 W25Q_PAGE_SIZE整数倍，运算结果 Addr值为 0 */
//	Addr = Address % W25Q_PAGE_SIZE;
//	
//	/* 差count个数据值，刚好可以对齐到页地址 */
//	count = W25Q_PAGE_SIZE - Addr;
//	
//	/* 计算出要写多少整数页 */
//	NumOfPage =  Len / W25Q_PAGE_SIZE;
//	
//	/* 计算出剩余不满一页的字节数 */
//	NumOfSingle = Len % W25Q_PAGE_SIZE;
//	
//	/* Addr = 0,则 Address刚好按页对齐 */
//	if(0 == Addr)
//	{
//		/* Len <= W25Q_PAGE_SIZE */
//		if(0 == NumOfPage) 
//		{
//			/* 不到一页 or 刚好一页 */
//			W25Qxx_Page_Program(pBuffer, Address, Len);
//		}
//		else /* Len > W25Q_PAGE_SIZE */
//		{ 
//			/* 先把整数页的都写了 */
//			while(NumOfPage--)
//			{
//				W25Qxx_Page_Program(pBuffer, Address, W25Q_PAGE_SIZE);
//				Address += W25Q_PAGE_SIZE;
//				pBuffer += W25Q_PAGE_SIZE;
//			}
//			/* 若有多余的不满一页的数据，下一页把它写完 */
//			if(NumOfSingle != 0)
//			{
//				W25Qxx_Page_Program(pBuffer, Address, NumOfSingle);
//			}
//		}
//	}
//	/* 若地址与 W25Q_PAGE_SIZE不对齐  */
//	else 
//	{
//		/* Len < W25Q_PAGE_SIZE */
//		if(0 == NumOfPage)
//		{
//			/* 当前页剩余的 count个位置比 NumOfSingle小，一页写不完 */
//			if(NumOfSingle > count) 
//			{
//				/* 先写满当前页 */
//				W25Qxx_Page_Program(pBuffer, Address, count);
//						
//				temp = NumOfSingle - count;
//				Address += count;
//				pBuffer += count;
//				/* 再写剩余的数据 */
//				W25Qxx_Page_Program(pBuffer, Address, temp);
//			}
//			else /* 当前页剩余的 count个位置能写完 NumOfSingle个数据 */
//			{
//				W25Qxx_Page_Program(pBuffer, Address, Len);
//			}
//		}
//		else /* Len > W25Q_PAGE_SIZE */
//		{
//			/* 地址不对齐多出的 count分开处理，不加入这个运算 */
//			Len -= count;
//			NumOfPage =  Len / W25Q_PAGE_SIZE;
//			NumOfSingle = Len % W25Q_PAGE_SIZE;
//			
//			if(count != 0)
//			{
//				/* 先写完count个数据，为的是让下一次要写的地址对齐 */
//				W25Qxx_Page_Program(pBuffer, Address, count);
//					
//				/* 接下来就重复地址对齐的情况 */
//				Address +=  count;
//				pBuffer += count;
//			}
//			
//			/* 把整数页都写了 */
//			while(NumOfPage--)
//			{
//				W25Qxx_Page_Program(pBuffer, Address, W25Q_PAGE_SIZE);
//				Address +=  W25Q_PAGE_SIZE;
//				pBuffer += W25Q_PAGE_SIZE;
//			}
//			/* 若有多余的不满一页的数据，把它写完*/
//			if(NumOfSingle != 0)
//			{
//				W25Qxx_Page_Program(pBuffer, Address, NumOfSingle);
//			}
//		}
//	}
//}

/************************************************
函数名称 ： W25Qxx_Read_Flash
功    能 ： 读 W25Qxx闪存数据
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
//void W25Qxx_Read_Flash( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
//{
//    W25Q_CS(LOW);
//	
//	SPI_Flash_SendByte(W25Q_READ_DATA);
//	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
//	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
//	SPI_Flash_SendByte(Address & 0xFF);
//	
//	/* 读取数据 */
//	while(Len--)
//	{
//		*pBuffer = SPI_Flash_SendByte(W25Q_DUMMY_BYTE);
//		pBuffer++;
//	}
//	
//    W25Q_CS(HIGH);
//}

/************************************************
函数名称 ： W25Qxx_Sector_Erase
功    能 ： FLASH扇区擦除
参    数 ： Address ---- 擦除地址
返 回 值 ： 无
*************************************************/
//void W25Qxx_Sector_Erase( uint32_t Address )
//{
//	W25Qxx_Write_Enable();
//	
//    W25Q_CS(LOW);
//	
//	SPI_Flash_SendByte(W25Q_SECTOR_ERASE);
//	SPI_Flash_SendByte((Address & 0xFF0000) >> 16);
//	SPI_Flash_SendByte((Address & 0xFF00) >> 8);
//	SPI_Flash_SendByte(Address & 0xFF);

//    W25Q_CS(HIGH);
//	
//	W25Qxx_Busy_Wait();
//}

/************************************************
函数名称 ： W25Qxx_Chip_Erase
功    能 ： FLASH整片擦除(为了安全起见，若要调用，请先调用 W25Qxx_Write_Enable函数)
参    数 ： 无
返 回 值 ： 无
*************************************************/
//void W25Qxx_Chip_Erase(void)
//{
//    W25Q_CS(LOW);
//	
//	SPI_Flash_SendByte(W25Q_CHIP_ERASE);
//	
//    W25Q_CS(HIGH);
//	
//	W25Qxx_Busy_Wait();
//}

/************************************************
函数名称 ： W25Qxx_Write_Enable
功    能 ： W25Qxx写使能
参    数 ： 无
返 回 值 ： 无
*************************************************/
//void W25Qxx_Write_Enable(void)
//{
//	uint8_t flash_status = 0;

//    W25Q_CS(LOW);
//    SPI_Flash_SendByte(W25Q_WRITE_ENABLE);
//    W25Q_CS(HIGH);
//		
//    W25Q_CS(LOW);
//	/* 等待写使能位置 1 */
//	do
//	{
//		flash_status = SPI_Flash_SendByte(W25Q_STATUS_REG1);
//	}while(!(flash_status & BIT_WEL));
//    W25Q_CS(HIGH);
//}

/************************************************
函数名称 ： W25Qxx_Write_Disable
功    能 ： W25Qxx写失能
参    数 ： 无
返 回 值 ： 无
*************************************************/
//void W25Qxx_Write_Disable(void)
//{
//	uint8_t flash_status = 0;

//    W25Q_CS(LOW);
//    SPI_Flash_SendByte(W25Q_WRITE_DISABLE);
//    W25Q_CS(HIGH);
//	
//    W25Q_CS(LOW);
//	/* 等待写使能清 0 */
//	do
//	{
//		flash_status = SPI_Flash_SendByte(W25Q_STATUS_REG1);
//	}while(!(flash_status & BIT_WEL));
//    W25Q_CS(HIGH);

//}

/************************************************
函数名称 ： W25Qxx_Power_Down
功    能 ： W25Qxx掉电
参    数 ： 无
返 回 值 ： 无
*************************************************/
//void W25Qxx_Power_Down(void)
//{
//    W25Q_CS(LOW);
//    SPI_Flash_SendByte(W25Q_POWER_DOWN);
//    W25Q_CS(HIGH);
//}

/************************************************
函数名称 ： W25Qxx_Release_PowerDown
功    能 ： W25Qxx唤醒
参    数 ： 无
返 回 值 ： 无
*************************************************/
//void W25Qxx_Release_PowerDown(void)
//{
//    W25Q_CS(LOW);
//    SPI_Flash_SendByte(W25Q_RELEASE_POWER_DOWN);
//    W25Q_CS(HIGH);
//}

/************************************************
函数名称 ： W25Qxx_Config
功    能 ： W25Qxx配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Config(void)
{
	
//	W25Q_CS(HIGH);
//	SPI_SCK(HIGH);
//	SPI_MOSI(HIGH);
	sample_gpio_set(SAMPLE_SPI_CS, 1);
	sample_gpio_set(SAMPLE_SPI_CLK, 1);
	sample_gpio_set(SAMPLE_SPI_MOSI, 1);

}

/************************************************
函数名称 ： W25Qxx_Init
功    能 ： W25Qxx初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void W25Qxx_Init(void)
{
	uint32_t FlashID = 0;
	
	W25Qxx_Config();
	
//#if(_W25Q_DEBUG)
	FlashID = W25Qxx_Read_JEDECID();
	sample_spi_catstudio_printf("FlashID is 0x%X,Manufacturer Device ID is 0x%X\r\n", FlashID, W25Qxx_Read_DeviceID());
//	if(FlashID != JEDEC_ID)
//	{
//		/* 读取错误处理 */
//		W25Q_DEBUG_PRINTF("SPI read-write Error, please check the connection between MCU and SPI Flash\n");
//	}
//	else
//	{
		/* 读取成功处理 */
//		W25Q_DEBUG_PRINTF("SPI read-write succeed\n");
		
//		uint8_t Tx_buff[] = "FLASH读写测试实验\r\n";
//		uint8_t Rx_buff[] = "FLASH读写测试实验\r\n";

//		W25Qxx_Sector_Erase(0x0100);
//		W25Qxx_Write_Flash(Tx_buff, 0x0100, (sizeof(Tx_buff) / sizeof(*(Tx_buff))));
//		W25Qxx_Read_Flash(Rx_buff, 0x0100, (sizeof(Tx_buff) / sizeof(*(Tx_buff))));
//		W25Q_DEBUG_PRINTF("读出的数据：%s\n", Rx_buff);
//	}

//#endif /* _W25Q_DEBUG */
}


/*---------------------------- END OF FILE ----------------------------*/

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

    sample_spi_task_stack = malloc(SAMPLE_SPI_STACK_SIZE);

    ret = OSATaskCreate(&sample_spi_task_ref, sample_spi_task_stack, SAMPLE_SPI_STACK_SIZE, 88, "sample_spi_task", sample_spi_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

static void sample_spi_task(void *ptr)
{
    int ret = 0;
    unsigned char writeBuf[30] = {0};
    unsigned char readBuf[30] = {0};
    GPIOReturnCode status = GPIORC_OK;
    int highlow = 0;
    GPIOConfiguration config = {0};
//    ret = ql_spi_init(QL_SPI_PORT0, QL_SPI_MODE3, QL_SPI_CLK_812_5KHZ);
//    sample_spi_catstudio_printf("ql_spi_init ret %d", ret);
    status = GpioSetDirection(SAMPLE_SPI_CS, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        sample_spi_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(SAMPLE_SPI_CLK, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        sample_spi_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(SAMPLE_SPI_MOSI, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        sample_spi_uart_printf("status: 0x%lx", status);
    }
    config.pinDir = GPIO_IN_PIN;
//    config.pinEd = GPIO_RISE_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;
//    config.isr = sample_gpio_irq_handler;
    GpioInitConfiguration(SAMPLE_SPI_MISO,config);
//    W25Qxx_Init();

    while (1)
    {
        W25Qxx_Init();

//        while(1)
//        {
//            OSATaskSleep(200);
//            sample_spi_uart_printf("%s: highlow: %d\n", __FUNCTION__, highlow);    
//            if (highlow)
//            {
//                highlow = 0;
//                sample_gpio_set(16, 1);
//                sample_gpio_set(17, 1);
//                sample_gpio_set(18, 1);
//                sample_gpio_set(19, 1);

//            }
//            else
//            {
//                highlow = 1;
//                sample_gpio_set(16, 0);
//                sample_gpio_set(17, 0);
//                sample_gpio_set(18, 0);
//                sample_gpio_set(19, 0);
//            }

//        }
//        memset(writeBuf, 0x00, sizeof(writeBuf));
//        memset(readBuf, 0x00, sizeof(readBuf));

//        writeBuf[0] = 0x9F;

//        ret = ql_spi_write_read(QL_SPI_PORT0, readBuf, writeBuf, 1);
//        sample_spi_catstudio_printf("ql_spi_write_read ret %d, readBuf %02X,%02X,%02X\n", ret, readBuf[0], readBuf[1], readBuf[2]);
//        ret = ql_spi_write(QL_SPI_PORT0, writeBuf, 1);
//        sample_spi_catstudio_printf("ql_spi_write_read ret %d, readBuf %02X\n", ret, writeBuf[0]);
//        ret = ql_spi_read(QL_SPI_PORT0, readBuf, 3);
//        sample_spi_catstudio_printf("ql_spi_write_read ret %d, readBuf %02X,%02X,%02X\n", ret, readBuf[0], readBuf[1], readBuf[2]);
//        OSATaskSleep(200);
    }
}

// End of file : main.c 2023-5-17 9:22:29 by: zhaoning 

