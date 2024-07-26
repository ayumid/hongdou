//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_ty_sw_file.c
// Auther      : zhaoning
// Version     :
// Date : 2023-3-18
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-3-18
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "FDI_TYPE.h"
#include "FDI_FILE.h"
#include "am_ty_sw_file.h"
#include "utils_common.h"


// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

trans_conf transconf;//weihu yi ge save in flash

// functions prototypes -------------------------------------------------

// Functions --------------------------------------------------------------------

/*function for file*/
void trans_conf_file_init(void)
{
    int fd = 0;
    //init transconf0
    //init_trans_conf0();
    fd = FDI_fopen("trans_file", "rb");
    if (fd)
    {
      printf("FDI_fopen is OK!");
      transconf = trans_conf_file_read(fd);
    }
    else
    {
      printf("FDI_fopen is ERROR!");
      //init write tans_file
      transconf = init_trans_conf(transconf);
      trans_conf_file_write(transconf);
    }  
}

trans_conf trans_conf_file_read(int fd)
{
    trans_conf transconf = {0};
    
    FDI_fread(&transconf,sizeof(transconf),1,fd);
    FDI_fclose(fd);
    
    return transconf;
}

int trans_conf_file_write(trans_conf transconf)
{
   int fd = 0;
   fd = FDI_fopen("trans_file", "wb");
   if (fd != 0)
   {
        printf("debug> open file for write OK");
        FDI_fwrite(&transconf,sizeof(transconf), 1, fd);
           FDI_fclose(fd);
        printf("debug> trans_file write OK");
   }
   else
   {
        printf("debug> open file for write error");
        return -1;
   }
   return 0;
}

trans_conf init_trans_conf(trans_conf transconf)
{
    transconf.licenseLen = 0;
    sprintf(transconf.license,"");
    sprintf(transconf.deviceId,"");
    sprintf(transconf.factory_apikey,"");
    sprintf(transconf.sta_mac,"");
    sprintf(transconf.sap_mac,"");
    sprintf(transconf.device_model,"");
    
    sprintf(transconf.linshi,"https://disp.coolkit.cn/dispatch/device");
    sprintf(transconf.zhengshi,"");

    
    transconf.initstatus0 = 0;
    transconf.initstatus1 = 0;
    transconf.initstatus2 = 0;
    transconf.initstatus3 = 0;
    transconf.activeFlag = 0;

    transconf.sdstatus0 = 0;
    transconf.sdstatus1 = 0;
    transconf.sdstatus2 = 0;
    transconf.sdstatus3 = 0;

    transconf.pulsetime0 = 0;
    transconf.pulsetime1 = 0;
    transconf.pulsetime2 = 0;
    transconf.pulsetime3 = 0;

    transconf.sledstatus = 1;
    transconf.lockstatus = 0;
    
    return transconf;
}

bool transfile_is_null(void)
{
    if(transconf.licenseLen == 0 || strlen(transconf.license)==0 ||strlen(transconf.deviceId)==0 ||strlen(transconf.factory_apikey)==0||strlen(transconf.sta_mac)==0||strlen(transconf.sap_mac)==0||strlen(transconf.device_model)==0)
    {
        return true;   
    }
    return false;
}

// End of file : am_ty_sw_file.h 2023-3-18 17:12:08 by: zhaoning 

