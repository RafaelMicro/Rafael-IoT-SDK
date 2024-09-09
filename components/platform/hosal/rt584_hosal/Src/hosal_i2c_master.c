/**
 * \file            hosal_i2c_master.c
 * \brief           Hosal i2c master driver file
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
#include "hosal_i2c_master.h"

uint32_t hosal_i2c_preinit(uint32_t scl_pin, uint32_t sda_pin) {
    uint32_t rval;

    rval = i2c_preinit(scl_pin, sda_pin);

    return rval;
}

uint32_t hosal_i2c_init(uint32_t i2c_master_port, uint32_t i2c_speed) {
    uint32_t rval;

    rval = i2c_init(i2c_speed);

    return rval;
}

uint32_t hosal_i2c_write(uint32_t i2c_master_port, void* slave, uint8_t* data,
                         uint32_t len) {
    hosal_i2c_slave_data_t* hosal_cfg;
    i2c_slave_data_t drv_cfg;
    
    uint32_t rval;

    hosal_cfg = (hosal_i2c_slave_data_t*)slave;

    drv_cfg.bFlag_16bits = hosal_cfg->bFlag_16bits;
    drv_cfg.dev_addr = hosal_cfg->dev_addr;
    drv_cfg.reg_addr = hosal_cfg->reg_addr;

    rval = i2c_write(&drv_cfg, data, len, hosal_cfg->i2c_usr_isr);

    return rval;
}

uint32_t hosal_i2c_read(uint32_t i2c_master_port, hosal_i2c_slave_data_t* slave,
                        uint8_t* data, uint32_t len) {
    i2cm_cb_fn usr_cb;
    i2c_slave_data_t drv_cfg;

    uint32_t rval;

    drv_cfg.bFlag_16bits = slave->bFlag_16bits;
    drv_cfg.dev_addr = slave->dev_addr;
    drv_cfg.reg_addr = slave->reg_addr;

    rval = i2c_read(&drv_cfg, data, len, slave->i2c_usr_isr);

    return rval;
}
