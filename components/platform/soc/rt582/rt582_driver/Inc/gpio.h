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
extern "C"
{
#endif /* __cplusplus */


/**
 * \brief           Max number of gpio pin
 */
#define  MAX_NUMBER_OF_PINS    (32)             /*!< Specify Maximum Pins of GPIO   */

/**
 * \brief           Gpio debounce_time definitions
 */
#define  DEBOUNCE_SLOWCLOCKS_32     (0)         /*!< setting for sampling cycle = 32 clocks   */
#define  DEBOUNCE_SLOWCLOCKS_64     (1)         /*!< setting for sampling cycle = 64 clocks   */
#define  DEBOUNCE_SLOWCLOCKS_128    (2)         /*!< setting for sampling cycle = 128 clocks  */
#define  DEBOUNCE_SLOWCLOCKS_256    (3)         /*!< setting for sampling cycle = 256 clocks  */
#define  DEBOUNCE_SLOWCLOCKS_512    (4)         /*!< setting for sampling cycle = 512 clocks  */
#define  DEBOUNCE_SLOWCLOCKS_1024   (5)         /*!< setting for sampling cycle = 1024 clocks */
#define  DEBOUNCE_SLOWCLOCKS_2048   (6)         /*!< setting for sampling cycle = 2048 clocks */
#define  DEBOUNCE_SLOWCLOCKS_4096   (7)         /*!< setting for sampling cycle = 4096 clocks */


/**
 * \brief           Pin direction definitions.
 */
typedef enum
{
    GPIO_PIN_DIR_INPUT,                         /*!< GPIO Input Mode   */
    GPIO_PIN_DIR_OUTPUT,                        /*!< GPIO Output Mode  */
    GPIO_PIN_DIR_INVALID
} gpio_pin_dir_t;

/**
 * \brief           Selecting the pin to sense high or low level, edge for pin input.
 */
typedef enum
{
    GPIO_PIN_NOINT,                             /*!< GPIO Interrupt mode disable  */
    GPIO_PIN_INT_LEVEL_LOW,                     /*!< GPIO Interrupt enable for Level-Low */
    GPIO_PIN_INT_LEVEL_HIGH,                    /*!< GPIO Interrupt enable for Level-High  */
    GPIO_PIN_INT_EDGE_RISING,                   /*!< GPIO Interrupt enable for Rising Edge */
    GPIO_PIN_INT_EDGE_FALLING,                  /*!< GPIO Interrupt enable for Falling Edge */
    GPIO_PIN_INT_BOTH_EDGE,                     /*!< GPIO Interrupt enable for both Rising and Falling Edge  */
} gpio_pin_int_mode_t;

/**
 * \brief           GPIO interrupt service routine callback for user application.
 * \param[in]       pin: interrupt pin number
 * \param[in]       isr_param: isr_param passed to user interrupt handler
 */
typedef void (* gpio_cb_fn)(uint32_t pin, void *isr_param);


/**
 * \brief           GPIO pin configuration function
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       dir: pin direction.
 * \param[in]       int_mode: pin interrupt mode
 */
void gpio_cfg(uint32_t pin_number, gpio_pin_dir_t dir, 
              gpio_pin_int_mode_t int_mode);

/**
 * \brief           Register user interrupt ISR callback function.
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       gpio_cb: Specifies user callback function when 
 *                           the pin interrupt generated.
 * \param[in]       param: passed to user interrupt handler "gpio_cb"
 */
void gpio_callback_register(uint32_t pin_number, gpio_cb_fn gpio_cb, void *param);

/**
 * \brief           Set gpio pin to output mode
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_cfg_output(uint32_t pin_number);

/**
 * \brief           Set gpio pin to input mode
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       int_mode: specifies the pin number interrupt if this pin 
 *                            need to be gpio interrupt source
 */
void gpio_cfg_input(uint32_t pin_number, 
                    gpio_pin_int_mode_t int_mode);

/**
 * \brief           Set gpio pin output high
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_pin_set(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    GPIO->state.output_high = (1 << pin_number);
}

/**
 * \brief           Set gpio pin output low
 * \param[in]       pin_number: specifies the pin number
 */
__STATIC_INLINE void gpio_pin_clear(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    GPIO->int_status.output_low = (1 << pin_number);
}

/**
 * \brief           Set gpio pin output value
 * \param[in]       pin_number: specifies the pin number
 * \param[in]       value: value 0 for output low, value 1 for output high
 */
void gpio_pin_write(uint32_t pin_number, uint32_t value);

/**
 * \brief           Toggle gpio pin output value
 * \param[in]       pin_number: specifies the pin number
 */
void gpio_pin_toggle(uint32_t pin_number);

/**
 * \brief           Get gpio pin input value
 * \param[in]       pin_number: specifies the pin number
 * \return          pin input value,0 is low level, 1 is high level
 */
__STATIC_INLINE uint32_t gpio_pin_get(uint32_t pin_number) {
    assert_param(pin_number < MAX_NUMBER_OF_PINS);

    return ((GPIO->state.state & (1 << pin_number)) ? 1 : 0);
}

/**
 * \brief           Enable gpio pin interrupt
 * \param[in]       pin_number: specifies the pin number that enable interrupt
 */
__STATIC_INLINE void gpio_int_enable(uint32_t pin) {
    assert_param(pin < MAX_NUMBER_OF_PINS);
    GPIO->enable_int = (1 << pin);
}

/**
 * \brief           Disable gpio pin interrupt
 * \param[in]       pin_number: specifies the pin number that disable interrupt
 */
__STATIC_INLINE void gpio_int_disable(uint32_t pin) {
    assert_param(pin < MAX_NUMBER_OF_PINS);
    GPIO->disable_int = (1 << pin);
}

/**
 * \brief           Enable gpio pin debounce function
 * \param[in]       pin_number: specifies the pin number that enable debounce 
 *                              when interrupt happened
 */
void gpio_debounce_enable(uint32_t pin);

/**
 * \brief           Disable gpio pin debounce function
 * \param[in]       pin_number: specifies the pin number that disable debounce 
 *                              when interrupt happened
 */
void gpio_debounce_disable(uint32_t pin);

/**
 * \brief           Set GPIO debounce time
 * \param[in]       mode: Specifies the sampling clock of debounce function 
 */
void gpio_set_debounce_time(uint32_t mode);


#ifdef __cplusplus
}
#endif

#endif /* End of GPIO_H */
