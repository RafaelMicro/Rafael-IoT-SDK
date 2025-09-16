/** @file ble_cmd_scan.c
 *
 * @brief Define BLE scan command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_event.h"
#include "ble_host_cmd.h"
#include "ble_privacy.h"
#include "ble_scan.h"
#include "hci_cmd_scan.h"
#include "log.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define SCAN_OWN_ADDR_TYPE_PUBLIC               0UL
#define SCAN_OWN_ADDR_TYPE_RANDOM               1UL
#define SCAN_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR  2UL
#define SCAN_OWN_ADDR_TYPE_RESOLVABLE_ADDR      3UL

/**
 * @brief  Define scan state.
 */
typedef uint8_t ble_scan_state_t;
#define SCAN_ENABLE_PROCESSING           0x01    /**< Scan enable command is processing. */
#define SCAN_ENABLED                     0x02    /**< Scan is enabled. */
#define SCAN_DISABLE_PROCESSING          0x03    /**< Scan disable command is processing. */
#define SCAN_DISABLED                    0x04    /**< Scan is disabled. */
/** @} */


/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static ble_scan_state_t g_scan_enable = SCAN_DISABLED;
static ble_scan_param_t g_scan_param =
{
    .scan_type = SCAN_TYPE_ACTIVE,
    .own_addr_type = SCAN_OWN_ADDR_TYPE_RANDOM,
    .scan_window = 80,
    .scan_interval = 160,
    .scan_filter_policy = SCAN_FILTER_POLICY_BASIC_UNFILTERED,
};

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static ble_err_t scan_status_set(ble_scan_state_t state)
{
    vPortEnterCritical();
    g_scan_enable = state;
    vPortExitCritical();

    return BLE_ERR_OK;
}

static ble_err_t scan_param_set(ble_scan_param_t *p_param)
{
    vPortEnterCritical();
    g_scan_param.scan_type = p_param->scan_type;
    g_scan_param.own_addr_type = p_param->own_addr_type;
    g_scan_param.scan_window = p_param->scan_window;
    g_scan_param.scan_interval = p_param->scan_interval;
    g_scan_param.scan_filter_policy = p_param->scan_filter_policy;
    vPortExitCritical();

    return BLE_ERR_OK;
}

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/** Check if the BLE scan is disabled or not.
 *
 */
bool ble_scan_idle_state_check(void)
{
    if (g_scan_enable == SCAN_DISABLED)
    {
        return TRUE;
    }

    return FALSE;
}

/** Set BLE scan parameters.
 *
 */
