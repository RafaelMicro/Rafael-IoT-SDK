/**
 * \file            i2c_slave.h
 * \brief           i2c slave driver header file
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

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mcu.h"


/**
 * \brief           I2C Slave callback status definitions
 */
#define I2C_SLAVE_STATUS_ADDR_MATCH          (1 << 0)
#define I2C_SLAVE_STATUS_DATA_READY          (1 << 1)
#define I2C_SLAVE_STATUS_STOP                (1 << 2)
#define I2C_SLAVE_STATUS_TIMEOUT             (1 << 3)
#define I2C_SLAVE_STATUS_READ                (1 << 4)
#define I2C_SLAVE_STATUS_WRITE               (1 << 5)
#define I2C_SLAVE_STATUS_ERROR               (1 << 31)

/**
 * \brief           I2C Slave interrupt register definitions
 */
#define  I2C_SLAVE_MATCH_ADDR             (0x01)
#define  I2C_SLAVE_DATA_READY             (0x02)
#define  I2C_SLAVE_BUS_STOP               (0x04)
#define  I2C_SLAVE_BUS_TIMEOUT            (0x08)
#define  I2C_SLAVE_ALL_INT          (I2C_SLAVE_MATCH_ADDR | I2C_SLAVE_DATA_READY | I2C_SLAVE_BUS_STOP | I2C_SLAVE_BUS_TIMEOUT)

/**
 * \brief           I2C Slave status register definitions
 */
#define  I2C_SLAVE_READ_OP                (1<<1)


/**
 * \brief           I2C slave finish routine notify callback for user application
 * \param[in]       status: I2C slave transfer status.
 */
typedef void (*i2c_slave_cb_fn)(uint32_t status);

/**
 * \brief           Structure for the I2C slave configuration.
 */
typedef struct
{
    i2c_slave_cb_fn i2c_slave_cb_func;          /*!< i2c slave callback function */
    uint8_t  i2c_bus_timeout_enable;            /*!< i2c bus timeout enable */
    uint8_t  i2c_bus_timeout;                   /*!< i2c bus timeout value */
    uint8_t  i2c_slave_addr;                    /*!< i2c slave 7 bits only */
} i2c_slave_mode_t;

/**
 * \brief           Set I2C slave initialize
 * \param[in]       i2c_slave_client: i2c slave configuration
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t i2c_slave_open(i2c_slave_mode_t *i2c_slave_client);

/**
 * \brief           Close I2C slave
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t i2c_slave_close(void);

/**
 * \brief           Get one byte from i2c master
 * \return          i2c master to slave byte
 */
__STATIC_INLINE uint8_t i2c_slave_read_byte(void) {
    return (I2C_SLAVE->i2c_slave_data & 0xFF);
}

/**
 * \brief           Send one byte from i2c master
 */
__STATIC_INLINE void i2c_slave_write_byte(uint8_t data) {
    I2C_SLAVE->i2c_slave_data = data;
}

#ifdef __cplusplus
}
#endif

#endif /* End of I2C_SLAVE_H */
