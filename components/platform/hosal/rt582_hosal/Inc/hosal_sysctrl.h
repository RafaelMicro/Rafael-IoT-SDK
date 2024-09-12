/**
 * \file            hosal_sysctrl.h
 * \brief           Hosal system control driver header file
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
 * Author:          
 */

#ifndef HOSAL_SYSCTRL_H
#define HOSAL_SYSCTRL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Output pin mode 
 */
#define HOSAL_MODE_GPIO        0x00
#define HOSAL_MODE_UART0_TX    0x01
#define HOSAL_MODE_UART0_RX    0x02
#define HOSAL_MODE_UART1_TX    0x03
#define HOSAL_MODE_UART1_RX    0x04
#define HOSAL_MODE_UART1_RTSN  0x03
#define HOSAL_MODE_UART1_CTSN  0x04
#define HOSAL_MODE_UART2_TX    0x05
#define HOSAL_MODE_UART2_RX    0x06
#define HOSAL_MODE_UART2_RTSN  0x07
#define HOSAL_MODE_UART2_CTSN  0x08
#define HOSAL_MODE_PWM0        0x09
#define HOSAL_MODE_PWM1        0x0A
#define HOSAL_MODE_PWM2        0x0B
#define HOSAL_MODE_PWM3        0x0C
#define HOSAL_MODE_PWM4        0x0D
#define HOSAL_MODE_I2CM0_SCL   0x0E
#define HOSAL_MODE_I2CM0_SDA   0x0F
#define HOSAL_MODE_SPI0_SCLK   0x11
#define HOSAL_MODE_SPI0_SDATA0 0x12
#define HOSAL_MODE_SPI0_SDATA1 0x13
#define HOSAL_MODE_SPI0_SDATA2 0x14
#define HOSAL_MODE_SPI0_SDATA3 0x15
#define HOSAL_MODE_SPI0_CSN0   0x16
#define HOSAL_MODE_SPI0_CSN1   0x17
#define HOSAL_MODE_SPI0_CSN2   0x18
#define HOSAL_MODE_SPI0_CSN3   0x19
#define HOSAL_MODE_SPI1_SCLK   0x1A
#define HOSAL_MODE_SPI1_SDATA0 0x1B
#define HOSAL_MODE_SPI1_SDATA1 0x1C
#define HOSAL_MODE_SPI1_SDATA2 0x1D
#define HOSAL_MODE_SPI1_SDATA3 0x1E
#define HOSAL_MODE_SPI1_CSN0   0x1F
#define HOSAL_MODE_SPI1_CSN1   0x20
#define HOSAL_MODE_SPI1_CSN2   0x21
#define HOSAL_MODE_SPI1_CSN3   0x22
#define HOSAL_MODE_I2S_BCK     0x23
#define HOSAL_MODE_I2S_WCK     0x24
#define HOSAL_MODE_I2S_SDO     0x25
#define HOSAL_MODE_I2S_SDI     0x26
#define HOSAL_MODE_I2S_MCLK    0x27

/**
 * \brief           Gpio pull up define
 */
#define HOSAL_PULL_NONE      0
#define HOSAL_PULL_DOWN_10K  1
#define HOSAL_PULL_DOWN_100K 2
#define HOSAL_PULL_DOWN_1M   3
#define HOSAL_PULL_UP_10K    5
#define HOSAL_PULL_UP_100K   6
#define HOSAL_PULL_UP_1M     7

/**
 * \brief           Get pin function mode
 * \param[in]       pin_number: Specifies the pin number
 * \return          The pin function mode
 */
uint32_t hosal_pin_get_mode(uint32_t pin_number);

/**
 * \brief           Set pin function mode
 * \param[in]       pin_number: Specifies the pin number
 * \param[in]       mode: The specail function mode for the pin_number
 *                        Config GPIO To --> UART/I2S/PWM/SADC/I2C/SPI
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_pin_set_mode(uint32_t pin_number, uint32_t mode);

/**
 * \brief           Set pin pull option
 * \param[in]       pin_number: Specifies the pin number
 * \param[in]       mode: The specail pull option for the pin_number
 *                        PULL_NONE        0
 *                        PULL_DOWN_10K    1
 *                        PULL_DOWN_100K   2
 *                        PULL_DOWN_1M     3
 *                        PULL_UP_10K      5
 *                        PULL_UP_100K     6
 *                        PULL_UP_1M       7
 */
void hosal_pin_set_pullopt(uint32_t pin_number, uint32_t mode);

/**
 * \brief           Set pin to opendrain option
 * \param[in]       pin_number: Specifies the pin number
 */
void hosal_enable_pin_opendrain(uint32_t pin_number);

/**
 * \brief           Disable pin to opendrain option
 * \param[in]       pin_number: Specifies the pin number
 */
void hosal_disable_pin_opendrain(uint32_t pin_number);

#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_SYSCTRL_H */
