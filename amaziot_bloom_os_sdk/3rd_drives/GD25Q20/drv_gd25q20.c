//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_gd25q20.c
// Auther      : zhaoning
// Version     :
// Date : 2024-5-11
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-11
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_gd25q20.h"
#include "drv_3rd.h"

// Private defines / typedefs ---------------------------------------------------

/*
 * MODE 0:CPOL=0，CPHA=0：此时空闲态时，SCLK处于低电平，数据采样是在第 1个边沿， \
 *          也就是 SCLK由低电平到高电平的跳变，所以数据采样是在上升沿，数据发送是在下降沿。

 * MODE 1:CPOL=0，CPHA=1：此时空闲态时，SCLK处于低电平，数据发送是在第 2个边沿， \
 *          也就是 SCLK由高电平到低电平的跳变，所以数据采样是在下降沿，数据发送是在上升沿。

 * MODE 2:CPOL=1，CPHA=0：此时空闲态时，SCLK处于高电平，数据采集是在第 1个边沿， \
 *          也就是 SCLK由高电平到低电平的跳变，所以数据采集是在下降沿，数据发送是在上升沿。

 * MODE 3:CPOL=1，CPHA=1：此时空闲态时，SCLK处于高电平，数据发送是在第 2个边沿， \
 *          也就是 SCLK由低电平到高电平的跳变，所以数据采集是在上升沿，数据发送是在下降沿。
*/

//The GD25Q20C features a serial peripheral interface on 4 signals bus: Serial Clock (SCLK), Chip Select (CS#)
//SerialData Input (SI) and Serial Data Output (SO)
//Both SPI bus mode 0 and 3 are supported
//Input data is latched on the risingedge of SCLK and data shifts out on the falling edge of SCLK
#define _CPOL     1
#define _CPHA     1

#define WAIT_TIME                   2

// Private variables ------------------------------------------------------------

static uint32_t _TimeOut = MAX_TIME_OUT;
static uint8_t gd25q20_buffer[DRV_GD25Q20_SOCTOR_SIZE];

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

static void drv_gd25q20_delay_us(uint32_t count)
{
    while(count)
    {
        count--;
    }
}

