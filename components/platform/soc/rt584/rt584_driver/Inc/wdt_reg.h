/**
 * \file            wdt_reg.h
 * \brief           watch dog timer register header file
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
#ifndef WDT_REG_H
#define WDT_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Watch dog timer control register (0x08)
 */
typedef union wdt_ctrl_s {
    struct wdt_ctrl_b {
        uint32_t lockout   : 1;                 /*!< wdt lockout bit */
        uint32_t reserved1 : 4;                 /*!< reserved bits */
        uint32_t reset_en  : 1;                 /*!< wdt reset enable bit */
        uint32_t int_en    : 1;                 /*!< wdt interrupt enable bit */
        uint32_t wdt_en    : 1;                 /*!< wdt enable bit */
        uint32_t reserved2 : 24;                /*!< reserved bits */
    } bit;
    uint32_t reg;
} wdt_ctrl_t;

/**
 * \brief           Watch dog timer reset register (0x10)
 */
typedef union wdt_reset_occur_s {
    struct wdt_reset_occur_b {
        uint32_t reset_occur : 8;               /*!< reset occur count */
        uint32_t reserved    : 24;              /*!< reserved bits */
    } bit;
    uint32_t reg;
} wdt_reset_occur_t;

/**
 * \brief           Watch dog timer total register 
 */
typedef struct
{
    __IO uint32_t          win_max;             /*!< 0x00 wdt reload value */
    __I  uint32_t          value;               /*!< 0x04 current wdt value */
    __IO wdt_ctrl_t        control;             /*!< 0x08 wdt control register */
    __IO uint32_t          wdt_kick;            /*!< 0x0C write 0xA5A5 
                                                    to reload wdt and 
                                                    clear interrupt */
    __IO wdt_reset_occur_t rst_occur;           /*!< 0x10 reset occurred counter, 
                                                    write any value to clear */
    __O  uint32_t          clear;               /*!< 0x14 write any value to 
                                                    clear interrupt */
    __IO uint32_t          int_value;           /*!< 0x18 wdt interrupt value, 
                                                    when 'load' equal 'int_value', 
                                                    interrupt will generated */
    __IO uint32_t          win_min;             /*!< 0x1C to avoid too quick 
                                                    trigger wdt*/
    __IO uint32_t          prescale;            /*!< 0x20 wdt prescale value */

} wdt_t;

#ifdef __cplusplus
}
#endif

#endif /* End of WDT_REG_H */
