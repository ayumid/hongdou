/**
 * @file ringbuf.h
 * @brief
 */

#ifndef __RINGBUF_H__
#define __RINGBUF_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/
/**
 * @brief 创建环形缓冲区
 *
 * @param buf_size：缓冲区大小（字节）
 * @return 成功 - 缓冲区结构体指针，失败 - NULL
 */
void *ringbuf_create(uint32_t buf_size);

/**
 * @brief 删除环形缓冲区
 *
 * @param rb：缓冲区结构体指针
 * @return 无
 */
void ringbuf_delete(void *rb);

/**
 * @brief 从缓冲区获取指定字节长度数据
 *
 * @param rb：缓冲区结构体指针
 * @param buf：获取数据的存放地址
 * @param buf_len：需要获取的数据长度
 * @return >0 - 成功获取的数据长度，其他 - 获取失败
 */
int32_t ringbuf_get(void *rb, uint8_t *buf, uint32_t buf_len);

/**
 * @brief 存放指定字节长度数据到缓冲区
 *
 * @param rb：缓冲区结构体指针
 * @param buf：需要存放的数据
 * @param buf_len：需要存放的数据长度
 * @return >0 - 成功存放的数据长度，其他 - 存放失败
 */
int32_t ringbuf_put(void *rb, uint8_t *buf, uint32_t buf_len);

/**
 * @brief 清空环形缓冲区
 *
 * @param rb：缓冲区结构体指针
 */
void ringbuf_reset(void *rb);

#ifdef __cplusplus
}
#endif

#endif
