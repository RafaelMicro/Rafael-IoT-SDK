/******************************************************************************
 * \file     hosal_slow_timer.c
 * \version
 * \brief    Hosal Slow Timer driver
 *
 * @copyright
 ******************************************************************************/
/**
 * \file            hosal_slow_timer.h
 * \brief           Hosal Slow Timer driver header file
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
#include "hosal_slow_timer.h"


int hosal_slow_timer_init(uint8_t timer_id, hosal_slow_timer_config_t cfg, 
                          void* usr_call_back) {
    timer_config_mode_t drv_cfg;

    uint32_t rval;

    drv_cfg.int_en = cfg.int_enable;
    drv_cfg.mode = cfg.mode;
    drv_cfg.prescale = cfg.prescale;
    drv_cfg.repeat_delay = cfg.repeat_delay;

    rval = timer_open(timer_id, drv_cfg, usr_call_back);

    return rval;
}

int hosal_slow_timer_start(uint8_t timer_id, 
                           hosal_slow_timer_tick_config_t tick_cfg) {
    uint32_t rval;

    rval = timer_start(timer_id, tick_cfg.timeload_ticks);

    return rval;
}

int hosal_slow_timer_stop(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_stop(timer_id);

    return rval;
}

int hosal_slow_timer_reload(uint8_t timer_id, 
                            hosal_slow_timer_tick_config_t tick_cfg) {
    uint32_t rval;

    rval = timer_load(timer_id, tick_cfg.timeload_ticks);

    return rval;
}

int hosal_slow_timer_finalize(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_close(timer_id);

    return rval;
}

int hosal_slow_timer_current_get(uint8_t timer_id) {
    uint32_t value;

    value = timer_current_get(timer_id);

    return value;
}

