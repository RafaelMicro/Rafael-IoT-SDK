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

#include <stdint.h>
#include "hosal_slow_timer.h"
#include "mcu.h"
#include "stdio.h"

int hosal_slow_timer_init(uint8_t timer_id, hosal_slow_timer_config_t cfg,
                          void* usr_call_back) {
    timer_config_mode_t drv_cfg = {.int_en = cfg.int_enable,
                                   .mode = cfg.mode,
                                   .prescale = cfg.prescale,
                                   .repeat_delay = cfg.repeat_delay};
    return timer_open(timer_id, drv_cfg, usr_call_back);
}

int hosal_slow_timer_start(uint8_t timer_id,
                           hosal_slow_timer_tick_config_t tick_cfg) {
    return timer_start(timer_id, tick_cfg.timeload_ticks);
}

int hosal_slow_timer_stop(uint8_t timer_id) { return timer_stop(timer_id); }

int hosal_slow_timer_reload(uint8_t timer_id,
                            hosal_slow_timer_tick_config_t tick_cfg) {
    return timer_load(timer_id, tick_cfg.timeload_ticks);
}

int hosal_slow_timer_finalize(uint8_t timer_id) {
    return timer_close(timer_id);
}

int hosal_slow_timer_current_get(uint8_t timer_id) {
    return timer_current_get(timer_id);
}
