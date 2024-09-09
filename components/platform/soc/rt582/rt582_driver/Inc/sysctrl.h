/**
 * \file            sysctrl.h
 * \brief           System control driver header file
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

#ifndef SYSCTRL_H
#define SYSCTRL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/**
 * @brief  Define AHB SYSTEM CLOCK mode
 */
#define SYS_CLK_32MHZ    0              /*!< SYS HCLK RUN IN 32MHz  */
#define SYS_CLK_48MHZ    1              /*!< SYS HCLK RUN IN 48MHz  */
#define SYS_CLK_64MHZ    2              /*!< SYS HCLK RUN IN 64MHz  */

#define FLASH_DRV_SEL_MASK          (0x03<<16)
#define FLASH_DRV_SEL_SET           (0x01<<16)


#define HCLK_SEL_MASK            (3<<0)
#define HCLK_SEL_32M             (0)
#define HCLK_SEL_BASEBAND_PLL    (3)


#define HCLK_SLEEP_SELECT_SHIFT   4
#define SLEEP_CLOCK_SEL_MASK      (3<<HCLK_SLEEP_SELECT_SHIFT)

#define HCLK_SLEEP_DISABLE        0
#define HCLK_SLEEP_RUN_IN_32K     (1<<HCLK_SLEEP_SELECT_SHIFT)

#define PERCLK_SLEEP_DISABLE      0
#define PERCLK_SLEEP_RUN_IN_32K   (1<<5)

#define BASEBAND_PLL_EN_SHIFT     8
#define BASEBAND_PLL_MASK         (1<<BASEBAND_PLL_EN_SHIFT)

#define BASEBAND_PLL_ENABLE       (1<<BASEBAND_PLL_EN_SHIFT)
#define BASEBAND_PLL_DISABLE      (0<<BASEBAND_PLL_EN_SHIFT)

#define PLL_FREQ_MASK                   (3<<BASEBAND_PLL_EN_SHIFT)
#define PLL_RUN_IN_64M                   (3<<BASEBAND_PLL_EN_SHIFT)
#define PLL_RUN_IN_48M                   (1<<BASEBAND_PLL_EN_SHIFT)


#define BASEBAND_PLL_FREQ_SHIFT   9
#define BASEBAND_PLL_FREQ_MASK    (1<<BASEBAND_PLL_FREQ_SHIFT)

#define BASEBAND_PLL_64M          (1<<BASEBAND_PLL_FREQ_SHIFT)
#define BASEBAND_PLL_48M          (0<<BASEBAND_PLL_FREQ_SHIFT)
#define BASEBAND_PLL_32M          (0)

#define BASEBAND_PLL_FREQ_48M     (0<<BASEBAND_PLL_FREQ_SHIFT)
#define BASEBAND_PLL_FREQ_64M     (1<<BASEBAND_PLL_FREQ_SHIFT)

#define CK32_TIMER3_CLOCK_SHIFT    26
#define CK32_TIMER3_CLOCK_MASK     (1<<CK32_TIMER3_CLOCK_SHIFT)
#define CK32_TIMER4_CLOCK_SHIFT    27
#define CK32_TIMER4_CLOCK_MASK     (1<<CK32_TIMER4_CLOCK_SHIFT)

#define UART0_CLK                 16
#define UART1_CLK                 17
#define UART2_CLK                 18
#define I2CM_CLK                  19
#define QSPI0_CLK                 20
#define QSPI1_CLK                 21
#define TIMER0_CLK_ENABLE         22
#define TIMER1_CLK                23
#define TIMER2_CLK                24
#define I2S_CLK                   25

#define UART0_SLEEP_WAKE_EN_SHIFT   16
#define UART0_SLEEP_WAKE_EN_MASK    (1<<UART0_SLEEP_WAKE_EN_SHIFT)
#define UART0_SLEEP_WAKE_ENABLE     (1<<UART0_SLEEP_WAKE_EN_SHIFT)
#define UART0_SLEEP_WAKE_DISABLE    (0)

#define UART1_SLEEP_WAKE_EN_SHIFT   17
#define UART1_SLEEP_WAKE_EN_MASK    (1<<UART1_SLEEP_WAKE_EN_SHIFT)
#define UART1_SLEEP_WAKE_ENABLE     (1<<UART1_SLEEP_WAKE_EN_SHIFT)
#define UART1_SLEEP_WAKE_DISABLE    (0)

