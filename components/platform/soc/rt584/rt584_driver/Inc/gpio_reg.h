/**
 * \file            gpio_reg.h
 * \brief           GPIO register header file
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

#ifndef GPIO_REG_H
#define GPIO_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Gpio total register
 */
typedef struct {

    union {
        __IM uint32_t state;                    /*!< 0x00 gpio pin state(read) */
        __OM uint32_t output_high;              /*!< 0x00 gpio set output high(write) */
    };                                          

    union {
        __IM uint32_t int_status;               /*!< 0x04 gpio interrupt status(read) */
        __OM uint32_t output_low;               /*!< 0x04 gpio set ouput low(write) */
    };

    __IOM uint32_t output_en;                   /*!< 0x08 gpio output enable */

    union {
        __IM uint32_t output_state;             /*!< 0x0C gpio output state(read) */
        __OM uint32_t input_en;                 /*!< 0x0C gpio input enable(write) */
    };

    union {
        __IO  uint32_t int_enable_status;       /*!< 0x10 gpio interruput enable 
                                                    status of enable(read) */
        __OM  uint32_t enable_int;              /*!< 0x10 gpio interrupt enable(write) */
    };
    __OM  uint32_t disable_int;                 /*!< 0x14 gpio interruput disable */
    __IOM uint32_t edge_trig;                   /*!< 0x18 edge trigger interrupt */
    __IOM uint32_t level_trig;                  /*!< 0x1C level trigger interrupt */
    __IOM uint32_t postitive;                   /*!< 0x20 gpio interrupt trigger 
                                                    condition(read), gpio high or 
                                                    rising edge trigger interrupt(write) */
    __IOM uint32_t negative;                    /*!< 0x24 low or falling edge
                                                    trigger interrupt */
    __IOM uint32_t both_edge_en;                /*!< 0x28 both edge trigger interrupt */
    __OM  uint32_t both_edge_clr;               /*!< 0x2C disable both edge trigger 
                                                    interrupt */
    __OM  uint32_t edge_int_clr;                /*!< 0x30 edge interrupt status clear */
    __IOM uint32_t loopback;                    /*!< 0x34 disable both edge trigger 
                                                    interrupt */
    __IOM uint32_t enable_input;                /*!< 0x38 set gpio input enable */
    __OM  uint32_t disable_input;               /*!< 0x3C set gpio input disable */
    __IOM uint32_t debouce_en;                  /*!< 0x40 enable debounce */
    __OM  uint32_t debouce_dis;                 /*!< 0x44 disable debounce */
    __IOM uint32_t debounce_time;               /*!< 0x48 debounce time settle */
    __IOM uint32_t reserve1;                    /*!< 0x4C reserved */

    __IOM uint32_t set_ds_en;                   /*!< 0x50 set corresponding gpio 
                                                    enable deepsleep wakeup(write),
                                                    the states of Deepsleep wakeup
                                                    enable for each GPIO(read)*/
    __OM  uint32_t dis_ds_en;                   /*!< 0x54 set corresponding gpio 
                                                    disable deepsleep wakeup(write)*/
    __IOM uint32_t set_ds_inv;                  /*!< 0x58, write 1 to set corresponding 
                                                    gpio wakeup polarity of high in 
                                                    deepsleep*/
    __OM  uint32_t dis_ds_inv;                  /*!< 0x5c, write 1 to set corresponding 
                                                    gpio wakeup polarity of low in 
                                                    deepsleep*/

} gpio_t;

#ifdef __cplusplus
}
#endif

#endif /* End of GPIO_REG_H */
