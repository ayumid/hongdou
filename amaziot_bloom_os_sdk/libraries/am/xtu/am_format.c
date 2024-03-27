//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_format.c
// Auther      : zhaoning
// Version     :
// Date : 2023-12-7
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-12-7
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "am_format.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

void dtu_format_send(char * raw_data, UINT16 len)
{
    char *data_format = NULL;
    UINT32 format_size = 0;
    UINT8 buf[DTU_IMEI_LEN + 1] = {0};
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;

    dtu_file_ctx = dtu_get_file_ctx();
    SDK_GetDevImei(buf ,DTU_IMEI_LEN + 1);
    uprintf("raw_data:%s",raw_data);
    if(dtu_file_ctx->format.type == 1)
    {
        format_size = len;
        format_size += sizeof(DTU_FORMAT_HEAD_T) + sizeof(DTU_FORMAT_TAIL_T);
        data_format = malloc(format_size + 1);

        DTU_FORMAT_HEAD_T header = {0};
        DTU_FORMAT_TAIL_T tail = {0};
        
        header.header = dtu_file_ctx->format.header;
        header.len = 0;
        header.timestamp = utils_utc8_2_timestamp();
        memcpy(header.imei, buf, DTU_IMEI_LEN);
        header.num++;
        tail.tailer = dtu_file_ctx->format.tailer;

        memcpy(data_format, (void*)&header, sizeof(DTU_FORMAT_HEAD_T));
        memcpy(data_format + sizeof(DTU_FORMAT_HEAD_T), (void*)raw_data, len);
        memcpy(data_format + sizeof(DTU_FORMAT_HEAD_T) + len, (void*)&tail, sizeof(DTU_FORMAT_TAIL_T));

        uprintf("%s[%d] senddata len: %d\n", __FUNCTION__, __LINE__, format_size);
        //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
        dtu_socket_write(data_format, format_size);
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
        dtu_mqtt_send(data_format, format_size);
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */


        if(NULL != data_format)
        {
            free(data_format);
            data_format = NULL;
        }
    }
    else if(dtu_file_ctx->format.type == 2)
    {
        cJSON *format = NULL;
        char* out = NULL;
        char* send = NULL;
        
        //创建json对象
        format = cJSON_CreateObject();

        //按照协议，对象内加入成员
        cJSON_AddItemToObject(format, "imei", cJSON_CreateString((char*)buf));
        cJSON_AddItemToObject(format, "timestamp", cJSON_CreateNumber(utils_utc8_2_timestamp()));
        cJSON_AddItemToObject(format, "raw_data", cJSON_CreateRaw(raw_data));
        
        //申请内存
        out = cJSON_Print(format);
        if(NULL != out)
        {
            send = cJSON_PrintUnformatted(out);  // 生成不带空格的JSON字符串
            if(NULL != send)
            {
                uprintf("send:%s",send);

                //发送数据到服务器
#ifdef DTU_BASED_ON_TCP
                dtu_socket_write(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_TCP.2023-11-3 15:46:57 by: zhaoning */
#ifdef DTU_BASED_ON_MQTT
                dtu_mqtt_send(send, strlen(send));
#endif /* ifdef DTU_BASED_ON_MQTT.2023-10-30 14:24:51 by: zhaoning */
                free(send);
                send = NULL;
            }
            free(out);
            out = NULL;
        }
        //删除json对象
        cJSON_Delete(format);
        format = NULL;
    }
    
}

// End of file : am_format.c 2023-12-7 11:50:10 by: zhaoning 

