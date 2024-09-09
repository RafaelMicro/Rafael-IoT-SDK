/**
 * \file            hosal_wdt.c
 * \brief           Hosal WDT driver file
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

#include "hosal_wdt.h"
#include "mcu.h"


uint32_t hosal_wdt_start(hosal_wdt_config_mode_t hal_wdt_cfg,
                         hosal_wdt_config_tick_t hal_wdt_tick_cfg,
                         void* usr_call_back) {
    /* mode setting */
    wdt_config_mode_t wdt_cfg = {
        .int_enable = hal_wdt_cfg.int_enable,
        .reset_enable = hal_wdt_cfg.reset_enable,
        .lock_enable = hal_wdt_cfg.lock_enable,
        .prescale = hal_wdt_cfg.prescale,
    };

    /* tick setting */
    wdt_config_tick_t wdt_tick_cfg = {
        .wdt_ticks = hal_wdt_tick_cfg.wdt_ticks,
        .int_ticks = hal_wdt_tick_cfg.int_ticks,
        .wdt_min_ticks = hal_wdt_tick_cfg.wdt_min_ticks,
    };
    return wdt_start(wdt_cfg, wdt_tick_cfg, usr_call_back);
}

uint32_t hosal_wdt_stop(void) { return wdt_stop(); }

uint32_t hosal_wdt_reset_event_get(void) { return wdt_reset_event_get(); }

void hosal_wdt_reset_event_clear(void) { wdt_reset_event_clear(); }

void hosal_wdt_kick(void) { wdt_kick(); }

void hosal_wdt_interrupt_clear(void) { wdt_interrupt_clear(); }

uint32_t hosal_wdt_current_get(void) { return wdt_current_get(); }
