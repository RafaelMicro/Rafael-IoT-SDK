/** @file ble_cmd_connect_cte.c
 *
 * @brief Define BLE connection CTE command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_connect_cte.h"
#include "ble_host_cmd.h"
#include "hci_cmd_connect_cte.h"
#include "log.h"

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** BLE set connection CTE receive parameters
 *
 */
ble_err_t ble_cmd_connection_cte_receive_parameters_set(ble_connection_cte_rx_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_conn_cte_rx_param_t *p_cte_rx_param;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
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

        p_cte_rx_param = pvPortMalloc(sizeof(ble_hci_cmd_set_conn_cte_rx_param_t) + p_param->sw_pattern_length);
        p_cte_rx_param->conn_handle = conn_id;
        p_cte_rx_param->sampling_enable = p_param->sampling_enable;
        p_cte_rx_param->slot_durations = p_param->slot_durations;
        p_cte_rx_param->sw_pattern_length = p_param->sw_pattern_length;
        for (uint8_t i = 0; i < p_param->sw_pattern_length; ++i)
        {
            *(p_cte_rx_param->antenna_ids + i) = *(p_param->antenna_ids + i);
        }

        if (hci_le_set_conn_cte_rx_param_cmd(p_cte_rx_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CTE_RX_PARAMETERS_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_cte_rx_param);
    } while (0);

    return status;
}

/** BLE set connection CTE transmit parameters
 *
 */
ble_err_t ble_cmd_connection_cte_transmit_parameters_set(ble_connection_cte_tx_param_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_conn_cte_tx_param_t *p_cte_tx_param;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
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

        p_cte_tx_param = pvPortMalloc(sizeof(ble_hci_cmd_set_conn_cte_tx_param_t) + p_param->sw_pattern_length);
        p_cte_tx_param->conn_handle = conn_id;
        p_cte_tx_param->cte_types = p_param->cte_types;
        p_cte_tx_param->sw_pattern_length = p_param->sw_pattern_length;
        for (uint8_t i = 0; i < p_param->sw_pattern_length; ++i)
        {
            *(p_cte_tx_param->antenna_ids + i) = *(p_param->antenna_ids + i);
        }

        if (hci_le_set_conn_cte_tx_param_cmd(p_cte_tx_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CTE_TX_PARAMETERS_SET> HCI TX CMD QUEUE FULL.\n");
        }
        vPortFree(p_cte_tx_param);
    } while(0);

    return status;
}

/** BLE set connection request enable
 *
 */
ble_err_t ble_cmd_connection_cte_request_enable(ble_connection_cte_req_enable_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_conn_cte_req_param_t p_cte_req_param;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
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

        p_cte_req_param.conn_handle = conn_id;
        p_cte_req_param.enable = p_param->enable;
        p_cte_req_param.cte_req_interval = p_param->cte_req_interval;
        p_cte_req_param.req_cte_length = p_param->req_cte_length;
        p_cte_req_param.req_cte_type = p_param->req_cte_type;

        if (hci_le_conn_cte_req_enable_cmd(&p_cte_req_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CTE_REQ_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** BLE set connection CTE response enable
 *
 */
ble_err_t ble_cmd_connection_cte_response_enable(ble_connection_cte_rsp_enable_t *p_param)
{
    ble_err_t status;
    ble_hci_cmd_set_conn_cte_rsp_param_t p_cte_rsp_param;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
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

        p_cte_rsp_param.conn_handle = conn_id;
        p_cte_rsp_param.enable = p_param->enable;
        if (hci_le_conn_cte_rsp_enable_cmd(&p_cte_rsp_param) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<CONNECTION_CTE_RSP_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}
