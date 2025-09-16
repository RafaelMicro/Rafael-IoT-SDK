/** @file ble_cmd_advertising.c
 *
 * @brief Define BLE advertising command definition, structure and functions.
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
#include "ble_host_cmd.h"
#include "ble_host_ref.h"
#include "ble_privacy.h"
#include "hci_cmd_advertising.h"
#include "log.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

#define ADV_OWN_ADDR_TYPE_PUBLIC                0UL
#define ADV_OWN_ADDR_TYPE_RANDOM                1UL
#define ADV_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR   2UL
#define ADV_OWN_ADDR_TYPE_RESOLVABLE_ADDR       3UL

/**
 * @brief  Define advertising state.
 */
typedef uint8_t ble_adv_state_t;
#define ADV_ENABLE_PROCESSING           0x01    /**< Advertising enable command is processing. */
#define ADV_ENABLED                     0x02    /**< Advertising is enabled. */
#define ADV_DISABLE_PROCESSING          0x03    /**< Advertising disable command is processing. */
#define ADV_DISABLED                    0x04    /**< Advertising is disabled. */
/** @} */


/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static ble_adv_state_t g_adv_enable = ADV_DISABLED;
static ble_adv_param_t g_adv_param =
{
    .own_addr_type = ADV_OWN_ADDR_TYPE_RANDOM,
    .adv_type = ADV_TYPE_ADV_IND,
    .adv_interval_min = 160,    // 100ms
    .adv_interval_max = 160,    // 100ms
    .adv_peer_addr_param = 0,
    .adv_channel_map = ADV_CHANNEL_ALL,
    .adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL,
};

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static ble_err_t adv_status_set(ble_adv_state_t state)
{
    vPortEnterCritical();
    g_adv_enable = state;
    vPortExitCritical();

    return BLE_ERR_OK;
}

static ble_err_t adv_param_set(ble_adv_param_t *p_param)
{
    vPortEnterCritical();
    g_adv_param.own_addr_type = p_param->own_addr_type;
    g_adv_param.adv_channel_map = p_param->adv_channel_map;
    g_adv_param.adv_filter_policy = p_param->adv_filter_policy;
    g_adv_param.adv_interval_max = p_param->adv_interval_max;
    g_adv_param.adv_interval_min = p_param->adv_interval_min;
    g_adv_param.adv_peer_addr_param.addr_type = p_param->adv_peer_addr_param.addr_type;
    memcpy(g_adv_param.adv_peer_addr_param.addr, p_param->adv_peer_addr_param.addr, BLE_ADDR_LEN);
    g_adv_param.adv_type = p_param->adv_type;
    vPortExitCritical();

    return BLE_ERR_OK;
}

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/** Check if the BLE advertising is disabled or not.
 *
 */
bool ble_adv_idle_state_check(void)
{
    if (g_adv_enable == ADV_DISABLED)
    {
        return TRUE;
    }

    return FALSE;
}

/** Set BLE advertising parameters.
 *
 */
