/**
 * \file            gpio.h
 * \brief           GPIO driver header file
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
#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <stdio.h>
#include <string.h>
#include "mcu.h"


/**
 * \brief           Max number of gpio pin
 */
#define  MAX_NUMBER_OF_PINS    (32)             /*!< Specify Maximum Pins of GPIO */

/**
 * \brief           GPIO_DEBOUNCE CLOCK Constant Definitions
 */
#define  DEBOUNCE_SLOWCLOCKS_32     (0)         /*!< setting for sampling cycle 
                                                    = 32 clocks */
#define  DEBOUNCE_SLOWCLOCKS_64     (1)         /*!< setting for sampling cycle 
                                                    = 64 clocks */
#define  DEBOUNCE_SLOWCLOCKS_128    (2)         /*!< setting for sampling cycle 
                                                    = 128 clocks */
#define  DEBOUNCE_SLOWCLOCKS_256    (3)         /*!< setting for sampling cycle 
                                                    = 256 clocks */
#define  DEBOUNCE_SLOWCLOCKS_512    (4)         /*!< setting for sampling cycle 
                                                    = 512 clocks */
#define  DEBOUNCE_SLOWCLOCKS_1024   (5)         /*!< setting for sampling cycle 
                                                    = 1024 clocks */
#define  DEBOUNCE_SLOWCLOCKS_2048   (6)         /*!< setting for sampling cycle 
                                                    = 2048 clocks */
#define  DEBOUNCE_SLOWCLOCKS_4096   (7)         /*!< setting for sampling cycle 
                                                    = 4096 clocks */


/**
 * \brief           Pin direction definitions.
 */
typedef enum {
    GPIO_PIN_DIR_INPUT,                         /*!< GPIO Input Mode */
    GPIO_PIN_DIR_OUTPUT,                        /*!< GPIO Output Mode */
    GPIO_PIN_DIR_INVALID
} gpio_pin_dir_t;

/**
 * \brief           Selecting the pin to sense high or low level, edge for pin input.
 */
typedef enum {
    GPIO_PIN_NOINT,                             /*!< GPIO Interrupt mode disable */
    GPIO_PIN_INT_LEVEL_LOW,                     /*!< GPIO Interrupt enable for 
                                                    Level-Low */
    GPIO_PIN_INT_LEVEL_HIGH,                    /*!< GPIO Interrupt enable for 
                                                    Level-High */
    GPIO_PIN_INT_EDGE_RISING,                   /*!< GPIO Interrupt enable for 
                                                    Rising Edge */
    GPIO_PIN_INT_EDGE_FALLING,                  /*!< GPIO Interrupt enable for 
                                                    Falling Edge */
    GPIO_PIN_INT_BOTH_EDGE,                     /*!< GPIO Interrupt enable for 
                                                    both Rising and Falling Edge */
} gpio_pin_int_mode_t;

/**
 * \brief           Selecting the pin to wake up high or low level.
 */
typedef enum {
    GPIO_LEVEL_LOW,                             /*!< GPIO Level-Low wake up */
    GPIO_LEVEL_HIGH,                            /*!< GPIO Level-High wake up */
} gpio_pin_wake_t;

/**
 * \brief           GPIO interrupt service routine callback for user application.
 * \param[in]       pin_number: interrupt pin number
 * \param[in]       isr_param: isr_param passed to user interrupt handler
 */
typedef void (*gpio_cb_fn)(uint32_t pin_number, void* isr_param);

/**
 * \brief           GPIO pin configuration function
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       dir: pin direction.
 * \param[in]       int_mode: pin interrupt mode
 */
void gpio_cfg(uint32_t pin_number, gpio_pin_dir_t dir, gpio_pin_int_mode_t int_mode);

/**
 * \brief           Register user interrupt ISR callback function.
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       gpio_cb: specifies user callback function when 
 *                           the pin interrupt generated.
 * \param[in]       param: passed to user interrupt handler "gpio_cb"
 */
void gpio_register_callback(uint32_t pin_number, gpio_cb_fn gpio_cb,
                            void* param);

/**
 * \brief           Setup gpio wakeup from deep sleep with level high or low.
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       level: set wakeup polarity low or high in deepsleep.
 */
