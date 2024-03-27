//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_lora_form.c
// Auther      : zhaoning
// Version     :
// Date : 2024-1-11
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-1-11
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "FDI_TYPE.h"
#include "FDI_FILE.h"

#include "am_lora_form.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static int dtu_form_conf_file_read(int fd);

// Public functions prototypes --------------------------------------------------

DTU_FORM_FILE_PARAM_T st_dtu_form_file_t[256];//文件结构体变量

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_form_file_ctx
  * Description : 获取文件上下文指针
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_FORM_FILE_PARAM_T* dtu_get_form_file_ctx(void)
{
    return st_dtu_form_file_t;
}

/**
  * Function    : dtu_init_form_conf
  * Description : 初始化云服务器信息到文件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_init_form_conf(void)
{
    memset(st_dtu_form_file_t, 0, sizeof(st_dtu_form_file_t));
    //返回文件首地址
    return 0;
}

/**
  * Function    : dtu_form_conf_file_init
  * Description : 文件创建
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_form_conf_file_init(void)
{
    int fd = 0;

    //打开信息存储文件
    fd = FDI_fopen("form_file", "rb");
    //如果文件存在，就读文件
    if (fd)
    {
        dtu_form_conf_file_read(fd);
        //uprintf("%s[%d] file exist, %ld, %s", __FUNCTION__, __LINE__, st_dtu_form_file_t[0].day_timestamp, st_dtu_form_file_t[0].devid);
    }
    //如果文件不存在，就创建文件，初始化文件
    else
    {
        //初始化文件
        dtu_init_form_conf();
        //写入文件
        dtu_form_conf_file_write(&st_dtu_form_file_t);
    }
    
}

/**
  * Function    : dtu_form_conf_file_read
  * Description : 云服务器信息读文件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_form_conf_file_read(int fd)
{
    DTU_FORM_FILE_PARAM_T st_file = {0};
    
    //读文件
    FDI_fread(&st_file, sizeof(st_file), 1, fd);
    //关闭文件
    FDI_fclose(fd);
    //拷贝到文件
    memcpy(&st_dtu_form_file_t, &st_file, sizeof(DTU_FORM_FILE_PARAM_T));
//    uprintf("debug> ip = %s", st_dtu_form_file_t.ip);

    //返回文件首地址
    return 0;
}

/**
  * Function    : dtu_form_conf_file_write
  * Description : 云服务器信息写入文件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_form_conf_file_write(DTU_FORM_FILE_PARAM_T* st_dtu_file)
{
   int fd = 0;

   //打开文件
   fd = FDI_fopen("form_file", "wb");
   //如果文件存在，写入，并关闭文件
   if (fd != 0)
   {
        uprintf("%s[%d] open file for write OK", __FUNCTION__, __LINE__);
        FDI_fwrite(st_dtu_file, sizeof(DTU_FORM_FILE_PARAM_T), 1, fd);
        FDI_fclose(fd);
   }
   //文件不存在，报错
   else
   {
        uprintf("%s[%d] open file for write error", __FUNCTION__, __LINE__);
        return -1;
   }
   
   return 0;
}

// End of file : am_lora_form.c 2024-1-11 17:31:43 by: zhaoning 