/**
  * Function    : drv_gd25q20_gpio_set
  * Description : 模拟SPI设置IO输出电平
  * Input       : num    引脚号
  *               val         0 低电平，1 高电平
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int drv_gd25q20_gpio_set(UINT32 num, int val)
{
    GPIOReturnCode status = GPIORC_OK;
    
    status = GpioSetLevel(num, val ? DRV_GD25Q20_GPIO_HIGH : DRV_GD25Q20_GPIO_LOW);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
        return -1;
    }
    
    return 0;
}

#if (0 == _CPOL && 0 == _CPHA) /* ----- MODE 0 ----- */
/**
  * Function    : drv_gd25q20_byte_wr
  * Description : SPI写读一个字节
  * Input       : byte ---- 数据
  *               
  * Output      : byte ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_wr(uint8_t byte)
{
    uint8_t i;
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        
#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
        
#else
        if(byte & 0x80)
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_HIGH);
        }
        else
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_LOW);
        }
        
#endif
        byte <<= 1;
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        byte |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
    }
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
    
    return byte;
}

/**
  * Function    : drv_gd25q20_byte_rd
  * Description : SPI只读一个字节
  * Input       : 
  *               
  * Output      : data ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_rd(void)
{
    uint8_t i;
    uint8_t temp = 0;
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        temp <<= 1;
        
#if 1
        temp |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
        
#else
        if(GpioGetLevel(DRV_GD25Q20_SPI_MISO))
        {
            temp++;
        }
        
#endif
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
    }

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);

    return temp;
}

#elif (0 == _CPOL && 1 == _CPHA) /* ----- MODE 1 ----- */
/**
  * Function    : drv_gd25q20_byte_wr
  * Description : SPI写读一个字节
  * Input       : byte ---- 数据
  *               
  * Output      : byte ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_wr(uint8_t byte)
{
    uint8_t i;

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        
#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
        
#else
        if(byte & 0x80)
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_HIGH);
        }
        else
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_LOW);
        }
        
#endif
        byte <<= 1;
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        byte |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
    }
    
    return byte;
}

/**
  * Function    : drv_gd25q20_byte_rd
  * Description : SPI只读一个字节
  * Input       : 
  *               
  * Output      : data ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_rd(void)
{
    uint8_t i;
    uint8_t temp = 0;

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        temp <<= 1;
        
#if 1
        temp |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
        
#else
        if(GpioGetLevel(DRV_GD25Q20_SPI_MISO))
        {
            temp++;
        }
        
#endif
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
    }
    
    return temp;
}

#elif (1 == _CPOL && 0 == _CPHA) /* ----- MODE 2 ----- */
/**
  * Function    : drv_gd25q20_byte_wr
  * Description : SPI写读一个字节
  * Input       : byte ---- 数据
  *               
  * Output      : byte ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_wr(uint8_t byte)
{
    uint8_t i;
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        
#if 0
        SPI_MOSI((Byte & 0x80) >> 7);
        
#else
        if(byte & 0x80)
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_HIGH);
        }
        else
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_LOW);
        }
        
#endif
        byte <<= 1;
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        byte |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
    }
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
    
    return byte;
}

/**
  * Function    : drv_gd25q20_byte_rd
  * Description : SPI只读一个字节
  * Input       : 
  *               
  * Output      : data ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_rd(void)
{
    uint8_t i;
    uint8_t temp = 0;
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        temp <<= 1;
        
#if 1
        temp |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
        
#else
        if(GpioGetLevel(DRV_GD25Q20_SPI_MISO))
        {
            temp++;
        }
        
#endif
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
    }

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);

    return temp;
}

#elif (1 == _CPOL && 1 == _CPHA) /* ----- MODE 3 ----- */
/**
  * Function    : drv_gd25q20_byte_wr
  * Description : SPI写读一个字节
  * Input       : byte ---- 数据
  *               
  * Output      : byte ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_wr(uint8_t byte)
{
    uint8_t i = 0;
 
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
    
    for(i = 0; i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        
        if(byte & 0x80)
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_HIGH);
        }
        else
        {
            drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_LOW);
        }
        
        byte <<= 1;
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        byte |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
    }
    
    return byte;
}

/**
  * Function    : drv_gd25q20_byte_rd
  * Description : SPI只读一个字节
  * Input       : 
  *               
  * Output      : data ---- 数据
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_byte_rd(void)
{
    uint8_t i = 0;
    uint8_t data = 0;
 
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
    
    for(i = 0;i < 8;i++)
    {
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_LOW);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
        data <<= 1;
        
#if 1
        data |= GpioGetLevel(DRV_GD25Q20_SPI_MISO);
        
#else
        if(GpioGetLevel(DRV_GD25Q20_SPI_MISO))
        {
            temp++;
        }
        
#endif
        drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
        drv_gd25q20_delay_us(WAIT_TIME);       // 空等待
    }
    
    return data;
}


#endif /* SPI MODE */

#if (0 == USE_SIMULATE_SPI)
static uint8_t TimeOut_Callback( char ErrorCode )
{
    /* 等待超时后的处理,输出错误信息 */
    drv_3rd_uart_printf("SPI 等待超时!      EerrorCode = %d\n",ErrorCode);

    return 0;
}

#endif /* USE_SIMULATE_SPI */

/**
  * Function    : drv_gd25q20_read_reg1
  * Description : 读reg1
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
unsigned char drv_gd25q20_read_reg1(void)
{
    unsigned char reg = 0;
 
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    drv_gd25q20_byte_wr(DRV_GD25Q20_STATUS_REG1);     //读取状态；
    reg = drv_gd25q20_byte_rd();
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
 
    return reg;
}

/************************************************
函数名称 ： drv_gd25q20_busy_wait
功    能 ： GD25Q20 忙等待
参    数 ： 无
返 回 值 ： 无
*************************************************/
static void drv_gd25q20_busy_wait(void)
{
    while(drv_gd25q20_read_reg1() & BIT_BUSY);
}

