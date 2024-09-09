/***********************************************************************************************************************
 * @file     XDMA_reg.h
 * @version
 * @brief    XDMA register definition header file
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup XDMA_Register XDMA
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  XDMA_Register header information
*/

#ifndef XDMA_REG_H
#define XDMA_REG_H

#if defined (__CC_ARM)
#pragma anon_unions
#endif



typedef struct
{
    __IO uint32_t xdma_ctl0;        /*offset:0x00*/
    __O  uint32_t xdma_ctl1;        /*offset:0x04*/

} XDMA_T;


/***********************************************************************************************************************
 *    CONSTANTS AND DEFINES
 **********************************************************************************************************************/
#define XDMA_ENABLE            (1UL<<0)
#define XDMA_RESET             (1UL<<0)



/*@}*/ /* end of peripheral_group XDMA_Register */
#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* end of _RT584_XDMA_REG_H */