ble_err_t ble_cmd_scan_param_set(ble_scan_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_scan_param_param_t p_hci_param;
    ble_gap_addr_t ble_device_addr;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check scan filter policy
        if ((p_param->scan_filter_policy == SCAN_FILTER_POLICY_EXTENED_UNFILTERED) || (p_param->scan_filter_policy == SCAN_FILTER_POLICY_EXTENED_FILTERED))
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check status
        if ((g_scan_enable == SCAN_ENABLE_PROCESSING) || (g_scan_enable == SCAN_ENABLED))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check parameters
        if ( (p_param->scan_type != SCAN_TYPE_PASSIVE && p_param->scan_type != SCAN_TYPE_ACTIVE) ||
                (p_param->scan_interval < SCAN_INTERVAL_MIN || p_param->scan_interval > SCAN_INTERVAL_MAX) ||
                (p_param->scan_window < SCAN_WINDOW_MIN || p_param->scan_window > SCAN_WINDOW_MAX) ||
                (p_param->scan_window > p_param->scan_interval))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // get device address
        ble_gap_device_address_get(&ble_device_addr);

        // set HCI parameters
        p_hci_param.scan_type = p_param->scan_type;
        p_hci_param.scan_interval = p_param->scan_interval;
        p_hci_param.scan_window = p_param->scan_window;
        p_hci_param.scan_filter_policy = p_param->scan_filter_policy;

        if (ble_device_addr.addr_type == PUBLIC_ADDR)
        {
            p_hci_param.own_addr_type = SCAN_OWN_ADDR_TYPE_PUBLIC;
        }
        else if ((ble_device_addr.addr_type == RANDOM_STATIC_ADDR) ||
                (ble_device_addr.addr_type == RANDOM_NON_RESOLVABLE_ADDR) ||
                (ble_device_addr.addr_type == RANDOM_RESOLVABLE_ADDR))
        {
            p_hci_param.own_addr_type = SCAN_OWN_ADDR_TYPE_RANDOM;
        }
        else
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // issue HCI cmd
        if (hci_le_set_scan_param_cmd(&p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<SCAN_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        // set to local parameter
        scan_param_set(p_param);
    } while(0);

    return status;
}

/** Enable BLE scan.
 *
 */
ble_err_t ble_cmd_scan_enable(void)
{
    ble_gap_addr_t addr_param;
    ble_err_t status;
    ble_hci_cmd_set_scan_enable_param_t p_hci_cmd_parm;
    uint8_t host_id;

    status = BLE_ERR_OK;
    do {
        // check status
        if (ble_scan_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if ((g_scan_param.own_addr_type == SCAN_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR) ||
                (g_scan_param.own_addr_type == SCAN_OWN_ADDR_TYPE_RESOLVABLE_ADDR))
        {
            if (ble_privacy_parameter_enable_check() == FALSE)
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
            if ((ble_adv_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
            host_id = ble_privacy_host_id_get();
            addr_param.addr_type = g_scan_param.own_addr_type;
            if (g_scan_param.own_addr_type == SCAN_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR)
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
                if ((ble_adv_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
                {
                    status = BLE_ERR_INVALID_STATE;
                    break;
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
        scan_status_set(SCAN_ENABLE_PROCESSING);

        // set HCI parameters
        p_hci_cmd_parm.scan_enable = 0x01;
        p_hci_cmd_parm.scan_filter_duplicates = 0x00;

        // issue HCI cmd
        if (hci_le_set_scan_enable_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<SCAN_ENABLE> HCI TX CMD QUEUE FULL.\n");
        }

    } while(0);

    return status;
}

ble_err_t ble_cmd_scan_disable(void)
{
    ble_err_t status;
    ble_hci_cmd_set_scan_enable_param_t p_hci_cmd_parm;

    status = BLE_ERR_OK;
    do {
        // check status
        if ((g_scan_enable == SCAN_DISABLE_PROCESSING) || (g_scan_enable == SCAN_DISABLED))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // set status
        scan_status_set(SCAN_DISABLE_PROCESSING);

        // set HCI parameters
        p_hci_cmd_parm.scan_enable = 0x00;
        p_hci_cmd_parm.scan_filter_duplicates = 0x00;
        // issue HCI cmd
        if (hci_le_set_scan_enable_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<SCAN_DISABLE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Parsing scanned BLE device's advertising data.
 *
 */
ble_err_t ble_cmd_scan_report_adv_data_parsing(ble_evt_scan_adv_report_t *p_rpt_data,
        ble_gap_ad_type_t           ad_type,
        uint8_t                     *p_data,
        uint8_t                     *p_data_length)
{
    ble_err_t status;
    uint8_t i = 0;

    status = BLE_ERR_INVALID_PARAMETER;

    do {
        if ((p_rpt_data->length == 0) || (p_rpt_data->length > BLE_ADV_DATA_SIZE_MAX))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        *p_data_length = 0;
        while (i < p_rpt_data->length)
        {
            if (p_rpt_data->data[i + 1] == ad_type)   // type = 2nd byte
            {
                *p_data_length = p_rpt_data->data[i] - 1;                        // data len = 1st byte - type len(1)
                memcpy(p_data, p_rpt_data->data + (i + 2), *p_data_length);      // data     = start from 3rd byte
                status = BLE_ERR_OK;
                break;
            }
            i = i + p_rpt_data->data[i] + 1;
        }
        if (status != BLE_ERR_OK)
        {
            status = BLE_ERR_INVALID_PARAMETER;
        }
    } while (0);

    return status;
}

/** BLE scan module event handler.
 *
 */
ble_err_t ble_evt_scan_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_SCAN_EVT_SET_PARAM:
    {
        ble_evt_scan_set_scan_param_t *p_scan_param = (ble_evt_scan_set_scan_param_t *)&p_evt_param->event_param.ble_evt_scan.param.evt_set_scan_param;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[SCAN] SET PARAM status = %d\n", p_scan_param->status);
        }
    }
    break;

    case BLE_SCAN_EVT_SET_ENABLE:
    {
        ble_evt_scan_set_scan_enable_t *p_scan_enable = (ble_evt_scan_set_scan_enable_t *)&p_evt_param->event_param.ble_evt_scan.param.evt_set_scan_enable;

        // post to user
        if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (g_scan_enable == SCAN_ENABLE_PROCESSING)
            {
                p_scan_enable->scan_enabled = TRUE;
            }
            else if (g_scan_enable == SCAN_DISABLE_PROCESSING)
            {
                p_scan_enable->scan_enabled = FALSE;
            }
        }
        else
        {
            // recover to last state
            if (g_scan_enable == SCAN_ENABLE_PROCESSING)
            {
                p_scan_enable->scan_enabled = FALSE;
            }
            else if (g_scan_enable == SCAN_DISABLE_PROCESSING)
            {
                p_scan_enable->scan_enabled = TRUE;
            }
        }
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
            {
                if (g_scan_enable == SCAN_ENABLE_PROCESSING)
                {
                    scan_status_set(SCAN_ENABLED);
                }
                else if (g_scan_enable == SCAN_DISABLE_PROCESSING)
                {
                    scan_status_set(SCAN_DISABLED);
                }
            }
            else
            {
                // recover to last state
                if (g_scan_enable == SCAN_ENABLE_PROCESSING)
                {
                    scan_status_set(SCAN_DISABLED);
                }
                else if (g_scan_enable == SCAN_DISABLE_PROCESSING)
                {
                    scan_status_set(SCAN_ENABLED);
                }
            }

            log_info("[SCAN] SET ENABLE status = %d\n", p_scan_enable->status);
        }
    }
    break;

    case BLE_SCAN_EVT_ADV_REPORT:
    {
        // post to user
        status = g_ble_event_cb(p_evt_param);
    }
    break;

    default:
        break;
    }

    return status;
}