/**
  * Function    : drv_gd25q20_read_identification
  * Description : 读 GD25Q20 JEDEC_ID（制造商、类型、容量）
  * Input       : 
  *               
  * Output      : temp[0] ---- JEDEC_ID
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint32_t drv_gd25q20_read_identification (void)
{
    uint32_t identification[4] = {0};

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_JEDEC_ID);
    identification[1] = drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);        // 制造商
    identification[2] = drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);        // 类型
    identification[3] = drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);        // 容量
    identification[0] = (identification[1] << 16) | (identification[2] << 8) | identification[3];
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    
    return identification[0];
}

/**
  * Function    : drv_gd25q20_read_manufacturer_id
  * Description : 读 GD25Q20 制造商 ID
  * Input       : 
  *               
  * Output      : id_num ---- 制造商 ID
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint16_t drv_gd25q20_read_manufacturer_id(void)
{
    uint16_t id = 0;

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_MANUFACTURER_ID);
    drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    drv_gd25q20_byte_wr(0x00);
    id |= drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE) << 8;
    id |= drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    
    return id;
}

/**
  * Function    : drv_gd25q20_read_device_id
  * Description : 读 GD25Q20 设备 ID
  * Input       : 
  *               
  * Output      : id_num ---- 设备 ID
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
uint8_t drv_gd25q20_read_device_id(void)
{
    uint8_t id = 0;

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_DEVICE_ID);
    drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    id = drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    
    return id;
}

/**
  * Function    : drv_gd25q20_wel_wait
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
//void drv_gd25q20_wel_wait(void)
//{
//    while(drv_gd25q20_read_reg1() & BIT_WEL);
//}

/**
  * Function    : drv_gd25q20_write_enable
  * Description : 写使能
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_write_enable(void)
{
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    drv_gd25q20_byte_wr(DRV_GD25Q20_WRITE_ENABLE);
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);

//    drv_gd25q20_wel_wait();

}

/**
  * Function    : drv_gd25q20_write_disable
  * Description : 写失能
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_write_disable(void)
{
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    drv_gd25q20_byte_wr(DRV_GD25Q20_WRITE_DISABLE);
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);

}

/**
  * Function    : drv_gd25q20_write_page
  * Description : 页编程（调用本函数写入数据前需要先擦除扇区）
  * Input       : pbuf ---- 数据
  *               addr ---- 地址
                  len ---- 长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_write_page(uint8_t* pbuf, uint32_t addr, uint16_t len)
{
    drv_gd25q20_write_enable();
    drv_gd25q20_busy_wait();

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_PAGE_PROGRAM);
    drv_gd25q20_byte_wr((addr & 0xFF0000) >> 16);
    drv_gd25q20_byte_wr((addr & 0xFF00) >> 8);
    drv_gd25q20_byte_wr(addr & 0xFF);
    
    if(len > DRV_GD25Q20_PAGE_SIZE)
    {
        len = DRV_GD25Q20_PAGE_SIZE;
        drv_3rd_uart_printf("Page Program data too large!"); 
    }
    while(len--)
    {
        drv_gd25q20_byte_wr(*pbuf);
        pbuf++;
    }
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    
    drv_gd25q20_busy_wait();
//    drv_gd25q20_write_disable();
}

/**
  * Function    : drv_gd25q20_read
  * Description : 读闪存数据
  * Input       : pbuf ---- 数据
  *               addr ---- 地址
                  len ---- 长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_read(uint8_t* pbuf, uint32_t addr, uint16_t len)
{
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_READ_DATA);
    drv_gd25q20_byte_wr((addr & 0xFF0000) >> 16);
    drv_gd25q20_byte_wr((addr & 0xFF00) >> 8);
    drv_gd25q20_byte_wr(addr & 0xFF);
    
    /* 读取数据 */
    while(len--)
    {
        *pbuf = drv_gd25q20_byte_wr(DRV_GD25Q20_DUMMY_BYTE);
        pbuf++;
    }
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
}

