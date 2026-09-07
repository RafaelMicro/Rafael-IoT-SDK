#ifndef LIGHTING_APP_BLE_PROFILE_H
#define LIGHTING_APP_BLE_PROFILE_H

#include <stdint.h>

#include "ble_common.h"
#include "ble_service_common.h"
#include "ble_service_dis.h"
#include "ble_service_fotas.h"
#include "ble_service_gaps.h"
#include "ble_service_gatts.h"
#include "ble_service_trsps.h"

#define MAX_NUM_CONN_GAPS 1
#define MAX_NUM_CONN_GATTS 1
#define MAX_NUM_CONN_DIS 1
#define MAX_NUM_CONN_TRSPS 1
#define MAX_NUM_CONN_FOTAS 1

typedef struct ble_info_link0_s {
    ble_svcs_gaps_info_t svcs_info_gaps;
    ble_svcs_gatts_info_t svcs_info_gatts;
    ble_svcs_dis_info_t svcs_info_dis;
    ble_svcs_trsps_info_t svcs_info_trsps;
    ble_svcs_fotas_info_t svcs_info_fotas;
} ble_info_link0_t;

typedef struct ble_app_link_info_s {
    ble_gap_role_t gap_role;
    uint8_t state;
    void *profile_info;
} ble_app_link_info_t;

extern const uint8_t max_num_conn_host;
extern const ble_att_role_by_id_t att_db_link[];
extern const ble_att_db_mapping_by_id_t att_db_mapping[];
extern const ble_att_db_mapping_by_id_size_t att_db_mapping_size[];
extern ble_app_link_info_t ble_app_link_info[];

#endif
