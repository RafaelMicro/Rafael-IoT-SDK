/** @file ble_cmd_gap.c
 *
 * @brief Define BLE GAP command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_event.h"
#include "ble_gap.h"
#include "ble_host_cmd.h"
#include "ble_host_ref.h"
#include "ble_privacy.h"
#include "ble_profile.h"
#include "hci_cmd_controller.h"
#include "hci_cmd_connect.h"
#include "hci_cmd_security.h"
#include "hci_cmd_vendor.h"
#include "log.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define MSK_HCI_CONNID_16_CENTRAL                   0x0200
#define MSK_HCI_CONNID_16_PERIPHERAL                0x0300

#define CRCON_OWN_ADDR_TYPE_PUBLIC                  0UL
#define CRCON_OWN_ADDR_TYPE_RANDOM                  1UL
#define CRCON_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR     2UL
#define CRCON_OWN_ADDR_TYPE_RESOLVABLE_ADDR         3UL

/**
* @brief  Define initiator state.
*/
typedef uint8_t ble_cmd_state_t;
#define STATE_IDLE                    0x00    /**< Idle mode. */
#define STATE_CMD_PROCESSING          0x01    /**< Command is in processing mode. */
/** @} */

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/


/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static ble_gap_addr_t  g_ble_device_addr =
{
    .addr_type = RANDOM_STATIC_ADDR,
    .addr = {0x11, 0x12, 0x13, 0x14, 0x15, 0xC1}
};

static ble_gap_addr_t  g_ble_identity_addr =
{
    .addr_type = RANDOM_STATIC_ADDR,
    .addr = {0x11, 0x12, 0x13, 0x14, 0x15, 0xC1}
};

ble_cmd_state_t  g_init_enable    = STATE_IDLE;

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static ble_err_t ble_init_status_set(ble_cmd_state_t state)
{
    vPortEnterCritical();
    g_init_enable = state;
    vPortExitCritical();

    return BLE_ERR_OK;
}

static ble_err_t ble_global_addr_set(ble_gap_addr_t *p_addr)
{
    vPortEnterCritical();
    if ((p_addr->addr_type  == PUBLIC_ADDR) ||
            (p_addr->addr_type  == RANDOM_STATIC_ADDR))
    {
        memcpy(&g_ble_identity_addr, p_addr, sizeof(ble_gap_addr_t));
    }
    else
    {
        p_addr->addr_type = RANDOM_STATIC_ADDR;
    }
    memcpy(&g_ble_device_addr, p_addr, sizeof(ble_gap_addr_t));
    vPortExitCritical();

    return BLE_ERR_OK;
}


static ble_err_t random_addr_set(ble_gap_addr_t *p_addr)
{
    ble_err_t status;
    ble_hci_cmd_set_random_addr_param_t p_hci_cmd_parm;

    status = BLE_ERR_OK;
    // set HCI parameters
    memcpy(p_hci_cmd_parm.addr, p_addr->addr, BLE_ADDR_LEN);

    // issue HCI cmd
    if (hci_le_set_random_addr_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<RANDOM_ADDR_SET> HCI TX CMD QUEUE FULL.\n");
    }
    else
    {
        ble_global_addr_set(p_addr);
    }

    return status;
}


static ble_err_t public_addr_set(ble_gap_addr_t *p_addr)
{
    ble_err_t status;
    ble_hci_vcmd_cntlr_info_param_t p_hci_cmd_parm;

    status = BLE_ERR_OK;
    p_hci_cmd_parm.ble_version = BLE_STACK_VERSION;
    p_hci_cmd_parm.ble_company_id = BLE_COMPANY_ID;
    memcpy(p_hci_cmd_parm.ble_public_addr, p_addr->addr, BLE_ADDR_LEN);

    // issue HCI cmd
    if (hci_vendor_set_controller_info_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<CTRL_INFO_SET> HCI TX CMD QUEUE FULL.\n");
    }
    else
    {
        ble_global_addr_set(p_addr);
    }

    return status;
}

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/** Set BLE device address type and address.
 *
 */
