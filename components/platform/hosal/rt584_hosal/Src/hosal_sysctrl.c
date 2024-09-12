/**
 * \file            hosal_sysctrl.c
 * \brief           Hosal system control driver file
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
#include "stdio.h"
#include <stdint.h>
#include "mcu.h"
#include "hosal_sysctrl.h"


uint32_t hosal_pin_get_mode(uint32_t pin_number) {
    return pin_get_mode(pin_number);
}

uint32_t hosal_pin_set_mode(uint32_t pin_number, uint32_t mode) {
    if (mode == HOSAL_MODE_GPIO) {
        pin_set_mode(pin_number, MODE_GPIO);
    } else if (mode == HOSAL_MODE_UART0_RX && pin_number == 16) {
        pin_set_mode(pin_number, MODE_UART);
    } else if (mode == HOSAL_MODE_UART0_TX && pin_number == 17) {
        pin_set_mode(pin_number, MODE_UART);
    } else if (mode == HOSAL_MODE_I2CM0_SCL && pin_number == 22) {
        pin_set_mode(pin_number, MODE_I2CM0);
    } else if (mode == HOSAL_MODE_I2CM0_SDA && pin_number == 23) {
        pin_set_mode(pin_number, MODE_I2CM0);
    } else if (mode == HOSAL_MODE_I2S_BCK && pin_number == 0) {
        pin_set_mode(pin_number, MODE_I2S0);
    } else if (mode == HOSAL_MODE_I2S_WCK && pin_number == 1) {
        pin_set_mode(pin_number, MODE_I2S0);
    } else if (mode == HOSAL_MODE_I2S_SDO && pin_number == 2) {
        pin_set_mode(pin_number, MODE_I2S0);
    } else if (mode == HOSAL_MODE_I2S_SDI && pin_number == 3) {
        pin_set_mode(pin_number, MODE_I2S0);
    } else if (mode == HOSAL_MODE_I2S_MCLK && pin_number == 14) {
        pin_set_mode(pin_number, MODE_I2S0);
    } else if (mode == HOSAL_MODE_I2C_SLAVE_SCL && pin_number == 28) {
        pin_set_mode(pin_number, MODE_I2C_SLAVE);
    } else if (mode == HOSAL_MODE_I2C_SLAVE_SDA && pin_number == 29) {
        pin_set_mode(pin_number, MODE_I2C_SLAVE);
    } else {
        return STATUS_INVALID_PARAM;
    }
    return STATUS_SUCCESS;
}

void hosal_pin_set_pullopt(uint32_t pin_number, uint32_t mode) {
    pin_set_pullopt(pin_number, mode);
}

void hosal_enable_pin_opendrain(uint32_t pin_number) {
    enable_pin_opendrain(pin_number);
}

void hosal_disable_pin_opendrain(uint32_t pin_number) {
    disable_pin_opendrain(pin_number);
}

void hosal_pin_enable_schmitt(uint32_t pin_number) {
    pin_enable_schmitt(pin_number);
}

void hosal_pin_disable_schmitt(uint32_t pin_number) {
    pin_disable_schmitt(pin_number);
}

void hosal_pin_enable_filter(uint32_t pin_number) {
    pin_enable_filter(pin_number);
}

void hosal_pin_disable_filter(uint32_t pin_number) {
    pin_disable_filter(pin_number);
}
