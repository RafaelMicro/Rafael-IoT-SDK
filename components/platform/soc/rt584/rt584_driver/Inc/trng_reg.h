/***********************************************************************************************************************
 * @file     trng_reg.h
 * @version
 * @brief    true random number generator register definition header file
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup TRNG_Register TRNG
*  @ingroup  peripheral_group
*  @{
*  @brief  TRNG_Register header information
*/
#ifndef TRNG_REG_H
#define TRNG_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef struct trng_ctrl_struct
{

    __IO uint32_t       trng0;          /*offset:0x00*/
    __IO uint32_t       trng1;          /*offset:0x04*/
    __I  uint32_t       trng2;          /*offset:0x08*/
    __IO uint32_t       trng3;          /*offset:0x0C*/

} TRNG_T;

#define  TRNG_ENABLE          (1UL << 0)
#define  TRNG_INTR_CLEAR      (1UL << 1)

#define  TRNG_SEL             (1UL << 0)
#define  TRNG_INTR_ENABLE     (1UL << 1)

#define  TRNG_BUSY            (1UL << 0)
#define  TRNG_INTR_STATUS     (1UL << 1)

/*@}*/ /* end of peripheral_group TRNG_Register */
#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* end of _RT584_TRNG_REG_H */
