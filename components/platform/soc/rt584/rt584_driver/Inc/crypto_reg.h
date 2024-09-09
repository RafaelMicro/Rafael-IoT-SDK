/**************************************************************************//**
 * @file     crypto_reg.h
 * @version  V1.00
 * @brief    CRYPTO ACCELEATOR register definition header file
 *
 * @copyright
 *****************************************************************************/
#ifndef CRYPTO_REG_H
#define CRYPTO_REG_H



/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup   CRYPTO ACCELEATOR Controller
    Memory Mapped Structure for CRYPTO ACCELEATOR Controller
@{ */


typedef union crypto_ctrl_struct
{

    struct crypto_ctrl_b
    {
        __IO uint32_t vlw_op_num     : 8;
        __IO uint32_t vlw_sb_num     : 5;
        __IO uint32_t reserved1      : 3;
        __IO uint32_t en_crypto      : 1;
        __IO uint32_t enable_sha     : 1;
        __IO uint32_t reserved2      : 6;
        __IO uint32_t crypto_done    : 1;
        __IO uint32_t sha_done       : 1;
        __IO uint32_t crypto_busy    : 1;
        __IO uint32_t sha_busy       : 1;
        __IO uint32_t reserved3      : 3;
        __IO uint32_t clr_crypto_int : 1;
    } bit;

    uint32_t reg;

} crypto_ctrl_t;

typedef struct
{
    __IO crypto_ctrl_t  crypto_cfg;
    __IO uint32_t       sha_digest_base;
    __IO uint32_t       sha_k_base;
    __IO uint32_t       sha_dma_base;
    __IO uint32_t       sha_dma_length;
    __IO uint32_t       sha_misc_cfg;
} crypto_t;


/**
    @addtogroup CRYPTO ACCELEATOR REGISTER BIT DEFINITIONS

@{ */



/**@}*/ /* end of CRYPTO ACCELEATOR REGISTER BIT DEFINITIONS */


/**@}*/ /* end of CRYPTO ACCELEATOR Controller */
/**@}*/ /* end of REGISTER group */



#endif