#define UART2_SLEEP_WAKE_EN_SHIFT   18
#define UART2_SLEEP_WAKE_EN_MASK    (1<<UART2_SLEEP_WAKE_EN_SHIFT)
#define UART2_SLEEP_WAKE_ENABLE     (1<<UART2_SLEEP_WAKE_EN_SHIFT)
#define UART2_SLEEP_WAKE_DISABLE    (0)


#define EN_HCLK_FROZEN_SHIFT        28
#define HCLK_FROZEN_MASK            (1<<EN_HCLK_FROZEN_SHIFT)
#define HCLK_FROZEN_ENABLE          (1<<EN_HCLK_FROZEN_SHIFT)
#define HCLK_FROZEN_DISABLE         (0)

#define EN_RTC_PCLK_DS_SHIFT        29
#define RTC_PCLK_DS_MASK            (1<<EN_RTC_PCLK_DS_SHIFT)
#define RTC_PCLK_DS_ENABLE          (1<<EN_RTC_PCLK_DS_SHIFT)
#define RTC_PCLK_DS_DISABLE         (0)


#define ENTER_SLEEP                 (1)
#define ENTER_DEEP_SLEEP            (2)


#define SLOW_CLOCK_SEL_SHIFT        6
#define SLOW_CLOCK_SEL_MASK         (0x3<<SLOW_CLOCK_SEL_SHIFT)

#define SLOW_CLOCK_INTERNAL         0
#define SLOW_CLOCK_FROM_GPIO        1
#define SLOW_CLOCK_32M_DIV_1000     2
#define SLOW_CLOCK_XO_32K           3


#define SYS_SCRATCH_OFFSET       (0x60)


#define LOWPOWER_SRAM_DS_AUTO_SHIFT   7
#define LOWPOWER_SRAM_DS_AUTO_MASK    (1<<LOWPOWER_SRAM_DS_AUTO_SHIFT)
#define LOWPOWER_SRAM_DS_AUTO_ENABLE  (1<<LOWPOWER_SRAM_DS_AUTO_SHIFT)
#define LOWPOWER_SRAM_DS_AUTO_DISABLE (0)

#define MAP_BASE         (SYSCTRL_BASE + 0x10)
#define PULLOPT_BASE     (SYSCTRL_BASE + 0x20)
#define DRV_BASE         (SYSCTRL_BASE + 0x30)
#define OD_BASE          (SYSCTRL_BASE + 0x38)


#define PLL_LOCK_DOWN                   1
#define PLL_UNLOCK_VIBIT_A              0
#define PLL_UNLOCK_VIBIT_B              3
#define PLL_UNLOCK_BANK_VCO_A           4
#define PLL_UNLOCK_BANK_VCO_B           7
#define PLL_CHECK_COUNT                 2
/*Because compiler code optimize, we should set PLL_WAIT_PERIOD as 4N */
#define PLL_WAIT_PERIOD              1200
#define PLL_DELAY_PERIOD             400

/**
 * @brief  Define pin function mode.
 * When system after power up, it should initialize the pin mode ASAP, and don't change setting anymore.
 * The pin mode will not change after sleep return back.
 *
 */
#define MODE_GPIO            0          /*!< set pin for GPIO mode  */
#define MODE_QSPI0           1          /*!< set pin for QSPI0 mode */
#define MODE_I2C             4          /*!< set pin for I2C mode   */
#define MODE_UART            6          /*!< set pin for UART mode  */

#define MODE_I2S             4          /*!< set pin for I2S mode  */
#define MODE_PWM             4          /*!< set pin for PWM mode  */
#define MODE_PWM0            1          /*!< set pin for PWM0 mode  */
#define MODE_PWM1            2          /*!< set pin for PWM1 mode  */
#define MODE_PWM2            3          /*!< set pin for PWM2 mode  */
#define MODE_PWM3            5          /*!< set pin for PWM3 mode  */
#define MODE_PWM4            7          /*!< set pin for PWM4 mode  */

#define MODE_QSPI1           5          /*!< set pin for QSPI1 mode  */

#define MODE_EXT32K          5          /*!< set pin for EXT32K mode, only GPIO0~GPIO9 available for this setting  */

