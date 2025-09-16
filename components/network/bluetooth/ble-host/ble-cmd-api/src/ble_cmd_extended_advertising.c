/** @file ble_cmd_extended_advertising.c
 *
 * @brief Define BLE Extended advertising command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_common.h"
#include "ble_event.h"
#include "ble_host_cmd.h"
#include "ble_host_ref.h"
#include "ble_privacy.h"
#include "ble_extended_advertising.h"
#include "hci_cmd_extended_advertising.h"
#include "log.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
typedef uint8_t ble_ext_adv_state_t;
#define EXTENDED_ADV_ENABLE_PROCESSING          0x01    /**< Extended Advertising enable command is processing. */
#define EXTENDED_ADV_ENABLED                    0x02    /**< Extended Advertising is enabled. */
#define EXTENDED_ADV_DISABLE_PROCESSING         0x03    /**< Extended Advertising disable command is processing. */
#define EXTENDED_ADV_DISABLED                   0x04    /**< Extended Advertising is disabled. */

typedef uint8_t ble_padv_state_t;
#define PERIODIC_ADV_ENABLE_PROCESSING          0x01    /**< Periodic Advertising enable command is processing. */
#define PERIODIC_ADV_ENABLED                    0x02    /**< Periodic Advertising is enabled. */
#define PERIODIC_ADV_DISABLE_PROCESSING         0x03    /**< Periodic Advertising disable command is processing. */
#define PERIODIC_ADV_DISABLED                   0x04    /**< Periodic Advertising is disabled. */

typedef uint8_t ble_sync_state_t;
#define BLE_SYNC_ENABLE_PROCESSING              0x01    /**< Synchronization enable command is processing. */
#define BLE_SYNC_ENABLED                        0x02    /**< Synchronization is enabled. */
#define BLE_SYNC_DISABLE_PROCESSING             0x03    /**< Synchronization disable command is processing. */
#define BLE_SYNC_DISABLED                       0x04    /**< Synchronization is disabled. */

typedef uint8_t ble_padv_subevent_data_state_t;
#define PERIODIC_ADV_SUBEVENT_DATA_SET          0x01    /**< Synchronization enable command is processing. */
#define PERIODIC_ADV_SUBEVENT_DATA_CLEAR        0x02    /**< Synchronization is enabled. */


/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static ble_ext_adv_state_t g_ext_adv_enable[BLE_MAX_PADV_HANDLE] = {EXTENDED_ADV_DISABLED};
static ble_padv_state_t g_padv_enable[BLE_MAX_PADV_HANDLE] = {PERIODIC_ADV_DISABLED};
static ble_sync_state_t g_sync_enable[BLE_MAX_PADV_HANDLE] = {BLE_SYNC_DISABLED};

static ble_padv_subevent_data_state_t g_padv_subevent_data[BLE_MAX_PADV_HANDLE] = {PERIODIC_ADV_SUBEVENT_DATA_CLEAR};

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static ble_err_t extended_adv_status_set(ble_padv_state_t state, uint8_t adv_handle)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (adv_handle < BLE_MAX_PADV_HANDLE)
    {
        vPortEnterCritical();
        g_ext_adv_enable[adv_handle] = state;
        vPortExitCritical();
    }
    else
    {
        status = BLE_ERR_INVALID_HANDLE;
    }

    return status;
}

static ble_err_t periodic_adv_status_set(ble_padv_state_t state, uint8_t adv_handle)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (adv_handle < BLE_MAX_PADV_HANDLE)
    {
        vPortEnterCritical();
        g_padv_enable[adv_handle] = state;
        vPortExitCritical();
    }
    else
    {
        status = BLE_ERR_INVALID_HANDLE;
    }

    return status;
}

static ble_err_t synchronization_status_set(ble_sync_state_t state, uint16_t sync_handle)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (sync_handle < BLE_MAX_SYNC_HANDLE)
    {
        vPortEnterCritical();
        g_sync_enable[sync_handle] = state;
        vPortExitCritical();
    }
    else
    {
        status = BLE_ERR_INVALID_HANDLE;
    }

    return status;
}

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/** Check if the BLE extended advertising is disabled or not.
 *
 */
bool ble_extended_adv_idle_state_check(uint8_t adv_handle)
{
    if (g_ext_adv_enable[adv_handle] == PERIODIC_ADV_DISABLED)
    {
        return TRUE;
    }

    return FALSE;
}

