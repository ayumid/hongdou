/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file plat_osl.h
 * @brief 标准库通用接口封装。
 */

#ifndef __PLAT_OSL_H__
#define __PLAT_OSL_H__

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
  void *osl_malloc(size_t size);
  void *osl_calloc(size_t num, size_t size);
  void osl_free(void *ptr);

  void *osl_memcpy(void *dst, const void *src, size_t n);
  void *osl_memmove(void *dst, const void *src, size_t n);
  void *osl_memset(void *dst, int32_t val, size_t n);

  uint8_t *osl_strdup(const uint8_t *s);
  uint8_t *osl_strndup(const uint8_t *s, size_t n);
  uint8_t *osl_strcpy(uint8_t *s1, const uint8_t *s2);
  uint8_t *osl_strcat(uint8_t *dst, const uint8_t *src);
  uint8_t *osl_strstr(const uint8_t *s1, const uint8_t *s2);

  uint32_t osl_strlen(const uint8_t *s);
  int32_t osl_strcmp(const uint8_t *s1, const uint8_t *s2);
  int32_t osl_strncmp(const uint8_t *s1, const uint8_t *s2, size_t n);
  int32_t osl_sprintf(uint8_t *str, const uint8_t *format, ...);
  int32_t osl_sscanf(const uint8_t *str, const uint8_t *format, ...);
  void osl_assert(boolean expression);
  int32_t osl_get_random(unsigned char *buf, size_t len);

  int32_t osl_atoi(const uint8_t *nptr);

  

  /// @brief  随机生成位于闭区间[min,max]的数
  /// @param  min 最小值
  /// @param  max 最大值
  /// @return  随机数
  int32_t osl_rand(int32_t min, int32_t max);

  /// @brief  随机生成长度为len的字符串
  /// @param  buf 字符串缓存地址
  /// @param  len 长度
  /// @return  随机字符串
  uint8_t *osl_random_string(uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif

#endif
