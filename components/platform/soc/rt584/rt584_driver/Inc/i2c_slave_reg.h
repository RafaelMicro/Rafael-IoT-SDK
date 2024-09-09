/**************************************************************************//**
 * @file     i2c_slave_reg.h
 * @version  V1.00
 * @brief    i2c slave register definition header file
 *
 * @copyright
 *****************************************************************************/
/** @defgroup I2C_Slave_Register I2C_Slave
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  I2C_Slave_Register header information
*/
#ifndef I2C_SLAVE_REG_H
#define I2C_SLAVE_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef struct
{
    __IO  uint32_t  rx_data;             /*0x00*/
    __IO  uint32_t  i2c_slave_addr;      /*0x04*/
    __IO  uint32_t  i2c_int_enable;      /*0x08*/
    __IO  uint32_t  i2c_int_status;      /*0x0c*/
    __IO  uint32_t  i2c_timeout;         /*0x10*/
    __IO  uint32_t  i2c_slave_enable;    /*0x14*/
    __I   uint32_t  i2c_slave_status;    /*0x18*/
} I2C_SLAVE_T;

/**
 *  @Brief I2C Slave interrupt register definitions
 */
#define  WR_DATA     RX_DATA

/*@}*/ /* end of peripheral_group I2C_Slave_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_I2C_SLAVE_REG_H__ */
