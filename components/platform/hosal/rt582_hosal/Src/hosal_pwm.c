/**
 * \file            hosal_pwm.c
 * \brief           Hosal PWM driver file
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
 * Author:          ives.lee
 */
#include <stdint.h>
#include "mcu.h"

#include "hosal_pwm.h"
#include "stdio.h"

int hosal_pwm_pin_conifg(uint32_t id, uint32_t pin_number) {
    if ((pin_number < 8) || (pin_number > 23) || (pin_number == 16)
        || (pin_number == 17))
        return STATUS_INVALID_PARAM;

    switch (id) {
        case 0:
            pin_set_mode(pin_number, (pin_number == 8) ? MODE_PWM : MODE_PWM0);
            break;
        case 1:
            pin_set_mode(pin_number, (pin_number == 9) ? MODE_PWM : MODE_PWM1);
            break;
        case 2:
            pin_set_mode(pin_number, (pin_number == 14) ? MODE_PWM : MODE_PWM2);
            break;
        case 3:
            pin_set_mode(pin_number, (pin_number == 15) ? MODE_PWM : MODE_PWM3);
            break;
        case 4: pin_set_mode(pin_number, MODE_PWM4); break;
        default: break;
    }
    return STATUS_SUCCESS;
}

int hosal_pwm_init_fmt1_ex(hosal_pwm_dev_t* dev) {
    hosal_pwm_pin_conifg(dev->config.id, dev->config.pin_out);
    return pwm_init_fmt1_ex(dev->config.id, dev->config.frequency);
}

int hosal_pwm_init_fmt0_ex(hosal_pwm_dev_t* dev) {
    hosal_pwm_pin_conifg(dev->config.id, dev->config.pin_out);
    return pwm_init_fmt0_ex(dev->config.id, dev->config.frequency,
                            dev->config.count_end_val);
}

int hosal_pwm_fmt1_duty_ex(uint32_t id, uint8_t duty) {
    pwm_fmt1_duty_ex(id, duty);
    return STATUS_SUCCESS;
}

int hosal_pwm_fmt0_duty_ex(uint32_t id, uint8_t duty) {
    pwm_fmt0_duty_ex(id, duty);
    return STATUS_SUCCESS;
}

int hosal_pwm_fmt1_count_ex(uint32_t id, uint32_t count) {
    hosal_pwm_fmt1_count_ex(id, count);
    return STATUS_SUCCESS;
}

int hosal_pwm_fmt0_count_ex(uint32_t id, uint32_t count) {
    pwm_fmt0_count_ex(id, count);
    return STATUS_SUCCESS;
}

int hosal_pwm_multi_init_ex(hosal_pwm_dev_t* pwm_dev) {
    hosal_pwm_pin_conifg(pwm_dev->config.id, pwm_dev->config.pin_out);
    pwm_multi_init_ex(pwm_dev->config, pwm_dev->config.frequency);
    return STATUS_SUCCESS;
}

int hosal_pwm_multi_fmt1_duty_ex(uint32_t id, hosal_pwm_dev_t* pwm_dev,
                                 uint32_t element, uint8_t duty) {
    switch (pwm_dev->config.seq_order) {
        case PWM_SEQ_ORDER_T:
        case PWM_SEQ_ORDER_R:
            pwm_multi_fmt1_duty_ex(id, pwm_dev->config.seq_order, element,
                                   duty);
            break;
        default: return STATUS_INVALID_PARAM;
    }
    return STATUS_SUCCESS;
}

int hosal_pwm_multi_fmt0_duty_ex(uint32_t id, hosal_pwm_dev_t* pwm_dev,
                                 uint32_t element, uint8_t thd1_duty,
                                 uint8_t thd2_duty) {
    switch (pwm_dev->config.seq_order) {
        case PWM_SEQ_ORDER_T:
        case PWM_SEQ_ORDER_R:
            pwm_multi_fmt0_duty_ex(id, pwm_dev->config.seq_order, element,
                                   thd1_duty, thd2_duty);
            break;
        default: return STATUS_INVALID_PARAM;
    }
    return STATUS_SUCCESS;
}

int hosal_pwm_multi_fmt1_count_ex(uint32_t id, hosal_pwm_dev_t* pwm_dev,
                                  uint32_t element, uint32_t count) {
    switch (pwm_dev->config.seq_order) {
        case PWM_SEQ_ORDER_T:
        case PWM_SEQ_ORDER_R:
            pwm_multi_fmt1_count_ex(id, pwm_dev->config.seq_order, element,
                                    count);
            break;
        default: return STATUS_INVALID_PARAM;
    }
    return STATUS_SUCCESS;
}

int hosal_pwm_multi_fmt0_count_ex(uint32_t id, hosal_pwm_dev_t* pwm_dev,
                                  uint32_t element, uint32_t thd1_Count,
                                  uint32_t thd2_count) {
    switch (pwm_dev->config.seq_order) {
        case PWM_SEQ_ORDER_T:
        case PWM_SEQ_ORDER_R:
            pwm_multi_fmt0_count_ex(id, pwm_dev->config.seq_order, element,
                                    thd1_Count, thd2_count);
            break;
        default: return STATUS_INVALID_PARAM;
    }
    return STATUS_SUCCESS;
}

int hosal_pwm_start_ex(uint32_t id) { return (int)pwm_start_ex(id); }

int hosal_pwm_sotp_ex(uint32_t id) { return (int)pwm_stop_ex(id); }

int hosal_pwm_ioctl(hosal_pwm_dev_t* dev, int ctl, void* p_arg) {
    switch (ctl) {
        case HOSAL_PWM_GET_FRQUENCY:
            return pwm_get_frequency_ex(dev->config.id, p_arg);
        case HOSAL_PWM_SET_FRQUENCY:
            return pwm_set_frequency_ex(dev->config.id, dev->config.frequency);
        case HOSAL_PWM_SET_CLOCK_DIVIDER:
            return pwm_clock_divider_ex(dev->config.id, dev->config.clk_div);
        case HOSAL_PWM_GET_PHASE: return pwm_get_pahse(dev->config.id, p_arg);
        case HOSAL_PWM_SET_PHASE:
            return pwm_set_pahse_ex(dev->config.id, dev->config.phase);
        case HOSAL_PWM_GET_COUNT:
            return pwm_get_count_ex(dev->config.id, p_arg);
        case HOSAL_PWM_SET_COUNT_MODE:
            return pwm_set_counter_mode_ex(dev->config.id,
                                           dev->config.counter_mode);
        case HOSAL_PWM_SET_COUNT_END_VALUE:
            return pwm_set_counter_end_value_ex(dev->config.id,
                                                dev->config.count_end_val);
        case HOSAL_PWM_SET_DMA_FORMAT:
            return pwm_set_dma_format_ex(dev->config.id,
                                         dev->config.dma_smp_fmt);
        case HOSAL_PWM_SET_REPEAT_NUMBER:
            return pwm_set_repeat_number_ex(
                dev->config.id, dev->config.seq_order, *(uint32_t*)p_arg);
        case HOSAL_PWM_GET_REPEAT_NUMBER:
            return pwm_get_repeat_number_ex(dev->config.id,
                                            dev->config.seq_order, p_arg);
        case HOSAL_PWM_SET_DELAY_NUMBER:
            return pwm_set_delay_number_ex(
                dev->config.id, dev->config.seq_order, *(uint32_t*)p_arg);
        case HOSAL_PWM_GET_DELAY_NUMBER:
            return pwm_get_delay_number_ex(dev->config.id,
                                           dev->config.seq_order, p_arg);
        default: return -1;
    }
}
