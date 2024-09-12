#include "atcmd.h"
#include "atcmd_init.h"
#include "atcmd_scan.h"
#include "atcmd_stress_test.h"
#include "atcmd_command_list.h"
#include "atcmd_helper.h"
#include "ble_profile_init.h"
#include "log.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/

/**************************************************************************************************
 *    LOCAL FUNCTIONS DECLARE
 *************************************************************************************************/
static void atcmd_cmd_run(atcmd_t *this);
static void atcmd_err_str_print(ble_err_t status);
static void atcmd_event_rsp_handle(atcmd_t *this, ble_module_evt_t event, void *p_param);
static bool atcmd_matched_cmd_assign(char *cmd_str, atcmd_item_t *running_at_item);
static void atcmd_gap_event_rsp_handle(atcmd_t *this, ble_evt_att_param_t *p_param);
static void atcmd_atcmd_event_rsp_handle(atcmd_t *this, ble_evt_att_param_t *p_param);

/**************************************************************************************************
 *    FUNCTIONS IMPLEMENT
 *************************************************************************************************/
void atcmd_init(atcmd_t *this)
{
    atcmd_queue_init(&this->at_queue);
    at_cmd_item_init(&this->running_at_item, &this->ble_param);
    cmd_list_init();

#ifdef NO_CONNECT
    this->is_no_connect_mode = true;
#else
    this->is_no_connect_mode = false;
#endif
}
ble_err_t atcmd_ble_param_init(atcmd_t *this)
{
    ble_err_t  status;

    status = ble_cmd_phy_controller_init();
    CHECK(status);
    status = ble_addr_init(this);
    CHECK(status);
    status = ble_scan_param_init(this);
    CHECK(status);
    status = ble_adv_param_init(this);
    CHECK(status);
    status = ble_adv_data_init(this);
    CHECK(status);
    status = ble_scan_rsp_init(this);
    CHECK(status);

    this->ble_param.preferred_mtu_size = BLE_GATT_ATT_MTU_MAX;
    status = ble_cmd_default_mtu_size_set(0, BLE_GATT_ATT_MTU_MAX);
    CHECK(status);

    //status = ble_cmd_lesc_init();
    CHECK(status);

    ble_service_data_init(this);
    ble_con_param_init(this);
    ble_create_con_param_init(this);

    this->ble_param.preferred_tx_data_length = 27;

    stress_test_init(&this->ble_param.stress_test);

    return BLE_ERR_OK;
}
ble_err_t atcmd_profile_init(atcmd_t *this, ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback)
{
    ble_err_t status = BLE_ERR_OK;
#ifndef NO_CONNECT
    status = profile_init(gap_callback, atcmd_callback);
#endif
    return status;
}
void atcmd_main_handle(atcmd_t *this)
{
    atcmd_cmd_run(this);
    // do stress test
    this->ble_param.stress_test.send_data(&this->ble_param.stress_test);
}
void atcmd_uart_handle(atcmd_t *this, uint8_t *data, uint8_t length)
{
    // remove '\n' or '\r'
    data[length - 1] = 0;
    char *str = (char *)data;

    // check is AT cmd or not
    if (strlen(str) < 2 || (toupper(str[0]) != 'A') || (toupper(str[1]) != 'T'))
    {
        printf("ERROR : this is not at command [%s] \n", data);
        return;
    }

    // remove AT
    str = str + 2;

    //split command by ';'
    char *cmd = my_strtok(str, AT_CMD_SEP_PUNC);
    while (cmd != NULL)
    {
        atcmd_queue_t *at_queue = &this->at_queue;

        // do high level cmd
        if (high_level_cmd_check(cmd))
        {
            while (!at_queue->empty(at_queue))
            {
                log_debug("2 at_queue->pop(%s) \n", at_queue->front(at_queue)->str);
                at_queue->pop(at_queue);
            }
            this->running_at_item.status = AT_CMD_STATUS_QUEUE;
        }

        //put command into queue
        atcmd_string_t tmp;
        if (strlen(cmd) > (sizeof(tmp.str) - 1))
        {
            printf("ERROR : this at command length is too long\r\n");
            return;
        }
        strcpy(tmp.str, cmd);
        log_debug("at_queue->Push(%s) \n", tmp.str);
        at_queue->push(at_queue, &tmp);

        cmd = my_strtok(NULL, AT_CMD_SEP_PUNC);
    }
    bool check = jump_to_main();
    CHECK_BOOL(check);
}
void atcmd_event_handle(atcmd_t *this, ble_module_evt_t event, void *p_param)
{
    atcmd_event_rsp_handle(this, event, p_param);
    //check stress test
    this->ble_param.stress_test.ble_event_handle(&this->ble_param.stress_test, event, p_param);
    // check event for running at cmd
    atcmd_item_t *running_at_item = &this->running_at_item;
    if (running_at_item->status != AT_CMD_STATUS_BUSY)
    {
        return;
    }
    running_at_item->cmd_info->check_event(event, p_param, running_at_item);
    bool check = jump_to_main();
    CHECK_BOOL(check);
}
void atcmd_gap_service_handle(atcmd_t *this, ble_evt_att_param_t *p_param)
{
    atcmd_gap_event_rsp_handle(this, p_param);
    // running at item check service
    atcmd_item_t *running_at_item = &this->running_at_item;
    if (running_at_item->status != AT_CMD_STATUS_BUSY)
    {
        return;
    }
    running_at_item->cmd_info->check_service(p_param, running_at_item);
    bool check = jump_to_main();
    CHECK_BOOL(check);
}
void atcmd_atcmd_service_handle(atcmd_t *this, ble_evt_att_param_t *p_param)
{
    //check stress test
    this->ble_param.stress_test.receive_data(&this->ble_param.stress_test, p_param);

    // can't print service info if enter stress test mode
    if (!this->ble_param.stress_test.is_enable)
    {
        atcmd_atcmd_event_rsp_handle(this, p_param);
    }

    //check event for running at cmd
    atcmd_item_t *running_at_item = &this->running_at_item;
    if (running_at_item->status != AT_CMD_STATUS_BUSY)
    {
        return;
    }
    running_at_item->cmd_info->check_service(p_param, running_at_item);
    bool check = jump_to_main();
    CHECK_BOOL(check);
}

