/**
 * \file            i2c_master_reg.h
 * \brief           i2c master register header file
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

#ifndef I2C_MASTER_REG_H
#define I2C_MASTER_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           i2c master total register 
 */
typedef struct {
    __IO  uint32_t control;                     /*!< 0x00 i2c control register */
    __IO  uint32_t tar;                         /*!< 0x04 i2c target address */
    __IO  uint32_t buf;                         /*!< 0x08 i2c command register */
    __I   uint32_t int_status;                  /*!< 0x0C i2c interrupt status 
                                                    register*/
    __IO uint32_t  int_enable;                  /*!< 0x10 i2c interrupt enable 
                                                    register */
    __I  uint32_t  int_raw_status;              /*!< 0x14 i2c interrupt raw status 
                                                    register*/
    __IO uint32_t  int_clear;                   /*!< 0x18 i2c interrupt clear 
                                                    register */
    __IO  uint32_t slck_gen;                    /*!< 0x1C i2c source clock divider */
} i2c_master_t;

#ifdef __cplusplus
}
#endif

#endif /* End of I2C_MASTER_REG_H */
