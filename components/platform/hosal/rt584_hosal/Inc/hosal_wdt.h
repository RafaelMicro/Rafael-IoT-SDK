/**
 * \file            hosal_wdt.h
 * \brief           Hosal Watch Dog timer header file
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

#ifndef HOSAL_WDT_H
#define HOSAL_WDT_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "mcu.h"
#include "wdt.h"

/**
 * \brief           Gosal WDT prescale definitions.
 */
#define HOSAL_WDT_PRESCALE_1               0
#define HOSAL_WDT_PRESCALE_16              15
#define HOSAL_WDT_PRESCALE_32              31
#define HOSAL_WDT_PRESCALE_128             127
#define HOSAL_WDT_PRESCALE_256             255
#define HOSAL_WDT_PRESCALE_1024            1023
#define HOSAL_WDT_PRESCALE_4096            4095


/**
 * \brief           Hal wdt config structure.
 */
typedef struct {
    uint8_t  int_enable   : 1;                  /*!< config of interrupt enable */
    uint8_t  reset_enable : 1;                  /*!< config of reset enable */
    uint8_t  lock_enable  : 1;                  /*!< config of lockout enable */
    uint16_t prescale     : 12;                 /*!< config of prescale */
} hosal_wdt_config_mode_t;

/**
 * \brief           Hal wdt ticks config structure.
 */
typedef struct {
    uint32_t wdt_ticks;                         /*!< config of load value */
    uint32_t int_ticks;                         /*!< config of interrupt value */
    uint32_t wdt_min_ticks;                     /*!< config of wdt min value */
} hosal_wdt_config_tick_t;


/**
 * \brief           Hosal wdt initialization
 * \param[in]       wdt_mode: wdt parameter setting
 * \param[in]       wdt_cfg_ticks: timeout value
 * \param[in]       wdt_cb: timeout value
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_INVALID_REQUEST
 */
uint32_t hosal_wdt_start(hosal_wdt_config_mode_t wdt_mode, 
                         hosal_wdt_config_tick_t wdt_cfg_ticks, 
                         void* usr_call_back);

/**
 * \brief           Hosal WDT stop
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_REQUEST
 */
uint32_t hosal_wdt_stop(void);

/**
 * \brief           Get WDT number of resets
 * \return          WDT number of resets
 */
uint32_t hosal_wdt_reset_event_get(void);

/**
 * \brief           Clear WDT number of resets
 */
void hosal_wdt_reset_event_clear(void);

/**
 * \brief           Reload the watchdog value
 */
void hosal_wdt_kick(void);

/**
 * \brief           Clear Watchdog interrupt flag
 */
void hosal_wdt_interrupt_clear(void);

/**
 * \brief           Get watchdog timer current tick value
 * \return          current tick value
 */
uint32_t hosal_wdt_current_get(void);

#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_WDT_H */
