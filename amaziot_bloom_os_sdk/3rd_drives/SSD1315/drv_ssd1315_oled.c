//------------------------------------------------------------------------------
// Copyright , 2017-2023 �漣�������������Ƽ����޹�˾
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
  * Description : ��ʱ ms
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
  * Description : ���Ժ���
  * Input       : i ��ʾ����
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
            drv_ssd1315_wr_byte(0xA6,DRV_SSD1315_OLED_CMD);//������ʾ
        }
    if(i==1)
        {
            drv_ssd1315_wr_byte(0xA7,DRV_SSD1315_OLED_CMD);//��ɫ��ʾ
        }
}

/**
  * Function    : drv_ssd1315_display_turn
  * Description : ��Ļ��ת180��
  * Input       : i ��ʾ����
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
            drv_ssd1315_wr_byte(0xC8,DRV_SSD1315_OLED_CMD);//������ʾ
            drv_ssd1315_wr_byte(0xA1,DRV_SSD1315_OLED_CMD);
        }
    if(i==1)
        {
            drv_ssd1315_wr_byte(0xC0,DRV_SSD1315_OLED_CMD);//��ת��ʾ
            drv_ssd1315_wr_byte(0xA0,DRV_SSD1315_OLED_CMD);
        }
}

/**
  * Function    : drv_ssd1315_wr_byte
  * Description : дbyte
  * Input       : dat ����
  *               cmd ����
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
  * Description : ����OLED��ʾ 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_display_on(void)
{
    drv_ssd1315_wr_byte(0x8D,DRV_SSD1315_OLED_CMD);//��ɱ�ʹ��
    drv_ssd1315_wr_byte(0x14,DRV_SSD1315_OLED_CMD);//������ɱ�
    drv_ssd1315_wr_byte(0xAF,DRV_SSD1315_OLED_CMD);//������Ļ
}

/**
  * Function    : drv_ssd1315_display_off
  * Description : �ر�OLED��ʾ 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_display_off(void)
{
    drv_ssd1315_wr_byte(0x8D,DRV_SSD1315_OLED_CMD);//��ɱ�ʹ��
    drv_ssd1315_wr_byte(0x10,DRV_SSD1315_OLED_CMD);//�رյ�ɱ�
    drv_ssd1315_wr_byte(0xAE,DRV_SSD1315_OLED_CMD);//�ر���Ļ
}

/**
  * Function    : drv_ssd1315_refresh
  * Description : �����Դ浽OLED
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
       drv_ssd1315_wr_byte(0xb0+i,DRV_SSD1315_OLED_CMD); //��������ʼ��ַ
       drv_ssd1315_wr_byte(0x00,DRV_SSD1315_OLED_CMD);   //���õ�����ʼ��ַ
       drv_ssd1315_wr_byte(0x10,DRV_SSD1315_OLED_CMD);   //���ø�����ʼ��ַ
       for(n=0;n<128;n++)
         drv_ssd1315_wr_byte(OLED_GRAM[n][i],DRV_SSD1315_OLED_DATA);
  }
}

/**
  * Function    : drv_ssd1315_clear
  * Description : ��������
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
             OLED_GRAM[n][i]=0;//�����������
            }
  }
    drv_ssd1315_refresh();//������ʾ
}

/**
  * Function    : drv_ssd1315_draw_point
  * Description : ���� 
  * Input       : x:0~127
  *               t:1 ��� 0,���    
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
  * Description : ����
  * Input       : x1,y1:�������
  *               x2,y2:��������
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
    delta_x=x2-x1; //������������ 
    delta_y=y2-y1;
    uRow=x1;//�����������
    uCol=y1;
    if(delta_x>0)incx=1; //���õ������� 
    else if (delta_x==0)incx=0;//��ֱ�� 
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//ˮƽ�� 
    else {incy=-1;delta_y=-delta_x;}
    if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        drv_ssd1315_draw_point(uRow,uCol,mode);//����
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
  * Description : ��Բ
  * Input       : x,y:Բ������
  *               r:Բ�İ뾶
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
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

/**
  * Function    : drv_ssd1315_show_char
  * Description : ��ָ��λ����ʾһ���ַ�,���������ַ�
  * Input       : x:0~127
  *               y:0~63
  *               size1:ѡ������ 6x8/6x12/8x16/12x24
  *               mode:0,��ɫ��ʾ;1,������ʾ
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
    else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    chr1=chr-' ';  //����ƫ�ƺ��ֵ
    for(i=0;i<size2;i++)
    {
        if(size1==8)
              {temp=asc2_0806[chr1][i];} //����0806����
        else if(size1==12)
        {temp=asc2_1206[chr1][i];} //����1206����
        else if(size1==16)
        {temp=asc2_1608[chr1][i];} //����1608����
        else if(size1==24)
        {temp=asc2_2412[chr1][i];} //����2412����
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
  * Description : ��ʾ�ַ���
  * Input       : x,y:�������  
  *               size1:�����С 
  *               *chr:�ַ�����ʼ��ַ 
  *               mode:0,��ɫ��ʾ;1,������ʾ
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_string(UINT8 x,UINT8 y,UINT8 *chr,UINT8 size1,UINT8 mode)
{
    while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
    {
        drv_ssd1315_show_char(x,y,*chr,size1,mode);
        if(size1==8)x+=6;
        else x+=size1/2;
        chr++;
  }
}

/**
  * Function    : drv_ssd1315_pow
  * Description : ��ʾ����
  * Input       : m������nָ��
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
  * Description : ��ʾ����
  * Input       : x,y :�������
  *               num :Ҫ��ʾ������
  *               len :���ֵ�λ��
  *               size:�����С
  *               mode:0,��ɫ��ʾ;1,������ʾ
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
  * Description : ��ʾ����
  * Input       : x,y:�������
  *               num:���ֶ�Ӧ�����
  *               mode:0,��ɫ��ʾ;1,������ʾ
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_ssd1315_show_chinese(UINT8 x,UINT8 y,UINT8 num,UINT8 size1,UINT8 mode)
{
    UINT8 m,temp;
    UINT8 x0=x,y0=y;
    UINT16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    for(i=0;i<size3;i++)
    {
        if(size1==16)
                {temp=Hzk1[num][i];}//����16*16����
        else if(size1==24)
                {temp=Hzk2[num][i];}//����24*24����
        else if(size1==32)       
                {temp=Hzk3[num][i];}//����32*32����
        else if(size1==64)
                {temp=Hzk4[num][i];}//����64*64����
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
  * Description : ������ʾ����
  * Input       : num ��ʾ���ֵĸ���
  *               space ÿһ����ʾ�ļ��
  *               mode:0,��ɫ��ʾ;1,������ʾ
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
            drv_ssd1315_show_chinese(128,24,t,16,mode); //д��һ�����ֱ�����OLED_GRAM[][]������
            t++;
        }
        if(t==num)
        {
            for(r=0;r<16*space;r++)      //��ʾ���
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
        for(i=1;i<144;i++)   //ʵ������
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
  * Description : ��ʾͼƬ
  * Input       : x,y���������
  *               sizex,sizey,ͼƬ����
  *               BMP[]��Ҫд���ͼƬ����
  *               mode:0,��ɫ��ʾ;1,������ʾ
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
  * Description : ��ʼ��gpio
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

    //��ʼ�� CS CLK MOSI
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

    //����Ϊ�ߵ�ƽ
    GpioSetLevel(DRV_SSD1315_SPI_CS, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_SDA, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_SCL, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_DC, DRV_ST7735S_GPIO_HIGH);
    GpioSetLevel(DRV_SSD1315_SPI_RES, DRV_ST7735S_GPIO_HIGH);

}

/**
  * Function    : drv_ssd1315_init
  * Description : OLED�ĳ�ʼ��
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
    drv_ssd1315_wr_byte(0xA1,DRV_SSD1315_OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    drv_ssd1315_wr_byte(0xC8,DRV_SSD1315_OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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

