/**
 * \file            i2c_slave_reg.h
 * \brief           i2c slave register header file
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

#ifndef I2C_SLAVE_REG_H
#define I2C_SLAVE_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           i2c slave total register 
 */
typedef struct {
    __IO uint32_t i2c_slave_data;           /*!< 0x00 i2c slave T/Rx data */
    __IO uint32_t i2c_slave_addr;           /*!< 0x04 i2c slave address */
    __IO uint32_t i2c_slave_int_enable;     /*!< 0x08 i2c slave interrupt enable 
                                                register */
    __IO uint32_t i2c_slave_int_status;     /*!< 0x0C i2c slave interrupt status 
                                                register */
    __IO uint32_t i2c_slave_timeout;        /*!< 0x10 i2c slave timeout register */
    __IO uint32_t i2c_slave_enable;         /*!< 0x14 i2c slave enable register */
    __I  uint32_t i2c_slave_status;         /*!< 0x18 i2c slave status register */
} i2c_slave_t;


#ifdef __cplusplus
}
#endif

#endif /* End of I2C_SLAVE_REG_H */