/*NOTICE: The following setting only in GPIO0~GPIO3*/
#define MODE_QSPI0_CSN1       2         /*!< set pin for QSPI0 CSN1 mode, only GPIO0~GPIO3 available for this setting  */
#define MODE_QSPI0_CSN2       3         /*!< set pin for QSPI0 CSN2 mode, only GPIO0~GPIO3 available for this setting  */
#define MODE_QSPI0_CSN3       6         /*!< set pin for QSPI0 CSN3 mode, only GPIO0~GPIO3 available for this setting  */


/**
 * @brief  Define pin pull option
 */

/*Driving through setting mode*/
#define MODE_PULL_NONE         0          /*!< set pin for no pull, if you set pin to GPIO output mode, system will set this option for output pin */
#define MODE_PULLDOWN_10K      1          /*!< set pin for 10K pull down  */
#define MODE_PULLDOWN_100K     2          /*!< set pin for 100K pull down  */
#define MODE_PULLDOWN_1M       3          /*!< set pin for 1M pull down  */
#define MODE_PULLUP_10K        5          /*!< set pin for 10K pull up  */
#define MODE_PULLUP_100K       6          /*!< set pin for 100K pull up, this is default pin mode */
#define MODE_PULLUP_1M         7          /*!< set pin for 1M pull up  */

/*Define used for pin_set_pullopt */
#define PULL_NONE           0
#define PULL_DOWN_10K       1
#define PULL_DOWN_100K      2
#define PULL_DOWN_1M        3
#define PULL_UP_10K         5
#define PULL_UP_100K        6
#define PULL_UP_1M          7


/**
 * @brief  Define pin driver option
 */
#define DRV_4MA             0               /*!< set pin for 4mA driver   */
#define DRV_10MA            1               /*!< set pin for 10mA driver  */
#define DRV_14MA            2               /*!< set pin for 14mA driver  */
#define DRV_20MA            3               /*!< set pin for 20mA driver  */

/**
 * @brief  Define IC chip id  and chip revision information
 */

#define IC_CHIP_ID_MASK_SHIFT      8
#define IC_CHIP_ID_MASK           (0xFF<<IC_CHIP_ID_MASK_SHIFT)

#define IC_RT58X                  (0x70<<IC_CHIP_ID_MASK_SHIFT)     /*!< RT58X IC Chip ID  */

#define IC_CHIP_REVISION_MASK_SHIFT      4
#define IC_CHIP_REVISION_MASK            (0xF<<IC_CHIP_REVISION_MASK_SHIFT)

#define IC_CHIP_REVISION_MPA             (1<<IC_CHIP_REVISION_MASK_SHIFT)       /*!< RT58X IC Chip Revision ID For MPA  */
#define IC_CHIP_REVISION_MPB             (2<<IC_CHIP_REVISION_MASK_SHIFT)       /*!< RT58X IC Chip Revision ID For MPB  */

typedef enum {
    SYS_32MHZ_CLK = SYS_CLK_32MHZ,
    SYS_48MHZ_CLK = SYS_CLK_48MHZ,
    SYS_64MHZ_CLK = SYS_CLK_64MHZ,
} sys_clk_sel_t;


/**
 * \brief           Delay us handler prototype
 * \param[in]       number_of_us: number of delay us
 */
typedef void (*delay_us_fn)(uint32_t volatile number_of_us);

/**
 * \brief           Delay ms handler prototype
 * \param[in]       number_of_ms: number of delay ms
 */
typedef void (*delay_ms_fn)(uint32_t volatile number_of_ms);

extern delay_us_fn Delay_us;
extern delay_ms_fn Delay_ms;

/**
 * \brief           Delay function initialize
 */
void Delay_Init(void);

/**
 * \brief           Get pin function mode
 * \param[in]       pin_number: Specifies the pin number
 * \return          The pin function mode
 */
uint32_t pin_get_mode(uint32_t pin_number);

/**
 * \brief           Set pin function mode
 * \param[in]       pin_number: Specifies the pin number
 * \param[in]       mode: The specail function mode for the pin_number
 *                        Config GPIO To --> UART/I2S/PWM/SADC/I2C/SPI
 */
void pin_set_mode(uint32_t pin_number, uint32_t mode);

/**
 * \brief           Enable peripherial interface clock
 * \param[in]       clock_id: enable the specifies peripheral "clock_id" interface clock
 *                            UART0_CLK
 *                            UART1_CLK
 *                            UART2_CLK
 *                            I2CM_CLK
 *                            QSPI0_CLK
 *                            QSPI1_CLK
 *                            TIMER1_CLK
 *                            TIMER2_CLK
 *                            I2S_CLK
 */
