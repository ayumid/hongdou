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
#ifdef DTU_BASED_ON_TCP
    //初始化连接类型
    st_dtu_file_t.socket.type = 0;
    //初始化默认IP，onenet 多协议接入 tcp
    sprintf(st_dtu_file_t.socket.ip,"183.230.40.40");
    //初始化端口
    st_dtu_file_t.socket.port = 1811;
#endif /* ifdef DTU_BASED_ON_TCP.2023-10-27 15:58:38 by: zhaoning */

#ifdef DTU_BASED_ON_MQTT
    sprintf(st_dtu_file_t.mqtt.ip,"183.230.40.39");
    st_dtu_file_t.mqtt.port = 6002;
    
    sprintf(st_dtu_file_t.mqtt.clientid,"741522867");
    st_dtu_file_t.mqtt.keeplive = 120;
    st_dtu_file_t.mqtt.cleansession = 0;
    sprintf(st_dtu_file_t.mqtt.username,"444950");
    sprintf(st_dtu_file_t.mqtt.password,"123456");

    sprintf(st_dtu_file_t.mqtt.subtopic,"aaa");
    sprintf(st_dtu_file_t.mqtt.subtopic1,"");
    sprintf(st_dtu_file_t.mqtt.subtopic2,"");
    st_dtu_file_t.mqtt.subflag = 1;
    st_dtu_file_t.mqtt.subqos = 0;

    
    sprintf(st_dtu_file_t.mqtt.pubtopic,"aaa");
    st_dtu_file_t.mqtt.pubqos = 0;
    st_dtu_file_t.mqtt.duplicate = 0;
    st_dtu_file_t.mqtt.retain = 0;
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-27 16:52:32 by: zhaoning */

    //设置默认上报心跳
    st_dtu_file_t.hb.heartflag = 1;
    //设置默认心跳内容为hearttest
    sprintf(st_dtu_file_t.hb.heart,"hearttest");
    //设置心跳间隔时间为30s
    st_dtu_file_t.hb.hearttime = 30;
    //设置默认上报注册包
    st_dtu_file_t.reg.linkflag = 1;
    //设置注册包内容为"*275619#amaziot4000mt#AP4000MT*"，内容是根据onenet 多协议接入，tcp配置的脚本上报规则，上报后，onenet会不停下发
    sprintf(st_dtu_file_t.reg.link,"*275619#amaziot4000mt#AP4000MT*");
    //配置网络AT指令默认前缀是"am.iot"
    sprintf(st_dtu_file_t.net_at.cmdpw,"am.iot");

#ifdef DTU_TYPE_3IN1
    //三合一切卡，默认卡是
    st_dtu_file_t.sim.sim = 1;
    //sim卡禁止切卡标志
    st_dtu_file_t.sim.simlock = 0;
#endif

#ifdef DTU_TYPE_GNSS_INCLUDE
    //默认不上报gnss语句
    st_dtu_file_t.gnss.gpsflag = 0;
#endif

#ifdef DTU_TYPE_DODIAI_INCLUDE
    //初始化ai检测时间（do di都是at指令下发后开始定时器，ai是上电自动发起）
    st_dtu_file_t.ai.ai_interval = 15;
#endif /* ifdef DTU_TYPE_DODIAI_INCLUDE.2023-10-27 11:58:40 by: zhaoning */

#ifdef DTU_TYPE_MODBUS_INCLUDE
    //配置modbus参数
    //默认关闭modbus
    st_dtu_file_t.modbus.config.type = 0;
    //默认一条指令等待时间 1s
    st_dtu_file_t.modbus.config.wait = 1;
    //默认指令之间间隔 1s
    st_dtu_file_t.modbus.config.interval = 1;
    //默认指令都发送完毕后休息时间 5s
    st_dtu_file_t.modbus.config.delay = 5;
#endif /* ifdef DTU_TYPE_MODBUS_INCLUDE.2023-10-10 10:27:59 by: zhaoning */

#ifdef DTU_TYPE_HTTP_INCLUDE
    st_dtu_file_t.http.config.type = 0;
    st_dtu_file_t.http.http1.type = 0;
    memset(st_dtu_file_t.http.http1.head1, 0, DTU_HTTP_HEAD_MAX_LEN);
    memset(st_dtu_file_t.http.http1.head2, 0, DTU_HTTP_HEAD_MAX_LEN);
    memset(st_dtu_file_t.http.http1.head3, 0, DTU_HTTP_HEAD_MAX_LEN);
    st_dtu_file_t.http.http2.type = 0;
    memset(st_dtu_file_t.http.http2.head1, 0, DTU_HTTP_HEAD_MAX_LEN);
    memset(st_dtu_file_t.http.http2.head2, 0, DTU_HTTP_HEAD_MAX_LEN);
    memset(st_dtu_file_t.http.http2.head3, 0, DTU_HTTP_HEAD_MAX_LEN);
#endif /* ifdef DTU_TYPE_HTTP_INCLUDE.2023-10-10 10:10:43 by: zhaoning */

#ifdef DTU_TYPE_FORMAT_INCLUDE
    st_dtu_file_t.format.type = 0;
    st_dtu_file_t.format.header = 0x3F3F;
    st_dtu_file_t.format.tailer = 0xF3F3;
    
#endif /* ifdef DTU_TYPE_FORMAT_INCLUDE.2023-12-7 15:40:58 by: zhaoning */

#ifdef DTU_TYPE_LORA_INCLUDE
    st_dtu_file_t.lora.type = 0;
    st_dtu_file_t.lora.wait = 5;
    st_dtu_file_t.lora.interval = 3;
    st_dtu_file_t.lora.delay = 5;
#endif /* ifdef DTU_TYPE_LORA_INCLUDE.2024-1-12 15:40:05 by: zhaoning */
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
//    uprintf("debug> ip = %s", st_dtu_file_t.ip);

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
        uprintf("%s[%d] open file for write OK", __FUNCTION__, __LINE__);
        FDI_fwrite(st_dtu_file, sizeof(DTU_FILE_PARAM_T), 1, fd);
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

// End of file : am_file.c 2023-8-28 11:43:45 by: zhaoning 