/** Check if the BLE periodic advertising is disabled or not.
 *
 */
bool ble_padv_idle_state_check(uint8_t adv_handle)
{
    if (g_padv_enable[adv_handle] == PERIODIC_ADV_DISABLED)
    {
        return TRUE;
    }

    return FALSE;
}

/** Check if the BLE periodic advertising sync is disabled or not.
 *
 */
bool ble_sync_idle_state_check(uint16_t sync_handle)
{
    if (g_sync_enable[sync_handle] == BLE_SYNC_DISABLED)
    {
        return TRUE;
    }

    return FALSE;
}

/** Check if BLE Extended Advertisement is processing enabled.
 *
 */
bool ble_extended_adv_processing_check(void)
{
    uint8_t i;

    for (i = 0; i < BLE_MAX_PADV_HANDLE; i++)
    {
        if ((g_ext_adv_enable[i] == EXTENDED_ADV_ENABLE_PROCESSING) || 
            (g_ext_adv_enable[i] == EXTENDED_ADV_DISABLE_PROCESSING))
        {
            break;
        }
    }
    if (i == BLE_MAX_PADV_HANDLE)
    {
        return FALSE;
    }

    return TRUE;
}

/** Check if BLE Extended Advertisement is processing enabled.
 *
 */
uint8_t ble_extended_adv_processing_handle_get(void)
{
    uint8_t i;

    for (i = 0; i < BLE_MAX_PADV_HANDLE; i++)
    {
        if ((g_ext_adv_enable[i] == EXTENDED_ADV_ENABLE_PROCESSING) || 
            (g_ext_adv_enable[i] == EXTENDED_ADV_DISABLE_PROCESSING))
        {
            break;
        }
    }

    return i;
}

/** Set BLE extended advertising parameter.
 *
 */
