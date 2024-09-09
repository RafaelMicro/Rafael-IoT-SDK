/***********************************************************************************************************************
 * @file     lpm.h
 *
 * @brief    Low Power Mode driver header file
 *
 *
 ******************************************************************************/
/**
* @defgroup lpm_group LPM
* @ingroup peripheral_group
* @{
* @brief  Define LPM definitions, structures, and functions
*/
#ifndef _RT584_LPM_H__
#define _RT584_LPM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/


/***********************************************************************************************************************
 *    CONSTANTS AND DEFINES
 **********************************************************************************************************************/
#define LOW_POWER_NO_MASK                 0x00000000
#define LOW_POWER_MASK_BIT_TASK_HCI       0x00000001             /**< bit0 */
#define LOW_POWER_MASK_BIT_TASK_HOST      0x00000002             /**< bit1 */
#define LOW_POWER_MASK_BIT_TASK_BLE_APP   0x00000004             /**< bit2 */
#define LOW_POWER_MASK_BIT_TASK_ADC       0x00000008             /**< bit3 */
#define LOW_POWER_MASK_BIT_RESERVED4      0x00000010             /**< bit4 */
#define LOW_POWER_MASK_BIT_RESERVED5      0x00000020             /**< bit5 */
#define LOW_POWER_MASK_BIT_RESERVED6      0x00000040             /**< bit6 */
#define LOW_POWER_MASK_BIT_RESERVED7      0x00000080             /**< bit7 */
#define LOW_POWER_MASK_BIT_RESERVED8      0x00000100             /**< bit8 */
#define LOW_POWER_MASK_BIT_RESERVED9      0x00000200             /**< bit9 */
#define LOW_POWER_MASK_BIT_RESERVED10     0x00000400             /**< bit10 */
#define LOW_POWER_MASK_BIT_RESERVED11     0x00000800             /**< bit11 */
#define LOW_POWER_MASK_BIT_RESERVED12     0x00001000             /**< bit12 */
#define LOW_POWER_MASK_BIT_RESERVED13     0x00002000             /**< bit13 */
#define LOW_POWER_MASK_BIT_RESERVED14     0x00004000             /**< bit14 */
#define LOW_POWER_MASK_BIT_RESERVED15     0x00008000             /**< bit15 */
#define LOW_POWER_MASK_BIT_RESERVED16     0x00010000             /**< bit16 */
#define LOW_POWER_MASK_BIT_RESERVED17     0x00020000             /**< bit17 */
#define LOW_POWER_MASK_BIT_RESERVED18     0x00040000             /**< bit18 */
#define LOW_POWER_MASK_BIT_RESERVED19     0x00080000             /**< bit19 */
#define LOW_POWER_MASK_BIT_RESERVED20     0x00100000             /**< bit20 */
#define LOW_POWER_MASK_BIT_RESERVED21     0x00200000             /**< bit21 */
#define LOW_POWER_MASK_BIT_RESERVED22     0x00400000             /**< bit22 */
#define LOW_POWER_MASK_BIT_RESERVED23     0x00800000             /**< bit23 */
#define LOW_POWER_MASK_BIT_RESERVED24     0x01000000             /**< bit24 */
#define LOW_POWER_MASK_BIT_RESERVED25     0x02000000             /**< bit25 */
#define LOW_POWER_MASK_BIT_RESERVED26     0x04000000             /**< bit26 */
#define LOW_POWER_MASK_BIT_RESERVED27     0x08000000             /**< bit27 */
#define LOW_POWER_MASK_BIT_RESERVED28     0x10000000             /**< bit28 */
#define LOW_POWER_MASK_BIT_RESERVED29     0x20000000             /**< bit29 */
#define LOW_POWER_MASK_BIT_RESERVED30     0x40000000             /**< bit30 */
#define LOW_POWER_MASK_BIT_RESERVED31     0x80000000             /**< bit31 */

