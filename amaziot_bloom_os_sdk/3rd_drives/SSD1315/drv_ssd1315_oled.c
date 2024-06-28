//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_ssd1315_oled.c
// Auther      : zhaoning
// Version     :
// Date : 2024-6-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-6-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_ssd1315_oled.h"
#include "stdlib.h"
#include "drv_ssd1315_oledfont.h"
#include "drv_3rd.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static UINT8 OLED_GRAM[144][8];

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_ssd1315_delay_ms
  * Description : 延时 ms
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_delay_ms(unsigned int ms)
{
    OSATaskSleep((ms > 5) ? (ms / 5) : 1);
}

/**
  * Function    : drv_ssd1315_color_turn
  * Description : 反显函数
  * Input       : i 显示控制
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_color_turn(UINT8 i)
{
    if(i==0)
        {
            drv_ssd1315_wr_byte(0xA6,DRV_SSD1315_OLED_CMD);//正常显示
        }
    if(i==1)
        {
            drv_ssd1315_wr_byte(0xA7,DRV_SSD1315_OLED_CMD);//反色显示
        }
}

/**
  * Function    : drv_ssd1315_display_turn
  * Description : 屏幕旋转180度
  * Input       : i 显示控制
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_display_turn(UINT8 i)
{
    if(i==0)
        {
            drv_ssd1315_wr_byte(0xC8,DRV_SSD1315_OLED_CMD);//正常显示
            drv_ssd1315_wr_byte(0xA1,DRV_SSD1315_OLED_CMD);
        }
    if(i==1)
        {
            drv_ssd1315_wr_byte(0xC0,DRV_SSD1315_OLED_CMD);//反转显示
            drv_ssd1315_wr_byte(0xA0,DRV_SSD1315_OLED_CMD);
        }
}

/**
  * Function    : drv_ssd1315_wr_byte
  * Description : 写byte
  * Input       : dat 数据
  *               cmd 命令
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_wr_byte(UINT8 dat,UINT8 cmd)
{    
    UINT8 i;              
    if(cmd)
      DRV_SSD1315_DC_SET();
    else 
      DRV_SSD1315_DC_CLR();          
    DRV_SSD1315_CS_CLR();
    for(i=0;i<8;i++)
    {              
        DRV_SSD1315_SCL_CLR();
        if(dat&0x80)
           DRV_SSD1315_SDA_SET();
        else 
           DRV_SSD1315_SDA_CLR();
        DRV_SSD1315_SCL_SET();
        dat<<=1;   
    }                           
    DRV_SSD1315_CS_SET();
    DRV_SSD1315_DC_SET();         
}

/**
  * Function    : drv_ssd1315_display_on
  * Description : 开启OLED显示 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_display_on(void)
{
    drv_ssd1315_wr_byte(0x8D,DRV_SSD1315_OLED_CMD);//电荷泵使能
    drv_ssd1315_wr_byte(0x14,DRV_SSD1315_OLED_CMD);//开启电荷泵
    drv_ssd1315_wr_byte(0xAF,DRV_SSD1315_OLED_CMD);//点亮屏幕
}

/**
  * Function    : drv_ssd1315_display_off
  * Description : 关闭OLED显示 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_display_off(void)
{
    drv_ssd1315_wr_byte(0x8D,DRV_SSD1315_OLED_CMD);//电荷泵使能
    drv_ssd1315_wr_byte(0x10,DRV_SSD1315_OLED_CMD);//关闭电荷泵
    drv_ssd1315_wr_byte(0xAE,DRV_SSD1315_OLED_CMD);//关闭屏幕
}

/**
  * Function    : drv_ssd1315_refresh
  * Description : 更新显存到OLED
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_refresh(void)
{
    UINT8 i,n;
    for(i=0;i<8;i++)
    {
       drv_ssd1315_wr_byte(0xb0+i,DRV_SSD1315_OLED_CMD); //设置行起始地址
       drv_ssd1315_wr_byte(0x00,DRV_SSD1315_OLED_CMD);   //设置低列起始地址
       drv_ssd1315_wr_byte(0x10,DRV_SSD1315_OLED_CMD);   //设置高列起始地址
       for(n=0;n<128;n++)
         drv_ssd1315_wr_byte(OLED_GRAM[n][i],DRV_SSD1315_OLED_DATA);
  }
}

/**
  * Function    : drv_ssd1315_clear
  * Description : 清屏函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_clear(void)
{
    UINT8 i,n;
    for(i=0;i<8;i++)
    {
       for(n=0;n<128;n++)
            {
             OLED_GRAM[n][i]=0;//清除所有数据
            }
  }
    drv_ssd1315_refresh();//更新显示
}

/**
  * Function    : drv_ssd1315_draw_point
  * Description : 画点 
  * Input       : x:0~127
  *               t:1 填充 0,清空    
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_draw_point(UINT8 x,UINT8 y,UINT8 t)
{
    UINT8 i,m,n;
    i=y/8;
    m=y%8;
    n=1<<m;
    if(t){OLED_GRAM[x][i]|=n;}
    else
    {
        OLED_GRAM[x][i]=~OLED_GRAM[x][i];
        OLED_GRAM[x][i]|=n;
        OLED_GRAM[x][i]=~OLED_GRAM[x][i];
    }
}

/**
  * Function    : drv_ssd1315_draw_line
  * Description : 画线
  * Input       : x1,y1:起点坐标
  *               x2,y2:结束坐标
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_draw_line(UINT8 x1,UINT8 y1,UINT8 x2,UINT8 y2,UINT8 mode)
{
    UINT16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量 
    delta_y=y2-y1;
    uRow=x1;//画线起点坐标
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向 
    else if (delta_x==0)incx=0;//垂直线 
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//水平线 
    else {incy=-1;delta_y=-delta_x;}
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        drv_ssd1315_draw_point(uRow,uCol,mode);//画点
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}

/**
  * Function    : drv_ssd1315_draw_circle
  * Description : 画圆
  * Input       : x,y:圆心坐标
  *               r:圆的半径
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_draw_circle(UINT8 x,UINT8 y,UINT8 r)
{
    int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        drv_ssd1315_draw_point(x + a, y - b,1);
        drv_ssd1315_draw_point(x - a, y - b,1);
        drv_ssd1315_draw_point(x - a, y + b,1);
        drv_ssd1315_draw_point(x + a, y + b,1);
 
        drv_ssd1315_draw_point(x + b, y + a,1);
        drv_ssd1315_draw_point(x + b, y - a,1);
        drv_ssd1315_draw_point(x - b, y - a,1);
        drv_ssd1315_draw_point(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

/**
  * Function    : drv_ssd1315_show_char
  * Description : 在指定位置显示一个字符,包括部分字符
  * Input       : x:0~127
  *               y:0~63
  *               size1:选择字体 6x8/6x12/8x16/12x24
  *               mode:0,反色显示;1,正常显示
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_char(UINT8 x,UINT8 y,UINT8 chr,UINT8 size1,UINT8 mode)
{
    UINT8 i,m,temp,size2,chr1;
    UINT8 x0=x,y0=y;
    if(size1==8)size2=6;
    else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
    chr1=chr-' ';  //计算偏移后的值
    for(i=0;i<size2;i++)
    {
        if(size1==8)
              {temp=asc2_0806[chr1][i];} //调用0806字体
        else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
        else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
        else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
        else return;
        for(m=0;m<8;m++)
        {
            if(temp&0x01)drv_ssd1315_draw_point(x,y,mode);
            else drv_ssd1315_draw_point(x,y,!mode);
            temp>>=1;
            y++;
        }
        x++;
        if((size1!=8)&&((x-x0)==size1/2))
        {x=x0;y0=y0+8;}
        y=y0;
  }
}

/**
  * Function    : drv_ssd1315_show_string
  * Description : 显示字符串
  * Input       : x,y:起点坐标  
  *               size1:字体大小 
  *               *chr:字符串起始地址 
  *               mode:0,反色显示;1,正常显示
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_string(UINT8 x,UINT8 y,UINT8 *chr,UINT8 size1,UINT8 mode)
{
    while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
    {
        drv_ssd1315_show_char(x,y,*chr,size1,mode);
        if(size1==8)x+=6;
        else x+=size1/2;
        chr++;
  }
}

/**
  * Function    : drv_ssd1315_pow
  * Description : 显示数字
  * Input       : m底数，n指数
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 drv_ssd1315_pow(UINT8 m,UINT8 n)
{
    UINT32 result=1;
    while(n--)
    {
      result*=m;
    }
    return result;
}

/**
  * Function    : drv_ssd1315_show_num
  * Description : 显示数字
  * Input       : x,y :起点坐标
  *               num :要显示的数字
  *               len :数字的位数
  *               size:字体大小
  *               mode:0,反色显示;1,正常显示
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_num(UINT8 x,UINT8 y,UINT32 num,UINT8 len,UINT8 size1,UINT8 mode)
{
    UINT8 t,temp,m=0;
    if(size1==8)m=2;
    for(t=0;t<len;t++)
    {
        temp=(num/drv_ssd1315_pow(10,len-t-1))%10;
            if(temp==0)
            {
                drv_ssd1315_show_char(x+(size1/2+m)*t,y,'0',size1,mode);
      }
            else 
            {
              drv_ssd1315_show_char(x+(size1/2+m)*t,y,temp+'0',size1,mode);
            }
  }
}

/**
  * Function    : drv_ssd1315_show_chinese
  * Description : 显示汉字
  * Input       : x,y:起点坐标
  *               num:汉字对应的序号
  *               mode:0,反色显示;1,正常显示
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_chinese(UINT8 x,UINT8 y,UINT8 num,UINT8 size1,UINT8 mode)
{
    UINT8 m,temp;
    UINT8 x0=x,y0=y;
    UINT16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
    for(i=0;i<size3;i++)
    {
        if(size1==16)
                {temp=Hzk1[num][i];}//调用16*16字体
        else if(size1==24)
                {temp=Hzk2[num][i];}//调用24*24字体
        else if(size1==32)       
                {temp=Hzk3[num][i];}//调用32*32字体
        else if(size1==64)
                {temp=Hzk4[num][i];}//调用64*64字体
        else return;
        for(m=0;m<8;m++)
        {
            if(temp&0x01)drv_ssd1315_draw_point(x,y,mode);
            else drv_ssd1315_draw_point(x,y,!mode);
            temp>>=1;
            y++;
        }
        x++;
        if((x-x0)==size1)
        {x=x0;y0=y0+8;}
        y=y0;
    }
}

/**
  * Function    : drv_ssd1315_scroll_display
  * Description : 滚动显示汉字
  * Input       : num 显示汉字的个数
  *               space 每一遍显示的间隔
  *               mode:0,反色显示;1,正常显示
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_scroll_display(UINT8 num,UINT8 space,UINT8 mode)
{
    UINT8 i,n,t=0,m=0,r;
    while(1)
    {
        if(m==0)
        {
            drv_ssd1315_show_chinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
            t++;
        }
        if(t==num)
        {
            for(r=0;r<16*space;r++)      //显示间隔
            {
                for(i=1;i<144;i++)
                {
                    for(n=0;n<8;n++)
                    {
                        OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
                    }
                }
                drv_ssd1315_refresh();
            }
            t=0;
        }
        m++;
        if(m==16){m=0;}
        for(i=1;i<144;i++)   //实现左移
        {
            for(n=0;n<8;n++)
            {
                OLED_GRAM[i-1][n] = OLED_GRAM[i][n];
            }
        }
        drv_ssd1315_refresh();
    }
}

/**
  * Function    : drv_ssd1315_show_picture
  * Description : 显示图片
  * Input       : x,y：起点坐标
  *               sizex,sizey,图片长宽
  *               BMP[]：要写入的图片数组
  *               mode:0,反色显示;1,正常显示
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_picture(UINT8 x,UINT8 y,UINT8 sizex,UINT8 sizey,UINT8 BMP[],UINT8 mode)
{
    UINT16 j=0;
    UINT8 i,n,temp,m;
    UINT8 x0=x,y0=y;
    sizey=sizey/8+((sizey%8)?1:0);
    for(n=0;n<sizey;n++)
    {
         for(i=0;i<sizex;i++)
         {
                temp=BMP[j];
                j++;
                for(m=0;m<8;m++)
                {
                    if(temp&0x01)drv_ssd1315_draw_point(x,y,mode);
                    else drv_ssd1315_draw_point(x,y,!mode);
                    temp>>=1;
                    y++;
                }
                x++;
                if((x-x0)==sizex)
                {
                    x=x0;
                    y0=y0+8;
                }
                y=y0;
     }
     }
}
/**
  * Function    : drv_st7735s_gpio_init
  * Description : 初始化gpio
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
    status = GpioSetDirection(DRV_SSD1315_SPI_CS, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_SSD1315_SPI_SDA, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_SSD1315_SPI_SCL, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);
    }
    status = GpioSetDirection(DRV_SSD1315_SPI_DC, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);

    }
    status = GpioSetDirection(DRV_SSD1315_SPI_RES, GPIO_OUT_PIN);
    if (status != GPIORC_OK)
    {
        drv_3rd_uart_printf("status: 0x%lx", status);

    }

    //设置为高电平
    GpioSetLevel(DRV_SSD1315_SPI_CS, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_SDA, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_SCL, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_DC, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_RES, DRV_ST7735S_GPIO_HIGH);

}

/**
  * Function    : drv_ssd1315_init
  * Description : OLED的初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_init(void)
{
    drv_st7735s_gpio_init();
    
    DRV_SSD1315_RES_CLR();
    drv_ssd1315_delay_ms(200);
    DRV_SSD1315_RES_SET();
    
    drv_ssd1315_wr_byte(0xAE,DRV_SSD1315_OLED_CMD);//--turn off oled panel
    drv_ssd1315_wr_byte(0x00,DRV_SSD1315_OLED_CMD);//---set low column address
    drv_ssd1315_wr_byte(0x10,DRV_SSD1315_OLED_CMD);//---set high column address
    drv_ssd1315_wr_byte(0x40,DRV_SSD1315_OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    drv_ssd1315_wr_byte(0x81,DRV_SSD1315_OLED_CMD);//--set contrast control register
    drv_ssd1315_wr_byte(0xCF,DRV_SSD1315_OLED_CMD);// Set SEG Output Current Brightness
    drv_ssd1315_wr_byte(0xA1,DRV_SSD1315_OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    drv_ssd1315_wr_byte(0xC8,DRV_SSD1315_OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    drv_ssd1315_wr_byte(0xA6,DRV_SSD1315_OLED_CMD);//--set normal display
    drv_ssd1315_wr_byte(0xA8,DRV_SSD1315_OLED_CMD);//--set multiplex ratio(1 to 64)
    drv_ssd1315_wr_byte(0x3f,DRV_SSD1315_OLED_CMD);//--1/64 duty
    drv_ssd1315_wr_byte(0xD3,DRV_SSD1315_OLED_CMD);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    drv_ssd1315_wr_byte(0x00,DRV_SSD1315_OLED_CMD);//-not offset
    drv_ssd1315_wr_byte(0xd5,DRV_SSD1315_OLED_CMD);//--set display clock divide ratio/oscillator frequency
    drv_ssd1315_wr_byte(0x80,DRV_SSD1315_OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    drv_ssd1315_wr_byte(0xD9,DRV_SSD1315_OLED_CMD);//--set pre-charge period
    drv_ssd1315_wr_byte(0xF1,DRV_SSD1315_OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    drv_ssd1315_wr_byte(0xDA,DRV_SSD1315_OLED_CMD);//--set com pins hardware configuration
    drv_ssd1315_wr_byte(0x12,DRV_SSD1315_OLED_CMD);
    drv_ssd1315_wr_byte(0xDB,DRV_SSD1315_OLED_CMD);//--set vcomh
    drv_ssd1315_wr_byte(0x40,DRV_SSD1315_OLED_CMD);//Set VCOM Deselect Level
    drv_ssd1315_wr_byte(0x20,DRV_SSD1315_OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    drv_ssd1315_wr_byte(0x02,DRV_SSD1315_OLED_CMD);//
    drv_ssd1315_wr_byte(0x8D,DRV_SSD1315_OLED_CMD);//--set Charge Pump enable/disable
    drv_ssd1315_wr_byte(0x14,DRV_SSD1315_OLED_CMD);//--set(0x10) disable
    drv_ssd1315_wr_byte(0xA4,DRV_SSD1315_OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    drv_ssd1315_wr_byte(0xA6,DRV_SSD1315_OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    drv_ssd1315_clear();
    drv_ssd1315_wr_byte(0xAF,DRV_SSD1315_OLED_CMD);
}

// End of file : drv_ssd1315_oled.c 2024-6-7 9:57:14 by: zhaoning 

