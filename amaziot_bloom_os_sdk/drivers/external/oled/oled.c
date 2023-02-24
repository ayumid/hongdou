//------------------------------------------------------------------------------
// Copyright , 2017-2021 �漣�������������Ƽ����޹�˾
// Filename    : oled.c
// Auther      : win
// Version     :
// Date : 2021-12-29
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-29
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	

// Includes ---------------------------------------------------------------------
#include "utils_common.h"
#include "oled.h"
#include "oledfont.h"  	 

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------
//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				
// Functions --------------------------------------------------------------------
/**
  * Function    : OLED_Gpio_Init
  * Description : OLED��Ӧ��GPIO��ʼ��
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Gpio_Init(void)
{
	unsigned long tempaddr = 0;
	volatile unsigned int value;

	tempaddr = GPIO_MFPR_ADDR(OLED_SCL);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("OLED_SCL value %lx\n", value); 
	if (value & 0x01){		
		value = value & (~0x01);		//�๦��pin�ţ��󲿷�GPIO���ܶ���Ĭ�����λΪ0
	}	
	*((volatile unsigned long *)tempaddr) = 0xd040;
	sdk_uart_printf("OLED_SCL value %lx\n", *((volatile unsigned long *)tempaddr));

	tempaddr = GPIO_MFPR_ADDR(OLED_SDIN);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("OLED_SDIN value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//�๦��pin�ţ��󲿷�GPIO���ܶ���Ĭ�����λΪ0
	}	
	*((volatile unsigned long *)tempaddr) = 0xd040;;
	sdk_uart_printf("OLED_SDIN value %lx\n", *((volatile unsigned long *)tempaddr));	

	tempaddr = GPIO_MFPR_ADDR(OLED_DC);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("OLED_DC value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//�๦��pin�ţ��󲿷�GPIO���ܶ���Ĭ�����λΪ0
	}	
	*((volatile unsigned long *)tempaddr) = 0xd040;;
	sdk_uart_printf("OLED_DC value %lx\n", *((volatile unsigned long *)tempaddr));

	tempaddr = GPIO_MFPR_ADDR(OLED_CS);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("OLED_CS value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//�๦��pin�ţ��󲿷�GPIO���ܶ���Ĭ�����λΪ0
	}	
	*((volatile unsigned long *)tempaddr) = 0xd040;;
	sdk_uart_printf("OLED_CS value %lx\n", *((volatile unsigned long *)tempaddr));	

	tempaddr = GPIO_MFPR_ADDR(OLED_RST);
	value = *((volatile unsigned long *)tempaddr);
	sdk_uart_printf("OLED_RST value %lx\n", value);	
	if (value & 0x01){		
		value = value & (~0x01);		//�๦��pin�ţ��󲿷�GPIO���ܶ���Ĭ�����λΪ0
	}	
	*((volatile unsigned long *)tempaddr) = 0xd040;;
	sdk_uart_printf("OLED_RST value %lx\n", *((volatile unsigned long *)tempaddr));	
}
#if OLED_MODE==1
/**
  * Function    : OLED_WR_Byte
  * Description : ��SSD1106д��һ���ֽ�
  * Input       : dat:Ҫд�������/����
  *               cmd:����/�����־ 0,��ʾ����;1,��ʾ����
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATAOUT(dat);	    
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		   
	OLED_CS_Clr();
	OLED_WR_Clr();	 
	OLED_WR_Set();
	OLED_CS_Set();	  
	OLED_DC_Set();	 
} 	    	    
#else
/**
  * Function    : OLED_WR_Byte
  * Description : ��SSD1106д��һ���ֽ�
  * Input       : dat:Ҫд�������/����
  *               cmd:����/�����־ 0,��ʾ����;1,��ʾ����
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd){
		OLED_DC_Set();
	}else
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		{
			OLED_SDIN_Set();
		}else
			OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 
#endif

/**
  * Function    : OLED_Set_Pos
  * Description : ָ����ʾλ��
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
/**
  * Function    : OLED_Display_On
  * Description : ����OLED��ʾ 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}   
/**
  * Function    : OLED_Display_Off
  * Description : �ر�OLED��ʾ  
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
/**
  * Function    : OLED_Clear
  * Description : ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //������ʾ
}
/**
  * Function    : OLED_ShowChar
  * Description : ��ָ��λ����ʾһ���ַ�,���������ַ�
  * Input       : x:0~127
  *               y:0~63
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : //mode:0,������ʾ;1,������ʾ				 
  *  			  //size:ѡ������ 16/12 
  **/
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//�õ�ƫ�ƺ��ֵ			
	if(x>Max_Column-1){x=0;y=y+2;}
	if(SIZE ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else{	
		OLED_Set_Pos(x,y+1);
		for(i=0;i<6;i++)
		OLED_WR_Byte(F6x8[c][i],OLED_DATA);
		
	}
}	  
/**
  * Function    : OLED_ShowNum
  * Description : ��ʾ2������
  * Input       : x,y :�������
  *				  num:��ֵ(0~4294967295)
  *               len :���ֵ�λ��
  *               size:�����С
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : //mode:ģʽ	0,���ģʽ;1,����ģʽ
  **/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0'); 
	}
} 
/**
  * Function    : OLED_ShowString
  * Description : ��ʾһ���ַ���
  * Input       : x,y :�������
  *               chr :�ַ����׵�ַ
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j]);
		x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
/**
  * Function    : OLED_ShowCHinese
  * Description : ��ʾ����
  * Input       : x,y :�������
  *               no  :���ֱ��
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{
		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
		adder+=1;
    }	
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
	{	
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
	}					
}
/**
  * Function    : OLED_DrawBMP
  * Description : ��ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 
/**
  * Function    : OLED_Init
  * Description : ��ʼ��SSD1306	
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void OLED_Init(void)
{
	OLED_Gpio_Init();
	
 	OLED_RST_Set();
	msleep(1);
	OLED_RST_Clr();
	msleep(1);
	OLED_RST_Set(); 
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  

// End of file : oled.h 2021-12-29 10:14:52 by: win 




