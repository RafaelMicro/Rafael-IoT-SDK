/******************************************************************************
 * @file     system_cm33.h
 * @version
 * @brief    system initialization header file
 *
 * @copyright
 ******************************************************************************/
/** @defgroup    Sytem_RT584_cm33
 *  @ingroup     peripheral_group
 *  @breif
 *  @{
 *  @details   System Initialization header file for RT584-CM3 device based on CMSIS-CORE
*/
#ifndef SYSTEM_CM33_H
#define SYSTEM_CM33_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "mcu.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
/**
 *  @Brief Exception / Interrupt Handler Function Prototype
 */
#define PMU_LDO_MODE     0
#define PMU_DCDC_MODE    1

#ifndef SET_PMU_MODE
#define SET_PMU_MODE    PMU_DCDC_MODE
#endif
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 *  @Brief Exception / Interrupt Handler Function Prototype
 */
typedef void(*VECTOR_TABLE_Type)(void);

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
typedef enum
{
    PMU_MODE_LDO = 0,               /*!< System PMU LDO Mode */
    PMU_MODE_DCDC,                  /*!< System PMU DCDC Mode */
} pmu_mode_cfg_t;
/**
 *  @Brief Processor Clock Frequency
 */
extern uint32_t SystemCoreClock;
extern uint32_t SystemFrequency;  /* System Core Clock Frequency */
/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
void systeminit(void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
void systemcoreclockupdate(void);

/*@}*/ /* end of peripheral_group Sytem_RT584_cm33_mcu Driver */

#ifdef __cplusplus
}
#endif

#endif /* end of _SYSTEM_RT584_CM33_MCU_H_ */