#define COMM_SUBSYS_WAKEUP_NO_MASK                 0x00000000
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED0      0x00000001       /**< bit0 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED1      0x00000002       /**< bit1 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED2      0x00000004       /**< bit2 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED3      0x00000008       /**< bit3 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED4      0x00000010       /**< bit4 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED5      0x00000020       /**< bit5 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED6      0x00000040       /**< bit6 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED7      0x00000080       /**< bit7 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED8      0x00000100       /**< bit8 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED9      0x00000200       /**< bit9 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED10     0x00000400       /**< bit10 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED11     0x00000800       /**< bit11 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED12     0x00001000       /**< bit12 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED13     0x00002000       /**< bit13 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED14     0x00004000       /**< bit14 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED15     0x00008000       /**< bit15 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED16     0x00010000       /**< bit16 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED17     0x00020000       /**< bit17 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED18     0x00040000       /**< bit18 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED19     0x00080000       /**< bit19 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED20     0x00100000       /**< bit20 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED21     0x00200000       /**< bit21 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED22     0x00400000       /**< bit22 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED23     0x00800000       /**< bit23 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED24     0x01000000       /**< bit24 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED25     0x02000000       /**< bit25 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED26     0x04000000       /**< bit26 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED27     0x08000000       /**< bit27 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED28     0x10000000       /**< bit28 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED29     0x20000000       /**< bit29 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED30     0x40000000       /**< bit30 */
#define COMM_SUBSYS_WAKEUP_MASK_BIT_RESERVED31     0x80000000       /**< bit31 */

#define COMMUMICATION_SUBSYSTEM_WAKEUP             (1 << 8)
#define COMMUMICATION_SUBSYSTEM_SLEEP              (1 << 9)
#define COMMUMICATION_SUBSYSTEM_DEEPSLEEP          (1 << 10)
#define COMMUMICATION_SUBSYSTEM_RESET              (1 << 11)
#define COMMUMICATION_SUBSYSTEM_HOSTMODE           (1 << 24)
#define COMMUMICATION_SUBSYSTEM_DIS_HOSTMODE       (1 << 25)

#define SUBSYSTEM_CFG_WAIT_32K_DONE                 0x418
#define SUBSYSTEM_CFG_WAIT_32K_DONE_DISABLE         0x04

#define SUBSYSTEM_CFG_PMU_MODE                      0x4B0
#define SUBSYSTEM_CFG_LDO_MODE_DISABLE              0x02

/***********************************************************************************************************************
 *    TYPEDEFS
 **********************************************************************************************************************/
/**@brief DMA config structure for DMA setting
**
*/
typedef enum
{
    LOW_POWER_PLATFORM_NORMAL                       = 0x00, /**< Platform system normal mode                                */
    LOW_POWER_PLATFORM_ENTER_SLEEP                  = 0x01, /**< Platform system enter sleep mode            */
    LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP             = 0x02, /**< Platform system enter deep sleep mode       */
    LOW_POWER_PLATFORM_ENTER_POWER_DOWN_MODE        = 0x04, /**< Platform system enter deep power done mode  */
} low_power_platform_enter_mode_cfg_t;

/**@brief DMA config structure for DMA setting
 **
 */
typedef enum
{
    LOW_POWER_LEVEL_NORMAL = 0,             /**< platform system: run, communication system: run */
    LOW_POWER_LEVEL_SLEEP0,                 /**< platform system: sleep, communication system: run */
    LOW_POWER_LEVEL_SLEEP1,                 /**< platform system: sleep, communication system: sleep */
    LOW_POWER_LEVEL_SLEEP2,                 /**< platform system: sleep, communication system: deep sleep */
    LOW_POWER_LEVEL_SLEEP3,                             /**< platform system: deep sleep, communication system: run */
} low_power_level_cfg_t;

#define ENTER_SLEEP             LOW_POWER_LEVEL_SLEEP0
#define ENTER_DEEP_SLEEP        LOW_POWER_LEVEL_SLEEP2
#define ENTER_POWER_DOWN        LOW_POWER_LEVEL_SLEEP3
/**@brief DMA config structure for DMA setting
**
*/
typedef enum
{
    COMMUMICATION_SUBSYSTEM_PWR_STATE_TRANSITION    = 0x00, /**< sub system transition mode             */
    COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP    = 0x01, /**< sub system enter sleep mode            */
    COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP         = 0x02, /**< sub system enter deep sleep mode       */
    COMMUMICATION_SUBSYSTEM_PWR_STATE_NORMAL        = 0x03, /**< sub system normal mode                 */
} commumication_subsystem_pwr_mode_cfg_t;

/**@brief DMA config structure for DMA setting
 **
 */
