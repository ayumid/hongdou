/**
 * Copyright (c), 2012~2021 iot.10086.cn All Rights Reserved
 *
 * @file data_check.c
 * @brief 校验码计算
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_check.h"

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
uint16_t check_get_crc16(uint8_t *buf, uint32_t buf_len)
{
    uint16_t wcrcin = 0xFFFF;
    uint16_t wcpoly = 0x1021;
    int32_t  i;

    while (buf_len--)
    {
        wcrcin ^= (*buf++ << 8);

        for (i = 0; i < 8; i++)
        {
            if (wcrcin & 0x8000)
            {
                wcrcin = (wcrcin << 1) ^ wcpoly;
            }
            else
            {
                wcrcin = wcrcin << 1;
            }
        }
    }
    return wcrcin;
}

uint32_t check_get_crc32(uint8_t *buf, uint32_t buf_len)
{
    return 0;
}
