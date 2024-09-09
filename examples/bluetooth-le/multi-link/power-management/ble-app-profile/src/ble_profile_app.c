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

/* link0 information */
ble_info_link0_t   ble_info_link0;

/* link1 information */
ble_info_link0_t   ble_info_link1;

/* BLE application links information which is order by host id. */
ble_app_link_info_t ble_app_link_info[BLE_SUPPORT_NUM_CONN_MAX] =
{
    // Link 0, host id = 0
    {
        .gap_role = BLE_GAP_ROLE_CENTRAL,
        .state = 0x00,
        .profile_info = (void *) &ble_info_link0,
    },
    // Link 1, host id = 1
    {
        .gap_role = BLE_GAP_ROLE_PERIPHERAL,
        .state = 0x00,
        .profile_info = (void *) &ble_info_link1,
    },
};

/**************************************************************************
 * Profile Application GENERAL Public Functions
 **************************************************************************/

/** Get BLE link0 Service All Handles */
ble_err_t link0_svcs_handles_get(uint8_t host_id)
{
    ble_err_t status;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    status = BLE_ERR_OK;
    do
    {
        // Get GAPS handles
        status = ble_svcs_gaps_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_gaps);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // Get GATTS handles
        status = ble_svcs_gatts_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_gatts);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // Get DIS handles
        status = ble_svcs_dis_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_dis);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // Get TRSPS handles
        status = ble_svcs_trsps_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_trsps);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

