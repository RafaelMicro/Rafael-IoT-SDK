/** @file ble_cmd_security_manager.c
 *
 * @brief Define BLE security manager command definition, structure and functions.
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
#include "ble_event.h"
#include "ble_gap.h"
#include "ble_host_cmd.h"
#include "ble_security_manager.h"
#include "ble_scan.h"
#include "log.h"

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** BLE send security request.
 *
 */
ble_err_t ble_cmd_security_request_set(uint8_t host_id)
{
    ble_err_t status;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        // check host id is valid or not
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection is exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        status = bhc_sm_security_request(host_id);
        if (status != BLE_ERR_OK)
        {
            log_info("<SM_SECURITY_REQ_SET> status:%d\n", status);
        }
    } while(0);

    return status;
}

/** Set BLE Pairing PassKey Value
 */
ble_err_t ble_cmd_passkey_set(ble_sm_passkey_param_t *p_param)
{
    ble_err_t status;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check host id is valid or not
        if (bhc_host_id_is_valid_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection is exist or not
        if (bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (bhc_sm_passkey_set(p_param->passkey, conn_id) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<SM_PASSKEY_SET> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** Set BLE Numeric Comparison Value Result
 */
ble_err_t ble_cmd_numeric_comp_result_set(ble_sm_numeric_comp_result_param_t *p_param)
{
    ble_err_t status;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check host id is valid or not
        if (bhc_host_id_is_valid_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection is exist or not
        if (bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (bhc_sm_numeric_comparison_set(p_param->same_numeric, conn_id) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<SM_NUMERIC_COMP_RESULT_SET> BLE BUSY.\n");
        }
    } while (0);

    return status;
}

/** Set BLE IO Capabilities
 */
ble_err_t ble_cmd_io_capability_set(ble_sm_io_cap_param_t *p_param)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check BLE state, initial state and check connection number
        if ((ble_adv_idle_state_check() != TRUE)    ||
                (bhc_host_connected_link_number_get()  != 0))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }


        if ( (ble_scan_idle_state_check() != TRUE)   ||
                (ble_init_idle_state_check() != TRUE)   ||
                (bhc_host_connected_link_number_get()  != 0))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }   

        if ((p_param->io_caps_param != DISPLAY_ONLY) &&
                (p_param->io_caps_param != DISPLAY_YESNO) &&
                (p_param->io_caps_param != KEYBOARD_ONLY) &&
                (p_param->io_caps_param != NOINPUT_NOOUTPUT) &&
                (p_param->io_caps_param != KEYBOARD_DISPLAY))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        bhc_sm_io_caps_set(p_param);
    } while(0);

    return status;
}

/** Set BLE Bonding Flags
 */
ble_err_t ble_cmd_bonding_flag_set(ble_sm_bonding_flag_param_t *p_param)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check BLE state, initial state and check connection number
        if ((ble_adv_idle_state_check() != TRUE)    ||
                (bhc_host_connected_link_number_get()  != 0))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if ( (ble_scan_idle_state_check() != TRUE)   ||
                (ble_init_idle_state_check() != TRUE)   ||
                (bhc_host_connected_link_number_get()  != 0))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if ((p_param->bonding_flag != NO_BONDING) && (p_param->bonding_flag != BONDING))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        bhc_sm_bonding_flag_set(p_param);
    } while(0);

    return status;
}

/** BLE retoste cccd command
 */
ble_err_t ble_cmd_cccd_restore(uint8_t host_id)
{
    ble_err_t status;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        // check host id is valid or not
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection is exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        status = bhc_sm_restore_cccd_from_bond(host_id);
    } while(0);

    return status;
}

/** BLE bonding space init
 */
ble_err_t ble_cmd_bonding_space_init(void)
{
    return bhc_sm_bonding_space_init();
}

/** BLE write identity resolving key command
 */
ble_err_t ble_cmd_write_identity_resolving_key(ble_sm_irk_param_t *p_param)
{
    ble_err_t status;
    
    status = BLE_ERR_OK;
    do {
        if ((ble_adv_idle_state_check() != TRUE)    ||
                (bhc_host_connected_link_number_get()  != 0))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if ( (ble_scan_idle_state_check() != TRUE)   ||
                (ble_init_idle_state_check() != TRUE)   ||
                (bhc_host_connected_link_number_get()  != 0))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }
        status = bhc_sm_identity_resolving_key_set(p_param);
    } while(0);

    return status;
}

/** BLE LESC init
 */
ble_err_t ble_cmd_lesc_init(void)
{
    return ble_lesc_init();
}


/** BLE security module event handler.
 *
 */
ble_err_t ble_evt_sm_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_SM_EVT_STK_GENERATION_METHOD:
    {
        ble_evt_sm_stk_gen_method_t *p_gen_method = (ble_evt_sm_stk_gen_method_t *)&p_evt_param->event_param.ble_evt_sm.param.evt_stk_gen_method;

        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[SM] STK GENERATION METHOD = 0x%02x\n", p_gen_method->key_gen_method);
        }
    }
    break;

    case BLE_SM_EVT_PASSKEY_CONFIRM:
    {
        ble_evt_sm_passkey_confirm_param_t *p_passkey = (ble_evt_sm_passkey_confirm_param_t *)&p_evt_param->event_param.ble_evt_sm.param.evt_passkey_confirm_param;

        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[SM] PASSKEY CONFIRMED host id = %d\n", p_passkey->host_id);
        }
    }
    break;

    case BLE_SM_EVT_AUTH_STATUS:
    {
        ble_evt_sm_auth_status_t *p_auth = (ble_evt_sm_auth_status_t *)&p_evt_param->event_param.ble_evt_sm.param.evt_auth_status;

        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[SM] AUTH CONFIRM status = %d\n", p_auth->status);
        }
    }
    break;

    case BLE_SM_EVT_IRK_RESOLVING_FAIL:
    {
        ble_evt_sm_irk_resolving_fail_t *p_auth = (ble_evt_sm_irk_resolving_fail_t *)&p_evt_param->event_param.ble_evt_sm.param.evt_irk_resolving_fail;

        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[SM] Resolving address fail by host id = %d\n", p_auth->host_id);
        }
    }
    break;

    case BLE_SM_EVT_NUMERIC_COMPARISON:
    {
        ble_evt_sm_numeric_comparison_param_t *p_num_comp = (ble_evt_sm_numeric_comparison_param_t *)&p_evt_param->event_param.ble_evt_sm.param.evt_numeric_comparison_param;

        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[SM] Numeric Comparison host id = %d\n", p_num_comp->host_id);
        }
    }
    break;

    default:
        break;
    }

    return status;
}