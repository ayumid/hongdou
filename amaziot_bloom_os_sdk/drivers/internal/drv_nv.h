//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : drv_nv.h
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_NV_H_
#define _DRV_NV_H_

// Includes ---------------------------------------------------------------------
#include "osa.h"
#include "sdk_api.h"
// Public defines / typedef -----------------------------------------------------

// Public functions prototypes --------------------------------------------------
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
unsigned long drv_get_custem_start_address(void);
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
unsigned long drv_get_custem_end_address(void);

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
int drv_nv_write_data(UINT32 addr, unsigned char * buf_addr,unsigned int size);

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
int drv_nv_read_data(UINT32 addr, unsigned char * buf_addr, unsigned int size);
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
int drv_nv_delete_data(UINT32 addr, unsigned int size);

#endif /* ifndef _DRV_NV_H_.2021-11-23 18:01:17 by: win */


