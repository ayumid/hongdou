//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_st7735s_lcd.c
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

#include "drv_st7735s_lcd.h"
#include "drv_st7735s_lcd_init.h"
#include "drv_st7735s_lcdfont.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : drv_st7735s_lcd_fill
  * Description : 在指定区域填充颜色
  * Input       : xsta,ysta   起始坐标
  *               xend,yend   终止坐标
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_lcd_fill(UINT16 xsta,UINT16 ysta,UINT16 xend,UINT16 yend,UINT16 color)
{          
    UINT16 i,j; 
    drv_st7735s_addr_set(xsta,ysta,xend-1,yend-1);//设置显示范围
    for(i=ysta;i<yend;i++)
    {                                                                
        for(j=xsta;j<xend;j++)
        {
            drv_st7735s_wr_word(color);
        }
    }                               
}

/**
  * Function    : drv_st7735s_draw_point
  * Description : 在指定位置画点
  * Input       : x,y 画点坐标
  *               color 点的颜色
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_draw_point(UINT16 x,UINT16 y,UINT16 color)
{
    drv_st7735s_addr_set(x,y,x,y);//设置光标位置 
    drv_st7735s_wr_word(color);
} 

/**
  * Function    : drv_st7735s_draw_line
  * Description : 根据坐标画圆
  * Input       : x1,y1   起始坐标
  *               x2,y2   终止坐标
  *               color   线的颜色
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_draw_line(UINT16 x1,UINT16 y1,UINT16 x2,UINT16 y2,UINT16 color)
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
    else {incy=-1;delta_y=-delta_y;}
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        drv_st7735s_draw_point(uRow,uCol,color);//画点
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
  * Function    : drv_st7735s_draw_rectangle
  * Description : 指定坐标画矩形
  * Input       : x1,y1   起始坐标
  *               x2,y2   终止坐标
  *               color   矩形的颜色
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_draw_rectangle(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2,UINT16 color)
{
    drv_st7735s_draw_line(x1,y1,x2,y1,color);
    drv_st7735s_draw_line(x1,y1,x1,y2,color);
    drv_st7735s_draw_line(x1,y2,x2,y2,color);
    drv_st7735s_draw_line(x2,y1,x2,y2,color);
}

/**
  * Function    : drv_st7735s_draw_circle
  * Description : 画圆
  * Input       : x0,y0   圆心坐标
  *               r       半径
  *               color   圆的颜色
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_draw_circle(UINT16 x0,UINT16 y0,UINT8 r,UINT16 color)
{
    int a,b;
    a=0;b=r;      
    while(a<=b)
    {
        drv_st7735s_draw_point(x0-b,y0-a,color);             //3           
        drv_st7735s_draw_point(x0+b,y0-a,color);             //0           
        drv_st7735s_draw_point(x0-a,y0+b,color);             //1                
        drv_st7735s_draw_point(x0-a,y0-b,color);             //2             
        drv_st7735s_draw_point(x0+b,y0+a,color);             //4               
        drv_st7735s_draw_point(x0+a,y0-b,color);             //5
        drv_st7735s_draw_point(x0+a,y0+b,color);             //6 
        drv_st7735s_draw_point(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//判断要画的点是否过远
        {
            b--;
        }
    }
}

/**
  * Function    : drv_st7735s_show_chinese
  * Description : 显示汉字串
  * Input       : x,y显示坐标
  *               *s 要显示的汉字串
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号 可选 12 16 24 32
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_chinese(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{
    while(*s!=0)
    {
        if(sizey==12) drv_st7735s_show_chinese_12x12(x,y,s,fc,bc,sizey,mode);
        else if(sizey==16) drv_st7735s_show_chinese_16x16(x,y,s,fc,bc,sizey,mode);
        else if(sizey==24) drv_st7735s_show_chinese_24x24(x,y,s,fc,bc,sizey,mode);
        else if(sizey==32) drv_st7735s_show_chinese_32x32(x,y,s,fc,bc,sizey,mode);
        else return;
        s+=2;
        x+=sizey;
    }
}

/**
  * Function    : drv_st7735s_show_chinese_12x12
  * Description : 显示单个12x12汉字
  * Input       : x,y显示坐标
  *               *s 要显示的汉字
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_chinese_12x12(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{
    UINT8 i,j,m=0;
    UINT16 k;
    UINT16 HZnum;//汉字数目
    UINT16 TypefaceNum;//一个字符所占字节大小
    UINT16 x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
                             
    HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);    //统计汉字数目
    for(k=0;k<HZnum;k++) 
    {
        if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
        {     
            drv_st7735s_addr_set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {    
                    if(!mode)//非叠加方式
                    {
                        if(tfont12[k].Msk[i]&(0x01<<j))drv_st7735s_wr_word(fc);
                        else drv_st7735s_wr_word(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont12[k].Msk[i]&(0x01<<j))    drv_st7735s_draw_point(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }                      
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
} 

/**
  * Function    : drv_st7735s_show_chinese_16x16
  * Description : 显示单个16x16汉字
  * Input       : x,y显示坐标
  *               *s 要显示的汉字
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_chinese_16x16(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{
    UINT8 i,j,m=0;
    UINT16 k;
    UINT16 HZnum;//汉字数目
    UINT16 TypefaceNum;//一个字符所占字节大小
    UINT16 x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
    HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);    //统计汉字数目
    for(k=0;k<HZnum;k++) 
    {
        if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
        {     
            drv_st7735s_addr_set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {    
                    if(!mode)//非叠加方式
                    {
                        if(tfont16[k].Msk[i]&(0x01<<j))drv_st7735s_wr_word(fc);
                        else drv_st7735s_wr_word(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont16[k].Msk[i]&(0x01<<j))    drv_st7735s_draw_point(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }                      
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
} 

/**
  * Function    : drv_st7735s_show_chinese_24x24
  * Description : 显示单个24x24汉字
  * Input       : x,y显示坐标
  *               *s 要显示的汉字
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_chinese_24x24(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{
    UINT8 i,j,m=0;
    UINT16 k;
    UINT16 HZnum;//汉字数目
    UINT16 TypefaceNum;//一个字符所占字节大小
    UINT16 x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
    HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);    //统计汉字数目
    for(k=0;k<HZnum;k++) 
    {
        if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
        {     
            drv_st7735s_addr_set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {    
                    if(!mode)//非叠加方式
                    {
                        if(tfont24[k].Msk[i]&(0x01<<j))drv_st7735s_wr_word(fc);
                        else drv_st7735s_wr_word(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont24[k].Msk[i]&(0x01<<j))    drv_st7735s_draw_point(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }                      
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
} 

/**
  * Function    : drv_st7735s_show_chinese_32x32
  * Description : 显示单个32x32汉字
  * Input       : x,y显示坐标
  *               *s 要显示的汉字
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_chinese_32x32(UINT16 x,UINT16 y,UINT8 *s,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{
    UINT8 i,j,m=0;
    UINT16 k;
    UINT16 HZnum;//汉字数目
    UINT16 TypefaceNum;//一个字符所占字节大小
    UINT16 x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
    HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);    //统计汉字数目
    for(k=0;k<HZnum;k++) 
    {
        if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
        {     
            drv_st7735s_addr_set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {    
                    if(!mode)//非叠加方式
                    {
                        if(tfont32[k].Msk[i]&(0x01<<j))drv_st7735s_wr_word(fc);
                        else drv_st7735s_wr_word(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont32[k].Msk[i]&(0x01<<j))    drv_st7735s_draw_point(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }                      
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/**
  * Function    : drv_st7735s_show_char
  * Description : 显示单个字符
  * Input       : x,y显示坐标
  *               *s 要显示的汉字
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_char(UINT16 x,UINT16 y,UINT8 num,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{
    UINT8 temp,sizex,t,m=0;
    UINT16 i,TypefaceNum;//一个字符所占字节大小
    UINT16 x0=x;
    sizex=sizey/2;
    TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
    num=num-' ';    //得到偏移后的值
    drv_st7735s_addr_set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置 
    for(i=0;i<TypefaceNum;i++)
    { 
        if(sizey==12)temp=ascii_1206[num][i];               //调用6x12字体
        else if(sizey==16)temp=ascii_1608[num][i];         //调用8x16字体
        else if(sizey==24)temp=ascii_2412[num][i];         //调用12x24字体
        else if(sizey==32)temp=ascii_3216[num][i];         //调用16x32字体
        else return;
        for(t=0;t<8;t++)
        {
            if(!mode)//非叠加模式
            {
                if(temp&(0x01<<t))drv_st7735s_wr_word(fc);
                else drv_st7735s_wr_word(bc);
                m++;
                if(m%sizex==0)
                {
                    m=0;
                    break;
                }
            }
            else//叠加模式
            {
                if(temp&(0x01<<t))drv_st7735s_draw_point(x,y,fc);//画一个点
                x++;
                if((x-x0)==sizex)
                {
                    x=x0;
                    y++;
                    break;
                }
            }
        }
    }              
}

/**
  * Function    : drv_st7735s_show_string
  * Description : 显示字符串
  * Input       : x,y显示坐标
  *               *s 要显示的汉字
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  *               mode:  0非叠加模式  1叠加模式
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_string(UINT16 x,UINT16 y,const UINT8 *p,UINT16 fc,UINT16 bc,UINT8 sizey,UINT8 mode)
{         
    while(*p!='\0')
    {       
        drv_st7735s_show_char(x,y,*p,fc,bc,sizey,mode);
        x+=sizey/2;
        p++;
    }  
}

/**
  * Function    : drv_st7735s_pow
  * Description : 显示数字
  * Input       : m底数，n指数
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 drv_st7735s_pow(UINT8 m,UINT8 n)
{
    UINT32 result=1;     
    while(n--)result*=m;
    return result;
}

/**
  * Function    : drv_st7735s_show_int_num
  * Description : 
  * Input       : x,y显示坐标
  *               num 要显示整数变量
  *               len 要显示的位数
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_int_num(UINT16 x,UINT16 y,UINT16 num,UINT8 len,UINT16 fc,UINT16 bc,UINT8 sizey)
{             
    UINT8 t,temp;
    UINT8 enshow=0;
    UINT8 sizex=sizey/2;
    for(t=0;t<len;t++)
    {
        temp=(num/drv_st7735s_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                drv_st7735s_show_char(x+t*sizex,y,' ',fc,bc,sizey,0);
                continue;
            }else enshow=1; 
              
        }
         drv_st7735s_show_char(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
} 

/**
  * Function    : drv_st7735s_show_float_num
  * Description : 
  * Input       : x,y显示坐标
  *               num 要显示小数变量
  *               len 要显示的位数
  *               fc 字的颜色
  *               bc 字的背景色
  *               sizey 字号
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_float_num(UINT16 x,UINT16 y,float num,UINT8 len,UINT16 fc,UINT16 bc,UINT8 sizey)
{             
    UINT8 t,temp,sizex;
    UINT16 num1;
    sizex=sizey/2;
    num1=num*100;
    for(t=0;t<len;t++)
    {
        temp=(num1/drv_st7735s_pow(10,len-t-1))%10;
        if(t==(len-2))
        {
            drv_st7735s_show_char(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
            t++;
            len+=1;
        }
         drv_st7735s_show_char(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}

/**
  * Function    : drv_st7735s_show_pic
  * Description : 显示图片
  * Input       : x,y起点坐标
  *               length 图片长度
  *               width  图片宽度
  *               pic[]  图片数组
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void drv_st7735s_show_pic(UINT16 x,UINT16 y,UINT16 length,UINT16 width,const UINT8 pic[])
{
    UINT16 i,j;
    UINT32 k=0;
    drv_st7735s_addr_set(x,y,x+length-1,y+width-1);
    for(i=0;i<length;i++)
    {
        for(j=0;j<width;j++)
        {
            drv_st7735s_wr_byte(pic[k*2]);
            drv_st7735s_wr_byte(pic[k*2+1]);
            k++;
        }
    }            
}

// End of file : drv_st7735s_lcd.c 2024-6-5 12:00:58 by: zhaoning 
