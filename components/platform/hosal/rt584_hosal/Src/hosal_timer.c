/**
 * \file            hosal_timer.h
 * \brief           Hosal Timer driver header file
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
#include "hosal_timer.h"
#include "mcu.h"
#include "stdio.h"

uint32_t hosal_timer_init(uint8_t timer_id, hosal_timer_config_t cfg,
                          void* usr_call_back) {
    timer_config_mode_t drv_cfg;

    uint32_t rval;

    drv_cfg.int_en = cfg.int_en;
    drv_cfg.mode = cfg.mode;
    drv_cfg.prescale = cfg.prescale;

    drv_cfg.oneshot_mode = cfg.oneshot_mode;
    drv_cfg.counting_mode = cfg.counting_mode;
    drv_cfg.user_prescale = cfg.user_prescale;

    rval = timer_open(timer_id, drv_cfg, usr_call_back);

    return rval;
}

uint32_t hosal_timer_start(uint8_t timer_id,
                           hosal_timer_tick_config_t tick_cfg) {
    uint32_t rval;

    rval = timer_start(timer_id, tick_cfg.timeload_ticks, tick_cfg.timeout_ticks);

    return rval;
}

uint32_t hosal_timer_stop(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_stop(timer_id);

    return rval;
}

uint32_t hosal_timer_reload(uint8_t timer_id,
                            hosal_timer_tick_config_t tick_cfg) {
    uint32_t rval;

    rval = timer_load(timer_id, tick_cfg.timeload_ticks, tick_cfg.timeout_ticks);

    return rval;
}

uint32_t hosal_timer_finalize(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_close(timer_id);

    return rval;
}

uint32_t hosal_timer_current_get(uint8_t timer_id) {
    uint32_t value;

    value = timer_current_get(timer_id);

    return value;
}

uint32_t hosal_timer_capture_init(uint8_t timer_id,
                                  hosal_timer_capture_config_mode_t cfg,
                                  void* usr_call_back) {
    timer_capture_config_mode_t drv_cfg;

    uint32_t rval;

    drv_cfg.int_en = cfg.int_en;
    drv_cfg.mode = cfg.mode;
    drv_cfg.prescale = cfg.prescale;

    drv_cfg.oneshot_mode = cfg.oneshot_mode;
    drv_cfg.counting_mode = cfg.counting_mode;
    drv_cfg.user_prescale = cfg.user_prescale;

    drv_cfg.ch0_capture_edge = cfg.ch0_capture_edge;
    drv_cfg.ch0_deglich_enable = cfg.ch0_deglich_enable;
    drv_cfg.ch0_int_enable = cfg.ch0_int_enable;
    drv_cfg.ch0_iosel = cfg.ch0_iosel;
    drv_cfg.ch1_capture_edge = cfg.ch1_capture_edge;
    drv_cfg.ch1_deglich_enable = cfg.ch1_deglich_enable;
    drv_cfg.ch1_int_enable = cfg.ch1_int_enable;
    drv_cfg.ch1_iosel = cfg.ch1_iosel;

    rval = timer_capture_open(timer_id, drv_cfg, usr_call_back);

    return rval;
}

uint32_t hosal_timer_capture_start(uint8_t timer_id, uint32_t timeload_ticks,
                                   uint32_t timeout_ticks, bool chanel0_enable,
                                   bool chanel1_enable) {
    uint32_t rval;

    rval = timer_capture_start(timer_id, timeload_ticks, timeout_ticks,
                               chanel0_enable, chanel1_enable);

    return rval;
}

uint32_t hosal_timer_capture_stop(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_capture_stop(timer_id);

    return rval;
}

uint32_t hosal_timer_capture_finalize(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_capture_close(timer_id);

    return rval;
}

uint32_t hosal_timer_ch0_capture_value_get(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_ch0_capture_value_get(timer_id);

    return rval;
}

uint32_t hosal_timer_ch0_capture_int_status(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_ch0_capture_int_status(timer_id);

    return rval;
}

uint32_t hosal_timer_ch1_capture_value_get(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_ch1_capture_value_get(timer_id);

    return rval;
}

uint32_t hosal_timer_ch1_capture_int_status(uint8_t timer_id) {
    uint32_t rval;

    rval = timer_ch1_capture_int_status(timer_id);

    return rval;
}
