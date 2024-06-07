/**
 * @file ringbuf.c
 * @brief 环形缓冲区接口封装
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "plat_osl.h"
#include "ringbuf.h"
#include "common/log.h"

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/
struct at_ringbuf_t
{
    uint8_t *buf;
    uint32_t buf_len;
    int16_t  rd_offset;
    int16_t  wr_offset;
};

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
void *ringbuf_create(uint32_t buf_size)
{
    struct at_ringbuf_t *ringbuf = NULL;

    if (NULL == (ringbuf = osl_malloc(sizeof(*ringbuf))))
    {
        return NULL;
    }
    if (NULL == (ringbuf->buf = osl_malloc(buf_size)))
    {
        osl_free(ringbuf);
        return NULL;
    }
    ringbuf->buf_len   = buf_size;
    ringbuf->rd_offset = 0;
    ringbuf->wr_offset = 0;

    return (void *)ringbuf;
}

void ringbuf_delete(void *rb)
{
    struct at_ringbuf_t *ringbuf = (struct at_ringbuf_t *)rb;

    if (ringbuf)
    {
        if (ringbuf->buf)
        {
            osl_free(ringbuf->buf);
        }
        osl_free(ringbuf);
    }
}

int32_t ringbuf_get(void *rb, uint8_t *buf, uint32_t buf_len)
{
    struct at_ringbuf_t *ringbuf = (struct at_ringbuf_t *)rb;

    uint32_t wr_offset = ringbuf->wr_offset;
    uint32_t rd_offset = ringbuf->rd_offset;
    uint32_t avaliable_bytes =
        (wr_offset >= rd_offset) ? (wr_offset - rd_offset) : (ringbuf->buf_len - rd_offset + wr_offset);
    uint32_t recv_len = (avaliable_bytes < buf_len) ? avaliable_bytes : buf_len;
    uint32_t i        = 0;

    for (i = 0; i < recv_len; i++)
    {
        buf[i]    = ringbuf->buf[rd_offset];
        rd_offset = (rd_offset + 1) % ringbuf->buf_len;
    }
    ringbuf->rd_offset = rd_offset;

    return recv_len;
}

int32_t ringbuf_put(void *rb, uint8_t *buf, uint32_t buf_len)
{
    struct at_ringbuf_t *ringbuf = (struct at_ringbuf_t *)rb;

    uint32_t wr_offset = ringbuf->wr_offset;
    uint32_t i         = 0;

    for (i = 0; i < buf_len; i++)
    {
        ringbuf->buf[wr_offset] = buf[i];
        wr_offset               = (wr_offset + 1) % ringbuf->buf_len;
    }
    ringbuf->wr_offset = wr_offset;

    return buf_len;
}

void ringbuf_reset(void *rb)
{
    struct at_ringbuf_t *ringbuf = (struct at_ringbuf_t *)rb;

    ringbuf->wr_offset = ringbuf->rd_offset;
}
