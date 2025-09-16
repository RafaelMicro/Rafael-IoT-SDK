/** @file ble_cmd_common.c
 *
 * @brief Define BLE common definition, structure and functions.
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
#include "ble_scan.h"
#include "flashctl.h"
#include "hci_cmd_vendor.h"
#include "hci_cmd_controller.h"
#include "log.h"


/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** BLE Set PHY controller initialization function.
*/
ble_err_t ble_cmd_phy_controller_init(void)
{
    ble_err_t status;
    ble_hci_vcmd_cntlr_info_param_t hci_cmd_parm;

    status = BLE_ERR_OK;
    do {
        // set HCI parameters
        memcpy(hci_cmd_parm.ble_public_addr, g_ble_default_public_addr, BLE_ADDR_LEN);
        hci_cmd_parm.ble_version = BLE_STACK_VERSION;
        hci_cmd_parm.ble_company_id = BLE_COMPANY_ID;

        // set vendor controller information
        if (hci_vendor_set_controller_info_cmd(&hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONTROLLER_INIT> HCI TX CMD QUEUE FULL.\n");
            break;
        }

        // set BLE event mask
        if (hci_le_set_event_mask_cmd((uint8_t *)&g_le_event_mask) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONTROLLER_INIT> HCI TX CMD QUEUE FULL.\n");
            break;
        }

        // get local version
        if (hci_read_local_ver_info_cmd() != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONTROLLER_INIT> HCI TX CMD QUEUE FULL.\n");
            break;
        }

        // get buffer size
        if (hci_le_read_buffer_size_cmd() != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONTROLLER_INIT> HCI TX CMD QUEUE FULL.\n");
            break;
        }
    } while(0); 

    return status;
}

/** Read the unique code of BLE mac address and IRK
*/
ble_err_t ble_cmd_read_unique_code(ble_unique_code_format_t *p_param)
{
    ble_err_t status;
    uint8_t   *p_read_param;
    uint8_t   *p_cmp;

    status = BLE_ERR_OK;
    p_read_param = pvPortMalloc(256);
    do
    {
        if (p_read_param != NULL)
        {
            p_cmp = pvPortMalloc(sizeof(ble_unique_code_format_t));
            if (p_cmp != NULL)
            {
                flash_read_sec_register((uint32_t)p_read_param, SEC_BLE_MAC_REG);
                while (flash_check_busy());

                memset(p_cmp, 0, sizeof(ble_unique_code_format_t));
                if (memcmp(p_read_param, p_cmp, sizeof(ble_unique_code_format_t)) == 0)
                {
                    status = BLE_ERR_INVALID_PARAMETER;
                    vPortFree(p_read_param);
                    vPortFree(p_cmp);
                    break;
                }
                memset(p_cmp, 0xFF, sizeof(ble_unique_code_format_t));
                if (memcmp(p_read_param, p_cmp, sizeof(ble_unique_code_format_t)) == 0)
                {
                    status = BLE_ERR_INVALID_PARAMETER;
                    vPortFree(p_read_param);
                    vPortFree(p_cmp);
                    break;
                }
                memcpy(p_param, p_read_param, sizeof(ble_unique_code_format_t));
                vPortFree(p_read_param);
                vPortFree(p_cmp);
            }
            else
            {
                vPortFree(p_read_param);
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    } while (0);

    return status;
}

/** Read filter accept list size
*/
ble_err_t ble_cmd_read_filter_accept_list_size(void)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (hci_le_read_filter_accept_list_size_cmd() != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<READ_FILTER_ACCEPT_LIST_SIZE> HCI TX CMD QUEUE FULL.\n");
    }

    return status;
}

/** Clear filter accept list
*/
ble_err_t ble_cmd_clear_filter_accept_list(void)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (hci_le_clear_filter_accept_list_cmd() != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<CLEAR_FILTER_ACCEPT_LIST> HCI TX CMD QUEUE FULL.\n");
    }

    return status;
}

/** Add device to filter accept list
*/
ble_err_t ble_cmd_add_device_to_accept_list(ble_filter_accept_list_t *p_accept_list)
{
    ble_err_t status;

    // check init state
    if (ble_init_idle_state_check() == FALSE)
    {
        return BLE_ERR_INVALID_STATE;
    }

    if (p_accept_list == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // check if in advertising state
    if (ble_adv_idle_state_check() == FALSE)
    {
        return BLE_ERR_INVALID_STATE;
    }

    // check if in scan state
    if (ble_scan_idle_state_check() == FALSE)
    {
        return BLE_ERR_INVALID_STATE;
    }

    status = BLE_ERR_OK;
    if (hci_le_add_device_to_filter_accept_list_cmd((ble_hci_cmd_add_device_to_accept_list_t *)p_accept_list) != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<ADD_DEVICE_TO_FILTER_ACCEPT_LIST> HCI TX CMD QUEUE FULL.\n");
    }

    return status;
}

/** Remove device from filter accept list
*/
ble_err_t ble_cmd_remove_device_from_accept_list(ble_filter_accept_list_t *p_accept_list)
{
    ble_err_t status;

    // check init state
    if (ble_init_idle_state_check() == FALSE)
    {
        return BLE_ERR_INVALID_STATE;
    }

    if (p_accept_list == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // check if in advertising state
    if (ble_adv_idle_state_check() == FALSE)
    {
        return BLE_ERR_INVALID_STATE;
    }

    // check if in scan state
    if (ble_scan_idle_state_check() == FALSE)
    {
        return BLE_ERR_INVALID_STATE;
    }

    status = BLE_ERR_OK;
    if (hci_le_remove_device_from_filter_accept_list_cmd((ble_hci_cmd_remove_device_from_accept_list_t *)p_accept_list) != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST> HCI TX CMD QUEUE FULL.\n");
    }

    return status;
}

ble_err_t ble_cmd_antenna_info_read(void)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    if (hci_le_read_antenna_info_cmd() != BLE_ERR_OK)
    {
        status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
        log_info("<ANTENNA_INFO_READ> HCI TX CMD QUEUE FULL.\n");
    }

    return status;
}

/** BLE set scan report function.
*/
ble_err_t ble_vendor_scan_req_report_set(ble_vendor_scan_req_rpt_t *p_param)
{
    ble_vendor_scan_req_rpt_t *p_rpt_param = (ble_vendor_scan_req_rpt_t *)p_param;
    ble_err_t status = BLE_ERR_OK;

    // issue HCI cmd
    if (hci_vendor_set_scan_request_report_cmd((ble_hci_vcmd_scan_req_rpt_param_t *)p_rpt_param) != BLE_ERR_OK)
    {
        status = BLE_ERR_SENDTO_FAIL;
    }

    return status;
}


/** BLE common module event handler.
 *
 */
ble_err_t ble_evt_common_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_COMMON_EVT_SET_CONTROLLER_INFO:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] SET CONTLR INFO status = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_set_cntlr_info.status);
        }
        break;

    case BLE_COMMON_EVT_SET_EVENT_MASK:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] SET EVENT MASK status = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_set_event_mask.status);
        }
        break;

    case BLE_COMMON_EVT_READ_LOCAL_VER:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] READ VER status = %d, hci_revision = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_read_local_ver.status, p_evt_param->event_param.ble_evt_common.param.evt_read_local_ver.hci_revision);
        }
        break;

    case BLE_COMMON_EVT_READ_BUFFER_SIZE:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] READ BUFFER status = %d size = %d \n", p_evt_param->event_param.ble_evt_common.param.evt_read_buffer_size.status, p_evt_param->event_param.ble_evt_common.param.evt_read_buffer_size.total_num_data_packet);
        }
        break;

    case BLE_COMMON_EVT_READ_FILTER_ACCEPT_LIST_SIZE:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] READ Filter accept list size status = %d size = %d \n", p_evt_param->event_param.ble_evt_common.param.evt_read_accept_list_size.status, p_evt_param->event_param.ble_evt_common.param.evt_read_accept_list_size.filter_accept_list_size);
        }
        break;

    case BLE_COMMON_EVT_CLEAR_FILTER_ACCEPT_LIST:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] Clear Filter accept list status = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_clear_accept_list.status);
        }
        break;

    case BLE_COMMON_EVT_ADD_FILTER_ACCEPT_LIST:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] Add Filter accept list status = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_add_accept_list.status);
        }
        break;

    case BLE_COMMON_EVT_REMOVE_FILTER_ACCEPT_LIST:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] Remove Filter accept list status = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_remove_accept_list.status);
        }
        break;

    case BLE_COMMON_EVT_READ_ANTENNA_INFO:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[COMMON] Read antenna info status = %d\n", p_evt_param->event_param.ble_evt_common.param.evt_read_antenna_info.status);
        }
        break;

    default:
        break;
    }

    return status;
}

/** BLE common module event handler.
 *
 */
ble_err_t ble_evt_vendor_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_VENDOR_EVT_SCAN_REQ_REPORT:
        status = g_ble_event_cb(p_evt_param);
        break;

    default:
        break;
    }

    return status;
}
