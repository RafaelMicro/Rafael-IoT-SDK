#include "ble_profile.h"

const ble_att_param_t *const att_service_comb00[] = {
    &ATT_NULL_INVALID,
    ATT_GAPS_SERVICE,
    ATT_GATTS_SERVICE,
    ATT_DIS_SERVICE,
    ATT_TRSPS_SERVICE,
    ATT_FOTAS_SERVICE,
};

const ble_att_role_by_id_t att_db_link[] = {
    {
        ((const ble_att_param_t **)0),
        att_service_comb00,
    },
};

ble_att_handle_param_t att_hdl_para_links00[SIZE_ARRAY_ROW(att_service_comb00)];

const ble_att_db_mapping_by_id_t att_db_mapping[] = {
    {
        ((ble_att_handle_param_t *)0),
        att_hdl_para_links00,
    },
};

const ble_att_db_mapping_by_id_size_t att_db_mapping_size[] = {
    {
        0,
        SIZE_ARRAY_ROW(att_service_comb00),
    },
};

const uint8_t max_num_conn_host = SIZE_ARRAY_ROW(att_db_mapping_size);

uint8_t *param_rsv_host[SIZE_ARRAY_ROW(att_db_mapping_size)][(REF_SIZE_LE_HOST_PARA >> 2)];