typedef enum
{
    LOW_POWER_WAKEUP_NULL       = (0),                 /**< Low power mode wake-up source: Null */

    LOW_POWER_WAKEUP_RTC_TIMER  = ((1 << 0) & 0xFFFF), /**< Low power mode wake-up source: RTC Timer */
    LOW_POWER_WAKEUP_GPIO       = ((1 << 1) & 0xFFFF), /**< Low power mode wake-up source: GPIO */
    LOW_POWER_WAKEUP_COMPARATOR = ((1 << 2) & 0xFFFF), /**< Low power mode wake-up source: Analog Comparator */
    LOW_POWER_WAKEUP_32K_TIMER  = ((1 << 3) & 0xFFFF), /**< Low power mode wake-up source: 32KHz Timers */
    LOW_POWER_WAKEUP_UART_RX    = ((1 << 4) & 0xFFFF), /**< Low power mode wake-up source: UART Rx Break Signal */
    LOW_POWER_WAKEUP_BOD_CMP    = ((1 << 5) & 0xFFFF), /**< Low power mode wake-up source: Bod Comparator  */

    LOW_POWER_WAKEUP_GPIO0      = ((0 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO0 */
    LOW_POWER_WAKEUP_GPIO1      = ((1 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO1 */
    LOW_POWER_WAKEUP_GPIO2      = ((2 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO2 */
    LOW_POWER_WAKEUP_GPIO3      = ((3 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO3 */
    LOW_POWER_WAKEUP_GPIO4      = ((4 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO4 */
    LOW_POWER_WAKEUP_GPIO5      = ((5 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO5 */
    LOW_POWER_WAKEUP_GPIO6      = ((6 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO6 */
    LOW_POWER_WAKEUP_GPIO7      = ((7 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO7 */
    LOW_POWER_WAKEUP_GPIO8      = ((8 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO8 */
    LOW_POWER_WAKEUP_GPIO9      = ((9 << 16)  | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO9 */
    LOW_POWER_WAKEUP_GPIO10     = ((10 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO10 */
    LOW_POWER_WAKEUP_GPIO11     = ((11 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO11 */
    LOW_POWER_WAKEUP_GPIO12     = ((12 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO12 */
    LOW_POWER_WAKEUP_GPIO13     = ((13 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO13 */
    LOW_POWER_WAKEUP_GPIO14     = ((14 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO14 */
    LOW_POWER_WAKEUP_GPIO15     = ((15 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO15 */
    LOW_POWER_WAKEUP_GPIO16     = ((16 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO16 */
    LOW_POWER_WAKEUP_GPIO17     = ((17 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO17 */
    LOW_POWER_WAKEUP_GPIO18     = ((18 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO18 */
    LOW_POWER_WAKEUP_GPIO19     = ((19 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO19 */
    LOW_POWER_WAKEUP_GPIO20     = ((20 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO20 */
    LOW_POWER_WAKEUP_GPIO21     = ((21 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO21 */
    LOW_POWER_WAKEUP_GPIO22     = ((22 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO22 */
    LOW_POWER_WAKEUP_GPIO23     = ((23 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO23 */
    LOW_POWER_WAKEUP_GPIO24     = ((24 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO24 */
    LOW_POWER_WAKEUP_GPIO25     = ((25 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO25 */
    LOW_POWER_WAKEUP_GPIO26     = ((26 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO26 */
    LOW_POWER_WAKEUP_GPIO27     = ((27 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO27 */
    LOW_POWER_WAKEUP_GPIO28     = ((28 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO28 */
    LOW_POWER_WAKEUP_GPIO29     = ((29 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO29 */
    LOW_POWER_WAKEUP_GPIO30     = ((30 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO30 */
    LOW_POWER_WAKEUP_GPIO31     = ((31 << 16) | LOW_POWER_WAKEUP_GPIO), /**< Low power mode wake-up source: GPIO31 */

    LOW_POWER_WAKEUP_UART0_RX   = ((0 << 16)  | LOW_POWER_WAKEUP_UART_RX), /**< Low power mode wake-up source: UART0 Rx Break Signal */
    LOW_POWER_WAKEUP_UART1_RX   = ((1 << 16)  | LOW_POWER_WAKEUP_UART_RX), /**< Low power mode wake-up source: UART1 Rx Break Signal */
    LOW_POWER_WAKEUP_UART2_RX   = ((2 << 16)  | LOW_POWER_WAKEUP_UART_RX), /**< Low power mode wake-up source: UART2 Rx Break Signal */
} low_power_wakeup_cfg_t;




/***********************************************************************************************************************
 *    GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
 * @brief low poer mode mask
 * @details
 * @param[in] mask, config CM33 wakeup vaule
 *
 */
void Lpm_Low_Power_Mask(uint32_t mask);
/**
 * @brief ow poer mode unmask
 * @details
 * @param[in] unmask, config CM33 wakeup vaule
 *
 */
void Lpm_Low_Power_Unmask(uint32_t unmask);
/**
 * @brief get lower power mode mask status
 * @details
 * @param    NONE
 *
 */
uint32_t Lpm_Get_Low_Power_Mask_Status(void);
/**
 * @brief Config communication subsystem wakeup mask
 * @details
 * @param[in] mask, config subsystem wakeup mask vaule
 *
 */
void Lpm_Comm_Subsystem_Wakeup_Mask(uint32_t mask);
/**
 * @brief Config communication subsystem wakeup unmask
 * @details
 * @param[in] unmask, config subsystem wakeup unmask vaule
 *
 */
void Lpm_Comm_Subsystem_Wakeup_Unmask(uint32_t unmask);
/**
 * @brief Get communication subsystem wakeup mask status
 * @details
 * @param    NONE
 *
 */
uint32_t Lpm_Get_Comm_Subsystem_Wakeup_Mask_Status(void);
/**
 * @brief Set System Low Power Level
 * @details
 * @param[in] low_power_level_cfg,  config low poer level
 *
 */
void Lpm_Set_Low_Power_Level(low_power_level_cfg_t low_power_level_cfg);
/**
 * @brief Enable Low Power Mode Wake up source
 * @details
 * @param[in] low_power_wakeup_cfg low power wakeup source
 *
 */
void Lpm_Enable_Low_Power_Wakeup(low_power_wakeup_cfg_t low_power_wakeup_cfg);
/**
 * @brief Disable Low Power Mode Wake up source
 * @details
 * @param[in] low_power_wakeup_cfg low power wakeup source
 *
 */
void Lpm_Disable_Low_Power_Wakeup(low_power_wakeup_cfg_t low_power_wakeup_cfg);
/**
 * @brief Config SRAM power off in Normal mode
 * @details
 * @param[in] value, disable sram block reference register table
 *
 */
void Lpm_Set_Sram_Normal_Shutdown(uint32_t value);
/**
 * @brief Config SRAM power off in sleep/deepsleep mode
 * @details
 * @param[in] value, disable sram block reference register table
 *
 */
void Lpm_Set_Sram_Sleep_Deepsleep_Shutdown(uint32_t value);

void Lpm_Set_GPIO_Deepsleep_Wakeup_Invert_Ex(uint8_t num, uint32_t value);
void Lpm_Set_GPIO_Powerdown_Wakeup_Invert_Ex(uint8_t num, uint32_t value);

/**
 * @brief System Enter Low Power Mode
 * @details
 * @param    NONE
 *
 */
void Lpm_Enter_Low_Power_Mode(void);
/**
 * @brief disable communication subsystem system LDO Mode
 * @details
 * @param    NONE
 *
 */
void Lpm_Comm_Subsystem_Disable_LDO_Mode(void);
/**
 * @brief check communication subsystem system ready flag
 * @details
 * @param    NONE
 *
 */
void Lpm_Comm_Subsystem_Check_System_Ready(void);
/**
 * @brief Disable communication subsystem 32k done
 * @details
 * @param    NONE
 *
 */
void Lpm_Comm_Subsystem_Disable_Wait_32k_Done(void);
/**
 * @brief Set rt584 enter sleep mode
 * @details
 * @param    NONE
 *
 */
void Rt584_Sleep_Mode(void);
/**
 * @brief Set rt584 enter deep sleep mode
 * @details
 * @param    NONE
 *
 */
void Rt584_Deep_Sleep_Mode(void);
/**
 * @brief Set rt584 enter deep powerdown mode
 * @details
 * @param    NONE
 *
 */
void Rt584_Deep_Powerdown_Mode(void);
/**
 * @brief Lpm_Comm_Subsystem_Power_Status_Check
 * @details
 * @param    NONE
 *
 */

void Lpm_Comm_Subsystem_Power_Status_Check(commumication_subsystem_pwr_mode_cfg_t mode);
void Lpm_Sub_System_Low_Power_Mode(commumication_subsystem_pwr_mode_cfg_t mode);
/*@}*/ /* end of peripheral_group LPM */
#ifdef __cplusplus
}
#endif

#endif /* end of _RT584_LPM_H_ */

