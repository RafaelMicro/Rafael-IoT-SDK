#include "ble_profile.h"

ble_info_link0_t ble_info_link0;

ble_app_link_info_t ble_app_link_info[BLE_SUPPORT_NUM_CONN_MAX] = {
    {
        .gap_role = BLE_GAP_ROLE_PERIPHERAL,
        .state = 0x00,
        .profile_info = (void *)&ble_info_link0,
    },
};
