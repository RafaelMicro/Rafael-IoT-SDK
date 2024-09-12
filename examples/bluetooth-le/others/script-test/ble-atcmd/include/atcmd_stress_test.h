/*******************************************************************
 *
 * File Name  : stress_test.h
 * Description: This file contains the functions of stress test mode
 *
 *******************************************************************
 *
 *      Copyright (c) 2020, All Right Reserved
 *      Rafael Microelectronics Co. Ltd.
 *      Taiwan, R.O.C.
 *
 *******************************************************************/
#ifndef _STRESS_TEST_H_
#define _STRESS_TEST_H_

#include <stdbool.h>
#include "atcmd_setting.h"
#include "ble_profile.h"
#include "atcmd_helper.h"
#include "ble_event.h"

// define MAX_HOST_ID
#if(BLE_SUPPORT_NUM_CONN_MAX == 0)
#define MAX_HOST_ID 1
#else
#define MAX_HOST_ID BLE_SUPPORT_NUM_CONN_MAX
#endif //#if(BLE_SUPPORT_NUM_CONN_MAX == 0)

typedef struct stress_test_unit_s
{
    bool is_enable;
    bool is_timer_click;
    int host_id;
    ble_gatt_role_t role;
    uint8_t tx_data_arr[256];
    uint8_t tx_data_len;
    uint8_t tx_index;
    uint8_t rx_index;
    uint8_t rx_test_count;
    void (*send_data)(struct stress_test_unit_s *this);
    void (*receive_data)(struct stress_test_unit_s *this, uint8_t cmdAccess, uint8_t *data, uint16_t length);
    void (*reset)(struct stress_test_unit_s *this);
} stress_test_unit_t;

void stress_test_unit_init(stress_test_unit_t *this, int host_id);

typedef struct stress_test_s
{
    bool is_enable;
    bool is_timer_click;
    stress_test_unit_t uint_list[MAX_HOST_ID];
    void (*start)(struct stress_test_s *this);
    void (*stop)(struct stress_test_s *this);
    void (*send_data)(struct stress_test_s *this);
    void (*receive_data)(struct stress_test_s *this, ble_evt_att_param_t *p_param);
    void (*enable)(struct stress_test_s *this, int host_id);
    void (*disable)(struct stress_test_s *this, int host_id);
    void (*ble_event_handle)(struct stress_test_s *this, ble_module_evt_t event, void *param);
} stress_test_t;

void stress_test_init(stress_test_t *this);

#endif //_STRESS_TEST_H_
