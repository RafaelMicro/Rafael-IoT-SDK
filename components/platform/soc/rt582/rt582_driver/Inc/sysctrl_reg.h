/**
 * \file            sysctrl_reg.h
 * \brief           system control driver register header file
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

#ifndef SYSCTRL_REG_H
#define SYSCTRL_REG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \brief           System Control total register 
 */

typedef struct
{
    __IO uint32_t power_state_ctrl;             /*!< 0x00 cm3 power state */
    __IO uint32_t sys_clk_ctrl;                 /*!< 0x04 cm3 clock source */
    __IO uint32_t sys_lowpower_ctrl;            /*!< 0x08 low power control */
    __I  uint32_t reserved1;                    /*!< 0x0C reserved */
    __IO uint32_t gpio_map0;                    /*!< 0x10 gpio map0 0-7 */
    __IO uint32_t gpio_map1;                    /*!< 0x14 gpio map1 8-15 */
    __IO uint32_t gpio_map2;                    /*!< 0x18 gpio map2 16-23 */
    __IO uint32_t gpio_map3;                    /*!< 0x1C gpio map3 24-31 */
    __IO uint32_t gpio_pull_ctrl[4];            /*!< 0x20 - 0x2C gpio pull control */
    __IO uint32_t gpio_drv_ctrl[2];             /*!< 0x30 - 0x34 gpio driving control */
    __IO uint32_t gpio_od_ctrl;                 /*!< 0x38 gpio open drain control */
    __IO uint32_t gpio_aio_ctrl;                /*!< 0x38 gpio aio control */
    __IO uint32_t trng0;                        /*!< 0x40 */
    __IO uint32_t trng1;                        /*!< 0x44 */
    __IO uint32_t trng2;                        /*!< 0x48 */
    __IO uint32_t trng3;                        /*!< 0x4C */
    __IO uint32_t sram_sd_nm;                   /*!< 0x50 sram shut-down control 
                                                    in normal mode*/
    __IO uint32_t sram_sd_sl;                   /*!< 0x54 sram shut-down control 
                                                    in sleep/deepsleep*/
    __I  uint32_t reserced2[2];                 /*!< 0x58 - 0x5C reserved */
    __IO uint32_t sys_scratch[8];               /*!< 0x60 - 0x7C scratch pad */
    __IO uint32_t deepsleep_wakeup;             /*!< 0x80 enable the wakeup of 
                                                    gpiox in deep sleep */
    __IO uint32_t deepsleep_inv;                /*!< 0x84 invert the gpiox for 
                                                    wakeup in deep sleep */
    __I  uint32_t reserced3[2];                 /*!< 0x88 - 0x8C reserved */
    __IO uint32_t chip_info;                    /*!< 0x90 chip infomation */
} sysctrl_t;


#ifdef __cplusplus
}
#endif

#endif /* End of SYSCTRL_REG_H */

