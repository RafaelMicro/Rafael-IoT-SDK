/**
 * \file            hosal_swi.c
 * \brief           hosal software interrupt driver file
 */

/*
 * Copyright (c) 2024 Rafael Micro
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
 * This file is part of library_name.
 *
 * Author:
 */
#include <stdint.h>
#include "hosal_swi.h"
#include "mcu.h"
#include "stdio.h"
#include "swi.h"

int hosal_swi_uninit(void) {
    uint32_t i;
    for (i = 0; i < HOSAL_MAX_NUMBER_OF_SWI; i++)
        swi_int_disable((swi_id_sel_t)i);
    return STATUS_SUCCESS;
}

int hosal_swi_init(void) {
    swi_int_callback_clear();
    return STATUS_SUCCESS;
}

int hosal_swi_trigger(uint8_t swi_id) {
    if (swi_id >= HOSAL_MAX_NUMBER_OF_SWI)
        return STATUS_INVALID_PARAM;
    swi_int_trigger(swi_id);
    return STATUS_SUCCESS;
}

int hosal_swi_callback_register(uint8_t swi_id,
                                hosal_swi_callback_fn pfn_callback) {
    if (swi_id >= HOSAL_MAX_NUMBER_OF_SWI)
        return STATUS_INVALID_PARAM;
    swi_int_enable(swi_id, (swi_isr_handler_t)pfn_callback);
    return STATUS_SUCCESS;
}

int hosal_swi_callback_unregister(uint8_t swi_id) {
    if (swi_id >= HOSAL_MAX_NUMBER_OF_SWI)
        return STATUS_INVALID_PARAM;
    swi_int_disable(swi_id);
    return STATUS_SUCCESS;
}
