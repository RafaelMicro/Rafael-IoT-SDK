/***********************************************************************************************************************
 * @file     RT584_XDMA.h
 * @version
 * @brief     xdma driver header file
 *
 * @copyright
 **********************************************************************************************************************/
/**
* @defgroup XDMA_group XDMA
* @ingroup peripheral_group
* @{
* @brief  Define XDMA definitions, structures, and functions
*/
#ifndef XDMA_H
#define XDMA_H


#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************************************************************
 *    GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
* @brief xdma_enable
*
* @param dma_channel_id   Specifies the dma channel number.
* @param app_dma_callback
*/
__STATIC_INLINE void xdma_enable(void)
{
    XDMA->xdma_ctl0 |= XDMA_ENABLE;
}
/**
 * @brief xdma_disable
 *
 * @param dma_channel_id   Specifies the dma channel number.
 * @param app_dma_callback
 */
__STATIC_INLINE void xdma_disable(void)
{
    XDMA->xdma_ctl0 &= ~XDMA_ENABLE;
}
/**
 * @brief Dxdma_reset
 *
 * @param dma_channel_id   Specifies the dma channel number.
 * @param app_dma_callback
 */
__STATIC_INLINE void xdma_reset(void)
{
    XDMA->XDMA_CTL1 |= XDMA_RESET;
}

#ifdef __cplusplus
}
#endif

#endif /* end of _RT584_XDMA_H_ */
/*@}*/ /* end of peripheral_group XDMA */