void enable_perclk(uint32_t clock_id);

/**
 * \brief           Disable peripherial interface clock
 * \param[in]       clock_id: disable the specifies peripheral "clock_id" interface clock
 *                            UART0_CLK
 *                            UART1_CLK
 *                            UART2_CLK
 *                            I2CM_CLK
 *                            QSPI0_CLK
 *                            QSPI1_CLK
 *                            TIMER1_CLK
 *                            TIMER2_CLK
 *                            I2S_CLK
 */
void disable_perclk(uint32_t clock);

/**
 * \brief           Set pin pull option
 * \param[in]       pin_number: Specifies the pin number
 * \param[in]       mode: The specail pull option for the pin_number
 *                        PULL_NONE        0
 *                        PULL_DOWN_10K    1
 *                        PULL_DOWN_100K   2
 *                        PULL_DOWN_1M     3
 *                        PULL_UP_10K      5
 *                        PULL_UP_100K     6
 *                        PULL_UP_1M       7
 */
void pin_set_pullopt(uint32_t pin_number, uint32_t mode);

/**
 * @brief Set pin driving option
 *
 * @param[in] pin_number    Specifies the pin number.
 * @param[in] mode              pin driving option
 *                                              DRV_4MA      0
 *                                              DRV_10MA     1
 *                                              DRV_14MA     2
 *                                              DRV_20MA     3
 * @return
 *         NONE
 *
 * @details
 *      Pin default driving option is 20mA, User can use this function to change the pin driving setting.
 *
 */
extern void pin_set_drvopt(uint32_t pin_number, uint32_t mode);
/**
 * @brief Set pin to opendrain option
 *
 * @param[in] pin_number  Specifies the pin number.
 *                                              GPIO0~GPIO31
 * @return
 *         NONE
 *
 * @details
 *        Set the pin to opendrain mode.
 */
extern void enable_pin_opendrain(uint32_t pin_number);
/**
 * @brief Disable pin to opendrain option
 *
 * @param[in] pin_number   Specifies the pin number
 *                                               GPIO0~GPIO31
 * @return
 *         NONE
 *
 * @details
 *        Reset the pin to non-opendrain mode.
 */
extern void disable_pin_opendrain(uint32_t pin_number);
/**
 * @brief check pll unlock
 *
 * @param[in] NONE
 * @return    system clock mode 32MHZ/48MHz/64MHz
 * @retval    SYS_32MHZ_CLK   for CPU AHB 32MHz clock.
 * @retval    SYS_48MHZ_CLK   for CPU AHB 48MHz clock.
 * @retval    SYS_64MHZ_CLK   for CPU AHB 64MHz clock.
 * @details
 *
 *
 */
extern sys_clk_sel_t Pll_Unlock_Check(void);
/**
 * @brief Pll Status Check
 *
 * @param[in] NONE
 * @retval    STATUS_INVALID_REQUEST lock fail.
 * @retval    STATUS_EBUSY   pll lock again
 * @retval    STATUS_SUCCESS lock success.
 * @details
 */
extern uint32_t Pll_Status_Check(void);
/**
 * @brief Adjust RT58x HCLK clock
 *
 * @param[in] sys_clk_mode
 *                      SYS_32MHZ_CLK = 0,
 *                      SYS_48MHZ_CLK = 1,
 *                      SYS_64MHZ_CLK = 2,
 * @return
 * @retval    STATUS_SUCCESS for change success.
 * @retval    STATUS_ERROR   for Invalid parameter
 * @details
 *        Calling this function will change RT58x HCLK, it will also change flash timing setting.
 *
 */
extern uint32_t Change_Ahb_System_Clk(sys_clk_sel_t sys_clk_mode);

/**
 * @brief Get RT58x HCLK clock
 *
 * @return
 * @retval    SYS_CLK_32MHZ   for CPU AHB 32MHz clock.
 * @retval    SYS_CLK_48MHZ   for CPU AHB 48MHz clock.
 * @retval    SYS_CLK_64MHZ   for CPU AHB 64MHz clock.
 *
 */
extern sys_clk_sel_t Get_Ahb_System_Clk(void);

