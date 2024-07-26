//------------------------------------------------------------------------------
// Copyright , 2017-2022 奇迹物联（北京）科技有限公司
// Filename    : am_file.h
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
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _AM_TY_SW_FILE_H_
#define _AM_TY_SW_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include <stdbool.h>

// Public defines / typedef -----------------------------------------------------

typedef struct
{
    char deviceId[16];
    char factory_apikey[64];
    char sta_mac[32];
    char sap_mac[32];
    char device_model[16];
    
    char linshi[64];
    char zhengshi[64];
    
    UINT8 initstatus0;
    UINT8 initstatus1;
    UINT8 initstatus2;
    UINT8 initstatus3;
    UINT8 activeFlag;

    UINT8 sdstatus0;
    UINT8 sdstatus1;
    UINT8 sdstatus2;
    UINT8 sdstatus3;
    
    char license[256];
    UINT16 licenseLen;

    int pulsetime0;
    int pulsetime1;
    int pulsetime2;
    int pulsetime3;

    UINT8 sledstatus;
    UINT8 lockstatus;
    
}trans_conf;

// Public functions prototypes --------------------------------------------------

void trans_conf_file_init(void);
trans_conf trans_conf_file_read(int fd);
int trans_conf_file_write(trans_conf transconf);
trans_conf init_trans_conf(trans_conf transconf);
bool transfile_is_null(void);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _AM_TY_SW_FILE_H_.2023-3-18 17:20:14 by: zhaoning */

