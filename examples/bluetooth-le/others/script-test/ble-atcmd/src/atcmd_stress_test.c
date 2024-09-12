/*******************************************************************
 *
 * File Name  : stress_test.c
 * Description: This file contains the functions of stress test mode
 *
 *******************************************************************
 *
 *      Copyright (c) 2020, All Right Reserved
 *      Rafael Microelectronics Co. Ltd.
 *      Taiwan, R.O.C.
 *
 *******************************************************************/
#include "atcmd_stress_test.h"
#include "FreeRTOS.h"
#include "timers.h"

#define CONNECT
#ifdef CONNECT

static TimerHandle_t stress_test_timer;
static stress_test_t *stress_test;

static void stress_test_timer_handler(TimerHandle_t timer);
static bool stress_test_timer_start(void);
static void stress_test_timer_init(stress_test_t *st_test);

static void stress_test_timer_init(stress_test_t *st_test)
{
    stress_test = st_test;
    stress_test_timer = xTimerCreate("Stress_Test_Timer", pdMS_TO_TICKS(50), pdTRUE, ( void * ) 0, stress_test_timer_handler);
    if (stress_test_timer == NULL)
    {
        printf("The Stress_Test_Timer was not created.\n");
    }
    if ( xTimerIsTimerActive( stress_test_timer ) != pdFALSE )
    {
        printf("The Stress_Test_Timer was not active.\n");
    }
}
static void stress_test_timer_handler(TimerHandle_t timer)
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(timer);

    if (stress_test->is_enable)
    {
        stress_test->is_timer_click = true;
        bool check = jump_to_main();
        CHECK_BOOL(check);
    }
}
static bool stress_test_timer_start(void)
{
    bool state = (xTimerStart( stress_test_timer, 0 ) == pdPASS);
    return state;
}

static void stress_test_unit_send_data(stress_test_unit_t *this);
static void stress_test_unit_receive_data(stress_test_unit_t *this, uint8_t cmdAccess, uint8_t *data, uint16_t length);
static void stress_test_unit_reset(stress_test_unit_t *this);

void stress_test_unit_init(stress_test_unit_t *this, int host_id)
{
    int i;

    //init variable
    this->is_enable = false;
    this->host_id = host_id;
#ifdef CONNECT
    this->role = link_info[host_id].role;
#endif
    for (i = 0; i < sizeof(this->tx_data_arr); i++)
    {
        this->tx_data_arr[i] = i;
    }
    this->tx_data_len = 244;
    this->tx_index = 0;
    this->rx_index = 0;
    this->rx_test_count = 0;

    //init function
    this->send_data = stress_test_unit_send_data;
    this->receive_data = stress_test_unit_receive_data;
    this->reset = stress_test_unit_reset;

}
static void stress_test_unit_send_data(stress_test_unit_t *this)
{
    uint16_t hdl_num;
    ble_err_t err = BLE_BUSY;

    if (!this->is_enable)
    {
        return;
    }


    hdl_num = ble_info_link[this->host_id].svcs_info_atcmd.info.handles.hdl_charac01;
    this->tx_data_arr[0] = this->tx_index;

    //send tx data
    if (this->role == BLE_GATT_ROLE_CLIENT)
    {
        ble_gatt_data_param_t param =
        {
            .host_id = this->host_id,
            .handle_num = hdl_num,
            .length = this->tx_data_len,
            .p_data = this->tx_data_arr
        };
        err = ble_cmd_gatt_write_cmd(&param);
    }
    else // BLE_GATT_ROLE_SERVER
    {
        //check cccd
        int cccd = ble_info_link[this->host_id].svcs_info_atcmd.info.data.charac01_cccd;
        if (cccd & 1 == 1)
        {
            ble_gatt_data_param_t param =
            {
                .host_id = this->host_id,
                .handle_num = hdl_num,
                .length = this->tx_data_len,
                .p_data = this->tx_data_arr
            };
            err = ble_cmd_gatt_notification(&param);
        }
    }

    //check if send tx data success
    if (err == BLE_ERR_OK)
    {
        this->tx_index++;
        this->tx_index &= 0xFF;
    }
    else if (err == BLE_BUSY) {}
    else
    {
        printf("[Err]TX_E = %d\n", err);
    }

}
static void stress_test_unit_receive_data(stress_test_unit_t *this, uint8_t cmdAccess, uint8_t *data, uint16_t length)
{
    int i;
    int error_counter = 0;

    if (!this->is_enable)
    {
        return;
    }

    // only make certain event pass
    if (this->role == BLE_GATT_ROLE_CLIENT)
    {
        if (cmdAccess != BLESERVICE_ATCMD_CHARAC01_NOTIFY_EVENT)
        {
            return;
        }
    }
    else     // BLE_GATT_ROLE_SERVER
    {
        if (cmdAccess != BLESERVICE_ATCMD_CHARAC01_WRITE_WITHOUT_RSP_EVENT)
        {
            return;
        }
    }

    //calculate the test count
    if (this->rx_index == 0)
    {
        this->rx_test_count++;
    }

    //check length
    if (length != this->tx_data_len)
    {
        printf("[Err]ErrLen = %d\n", length);
        return;
    }

    //check first index
    if (data[0] != this->rx_index)
    {
        printf("[Err]ErrFirst[%d] = %d\n", this->rx_index, data[0]);
        error_counter++;
    }

    //check each data
    for (i = 1; i < length; i++)
    {
        if (data[i] != i)
        {
            printf("[Err]ErrIdx[%d] = %d\n", i, data[i]);
            error_counter++;
            break;
        }
    }

    //modify index
    this->rx_index = data[0] + 1;
    this->rx_index &= 0xFF;

    // print info
    if (error_counter == 0)
    {
        printf(".\n");
    }
    else
    {
        printf("[Err]ErrCount = %d, First_Idx = %d\n", error_counter, data[0]);
    }
}
static void stress_test_unit_reset(stress_test_unit_t *this)
{
    this->tx_index = 0;
    this->rx_index = 0;
    this->rx_test_count = 0;
}

