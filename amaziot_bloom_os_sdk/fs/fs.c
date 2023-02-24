//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : fs.c
// Auther      : win
// Version     :
// Date : 2021-11-24
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-24
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "fs.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : fs_write_data
  * Description : 写入文件数据
  * Input       : key       标识
  *               buffer    数据
  *               length    长度               
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int fs_write_data(const char* key, const unsigned char* buffer, unsigned int length)
{
	
    if (NULL == key || NULL == buffer) {
        return -1;
    }	
	int fd = 0;
    fd = FDI_fopen(key, "wb");
    if (fd != 0){
        FDI_fwrite(buffer,length, 1, fd);
   		FDI_fclose(fd);
    } else {
        return -1;
    }
   
    return 0;
}
/**
  * Function    : fs_read_data
  * Description : 读取文件数据
  * Input       : key    标识
  *               buffer 读取数据存储
  *               length 读取长度
  *               
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int fs_read_data(const char* key, unsigned char* buffer, unsigned int* length)
{
    if (NULL == key || NULL == buffer || NULL == length) {
        return -1;
    }
    int fd = 0;
    fd = FDI_fopen(key, "rb");
	if (fd) {
	  	FDI_fread(buffer,length,1,fd);
		FDI_fclose(fd);
	} else {
		return -1;
	} 
	return 0;
}

/**
  * Function    : fs_delete_data
  * Description : 删除文件数据
  * Input       : key    标识
  *               
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int fs_delete_data(const char* key)
{
	if (NULL == key) {
        return -1;
    }	
    if (FDI_remove(key) == 0) {
        return 0;
    } else {
       return -1;
    }
    return 0;
}

// End of file : fs.h 2021-11-24 16:38:01 by: win 

