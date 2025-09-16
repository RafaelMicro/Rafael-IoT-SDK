/** @file ble_cmd_privacy.c
 *
 * @brief Define BLE Privacy command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_advertising.h"
#include "ble_common.h"
#include "ble_host_cmd.h"
#include "ble_privacy.h"
#include "ble_scan.h"
#include "log.h"

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static ble_privacy_param_t g_privacy_param = BLE_PRIVACY_PARAM_DISABLE;
static uint8_t g_privacy_host_id = BLE_HOSTID_RESERVED;

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/**
 * TRUE: enable
 * FALSE: disable
 */
bool ble_privacy_parameter_enable_check(void)
{
    if (g_privacy_param & BLE_PRIVACY_PARAM_FLD_ENABLE)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * TRUE: Device Privacy Mode
 * FALSE: Network Privacy Mode
 */
bool ble_privacy_parameter_device_mode_check(void)
{
    if (g_privacy_param & BLE_PRIVACY_PARAM_FLD_MODE)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * TRUE: LL_privacy enable
 * FALSE: LL_privacy disable
 */
bool ble_privacy_parameter_LL_privacy_on_check(void)
{
    if (g_privacy_param & BLE_PRIVACY_PARAM_FLD_LL_PRIVACY)
    {
        return TRUE;
    }

    return FALSE;
}


/**
 *
 */
uint8_t ble_privacy_host_id_get(void)
{
    return g_privacy_host_id;
}


/** Set Privacy enable & privacy mode
 *
*/
ble_err_t ble_cmd_privacy_enable(ble_set_privacy_cfg_t *p_param)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        // check status
        if ((ble_scan_idle_state_check() == FALSE) || (ble_adv_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }
        if (bhc_irk_key_exist_check(p_param->host_id) == BLE_ERR_INVALID_PARAMETER)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        g_privacy_param |= BLE_PRIVACY_PARAM_ENABLE;
        if (p_param->privacy_mode == NETWORK_PRIVACY_MODE)
        {
            g_privacy_param &= (~BLE_PRIVACY_PARAM_FLD_MODE);
        }
        else
        {
            g_privacy_param |= BLE_PRIVACY_PARAM_MODE_DEVICE;
        }
        g_privacy_host_id = p_param->host_id;
    } while(0);

    return status;
}

/** Set Privacy disable
 *
*/
ble_err_t ble_cmd_privacy_disable(void)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        // check status
        if ((ble_scan_idle_state_check() == FALSE) || (ble_adv_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }
        g_privacy_param &= (~BLE_PRIVACY_PARAM_FLD_ENABLE);
        g_privacy_host_id = BLE_HOSTID_RESERVED;
    } while(0);

    return status;
}
