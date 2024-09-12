/**************************************************************************//**
 * @file  ble_profile.h
 * @brief Provide the declarations that for BLE Profile subsystem needed.
*****************************************************************************/

#ifndef _BLE_PROFILE_H_
#define _BLE_PROFILE_H_

#include "stdint.h"
#include "ble_common.h"
#include "ble_service_common.h"
#include "ble_service_atcmd.h"
#include "ble_service_gaps.h"
#include "atcmd_setting.h"


/**************************************************************************
 * Profile Application GENERAL Public Definitions and Functions
 **************************************************************************/
/** Define the maximum number of BLE GAPS link. */
#define MAX_NUM_CONN_GAPS      8

/** Define the maximum number of BLE ATCMD link. */
#define MAX_NUM_CONN_ATCMD     8


/**************************************************************************
 * Profile Application LINK Public Definitions and Functions
 **************************************************************************/

typedef enum
{
    STATE_STANDBY,
    STATE_ADVERTISING,
    STATE_SCANNING,
    STATE_INITIATING,
    STATE_CONNECTED,
} ble_state_t;

/** BLE Application Link 0 Profile Attribute Information Structure.*/
typedef struct ble_info_link_c
{
    uint8_t                   host_id;               /**< Host id. */
    ble_state_t               state;                 /**< Current state. */
    ble_svcs_gaps_info_t      svcs_info_gaps;        /**< GAPS information. */
    ble_svcs_atcmd_info_t     svcs_info_atcmd;       /**< ATCMD information. */
} ble_info_link_t;

typedef struct link_info_c
{
    uint8_t             host_id;
    ble_gatt_role_t     role;
} link_info_t;


/**************************************************************************
 * Global Functions
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
ble_err_t link_svcs_handles_get(ble_info_link_t *p_info);



/**************************************************************************
 * Extern Definitions
 **************************************************************************/

/** Extern maximum Number of Host Connection Link Definition. */
extern const uint8_t                              max_num_conn_host;

/** Extern BLE Connection Links Definition. */
extern const ble_att_role_by_id_t                 att_db_link[];

/** Extern BLE Connection Link Parameter Table Definition. */
extern const ble_att_db_mapping_by_id_t           att_db_mapping[];

/** Extern BLE Connection Link Mapping Size Definition. */
extern const ble_att_db_mapping_by_id_size_t      att_db_mapping_size[];

/** Extern BLE Link service information */
extern ble_info_link_t                            ble_info_link[LINK_NUM];

/** Extern BLE Link information */
extern link_info_t                                link_info[LINK_NUM];

#endif //_BLE_PROFILE_H_

