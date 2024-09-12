/**
 * \file            hosal_i2c_slave.h
 * \brief           Hosal I2C slave driver header file
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

#ifndef HOSAL_I2C_SLAVE_H
#define HOSAL_I2C_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "i2c_slave.h"

/**
 * \brief           I2C Slave callback status definitions
 */
#define HOSAL_I2C_SLAVE_STATUS_ADDR_MATCH (1 << 0)
#define HOSAL_I2C_SLAVE_STATUS_DATA_READY (1 << 1)
#define HOSAL_I2C_SLAVE_STATUS_STOP       (1 << 2)
#define HOSAL_I2C_SLAVE_STATUS_TIMEOUT    (1 << 3)
#define HOSAL_I2C_SLAVE_STATUS_READ       (1 << 4)
#define HOSAL_I2C_SLAVE_STATUS_WRITE      (1 << 5)
#define HOSAL_I2C_SLAVE_STATUS_ERROR      (1 << 31)

/**
 * \brief           Structure for the I2C slave configuration.
 */
typedef struct {
    i2c_slave_cb_fn i2c_slave_cb_func; /*!< i2c slave callback function */
    uint8_t i2c_bus_timeout_enable;    /*!< i2c bus timeout enable */
    uint8_t i2c_bus_timeout;           /*!< i2c bus timeout value */
    uint8_t i2c_slave_addr;            /*!< i2c slave 7 bits only */
} hosal_i2c_slave_mode_t;

/**
 * \brief           Set I2C slave initialize
 * \param[in]       i2c_slave_client: i2c slave configuration
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t hosal_i2c_slave_open(hosal_i2c_slave_mode_t* i2c_slave_client);

/**
 * \brief           Close I2C slave
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t hosal_i2c_slave_close(void);

/**
 * \brief           Get one byte from i2c master
 * \return          i2c master to slave byte
 */
__STATIC_INLINE uint8_t hosal_i2c_slave_read_byte(void) {
    return i2c_slave_read_byte();
}

/**
 * \brief           Send one byte from i2c master
 */
__STATIC_INLINE void hosal_i2c_slave_write_byte(uint8_t data) {
    i2c_slave_write_byte(data);
}

#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_I2C_SLAVE_H */