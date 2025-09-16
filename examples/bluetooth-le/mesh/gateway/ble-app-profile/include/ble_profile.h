/**************************************************************************//**
* @file       ble_profile.h
* @brief      Provide the declarations that for BLE Profile subsystem needed.
*
*****************************************************************************/

#ifndef _BLE_PROFILE_H_
#define _BLE_PROFILE_H_

#include <stdint.h>
#include "ble_common.h"
#include "ble_att_gatt.h"
#include "ble_service_common.h"
#include "ble_service_mesh_proxy.h"
#include "ble_service_mesh_prov.h"
#include "ble_service_trsps.h"
#include "ble_service_fotas.h"

/**************************************************************************
 * Profile Application GENERAL Public Definitions and Functions
 **************************************************************************/
/** Define the maximum number of BLE GAP service link. */
#define MAX_NUM_CONN_GAP            1

/** Define the maximum number of BLE GATT service link. */
#define MAX_NUM_CONN_GATT           1

/** Define the maximum number of BLE DIS service link. */
#define MAX_NUM_CONN_DIS            1

/** Define the maximum number of BLE UDF01 service link. */
#define MAX_NUM_CONN_UDF01S         1

/** Define the maximum number of BLE FOTAS link. */
#define MAX_NUM_CONN_FOTAS          3

/** Define the maximum number of BLE UDF01 service link. */
#define MAX_NUM_CONN_TRSPS          3
/**************************************************************************
 * Profile Application LINK HOST ID = 0 Public Definitions and Functions
 **************************************************************************/
/** BLE Application Link 0 Profile Attribute Information Structure.
*/
typedef struct ble_info_link0_s
{
    uint8_t                     host_id;                  /**< Host id. */
    uint8_t                     state;                    /**< Current state. */
    ble_svcs_mesh_prov_info_t   svcs_info_mesh_prov_s;    /**< mesh provisioning service information (server). */
    ble_svcs_trsps_info_t       svcs_info_trsps;           /**< TRSPS information (server). */
    ble_svcs_fotas_info_t       svcs_info_fotas;          /**< FOTAS information (server). */
} ble_info_link0_t;

typedef struct ble_info_link1_s
{
    uint8_t                     host_id;                   /**< Host id. */
    uint8_t                     state;                     /**< Current state. */
    ble_svcs_mesh_proxy_info_t  svcs_info_mesh_proxy_s;    /**< mesh proxy service information (server). */
    ble_svcs_trsps_info_t       svcs_info_trsps;           /**< TRSPS information (server). */
    ble_svcs_fotas_info_t       svcs_info_fotas;           /**< FOTAS information (server). */
} ble_info_link1_t;

typedef struct ble_info_link2_s
{
    uint8_t                     host_id;                   /**< Host id. */
    uint8_t                     state;                     /**< Current state. */
    ble_svcs_trsps_info_t       svcs_info_trsps;           /**< TRSPS information (server). */
    ble_svcs_fotas_info_t       svcs_info_fotas;           /**< FOTAS information (server). */
} ble_info_link2_t;

typedef struct ble_app_link_info_s
{
    ble_gap_role_t     gap_role;          /**< GAP role. */
    uint8_t            state;             /**< Current state. */
    void               *profile_info;     /**< Link profile information. */
} ble_app_link_info_t;

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

/** Extern BLE Link0 service information */
extern ble_info_link0_t                           ble_info_link0;

/** Extern BLE Link1 service information */
extern ble_info_link1_t                           ble_info_link1;

/** Extern BLE Link2 service information */
extern ble_info_link2_t                           ble_info_link2;

extern ble_app_link_info_t                        ble_app_link_info[];

#endif // _BLE_PROFILE_H_

