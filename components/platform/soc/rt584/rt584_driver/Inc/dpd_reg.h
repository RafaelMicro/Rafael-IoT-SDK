/**
 * \file            dpd_reg.h
 * \brief           Deep power down register header file
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
#ifndef DPD_REG_H
#define DPD_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           DPD reset cause register (0x00)
 */
typedef union dpd_rst_cause_s {
    struct dpd_rst_cause_b {
        uint32_t rst_cause_por  : 1;            /*!< wake up by power on */
        uint32_t rst_cause_ext  : 1;            /*!< wake up by external reset */
        uint32_t rst_cause_dpd  : 1;            /*!< wake up by power on */
        uint32_t rst_cause_ds   : 1;            /*!< wake up by power on */
        uint32_t rst_cause_wdt  : 1;            /*!< wake up by power on */
        uint32_t rst_cause_soft : 1;            /*!< wake up by power on */
        uint32_t rst_cause_lock : 1;            /*!< wake up by power on */
        uint32_t reserved       : 1;            /*!< wake up by power on */
        uint32_t boot_status    : 3;            /*!< wake up by power on */
        uint32_t reserve2       : 21;           /*!< reserved bits */
    } bit;
    uint32_t reg;
} dpd_rst_cause_t;

/**
 * \brief           DPD command register (0x04)
 */
typedef union dpd_cmd_s {
    struct dpd_cmd_b {
        uint32_t clr_rst_cause    : 1;          /*!< clear reset cause */
        uint32_t reserved         : 15;         /*!< reserved bits */
        uint32_t gpio_latch_en    : 1;          /*!< enable latch gpio state 
                                                    in deep power down mode */
        uint32_t reserved2        : 7;          /*!< reserved bits */
        uint32_t uvlo_div_sel     : 1;          /*!< uvlo test only */
        uint32_t uvlo_hys         : 1;          /*!< uvlo test only */
        uint32_t uvlo_ib          : 2;          /*!< uvlo test only */
        uint32_t reserved3        : 3;          /*!< reserved bits */
        uint32_t dpd_flash_dpd_en : 1;          /*!< elable Flash deep power down 
                                                    in low-power modes */
    } bit;
    uint32_t reg;
} dpd_cmd_t;

/**
 * \brief          Deep power down total register 
 */
typedef struct
{
    __IO dpd_rst_cause_t dpd_rst_cause;         /*!< 0x00 reset cause register */
    __IO dpd_cmd_t       dpd_cmd;               /*!< 0x04 dpd command register */
    __IO uint32_t        dpd_gpio_en;           /*!< 0x08 set gpio can wakeup in 
                                                    deep power down mode */
    __IO uint32_t        dpd_gpio_inv;          /*!< 0x0c set the wake up polarity 
                                                    of the corresponding GPIO */
    __IO uint32_t        dpd_ret0_reg;          /*!< 0x10 retention 0 register */
    __IO uint32_t        dpd_ret1_reg;          /*!< 0x14 retention 1 register */
    __IO uint32_t        dpd_ret2_reg;          /*!< 0x18 retention 2 register */
    __IO uint32_t        dpd_ret3_reg;          /*!< 0x1C retention 3 register */
} dpd_t;

#ifdef __cplusplus
}
#endif

#endif /* End of DPD_REG_H */
