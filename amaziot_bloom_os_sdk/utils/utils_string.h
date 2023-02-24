//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : utils_string.h
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _UTILS_STRING_H_
#define _UTILS_STRING_H_

// Includes ---------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
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
int utils_is_begin_with(const char * str1,char *str2);
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
int utils_is_end_with(const char *str1, char *str2);
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
void utils_itoi(char *str, unsigned long num);
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
char utils_itoc(unsigned char ichar);
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
void utils_Hex2Str(char* pSrc, char* pDst, unsigned int nSrcLength);
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
unsigned char utils_AsciiToHex(unsigned char cNum);
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
void utils_StrToHex(char *ptr, unsigned short len);

#endif /* ifndef _UTILS_STRING_H_.2021-12-22 15:21:58 by: win */

