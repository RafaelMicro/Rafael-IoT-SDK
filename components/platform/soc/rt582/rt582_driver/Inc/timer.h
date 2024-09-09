/**
 * \file            timer.h
 * \brief           Timer driver header file
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
#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define TIMER_PERIODIC_MODE            1
#define TIMER_FREERUN_MODE             0

#define TIMER_PRESCALE_1               0
#define TIMER_PRESCALE_2               3
#define TIMER_PRESCALE_8               4
#define TIMER_PRESCALE_16              1
#define TIMER_PRESCALE_32              5
#define TIMER_PRESCALE_128             6
#define TIMER_PRESCALE_256             2
#define TIMER_PRESCALE_1024            7


/**
 * \brief           TIMER interrupt service routine callback for user application.
 * \param[in]       timer_id: timer id number
 */
typedef void (*timer_cb_fn)(uint32_t timer_id);

/**
 * \brief           Timer config structure holding configuration settings for the 
 *                  timer and slow timer.
 */
typedef struct {
    uint8_t  mode     : 1;                      /*!< Timer operating mode setting. */
    uint8_t  prescale : 3;                      /*!< Timer prescale setting. */
    uint8_t  int_en   : 1;                      /*!< Timer interrupt enable setting. */
    uint16_t repeat_delay;                      /*!< Slow timer repeat delay times */
} timer_config_mode_t;

/**
 * \brief           Register timer callback function
 * \param[in]       timer_id: timer id number 
 * \param[in]       timer_cb: user callback function
 */
void timer_callback_register(uint32_t timer_id, timer_cb_fn timer_cb);

/**
 * \brief           Timer parameter setting 
 * \param[in]       timer_id: timer id number
 * \param[in]       mode: timer configuration
 * \param[in]       timer_cb: timer callback function
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_INVALID_REQUEST
 */
uint32_t timer_open(uint32_t timer_id, timer_config_mode_t mode, timer_cb_fn timer_cb);

/**
 * \brief           Timer load timeout value 
 * \param[in]       timer_id: timer id number
 * \param[in]       timeout_ticks: timeout value
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t timer_load(uint32_t timer_id, uint32_t timeout_ticks);

/**
 * \brief           Timer start working with tiomeout value
 * \param[in]       timer_id: timer id number
 * \param[in]       timeout_ticks: timeout value
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, S
 *                  TATUS_NO_INIT
 */
uint32_t timer_start(uint32_t timer_id, uint32_t timeout_ticks);

/**
 * \brief           Stop timer
 * \param[in]       timer_id: timer id number
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_NO_INIT
 */
uint32_t timer_stop(uint32_t timer_id);

/**
 * \brief           Close timer 
 * \param[in]       timer_id: timer id number
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t timer_close(uint32_t timer_id);

/**
 * \brief           Get timer current tick value
 * \param[in]       timer_id: timer id number
 * \return          Timer current tick value
 */
uint32_t timer_current_get(uint32_t timer_id);

#ifdef __cplusplus
}
#endif

#endif /* End of TIMER_H */