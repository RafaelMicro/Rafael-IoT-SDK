/** @file ble_privacy.h
 *
 *  * @brief BLE Privacy header file.
 *
 */

/**
 * @defgroup ble_privacy BLE Privacy
 * @ingroup BLE_group
 * @{
 * @brief Define BLE Privacy definitions, structures, and functions.
 * @}
 */

#ifndef __BLE_PRIVACY_H__
#define __BLE_PRIVACY_H__


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
/**
 * @defgroup ble_privacy_param BLE Privacy Parameter Definition
 * @{
 * @ingroup ble_privacy
 * @brief Define BLE privacy parameter definition.
 */
typedef uint8_t ble_privacy_param_t;

/** Enable or Disable */
#define BLE_PRIVACY_PARAM_FLD_ENABLE            0x01
#define BLE_PRIVACY_PARAM_DISABLE               0x00
#define BLE_PRIVACY_PARAM_ENABLE                0x01

/** 0: Network Privacy Mode, 1: Device Privacy Mode */
#define BLE_PRIVACY_PARAM_FLD_MODE              0x02
#define BLE_PRIVACY_PARAM_MODE_NETWORK          0x00
#define BLE_PRIVACY_PARAM_MODE_DEVICE           0x02

/** 0: LL_Privacy is not support, 1: LL_Privacy is support */
#define BLE_PRIVACY_PARAM_FLD_LL_PRIVACY        0x04
#define BLE_PRIVACY_PARAM_LL_PRIVACY_OFF        0x00
#define BLE_PRIVACY_PARAM_LL_PRIVACY_ON         0x04

/** Reserved bits */
#define BLE_PRIVACY_PARAM_FLD_RESERVED          0xF8
/** @} */

#define BLE_ADDR_SUB_TYPE_FLD_RANDOM            0xC0
#define BLE_ADDR_SUB_TYPE_RANDOM                0x00
#define BLE_ADDR_SUB_TYPE_RANDOM_RESOLVABLE     0x40
#define BLE_ADDR_SUB_TYPE_RANDOM_RESERVED       0x80
#define BLE_ADDR_SUB_TYPE_RANDOM_STATIC         0xC0


/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/** @defgroup ble_privacy_mode BLE Privacy mode Definition
 * @{
 * @ingroup ble_privacy
 * @details  Define different BLE Privacy mode.
 */
typedef uint8_t ble_privacy_mode_t;
#define NETWORK_PRIVACY_MODE                    0x00  /**< Use network privacy mode.  */
#define DEVICE_PRIVACY_MODE                     0x01  /**< Use device privacy mode.  */
/** @} */



/** @brief Set BLE Privacy mode parameter.
 * @ingroup ble_privacy
*/
typedef struct ble_set_privacy_cfg_s
{
    uint8_t                 host_id;            /**< Host id.*/
    ble_privacy_mode_t      privacy_mode;       /**< Privacy mode selection. */
} ble_set_privacy_cfg_t;



/** @brief Set BLE Privacy mode parameter.
 * @ingroup ble_privacy
*/
typedef struct ble_irk_rpa_addr_s
{
    uint8_t rpa_addr[6];                        /**< Resolvable private advertising address */
    uint8_t irk[16];                            /**< identify random key */
} ble_irk_rpa_addr_t;

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 * EXTERN DEFINITIONS
 *************************************************************************************************/

/** @brief  Pointer to the parameter of ble resolvable random address and identify key.
 * @ingroup ble_privacy
 */
extern ble_irk_rpa_addr_t *param_rpa;

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
/** @brief check if the privacy mode set to "enable".
 *
 * @ingroup ble_privacy
 *
 * @note TRUE: enable, FALSE: disable
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
bool ble_privacy_parameter_enable_check(void);


/** @brief check if the privacy mode set to "Device Privacy Mode".
 *
 * @ingroup ble_privacy
 *
 * @note TRUE: Device Privacy Mode, FALSE: Network Privacy Mode
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
bool ble_privacy_parameter_device_mode_check(void);


/** @brief check if the privacy mode set to run "LL_privacy".
 *
 * @ingroup ble_privacy
 *
 * @note TRUE: LL_privacy enable, FALSE: LL_privacy disable
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
bool ble_privacy_parameter_LL_privacy_on_check(void);


/** @brief Get the host id when privacy enable
 *
 * @ingroup ble_privacy
 *
 * @return host_id : the link id.
 */
uint8_t ble_privacy_host_id_get(void);


/** @brief BLE privacy enable function.
 *
 * @ingroup ble_privacy
 *
 * @param[in] p_param : a pointer to the privacy configuration value.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_cmd_privacy_enable(ble_set_privacy_cfg_t *p_param);

/** @brief BLE privacy disable function.
 *
 * @ingroup ble_privacy
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_cmd_privacy_disable(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLE_PRIVACY_H__ */
