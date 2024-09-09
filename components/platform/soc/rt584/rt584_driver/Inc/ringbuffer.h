/***********************************************************************************************************************
 * @file     RT584_ringbuffer.h
 * @version
 * @brief    RT584_ring buffer API
 *
 * @copyright
 **********************************************************************************************************************/
/**
* @defgroup UART_group UART ringbuffer
* @ingroup peripheral_group
* @{
* @brief  ringbuffer definitions, structures, and functions
*/
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************************************************************
 *    MACROS
 **********************************************************************************************************************/
#define bufsize_mask(X)    (X-1)
/***********************************************************************************************************************
 *    TYPEDEFS
 **********************************************************************************************************************/
/**
 * @brief ringbuffer typedef
 */
typedef struct
{
    uint8_t  *ring_buf;                 /* size should be 2^N, this value is buffersize-1 */
    uint16_t  bufsize_mask;             /* size should be 2^N, this value is buffersize-1 */
    volatile uint16_t  wr_idx;          /* write index must be volatile*/
    volatile uint16_t  rd_idx;          /* read index must be volatile*/
} ring_buf_t;

/*@}*/ /* end of peripheral_group UART ringbuffer */

#ifdef __cplusplus
}
#endif

#endif      /* end of _RT584_RINGBUFFER_H__ */