/**
  * Function    : drv_gd25q20_sector_erase
  * Description : 扇区擦除
  * Input       : addr ---- 擦除地址
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_sector_erase(uint32_t addr)
{
    drv_gd25q20_write_enable();

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_SECTOR_ERASE);
    drv_gd25q20_byte_wr((addr & 0xFF0000) >> 16);
    drv_gd25q20_byte_wr((addr & 0xFF00) >> 8);
    drv_gd25q20_byte_wr(addr & 0xFF);

    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    
    drv_gd25q20_busy_wait();
}

/**
  * Function    : drv_gd25q20_chip_rease
  * Description : FLASH整片擦除(为了安全起见，若要调用，请先调用 drv_gd25q20_write_enable 函数)
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_chip_rease(void)
{
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    
    drv_gd25q20_byte_wr(DRV_GD25Q20_CHIP_ERASE);
    
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    
    drv_gd25q20_busy_wait();
}

/**
  * Function    : drv_gd25q20_powr_down
  * Description : 掉电
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_powr_down(void)
{
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    drv_gd25q20_byte_wr(DRV_GD25Q20_POWER_DOWN);
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
}

/**
  * Function    : drv_gd25q20_release_powr_down
  * Description : 唤醒
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_release_powr_down(void)
{
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_LOW);
    drv_gd25q20_byte_wr(DRV_GD25Q20_RELEASE_POWER_DOWN);
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
}

/**
  * Function    : drv_gd25q20_write_nocheck
  * Description : 写数据
  * Input       : pbuf ---- 数据
  *               addr ---- 地址
                  len ---- 长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_write_nocheck(uint8_t* pbuf, uint32_t addr, uint16_t len)   
{
    uint16_t pageremain = 0;
    
    pageremain = DRV_GD25Q20_PAGE_SIZE - addr % DRV_GD25Q20_PAGE_SIZE;
    
    if(len <= pageremain)
    {
        pageremain = len;
    }

//    drv_3rd_uart_printf("pageremain:%d WriteAddr%d", pageremain, WriteAddr);
    
    while(1)
    {
        drv_gd25q20_write_page(pbuf, addr, pageremain);
        if(len == pageremain)
            break;
        else
        {
            pbuf += pageremain;
            addr += pageremain;
 
            len -= pageremain;
            
            if(len > DRV_GD25Q20_PAGE_SIZE)
                pageremain = DRV_GD25Q20_PAGE_SIZE;
            else 
                pageremain = len;
        }
    }
}

/**
  * Function    : drv_gd25q20_write
  * Description : 写闪存数据，可以使任意地址
  * Input       : pbuf ---- 数据
  *               addr ---- 地址
                  len ---- 长度
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_write(uint8_t* pbuf, uint32_t addr, uint16_t len)
{ 
    uint32_t secpos = 0;
    uint16_t secoff = 0;
    uint16_t secremain = 0;
    uint16_t i = 0;
    secpos = addr / DRV_GD25Q20_SOCTOR_SIZE;
    secoff = addr % DRV_GD25Q20_SOCTOR_SIZE;
    secremain = DRV_GD25Q20_SOCTOR_SIZE - secoff;
    
    if(len <= secremain)
    {
        secremain = len;
    }
    
    while(1) 
    {
        drv_gd25q20_read(gd25q20_buffer, secpos * DRV_GD25Q20_SOCTOR_SIZE, DRV_GD25Q20_SOCTOR_SIZE);

        for(i = 0; i < secremain; i++)
        {
            if(gd25q20_buffer[secoff + i] != 0XFF)
            {
                break;
            }
        }
        
        if(i < secremain)
        {
            drv_gd25q20_sector_erase(secpos * DRV_GD25Q20_SOCTOR_SIZE);
            
            for(i = 0; i < secremain; i++)
            {
                gd25q20_buffer[i + secoff] = pbuf[i];
            }
            drv_gd25q20_write_nocheck(gd25q20_buffer, secpos * DRV_GD25Q20_SOCTOR_SIZE, DRV_GD25Q20_SOCTOR_SIZE);
        }
        else
        {
            drv_gd25q20_write_nocheck(pbuf, addr, secremain);
        }
        
        if(len == secremain)
        {
            break;
        }
        else
        {
            secpos++;
            secoff = 0;
 
            pbuf += secremain;
            addr += secremain;
            len -= secremain;
            
            if(len > DRV_GD25Q20_SOCTOR_SIZE)
                secremain = DRV_GD25Q20_SOCTOR_SIZE;
            else 
                secremain = len;
        }
    }
}

/**
  * Function    : drv_gd25q20_init
  * Description : 配置输出IO，初始化模拟SPI引脚
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_gd25q20_init(void)
{
    GPIOReturnCode status = GPIORC_OK;
    GPIOConfiguration config = {0};

    //初始化 CS CLK MOSI
    status = GpioSetDirection(DRV_GD25Q20_SPI_CS, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_GD25Q20_SPI_CLK, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_GD25Q20_SPI_MOSI, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);

    }
    //初始化MISO
    config.pinDir = GPIO_IN_PIN;
    config.pinPull = GPIO_PULLUP_ENABLE;
    GpioInitConfiguration(DRV_GD25Q20_SPI_MISO,config);

    //设置为高电平
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CS, DRV_GD25Q20_GPIO_HIGH);
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_CLK, DRV_GD25Q20_GPIO_HIGH);
    drv_gd25q20_gpio_set(DRV_GD25Q20_SPI_MOSI, DRV_GD25Q20_GPIO_HIGH);

}

// End of file : drv_gd25q20.c 2024-5-11 15:23:55 by: zhaoning 

