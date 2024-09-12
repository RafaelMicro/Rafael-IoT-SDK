/**
 * \file            i2c_master.h
 * \brief           i2c master driver header file
 *          
 */
/*
 * Copyright (c) year FirstName LASTNAME
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author:          Kc.tseng
 */

#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "mcu.h"


/**
 * \brief           I2C Master Clock Constant Definitions
 */
#define I2C_CLOCK_400K      0
#define I2C_CLOCK_200K      1
#define I2C_CLOCK_100K      2
#define I2C_CLOCK_800K      3
#define I2C_CLOCK_1M        4
#define I2C_CLOCK_MAX       5

/**
 * \brief           I2C Master return callback constant definitions
 */
#define I2C_STATUS_OK             0
#define I2C_STATUS_ERR_NOACK      1

/**
 * \brief           I2C Master control register bit definitions
 */
#define I2CM_CONTROL_ENABLE             0x01
#define I2CM_CONTROL_RESTART            0x02
#define I2CM_CONTROL_STOP_EN            0x04
#define I2CM_CONTROL_BUS_CLEAR          0x08
#define I2CM_CONTROL_FIFO_CLEAR         0x10

/**
 * \brief           I2C Master interrupt register bit definitions
 */
#define I2CM_INT_RX_UNDER           0x0001
#define I2CM_INT_RX_OVER            0x0002
#define I2CM_INT_RX_FULL            0x0004
#define I2CM_INT_RX_FINISH          0x0008
#define I2CM_INT_TX_OVER            0x0010
#define I2CM_INT_TX_EMPTY           0x0020
#define I2CM_INT_ADDR_NACK          0x0040
#define I2CM_INT_WRITE_NACK         0x0080
#define I2CM_INT_LOSTARB            0x0100
#define I2CM_INT_IDLE               0x0200
#define I2CM_INT_MASK_ALL           0x03FF

/**
 * \brief           I2C Master FIFO number definitionss
 */
#define I2CM_FIFO_NUM           16      /*!< the max fifo number */


/**
 * \brief           I2C finish routine notify callback for user application
 * \param[in]       status: I2C transfer status.
 *                          It must be I2C_STATUS_OK or I2C_STATUS_ERR_NOACK
 */
typedef void (*i2cm_cb_fn)(uint32_t status);

/**
 * \brief           Structure for the I2C master transfer request.
 */
typedef struct
{
    i2cm_cb_fn endproc_cb;                      /*!< I2C complete callback function */
    uint8_t    dev_addr;                        /*!< I2C device address, 7bits only */
    uint8_t    bFlag_16bits;                    /*!< 1 for register address is 
                                                    16bits, 0 for register address 
                                                    is 8bits. */
    uint16_t   reg_addr;                        /*!< I2C register address value */
} i2c_master_mode_t;


/**
 * \brief           I2C master pre-initial function.
 * \param[in]       master_id: Select the I2C master number
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t i2c_preinit(uint32_t master_id);

/**
 * \brief           Set I2C master initialize
 * \param[in]       master_id: Select the I2C master number
 * \param[in]       i2c_speed: Set the I2C master bus clock frequency
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t i2c_master_init(uint32_t master_id, uint32_t I2C_SPEED);

/**
 * \brief           I2C write data to slave
 * \param[in]       master_id: Specifies the I2C master number
 * \param[in]       slave: Specifies the I2C slave address and register address
 * \param[in]       data: Pointer to buffer with data to write to I2C slave
 * \param[in]       len: Number of data bytes to transmit, maxmum size is 1024 bytes
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t i2c_master_write(uint32_t master_id,
                          const i2c_master_mode_t *slave,
                          uint8_t *data,
                          uint32_t len);

/**
 * \brief           I2C read data from slave
 * \param[in]       master_id: Specifies the I2C master number
 * \param[in]       slave: Specifies the I2C slave address and register address
 * \param[in]       data: Pointer to buffer with data to read to I2C slave
 * \param[in]       len: Number of data bytes to transmit, maxmum size is 1024 bytes
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t i2c_master_read(uint32_t master_id,
                         const i2c_master_mode_t *slave,
                         uint8_t *data,
                         uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* End of I2C_MASTER_H */