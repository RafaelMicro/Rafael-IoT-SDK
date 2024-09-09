/**************************************************************************//**
 * @file     bod_comp.h
 * @version
 * @brief    BOD Comparator driver header file
 *
 * @copyright
 ******************************************************************************/
/** @defgroup BOD_COMP_Driver BOD_COMP
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  BOD_Comp_Driver header information
*/
#ifndef BOD_COMP_H
#define BOD_COMP_H

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

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 * @brief Bod comparator interrupt service routine callback for user application.
 *
 * @param[in]   status  status passed to user bod routine for the reason
 *
 *
 * @details    This callback function is still running in interrupt mode, so this function
 *              should be as short as possible. It can NOT call any block function in this
 *              callback service routine.
 *
 */
typedef void (* bod_comp_proc_cb)(uint32_t status);

/**
 * @brief Debounce time definitions.
 */
typedef enum
{
    BOD_SLOW_CLOKC_2,          /*!< 2 slow clock debounce time */
    BOD_SLOW_CLOKC_4,          /*!< 4 slow clock debounce time */
    BOD_SLOW_CLOKC_8           /*!< 8 slow clock debounce time */
} bod_comp_debounce_time_t;

/**
 * @brief Counter mode edge definitions.
 */
typedef enum
{
    BOD_RISING_EDGE,          /*!< counter trigger when rising edge */
    BOD_FALLING_EDGE,         /*!< counter trigger when falling edge */
    BOD_BOTH_EDGE             /*!< counter trigger when both edge */
} bod_comp_counter_mode_edge_t;

/**
 * @brief Counter mode edge definitions.
 */
typedef enum
{
    BOD_LOW_LEVEL,          /*!< bod comparator without clock to wakeup at low level when deepsleep */
    BOD_HIGH_LEVEL          /*!< bod comparator without clock to wakeup at high level when deepsleep */
} bod_comp_wakeup_deepslepp_level_t;


/**
 * @brief bod comparator config structure.
 */
typedef struct
{
    uint8_t debounce_en             : 1;    /*!< set debounce enable */
    uint8_t debounce_sel            : 2;    /*!< debounce time select */
    uint8_t counter_mode_en         : 1;    /*!< set counter mode enable */
    uint8_t counter_mode_edge       : 2;    /*!< set the trigger polarity of counter mode */
    uint8_t counter_mode_int_en     : 1;    /*!< counter mode interrupt enable */
    uint8_t rising_edge_int_en      : 1;    /*!< set rising edge interrupt enable */
    uint8_t falling_edge_int_en     : 1;    /*!< set falling edge interrupt enable */
    uint16_t counter_mode_threshold;        /*!< set the trigger threshold of the counter mode. When COUNTER_CNT > counter_mode_threshold , Interrupt will be triggered. */
} bod_comp_config_t;


/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
/**
 * @brief Register user interrupt ISR callback function.
 *
 * @param[in]    bod_comp_callback  Specifies user callback function when the bod comparator interrupt generated.
 *
 * @retval     none
 */
void bod_comp_register_callback(bod_comp_proc_cb bod_comp_callback);

/**
 * @brief     Init Bod comparator Aanlog setting.
 *
 * @retval     none
 */
void bod_comp_ana_init(void);
    
/**
 * @brief Setting Bod comparator configuration.
 *
 * @param[in]    bod_cfg            Bod comparator configuration.
 * @param[in]    bod_comp_callback  Specifies user callback function when the bod comparator interrupt generated.
 *
 * @retval     none
 */
void bod_comp_open(bod_comp_config_t bod_cfg, bod_comp_proc_cb bod_comp_callback);

/**
 * @brief     Enable Bod comparator .
 *
 * @retval     none
 */
void bod_comp_normal_start(void);

/**
 * @brief     Disable Bod comparator .
 *
 * @retval     none
 */
void bod_comp_normal_stop(void);

/**
 * @brief    Enable Bod comparator in sleep mode.
 *
 * @retval     none
 */
void bod_comp_sleep_start(void);

/**
 * @brief    Disable Bod comparator in sleep mode.
 *
 * @retval     none
 */
void bod_comp_sleep_stop(void);

/**
 * @brief    Enable Bod comparator in deep sleep mode.
 *
 * @retval     none
 */
void bod_comp_deep_sleep_start(void);

/**
 * @brief    Disable Bod comparator in deep sleep mode.
 *
 * @retval     none
 */
void bod_comp_deep_sleep_stop(void);

/**
 * @brief    Enable clock source when Bod comparator in deep sleep mode.
 *
 * @retval     none
 */
void bod_comp_setup_deep_sleep_enable_clock(void);

/**
 * @brief    Disable clock source when Bod comparator in deep sleep mode.
 *
 * @param[in]    wakeup_level   Set wakeup polarity low or high in deepsleep without clock source.
 *
 * @retval     none
 */
void bod_comp_setup_deep_sleep_disable_clock(uint8_t wakeup_level);

/**
 * @brief    Get Bod comparator count.
 *
 * @retval     Counter count.
 */
uint32_t get_bod_comp_counter_count(void);

/**
 * @brief    Clear Bod comparator count.
 *
 * @retval     none
 */
void clear_bod_comp_counter_count(void);

/*@}*/ /* end of peripheral_group BOD_COMP_Driver */

#ifdef __cplusplus
}
#endif

#endif      /* end of __RT584_BOD_COMP_H__ */