static void stress_test_start(stress_test_t *this);
static void stress_test_stop(stress_test_t *this);
static void stress_test_send_data(stress_test_t *this);
static void stress_test_receive_data(stress_test_t *this, ble_evt_att_param_t *p_param);
static void stress_test_enable(stress_test_t *this, int host_id);
static void stress_test_disable(stress_test_t *this, int host_id);
static void stress_test_ble_event_handle(stress_test_t *this, ble_module_evt_t event, void *param);

void stress_test_init(stress_test_t *this)
{
    int i;

    //init variable
    this->is_enable = false;
    this->is_timer_click = false;
    for (i = 0; i < MAX_HOST_ID; i++)
    {
        stress_test_unit_init(&this->uint_list[i], i);
    }

    //init function
    this->start = stress_test_start;
    this->stop = stress_test_stop;
    this->send_data = stress_test_send_data;
    this->receive_data = stress_test_receive_data;
    this->enable = stress_test_enable;
    this->disable = stress_test_disable;
    this->ble_event_handle = stress_test_ble_event_handle;

    stress_test_timer_init(this);
}
static void stress_test_start(stress_test_t *this)
{
    if (!stress_test_timer_start())
    {
        printf("The Stress_Test_Timer start failed.\n");
    }
    this->is_enable = true;

}
static void stress_test_stop(stress_test_t *this)
{
    this->is_enable = false;
}
static void stress_test_send_data(stress_test_t *this)
{
    int i;
    if (!this->is_enable || !this->is_timer_click)
    {
        return;
    }
    for (i = 0; i < MAX_HOST_ID; i++)
    {
        stress_test_unit_t *st_uint = &this->uint_list[i];
        st_uint->send_data(st_uint);
    }
    this->is_timer_click = false;
}
static void stress_test_receive_data(stress_test_t *this, ble_evt_att_param_t *p_param)
{
    stress_test_unit_t *st_uint;
    if (!this->is_enable)
    {
        return;
    }
    st_uint = &this->uint_list[p_param->host_id];
    st_uint->receive_data(st_uint, p_param->event, p_param->data, p_param->length);
}
static void stress_test_enable(stress_test_t *this, int host_id)
{
    // After connected the host id, enable the stress test for the host id
    stress_test_unit_t *st_uint = &this->uint_list[host_id];
    st_uint->reset(st_uint);
    st_uint->is_enable = true;

}
static void stress_test_disable(stress_test_t *this, int host_id)
{
    // After disconnected the host id, disenable the stress test for the host id
    this->uint_list[host_id].is_enable = false;
}
static void stress_test_ble_event_handle(stress_test_t *this, ble_module_evt_t event, void *param)
{
    if (!this->is_enable)
    {
        return;
    }

    switch (event)
    {
    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_err_t err;
        ble_evt_att_db_parse_complete_t *db_parsing = (ble_evt_att_db_parse_complete_t *)param;
        uint8_t host_id = db_parsing->host_id;
        uint16_t hdl_num = ble_info_link[host_id].svcs_info_atcmd.info.handles.hdl_charac01_cccd;

        if (db_parsing->result == 0)
        {
            err = ble_cmd_mtu_size_update(host_id, BLE_GATT_ATT_MTU_MAX);
            if (err != BLE_ERR_OK)
            {
                printf("[Err]ble_gatt_exchange_mtu_req fail, id=%d\n", host_id);
            }

            err = ble_svcs_cccd_set(host_id, hdl_num, BLEGATT_CCCD_NOTIFICATION);
            if (err != BLE_ERR_OK)
            {
                printf("[Err]ble_svcs_cccd_set fail, id=%d\n", host_id);
            }
        }
    }
    break;
    case BLE_GAP_EVT_DISCONN_COMPLETE:
    {
        ble_evt_gap_disconn_complete_t *disconnParam = (ble_evt_gap_disconn_complete_t *)param;
        this->disable(this, disconnParam->host_id);
    }
    break;
    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *mtuParam = (ble_evt_mtu_t *)param;
        if (mtuParam->mtu == BLE_GATT_ATT_MTU_MAX)
        {
            // enable for master
            this->enable(this, mtuParam->host_id);
        }
    }
    break;
    }
}
#else
void stress_test_Init(stress_test *this) {}
#endif
