/**
 * \file            hosal_i2c_slave.c
 * \brief           Hosal i2c slave driver file
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

#include "hosal_i2c_slave.h"

uint32_t hosal_i2c_slave_open(hosal_i2c_slave_mode_t* i2c_slave_client) {
    i2c_slave_mode_t cfg = {
        .i2c_slave_cb_func = i2c_slave_client->i2c_slave_cb_func,
        .i2c_bus_timeout_enable = i2c_slave_client->i2c_bus_timeout_enable,
        .i2c_bus_timeout = i2c_slave_client->i2c_bus_timeout,
        .i2c_slave_addr = i2c_slave_client->i2c_slave_addr,
    };

    return i2c_slave_open(&cfg);
}

uint32_t hosal_i2c_slave_close(void) { 
    return i2c_slave_close ();
}
