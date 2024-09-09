/**
 * \file            dpd.h
 * \brief           Deep power down header file
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

#ifndef DPD_H
#define DPD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mcu.h"


/**
 * \brief           Get reset all cause.
 * \return          get cause register value
 */
__STATIC_INLINE uint32_t get_all_reset_cause(void)
{
    return (DPD_CTRL->dpd_rst_cause.reg);
}

/**
 * \brief           Reset by power on or not.
 * \return          0: reset not by power on, 
 *                  1: reset by power on
 */
__STATIC_INLINE uint32_t reset_by_power_on(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_por);
}

/**
 * \brief           Reset by external reset or not.
 * \return          0: reset not by external reset, 
 *                  1: reset by external reset
 */
__STATIC_INLINE uint32_t reset_by_external(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_ext);
}

/**
 * \brief           Reset by deep power down or not.
 * \return          0: reset not by deep power down, 
 *                  1: reset by deep power down
 */
__STATIC_INLINE uint32_t reset_by_deep_power_down(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_dpd);
}

/**
 * \brief           Reset by deep sleep or not.
 * \return          0: reset not by deep sleep, 
 *                  1: reset by deep sleep
 */
__STATIC_INLINE uint32_t reset_by_deep_sleep(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_ds);
}

/**
 * \brief           Reset by WDT or not.
 * \return          0: reset not by WDT, 
 *                  1: reset by WDT
 */
__STATIC_INLINE uint32_t reset_by_wdt(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_wdt);
}

/**
 * \brief           Reset by software or not.
 * \return          0: reset not by software, 
 *                  1: reset by software
 */
__STATIC_INLINE uint32_t reset_by_software(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_soft);
}

/**
 * \brief           Reset by mcu lockup or not.
 * \return          0: reset not by mcu lockup, 
 *                  1: reset by mcu lockup
 */
__STATIC_INLINE uint32_t reset_by_lock(void) {
    return (DPD_CTRL->dpd_rst_cause.bit.rst_cause_lock);
}

/**
 * \brief           Clear reset cause.
 */
__STATIC_INLINE void clear_reset_cause(void) {
    DPD_CTRL->dpd_cmd.bit.clr_rst_cause = 1;
}

#ifdef __cplusplus
}
#endif

#endif /* End of DPD_H */
