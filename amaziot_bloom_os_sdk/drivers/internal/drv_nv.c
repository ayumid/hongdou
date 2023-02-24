//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_nv.c
// Auther      : win
// Version     :
// Date : 2021-11-23
// Description :
//          
//          
// History     :
//     
//    1. Time         : 2021-11-23
//       Modificator  : win
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "drv_nv.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------
/**
  * Function    : drv_get_custem_start_address
  * Description : 获取用户可写nv起始地址
  * Input       : 
  *               
  * Output      : 
  * Return      : 用户可写nv起始地址
  * Auther      : win
  * Others      : 
  **/
unsigned long drv_get_custem_start_address(void)
{
	return yuge_nor_custem_start_address();
}
/**
  * Function    : drv_get_custem_end_address
  * Description : 获取用户可写nv结束地址
  * Input       : 
  *               
  * Output      : 
  * Return      : 用户可写nv结束地址
  * Auther      : win
  * Others      : 
  **/
unsigned long drv_get_custem_end_address(void)
{
	return yuge_nor_custem_end_address();
}

/**
  * Function    : drv_nv_write_data
  * Description : 写入数据
  * Input       : addr      写入地址
  *               buf_addr  写缓冲器首地址
  *               size      大小               
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int drv_nv_write_data(UINT32 addr, unsigned char * buf_addr,unsigned int size)
{
	
    return yuge_nor_do_write(addr,buf_addr,size);
}
/**
  * Function    : drv_nv_read_data
  * Description : 读取数据
  * Input       : addr     读取地址
  *               buf_addr 读缓冲器首地址
  *               size     大小
  *               
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int drv_nv_read_data(UINT32 addr, unsigned char * buf_addr, unsigned int size)
{
   return yuge_nor_do_read(addr,buf_addr,size);
}

/**
  * Function    : drv_nv_delete_data
  * Description : 删除数据
  * Input       : addr    地址
  *               size    大小
  * Output      : 
  * Return      : 成功/失败
  * Auther      : win
  * Others      : 
  **/
int drv_nv_delete_data(UINT32 addr, unsigned int size)
{
	return yuge_nor_do_erase(addr,size);	// min size 4k, max size 64k
}

// End of file : drv_nv.h 2021-11-23 17:49:44 by: win 