/*
 * Select Slow clock source.
 * Available mode:
 *         SLOW_CLOCK_INTERNAL   --- default value.
 *                  If system don't call this function, then slow clock source is from internal RCO by default.
 *
 *          SLOW_CLOCK_FROM_GPIO ---
 *                 If system set this mode, system should use an external 32K source from GPIO.
 *
 *
 */
extern void set_slow_clock_source(uint32_t mode);
/**
* @brief   Select external 32k input pin.
*          If system call this function, then the gpio will be slow clock source input.
* @param   pin_number: It should be GPIO0~GPIO7
* @return  None
*/
extern void set_ext32k_pin(uint32_t pin_number);

/**
 * @brief   Generate a true 32-bits random number.
 *
 * @return  a 32-bits random number
 *
 * @details
 *       Generate a true 32bits random number.
 *       Please notice: this function is block mode, it will block about 4ms ~ 6ms.
 *       If you want to use non-block mode, maybe you should use interrupt mode.
 *
 */
extern uint32_t get_random_number(void);

/**
 * @brief sys_set_retention_reg. Use to save some retention value.
 *
 * @param index: The index for which scratchpad register to save
 *                It should be 0~7.
 * @param value: register value
 *
 * @details   Please notice when system power-reset (cold boot), all system retention scratchpad register (0~7)
 *    will be reset to zero.
 *    System scratchpad register 6 is used for system hardware control:
 *           BIT0 for enable resetting  CM3 peripherals when watchdog reset is triggered.
 *           BIT1 for enable resetting  APBGPIO and Remap when watchdog reset is triggered.
 *           BIT2 for enable resetting  communication subsystem when watchdog reset is triggered.
 *
 *    System scratchpad register 7 is used for system hardware control:
 *           BIT0 for skip ISP function when system wakeup from deep sleep. This function is very useful for real product.
 *           BIT1 for RT58x communication subsystem used.
 *           BIT2 for RT58x application project to notify wakeup from sleep.
 *
 *    So users can use system retention scratchpad register 0~5 for their special purpose.
 *
 */

__STATIC_INLINE void sys_set_retention_reg(uint32_t index, uint32_t value) {
    if (index < 8) {
        outp32(((SYSCTRL_BASE + SYS_SCRATCH_OFFSET) + (index << 2)), value);
    }
}

/**
 * @brief sys_get_retention_reg. Use to get retention value.
 *
 * @param[in]   index:     The index for which scratchpad register to get
 *                         It should be 0~7.
 * @param[out]  *value     the address for return value.
 *
 *
 */

__STATIC_INLINE void sys_get_retention_reg(uint32_t index, uint32_t *value) {
    if (index < 8) {
        *value =  inp32((SYSCTRL_BASE + SYS_SCRATCH_OFFSET) + (index << 2));
    } else {
        *value = 0;    /*wrong index*/
    }
}

/*
 *
 */
__STATIC_INLINE void set_lowpower_control(uint32_t value) {
    SYSCTRL->sys_lowpower_ctrl = value;
}

__STATIC_INLINE uint32_t get_lowpower_control(void) {
    return ((uint32_t)(SYSCTRL->sys_lowpower_ctrl));
}

__STATIC_INLINE uint32_t get_clk_control(void) {
    return ((uint32_t)(SYSCTRL->sys_clk_ctrl));
}


__STATIC_INLINE void set_deepsleep_wakeup_pin(uint32_t value) {
    SYSCTRL->deepsleep_wakeup = value;
}

__STATIC_INLINE void set_deepsleep_wakeup_invert(uint32_t value) {
    SYSCTRL->deepsleep_inv = value;
}

__STATIC_INLINE void set_sram_shutdown_normal(uint32_t value) {
    SYSCTRL->sram_sd_nm = value;
}

__STATIC_INLINE void set_sram_shutdown_sleep(uint32_t value) {
    SYSCTRL->sram_sd_sl = value;
}


/**
 * @brief Check IC version
 *
 * @param  None
 *
 * @retval    IC version
 *
 * @details   Return IC version information
 *             Bit7 ~ Bit4 is chip_revision
 *             Bit15 ~ Bit8 is chip_id
 */

/*check Flash command Finish*/
__STATIC_INLINE uint32_t get_chip_version(void) {
    return ((uint32_t)(SYSCTRL->chip_info));
}


#ifdef __cplusplus
}
#endif

#endif /* End of SYSCTRL_H */
