//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : utils_string.c
// Auther      : win
// Version     :
// Date : 2021-12-22
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-12-22
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "utils_string.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : utils_is_begin_with
  * Description : 判断字符串开头
  * Input       : str1 原始字符串
  *               str2 开头字符串
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
int utils_is_begin_with(const char * str1,char *str2)
{
  if(str1 == NULL || str2 == NULL)
    return -1;
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  if((len1 < len2) || (len1 == 0 || len2 == 0))
    return -1;
  char *p = str2;
  int i = 0;
  while(*p != '\0')
  {
    if(*p != str1[i])
      return 0;
    p++;
    i++;
  }
  return 1;
}

/**
  * Function    : utils_is_end_with
  * Description : 判断字符串结尾
  * Input       : str1 原始字符串
  *               str2 结尾字符串
  *               
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
int utils_is_end_with(const char *str1, char *str2)
{
  if(str1 == NULL || str2 == NULL)
    return -1;
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  if((len1 < len2) || (len1 == 0 || len2 == 0))
    return -1;
  while(len2 >= 1)
  {
    if(str2[len2 - 1] != str1[len1 - 1])
      return 0;
    len2--;
    len1--;
  }
  return 1;
}
/**
  * Function    : utils_itoi
  * Description : 数字转化为字符串
  * Input       : str 转化字符串容器
  *               num 需转化的数字 
  *               
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
void utils_itoi(char *str, unsigned long num)
{
	char index[]="0123456789";
	char i = 0, j = 0;
    char temp;

    do
    {
        str[i++] = index[num%10];
        num /= 10;
    }while(num);

    str[i]='\0';

    for(j=0; j<=(i-1)/2; j++)
    {
        temp=str[j];
        str[j]=str[i-j-1];
        str[i-j-1]=temp;
    }
}
/**
  * Function    : utils_itoc
  * Description : 单个数字转化为字符
  * Input       : 需转化的数字
  *               
  * Output      : 转化后字符
  * Return      : 
  * Auther      : win
  * Others      : 
  **/
char utils_itoc(unsigned char ichar)
{
	char index[]="0123456789";

    return index[ichar%10];
}
/**
  * Function    : utils_Hex2Str
  * Description : 16进制数转化成字符串
  * Input       : pSrc  源数据指针
  *               pDst  目标字符串指针
  *               nSrcLength  转化长度
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01"
  **/
void utils_Hex2Str(char* pSrc, char* pDst, unsigned int nSrcLength)
{
	int i = 0;
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf的字符查找表

	for (i = 0; i < nSrcLength; i++)
	{
		*pDst++ = tab[*pSrc >> 4];		// 输出高4位
		*pDst++ = tab[*pSrc & 0x0f];	// 输出低4位
		pSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';
}
/**
  * Function    : utils_AsciiToHex
  * Description : ASCALL码转换成字符
  * Input       : cNum ASC-II字符码
  *              
  * Output      : 
  * Return      : HEX码
  * Auther      : win
  * Others      : 如：{'A'} --> 0xA
  **/
unsigned char utils_AsciiToHex(unsigned char cNum)
{
	if(cNum>='0'&&cNum<='9')
	{
		cNum -= '0';
	}
	else if(cNum>='A'&&cNum<='F')
	{
		cNum -= 'A';
		cNum += 10;
	}
	else if(cNum>='a'&&cNum<='f')
	{
		cNum -= 'a';
		cNum += 10;
	}
	return cNum;
}
/**
  * Function    : utils_StrToHex
  * Description : 字符串转换hex
  * Input       : ptr 字符存储区
  *               len 数据长度
  * Output      : 
  * Return      : 
  * Auther      : win
  * Others      : 如："C8329BFD0E01" -->  {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
  **/
void utils_StrToHex(char *ptr, unsigned short len)
{
	unsigned char n1  = 0;
	unsigned char n2  = 0;
	unsigned char num = 0;
	unsigned short i = 0;
	unsigned short index = 0;

	for(i=0; i<len; i++)
	{
		index = i << 1;				//index=i*2
		n1 = AsciiToHex(ptr[index]);
		n2 = AsciiToHex(ptr[index+1]);
		num = (n1<<4) + n2;
		ptr[i] = num;
	}
}

// End of file : utils_string.h 2021-12-22 15:21:51 by: win 

