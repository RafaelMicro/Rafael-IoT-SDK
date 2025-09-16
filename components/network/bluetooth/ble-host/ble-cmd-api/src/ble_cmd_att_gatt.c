/** @file ble_cmd_att_gatt.c
 *
 * @brief Define BLE stt / gatt command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_att_gatt.h"
#include "ble_event.h"
#include "ble_host_cmd.h"
#include "ble_profile.h"
#include "hci_cmd_connect.h"
#include "log.h"


/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/** Get BLE GATT MTU Size
 */
ble_err_t ble_gatt_mtu_get(ble_gatt_get_mtu_param_t *p_param)
{
    if (p_param == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // check current BLE status first
    if (bhc_host_id_is_valid_check(p_param->host_id) == FALSE)
    {
        return BLE_ERR_INVALID_HOST_ID;
    }

    *(p_param->p_mtu) = bhc_gatt_att_mtu_get(p_param->host_id);

    return BLE_ERR_OK;
}

/** Set suggested data length.
 */
ble_err_t ble_cmd_suggest_data_len_set(uint16_t tx_octets)
{
    ble_err_t status;
    ble_hci_cmd_write_default_data_length_param_t p_hci_cmd_parm;

    status = BLE_ERR_OK;
    do {
        // check default data length size
        if ((tx_octets > BLE_GATT_DATA_LENGTH_MAX) || (tx_octets < BLE_GATT_DATA_LENGTH_MIN))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.tx_octets = tx_octets;
        p_hci_cmd_parm.tx_time = ((tx_octets + 14u) << 3u);

        // issue HCI cmd
        if (hci_le_write_suggested_default_data_length_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<PREFERRED_DATA_LENGTH_SET> HCI TX CMD QUEUE FULL.\n");
        }
    } while (0);

    return status;
}

/** Set preferred MTU size.
 */
ble_err_t ble_cmd_default_mtu_size_set(uint8_t host_id, uint16_t mtu)
{
    ble_err_t status;

    status = BLE_ERR_OK;
    do {
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }

        // check default mtu size
        if ((mtu > BLE_GATT_ATT_MTU_MAX) || (mtu < BLE_GATT_ATT_MTU_MIN))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        if (bhc_gatt_preferred_mtu_set(host_id, mtu) == FALSE)
        {
            status = BLE_ERR_INVALID_STATE;
            log_info("<PREFERRED_MTU_SET> BLE ERROR INVALID STATE.\n");
        }
    } while(0);

    return status;
}

/** ATT_MTU exchange request.
 */
ble_err_t ble_cmd_mtu_size_update(uint8_t host_id, uint16_t mtu)
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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check default mtu size
        if ((mtu > BLE_GATT_ATT_MTU_MAX) || (mtu < BLE_GATT_ATT_MTU_MIN))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // check GATT role
        if (att_db_link[host_id].p_client_db == (const ble_att_param_t **)0)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_EXCHANGE_MTU_REQUEST, 0, (uint8_t *)&mtu, 2) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<EXCHANGE_MTU_REQ> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** Set data length update.
 */
ble_err_t ble_cmd_data_len_update(uint8_t host_id, uint16_t tx_octets)
{
    ble_err_t status;
    ble_hci_cmd_set_data_length_param_t p_hci_cmd_parm;
    uint16_t conn_id;

    status = BLE_ERR_OK;
    do {
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

        // check default data length size
        if ((tx_octets > BLE_GATT_DATA_LENGTH_MAX) || (tx_octets < BLE_GATT_DATA_LENGTH_MIN))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // set HCI parameters
        p_hci_cmd_parm.conn_handle = conn_id;
        p_hci_cmd_parm.tx_octets = tx_octets;
        p_hci_cmd_parm.tx_time = ((tx_octets + 14u) << 3u);

        // issue HCI cmd
        if (hci_le_set_data_length_cmd(&p_hci_cmd_parm) != BLE_ERR_OK)
        {
            status = BLE_ERR_HCI_TX_CMD_QUEUE_FULL;
            log_info("<DATA_LENGTH_UPDATE> HCI TX CMD QUEUE FULL.\n");
        }
    } while(0);

    return status;
}

/** Get BLE GATT MTU Size
 */
ble_err_t ble_cmd_mtu_size_get(uint8_t host_id, uint16_t *mtu)
{
    ble_err_t status;
    
    status = BLE_ERR_OK;
    do {
        // check current BLE status first
        if (bhc_host_id_is_valid_check(host_id) == FALSE)
        {
            status = BLE_ERR_INVALID_HOST_ID;
            break;
        }
        *mtu = bhc_gatt_att_mtu_get(host_id);
    } while(0);

    return status;
}

/** BLE Read Response
 */
