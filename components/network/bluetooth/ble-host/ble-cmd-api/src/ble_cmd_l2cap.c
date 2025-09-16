/** @file ble_cmd_l2cap.c
 *
 * @brief Define BLE L2CAP command definition, structure and functions.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_api.h"
#include "ble_att_gatt.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/


/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/


/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/


/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/** Set LE l2cap credit base connect request.
 *
 */
ble_err_t ble_cmd_l2cap_chan_connect(ble_l2cap_chan_connect_t* p_param)
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

        // check mtu is legal or not
        if (p_param->mtu < BLE_GATT_ATT_MTU_MIN)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        status = bhc_l2cap_chan_connect_reg(conn_id, p_param->spsm, p_param->mtu);
    } while(0);

    return status;
}

/** Set LE l2cap credit base connect request.
 *
 */
ble_err_t ble_cmd_l2cap_data_send(ble_l2cap_data_send_t* p_param)
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

        status = bhc_l2cap_data_send(conn_id, p_param->dest_id, p_param->data, p_param->length);
    } while(0);

    return status;
}