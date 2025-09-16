/************************************************************************
 *
 * File Name  : ble_profile_app.c
 * Description: This file contains the definitions and functions of BLE profiles for application.
 *
 ************************************************************************/
#include "stdio.h"
#include "ble_profile.h"


/**************************************************************************
 * Profile Application Public Definitions and Variables
 **************************************************************************/

/* BLE Link service information */
ble_info_link_t ble_info_link[LINK_NUM];

/**************************************************************************
 * Profile Application GENERAL Public Functions
 **************************************************************************/

/** Get BLE link1 Service All Handles
*
* @attention MUST call this API to get service handles after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED if role is client
*
* @param[out] p_info : a pointer to INK0 attribute information
*
* @retval BLE_STATUS_ERR_INVALID_HOSTID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t link_svcs_handles_get(ble_info_link_t *p_info)
{
    ble_err_t status;

    // Get GAPS handles
    status = ble_svcs_gaps_handles_get(p_info->host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_info->svcs_info_gaps);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Get ATCMD handles
    status = ble_svcs_atcmd_handles_get(p_info->host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_info->svcs_info_atcmd);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    return BLE_ERR_OK;
}
