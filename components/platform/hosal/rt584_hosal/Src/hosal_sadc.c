/**
 * \file            hosal_sadc.c
 * \brief           Hosal SADC driver file
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
#include "hosal_sadc.h"

static hosal_sadc_cb_fn hosal_sadc_reg_handler = NULL;

void hosal_sadc_register_int_callback(hosal_sadc_cb_fn hosal_sadc_int_callback) {
    hosal_sadc_reg_handler = hosal_sadc_int_callback;
    return;
}

void hosal_sadc_cb_handler(sadc_cb_t* p_cb) {
    hosal_sadc_cb_t hosal_cb;

    hosal_cb.type = (hosal_sadc_cb_type_t)p_cb->type;
    hosal_cb.raw.calibration_value = p_cb->raw.calibration_value;
    hosal_cb.raw.compensation_value = p_cb->raw.compensation_value;
    hosal_cb.raw.conversion_value = p_cb->raw.conversion_value;
    hosal_cb.data.done.p_buffer = p_cb->data.done.p_buffer;
    hosal_cb.data.done.size = p_cb->data.done.size;
    hosal_cb.data.sample.channel = p_cb->data.sample.channel;
    hosal_cb.data.sample.value = p_cb->data.sample.value;

    if (hosal_sadc_reg_handler!= NULL) {
        hosal_sadc_reg_handler(&hosal_cb);
    }

}

void hosal_sadc_config_enable(hosal_sadc_config_t sadc_cfg, void* sadc_usr_callback) {
    uint32_t resolution, oversample;

    resolution = sadc_cfg.resolution;
    oversample = sadc_cfg.oversample;

    if (sadc_usr_callback != NULL) {
        hosal_sadc_register_int_callback(sadc_usr_callback);
    }
    sadc_config_enable(resolution, oversample, hosal_sadc_cb_handler);
}

uint32_t hosal_sadc_channel_read(hosal_sadc_channel_config_t ch) {
    sadc_input_ch_t read_ch;
    uint32_t rval;

    read_ch = (sadc_input_ch_t) ch.channel;

    rval = sadc_channel_read(read_ch);

    return rval;
}

void hosal_sadc_compensation_init(uint32_t xPeriodicTimeInSec) {
    sadc_compensation_init(xPeriodicTimeInSec);
}