void gpio_setup_deep_sleep_io(uint8_t pin_number, gpio_pin_wake_t level);

/**
 * \brief           Disable gpio wakeup from deep sleep.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_disable_deep_sleep_io(uint8_t pin_number);

/**
 * \brief           Setup gpio wakeup from deep power down with level high or low.
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       level: set wakeup polarity low or high in deep power down.
 */
void gpio_setup_deep_powerdown_io(uint8_t pin_number, gpio_pin_wake_t level);

/**
 * \brief           Disable gpio wakeup from deep power down.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_disable_deep_powerdown_io(uint8_t pin_number);

/**
 * \brief           Setup gpio Schmitt.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_setup_io_schmitt(uint8_t pin_number);

/**
 * \brief           Disable gpio Schmitt.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_disable_io_schmitt(uint8_t pin_number);

/**
 * \brief           Setup gpio filter.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_setup_io_filter(uint8_t pin_number);

/**
 * \brief           Disable gpio filter.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_disable_io_filter(uint8_t pin_number);

/**
 * \brief           Set gpio pin to output mode.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_cfg_output(uint32_t pin_number) {
    gpio_cfg(pin_number, GPIO_PIN_DIR_OUTPUT, GPIO_PIN_NOINT);
}

/**
 * \brief           Set gpio pin to input mode.
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       int_mode: specifies the pin number interrupt if this pin 
 *                            need to be gpio interrupt source
 */
__STATIC_INLINE void gpio_cfg_input(uint32_t pin_number, 
                                    gpio_pin_int_mode_t int_mode) {
    gpio_cfg(pin_number, GPIO_PIN_DIR_INPUT, int_mode);
}

/**
 * \brief           Set gpio pin output high.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_pin_set(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    GPIO->output_high = (1 << pin_number);
}

/**
 * \brief           Set gpio pin output low.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_pin_clear(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    GPIO->output_low = (1 << pin_number);
}

/**
 * \brief           Set gpio pin output value.
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       value: value 0 for output low, value 1 for output high.
 */
__STATIC_INLINE void gpio_pin_write(uint32_t pin_number, uint32_t value) {
    if (value == 0)     {
        gpio_pin_clear(pin_number);
    } else {
        gpio_pin_set(pin_number);
    }
}

/**
 * \brief           Toggle gpio pin output value.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_pin_toggle(uint32_t pin_number) {
    uint32_t state, MASK;

    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    MASK = (1 << pin_number);
    state = GPIO->output_state & MASK;

    if (state) {
        GPIO->output_low = MASK;
    } else {
        GPIO->output_high = MASK;
    }
}

/**
 * \brief           Get gpio pin input value.
 * \param[in]       pin_number: specifies the pin number
 * \return          1 for input pin is high, 0 for input is low.
 */
__STATIC_INLINE uint32_t gpio_pin_get(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    return ((GPIO->state & (1 << pin_number)) ? 1 : 0);
}

/**
 * \brief           Enable gpio pin interrupt.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_int_enable(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);
    GPIO->enable_int = (1 << pin_number);
}

/**
 * \brief           Disable gpio pin interrupt
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_int_disable(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);
    GPIO->disable_int = (1 << pin_number);
}

/**
 * \brief           Enable gpio pin debounce function.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_debounce_enable(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);
    GPIO->debouce_en = (1 << pin_number);
}

/**
 * \brief           Disable gpio pin debounce function.
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_debounce_disable(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);
    GPIO->debouce_dis = (1 << pin_number);
}

/**
 * \brief           Set GPIO debounce time.
 * \param[in]       time: Specifies the sampling clock of debounce function.
 */
__STATIC_INLINE void gpio_set_debounce_time(uint32_t time) {
    assert_param(time < DEBOUNCE_SLOWCLOCKS_4096);

    if (time > DEBOUNCE_SLOWCLOCKS_4096) {
        time = DEBOUNCE_SLOWCLOCKS_4096;
    }

    GPIO->debounce_time = time;
}

/**
 * \brief           Disable gpio callback.
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_cancell_callback(uint32_t pin_number);

#ifdef __cplusplus
}
#endif

#endif /* End of GPIO_H */

