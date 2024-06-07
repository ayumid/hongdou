//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_st7735s_lcd_init.c
// Auther      : zhaoning
// Version     :
// Date : 2024-6-5
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-5
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_st7735s_lcd_init.h"
#include "drv_3rd.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_st7735s_delay_ms
  * Description : 延时函数
  * Input       : ms     毫秒
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_delay_ms(unsigned int ms)
{
    OSATaskSleep((ms > 5) ? (ms / 5) : 1);
}

/**
  * Function    : drv_st7735s_gpio_init
  * Description : gpio初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_gpio_init(void)
{
    GPIOReturnCode status = GPIORC_OK;
    GPIOConfiguration config = {0};

    //初始化 CS CLK MOSI
    status = GpioSetDirection(DRV_ST7735S_I2C_CS, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_ST7735S_I2C_SCL, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_ST7735S_I2C_SDA, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_ST7735S_I2C_DC, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);

    }
    status = GpioSetDirection(DRV_ST7735S_I2C_RES, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);

    }
    status = GpioSetDirection(DRV_ST7735S_I2C_BLK, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);

    }

    //设置为高电平
    GpioSetLevel(DRV_ST7735S_I2C_CS, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_ST7735S_I2C_SCL, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_ST7735S_I2C_SDA, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_ST7735S_I2C_DC, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_ST7735S_I2C_RES, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_ST7735S_I2C_BLK, DRV_ST7735S_GPIO_HIGH);

}

/**
  * Function    : drv_st7735s_wr
  * Description : LCD串行数据写入函数
  * Input       : dat 要写入的串行数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_wr(UINT8 dat) 
{    
    UINT8 i;
    DRV_ST7735S_CS_CLR();
    for(i=0;i<8;i++)
    {              
        DRV_ST7735S_SCLK_CLR();
        if(dat&0x80)
        {
           DRV_ST7735S_MOSI_SET();
        }
        else
        {
           DRV_ST7735S_MOSI_CLR();
        }
        DRV_ST7735S_SCLK_SET();
        dat<<=1;
    }    
  DRV_ST7735S_CS_SET();    
}

/**
  * Function    : drv_st7735s_wr_byte
  * Description : LCD写一个字节数据
  * Input       : dat 写入的数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_wr_byte(UINT8 dat)
{
    drv_st7735s_wr(dat);
}

/**
  * Function    : drv_st7735s_wr_word
  * Description : LCD写入两个字节数据
  * Input       : dat 写入的数据
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_wr_word(UINT16 dat)
{
    drv_st7735s_wr(dat>>8);
    drv_st7735s_wr(dat);
}

/**
  * Function    : drv_st7735s_wr_reg
  * Description : LCD写入命令
  * Input       : dat 写入的命令
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_wr_reg(UINT8 dat)
{
    DRV_ST7735S_DC_CLR();//写命令
    drv_st7735s_wr(dat);
    DRV_ST7735S_DC_SET();//写数据
}

/**
  * Function    : drv_st7735s_addr_set
  * Description : 设置起始和结束地址
  * Input       : x1,x2 设置列的起始和结束地址
  *               y1,y2 设置行的起始和结束地址
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_addr_set(UINT16 x1,UINT16 y1,UINT16 x2,UINT16 y2)
{
    if(DRV_ST7735S_USE_HORIZONTAL==0)
    {
        drv_st7735s_wr_reg(0x2a);//列地址设置
        drv_st7735s_wr_word(x1+26);
        drv_st7735s_wr_word(x2+26);
        drv_st7735s_wr_reg(0x2b);//行地址设置
        drv_st7735s_wr_word(y1+1);
        drv_st7735s_wr_word(y2+1);
        drv_st7735s_wr_reg(0x2c);//储存器写
    }
    else if(DRV_ST7735S_USE_HORIZONTAL==1)
    {
        drv_st7735s_wr_reg(0x2a);//列地址设置
        drv_st7735s_wr_word(x1+26);
        drv_st7735s_wr_word(x2+26);
        drv_st7735s_wr_reg(0x2b);//行地址设置
        drv_st7735s_wr_word(y1+1);
        drv_st7735s_wr_word(y2+1);
        drv_st7735s_wr_reg(0x2c);//储存器写
    }
    else if(DRV_ST7735S_USE_HORIZONTAL==2)
    {
        drv_st7735s_wr_reg(0x2a);//列地址设置
        drv_st7735s_wr_word(x1+1);
        drv_st7735s_wr_word(x2+1);
        drv_st7735s_wr_reg(0x2b);//行地址设置
        drv_st7735s_wr_word(y1+26);
        drv_st7735s_wr_word(y2+26);
        drv_st7735s_wr_reg(0x2c);//储存器写
    }
    else
    {
        drv_st7735s_wr_reg(0x2a);//列地址设置
        drv_st7735s_wr_word(x1+1);
        drv_st7735s_wr_word(x2+1);
        drv_st7735s_wr_reg(0x2b);//行地址设置
        drv_st7735s_wr_word(y1+26);
        drv_st7735s_wr_word(y2+26);
        drv_st7735s_wr_reg(0x2c);//储存器写
    }
}

/**
  * Function    : drv_st7735s_init
  * Description : st7735s初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_init(void)
{
    drv_st7735s_gpio_init();//初始化GPIO
    
    DRV_ST7735S_RES_CLR();//复位
    drv_st7735s_delay_ms(100);
    DRV_ST7735S_RES_SET();
    drv_st7735s_delay_ms(100);
    
    DRV_ST7735S_BLK_SET();//打开背光
    drv_st7735s_delay_ms(100);
    
    drv_st7735s_wr_reg(0x11);     //Sleep out
    drv_st7735s_delay_ms(120);                //Delay 120ms
    drv_st7735s_wr_reg(0xB1);     //Normal mode
    drv_st7735s_wr_byte(0x05);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_reg(0xB2);     //Idle mode
    drv_st7735s_wr_byte(0x05);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_reg(0xB3);     //Partial mode
    drv_st7735s_wr_byte(0x05);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_byte(0x05);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_byte(0x3C);   
    drv_st7735s_wr_reg(0xB4);     //Dot inversion
    drv_st7735s_wr_byte(0x03);   
    drv_st7735s_wr_reg(0xC0);     //AVDD GVDD
    drv_st7735s_wr_byte(0xAB);   
    drv_st7735s_wr_byte(0x0B);   
    drv_st7735s_wr_byte(0x04);   
    drv_st7735s_wr_reg(0xC1);     //VGH VGL
    drv_st7735s_wr_byte(0xC5);   //C0
    drv_st7735s_wr_reg(0xC2);     //Normal Mode
    drv_st7735s_wr_byte(0x0D);   
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_reg(0xC3);     //Idle
    drv_st7735s_wr_byte(0x8D);   
    drv_st7735s_wr_byte(0x6A);   
    drv_st7735s_wr_reg(0xC4);     //Partial+Full
    drv_st7735s_wr_byte(0x8D);   
    drv_st7735s_wr_byte(0xEE);   
    drv_st7735s_wr_reg(0xC5);     //VCOM
    drv_st7735s_wr_byte(0x0F);   
    drv_st7735s_wr_reg(0xE0);     //positive gamma
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x0E);   
    drv_st7735s_wr_byte(0x08);   
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x10);   
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x02);   
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x09);   
    drv_st7735s_wr_byte(0x0F);   
    drv_st7735s_wr_byte(0x25);   
    drv_st7735s_wr_byte(0x36);   
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_byte(0x08);   
    drv_st7735s_wr_byte(0x04);   
    drv_st7735s_wr_byte(0x10);   
    drv_st7735s_wr_reg(0xE1);     //negative gamma
    drv_st7735s_wr_byte(0x0A);   
    drv_st7735s_wr_byte(0x0D);   
    drv_st7735s_wr_byte(0x08);   
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x0F);   
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x02);   
    drv_st7735s_wr_byte(0x07);   
    drv_st7735s_wr_byte(0x09);   
    drv_st7735s_wr_byte(0x0F);   
    drv_st7735s_wr_byte(0x25);   
    drv_st7735s_wr_byte(0x35);   
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_byte(0x09);   
    drv_st7735s_wr_byte(0x04);   
    drv_st7735s_wr_byte(0x10);
         
    drv_st7735s_wr_reg(0xFC);    
    drv_st7735s_wr_byte(0x80);  
        
    drv_st7735s_wr_reg(0x3A);     
    drv_st7735s_wr_byte(0x05);   
    drv_st7735s_wr_reg(0x36);
    if(DRV_ST7735S_USE_HORIZONTAL == 0) 
    {
        drv_st7735s_wr_byte(0x08);
    }
    else if(DRV_ST7735S_USE_HORIZONTAL == 1)
    {
        drv_st7735s_wr_byte(0xC8);
    }
    else if(DRV_ST7735S_USE_HORIZONTAL == 2)
    {
        drv_st7735s_wr_byte(0x78);
    }
    else
    {
        drv_st7735s_wr_byte(0xA8);
    }
    drv_st7735s_wr_reg(0x21);     //Display inversion
    drv_st7735s_wr_reg(0x29);     //Display on
    drv_st7735s_wr_reg(0x2A);     //Set Column Address
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_byte(0x1A);  //26  
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_byte(0x69);   //105 
    drv_st7735s_wr_reg(0x2B);     //Set Page Address
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_byte(0x01);    //1
    drv_st7735s_wr_byte(0x00);   
    drv_st7735s_wr_byte(0xA0);    //160
    drv_st7735s_wr_reg(0x2C); 
}

// End of file : drv_st7735s_lcd_init.c 2024-6-5 11:52:44 by: zhaoning 