static void atcmd_cmd_run(atcmd_t *this)
{
    atcmd_item_t *running_at_item = &this->running_at_item;

    switch (running_at_item->status)
    {
    case AT_CMD_STATUS_BUSY:
        return;

    case AT_CMD_STATUS_OK:
    {
        printf(OK_STR);
        running_at_item->status = AT_CMD_STATUS_QUEUE;
    }
    break;

    case AT_CMD_STATUS_FAIL:
    {
        atcmd_err_str_print(running_at_item->err_status);
        running_at_item->status = AT_CMD_STATUS_QUEUE;
    }
    break;

    case AT_CMD_STATUS_QUEUE:
    {
        atcmd_queue_t *at_queue = &this->at_queue;
        if (at_queue->empty(at_queue))
        {
            return;
        }

        //get cmd_str from queue
        char *cmd_str = at_queue->front(at_queue)->str;
        log_debug("1 at_queue->pop(%s) \n", at_queue->front(at_queue)->str);
        at_queue->pop(at_queue);

        // assign cmd
        bool check = atcmd_matched_cmd_assign(cmd_str, running_at_item);
        if (!check)
        {
            running_at_item->status = AT_CMD_STATUS_FAIL;
            running_at_item->err_status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        //do command
        running_at_item->status = AT_CMD_STATUS_BUSY;
        cmd_info_t *cmd = running_at_item->cmd_info;
        log_debug("do cmd : %s\n", cmd->cmd_name);
        cmd->do_cmd(cmd, running_at_item);
    }
    break;

    default:
        CHECKNR(0xAA);
        return;
    }

    bool check = jump_to_main();
    CHECK_BOOL(check);
}
static void atcmd_err_str_print(ble_err_t status)
{
    switch (status)
    {
    case BLE_BUSY:
        printf("ERROR BUSY\r\n");
        break;
    case BLE_ERR_INVALID_PARAMETER:
        printf("ERROR INVALID_PARAM\r\n");
        break;
    case BLE_ERR_INVALID_STATE:
        printf("ERROR INVALID_STATE\r\n");
        break;
    case BLE_ERR_INVALID_HOST_ID:
        printf("ERROR INVALID_HOST_ID\r\n");
        break;
    case BLE_ERR_INVALID_HANDLE:
        printf("ERROR INVALID_HANDLE\r\n");
        break;
    case BLE_ERR_CMD_NOT_SUPPORTED:
        printf("ERROR CMD_NOT_SUPPORTED\r\n");
        break;
    case BLE_ERR_DB_PARSING_IN_PROGRESS:
        printf("ERROR DB_PARSING_IN_PROGRESS\r\n");
        break;
    case BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION:
        printf("ERROR SEQUENTIAL_PROTOCOL_VIOLATION\r\n");
        break;
    case BLE_ERR_WRONG_CONFIG:
        printf("ERROR WRONG_CONFIG\r\n");
        break;
    default:
        printf("ERROR %02d\r\n", status);
        break;
    }
}

static void atcmd_event_rsp_handle(atcmd_t *this, ble_module_evt_t event, void *p_param)
{
    switch (event)
    {

    case BLE_ADV_EVT_SET_ENABLE:
    {
        ble_evt_adv_set_adv_enable_t *p_adv_enable = (ble_evt_adv_set_adv_enable_t *)p_param;

        if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            PRINT_IF_ELSE(p_adv_enable->adv_enabled, "Advertising...\n", "End Advertising...\n");
        }
        else
        {
            printf("Advertising enable failed, status=0x%x.\n", p_adv_enable->status);
        }
    }
    break;

    case BLE_SCAN_EVT_SET_ENABLE:
    {
        ble_evt_scan_set_scan_enable_t *p_scan_enable = (ble_evt_scan_set_scan_enable_t *)p_param;
        if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (p_scan_enable->scan_enabled)
            {
                printf("Scanning...\n");
                scan_result_clear();
            }
            else
            {
                printf("End Scanning...\n");
            }
        }
        else
        {
            printf("Scanning enable failed, status=%d.\n", p_scan_enable->status);
        }
    }
    break;

    case BLE_SCAN_EVT_ADV_REPORT:
    {
        ble_evt_scan_adv_report_t *rpt = (ble_evt_scan_adv_report_t *)p_param;
        //make sure it will print after print OK
        if (this->running_at_item.status != AT_CMD_STATUS_QUEUE)
        {
            return;
        }
        scan_report_event_handle(rpt, this);
    }
    break;

    case BLE_GAP_EVT_CONN_COMPLETE:
    {
        ble_evt_gap_conn_complete_t *p_conn_param = (ble_evt_gap_conn_complete_t *)p_param;
        printf("Connected to %02x:%02x:%02x:%02x:%02x:%02x, addr_type=%d\n",
               p_conn_param->peer_addr.addr[5],
               p_conn_param->peer_addr.addr[4],
               p_conn_param->peer_addr.addr[3],
               p_conn_param->peer_addr.addr[2],
               p_conn_param->peer_addr.addr[1],
               p_conn_param->peer_addr.addr[0],
               p_conn_param->peer_addr.addr_type);
        printf("ID:%d, Status:%d, Interval:%d, Latency:%d, Timeout:%d, ",
               p_conn_param->host_id,
               p_conn_param->status,
               p_conn_param->conn_interval,
               p_conn_param->periph_latency,
               p_conn_param->supv_timeout);

        PRINT_IF_ELSE(p_conn_param->role == BLE_GAP_ROLE_CENTRAL, "Role:Master\n", "Role:Slave\n");

        if (p_conn_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            uint8_t host_id = p_conn_param->host_id;
            this->ble_param.con_param[host_id].max_conn_interval = p_conn_param->conn_interval;
            this->ble_param.con_param[host_id].min_conn_interval = p_conn_param->conn_interval;
            this->ble_param.con_param[host_id].periph_latency = p_conn_param->periph_latency;
            this->ble_param.con_param[host_id].supv_timeout = p_conn_param->supv_timeout;
        }
    }
    break;

    case BLE_GAP_EVT_CONN_CANCEL:
    {
        printf("Cancel Create Connecting...\n");
    }
    break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
    {
        ble_evt_gap_disconn_complete_t *p_discon_param = (ble_evt_gap_disconn_complete_t *)p_param;
        printf("Disconnected, ID:%d, Reason:0x%x\n", p_discon_param->host_id, p_discon_param->reason);
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        ble_evt_gap_conn_param_update_t *p_update_conn_param = (ble_evt_gap_conn_param_update_t *)p_param;
        printf("Connection Update, ID:%d, Status:%d, Interval:%d, Latency:%d, Timeout:%d\n",
               p_update_conn_param->host_id,
               p_update_conn_param->status,
               p_update_conn_param->conn_interval,
               p_update_conn_param->periph_latency,
               p_update_conn_param->supv_timeout);
        if (p_update_conn_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            uint8_t host_id = p_update_conn_param->host_id;
            this->ble_param.con_param[host_id].max_conn_interval = p_update_conn_param->conn_interval;
            this->ble_param.con_param[host_id].min_conn_interval = p_update_conn_param->conn_interval;
            this->ble_param.con_param[host_id].periph_latency = p_update_conn_param->periph_latency;
            this->ble_param.con_param[host_id].supv_timeout = p_update_conn_param->supv_timeout;
        }
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)p_param;
        printf("Phy Update, Status:%d, ID:%d, TX:%d, RX:%d\n",
               p_phy_param->status,
               p_phy_param->host_id,
               p_phy_param->tx_phy,
               p_phy_param->rx_phy);
    }
    break;

    case BLE_GAP_EVT_PHY_READ:
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)p_param;
        printf("Phy Read, Status:%d, ID:%d, TX:%d, RX:%d\n",
               p_phy_param->host_id,
               p_phy_param->status,
               p_phy_param->tx_phy,
               p_phy_param->rx_phy);
    }
    break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *p_mtu_param = (ble_evt_mtu_t *)p_param;
        printf("ID:%d, MTU Size: %d\n",
               p_mtu_param->host_id,
               p_mtu_param->mtu);
    }
    break;

    case BLE_GAP_EVT_RSSI_READ:
    {
        ble_evt_gap_rssi_read_t *p_rssi_param = (ble_evt_gap_rssi_read_t *)p_param;
        printf("Status:%d, ID:%d, RSSI:%d\n",
               p_rssi_param->status,
               p_rssi_param->host_id,
               p_rssi_param->rssi);
    }
    break;

    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
    {
        // ble_evt_suggest_data_length_set_t *p_data_len_param = (ble_evt_suggest_data_length_set_t *)p_param;
        // printf("Default Data Length Update, status: 0x%x\n", p_data_len_param->status);
    }
    break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
    {
        ble_evt_data_length_change_t *p_data_len_param = (ble_evt_data_length_change_t *)p_param;
        printf("Data Length Update, ID:%d, TX:%d, RX:%d, TX_Time:%d, Rx_Time:%d\n",
               p_data_len_param->host_id,
               p_data_len_param->max_tx_octets,
               p_data_len_param->max_rx_octets,
               p_data_len_param->max_tx_time,
               p_data_len_param->max_rx_time);
    }
    break;

    case BLE_SM_EVT_STK_GENERATION_METHOD:
    {
        ble_evt_sm_stk_gen_method_t *p_stk_method = (ble_evt_sm_stk_gen_method_t *)p_param;
        if (p_stk_method->key_gen_method == PASSKEY_ENTRY)
        {
            //Start scanning user-entered passkey.
        }
        else if (p_stk_method->key_gen_method == PASSKEY_DISPLAY)
        {
            //user can generate a 6-digit random code, and display it for pairing.
        }
        //TODO: add p_stk_method->host_id
    }
    break;

    case BLE_SM_EVT_AUTH_STATUS:
    {
        ble_evt_sm_auth_status_t *p_auth_result = (ble_evt_sm_auth_status_t *)p_param;
        printf("AUTH Report, ID:%d, STATUS:%d, LESC:%d\n", p_auth_result->host_id, p_auth_result->status, p_auth_result->lesc);
    }
    break;

    case BLE_SM_EVT_PASSKEY_CONFIRM:
    {
        //ble_evt_passkey_confirm_param_t *event_param = (ble_evt_passkey_confirm_param_t *)param;
    }
    break;

    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_err_t status;
        ble_evt_att_db_parse_complete_t *p_db_parsing = (ble_evt_att_db_parse_complete_t *)p_param;
        int host_id = p_db_parsing->host_id;

        if (p_db_parsing->result == 0)
        {
            status = link_svcs_handles_get(&ble_info_link[host_id]);
            if (status != BLE_ERR_OK)
            {
                printf("get service information fail\n");
            }
        }
        printf("Database Parsing Finished, ID:%d , Result:%d\n", p_db_parsing->host_id, p_db_parsing->result);
    }
    break;

    default:
        break;
    }
}
static bool atcmd_matched_cmd_assign(char *cmd_str, atcmd_item_t *running_at_item)
{
    bool check = true;

    //turn cmd string to item
    check = parse_cmd_string_to_item(cmd_str, running_at_item);
    if (!check)
    {
        printf("command [%s] length is too long\n", cmd_str);
        return check;
    }

    //assign cmd
    check = cmd_assign(running_at_item);
    if (!check)
    {
        printf("command [%s] doesn't exist\n", running_at_item->cmd_str);
        return check;
    }

    return check;
}
static void atcmd_gap_event_rsp_handle(atcmd_t *this, ble_evt_att_param_t *p_param)
{
    uint8_t *data = p_param->data;
    switch (p_param->event)
    {
    case BLESERVICE_GAPS_DEVICE_NAME_READ_RSP_EVENT:
    {
        uint8_t i;
        printf("Device name: ");
        for (i = 0; i < p_param->length; i++)
        {
            printf("%c", data[i]);
        }
        printf("\n");

    }
    break;
    case BLESERVICE_GAPS_APPEARANCE_READ_RSP_EVENT:
    {
        uint16_t appearance = (data[0] | (data[1] << 8));
        printf("Appearance read: %d\n", appearance);
    }
    break;
    case BLESERVICE_GAPS_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_READ_RSP_EVENT:
    {
        ble_gap_conn_param_t conn_param;

        conn_param.min_conn_interval = (data[0] | (data[1] << 8));
        conn_param.max_conn_interval = (data[2] | (data[3] << 8));
        conn_param.periph_latency = (data[4] | (data[5] << 8));
        conn_param.supv_timeout = (data[6] | (data[7] << 8));

        printf("Preferred connection parameters: %d %d %d %d\n",
               conn_param.min_conn_interval,
               conn_param.max_conn_interval,
               conn_param.periph_latency,
               conn_param.supv_timeout
              );
    }
    break;
    default:
        break;
    }
}
static void atcmd_atcmd_event_rsp_handle(atcmd_t *this, ble_evt_att_param_t *p_param)
{
    static uint8_t data[BLE_GATT_ATT_MTU_MAX];
    uint8_t hostId = p_param->host_id;
    uint16_t length = p_param->length;
    //uint8_t *data = p_param->data;

    switch (p_param->event)
    {
    case BLESERVICE_ATCMD_CHARAC01_READ_EVENT:
    {
        uint16_t hdlNum = ble_info_link[hostId].svcs_info_atcmd.info.handles.hdl_charac01;
        ble_gatt_data_param_t param =
        {
            .host_id = hostId,
            .handle_num = hdlNum,
            .p_data = send_data,
            .length = this->ble_param.read_data_len[hostId]
        };
        ble_err_t err = ble_cmd_gatt_read_rsp(&param);
        CHECKNR(err);
    }
    break;
    case BLESERVICE_ATCMD_CHARAC02_READ_EVENT:
    {
        uint16_t hdlNum = ble_info_link[hostId].svcs_info_atcmd.info.handles.hdl_charac02;
        ble_gatt_err_rsp_param_t param =
        {
            .host_id = hostId,
            .handle_num = hdlNum,
            .opcode = OPCODE_ATT_READ_REQUEST,
            .err_rsp = this->ble_param.error_code[hostId]
        };
        ble_err_t err = ble_cmd_gatt_error_rsp(&param);
        CHECKNR(err);
    }
    break;
    case BLESERVICE_ATCMD_CHARAC01_READ_RSP_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Read:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC01_WRITE_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Write:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC01_WRITE_WITHOUT_RSP_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Write_without_rsp:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC01_NOTIFY_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Notify:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC01_INDICATE_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Indicate:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC01_CCCD_READ_RSP_EVENT:
        memcpy(data, p_param->data, length);
        printf("CCCD:%d,%d\n", data[1], data[0]);
        break;
    case BLESERVICE_ATCMD_CHARAC02_WRITE_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Write_enc:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC03_WRITE_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Write_authe:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC04_WRITE_EVENT:
        memcpy(data, p_param->data, length);
        data[length] = 0;
        printf("Write_autho:%s\n", data);
        break;
    case BLESERVICE_ATCMD_CHARAC02_ERROR_RSP_EVENT:
        memcpy(data, p_param->data, length);
        printf("Error_rsp: op_code=%d, error_code = %d\n", data[0], data[3]);
        break;
    case BLESERVICE_ATCMD_CHARAC01_RESTORE_BOND_DATA_EVENT:
        memcpy(data, p_param->data, length);
        printf("Restore CCCD:%d\n", data[0]);
        break;
    default:
        break;
    }
}
