/***********************************************************************************************************************
 * @file     swi.h
 * @version
 * @brief    rt584 software driver API
 *
 * @copyright
 **********************************************************************************************************************/
/**
* @defgroup swi_group SWI
* @ingroup peripheral_group
* @{
* @brief  Define swi definitions, structures, and functions
*/
#ifndef SWI_H
#define SWI_H

#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************************************************************
 *    TYPEDEFS
 **********************************************************************************************************************/
#include "mcu.h"
/***********************************************************************************************************************
 *    TYPEDEFS
 **********************************************************************************************************************/
typedef enum
{
    SWI0_ID = 0,         /*!< swi0  idefinitions   */
    SWI1_ID = 1,         /*!< swi   idefinitions   */
    SWI_ID_MAX
} swi_id_t;
/**
 * @brief swi interrupt callback for user application.
 *
 * @param id   id.  0 for SWI0, 1 for SWI1
 *
 * @param data  SWI data value.
 *
 *
 */
typedef void (*swi_cb_t)(uint32_t id, uint32_t data);
/***********************************************************************************************************************
 *    GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
* @brief SWI register interrupt callback function
* @param[in] id SWI idefinitions
* @param[in] soft_cb_fun  swi callbac function
* @return None
*/
uint32_t register_soft_intr(uint32_t id, swi_cb_t soft_cb_fun);
/**
 * @brief Trigger Software interrupt
 * @param[in] id SWI idefinitions
 * @return None
 */
uint32_t enable_soft_intr(uint32_t id, uint32_t bit);
/**
 * @brief Clear Software interrupt
 * @param[in] id SWI idefinitions
 * @return None
 */
uint32_t clear_soft_intr(uint32_t id, uint32_t bit);
/**
 * @brief get Software interrupt data
 * @param[in] id SWI idefinitions
 * @param[in] *data point
 * @return None
 */
uint32_t get_soft_intr_data(uint32_t id, uint32_t *data);
/**
 * @brief set Software interrupt data
 * @param[in] id SWI idefinitions
 * @param[in] data  data value
 * @return None
 */
uint32_t set_soft_intr_data(uint32_t id, uint32_t data);

/** @} */ /* End of Peripheral Group */

#ifdef __cplusplus
}
#endif

#endif /* End of _SWI_H_ */