ble_err_t ble_gap_device_address_set(ble_gap_addr_t *p_addr)
{
    ble_err_t status = BLE_ERR_OK;

    status = BLE_ERR_OK;
    do {
        // check init state
        if (ble_init_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (p_addr == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check connection
        for (uint8_t i = 0; i < max_num_conn_host; i++)
        {
            uint16_t conn_id;

            if (bhc_host_id_is_connected_check(i, &conn_id) == TRUE)
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
        }
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check if in advertising state
        if (ble_adv_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check if in scan state
        if (ble_scan_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check BLE address type
        status = BLE_ERR_INVALID_PARAMETER;
        switch (p_addr->addr_type)
        {
        case PUBLIC_ADDR:
            status = public_addr_set(p_addr);
            break;

        case RANDOM_STATIC_ADDR:
            // check ADDDR MSB[7:6] = 11
            if ((p_addr->addr[BLE_ADDR_LEN - 1] & 0xC0) == 0xC0 )
            {
                status = random_addr_set(p_addr);
            }
            break;
        case RANDOM_NON_RESOLVABLE_ADDR:
            // check ADDDR MSB[7:6] = 00
            if ((p_addr->addr[BLE_ADDR_LEN - 1] & 0xC0) == 0x00 )
            {
                status = random_addr_set(p_addr);
            }
            break;
        case RANDOM_RESOLVABLE_ADDR:
            // check ADDDR MSB[7:6] = 01
            if ((p_addr->addr[BLE_ADDR_LEN - 1] & 0xC0) == 0x40 )
            {
                status = random_addr_set(p_addr);
            }
            break;

        default:
            break;
        }
    } while (0);

    return status;
}

/** Get BLE device address type and address.
 *
 */
bool ble_init_idle_state_check(void)
{
    if (g_init_enable == STATE_IDLE)
    {
        return TRUE;
    }
    return FALSE;
}

/** Get BLE device address type and address.
 *
 */
ble_err_t ble_gap_device_address_get(ble_gap_addr_t *p_addr)
{
    memcpy(p_addr, &g_ble_device_addr, sizeof(ble_gap_addr_t));

    return BLE_ERR_OK;
}

/** Get BLE device address type and address.
 *
 */
ble_err_t ble_gap_device_identity_address_get(ble_gap_addr_t *p_addr)
{
    memcpy(p_addr, &g_ble_identity_addr, sizeof(ble_gap_addr_t));

    return BLE_ERR_OK;
}

/** Get BLE device address type and address.
 *
 */
ble_err_t ble_gap_device_address_compare(void)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (memcmp(&g_ble_device_addr.addr_type, &g_ble_identity_addr.addr_type, sizeof(ble_gap_addr_t)) != 0)
    {
        status = BLE_ERR_INVALID_PARAMETER;
    }

    return status;
}

/** Get BLE device address and device address type.
 *
*/
ble_err_t ble_cmd_device_addr_get(ble_gap_addr_t *p_addr)
{
    memcpy(p_addr, &g_ble_device_addr, sizeof(ble_gap_addr_t));

    return BLE_ERR_OK;
}

/** Set BLE device address and device address type.
 *
*/
ble_err_t ble_cmd_device_addr_set(ble_gap_addr_t *p_addr)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        // check init state
        if (ble_init_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (p_addr == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check connection
        for (int8_t i = 0; i < max_num_conn_host; ++i)
        {
            uint16_t conn_id;

            if (bhc_host_id_is_connected_check(i, &conn_id) == TRUE)
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check if in advertising state
        if (ble_adv_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check if in scan state
        if (ble_scan_idle_state_check() == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check BLE address type
        status = BLE_ERR_INVALID_PARAMETER;
        switch (p_addr->addr_type)
        {
        case PUBLIC_ADDR:
            status = public_addr_set(p_addr);
            break;

        case RANDOM_STATIC_ADDR:
            // check ADDDR MSB[7:6] = 11
            if ((p_addr->addr[BLE_ADDR_LEN - 1] & 0xC0) == 0xC0 )
            {
                status = random_addr_set(p_addr);
            }
            break;
        case RANDOM_NON_RESOLVABLE_ADDR:
            // check ADDDR MSB[7:6] = 00
            if ((p_addr->addr[BLE_ADDR_LEN - 1] & 0xC0) == 0x00 )
            {
                status = random_addr_set(p_addr);
            }
            break;
        case RANDOM_RESOLVABLE_ADDR:
            // check ADDDR MSB[7:6] = 01
            if ((p_addr->addr[BLE_ADDR_LEN - 1] & 0xC0) == 0x40 )
            {
                status = random_addr_set(p_addr);
            }
            break;

        default:
            break;
        }
    } while (0);

    return status;
}

/** BLE connection create command.
 *
 */
ble_err_t ble_cmd_conn_create(ble_gap_create_conn_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_create_conn_param_t p_hci_cmd_parm;
    uint16_t conn_id;
    ble_gap_addr_t ble_device_addr;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check maximum connection link number
        if (MAX_CONN_NO_APP == 0)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host id is valid or not
        if (bhc_host_id_is_valid_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        //check host id is active or not
        if (bhc_host_id_is_active_check(p_param->host_id) == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check state
        if ((bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == TRUE) ||
                (ble_init_idle_state_check() == FALSE) ||
                (ble_scan_idle_state_check() == FALSE) )
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check parameters
        if ((p_param->scan_interval < SCAN_INTERVAL_MIN || p_param->scan_interval > SCAN_INTERVAL_MAX) ||
                (p_param->scan_window < SCAN_WINDOW_MIN || p_param->scan_window > SCAN_WINDOW_MAX) ||
                (p_param->scan_window > p_param->scan_interval))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if ( (p_param->conn_param.min_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->conn_param.min_conn_interval > BLE_CONN_INTERVAL_MAX) ||
                (p_param->conn_param.max_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->conn_param.max_conn_interval > BLE_CONN_INTERVAL_MAX) ||
                (p_param->conn_param.min_conn_interval > p_param->conn_param.max_conn_interval) ||
                (p_param->conn_param.periph_latency > BLE_CONN_LATENCY_MAX) ||
                (p_param->conn_param.supv_timeout < BLE_CONN_SUPV_TIMEOUT_MIN || p_param->conn_param.supv_timeout > BLE_CONN_SUPV_TIMEOUT_MAX) ||
                ((p_param->conn_param.supv_timeout * 4) < ((1 + p_param->conn_param.periph_latency) * p_param->conn_param.max_conn_interval )))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // get device address
        ble_gap_device_address_get(&ble_device_addr);

        // set HCI parameters
        p_hci_cmd_parm.scan_interval = p_param->scan_interval;
        p_hci_cmd_parm.scan_window = p_param->scan_window;
        p_hci_cmd_parm.init_filter_policy = p_param->init_filter_policy;
        p_hci_cmd_parm.peer_addr_type = p_param->peer_addr.addr_type;
        memcpy(p_hci_cmd_parm.peer_addr, p_param->peer_addr.addr, BLE_ADDR_LEN);
        p_hci_cmd_parm.conn_interval_min = p_param->conn_param.min_conn_interval;
        p_hci_cmd_parm.conn_interval_max = p_param->conn_param.max_conn_interval;
        p_hci_cmd_parm.max_latency = p_param->conn_param.periph_latency;
        p_hci_cmd_parm.supv_timeout = p_param->conn_param.supv_timeout;
        p_hci_cmd_parm.min_celength = 12;
        p_hci_cmd_parm.max_celength = 12;

        if (ble_device_addr.addr_type == PUBLIC_ADDR)
        {
            p_hci_cmd_parm.own_addr_type = CRCON_OWN_ADDR_TYPE_PUBLIC;
        }
        else if ((ble_device_addr.addr_type == RANDOM_STATIC_ADDR) ||
                (ble_device_addr.addr_type == RANDOM_NON_RESOLVABLE_ADDR) ||
                (ble_device_addr.addr_type == RANDOM_RESOLVABLE_ADDR))
        {
            p_hci_cmd_parm.own_addr_type = CRCON_OWN_ADDR_TYPE_RANDOM;
        }
        else
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (ble_privacy_parameter_enable_check() == TRUE)
        {
            if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
        }

        if ((p_param->own_addr_type == CRCON_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR) ||
                (p_param->own_addr_type == CRCON_OWN_ADDR_TYPE_RESOLVABLE_ADDR))
        {
            if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
            ble_device_addr.addr_type = p_param->own_addr_type;
            if (p_param->own_addr_type == CRCON_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR)
            {
                param_rpa[p_param->host_id].rpa_addr[5] &= 0x3F;
            }
            memcpy(ble_device_addr.addr, param_rpa[p_param->host_id].rpa_addr, BLE_ADDR_LEN);
            status = ble_gap_device_address_set(&ble_device_addr);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            if (ble_gap_device_address_compare() != BLE_ERR_OK)
            {
                if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE))
                {
                    status = BLE_ERR_INVALID_STATE;
                    break;
                }
                ble_gap_device_identity_address_get(&ble_device_addr);
                status = ble_gap_device_address_set(&ble_device_addr);
                if (status != BLE_ERR_OK)
                {
                    break;
                }
            }
        }

        // set state
        ble_init_status_set(STATE_CMD_PROCESSING);

        // issue HCI cmd
        bhc_host_id_state_active_set(p_param->host_id, BLE_GAP_ROLE_CENTRAL);
        if (hci_le_create_conn_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            bhc_host_id_state_active_release(BLE_GAP_ROLE_CENTRAL);
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CREATE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** BLE connection create command.
 *
 */
ble_err_t ble_cmd_extend_conn_create_v2(ble_gap_extend_create_conn_v2_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_extended_create_conn_v2_t *p_hci_cmd_parm;
    uint16_t conn_id;
    ble_gap_addr_t ble_device_addr;

    status = BLE_ERR_OK;
    p_hci_cmd_parm = NULL;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check maximum connection link number
        if (MAX_CONN_NO_APP == 0)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host id is valid or not
        if (bhc_host_id_is_valid_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        //check host id is active or not
        if (bhc_host_id_is_active_check(p_param->host_id) == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check state
        if ((bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == TRUE) ||
                (ble_init_idle_state_check() == FALSE) ||
                (ble_scan_idle_state_check() == FALSE) )
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check parameters
        if ((p_param->scan_interval < SCAN_INTERVAL_MIN || p_param->scan_interval > SCAN_INTERVAL_MAX) ||
                (p_param->scan_window < SCAN_WINDOW_MIN || p_param->scan_window > SCAN_WINDOW_MAX) ||
                (p_param->scan_window > p_param->scan_interval))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if ( (p_param->conn_param.min_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->conn_param.min_conn_interval > BLE_CONN_INTERVAL_MAX) ||
                (p_param->conn_param.max_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->conn_param.max_conn_interval > BLE_CONN_INTERVAL_MAX) ||
                (p_param->conn_param.min_conn_interval > p_param->conn_param.max_conn_interval) ||
                (p_param->conn_param.periph_latency > BLE_CONN_LATENCY_MAX) ||
                (p_param->conn_param.supv_timeout < BLE_CONN_SUPV_TIMEOUT_MIN || p_param->conn_param.supv_timeout > BLE_CONN_SUPV_TIMEOUT_MAX) ||
                ((p_param->conn_param.supv_timeout * 4) < ((1 + p_param->conn_param.periph_latency) * p_param->conn_param.max_conn_interval )))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // get device address
        ble_gap_device_address_get(&ble_device_addr);

        p_hci_cmd_parm = pvPortMalloc(sizeof(ble_hci_cmd_extended_create_conn_v2_t) + sizeof(extended_create_conn_param_t));

        if (p_hci_cmd_parm == NULL)
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            break;
        }
        // set HCI parameters
        p_hci_cmd_parm->adv_handle = p_param->adv_handle;
        p_hci_cmd_parm->subevent = p_param->subevent;
        p_hci_cmd_parm->initiating_phys = 1;
        p_hci_cmd_parm->conn_param[0].scan_interval = p_param->scan_interval;
        p_hci_cmd_parm->conn_param[0].scan_window = p_param->scan_window;
        p_hci_cmd_parm->initiator_filter_policy = p_param->init_filter_policy;
        p_hci_cmd_parm->peer_addr_type = p_param->peer_addr.addr_type;

        memcpy(p_hci_cmd_parm->peer_addr, p_param->peer_addr.addr, BLE_ADDR_LEN);
        p_hci_cmd_parm->conn_param[0].conn_interval_min = p_param->conn_param.min_conn_interval;
        p_hci_cmd_parm->conn_param[0].conn_interval_max = p_param->conn_param.max_conn_interval;
        p_hci_cmd_parm->conn_param[0].max_latency = p_param->conn_param.periph_latency;
        p_hci_cmd_parm->conn_param[0].supervision_timeout = p_param->conn_param.supv_timeout;
        p_hci_cmd_parm->conn_param[0].min_ce_length = 12;
        p_hci_cmd_parm->conn_param[0].max_ce_length = 12;

        if (ble_device_addr.addr_type == PUBLIC_ADDR)
        {
            p_hci_cmd_parm->own_addr_type = CRCON_OWN_ADDR_TYPE_PUBLIC;
        }
        else if ((ble_device_addr.addr_type == RANDOM_STATIC_ADDR) ||
                (ble_device_addr.addr_type == RANDOM_NON_RESOLVABLE_ADDR) ||
                (ble_device_addr.addr_type == RANDOM_RESOLVABLE_ADDR))
        {
            p_hci_cmd_parm->own_addr_type = CRCON_OWN_ADDR_TYPE_RANDOM;
        }
        else
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (ble_privacy_parameter_enable_check() == TRUE)
        {
            if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
        }

        if ((p_param->own_addr_type == CRCON_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR) ||
                (p_param->own_addr_type == CRCON_OWN_ADDR_TYPE_RESOLVABLE_ADDR))
        {
            if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE))
            {
                status = BLE_ERR_INVALID_STATE;
                break;
            }
            ble_device_addr.addr_type = p_param->own_addr_type;
            if (p_param->own_addr_type == CRCON_OWN_ADDR_TYPE_NON_RESOLVABLE_ADDR)
            {
                param_rpa[p_param->host_id].rpa_addr[5] &= 0x3F;
            }
            memcpy(ble_device_addr.addr, param_rpa[p_param->host_id].rpa_addr, BLE_ADDR_LEN);
            status = ble_gap_device_address_set(&ble_device_addr);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            if (ble_gap_device_address_compare() != BLE_ERR_OK)
            {
                if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE))
                {
                    status = BLE_ERR_INVALID_STATE;
                    break;
                }
                ble_gap_device_identity_address_get(&ble_device_addr);
                status = ble_gap_device_address_set(&ble_device_addr);
                if (status != BLE_ERR_OK)
                {
                    break;
                }
            }
        }

        // set state
        ble_init_status_set(STATE_CMD_PROCESSING);

        bhc_host_id_state_active_enhanced_set(p_param->host_id, p_param->adv_handle, PADV_ACTIVE);
        // issue HCI cmd
        if (hci_le_extended_create_conn_v2_cmd(p_hci_cmd_parm) != BLE_ERR_OK)
        {
            bhc_host_id_state_enhance_active_release(PADV_ACTIVE, p_param->adv_handle);
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CREATE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    if (p_hci_cmd_parm != NULL)
    {
        vPortFree(p_hci_cmd_parm);
    }
    return status;
}


/** BLE cancel create connection process command.
 *
 */
ble_err_t ble_cmd_conn_create_cancel(void)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        // check status
        if (ble_init_idle_state_check() == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // update state
        ble_init_status_set(STATE_IDLE);

        // issue HCI cmd
        if (hci_le_create_conn_cancel_cmd() != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CANCEL> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** BLE connection parameter update.
 *
 */
ble_err_t ble_cmd_conn_param_update(ble_gap_conn_param_update_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_conn_updated_param_t p_hci_cmd_parm;
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

        if (bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check parameters
        if ( (p_param->ble_conn_param.min_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->ble_conn_param.min_conn_interval > BLE_CONN_INTERVAL_MAX) ||
                (p_param->ble_conn_param.max_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->ble_conn_param.max_conn_interval > BLE_CONN_INTERVAL_MAX) ||
                (p_param->ble_conn_param.min_conn_interval > p_param->ble_conn_param.max_conn_interval) ||
                (p_param->ble_conn_param.periph_latency > BLE_CONN_LATENCY_MAX) ||
                (p_param->ble_conn_param.supv_timeout < BLE_CONN_SUPV_TIMEOUT_MIN || p_param->ble_conn_param.supv_timeout > BLE_CONN_SUPV_TIMEOUT_MAX) ||
                ((p_param->ble_conn_param.supv_timeout * 4) <= ((1 + p_param->ble_conn_param.periph_latency) * p_param->ble_conn_param.max_conn_interval )))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // set HCI parameters
        if ((conn_id & 0xFF00) == MSK_HCI_CONNID_16_CENTRAL)
        {
            p_hci_cmd_parm.conn_handle = conn_id;
            p_hci_cmd_parm.conn_interval_min = p_param->ble_conn_param.min_conn_interval;
            p_hci_cmd_parm.conn_interval_max = p_param->ble_conn_param.max_conn_interval;
            p_hci_cmd_parm.periph_latency = p_param->ble_conn_param.periph_latency;
            p_hci_cmd_parm.supv_timeout = p_param->ble_conn_param.supv_timeout;
            p_hci_cmd_parm.max_celength = 12;
            p_hci_cmd_parm.min_celength = 12;

            // issue HCI cmd
            if (hci_le_conn_update_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
            {
                status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
                log_info("<CONNECTION_UPDATE> HCI TX CMD QUEUE FULL.\n");
            }
        }
        else
        {
            if ((bhc_timer_evt_get(p_param->host_id, TIMER_EVENT_CONN_PARAMETER_UPDATE_RSP) == TIMER_EVENT_NULL) &&
                    (bhc_timer_evt_get(p_param->host_id, TIMER_EVENT_CONN_UPDATE_COMPLETE) == TIMER_EVENT_NULL))
            {
                if(bhc_gap_connection_update(conn_id, &p_param->ble_conn_param) != BLE_ERR_OK)
                {
                    status = BLE_ERR_HCI_TX_DATA_QUEUE_FULL;
                    log_info("<CONNECTION_UPDATE> HCI TX DATA QUEUE FULL.\n");
                }
            }
            else
            {
                status = BLE_BUSY;
            }
        }
    } while (0);

    return status;
}

/** BLE terminate the BLE connection link.
 *
 */
ble_err_t ble_cmd_conn_terminate(uint8_t host_id)
{
    ble_err_t status;
    ble_hci_cmd_disconnect_param_t p_hci_cmd_parm;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        // check host id is valid or not
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.conn_handle = conn_id;
        p_hci_cmd_parm.reason = BLE_HCI_ERR_CODE_REMOTE_USER_TERMINATED_CONNECTION;

        // issue HCI cmd
        if (hci_disconn_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_TERMINATE> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** BLE PHY update.
 *
 */
ble_err_t ble_cmd_phy_update(ble_gap_phy_update_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_phy_param_t p_hci_cmd_parm;
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

        // check connection exist or not
        if (bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check phy parameters
        if (((p_param->tx_phy != BLE_PHY_1M) && (p_param->tx_phy != BLE_PHY_2M) && (p_param->tx_phy != BLE_PHY_CODED)) ||
                ((p_param->rx_phy != BLE_PHY_1M) && (p_param->rx_phy != BLE_PHY_2M) && (p_param->rx_phy != BLE_PHY_CODED)) ||
                ((p_param->coded_phy_option != BLE_CODED_PHY_NO_PREFERRED) && (p_param->coded_phy_option != BLE_CODED_PHY_S2) && (p_param->coded_phy_option != BLE_CODED_PHY_S8)))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.conn_handle = conn_id;
        p_hci_cmd_parm.all_phys = 0x00; // The Host has no preference among the transmitter PHYs supported by the Controller
        p_hci_cmd_parm.tx_phys = p_param->tx_phy;
        p_hci_cmd_parm.rx_phys = p_param->rx_phy;
        p_hci_cmd_parm.phy_options = p_param->coded_phy_option;

        // issue HCI cmd
        if (hci_le_set_phy_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PHY_UPDATE> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** BLE read PHY.
 *
 */
ble_err_t ble_cmd_phy_read(uint8_t host_id)
{
    ble_err_t status;
    ble_hci_cmd_read_phy_param_t p_hci_cmd_parm;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        // check host id is valid or not
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.conn_handle = conn_id;

        // issue HCI cmd
        if (hci_le_read_phy_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PHY_READ> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** BLE preferred PHY set.
 *
 */
ble_err_t ble_cmd_default_phy_set(ble_gap_default_phy_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_default_phy_param_t p_hci_cmd_parm;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (((p_param->tx_phy != BLE_PHY_1M) && (p_param->tx_phy != BLE_PHY_2M) && (p_param->tx_phy != BLE_PHY_CODED)) ||
                ((p_param->rx_phy != BLE_PHY_1M) && (p_param->rx_phy != BLE_PHY_2M) && (p_param->rx_phy != BLE_PHY_CODED)))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // issue HCI cmd
        p_hci_cmd_parm.all_phys = 0x00; // The Host has no preference among the transmitter PHYs supported by the Controller
        p_hci_cmd_parm.tx_phys = p_param->tx_phy;
        p_hci_cmd_parm.rx_phys = p_param->rx_phy;
        if (hci_le_set_default_phy_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PHY_UPDATE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** BLE read RSSI.
 *
 */
ble_err_t ble_cmd_rssi_read(uint8_t host_id)
{
    ble_err_t status;
    ble_hci_cmd_read_rssi_param_t p_hci_cmd_parm;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        // check host id is valid or not
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.conn_handle = conn_id;

        // issue HCI cmd
        if (hci_read_rssi_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<RSSI_READ> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** BLE set le host channel classification.
 *
 */
ble_err_t ble_cmd_host_ch_classif_set(ble_gap_host_ch_classif_t *p_param)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // issue HCI cmd
        if (hci_le_set_host_channel_classif_cmd((ble_hci_cmd_le_channel_classification_t *)p_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CHANNEL_CLASSIFICATION_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** BLE read channel map.
 *
 */
ble_err_t ble_cmd_channel_map_read(uint8_t host_id)
{
    ble_err_t status;
    ble_hci_cmd_le_read_channel_map_t p_hci_cmd_parm;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        // check host id is valid or not
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check connection exist or not
        if (bhc_host_id_is_connected_check(host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.conn_handle = conn_id;

        // issue HCI cmd
        if (hci_le_read_channel_map_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CHANNEL_MAP_READ> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** BLE initial resolvable address.
 *
 */
ble_err_t ble_cmd_resolvable_address_init(void)
{
    return bhc_resolvable_address_init();
}

/** BLE regenerate resolvable address.
 *
 */
ble_err_t ble_cmd_regenerate_resolvable_address(uint8_t host_id, uint8_t en_new_irk)
{
    if ((ble_adv_idle_state_check() == FALSE) || (ble_scan_idle_state_check() == FALSE) || (ble_init_idle_state_check() == FALSE))
    {
        return BLE_ERR_INVALID_STATE;
    }

    return bhc_regenerate_resolvable_address(host_id, en_new_irk);
}


/** BLE gap module event handler.
 *
 */
ble_err_t ble_evt_gap_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_GAP_EVT_SET_RANDOM_ADDR:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] SET ADDR status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_set_addr.status);
        }
        break;


    case BLE_GAP_EVT_CONN_COMPLETE:
    {
        ble_evt_gap_conn_complete_t *p_conn_param = (ble_evt_gap_conn_complete_t *)&p_evt_param->event_param.ble_evt_gap.param.evt_conn_complete;

        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            // update state
            if ((p_conn_param->role == BLE_GAP_ROLE_CENTRAL) && (ble_init_idle_state_check() == FALSE))
            {
                ble_init_status_set(STATE_IDLE);
            }
            log_info("[GAP] CONN COMPLETE status = %d\n", p_conn_param->status);
        }
    }
    break;

    case BLE_GAP_EVT_CONN_CANCEL:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] CANCEL CREATE CONN status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_create_conn.status);
        }
        break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] CONN UPDATE status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_conn_param_update.status);
        }
        break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] DISCONN COMPLETE status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_disconn_complete.status);
        }
        break;

    case BLE_GAP_EVT_PHY_READ:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] PHY READ status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_phy.status);
        }
        break;

    case BLE_GAP_EVT_DEFAULT_PHY_SET:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] Set Preferred PHY status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_default_phy.status);
        }
        break;

    case BLE_GAP_EVT_PHY_UPDATE:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] PHY COMPLETE status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_phy.status);
        }
        break;

    case BLE_GAP_EVT_RSSI_READ:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] READ RSSI status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_rssi.status);
        }
        break;

    case BLE_GAP_EVT_SET_LE_HOST_CH_CLASSIFICATION:
        break;

    case BLE_GAP_EVT_READ_CHANNEL_MAP:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] READ channel map status = %d\n", p_evt_param->event_param.ble_evt_gap.param.evt_channel_map.status);
        }
        break;

    case BLE_CTE_EVT_SET_CONN_CTE_RX_PARAM:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] Set conn cte rx param status = %d\n", p_evt_param->event_param.ble_evt_cte.param.evt_conn_cte_rx_param.status);
        }
        break;

    case BLE_CTE_EVT_SET_CONN_CTE_TX_PARAM:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] Set conn cte tx param status = %d\n", p_evt_param->event_param.ble_evt_cte.param.evt_conn_cte_tx_param.status);
        }
        break;

    case BLE_CTE_EVT_SET_CONN_CTE_REQ:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] Set conn cte req status = %d\n", p_evt_param->event_param.ble_evt_cte.param.evt_conn_cte_req.status);
        }
        break;

    case BLE_CTE_EVT_SET_CONN_CTE_RSP:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] Set conn cte rsp status = %d\n", p_evt_param->event_param.ble_evt_cte.param.evt_conn_cte_rsp.status);
        }
        break;

    case BLE_CTE_EVT_IQ_REPORT:
        // post to user
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GAP] IQ Report received\n");
        }
        break;

    default:
        break;
    }

    return status;
}
