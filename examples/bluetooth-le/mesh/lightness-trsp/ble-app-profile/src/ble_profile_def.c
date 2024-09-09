/************************************************************************
 *
 * File Name  : ble_profile_def.c
 * Description: This file contains the definitions of BLE profiles.
 *
 *******************************************************************/
#include <stdio.h>
#include "ble_profile.h"

/**************************************************************************
 * Profile Definitions
 **************************************************************************/
/** Service Combination #00
 * @note included @ref ATT_GAP_SERVICE, @ref ATT_DIS_SERVICE and @ref ATT_UDF01S_SERVICE services
*/
const ble_att_param_t *const att_service_comb00[] =
{
    &ATT_NULL_INVALID,       //mandatory, don't remove it.
    ATT_MESH_PROV_SERVICE,
    ATT_TRSPS_SERVICE,
    ATT_FOTAS_SERVICE,
};

const ble_att_param_t *const att_service_comb01[] =
{
    &ATT_NULL_INVALID,                  //mandatory, don't remove it.
    ATT_MESH_PROXY_SERVICE,
    ATT_TRSPS_SERVICE,
    ATT_FOTAS_SERVICE,
};

const ble_att_param_t *const att_service_comb02[] =
{
    &ATT_NULL_INVALID,                  //mandatory, don't remove it.
    ATT_TRSPS_SERVICE,
    ATT_FOTAS_SERVICE,
};

/**************************************************************************
 * BLE Connection Link Definitions
 **************************************************************************/
/** BLE Connection Links Definition
 * @attention Do NOT modify the name of this definition.
 * @note If there does not support server or client please set to "((const ble_att_param_t **)0)" which means NULL.
*/
const ble_att_role_by_id_t att_db_link[] =
{
    // Link 0
    {
        ((const ble_att_param_t **)0),    // Client Profile
        att_service_comb00,             // Server Profile
    },
    // Link 1
    {
        ((const ble_att_param_t **)0),    // Client Profile
        att_service_comb01,             // Server Profile
    },
    // Link 2
    {
        ((const ble_att_param_t **)0),    // Client Profile
        att_service_comb02,             // Server Profile
    },
};


/** BLE Connection Link Parameter Definition
 * @attention Every active role in active links shall be defined related link parameters.
*/
ble_att_handle_param_t ATT_Hdl_Para_LinkP00[SIZE_ARRAY_ROW(att_service_comb00)]; // Link 0
ble_att_handle_param_t ATT_Hdl_Para_LinkP01[SIZE_ARRAY_ROW(att_service_comb01)];
ble_att_handle_param_t ATT_Hdl_Para_LinkP02[SIZE_ARRAY_ROW(att_service_comb02)];


/** BLE Connection Link Parameter Table Definition
 * @attention Do NOT modify the name of this definition.
 * @note If there does not support server or client please set to "((ble_att_handle_param_t *)0)" which means NULL.
*/
const ble_att_db_mapping_by_id_t att_db_mapping[] =
{
    // Link 0
    {
        ((ble_att_handle_param_t *)0),    // Client Link Parameter
        ATT_Hdl_Para_LinkP00,             // Server Link Parameter
    },
    // Link 1
    {
        ((ble_att_handle_param_t *)0),
        ATT_Hdl_Para_LinkP01,
    },
    // Link 2
    {
        ((ble_att_handle_param_t *)0),
        ATT_Hdl_Para_LinkP02,
    },
};



/** BLE Connection Link Mapping Size Definition
 * @attention Do NOT modify the name of this definition.
 * @note If there does not support server or client please set to 0.
*/
const ble_att_db_mapping_by_id_size_t att_db_mapping_size[] =
{
    // Link 0
    {
        0,                                    // Client Link Mapping Size
        SIZE_ARRAY_ROW(att_service_comb00),   // Server Link Mapping Size
    },
    // Link 1
    {
        0,
        SIZE_ARRAY_ROW(att_service_comb01),
    },
    // Link 2
    {
        0,
        SIZE_ARRAY_ROW(att_service_comb02),
    },
};


/** Maximum Number of Host Connection Link Definition
 * @attention Do NOT modify this definition.
 * @note Defined for host layer.
*/
const uint8_t max_num_conn_host = (SIZE_ARRAY_ROW(att_db_mapping_size));


/** Host Connection Link Information Definition
 * @attention Do NOT modify this definition.
 * @note Defined for host layer.
*/
uint8_t *param_rsv_host[SIZE_ARRAY_ROW(att_db_mapping_size)][(REF_SIZE_LE_HOST_PARA >> 2)];

