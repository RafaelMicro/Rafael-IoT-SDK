/**
 * \file            hosal_i2c_master.h
 * \brief           Hosal I2C master driver header file
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

#ifndef HOSAL_I2C_MASTER_H
#define HOSAL_I2C_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "i2c_master.h"

/**
 * \brief           I2C CLOCK Constant Definitions
 */
#define HOSAL_I2C_CLOCK_800K 3
#define HOSAL_I2C_CLOCK_400K 0
#define HOSAL_I2C_CLOCK_200K 1
#define HOSAL_I2C_CLOCK_100K 2

/**
 * \brief           Hosal structure for the I2C transfer request.
 */
typedef struct {
    uint8_t    dev_addr;                        /*!< I2C device address, 7bits only */
    uint8_t    bFlag_16bits;                    /*!< 1 for register address is 16bits, 
                                                     0 for register address is 8bits. */
    uint16_t   reg_addr;                        /*!< I2C register address value */
    i2cm_cb_fn i2c_usr_isr;                     /*!< I2C usr callback */
} hosal_i2c_master_mode_t;

/**
 * \brief           hosal I2C master pre-initial function
 * \param[in]       master_id: Specifies the i2c id number
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_i2c_preinit(uint32_t master_id);

/**
 * \brief           Set hosal I2C master initialize
 * \param[in]       master_id: Set the I2C master port number
 * \param[in]       i2c_speed: Set the I2C master bus clock frequency
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t hosal_i2c_init(uint32_t master_id, uint32_t i2c_speed);

/**
 * \brief           I2C write data to slave
 * \param[in]       master_id: Set the I2C master port number
 * \param[in]       slave: Specifies the I2C slave address and register address
 * \param[in]       data: Pointer to buffer with data to write to I2C slave
 * \param[in]       len:Number of data bytes to transmit, maxmum size is 1024 bytes
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_i2c_write(uint32_t master_id, hosal_i2c_master_mode_t* slave,
                         uint8_t* data, uint32_t len);

/**
 * \brief           I2C read data to slave
 * \param[in]       master_id: Set the I2C master port number
 * \param[in]       slave: Specifies the I2C slave address and register address
 * \param[in]       data: Pointer to buffer with data to receive from I2C slave
 * \param[in]       len: Number of data bytes to receive, maxmum size is 1024 bytes
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_NO_INIT, STATUS_EBUSY
 */
uint32_t hosal_i2c_read(uint32_t master_id, hosal_i2c_master_mode_t* slave,
                        uint8_t* data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_I2C_MASTER_H */