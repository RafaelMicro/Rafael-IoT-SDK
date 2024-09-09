/**
 * \file            gpio.c
 * \brief           GPIO driver file
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

#include "gpio.h"


/**
 * \brief           Gpio_cb save callback and the p_context
 */
typedef struct {
    gpio_cb_fn gpio_handler;                    /*!< user application ISR handler. */
    void* p_context;                            /*!< the context to callback */
} gpio_cb;

static gpio_cb user_isr[MAX_NUMBER_OF_PINS];

void gpio_cfg(uint32_t pin_number, gpio_pin_dir_t dir, gpio_pin_int_mode_t int_mode) {
    uint32_t  MASK;

    assert_param(pin_number < MAX_NUMBER_OF_PINS);
    assert_param(dir < GPIO_PIN_DIR_INVALID);

    MASK = (1 << pin_number);

    if (dir == GPIO_PIN_DIR_INPUT) {
        GPIO->input_en = MASK;
        GPIO->enable_input = MASK;
    } else {
        GPIO->disable_input = MASK;
        GPIO->output_en = MASK;
    }

    user_isr[pin_number].gpio_handler = NULL;

    switch (int_mode) {
        case GPIO_PIN_INT_LEVEL_LOW:
            GPIO->negative   = MASK;
            GPIO->level_trig = MASK;
            break;
        case GPIO_PIN_INT_LEVEL_HIGH:
            GPIO->postitive  = MASK;
            GPIO->level_trig = MASK;
            break;
        case GPIO_PIN_INT_EDGE_RISING:
            GPIO->postitive  = MASK;
            GPIO->edge_trig  = MASK;
            break;
        case GPIO_PIN_INT_EDGE_FALLING:
            GPIO->negative   = MASK;
            GPIO->edge_trig  = MASK;
            break;
        case GPIO_PIN_INT_BOTH_EDGE:
            GPIO->both_edge_en = MASK;
            GPIO->edge_trig  = MASK;
            break;

        case GPIO_PIN_NOINT:
        default:
            GPIO->disable_int = MASK;
            break;
    }

    return;
}

void gpio_setup_deep_sleep_io(uint8_t pin_number, gpio_pin_wake_t level) {
    uint32_t mask;

    mask = 1 << pin_number;
    GPIO->set_ds_en |= mask;
    if ( level == GPIO_LEVEL_LOW ) {
        GPIO->dis_ds_inv |= mask;
    } else {
        GPIO->set_ds_inv |= mask;
    }
}

void gpio_disable_deep_sleep_io(uint8_t pin_number) {
    uint32_t mask;

    mask = 1 << pin_number;
    GPIO->dis_ds_en |= mask;
}

void gpio_setup_deep_powerdown_io(uint8_t pin_number, gpio_pin_wake_t level) {
    uint32_t mask;

    mask = 1 << pin_number;

    DPD_CTRL->dpd_gpio_en |= mask;

    if ( level == GPIO_LEVEL_LOW ) {
        DPD_CTRL->dpd_gpio_inv &= ~mask;
    } else {
        DPD_CTRL->dpd_gpio_inv |= mask;
    }
}

void gpio_disable_deep_powerdown_io(uint8_t pin_number) {
    uint32_t mask;

    mask = 1 << pin_number;

    DPD_CTRL->dpd_gpio_en &= ~mask;
}

void gpio_setup_io_schmitt(uint8_t pin_number) {
    uint32_t mask;

    mask = 1 << pin_number;

    SYSCTRL->gpio_en_schmitt |= mask;
}

void gpio_disable_io_schmitt(uint8_t pin_number) {
    uint32_t mask;

    mask = 1 << pin_number;

    SYSCTRL->gpio_en_schmitt &= ~mask;
}

void gpio_setup_io_filter(uint8_t pin_number) {
    uint32_t mask;

    mask = 1 << pin_number;

    SYSCTRL->gpio_en_filter |= mask;
}

void gpio_disable_io_filter(uint8_t pin_number) {
    uint32_t mask;

    mask = 1 << pin_number;

    SYSCTRL->gpio_en_filter &= ~mask;
}

void gpio_register_callback(uint32_t pin_number, gpio_cb_fn app_gpio_callback,
                            void* param) {
    uint32_t Mask = 1;

    user_isr[pin_number].gpio_handler = app_gpio_callback;
    user_isr[pin_number].p_context = param;

    GPIO->edge_int_clr |= Mask << pin_number;
    return;
}

void gpio_cancell_callback(uint32_t pin_number) {
    user_isr[pin_number].gpio_handler = NULL;
    user_isr[pin_number].p_context = NULL;

    return;
}

/**
 * \brief           GPIO Interrupt Handler
 */
void gpio_handler(void) {
    gpio_cb_fn app_isr;

    uint32_t  irq_state;
    uint32_t  i = 0, Mask = 1;

    irq_state = GPIO->int_status;

    for (i = 0; i < MAX_NUMBER_OF_PINS; i++, Mask <<= 1) {
        if (irq_state & Mask) {
            app_isr = user_isr[i].gpio_handler;

            /*clear Edgeinterrupt status..
             * if the interrupt source is level trigger, this clear
             * does NOT have change...
             */
            if (app_isr != NULL) {
                app_isr(i, user_isr[i].p_context);
            }
            GPIO->edge_int_clr = Mask;
        }
    }

    return;
}
