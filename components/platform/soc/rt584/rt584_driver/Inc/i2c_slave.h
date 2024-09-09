/**************************************************************************//**
 * @file     i2c_slave.h
 * @version
 * @brief    I2C slave driver header file
 *
 * @copyright
 ******************************************************************************/
/** @defgroup I2C_Slave_Driver I2C_Slave
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  I2C_Slave_Driver header information
*/
#ifndef __RT584_I2C_SLAVE_H__
#define __RT584_I2C_SLAVE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include "mcu.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
/**
 *  @Brief I2C Slave callback status definitions
 */
#define I2C_SLAVE_STATUS_ADDR_MATCH          (1<<0)
#define I2C_SLAVE_STATUS_DATA_READY          (1<<1)
#define I2C_SLAVE_STATUS_STOP                (1<<2)
#define I2C_SLAVE_STATUS_TIMEOUT             (1<<3)
#define I2C_SLAVE_STATUS_READ                (1<<4)
#define I2C_SLAVE_STATUS_WRITE               (1<<5)

/**
 *  @Brief I2C Slave interrupt register definitions
 */
#define  I2C_SLAVE_MATCH_ADDR             (0x01)
#define  I2C_SLAVE_DATA_READY             (0x02)
#define  I2C_SLAVE_BUS_STOP               (0x04)
#define  I2C_SLAVE_BUS_TIMEOUT            (0x08)
#define  I2C_SLAVE_ALL_INT          (I2C_SLAVE_MATCH_ADDR | I2C_SLAVE_DATA_READY | I2C_SLAVE_BUS_STOP | I2C_SLAVE_BUS_TIMEOUT)

/**
 *  @Brief I2C Slave status register definitions
 */
#define  I2C_SLAVE_READ_OP                (1<<1)

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 * @brief I2C slave finish routine callback for user application.
 *
 * @param[in] statue I2C slave transfer status.
 *
 * @details    This callback function is still running in interrupt mode, so this function
 *              should be as short as possible. It can NOT call any block function in this
 *              callback service routine.
 *
 *              This function will be called when I2C slave finished the transfer request, or there
 *              is no ACK during the transfer (error case).
 *
 */
typedef void (*i2c_slave_proc_cb)(uint32_t status);

/**
 *@brief  Structure for the I2C slave configuration.
 */
typedef struct
{
    i2c_slave_proc_cb  i2c_slave_cb_func;  /*!< i2c slave callback function */
    uint8_t  i2c_bus_timeout_enable;         /*!< i2c bus timeout enable */
    uint8_t  i2c_bus_timeout;                /*!< i2c bus timeout value */
    uint8_t  i2c_slave_addr;                 /*!< i2c slave 7 bits only */
} i2c_slave_mode_t;

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
/**
 * @brief Set I2C slave initialize
 *
 * @param[in]
 *
 * @retval
 *       STATUS_SUCCESS
 *       STATUS_INVALID_REQUEST   --- I2C master is not in idle mode.
 *
 * @details
 *       Call this function to initail I2C slave, the whole I2C slave
 *       driver is interrupt-driven, all i2c slave response are processing
 *       in user "i2c_slave_cb_func" callback function. Please Notice the
 *       the function "i2c_slave_cb_func" is in ISR context, so finish the
 *       task ASAP. You can NOT block the function!
 */
uint32_t I2c_Slave_Open(i2c_slave_mode_t *i2c_slave_client);


/**
 * @brief Close I2C slave
 *
 * @param[in]
 *
 * @retval
 *       STATUS_SUCCESS
 *       STATUS_INVALID_REQUEST
 *
 * @details
 *       This function is disable I2C slave client function.
 *
 */
uint32_t I2c_Slave_Close(void);

/**
 *   @brief  Get one byte from i2c master
 *
 *   @details
 *   Notice: I2C read or write status is from I2C Master viewpoint...
 *   That is, when I2C slave get a write flag, slave should read a byte to RX_DATA
 *   Call this function when I2C slave receive a "write flag" Data Ready interrupt
 *
 */

__STATIC_INLINE uint8_t I2c_Slave_Read_Byte(void)
{
    return (I2C_SLAVE->RX_DATA & 0xFF);
}

/**
 *   @brief  Send one byte to i2c master
 *
 *   @details
 *   Notice: I2C read or write status is from I2C Master viewpoint...
 *   That is, when I2C slave get a read flag, slave should write a byte to TX_DATA
 *   Call this function when I2C slave receive a "read flag" Data Ready interrupt
 *
 */
__STATIC_INLINE void I2c_Slave_Write_Byte(uint8_t data)
{
    I2C_SLAVE->WR_DATA = data;
}

/*@}*/ /* end of peripheral_group I2C_Slave_Driver */

#ifdef __cplusplus
}
#endif

#endif      /* end of __RT584_I2C_SLAVE_H__ */

