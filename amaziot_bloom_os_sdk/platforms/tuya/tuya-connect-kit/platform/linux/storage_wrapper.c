#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "tuya_log.h"
#include "tuya_error_code.h"
#include "storage_interface.h"
#include "system_interface.h"
#include "osa.h"
#include "FDI_TYPE.h"
#include "FDI_FILE.h"

int local_storage_set(const char* key, const uint8_t* buffer, size_t length)
{
    if (NULL == key || NULL == buffer) {
        return OPRT_INVALID_PARM;
    }

    FILE_ID fptr = 0;
    printf("key:%s", key);
    fptr = FDI_fopen(key, "wb+");
    if (NULL == fptr) {
        printf("open file error");
        return OPRT_COM_ERROR;
    } else {
        printf("open file OK");
    }

    int file_len = FDI_fwrite(buffer, 1, length, fptr);
    FDI_fclose(fptr);
    if (file_len != length) {
        printf("uf_kv_write fail %d", file_len);
        return OPRT_COM_ERROR;
    }
    return OPRT_OK;
}

int local_storage_get(const char* key, uint8_t* buffer, size_t* length)
{
    if (NULL == key || NULL == buffer || NULL == length) {
        return OPRT_INVALID_PARM;
    }

    printf("key:%s, len:%d", key, (int)*length);
    FILE_ID fptr = FDI_fopen(key, "rb");
    if (0 == fptr) {
        *length = 0;
        printf("cannot open file");
        return OPRT_COM_ERROR;
    }

    int read_len = *length; // ?
    read_len = FDI_fread(buffer, 1, (size_t)read_len, fptr);
    FDI_fclose(fptr);
    if (read_len <= 0) {
        *length = 0;
        printf("read error %d", read_len);
        return OPRT_COM_ERROR;
    }

    *length = read_len;
    return OPRT_OK;
}

int local_storage_del(const char* key)
{
    printf("key:%s", key);
    if (remove(key) == 0) {
        printf("Deleted successfully");
        return OPRT_OK;
    } else {
        printf("Unable to delete the file");
        return OPRT_COM_ERROR;
    }
}

#ifdef __cplusplus
}
#endif
