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

#ifndef HOSAL_TIMER_H
#define HOSAL_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Hosal Timer counting mode definitions.
 */
#define HOSAL_TIMER_DOWN_COUNTING 0
#define HOSAL_TIMER_UP_COUNTING   1

/**
 * \brief           Hosal Timer one-shot mode definitions.
 */
#define HOSAL_TIMER_ONE_SHOT_DISABLE 0
#define HOSAL_TIMER_ONE_SHOT_ENABLE  1

/**
 * \brief           Hosal Timer mode definitions.
 */
#define HOSAL_TIMER_FREERUN_MODE  0
#define HOSAL_TIMER_PERIODIC_MODE 1

/**
 * \brief           Hosal Timer interrupt definitions.
 */
#define HOSAL_TIMER_INT_DISABLE 0
#define HOSAL_TIMER_INT_ENABLE  1

/**
 * \brief           Hosal Timer prescale definitions.
 */
#define HOSAL_TIMER_PRESCALE_1    0
#define HOSAL_TIMER_PRESCALE_2    3
#define HOSAL_TIMER_PRESCALE_8    4
#define HOSAL_TIMER_PRESCALE_16   1
#define HOSAL_TIMER_PRESCALE_32   5
#define HOSAL_TIMER_PRESCALE_128  6
#define HOSAL_TIMER_PRESCALE_256  2
#define HOSAL_TIMER_PRESCALE_1024 7

/**
 * \brief           Hosal Timer capture edge definitions.
 */
#define HOSAL_TIMER_CAPTURE_POS_EDGE 0
#define HOSAL_TIMER_CAPTURE_NEG_EDGE 1

/**
 * \brief           Hosal Timer capture deglich definitions.
 */
#define HOSAL_TIMER_CAPTURE_DEGLICH_DISABLE 0
#define HOSAL_TIMER_CAPTURE_DEGLICH_ENABLE  1

/**
 * \brief           Hosal Timer capture interrupt definitions.
 */
#define HOSAL_TIMER_CAPTURE_INT_DISABLE 0
#define HOSAL_TIMER_CAPTURE_INT_ENABLE  1

/**
 * \brief           Hosal Timer clock source definitions.
 */
#define HOSAL_TIMER_CLOCK_SOURCE_PERI  0
#define HOSAL_TIMER_CLOCK_SOURCE_RCO1M 2
#define HOSAL_TIMER_CLOCK_SOURCE_PMU   3

/**
 * \brief           Hal timer config structure holding configuration settings 
 *                  for the timer.
 */
typedef struct {
    uint8_t counting_mode  : 1;  /*!< Set counting mode */
    uint8_t oneshot_mode  : 1;  /*!< Enable one shot */
    uint8_t mode           : 1;  /*!< Set Freerun or Periodic mode */
    uint8_t int_en     : 1;  /*!< Enable Interrupt */
    uint8_t prescale       : 3;  /*!< Set prescale */
    uint16_t user_prescale : 10; /*!< Set user define prescale */
} hosal_timer_config_t;

/**
 * \brief           Hal timer config structure holding configuration settings 
 *                  for the timer.
 */
typedef struct {
    uint32_t timeload_ticks; /*!< Timer reload tick */
    uint32_t timeout_ticks;  /*!< Timer timeout tick */
} hosal_timer_tick_config_t;

/**
 * \brief           Hosal timer initialization
 * \param[in]       timer_id: timer id number
 * \param[in]       cfg: timer parameter setting
 * \param[in]       usr_call_back: user callback function
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_INVALID_REQUEST
 */
int hosal_timer_init(uint8_t timer_id, hosal_timer_config_t cfg,
                     void* usr_call_back);

/**
 * \brief           Hosal timer start working
 * \param[in]       timer_id: timer id number
 * \param[in]       tick_cfg: timeout and load value
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_NO_INIT
 */
int hosal_timer_start(uint8_t timer_id, hosal_timer_tick_config_t tick_cfg);

/**
 * \brief           Hosal stop timer
 * \param[in]       timer_id: timer id number
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM, 
 *                  STATUS_NO_INIT
 */
int hosal_timer_stop(uint8_t timer_id);

/**
 * \brief           Hosal timer reload load value 
 * \param[in]       timer_id: timer id number
 * \param[in]       tick_cfg: timeout and load value
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
int hosal_timer_reload(uint8_t timer_id, hosal_timer_tick_config_t tick_cfg);

/**
 * \brief           Hosal close timer 
 * \param[in]       timer_id: timer id number
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
int hosal_timer_finalize(uint8_t timer_id);

/**
 * \brief           Hosal get timer current tick value
 * \param[in]       timer_id: timer id number
 * \return          Timer current tick value
 */
int hosal_timer_current_get(uint8_t timer_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HOSAL_TIMER_H */
