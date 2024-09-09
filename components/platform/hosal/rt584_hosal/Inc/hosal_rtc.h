/**
 * \file            hosal_rtc.h
 * \brief           Hosal RTC driver header file
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

#ifndef HOSAL_RTC_H
#define HOSAL_RTC_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/**
 * \brief           Get current rtc time value
 * \param[out]      tm: save time value you want to get
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_rtc_get_time(void *tm);

/**
 * \brief           Set rtc current time value
 * \param[in]       tm: the time value you want to set
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_rtc_set_time(void *tm);

/**
 * \brief           Get rtc alarm  time value
 * \param[out]      tm: save the time alarm value you want to get
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_rtc_get_alarm(void *tm);

/**
 * \brief           Set rtc alarm time value and alarm mode
 * \param[in]       tm: the time value you want to alarm
 * \param[in]       rtc_int_mode: the rtc alarm mode
 * \param[in]       rtc_usr_isr: when rtc interrupt happen it will call rtc_usr_isr to
 *                               notify the interrupt happen
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_rtc_set_alarm(void *tm, uint32_t rtc_int_mode, void *rtc_usr_isr);

/**
 * \brief           Use to disable rtc alarm
 * \return          Function status, STATUS_SUCCESS
 */
uint32_t hosal_rtc_disable_alarm(void);

/**
 * \brief           Use to set rtc ticks for second
 * \param[in]       clk: Set ticks for one second used for RTC counter
 */
void hosal_rtc_set_clk(uint32_t clk);

/**
 * \brief           Use to reset the RTC to default setting
 */
void hosal_rtc_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_RTC_H */
