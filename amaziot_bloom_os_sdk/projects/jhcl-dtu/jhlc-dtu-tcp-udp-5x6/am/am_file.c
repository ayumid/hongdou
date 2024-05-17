//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_file.c
// Auther      : zhaoning
// Version     :
// Date : 2023-8-28
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-28
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "FDI_TYPE.h"
#include "FDI_FILE.h"

#include "am.h"
#include "am_file.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static int dtu_trans_conf_file_read(int fd);

// Public functions prototypes --------------------------------------------------

DTU_FILE_PARAM_T st_dtu_file_t;//文件结构体变量

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_file_ctx
  * Description : 获取文件上下文指针
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_FILE_PARAM_T* dtu_get_file_ctx(void)
{
    return &st_dtu_file_t;
}

/**
  * Function    : dtu_init_trans_conf
  * Description : 初始化云服务器信息到文件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_init_trans_conf(void)
{
    //初始化连接类型
    st_dtu_file_t.socket.type = 0;
    //初始化默认IP，onenet 多协议接入 tcp
    sprintf(st_dtu_file_t.socket.ip,"183.230.40.40");
    //初始化端口
    st_dtu_file_t.socket.port = 1811;
    //设置默认上报心跳
    st_dtu_file_t.hb.heartflag = 1;
    //设置默认心跳内容为hearttest
    sprintf(st_dtu_file_t.hb.heart,"hearttest");
    //设置心跳间隔时间为30s
    st_dtu_file_t.hb.hearttime = 30;
    //设置默认上报注册包
    st_dtu_file_t.reg.linkflag = 1;
    //注册包类型 0 str 1 hex
    st_dtu_file_t.reg.linktype = 0;
    //设置注册包内容为"*275619#amaziot4000mt#AP4000MT*"，内容是根据onenet 多协议接入，tcp配置的脚本上报规则，上报后，onenet会不停下发
    sprintf(st_dtu_file_t.reg.link,"*275619#amaziot4000mt#AP4000MT*");
    //配置网络AT指令默认前缀是"am.iot"
    sprintf(st_dtu_file_t.net_at.cmdpw,"am.iot");

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
    //三合一切卡，默认卡是
    st_dtu_file_t.sim.sim = 1;
    //sim卡禁止切卡标志
    st_dtu_file_t.sim.simlock = 0;
#endif

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
    //默认不上报gnss语句
    st_dtu_file_t.gnss.gpsflag = 0;
#endif

    //返回文件首地址
    return 0;
}

/**
  * Function    : dtu_trans_conf_file_init
  * Description : 文件创建
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_trans_conf_file_init(void)
{
    int fd = 0;

    //打开信息存储文件
    fd = FDI_fopen("trans_file", "rb");
    //如果文件存在，就读文件
    if (fd)
    {
        dtu_trans_conf_file_read(fd);
    }
    //如果文件不存在，就创建文件，初始化文件
    else
    {
        //初始化文件
        dtu_init_trans_conf();
        //写入文件
        dtu_trans_conf_file_write(&st_dtu_file_t);
    }
    //串口输出版本
    char arrStr[32] = {0};
    sprintf(arrStr, "AMAZIOT AP4000MT_430EV5\r\n");
    
    UART_SEND_DATA((UINT8 *)arrStr, strlen(arrStr));
}

/**
  * Function    : dtu_trans_conf_file_read
  * Description : 云服务器信息读文件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_trans_conf_file_read(int fd)
{
    DTU_FILE_PARAM_T st_file = {0};
    
    //读文件
    FDI_fread(&st_file, sizeof(st_file), 1, fd);
    //关闭文件
    FDI_fclose(fd);
    //拷贝到文件
    memcpy(&st_dtu_file_t, &st_file, sizeof(DTU_FILE_PARAM_T));
//    printf("debug> ip = %s", st_dtu_file_t.ip);

    //返回文件首地址
    return 0;
}

/**
  * Function    : dtu_trans_conf_file_write
  * Description : 云服务器信息写入文件
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_trans_conf_file_write(DTU_FILE_PARAM_T* st_dtu_file)
{
   int fd = 0;

   //打开文件
   fd = FDI_fopen("trans_file", "wb");
   //如果文件存在，写入，并关闭文件
   if (fd != 0)
   {
        printf("%s[%d] open file for write OK", __FUNCTION__, __LINE__);
        FDI_fwrite(st_dtu_file, sizeof(DTU_FILE_PARAM_T), 1, fd);
        FDI_fclose(fd);
   }
   //文件不存在，报错
   else
   {
        printf("%s[%d] open file for write error", __FUNCTION__, __LINE__);
        return -1;
   }
   
   return 0;
}

// End of file : am_file.c 2023-8-28 11:43:45 by: zhaoning 

