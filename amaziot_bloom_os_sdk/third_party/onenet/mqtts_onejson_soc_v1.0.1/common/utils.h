/**
 * Copyright (c), 2012~2018 iot.10086.cn All Rights Reserved
 * @file        utils.h
 * @brief       一些常用数据处理函数封装。
 */

#ifndef __UTILS_H__
#define __UTILS_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*****************************************************************************/
    /* External Definition（Constant and Macro )                                 */
    /*****************************************************************************/

    /*****************************************************************************/
    /* External Structures, Enum and Typedefs                                    */
    /*****************************************************************************/

    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/
    uint16_t set_16bit_le(uint8_t* buf, uint16_t val);
    uint16_t get_16bit_le(uint8_t* buf, uint16_t* val);

    uint16_t set_16bit_be(uint8_t* buf, uint16_t val);
    uint16_t get_16bit_be(uint8_t* buf, uint16_t* val);

    uint16_t set_32bit_le(uint8_t* buf, uint32_t val);
    uint16_t get_32bit_le(uint8_t* buf, uint32_t* val);

    uint16_t set_32bit_be(uint8_t* buf, uint32_t val);
    uint16_t get_32bit_be(uint8_t* buf, uint32_t* val);

    void    str_to_hex(uint8_t* strHex, uint8_t* str, uint16_t len);
    int32_t hex_to_str(uint8_t* strHex, uint8_t* str, uint16_t len);
    int32_t rand_number(int32_t min, int32_t max);
#ifdef __cplusplus
}
#endif

#endif