ble_err_t ble_cmd_gatt_read_rsp(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_get_mtu_param_t mtu_param;
    uint16_t conn_id, mtu_size;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if ((p_param->handle_num == 0) || (p_param->handle_num >= att_db_mapping_size[p_param->host_id].size_map_server_db))
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_server_property_value_is_match_check(p_param->host_id, p_param->handle_num, 0) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check data length -> mtuSize: 1 byte opcode other for attribute data
        // If the attribute value is longer than (ATT_MTU-1) then the first (ATT_MTU-1) octets shall be included in this response.
        p_param->length = (p_param->length > (mtu_size - 1)) ? ((mtu_size - 1)) : p_param->length;

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_READ_RESPONSE, 0, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<DATA_LENGTH_UPDATE> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Read By Type Response
 */
ble_err_t ble_cmd_gatt_read_by_type_rsp(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    uint16_t conn_id, mtu_size;
    ble_gatt_get_mtu_param_t mtu_param;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if ((p_param->handle_num == 0) || (p_param->handle_num >= att_db_mapping_size[p_param->host_id].size_map_server_db))
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_server_property_value_is_match_check(p_param->host_id, p_param->handle_num, 0) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check data length -> mtuSize: 1 byte opcode other for attribute data
        // If the attribute value is longer than (ATT_MTU-1) then the first (ATT_MTU-1) octets shall be included in this response.
        p_param->length = (p_param->length > (mtu_size - 1)) ? ((mtu_size - 1)) : p_param->length;

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_READ_BY_TYPE_RESPONSE, p_param->handle_num, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<READ_BY_TYPE_RSP> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Read Blob Response
 */
ble_err_t ble_cmd_gatt_read_blob_rsp(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_get_mtu_param_t mtu_param;
    uint16_t conn_id, mtu_size;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if ((p_param->handle_num == 0) || (p_param->handle_num >= att_db_mapping_size[p_param->host_id].size_map_server_db))
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_server_property_value_is_match_check(p_param->host_id, p_param->handle_num, 0) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check data length
        // mtuSize: 1 byte opcode other for attribute data
        if (p_param->length > (mtu_size - 1))
        {
            // If the attribute value is longer than (ATT_MTU-1) then the first (ATT_MTU-1) octets shall be included in this response.
            p_param->length = (mtu_size - 1);
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_READ_BLOB_RESPONSE, 0, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<READ_BLOB_RSP> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Error Response
 */
ble_err_t ble_cmd_gatt_error_rsp(ble_gatt_err_rsp_param_t *p_param)
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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if ((p_param->handle_num == 0) || (p_param->handle_num >= att_db_mapping_size[p_param->host_id].size_map_server_db))
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // send ATT error response
        if (bhc_att_error_rsp_req(conn_id, p_param->opcode, p_param->handle_num, p_param->err_rsp) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<ERROR_RSP> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Notification
 */
ble_err_t ble_cmd_gatt_notification(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_get_mtu_param_t mtu_param;
    uint16_t conn_id, mtu_size;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if ((p_param->handle_num == 0) || (p_param->handle_num >= att_db_mapping_size[p_param->host_id].size_map_server_db))
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_server_property_value_is_match_check(p_param->host_id, p_param->handle_num, GATT_DECLARATIONS_PROPERTIES_NOTIFY) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // Check data length < Current MTU size
        if (p_param->length > (mtu_size - 3))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_HANDLE_VALUE_NOTIFICATION, p_param->handle_num, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<NOTIFICATION> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Indication
 */
ble_err_t ble_cmd_gatt_indication(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_get_mtu_param_t mtu_param;
    uint16_t conn_id, mtu_size;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if ((p_param->handle_num == 0) || (p_param->handle_num >= att_db_mapping_size[p_param->host_id].size_map_server_db))
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_server_property_value_is_match_check(p_param->host_id, p_param->handle_num, GATT_DECLARATIONS_PROPERTIES_INDICATE) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // check GATT state
        if (bhc_host_is_wating_gatt_rsp_check(p_param->host_id) == TRUE)
        {
            status = BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // Check data length < Current MTU size
        if (p_param->length > (mtu_size - 3))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_HANDLE_VALUE_INDICATION, p_param->handle_num, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<INDICATION> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Write Request
 */
ble_err_t ble_cmd_gatt_write_req(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_get_mtu_param_t mtu_param;
    uint16_t conn_id, mtu_size;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if (p_param->handle_num == 0)
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_client_property_value_is_match_check(p_param->host_id, p_param->handle_num, GATT_DECLARATIONS_PROPERTIES_WRITE) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // check GATT state
        if (bhc_host_is_wating_gatt_rsp_check(p_param->host_id) == TRUE)
        {
            status = BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION;
            break;
        }

        // check data length < current MTU size
        if (p_param->length > (mtu_size - 3))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_WRITE_REQUEST, p_param->handle_num, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<WRITE_REQ> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE Write Command
 */
ble_err_t ble_cmd_gatt_write_cmd(ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_get_mtu_param_t mtu_param;
    uint16_t conn_id, mtu_size;

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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if (p_param->handle_num == 0)
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_client_property_value_is_match_check(p_param->host_id, p_param->handle_num, GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        // check host state
        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // get current MTU size
        mtu_param.host_id = p_param->host_id;
        mtu_param.p_mtu = &mtu_size;
        status = ble_gatt_mtu_get(&mtu_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // check data length < current MTU size
        if (p_param->length > (mtu_size - 3))
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_WRITE_COMMAND, p_param->handle_num, p_param->p_data, p_param->length) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<WRITE_CMD> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE GATT Read Request
 */
ble_err_t ble_cmd_gatt_read_req(ble_gatt_read_req_param_t *p_param)
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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if (p_param->handle_num == 0)
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_client_property_value_is_match_check(p_param->host_id, p_param->handle_num, GATT_DECLARATIONS_PROPERTIES_READ) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // check host state
        if (bhc_host_is_wating_gatt_rsp_check(p_param->host_id) == TRUE)
        {
            status = BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_READ_REQUEST, p_param->handle_num, NULL, 0) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<READ_REQ> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

/** BLE GATT Read Long Characteristic Value
 */
ble_err_t ble_cmd_gatt_read_blob_req(ble_gatt_read_blob_req_param_t *p_param)
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

        // check DB parsing is finished
        if (bhc_host_parsing_process_is_finished_check(p_param->host_id) == FALSE)
        {
            status = BLE_ERR_DB_PARSING_IN_PROGRESS;
            break;
        }

        // check handle
        if (p_param->handle_num == 0)
        {
            status = BLE_ERR_INVALID_HANDLE;
            break;
        }

        // check GATT role
        if (bhc_client_property_value_is_match_check(p_param->host_id, p_param->handle_num, GATT_DECLARATIONS_PROPERTIES_READ) == FALSE)
        {
            status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        if (bhc_host_is_in_encryption_check(p_param->host_id) == TRUE)
        {
            status = BLE_BUSY;
            break;
        }

        // check host state
        if (bhc_host_is_wating_gatt_rsp_check(p_param->host_id) == TRUE)
        {
            status = BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION;
            break;
        }

        // send ATT request
        if (bhc_att_req(conn_id, OPCODE_ATT_READ_BLOB_REQUEST, p_param->handle_num, (uint8_t *)&p_param->offset, 2) != BLE_ERR_OK)
        {
            status = BLE_BUSY;
            log_info("<READ_BLOB_REQ> BLE BUSY.\n");
        }
    } while(0);

    return status;
}

ble_err_t ble_cmd_gatt_att_handle_mapping_get(ble_gatt_handle_table_param_t *p_param)
{
    return ble_gatt_att_handle_mapping_get(p_param);
}


/** BLE ATT/GATT module event handler.
 *
 */
ble_err_t ble_evt_att_gatt_handler(void *p_param)
{
    ble_err_t status;
    ble_evt_param_t *p_evt_param = (ble_evt_param_t *)p_param;

    status = BLE_ERR_OK;
    switch (p_evt_param->event)
    {
    case BLE_ATT_GATT_EVT_DATA_LENGTH_SET:
    {
        ble_evt_data_length_set_t *p_cmd_param = (ble_evt_data_length_set_t *)&p_evt_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_set;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GATT] SET DATA LENGTH status = %d\n", p_cmd_param->status);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
    {
        ble_evt_suggest_data_length_set_t *p_cmd_param = (ble_evt_suggest_data_length_set_t *)&p_evt_param->event_param.ble_evt_att_gatt.param.ble_evt_suggest_data_length_set;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GATT] WRITE DEFAULT DATA LENGTH status = %d\n", p_cmd_param->status);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_evt_att_db_parse_complete_t *p_cmd_param = (ble_evt_att_db_parse_complete_t *)&p_evt_param->event_param.ble_evt_att_gatt.param.ble_evt_att_db_parse_complete;

        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GATT] DB PARSE COMPLETED status = %d\n", p_cmd_param->result);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_GET_ATT_HANDLES_TABLE_COMPLETE:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GATT] GET ATT HANDLES COMPLETED.\n");
        }
        break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GATT] MTU EXCHANGED\n");
        }
        break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
        status = g_ble_event_cb(p_evt_param);
        if (status == BLE_ERR_OK)
        {
            log_info("[GATT] DATA LENGTH EXCHANGED\n");
        }
        break;

    default:
        break;
    }

    return status;
}
