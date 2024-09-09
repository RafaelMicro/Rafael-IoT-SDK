/**************************************************************************//**
 * @file     i2c_master_reg.h
 * @version  V1.00
 * @brief    i2c master register definition header file
 *
 * @copyright
 *****************************************************************************/
/** @defgroup I2C_Master_Register I2C_Master
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  I2C_Master_Register header information
*/
#ifndef I2C_MATER_REG_H
#define I2C_MATER_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef struct
{
    __IO  uint32_t  control;            /*0x00*/
    __IO  uint32_t  tar;                /*0x04*/
    __IO  uint32_t  buf;                /*0x08*/
    __I   uint32_t  int_status;         /*0x0c*/
    __IO  uint32_t  int_enable;         /*0x10*/
    __I   uint32_t  int_raw_status;     /*0x14*/
    __IO  uint32_t  int_clear;          /*0x18*/
    __IO  uint32_t  slck_gen;           /*0x1c*/
} I2C_MASTER_T;

/*@}*/ /* end of peripheral_group I2C_Master_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_I2C_MATER_REG_H__ */