ble_err_t ble_cmd_adv_param_set(ble_adv_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_adv_param_param_t p_hci_param;
    ble_gap_addr_t ble_device_addr;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check status
        if (ble_adv_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check parameters
        if ( (p_param->adv_type > ADV_TYPE_ADV_NONCONN_IND) ||
                (p_param->adv_channel_map == 0x00) ||
                (p_param->adv_interval_min < ADV_INTERVAL_MIN || p_param->adv_interval_min > ADV_INTERVAL_MAX) ||
                (p_param->adv_interval_max < ADV_INTERVAL_MIN || p_param->adv_interval_max > ADV_INTERVAL_MAX) ||
                (p_param->adv_interval_min > p_param->adv_interval_max) ||
                (p_param->adv_channel_map > ADV_CHANNEL_ALL) ||
                (p_param->adv_filter_policy > ADV_FILTER_POLICY_ACCEPT_SCAN_CONN_REQ_FROM_FAL))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // get BLE device address
        ble_gap_device_address_get(&ble_device_addr);

        // set HCI parameters
        p_hci_param.adv_channel_map = p_param->adv_channel_map;
        p_hci_param.adv_filter_policy = p_param->adv_filter_policy;
        p_hci_param.adv_interval_max = p_param->adv_interval_max;
        p_hci_param.adv_interval_min = p_param->adv_interval_min;
        p_hci_param.peer_addr_type = p_param->adv_peer_addr_param.addr_type;
        memcpy(p_hci_param.peer_addr, p_param->adv_peer_addr_param.addr, BLE_ADDR_LEN);
        p_hci_param.adv_type = p_param->adv_type;

        switch (p_param->own_addr_type)
        {
        case PUBLIC_ADDR:
            p_hci_param.own_addr_type = ADV_OWN_ADDR_TYPE_PUBLIC;
            break;

        case RANDOM_STATIC_ADDR:
        case RANDOM_NON_RESOLVABLE_ADDR:
        case RANDOM_RESOLVABLE_ADDR:
            p_hci_param.own_addr_type = ADV_OWN_ADDR_TYPE_RANDOM;
            break;

        default:
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // issue HCI cmd
        if (hci_le_set_adv_param_cmd(&p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        else
        {
            // set to local parameter
            adv_param_set(p_param);
        }
    } while (0);

    return status;
}

/** Set BLE advertising data.
 *
 */
ble_err_t ble_cmd_adv_data_set(ble_adv_data_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_adv_data_param_t p_hci_param;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check data length
        if (p_param->length > BLE_ADV_DATA_SIZE_MAX)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // set HCI parameters
        p_hci_param.adv_data_length = p_param->length;
        memcpy(p_hci_param.adv_data, p_param->data, p_param->length);

        // issue HCI cmd
        if (hci_le_set_adv_data_cmd(&p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_DATA_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** Set BLE scan response data.
 *
 */
ble_err_t ble_cmd_adv_scan_rsp_set(ble_adv_data_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_scan_rsp_param_t p_hci_param;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check data length
        if (p_param->length > BLE_ADV_DATA_SIZE_MAX)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // set HCI parameters
        p_hci_param.scan_rsp_data_length = p_param->length;
        memcpy(p_hci_param.scan_rsp_data, p_param->data, p_param->length);

        // issue HCI cmd
        if (hci_le_set_scan_rsp_data_cmd(&p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_SCAN_RSP_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Set BLE start advertising.
 *
 */
ble_err_t ble_cmd_adv_enable(uint8_t host_id)
{
    ble_gap_addr_t addr_param;
    ble_err_t status;
    ble_hci_cmd_set_adv_enable_param_t p_hci_param;
    uint16_t conn_id;
    extern ble_irk_rpa_addr_t *param_rpa;

    status = BLE_ERR_OK;
    do {
        // check status
        if (ble_adv_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check connection is exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (bhc_host_id_is_active_check(host_id) == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check adv type
        if (((host_id == BLE_HOSTID_RESERVED) || (bhc_host_connected_link_number_get() == MAX_CONN_NO_APP)) &&
                ((g_adv_param.adv_type != ADV_TYPE_ADV_NONCONN_IND) && (g_adv_param.adv_type != ADV_TYPE_ADV_NONCONN_IND)))
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        if (ble_privacy_parameter_enable_check() == TRUE)
        {
            if ((ble_scan_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
        }

        if ((g_adv_param.own_addr_type == ADV_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR) ||
                (g_adv_param.own_addr_type == ADV_OWN_ADDR_TYPE_RESOLVABLE_ADDR))
        {
            if ((ble_scan_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
            addr_param.addr_type = g_adv_param.own_addr_type;
            if (g_adv_param.own_addr_type == ADV_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR)
            {
                param_rpa[host_id].rpa_addr[5] &= 0x3F;
            }
            memcpy(addr_param.addr, param_rpa[host_id].rpa_addr, BLE_ADDR_LEN);
            status = ble_gap_device_address_set(&addr_param);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            if (ble_gap_device_address_compare() != BLE_ERR_OK)
            {
                if ((ble_scan_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
                {
                    return BLE_ERR_INVALID_STATE;
                }
                ble_gap_device_identity_address_get(&addr_param);
                status = ble_gap_device_address_set(&addr_param);
                if (status != BLE_ERR_OK)
                {
                    break;
                }
            }
        }

        // set status
        adv_status_set(ADV_ENABLE_PROCESSING);

        // set HCI parameters
        p_hci_param.adv_enable = 0x01; // enable

        // set host id to active mode
        bhc_host_id_state_active_set(host_id, BLE_GAP_ROLE_PERIPHERAL);
        if (hci_le_set_adv_enable_cmd(&p_hci_param) != BLE_ERR_OK)
        {
            bhc_host_id_state_active_release(BLE_GAP_ROLE_PERIPHERAL);
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_ENABLE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Set BLE stop advertising.
 *
 */
ble_err_t ble_cmd_adv_disable(void)
{
    ble_err_t status;
    ble_hci_cmd_set_adv_enable_param_t p_hci_param;

    status = BLE_ERR_OK;
    do {
        // check status
        if ((g_adv_enable == ADV_DISABLE_PROCESSING) || (g_adv_enable == ADV_DISABLED))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // set status
        adv_status_set(ADV_DISABLE_PROCESSING);

        // set HCI parameters
        p_hci_param.adv_enable = 0x00; // disable

        // issue HCI cmd
        if (hci_le_set_adv_enable_cmd(&p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_DISABLE> Send to BLE stack fail\n");
        }
    } while (0);

    return status;
}

/** BLE advertising module event handler.
 *
 */
ble_err_t ble_evt_adv_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_ADV_EVT_SET_PARAM:
    {
        ble_evt_adv_set_adv_param_t *p_adv_param = (ble_evt_adv_set_adv_param_t *)&p_evt_param->event_param.ble_evt_adv.param.evt_set_adv_param;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] SET PARAM status = %d\n", p_adv_param->status);
        }
    }
    break;

    case BLE_ADV_EVT_SET_DATA:
    {
        ble_evt_adv_set_adv_data_t *p_adv_data = (ble_evt_adv_set_adv_data_t *)&p_evt_param->event_param.ble_evt_adv.param.evt_set_adv_data;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] SET DATA status = %d\n", p_adv_data->status);
        }
    }
    break;

    case BLE_ADV_EVT_SET_SCAN_RSP:
    {
        ble_evt_adv_set_scan_rsp_t *p_scan_rsp = (ble_evt_adv_set_scan_rsp_t *)&p_evt_param->event_param.ble_evt_adv.param.evt_set_scan_rsp;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] SET SCAN RSP status = %d\n", p_scan_rsp->status);
        }
    }
    break;

    case BLE_ADV_EVT_SET_ENABLE:
    {
        ble_evt_adv_set_adv_enable_t *p_adv_enable = &p_evt_param->event_param.ble_evt_adv.param.evt_set_adv_enable;

        // post to user
        if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (g_adv_enable == ADV_ENABLE_PROCESSING)
            {
                p_adv_enable->adv_enabled = TRUE;
            }
            else if (g_adv_enable == ADV_DISABLE_PROCESSING)
            {
                p_adv_enable->adv_enabled = FALSE;
            }
        }
        else
        {
            // recover to last state
            if (g_adv_enable == ADV_ENABLE_PROCESSING)
            {
                p_adv_enable->adv_enabled = FALSE;
            }
            else if (g_adv_enable == ADV_DISABLE_PROCESSING)
            {
                p_adv_enable->adv_enabled = TRUE;
            }
        }

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
            {
                if (g_adv_enable == ADV_ENABLE_PROCESSING)
                {
                    adv_status_set(ADV_ENABLED);
                }
                else if (g_adv_enable == ADV_DISABLE_PROCESSING)
                {
                    // update host id state
                    bhc_host_id_state_active_release(BLE_GAP_ROLE_PERIPHERAL);
                    adv_status_set(ADV_DISABLED);
                }
            }
            else
            {
                // recover to last state
                if (g_adv_enable == ADV_ENABLE_PROCESSING)
                {
                    adv_status_set(ADV_DISABLED);
                }
                else if (g_adv_enable == ADV_DISABLE_PROCESSING)
                {
                    adv_status_set(ADV_ENABLED);
                }
            }
            log_info("[ADV] SET ENABLE status = %d\n", p_adv_enable->status);
        }
    }
    break;

    case BLE_GAP_EVT_CONN_COMPLETE:
    {
        ble_evt_gap_conn_complete_t *p_conn_param = (ble_evt_gap_conn_complete_t *)&p_evt_param->event_param.ble_evt_gap.param.evt_conn_complete;

        if ((p_conn_param->role == BLE_GAP_ROLE_PERIPHERAL) || (p_conn_param->status == BLE_HCI_ERR_CODE_DIRECTED_ADVERTISING_TIMEOUT))
        {
            adv_status_set(ADV_DISABLED);
        }
    }
    break;

    default:
        break;
    }

    return status;
}
