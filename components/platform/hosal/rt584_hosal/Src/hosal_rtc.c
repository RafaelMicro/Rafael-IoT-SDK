/**
 * \file            hosal_rtc.c
 * \brief           Hosal RTC driver file
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
#include "hosal_rtc.h"


uint32_t hosal_rtc_get_time(void *tm) {
    uint32_t rval;

    rval = rtc_get_time(tm);

    return rval;
}

uint32_t hosal_rtc_set_time(void *tm) {
    uint32_t rval;

    rval = rtc_set_time(tm);

    return rval;
}

uint32_t hosal_rtc_get_alarm(void *tm) {
    uint32_t rval;

    rval = rtc_get_alarm(tm);

    return rval;
}

uint32_t hosal_rtc_set_alarm(void *tm, uint32_t rtc_int_mode,
                             void *rtc_usr_isr) {
    uint32_t rval;

    rval = rtc_set_alarm(tm, rtc_int_mode, rtc_usr_isr);

    return rval;
}

uint32_t hosal_rtc_disable_alarm(void) {
    uint32_t rval;

    rval = rtc_disable_alarm();

    return rval;
}

void hosal_rtc_set_clk(uint32_t clk) {
    rtc_set_clk(clk);
}

void hosal_rtc_reset(void) {
    rtc_reset();
}