ble_err_t ble_cmd_extended_adv_parameter_set(ble_extended_adv_param_set_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_extended_adv_param_t *p_hci_param;

    status = BLE_ERR_OK;
    do {
        if ((p_param == NULL) ||
            (p_param->adv_handle > BLE_MAX_PADV_HANDLE) ||
            (p_param->primary_adv_interval_min < BLE_EXTENDED_ADV_PRIMARY_ADV_INTERVAL_MIN) ||
            (p_param->primary_adv_interval_max < BLE_EXTENDED_ADV_PRIMARY_ADV_INTERVAL_MIN) ||
            (p_param->primary_adv_ch_map > ADV_CHANNEL_ALL) ||
            ((p_param->primary_adv_phy != BLE_EXTENDED_ADV_PRIMARY_ADV_PHY_1M) && (p_param->primary_adv_phy != BLE_EXTENDED_ADV_PRIMARY_ADV_PHY_CODED)) ||
            (p_param->secondary_adv_phy > BLE_EXTENDED_ADV_SECONDARY_ADV_PHY_CODED) ||
            (p_param->secondary_adv_phy < BLE_EXTENDED_ADV_SECONDARY_ADV_PHY_1M) ||
            (p_param->adv_sid > BLE_EXTENDED_ADV_SID_MAX) ||
            (p_param->scan_req_notification_enable > SCAN_REQ_NOTIFY_ENABLE))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check status
        if (ble_extended_adv_idle_state_check(p_param->adv_handle) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        p_hci_param = pvPortMalloc(sizeof(ble_hci_cmd_set_extended_adv_param_t));
        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_hci_param->adv_handle = p_param->adv_handle;
        p_hci_param->adv_evt_properties = p_param->adv_event_properties;
        p_hci_param->primary_adv_interval_min_l = (p_param->primary_adv_interval_min & 0xFFFF);
        p_hci_param->primary_adv_interval_min_h = ((p_param->primary_adv_interval_min >> 16) & 0xFF);
        p_hci_param->primary_adv_interval_max_l = (p_param->primary_adv_interval_max & 0xFFFF);
        p_hci_param->primary_adv_interval_max_h = ((p_param->primary_adv_interval_max >> 16) & 0xFF);
        p_hci_param->primary_adv_ch_map = p_param->primary_adv_ch_map;
        p_hci_param->own_addr_type = p_param->own_address_type;
        p_hci_param->peer_addr_type = p_param->peer_address_type;
        memcpy(p_hci_param->peer_addr, p_param->peer_address, BLE_ADDR_LEN);
        p_hci_param->adv_filter_policy = p_param->adv_filter_policy;
        p_hci_param->adv_tx_power = 0x00;
        p_hci_param->primary_adv_phy = p_param->primary_adv_phy;
        p_hci_param->secondary_adv_max_skip = p_param->secondary_adv_max_skip;
        p_hci_param->secondary_adv_phy = p_param->secondary_adv_phy;
        p_hci_param->adv_sid = p_param->adv_sid;
        p_hci_param->scan_req_notification_enable = p_param->scan_req_notification_enable;

        // issue HCI cmd
        if (hci_le_set_extended_adv_param_cmd(p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_hci_param);
    } while (0);

    return status;
}

/** Set BLE start extended advertising.
 *
 */
ble_err_t ble_cmd_extended_adv_enable_set(uint8_t host_id, ble_extended_adv_enable_set_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_extended_adv_enable_t *p_hci_param;
    extended_adv_enable_param_t *p_extended_adv_enable;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        if (p_param == NULL)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (ble_extended_adv_processing_check() == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        // check status
        if (((ble_extended_adv_idle_state_check(p_param->adv_handle) == FALSE) && (p_param->enable == 0x01)) ||
            ((ble_extended_adv_idle_state_check(p_param->adv_handle) == TRUE) && (p_param->enable == 0x00)))
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (host_id != BLE_HOSTID_RESERVED)
        {
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
        }

        p_hci_param = pvPortMalloc(sizeof(ble_hci_cmd_set_extended_adv_enable_t) + sizeof(extended_adv_enable_param_t));

        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        p_hci_param->enable = p_param->enable;
        p_hci_param->num_sets = 1;//
        p_extended_adv_enable = (extended_adv_enable_param_t *)p_hci_param->param_sets;
        p_extended_adv_enable->adv_handle = p_param->adv_handle;
        p_extended_adv_enable->duration = p_param->duration;
        p_extended_adv_enable->max_extended_adv_evt = p_param->max_extended_adv_events;

        if (host_id != BLE_HOSTID_RESERVED)
        {
            // set host id to active mode
            bhc_host_id_state_active_enhanced_set(host_id, p_param->adv_handle, PADV_ACTIVE);
        }

        // issue HCI cmd
        if (hci_le_set_extended_adv_enable_cmd(p_hci_param) != BLE_ERR_OK)
        {
            bhc_host_id_state_enhance_active_release(PADV_ACTIVE, p_param->adv_handle);
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        else
        {
            if (p_param->enable == 1)
            {
                extended_adv_status_set(EXTENDED_ADV_ENABLE_PROCESSING, p_param->adv_handle);
            }
            else if (p_param->enable == 0)
            {
                extended_adv_status_set(EXTENDED_ADV_DISABLE_PROCESSING, p_param->adv_handle);
            }
        }
        vPortFree(p_hci_param);
    } while (0);

    return status;
}

/** Set BLE extended advertising data.
 *
 */
ble_err_t ble_cmd_extended_adv_data_set(ble_extended_adv_data_set_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_extended_adv_data_t *p_hci_param;

    status = BLE_ERR_OK;
    do {
        p_hci_param = pvPortMalloc(sizeof(ble_hci_cmd_set_extended_adv_data_t) + p_param->adv_data_len);
        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_hci_param->adv_handle = p_param->adv_handle;
        p_hci_param->operation = 0x03;//
        p_hci_param->frag_preference = 0x01;
        p_hci_param->adv_data_len = p_param->adv_data_len;
        memcpy(p_hci_param->adv_data, p_param->adv_data, p_param->adv_data_len);

        // issue HCI cmd
        if (hci_le_set_extended_adv_data_cmd(p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_hci_param);
    } while (0);


    return status;
}

/** Set BLE advertising set random address.
 *
 */
ble_err_t ble_cmd_adv_set_random_addr_set(ble_adv_set_random_address_set_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_adv_set_random_addr_t hci_param;

    status = BLE_ERR_OK;

    do {

        if (p_param->adv_handle > BLE_MAX_PADV_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
    
        hci_param.adv_handle = p_param->adv_handle;
        memcpy(hci_param.random_addr, p_param->addr, BLE_ADDR_LEN);

        // issue HCI cmd
        if (hci_le_set_adv_set_random_addr_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);
    return status;
}

/** Set BLE periodic advertising parameters.
 *
 */
ble_err_t ble_cmd_default_periodic_adv_sync_transfer_param_set(ble_default_periodic_adv_sync_transfer_param_t *p_param)
{
    ble_err_t status;
    ble_hci_default_periodic_adv_sync_transfer_t hci_param;

    status = BLE_ERR_OK;

    do {

        if (p_param->mode > PERIODIC_ADV_EVENT_WITH_DUPLICATE_FILTER)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (p_param->skip > BLE_PERIODIC_ADV_PKT_SKIP_MAX)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if ((p_param->sync_timeout < BLE_PERIODIC_ADV_SYNC_TIMEOUT_MIN) ||
            (p_param->sync_timeout > BLE_PERIODIC_ADV_SYNC_TIMEOUT_MAX))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        hci_param.mode = p_param->mode;
        hci_param.skip = p_param->skip;
        hci_param.sync_timeout = p_param->sync_timeout;
        hci_param.cte_type = 0;
        // issue HCI cmd
        if (hci_le_default_periodic_adv_sync_transfer_param_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
            break;
        }

    }while(0);
    return status;
}

/** Set BLE periodic advertising sync transfer parameters.
 *
 */
ble_err_t ble_cmd_periodic_adv_sync_transfer_param_set(ble_periodic_adv_sync_transfer_param_t *p_param)
{
    ble_err_t status;
    ble_hci_set_periodic_adv_sync_transfer_param_t hci_param;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
        if (bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        if (p_param->mode > PERIODIC_ADV_EVENT_WITH_DUPLICATE_FILTER)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (p_param->skip > BLE_PERIODIC_ADV_PKT_SKIP_MAX)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if ((p_param->sync_timeout < BLE_PERIODIC_ADV_SYNC_TIMEOUT_MIN) ||
            (p_param->sync_timeout > BLE_PERIODIC_ADV_SYNC_TIMEOUT_MAX))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        hci_param.conn_handle = conn_id;
        hci_param.mode = p_param->mode;
        hci_param.skip = p_param->skip;
        hci_param.sync_timeout = p_param->sync_timeout;
        hci_param.cte_type = 0;
        // issue HCI cmd
        if (hci_le_set_periodic_adv_sync_transfer_param_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        else
        {
            // set to local parameter
        }
    } while(0);

    return status;
}

/** Set BLE periodic sync subevent.
 *
 */
ble_err_t ble_cmd_periodic_sync_subevent_set(ble_periodic_sync_subevent_param_t *p_param)
{
    ble_err_t status;
    ble_hci_set_periodic_sync_subevent_t *p_hci_param;

    status = BLE_ERR_OK;
    do {

        if (p_param->sync_handle > BLE_MAX_SYNC_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
    
        p_hci_param = pvPortMalloc(sizeof(ble_hci_set_periodic_sync_subevent_t) + sizeof(uint8_t));
        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        p_hci_param->sync_handle = p_param->sync_handle;
        p_hci_param->periodic_adv_properties = 0;
        p_hci_param->num_subevent = 1;
        p_hci_param->subevent[0] = p_param->subevent;
        // issue HCI cmd
        if (hci_le_set_periodic_sync_subevent_cmd(p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_hci_param);
    } while(0);

    return status;
}

/** Set BLE stop reception of the periodic advertising train.
 *
 */
ble_err_t ble_cmd_periodic_advertising_sync_terminate(ble_periodic_advertising_sync_terminate_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_hci_cmd_periodic_adv_terminate_sync_param_t hci_param;

    status = BLE_ERR_OK;
    do {

        if (p_param->sync_handle > BLE_MAX_SYNC_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (ble_sync_idle_state_check(p_param->sync_handle) == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        host_id = bhc_host_id_state_enhance_active_get(SYNC_ACTIVE, p_param->sync_handle);

        if (host_id == BLE_HOSTID_RESERVED)
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }
        hci_param.sync_handle = p_param->sync_handle;

        // issue HCI cmd
        if (hci_le_set_periodic_terminate_sync_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<ADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
            break;
        }
        else
        {
            bhc_host_id_state_enhance_active_release(SYNC_ACTIVE, p_param->sync_handle);
        }
    } while(0);

    return status;
}

/** Set BLE start receive periodic advertising.
 *
 */
ble_err_t ble_cmd_periodic_adv_receive_enable(uint16_t sync_handle)
{
    ble_err_t status;
    ble_hci_set_periodic_adv_receive_en_param_t hci_param;

    status = BLE_ERR_OK;
    do {

        if (sync_handle > BLE_MAX_SYNC_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        // check status
        if (ble_sync_idle_state_check(sync_handle) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        hci_param.sync_handle = sync_handle;
        hci_param.enable = 1;

        // issue HCI cmd
        if (hci_le_set_periodic_adv_receive_enable_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_RECEIVE_ENABLE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Set BLE stop receive periodic advertising.
 *
 */
ble_err_t ble_cmd_periodic_adv_receive_disable(uint16_t sync_handle)
{
    ble_err_t status;
    ble_hci_set_periodic_adv_receive_en_param_t hci_param;

    status = BLE_ERR_OK;
    do {

        if (sync_handle > BLE_MAX_SYNC_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check status
        if (ble_sync_idle_state_check(sync_handle) == TRUE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        hci_param.sync_handle = sync_handle;
        hci_param.enable = 0;
        // issue HCI cmd
        if (hci_le_set_periodic_adv_receive_enable_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_RECEIVE_DISABLE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Set BLE periodic advertising response data.
 *
 */
ble_err_t ble_cmd_periodic_adv_rsp_data_set(ble_periodic_adv_rsp_data_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_set_periodic_adv_rsp_data_t *p_hci_param;

    status = BLE_ERR_OK;

    do {

        if (p_param->sync_handle > BLE_MAX_SYNC_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (p_param->rsp_data_len > BLE_PERIODIC_ADV_RSP_DATA_LEN_MAX)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        p_hci_param = pvPortMalloc(sizeof(ble_hci_set_periodic_adv_rsp_data_t) + p_param->rsp_data_len);

        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
 
        p_hci_param->sync_handle = p_param->sync_handle;
        p_hci_param->req_event = p_param->req_event;
        p_hci_param->req_subevent = p_param->req_subevent;
        p_hci_param->rsp_subevent = p_param->rsp_subevent;
        p_hci_param->rsp_slot = p_param->rsp_slot;
        p_hci_param->rsp_len = p_param->rsp_data_len;
        memcpy(p_hci_param->rsp_data, p_param->rsp_data, p_param->rsp_data_len);
        // issue HCI cmd
        if (hci_le_set_periodic_adv_rsp_data_cmd(p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_RSP_DATA_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_hci_param);
    } while(0);

    return status;
}

/** Set BLE periodic advertising parameters.
 *
 */
ble_err_t ble_cmd_periodic_adv_parameter_set(ble_periodic_adv_param_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_set_periodic_adv_param_v2_t hci_param;

    status = BLE_ERR_OK;
    do {

        
        if ((p_param->adv_handle > BLE_MAX_PADV_HANDLE) ||
            (p_param->padv_interval_min < BLE_PERIODIC_ADV_INTERVAL_MIN) ||
            (p_param->padv_interval_max < BLE_PERIODIC_ADV_INTERVAL_MIN) ||
            (p_param->num_subevents > BLE_PERIODIC_ADV_SUBEVENT_NUM_MAX) ||
            (p_param->subevent_interval < BLE_PERIODIC_ADV_SUBEVENT_INTERVAL_MIN))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        hci_param.adv_handle = p_param->adv_handle;
        hci_param.periodic_adv_interval_min = p_param->padv_interval_min;
        hci_param.periodic_adv_interval_max = p_param->padv_interval_max;
        hci_param.periodic_adv_properties = p_param->padv_properties;
        hci_param.num_subevents = p_param->num_subevents;
        hci_param.subevent_interval = p_param->subevent_interval;
        hci_param.rsp_slot_delay = p_param->rsp_slot_delay;
        hci_param.rsp_slot_spacing = p_param->rsp_slot_spacing;
        hci_param.num_rsp_slots = p_param->num_rsp_slot;

    // issue HCI cmd
    if (hci_le_set_periodic_adv_param_v2_cmd(&hci_param) != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<PADV_PARAM_SET> HCI TX CMD QUEUE FULL.\n");
    }

    } while(0);

    return status;
}

/** Set BLE start periodic advertising.
 *
 */
ble_err_t ble_cmd_periodic_adv_enable_set(ble_periodic_adv_enable_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_periodic_adv_enable_t hci_param;

    status = BLE_ERR_OK;

    if (p_param->adv_handle > BLE_MAX_PADV_HANDLE)
    {
        status = BLE_ERR_INVALID_PARAMETER;
    }
    else
    {
        hci_param.adv_handle = p_param->adv_handle;
        hci_param.enable = 0x01;

        // issue HCI cmd
        if (hci_le_set_periodic_adv_enable_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_ENABLE_SET> HCI TX CMD QUEUE FULL.\n");
        }
    }
    return status;
}

/** Set BLE stop periodic advertising.
 *
 */
ble_err_t ble_cmd_periodic_adv_disable_set(ble_periodic_adv_disable_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_periodic_adv_enable_t hci_param;    

    status = BLE_ERR_OK;

    if (p_param->adv_handle > BLE_MAX_PADV_HANDLE)
    {
        status = BLE_ERR_INVALID_PARAMETER;
    }
    else
    {
        hci_param.adv_handle = p_param->adv_handle;
        hci_param.enable = 0x00;

        // issue HCI cmd
        if (hci_le_set_periodic_adv_enable_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_DISABLE_SET> HCI TX CMD QUEUE FULL.\n");
        }
    }
    return status;
}

/** Set BLE periodic advertising data.
 *
 */
ble_err_t ble_cmd_periodic_adv_data_set(ble_periodic_adv_data_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_periodic_adv_data_t *p_hci_param;

    status = BLE_ERR_OK;
    do {

        if ((p_param->adv_handle > BLE_MAX_PADV_HANDLE) ||
            (p_param->adv_data_len > BLE_PERIODIC_ADV_DATA_LEN_MAX))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        p_hci_param = pvPortMalloc(sizeof(ble_hci_cmd_set_periodic_adv_data_t)+p_param->adv_data_len);

        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
  
        p_hci_param->adv_handle = p_param->adv_handle;
        p_hci_param->operation = 0x03; //complete periodic advertising data
        p_hci_param->adv_data_len = p_param->adv_data_len;
        memcpy(p_hci_param->adv_data, p_param->adv_data, p_param->adv_data_len);

        // issue HCI cmd
        if (hci_le_set_periodic_adv_data_cmd(p_hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_DATA_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_hci_param);
    } while(0);

    return status;
}

/** Transfer the BLE periodic advertising set information.
 *
 */
ble_err_t ble_cmd_periodic_adv_set_info_transfer_set(ble_periodic_adv_set_info_transfer_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_periodic_adv_set_info_transfer_param_t hci_param;
    uint16_t conn_id;

    status = BLE_ERR_OK;

    do {
        if(bhc_host_id_is_valid_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        if (bhc_host_id_is_connected_check(p_param->host_id, &conn_id) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (p_param->adv_handle > BLE_MAX_PADV_HANDLE)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        hci_param.adv_handle = p_param->adv_handle;
        hci_param.service_data = p_param->service_data;; //
        hci_param.conn_handle = conn_id;

        // issue HCI cmd
        if (hci_le_periodic_adv_set_info_transfer_cmd(&hci_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PADV_DATA_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Set BLE periodic advertising subevent data.
 *
 */
ble_err_t ble_cmd_periodic_adv_subevent_data_set(ble_periodic_adv_subevent_data_set_param_t *p_param)
{
    ble_err_t status;
    ble_hci_set_periodic_adv_subevent_data_t *p_hci_param;
    periodic_adv_subevent_param_t *p_subevent_parm;
    ble_hci_subevent_param_t *p_subevent_parm_tx;
    uint8_t parse_len, fill_len, i;

    status = BLE_ERR_OK;

    do {
        parse_len = 0;
        fill_len = 0;

        if ((p_param->num_subevents < 0x01) || 
            (p_param->num_subevents > 0x0F) ||
            (p_param->adv_handle > BLE_MAX_PADV_HANDLE))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (g_padv_subevent_data[p_param->adv_handle] == PERIODIC_ADV_SUBEVENT_DATA_SET)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        for (i = 0; i < p_param->num_subevents; i++)
        {
            p_subevent_parm = (periodic_adv_subevent_param_t*)(p_param->subevents + parse_len);

            if (p_subevent_parm->subevent_data_len > 251)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            parse_len += (p_subevent_parm->subevent_data_len + sizeof(periodic_adv_subevent_param_t));
            fill_len += (p_subevent_parm->subevent_data_len + sizeof(ble_hci_subevent_param_t));
        } 

        if (status != BLE_ERR_OK)
        {
            break;
        }
        p_hci_param = pvPortMalloc(sizeof(ble_hci_set_periodic_adv_subevent_data_t) + fill_len);

        if (p_hci_param == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        p_hci_param->adv_handle = p_param->adv_handle;
        p_hci_param->num_subevents = p_param->num_subevents;
        parse_len = 0;
        fill_len = 0;

        for (i = 0; i < p_param->num_subevents; i++)
        {
            p_subevent_parm = (periodic_adv_subevent_param_t*)(p_param->subevents + parse_len);
            p_subevent_parm_tx = (ble_hci_subevent_param_t*)((uint8_t*)(p_hci_param->subevent) + fill_len);

            p_subevent_parm_tx->subevent = p_subevent_parm->subevent;
            p_subevent_parm_tx->rsp_slot_start = p_subevent_parm->rsp_slot_start;
            p_subevent_parm_tx->rsp_slot_count = p_subevent_parm->rsp_slot_count;
            p_subevent_parm_tx->subevent_data_len = p_subevent_parm->subevent_data_len;

            memcpy(p_subevent_parm_tx->subevent_data, p_subevent_parm->subevent_data, p_subevent_parm->subevent_data_len);
            parse_len += (p_subevent_parm->subevent_data_len + sizeof(periodic_adv_subevent_param_t));
            fill_len += (p_subevent_parm->subevent_data_len + sizeof(ble_hci_subevent_param_t));
        }

        // issue HCI cmd
        status = hci_le_set_periodic_adv_subevent_data_cmd(p_hci_param);
        if (status != BLE_ERR_OK)
        {
            log_info("<PADV_DATA_SET> HCI TX CMD FAIL %d.\n", status);
        }
        else
        {
            g_padv_subevent_data[p_param->adv_handle] = PERIODIC_ADV_SUBEVENT_DATA_SET;
        }
        vPortFree(p_hci_param);
    } while(0);

    return status;
}


/** BLE periodic advertising module event handler.
 *
 */
ble_err_t ble_evt_periodic_adv_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_PADV_EVT_SET_PADV_RECEIVE_ENABLE:
    {
        ble_evt_padv_set_padv_receive_enable_t *p_padv_receive_enable = (ble_evt_padv_set_padv_receive_enable_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_set_padv_receive_enable;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] SET PADV receive enable status = %d\n", p_padv_receive_enable->status);
        }
    }
    break;

    case BLE_PADV_EVT_SET_DEFAULT_PADV_SYNC_TRANSFER_PARAM:
    {
        ble_evt_padv_set_default_padv_sync_transfer_param_t *p_default_padv_sync_transfer_param = (ble_evt_padv_set_default_padv_sync_transfer_param_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_set_default_padv_sync_transfer_param;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] SET PADV default transfer PARAM status = %d\n", p_default_padv_sync_transfer_param->status);
        }
    }
    break;

     case BLE_PADV_EVT_SET_PADV_SYNC_TRANSFER_PARAM:
    {
        ble_evt_padv_set_padv_sync_transfer_param_t *p_padv_sync_transfer_param = (ble_evt_padv_set_padv_sync_transfer_param_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_set_padv_sync_transfer_param;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] SET PADV transfer PARAM status = %d\n", p_padv_sync_transfer_param->status);
        }
    }
    break;

     case BLE_PADV_EVT_PADV_SYNC_TRANSFER_RECEIVED_V2:
    {
        ble_evt_padv_padv_sync_transfer_received_v2_t *p_padv_sync_transfer_rcvd_v2 = (ble_evt_padv_padv_sync_transfer_received_v2_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_sync_transfer_received_v2;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            if (p_padv_sync_transfer_rcvd_v2->status == BLE_HCI_ERR_CODE_SUCCESS)
            {
                synchronization_status_set(BLE_SYNC_ENABLED, p_padv_sync_transfer_rcvd_v2->sync_handle);
            }
            log_info("[ADV] SET PADV transfer PARAM status = %d\n", p_padv_sync_transfer_rcvd_v2->status);
        }
    }
    break;

    case BLE_PADV_EVT_PADV_REPORT_V2:
    {
        ble_evt_padv_padv_report_v2_t *p_padv_report_v2 = (ble_evt_padv_padv_report_v2_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_report_v2;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[ADV] padv report\n");
        }
    }
    break;

    case BLE_PADV_EVT_PADV_SYNC_LOST:
    {
        ble_evt_padv_padv_sync_lost_t *p_padv_sync_lost = (ble_evt_padv_padv_sync_lost_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_sync_lost;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            synchronization_status_set(BLE_SYNC_DISABLED, p_padv_sync_lost->sync_handle);
            log_info("[ADV] sync lost : %d\n", p_padv_sync_lost->sync_handle);
        }
    }
    break;

    case BLE_PADV_EVT_SET_PADV_PARAM:
    {
        ble_evt_padv_padv_params_set_t *p_padv_param_set = (ble_evt_padv_padv_params_set_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_params_set;

        status = g_ble_event_cb(p_evt_param);
        log_info("PADV Parameter set %d\n", p_padv_param_set->status);
    }
    break;

    case BLE_PADV_EVT_SET_PADV_DATA:
    {
        ble_evt_padv_padv_data_set_t *p_padv_param_set = (ble_evt_padv_padv_data_set_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_data_set;

        status = g_ble_event_cb(p_evt_param);
        log_info("PADV Data set %d\n", p_padv_param_set->status);
    }
    break;

    case BLE_PADV_EVT_PADV_SUBEVENT_DATA_REQ:
    {
        ble_evt_padv_padv_subevent_data_req_t *p_padv_subevt_data_req = (ble_evt_padv_padv_subevent_data_req_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_subevent_data_req;

        g_padv_subevent_data[p_padv_subevt_data_req->adv_handle] = PERIODIC_ADV_SUBEVENT_DATA_CLEAR;
        status = g_ble_event_cb(p_evt_param);
        log_info("subevent data req, adv_handle %d\n", p_padv_subevt_data_req->adv_handle);
    }
    break;

    case BLE_PADV_EVT_PADV_RESPONSE_REPORT:
    {
        ble_evt_padv_padv_rsp_report_t *p_padv_rsp_report = (ble_evt_padv_padv_rsp_report_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_rsp_report;

        status = g_ble_event_cb(p_evt_param);
        log_info("PADV response report, adv_handle %d\n", p_padv_rsp_report->adv_handle);

    }
    break;

    case BLE_PADV_EVT_ENABLE_PADV:
    {
        ble_evt_padv_padv_enable_set_t *p_padv_param_set = (ble_evt_padv_padv_enable_set_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_padv_enable_set;

        status = g_ble_event_cb(p_evt_param);
        log_info("PADV enable %d\n", p_padv_param_set->status);
    }
    break;

    case BLE_EADV_EVT_ENABLE_ADV:
    {
        ble_evt_extended_adv_enable_t *p_adv_enable = (ble_evt_extended_adv_enable_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_set_extended_adv_enable;
        uint8_t adv_handle;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            adv_handle = ble_extended_adv_processing_handle_get();
            if (adv_handle < BLE_MAX_PADV_HANDLE)
            {
                if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
                {
                    if (g_ext_adv_enable[adv_handle] == EXTENDED_ADV_ENABLE_PROCESSING)
                    {
                        extended_adv_status_set(EXTENDED_ADV_ENABLED, adv_handle);
                    }
                    else if (g_ext_adv_enable[adv_handle] == EXTENDED_ADV_DISABLE_PROCESSING)
                    {
                        // update host id state
                        bhc_host_id_state_enhance_active_release(PADV_ACTIVE, adv_handle);
                        extended_adv_status_set(EXTENDED_ADV_DISABLED, adv_handle);
                    }
                }
                else
                {
                    // recover to last state
                    if (g_ext_adv_enable[adv_handle] == EXTENDED_ADV_ENABLE_PROCESSING)
                    {
                        extended_adv_status_set(EXTENDED_ADV_DISABLED, adv_handle);
                    }
                    else if (g_ext_adv_enable[adv_handle] == EXTENDED_ADV_DISABLE_PROCESSING)
                    {
                        extended_adv_status_set(EXTENDED_ADV_ENABLED, adv_handle);
                    }
                }
            }
            log_info("Extended ADV enable %d\n", p_adv_enable->status);
        }
    }
    break;

    case BLE_EADV_EVT_ADV_SET_TERMINATED:
    {
        ble_evt_adv_set_terminated_t *p_adv_set_terminated = (ble_evt_adv_set_terminated_t *)&p_evt_param->event_param.ble_evt_padv.param.evt_adv_set_terminated;

        status = g_ble_event_cb(p_evt_param);
        log_info("ADV set terminated status %d\n", p_adv_set_terminated->status);
        extended_adv_status_set(EXTENDED_ADV_DISABLED, p_adv_set_terminated->adv_handle);
    }
    break;

    default:
        break;
    }

    return status;
}
