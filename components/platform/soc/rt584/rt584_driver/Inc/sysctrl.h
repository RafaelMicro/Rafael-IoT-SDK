/******************************************************************************
 * @file     sysctrl.h
 * @version
 * @brief    SYS CONTROL driver header file
 *
 * @copyright
 ******************************************************************************/
/** @defgroup Sys_Ctrl System_Control
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  System Control Driver header information
*/
#ifndef __RT584_SYSCTRL_H__
#define __RT584_SYSCTRL_H__

#ifdef __cplusplus
extern "C"
{
#endif
/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define RT584_SHUTTLE_IC 1 // TBD: shuttle only, remove after MPA
/*Only Secure register can control these registers*/
#if (RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)
#define MAP_BASE         ((uint32_t)SYSCTRL + 0x10)
#else
#define MAP_BASE         ((uint32_t)SYSCTRL + 0x80)
#endif


#define PULLOPT_BASE     ((uint32_t)SYSCTRL + 0x20)
#define DRV_BASE         ((uint32_t)SYSCTRL + 0x30)
#define OD_BASE          ((uint32_t)SYSCTRL + 0x38)


#define SEL_CLK_RCO1MHZ             1
#define SEL_CLK_16MHZ               2
#define SEL_CLK_32MHZ               3
#define SEL_CLK_48MHZ               4
#define SEL_CLK_64MHZ               5
#define SEL_CLK_72MHZ               6
#define SEL_CLK_36MHZ               7
#define SEL_CLK_40MHZ               8


/*Define AHB SYSTEM CLOCK mode*/
#define SYS_CLK_32MHZ               0
#define SYS_CLK_16MHZ               2
#define SYS_CLK_RCO1MHZ             3

#define SYS_PLLCLK_48MHZ            0
#define SYS_PLLCLK_64MHZ            1
#define SYS_PLLCLK_72MHZ            2
#define SYS_PLLCLK_80MHZ            3
#define SYS_PLLCLK_24MHZ            4
#define SYS_PLLCLK_32MHZ            5
#define SYS_PLLCLK_36MHZ            6
#define SYS_PLLCLK_40MHZ            7

#define SYS_PLL_CLK_OFFSET  0x10

/*Because compiler code optimize, we should set PLL_WAIT_PERIOD as 4N */
#if RT584_SHUTTLE_IC==1
#define  PLL_WAIT_PERIOD     1024
#else
#define PLL_WAIT_PERIOD      800
#endif

/**
 *  @brief Pin mux setting.
 *         RT584 NEW PINMUX QSPI0 DOES NOT SUPPORT CSN2 CSN3
 *         RT584 EXT 32K is not set in pinmux, set it in SYC_CRTL register SYS_CLK_CTRL
 *         Use Example: RT58x use MODE_GPIO
 *                      RT584 use MODE_GPIO_EX
*/
//#if (RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)

#define MODE_GPIO            0
#define MODE_QSPI0           1
#define MODE_QSPI1           2
#define MODE_IR_OUT          3
#define MODE_QSPI0_CSN1      3
#define MODE_QSPI1_CSN1      3

#define MODE_I2S0_SDI_PIN06        3
#define MODE_I2S0_SDO_PIN07        3

#define MODE_I2S0_MCLK_PIN30       3
#define MODE_I2S0_MCLK_PIN31       3

#define MODE_I2S0                  4

#define MODE_UART1                 5
#define MODE_UART2                 6

#define MODE_I2S0_MCLK_PIN04       6
#define MODE_I2S0_MCLK_PIN05       6

#define MODE_IR_MODE7              7

#define MODE_UART                  7

#define MODE_I2CM0                 8
#define MODE_I2CM1                 9

#define MODE_I2C_SLAVE             10

#define MODE_SWCLK_PIN10           10
#define MODE_SWDIO_PIN11           10

#define MODE_PWM0                  11
#define MODE_PWM1                  12
#define MODE_PWM2                  13
#define MODE_PWM3                  14
#define MODE_PWM4                  15


//#else

// #define MODE_GPIO            0          /*!< set pin for GPIO mode  */
// #define MODE_QSPI0           1          /*!< set pin for QSPI0 mode */
// #define MODE_I2C             4          /*!< set pin for I2C mode   */
// #define MODE_UART            6          /*!< set pin for UART mode  */

// #define MODE_I2S             4          /*!< set pin for I2S mode  */
// #define MODE_PWM             4          /*!< set pin for PWM mode  */
// #define MODE_PWM0            1          /*!< set pin for PWM0 mode  */
// #define MODE_PWM1            2          /*!< set pin for PWM1 mode  */
// #define MODE_PWM2            3          /*!< set pin for PWM2 mode  */
// #define MODE_PWM3            5          /*!< set pin for PWM3 mode  */
// #define MODE_PWM4            7          /*!< set pin for PWM4 mode  */
// #define MODE_QSPI1           5          /*!< set pin for QSPI1 mode  */

// #define MODE_EXT32K          5          /*!< set pin for EXT32K mode, only GPIO0~GPIO9 available for this setting  */

// /*NOTICE: The following setting only in GPIO0~GPIO3*/
// #define MODE_QSPI0_CSN1       2         /*!< set pin for QSPI0 CSN1 mode, only GPIO0~GPIO3 available for this setting  */
// #define MODE_QSPI0_CSN2       3         /*!< set pin for QSPI0 CSN2 mode, only GPIO0~GPIO3 available for this setting  */
// #define MODE_QSPI0_CSN3       6         /*!< set pin for QSPI0 CSN3 mode, only GPIO0~GPIO3 available for this setting  */


// //RT584 output setting
// #define  MODE_GPIO_EX              0x00
// #define  MODE_UART0_TX_EX          0x01
// #define  MODE_UART1_TX_EX          0x02
// #define  MODE_UART1_RTSN_EX        0x03
// #define  MODE_UART2_TX_EX          0x04
// #define  MODE_UART2_RTSN_EX        0x05
// #define  MODE_PWM0_EX              0x06
// #define  MODE_PWM1_EX              0x07
// #define  MODE_PWM2_EX              0x08
// #define  MODE_PWM3_EX              0x09
// #define  MODE_PWM4_EX              0x0A
// #define  MODE_IRM_EX               0x0B
// #define  MODE_I2CM0_SCL_OUT_EX     0x0C
// #define  MODE_I2CM0_SDA_OUT_EX     0x0D
// #define  MODE_I2CM1_SCL_OUT_EX     0x0E
// #define  MODE_I2CM1_SDA_OUT_EX     0x0F
// #define  MODE_I2CS_SCL_OUT_EX      0x10
// #define  MODE_I2CS_SDA_OUT_EX      0x11
// #define  MODE_SPI0_SCLK_OUT_EX     0x12
// #define  MODE_SPI0_SDATA_OUT0_EX   0x13
// #define  MODE_SPI0_SDATA_OUT1_EX   0x14
// #define  MODE_SPI0_SDATA_OUT2_EX   0x15
// #define  MODE_SPI0_SDATA_OUT3_EX   0x16
// #define  MODE_SPI0_CSN0_EX         0x17
// #define  MODE_SPI0_CSN1_EX         0x18
// #define  MODE_SPI0_CSN2_EX         0x19
// #define  MODE_SPI0_CSN3_EX         0x1A
// #define  MODE_SPI1_SCLK_OUT_EX     0x1B
// #define  MODE_SPI1_SDATA_OUT0_EX   0x1C
// #define  MODE_SPI1_SDATA_OUT1_EX   0x1D
// #define  MODE_SPI1_SDATA_OUT2_EX   0x1E
// #define  MODE_SPI1_SDATA_OUT3_EX   0x1F
// #define  MODE_SPI1_CSN0_EX         0x20
// #define  MODE_SPI1_CSN1_EX         0x21
// #define  MODE_SPI1_CSN2_EX         0x22
// #define  MODE_SPI1_CSN3_EX         0x23
// #define  MODE_I2S_BCK_EX           0x24
// #define  MODE_I2S_WCK_EX           0x25
// #define  MODE_I2S_SDO_EX           0x26
// #define  MODE_I2S_MCLK_EX          0x27
// #define  MODE_SWD_EX               0x2F
// #define  MODE_DBG0_EX              0x30
// #define  MODE_DBG1_EX              0x31
// #define  MODE_DBG2_EX              0x32
// #define  MODE_DBG3_EX              0x33
// #define  MODE_DBG4_EX              0x34
// #define  MODE_DBG5_EX              0x35
// #define  MODE_DBG6_EX              0x36
// #define  MODE_DBG7_EX              0x37
// #define  MODE_DBG8_EX              0x38
// #define  MODE_DBG9_EX              0x39
// #define  MODE_DBGA_EX              0x3A
// #define  MODE_DBGB_EX              0x3B
// #define  MODE_DBGC_EX              0x3C
// #define  MODE_DBGD_EX              0x3D
// #define  MODE_DBGE_EX              0x3E
// #define  MODE_DBGF_EX              0x3F

// //RT584 input setting
// //A0
// #define  MODE_UART1_RX_EX           0x00000000
// #define  MODE_UART1_CTSN_EX         0x00000008
// #define  MODE_UART2_RX_EX           0x00000010
// #define  MODE_UART2_CTSN_EX         0x00000018
// //A4
// #define  MODE_UART0_RX_EX           0x10000000
// #define  MODE_I2S_SDI_EX            0x10000008
// #define  MODE_I2CS_SCL_EX           0x10000010
// #define  MODE_I2CS_SDA_EX           0x10000018
// //A8
// #define  MODE_I2CM0_SCL_EX          0x20000000
// #define  MODE_I2CM0_SDA_EX          0x20000008
// #define  MODE_I2CM1_SCL_EX          0x20000010
// #define  MODE_I2CM1_SDA_EX          0x20000018
// //AC
// #define  MODE_QSPI0_CSN_EX          0x40000000
// #define  MODE_QSPI0_SCLK_EX         0x40000008
// #define  MODE_QSPI0_SDATA0_EX       0x40000010
// #define  MODE_QSPI0_SDATA1_EX       0x40000018
// //B0
// #define  MODE_QSPI0_SDATA2_EX       0x50000000
// #define  MODE_QSPI0_SDATA3_EX       0x50000008
// //B4
// #define  MODE_QSPI1_CSN_EX          0x60000000
// #define  MODE_QSPI1_SCLK_EX         0x60000008
// #define  MODE_QSPI1_SDATA0_EX       0x60000010
// #define  MODE_QSPI1_SDATA1_EX       0x60000018
// //B8
// #define  MODE_QSPI1_SDATA2_EX       0x70000000
// #define  MODE_QSPI1_SDATA3_EX       0x70000008

// #define  MODE_PIN_MUX_NULL          0xFFFFFFFF
// #endif



/*Driving through setting mode*/
#define PULL_NONE         0          /*!< set pin for no pull, if you set pin to GPIO output mode, system will set this option for output pin */
#define PULLDOWN_10K      1          /*!< set pin for 10K pull down  */
#define PULLDOWN_100K     2          /*!< set pin for 100K pull down  */
#define PULLDOWN_1M       3          /*!< set pin for 1M pull down  */
#define PULLUP_10K        5          /*!< set pin for 10K pull up  */
#define PULLUP_100K       6          /*!< set pin for 100K pull up, this is default pin mode */
#define PULLUP_1M         7          /*!< set pin for 1M pull up  */


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


#define IC_CHIP_ID_MASK_SHIFT                           8
#define IC_CHIP_ID_MASK                             (0xFF<<IC_CHIP_ID_MASK_SHIFT)
#define IC_RT58X                                (0x70<<IC_CHIP_ID_MASK_SHIFT)                       /*!< RT58X IC Chip ID  */
#define IC_CHIP_REVISION_MASK_SHIFT         4
#define IC_CHIP_REVISION_MASK               (0xF<<IC_CHIP_REVISION_MASK_SHIFT)
#define IC_CHIP_REVISION_MPA                (1<<IC_CHIP_REVISION_MASK_SHIFT)       /*!< RT58X IC Chip Revision ID For MPA  */
#define IC_CHIP_REVISION_MPB                (2<<IC_CHIP_REVISION_MASK_SHIFT)       /*!< RT58X IC Chip Revision ID For MPB  */


#define DPD_RETENTION_BASE          0x10

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef enum
{
    SYS_32MHZ_CLK = SYS_CLK_32MHZ,
    SYS_16MHZ_CLK = SYS_CLK_16MHZ,
    SYS_RCO1MHZ_CLK = SYS_CLK_RCO1MHZ,
    SYS_48MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_48MHZ,
    SYS_64MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_64MHZ,
    SYS_72MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_72MHZ,
    SYS_80MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_80MHZ,
    SYS_24MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_24MHZ,
    SYS_32MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_32MHZ,
    SYS_36MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_36MHZ,
    SYS_40MHZ_PLLCLK = SYS_PLL_CLK_OFFSET + SYS_PLLCLK_40MHZ,
    SYS_CLK_MAX,
} sys_clk_sel_t;

/**
 * @brief Selecting the hclk source.
 */
typedef enum
{
    HCLK_SEL_32M,
    HCLK_SEL_PLL,
    HCLK_SEL_16M,
    HCLK_SEL_RCO1M,
} hclk_clk_sel_t;

/**
 * @brief Selecting the peripheral source.
 */
typedef enum
{
    PERCLK_SEL_32M,
    PERCLK_SEL_16M = 2,
    PERCLK_SEL_RCO1M,
} perclk_clk_sel_t;

/**
 * @brief Enable peripheral interface clock.
 */
typedef enum
{
    UART0_CLK,
    UART1_CLK,
    UART2_CLK,
    QSPI0_CLK = 4,
    QSPI1_CLK,
    I2CM0_CLK = 8,
    I2CM1_CLK,
    I2CS0_CLK,
    CRYPTO_CLK = 12,
    XDMA_CLK,
    IRM_CLK,
    TIMER0_CLK = 16,
    TIMER1_CLK,
    TIMER2_CLK,
    TIMER3_32K_CLK = 20,
    TIMER4_32K_CLK,
    RTC_32K_CLK,
    GPIO_32K_CLK,
    RTC_PCLK,
    GPIO_PCLK,
    AUX_PCLK,
    BOD_PCLK,
    AUX_32K_CLK,
    BOD_32K_CLK,
    CLK_32K_DIV,
    RCO32K_DIV2
} per_interface_clk_en_t;

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
/**
 * @brief Delay_us
 *
 * @param[in] us
 *
 * @retval  none
 *
 * @details
 *
 *
 */
void delay_us(unsigned int us);
/**
 * @brief Delay_ms
 *
 * @param[in] ms
 *
 * @retval none
 *
 * @details
 *
 *
 */
void delay_ms(unsigned int ms);

/**
 * @brief get pin function mode
 *
 * @param[in] pin_number    Specifies the pin number.
 *                                                  GPIO0~GPIO30
 * @return
 *         get the pin function mode UART/I2S/PWM/SADC/I2C/SPI....
 * @details
 *         each pin has different function pin setting, please read RT58x datasheet to know each pin
 *   function usage setting.
 */
uint32_t pin_get_mode(uint32_t pin_number);
/**
 * @brief set pin function mode
 *
 * @param[in] pin_number    Specifies the pin number.
 *                                                  GPIO0~GPIO30
 * @param[in] mode          The specail function mode for the pin_number
 *                                              Config GPIO To --> UART/I2S/PWM/SADC/I2C/SPI...
 * @return
 *         NONE
 * @details
 *         each pin has different function pin setting, please read RT58x datasheet to know each pin
 *   function usage setting.
 */
void pin_set_mode(uint32_t pin_number, uint32_t mode);

/**
 * @brief set pin function mode extend for RT584
 *
 * @param[in] pin_number    Specifies the pin number.
 *                                                  GPIO0~GPIO31
 * @param[in] mode          The specail function mode for the pin_number
 *                                              Config GPIO To --> UART/I2S/PWM/SADC/I2C/SPI...
 * @return
 *         NONE
 * @details
 *         each pin has different function pin setting, please read RT584 datasheet to know each pin
 *   function usage setting.
 */
void pin_set_out_mode_ex(uint32_t pin_number, uint32_t mode);

/**
 * @brief set pin function mode extend for RT584
 *
 * @param[in] pin_number    Specifies the pin number.
 *                                                  GPIO0~GPIO31
 * @param[in] mode          The specail function mode for the pin_number
 *                                              Config GPIO To --> UART/I2S/PWM/SADC/I2C/SPI...
 * @return
 *         NONE
 * @details
 *         each pin has different function pin setting, please read RT584 datasheet to know each pin
 *   function usage setting.
 */
void pin_set_in_mode_ex(uint32_t pin_number, uint32_t mode);

/**
 * @brief enable peripherial interface clock
 *
 * @param[in] clock_id    enable the specifies peripheral "clock_id" interface clock.
 *                                              UART0_CLK
 *                                              UART1_CLK
 *                                              UART2_CLK
 *                                              I2CM_CLK
 *                                              QSPI0_CLK
 *                                              QSPI1_CLK
 *                                              TIMER1_CLK
 *                                              TIMER2_CLK
 *                                              I2S_CLK
 * @return
 *         NONE
 */
void enable_perclk(uint32_t clock);
/**
 * @brief disable peripherial interface clock
 *
 * @param[in] clock_id  disable the specifies peripheral "clock_id" interface clock.
 *                                          UART0_CLK
 *                                          UART1_CLK
 *                                          UART2_CLK
 *                                          I2CM_CLK
 *                                          QSPI0_CLK
 *                                          QSPI1_CLK
 *                                          TIMER1_CLK
 *                                          TIMER2_CLK
 *                                          I2S_CLK
 * @return
 *         NONE
 */
void disable_perclk(uint32_t clock);
/**
 * @brief Set pin pull option.
 *
 * @param[in] clock   Specifies the pin number.
 *                                      PULL_NONE        0
 *                                      PULL_DOWN_10K    1
 *                                      PULL_DOWN_100K   2
 *                                      PULL_DOWN_1M     3
 *                                      PULL_UP_10K      5
 *                                      PULL_UP_100K     6
 *                                      PULL_UP_1M       7
 * @return
 *         NONE
 * @details
 *      Pin default pull option is 100K pull up, User can use this function to change the pull up/down setting.
 *      If user set the pin  set to gpio output mode, then the pin will auto to be set as no pull option.
 *
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
void pin_set_drvopt(uint32_t pin_number, uint32_t mode);
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
void enable_pin_opendrain(uint32_t pin_number);
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
void disable_pin_opendrain(uint32_t pin_number);

/*
 * Change CPU AHB CLOCK,
 *      return STATUS_SUCCESS(0) for change success.
 *      return STATUS_ERROR      for change fail.
 *
 */
uint32_t change_ahb_system_clk(sys_clk_sel_t sys_clk_mode);

/*
 * Get CPU AHB CLOCK,
 *      return SYS_CLK_32MHZ      for CPU AHB 32MHz clock.
 *      return SYS_CLK_48MHZ      for CPU AHB 48MHz clock.
 *      return SYS_CLK_64MHZ      for CPU AHB 64MHz clock.
 *
 */
uint32_t get_ahb_system_clk(void);

/**
 * @brief enable peripherial interface clock
 *
 * @param[in] perclk_clk_sel_t    enable the specifies peripheral "clock_id" interface clock.
 *                                  PERCLK_SEL_32M
 *                                  PERCLK_SEL_16M
 *                                  PERCLK_SEL_RCO1M
 *
 * @return
 *         NONE
 */
uint32_t change_peri_clk(perclk_clk_sel_t sys_clk_mode);

/**
 * @brief enable peripherial interface clock
 *
 * @return perclk_clk_sel_t    enable the specifies peripheral "clock_id" interface clock.
 *                                  PERCLK_SEL_32M
 *                                  PERCLK_SEL_16M
 *                                  PERCLK_SEL_RCO1M
 *
 */
uint32_t get_peri_clk(void);


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
void set_slow_clock_source(uint32_t mode);

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
__STATIC_INLINE uint32_t get_chip_version(void)
{
    return ((uint32_t)(SYSCTRL->soc_chip_info.reg));
}


/** for security world
 * @brief sys_set_retention_reg. Use to save some retention value.
 *
 * @param index: The index for which scratchpad register to save
 *                It should be 0~3.
 * @param value: register value
 *
 * @details   Please notice when system power-reset (cold boot), all system retention scratchpad register (0~3)
 *
 *
 *
 */

__STATIC_INLINE void sys_set_retention_reg(uint32_t index, uint32_t value)
{
    if (index < 3)
    {
        outp32((((void *)DPD_CTRL) + DPD_RETENTION_BASE + (index << 2)), value);
    }
}

/** for security world using
 * @brief sys_get_retention_reg. Use to get retention value.
 *
 * @param[in]   index:     The index for which scratchpad register to get
 *                         It should be 0~3.
 * @param[out]  *value     the address for return value.
 *
 *
 */

__STATIC_INLINE void sys_get_retention_reg(uint32_t index, uint32_t *value)
{
    if (index < 3)
    {
        *value =  inp32((((void *)DPD_CTRL) + DPD_RETENTION_BASE + (index << 2)));
    }
    else
    {
        *value = 0;
    }
}

__STATIC_INLINE void set_deepsleep_wakeup_pin(uint32_t value)
{
    GPIO->set_ds_en |= value;
}

__STATIC_INLINE void set_deepsleep_wakeup_invert(uint32_t value)
{
    GPIO->set_ds_inv |= value;
}


__STATIC_INLINE void set_sram_shutdown_normal(uint32_t value)
{
    SYSCTRL->sram_lowpower_1.reg |= value;
}

__STATIC_INLINE void set_sram_shutdown_sleep(uint32_t value)
{
    SYSCTRL->sram_lowpower_1.reg |= (value << 16) ;
}

__STATIC_INLINE void set_sram_shutdown_deepsleep(uint32_t value)
{
    SYSCTRL->sram_lowpower_2.reg |= value;
}



/*
 * Notice: if postmasking flag set... write_otp_key
 * is no use. write_otp_key is only use when postmasking is
 * not setting.
 *
 */
__STATIC_INLINE void enable_secure_write_protect(void)
{
    SEC_CTRL->sec_otp_write_key = SEC_WRITE_OTP_MAGIC_KEY;

}

__STATIC_INLINE void disable_secure_write_protect(void)
{
    SEC_CTRL->sec_otp_write_key = 0;
}

/*@}*/ /* end of peripheral_group Sys_Ctrl */

#ifdef __cplusplus
}
#endif

#endif      /* end of __RT584_SYSCTRL_H__ */
