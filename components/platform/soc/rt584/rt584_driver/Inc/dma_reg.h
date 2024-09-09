/***********************************************************************************************************************
 * @file     dma_reg.h
 * @version
 * @brief    DMA Register defined
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup DAM_Register DMA
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  DMA_Register header information
*/
#ifndef DMA_REG_H
#define DMA_REG_H


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef struct
{
    __IO  uint32_t   dma_src_adr;            /*offset:0x00*/
    __IO  uint32_t   dma_dest_adr;           /*offset:0x04*/
    __IO  uint32_t   dma_bytes_num;          /*offset:0x08*/
    __IO  uint32_t   dma_control;            /*offset:0x0c*/
    __IO  uint32_t   dma_int;                /*offset:0x10*/
    __IO  uint32_t   dma_port;               /*offset:0x14*/
} DMA_T;


#define  DMA_START_ENABLE                (1<<0)
#define  DMA_BUSY                        (1<<8)

#define  DMA_CHL_INT_STATUS              (1<<0)
#define  DMA_CHL_WAKEUP_STATUS           (1<<1)

#define  DMA_CHL_INT_ENABLE              (1<<8)
#define  DMA_CHL_INT_WAKEUP              (1<<9)

#define  DMA_CHL_INT_CLEAR_CLR           (1<<16)
#define  DMA_CHL_INT_WAKEUP_CLR          (1<<17)


/*@}*/ /* end of peripheral_group DMA_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_DMA_REG_H__ */
