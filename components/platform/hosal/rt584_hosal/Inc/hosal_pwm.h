/**
 * \file            hosal_pwm.h
 * \brief           hosal_pwm include file
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
 * Author:         ives.lee
 */
#ifndef HOSAL_PWM_H
#define HOSAL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                  Constant Definition
//=============================================================================
/**
 * \brief           software interrupt typedef callback function
 */
#define HOSAL_PWM_SET_COUNT_MODE            1
#define HOSAL_PWM_SET_FRQUENCY              2
#define HOSAL_PWM_SET_DELAY_NUMBER          3
#define HOSAL_PWM_SET_REPEAT_NUMBER         4
#define HOSAL_PWM_SET_PLAY_NUMBER           5
#define HOSAL_PWM_SET_TSEQ_ADDRESS          6
#define HOSAL_PWM_SET_RSEQ_ADDRESS          7
#define HOSAL_PWM_GET_COUNT_MODE            8
#define HOSAL_PWM_GET_FRQUENCY              9
#define HOSAL_PWM_GET_DUTY                  10
#define HOSAL_PWM_GET_COUNT                 11
#define HOSAL_PWM_GET_DELAY_NUMBER          12
#define HOSAL_PWM_GET_REPEAT_NUMBER         13
#define HOSAL_PWM_GET_PLAY_NUMBER           14
#define HOSAL_PWM_GET_TSEQ_ADDRESS          15
#define HOSAL_PWM_GET_RSEQ_ADDRESS          16
#define HOSAL_PWM_SET_CLOCK_DIVIDER         17
#define HOSAL_PWM_GET_PHASE                 18
#define HOSAL_PWM_SET_PHASE                 19
#define HOSAL_PWM_SET_COUNT_END_VALUE       20
#define HOSAL_PWM_SET_DMA_FORMAT            21
/**
 * \brief           software interrupt typedef callback function
 */
#define HOSAL_PWM_0                         0
#define HOSAL_PWM_1                         1
#define HOSAL_PWM_2                         2
#define HOSAL_PWM_3                         3
#define HOSAL_PWM_4                         4
//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Typedef struct Definition
//=============================================================================

typedef struct {
    pwm_config_t config;				/*!< pwm config struct*/
} hosal_pwm_dev_t;
//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
/**
 * \brief pwm pin config function.
 * \param[in]    id            pwm identifier
 * \param[in]    pin_number    pwm pin number.
 * \return       					 function status
 * \details
 *
 */
int hosal_pwm_pin_conifg(uint32_t id, uint32_t pin_number);

/**
 * \brief pwm format1 initinalfunction.
 * \param[in]    hosal_pwm_dev_t   pwm device config struct
 * \return       					 function status
 * \details
 *
 */
int hosal_pwm_init_fmt1_ex(hosal_pwm_dev_t *dev);
/**
 * \brief pwm format0 initinalfunction.
 * \param[in]    hosal_pwm_dev_t   pwm device config struct
 * \return       					 function status
 * \details
 *
 */
int hosal_pwm_init_fmt0_ex(hosal_pwm_dev_t *dev);
/**
 * \brief pwm format1 duty function.
 * \param[in]    id   pwm identifier
 * \param[in]    duty   pwm duty value (0~100)
* \return           function status
 * \details
 *
 */
int hosal_pwm_fmt1_duty_ex(uint32_t id, uint8_t duty);
/**
 * \brief pwm format0 duty function.
 * \param[in]    id   pwm identifier
 * \param[in]    duty   pwm duty value (0~100)
 * \return           function status
 * \details
 *
 */
int hosal_pwm_fmt0_duty_ex(uint32_t id, uint8_t duty);
/**
 * \brief pwm format1 count function.
 * \param[in]    id                pwm identifier
 * \param[in]    count             pwm count value
 * \return           function status
 * \details
 *
 */
int hosal_pwm_fmt1_count_ex(uint32_t id, uint32_t count);
/**
 * \brief pwm format0 duty function.
 * \param[in]    id                pwm identifier
 * \param[in]    count             pwm count value
 * \return           function status
 * \details
 *
 */
int hosal_pwm_fmt0_count_ex(uint32_t id, uint32_t count);
/**
 * \brief pwm multi element intinal function.
 * \param[in]    hosal_pwm_dev_t   pwm device config struct
 * \return           function status
 * \details
 */
int hosal_pwm_multi_init_ex(hosal_pwm_dev_t *dev);
/**
 * \brief pwm format1 duty function.
 * \param[in]    id                 pwm identifier
 * \param[in]    hosal_pwm_dev_t    pwm device struct
 * \param[in]    element            pwm element value
 * \param[in]    duty               pwm  value
 * \return           function status
 * \details
 */
int hosal_pwm_multi_fmt1_duty_ex(uint32_t id, hosal_pwm_dev_t *dev, uint32_t element, uint8_t duty);
/**
 * \brief pwm format0 duty function.
 * \param[in]    id                  pwm identifier
 * \param[in]    hosal_pwm_dev_t     pwm device struct
 * \param[in]    element             pwm element value
 * \param[in]    thd1_duty           pwm  thread 1 duty value
 * \param[in]    thd2_duty           pwm  thread 2 duty value
 * \return       					 function status
 * \details
 */
int hosal_pwm_multi_fmt0_duty_ex(uint32_t id, hosal_pwm_dev_t *dev, uint32_t element, uint8_t thd1_duty, uint8_t thd2_duty);
/**
 * \brief pwm format0 count function.
 * \param[in]    id                 pwm identifier
 * \param[in]    hosal_pwm_dev_t    pwm device struct
 * \param[in]    element            pwm element value
 * \param[in]    count              pwm  count value
* \return       					 function status 
 * \details
 */
int hosal_pwm_multi_fmt1_count_ex(uint32_t id, hosal_pwm_dev_t *dev, uint32_t element, uint32_t count);
/**
 * \brief pwm format0 count function.
 * \param[in]    id                 pwm identifier
 * \param[in]    hosal_pwm_dev_t    pwm device struct
 * \param[in]    element            pwm element value
 * \param[in]    count              pwm  count value
 * \return       					 function status
 * \details
 */
int hosal_pwm_multi_fmt0_count_ex(uint32_t id, hosal_pwm_dev_t *dev, uint32_t element, uint32_t thd1_Count, uint32_t thd2_count);
/**
 * \brief pwm stop function.
 * \param[in]    id                 pwm identifier
 * \return       					 function status
 * \details
 */
int hosal_pwm_sotp_ex(uint32_t id);
/**
 * \brief pwm start function.
 * \param[in]    id                 pwm identifier
 * \return       					 function status
 * \details
 */
int hosal_pwm_start_ex(uint32_t id);
/**
 * \brief pwm paramater function.
 * \param[in]    id                 pwm identifier
 * \param[in]    ctl                pwm control command
 * \param[in]    p_arg              pwm paramater
 * \return       					 function status
 * \details
 */
int hosal_pwm_ioctl(hosal_pwm_dev_t *dev, int ctl, void *p_arg);



#ifdef __cplusplus
}
#endif

#endif
