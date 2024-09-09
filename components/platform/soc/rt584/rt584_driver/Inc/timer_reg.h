/**
 * \file            timer_reg.h
 * \brief           timer register header file
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

#ifndef TIMER_REG_H
#define TIMER_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * \brief           timer control register at offet 0x08
 */
typedef union timern_ctrl_s {
    struct timern_ctrl_b {
        uint32_t up_count              : 1;    /*!< timer counting mode setting */
        uint32_t one_shot_en            : 1;    /*!< timer one shot mode enable */
        uint32_t prescale               : 3;    /*!< timer prescale setting */
        uint32_t int_enable             : 1;    /*!< timer interrupt enable */
        uint32_t mode                   : 1;    /*!< timer freerun or periodic 
                                                    setting */
        uint32_t en                     : 1;    /*!< timer enable */

        uint32_t int_status             : 1;    /*!< timer interrupt status */
        uint32_t timer_enable_status    : 1;    /*!< timer enable status */
        uint32_t ch0_capture_int_status : 1;    /*!< timer capture channel 0
                                                    interrupt status */
        uint32_t ch1_capture_int_status : 1;    /*!< timer capture channel 1
                                                    interrupt status */
        uint32_t reserved1              : 4;    /*!< reserved bits */

        uint32_t ch0_capture_edge       : 1;    /*!< timer capture channel 0
                                                    which edge trigger */
        uint32_t ch1_capture_edge       : 1;    /*!< timer capture channel 1
                                                    which edge trigger */
        uint32_t ch0_deglich_en         : 1;    /*!< timer capture channel 0
                                                    deglich enable */
        uint32_t ch1_deglich_en         : 1;    /*!< timer capture channel 1
                                                    deglich enable */
        uint32_t ch0_capture_int_en     : 1;    /*!< timer capture channel 0
                                                    interrupt enable */
        uint32_t ch1_capture_int_en     : 1;    /*!< timer capture channel 1
                                                    interrupt enable */
        uint32_t reserved2              : 10;   /*!< reserved bits */
    } bit;
    uint32_t reg;
} timern_ctrl_t;

/**
 * \brief           timer capture interrupt clear register at offet 0x10
 */
typedef union timern_cap_clr_s {
    struct timern_cap_clr_b {
        uint32_t ch0_capture_int_clear : 1;     /*!< timer capture channel 0
                                                    interrupt clear */
        uint32_t ch1_capture_int_clear : 1;     /*!< timer capture channel 1
                                                    interrupt clear */
        uint32_t reserved1             : 30;    /*!< reserved bits */
    } bit;
    uint32_t reg;
} timern_cap_clr_t;

/**
 * \brief           timer capture and pwm enable register at offet 0x24
 */
typedef union timern_cap_en_s {
    struct timern_cap_en_b {
        uint32_t ch0_capture_en : 1;            /*!< timer capture channel 0
                                                    enable */
        uint32_t ch1_capture_en : 1;            /*!< timer capture channel 1
                                                    enable */
        uint32_t timer_pwm_en   : 1;            /*!< timer pwm enable */
        uint32_t reserved1      : 29;           /*!< reserved bits */
    } bit;
    uint32_t reg;
} timern_cap_en_t;

/**
 * \brief           timer capture chanel io select register at offet 0x28
 */
typedef union timern_cap_io_sel_s {
    struct timern_cap_io_sel_b {
        uint32_t ch0_capture_io_sel : 5;        /*!< timer capture channel 0
                                                    io select */
        uint32_t reserved1          : 3;        /*!< reserved bits */
        uint32_t ch1_capture_io_sel : 5;        /*!< timer capture channel 1
                                                    io select */
        uint32_t reserved2          : 19;       /*!< reserved bits */
    } bit;
    uint32_t reg;
} timern_cap_io_sel_t;

/**
 * \brief           timer pwm phase setting register at offet 0x30
 */
typedef union timern_pha_s {
    struct timern_pha_b {
        uint32_t pha       : 1;                 /*!< timer pwm phase */
        uint32_t reserved1 : 31;                /*!< reserved bits */
    } bit;
    uint32_t reg;
} timern_pha_t;

typedef struct
{
    __IO uint32_t            load;              /*!< 0x00 timer load */
    __IO uint32_t            value;             /*!< 0x04 timer value */
    __IO timern_ctrl_t       control;           /*!< 0x08 timer control */
    __IO uint32_t            clear;             /*!< 0x0C timer interrupt clear */
    __O  timern_cap_clr_t    capture_clear;     /*!< 0x10 timer capture interrupt 
                                                    clear */
    __I  uint32_t            ch0_cap_value;     /*!< 0x14 timer capture channel 0
                                                    capture value */
    __I  uint32_t            ch1_cap_value;     /*!< 0x18 timer capture channel 1
                                                    capture value */
    __IO uint32_t            prescale;          /*!< 0x1C timer user prescale */
    __IO uint32_t            expried_value;     /*!< 0x20 timer expire value */
    __IO timern_cap_en_t     cap_en;            /*!< 0x24 timer capture enable */
    __IO timern_cap_io_sel_t cap_io_sel;        /*!< 0x28 timer capture io select */
    __IO uint32_t            thd;               /*!< 0x2C timer pwm threshold */
    __IO timern_pha_t        pha;               /*!< 0x30 timer pwm phase */

} timern_t;


/**
 * \brief           slow timer control register at offet 0x08
 */
typedef union slowtimern_ctrl_s
{
    struct slowtimern_ctrl_b {
        uint32_t up_count           : 1;       /*!< slow timer counting mode 
                                                    setting */
        uint32_t one_shot_en         : 1;       /*!< slow timer one shot mode enable */
        uint32_t prescale            : 3;       /*!< slow timer prescale setting */
        uint32_t int_enable          : 1;       /*!< slow timer interrupt enable */
        uint32_t mode                : 1;       /*!< slow timer freerun or periodic 
                                                    setting */
        uint32_t en                  : 1;       /*!< slow timer enable */
        uint32_t int_status          : 1;       /*!< slow timer interrupt status */
        uint32_t timer_enable_status : 1;       /*!< slow timer enable status */
        uint32_t reserved1           : 22;      /*!< reserved bits */
    } bit;
    uint32_t reg;
} slowtimern_ctrl_t;


/**
 * \brief           slow timer repeat delay register at offet 0x10
 */
typedef union slowtimern_repdly_s {
    struct slowtimern_repdly_b {
        uint32_t int_repeat_delay         : 16; /*!< slow timer repeat delay times */
        uint32_t int_repeat_delay_disable : 1;  /*!< slow timer repeat delay disable */
        uint32_t reserved                 : 15; /*!< reserved bits */
    } bit;
    uint32_t reg;
} slowtimern_repdly_t;

typedef struct
{
    __IO uint32_t            load;              /*!< 0x00 slow timer load */
    __IO uint32_t            value;             /*!< 0x04 slow timer value */
    __IO slowtimern_ctrl_t   control;           /*!< 0x08 slow timer control 
                                                    register */
    __IO uint32_t            clear;             /*!< 0x0C slow timer interrupt 
                                                    clear register */
    __O  slowtimern_repdly_t repeat_delay;      /*!< 0x10 slow timer repeat delay */
    __IO uint32_t            prescale;          /*!< 0x14 slow timer user prescale */
    __IO uint32_t            expried_value;     /*!< 0x18 slow timer ecpire value */

} slowtimern_t;

#ifdef __cplusplus
}
#endif

#endif /* End of TIMER_REG_H */
