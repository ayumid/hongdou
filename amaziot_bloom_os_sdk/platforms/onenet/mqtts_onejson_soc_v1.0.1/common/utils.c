/**
 * Copyright (c), 2012~2018 iot.10086.cn All Rights Reserved
 * @file        utils.c
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/

/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
uint16_t set_16bit_le(uint8_t* buf, uint16_t val)
{
    buf[0] = val & 0xFF;
    buf[1] = (val >> 8) & 0xFF;
    return 2;
}

uint16_t get_16bit_le(uint8_t* buf, uint16_t* val)
{
    *val = (buf[1] << 8) | buf[0];
    return 2;
}

uint16_t set_16bit_be(uint8_t* buf, uint16_t val)
{
    buf[0] = (val >> 8) & 0xFF;
    buf[1] = val & 0xFF;
    return 2;
}

uint16_t get_16bit_be(uint8_t* buf, uint16_t* val)
{
    *val = (buf[0] << 8) | buf[1];
    return 2;
}

uint16_t set_32bit_le(uint8_t* buf, uint32_t val)
{
    buf[0] = val & 0xFF;
    buf[1] = (val >> 8) & 0xFF;
    buf[2] = (val >> 16) & 0xFF;
    buf[3] = (val >> 24) & 0xFF;
    return 4;
}

uint16_t get_32bit_le(uint8_t* buf, uint32_t* val)
{
    *val = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
    return 4;
}

uint16_t set_32bit_be(uint8_t* buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
    return 4;
}

uint16_t get_32bit_be(uint8_t* buf, uint32_t* val)
{
    *val = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    return 4;
}

int32_t hex_to_str(uint8_t* strHex, uint8_t* str, uint16_t len)
{
    uint16_t i, j, k;
    uint8_t  data_buf[2] = { 0 };

    for (i = 0, j = 0; i < len; i++) {
        for (k = 0; k < 2; k++) {
            if ((strHex[j] >= '0') && (strHex[j] <= '9'))
                data_buf[k] = strHex[j] - '0';
            else if ((strHex[j] >= 'a') && (strHex[j] <= 'f'))
                data_buf[k] = strHex[j] - 'a' + 10;
            else if ((strHex[j] >= 'A') && (strHex[j] <= 'F'))
                data_buf[k] = strHex[j] - 'A' + 10;
            else
                return -1;
            j++;
        }
        str[i] = ((data_buf[0] << 4) | data_buf[1]);
    }

    return 0;
}

void str_to_hex(uint8_t* strHex, uint8_t* str, uint16_t Len)
{
    char ddl, ddh;
    int  i;

    for (i = 0; i < Len; i++) {
        ddh = 48 + str[i] / 16;
        ddl = 48 + str[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        strHex[i * 2]     = ddh;
        strHex[i * 2 + 1] = ddl;
    }
    strHex[Len * 2] = '\0';
}

#ifndef RAND_MAX
#define RAND_MAX 2147483647
#endif

int32_t rand_number(int32_t min, int32_t max)
{

    static int32_t s_seed = 0;
    assert(max > min);

    if (s_seed == 0) {
        s_seed = time(NULL);
        srand(s_seed);
    }

    int32_t _rand = rand();
    _rand         = min + (int32_t)((double)((double)(max) - (min) + 1.0) * ((_rand) / ((RAND_MAX) + 1.0)));
    return _rand;
}