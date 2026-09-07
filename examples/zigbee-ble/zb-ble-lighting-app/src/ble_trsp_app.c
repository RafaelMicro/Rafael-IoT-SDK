#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "ble_api.h"
#include "ble_app.h"
#include "ble_event.h"
#include "ble_fota.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ble_profile.h"
#include "task_host_tx.h"
#include "ble_trsp_app.h"
#include "hosal_uart.h"
#include "uart_stdio.h"
#include "zigbee_platform.h"

#define BLE_APP_CB_QUEUE_SIZE 32
#define APP_ISR_QUEUE_SIZE 2
#define APP_REQ_QUEUE_SIZE 6
#define APP_QUEUE_SIZE (BLE_APP_CB_QUEUE_SIZE + APP_ISR_QUEUE_SIZE + APP_REQ_QUEUE_SIZE)

#define APP_TRSP_P_HOST_ID 0
#define BLE_TRSP_TASK_STACK_SIZE 1024
#define BLE_TRSP_TASK_PRIORITY 2
#define DEFAULT_MTU BLE_GATT_ATT_MTU_MAX
#define UART_BUFF_DEPTH 2

#define DEVICE_NAME "LIGHTING_TRSP"
#define APP_ADV_INTERVAL_MIN 160U
#define APP_ADV_INTERVAL_MAX 160U

#ifndef CONFIG_APP_TRSP_OPERATION_UART_PORT
#define CONFIG_APP_TRSP_OPERATION_UART_PORT 1
#endif

#ifndef CONFIG_APP_TRSP_OPERATION_UART_TX_PIN
#define CONFIG_APP_TRSP_OPERATION_UART_TX_PIN 28
#endif

#ifndef CONFIG_APP_TRSP_OPERATION_UART_RX_PIN
#define CONFIG_APP_TRSP_OPERATION_UART_RX_PIN 29
#endif

#ifndef CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE
#define CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE 115200
#endif

static const uint8_t DEVICE_NAME_STR[] = {DEVICE_NAME};
static const ble_gap_addr_t DEVICE_ADDR = {
    .addr_type = RANDOM_STATIC_ADDR,
    .addr = {0x21, 0x22, 0x23, 0x24, 0x25, 0xC6},
};

#if (CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(trsp_uart_dev,
                    CONFIG_APP_TRSP_OPERATION_UART_PORT,
                    CONFIG_APP_TRSP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_TRSP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_115200)
#elif (CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE == 38400)
HOSAL_UART_DEV_DECL(trsp_uart_dev,
                    CONFIG_APP_TRSP_OPERATION_UART_PORT,
                    CONFIG_APP_TRSP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_TRSP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_38400)
#elif (CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE == 57600)
HOSAL_UART_DEV_DECL(trsp_uart_dev,
                    CONFIG_APP_TRSP_OPERATION_UART_PORT,
                    CONFIG_APP_TRSP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_TRSP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_57600)
#elif (CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(trsp_uart_dev,
                    CONFIG_APP_TRSP_OPERATION_UART_PORT,
                    CONFIG_APP_TRSP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_TRSP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_500000)
#elif (CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(trsp_uart_dev,
                    CONFIG_APP_TRSP_OPERATION_UART_PORT,
                    CONFIG_APP_TRSP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_TRSP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_1000000)
#elif (CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(trsp_uart_dev,
                    CONFIG_APP_TRSP_OPERATION_UART_PORT,
                    CONFIG_APP_TRSP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_TRSP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_2000000)
#else
#error "Unsupported CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE"
#endif

static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;
static TimerHandle_t g_fota_timer;
static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;
static uint8_t g_rx_buffer[UART_BUFF_DEPTH][DEFAULT_MTU];
static uint8_t g_rx_buffer_length[UART_BUFF_DEPTH];
static uint8_t g_uart_index = 0;
static uint8_t g_uart_transmit_index = 0;
static uint8_t g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
static bool g_data_send_pending = false;

static void ble_trsp_task(void *parameters_ptr);
static void ble_app_main(app_req_param_t *p_param);
static void app_peripheral_handler(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static ble_err_t server_profile_init(uint8_t host_id);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static void svcs_fota_data_init(ble_svcs_fotas_data_t *p_data);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);
static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool fota_sw_timer_start(void);
static ble_err_t ble_app_event_cb(void *p_param);
static ble_err_t ble_service_data_cb(void *p_param);
static ble_err_t ble_l2cap_data_cb(void *p_param);
static void ble_evt_handler(ble_evt_param_t *p_param);
static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param);
static void ble_svcs_fotas_evt_handler(ble_evt_att_param_t *p_param);
static bool uart_data_handler(char ch);
static int uart0_rx_callback(void *p_arg);
static void ble_hci_cmd_step_delay(void);
static bool ble_wait_hci_cmd_queue_ready(TickType_t timeout_ticks);
static void trsp_data_send_from_isr(uint8_t *p_data, uint8_t length);
static void fota_timer_handler(TimerHandle_t timer);
static void app_uart_init(void);

static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data) {
    p_data->service_changed_cccd = 0;
}

static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data) {
    p_data->udatni01_cccd = 0;
}

static void svcs_fota_data_init(ble_svcs_fotas_data_t *p_data) {
    p_data->command_cccd = 0;
    p_data->data_cccd = 0;
}

static void ble_hci_cmd_step_delay(void) {
    /* Give Host_TX a chance to drain commands before the next HCI-heavy step. */
    vTaskDelay(pdMS_TO_TICKS(20));
}

static bool ble_wait_hci_cmd_queue_ready(TickType_t timeout_ticks) {
    TickType_t start_ticks = xTaskGetTickCount();

    for (;;) {
        if ((g_hci_tx_cmd_handle != NULL) && (g_hci_common_handle != NULL) &&
            (uxQueueSpacesAvailable(g_hci_tx_cmd_handle) > 4U) &&
            (uxQueueSpacesAvailable(g_hci_common_handle) > 5U)) {
            return true;
        }

        if ((xTaskGetTickCount() - start_ticks) >= timeout_ticks) {
            return false;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static bool handle_trsp_command(const uint8_t *data, uint16_t length) {
    static const uint8_t leave_cmd[] = "Leave";

    if ((length == (sizeof(leave_cmd) - 1U)) && (memcmp(data, leave_cmd, sizeof(leave_cmd) - 1U) == 0)) {
        printf("TRSP command: Leave -> factory reset\n");
        ZB_THREAD_SAFE(zigbee_do_factory_reset();)
        return true;
    }

    return false;
}

static bool uart_data_handler(char ch) {
    static uint8_t rx_buffer[DEFAULT_MTU];
    static uint8_t index = 0;

    if (index < sizeof(rx_buffer)) {
        rx_buffer[index++] = (uint8_t)ch;
    } else {
        trsp_data_send_from_isr(rx_buffer, index);
        index = 0;
        rx_buffer[index++] = (uint8_t)ch;
    }

    if (index >= (g_trsp_mtu - 3U)) {
        trsp_data_send_from_isr(rx_buffer, index);
        index = 0;
        return false;
    }

    if ((ch == '\n') || (ch == '\r')) {
        if (index > 1U) {
            trsp_data_send_from_isr(rx_buffer, index - 1U);
        }
        index = 0;
        return true;
    }

    return false;
}

static int uart0_rx_callback(void *p_arg) {
    char ch;

    (void)p_arg;
    hosal_uart_receive(&trsp_uart_dev, &ch, 1);
    (void)uart_data_handler(ch);
    return 0;
}

static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param) {
    if (p_param->gatt_role != BLE_GATT_ROLE_SERVER) {
        return;
    }

    switch (p_param->event) {
    case BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT:
    case BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT:
        if (p_param->length != 0U) {
            if (handle_trsp_command(p_param->data, p_param->length)) {
                break;
            }
            (void)hosal_uart_send(&trsp_uart_dev, p_param->data, p_param->length);
        }
        break;

    case BLESERVICE_TRSPS_UDATRW01_READ_EVENT: {
        static const uint8_t read_data[] = "UDATRW01 data";
        ble_gatt_data_param_t gatt_data_param;

        gatt_data_param.host_id = p_param->host_id;
        gatt_data_param.handle_num = p_param->handle_num;
        gatt_data_param.length = SIZE_STRING(read_data);
        gatt_data_param.p_data = (uint8_t *)read_data;

        if (ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &gatt_data_param) != BLE_ERR_OK) {
            printf("TRSP read response failed\n");
        }
        break;
    }

    default:
        break;
    }
}

static void fota_timer_handler(TimerHandle_t timer) {
    configASSERT(timer);

    if (ble_app_link_info[APP_TRSP_P_HOST_ID].state != STATE_CONNECTED) {
        return;
    }

    if (ble_fota_timertick() == EXPIRED) {
        (void)ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_FOTA_TIMER_EXPIRY, false);
    }
}

static void handle_adv_set_enable(ble_evt_adv_set_adv_enable_t *p_adv_enable) {
    if (p_adv_enable->status != BLE_HCI_ERR_CODE_SUCCESS) {
        printf("Advertising enable failed\n");
        return;
    }

    if (g_advertising_host_id == BLE_HOSTID_RESERVED) {
        return;
    }

    if (p_adv_enable->adv_enabled) {
        ble_app_link_info[g_advertising_host_id].state = STATE_ADVERTISING;
        printf("BLE advertising\n");
    } else {
        ble_app_link_info[g_advertising_host_id].state = STATE_STANDBY;
        printf("BLE idle\n");
    }
}

static void handle_conn_complete(ble_evt_gap_conn_complete_t *p_conn_param) {
    if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS) {
        printf("BLE connect failed: 0x%02x\n", p_conn_param->status);
        return;
    }

    ble_app_link_info[p_conn_param->host_id].state = STATE_CONNECTED;
    printf("BLE connected, ID=%u\n", p_conn_param->host_id);
}

static void handle_conn_param_update(ble_evt_gap_conn_param_update_t *p_conn_param) {
    if (p_conn_param->status == BLE_HCI_ERR_CODE_SUCCESS) {
        printf("BLE conn updated, ID=%u\n", p_conn_param->host_id);
    }
}

static void handle_phy_update(ble_evt_gap_phy_t *p_phy_param) {
    if (p_phy_param->status == BLE_HCI_ERR_CODE_SUCCESS) {
        printf("BLE PHY tx=%u rx=%u\n", p_phy_param->tx_phy, p_phy_param->rx_phy);
    }
}

static void handle_mtu_exchange(ble_evt_mtu_t *p_mtu_param) {
    g_trsp_mtu = p_mtu_param->mtu;
    printf("BLE MTU=%u\n", p_mtu_param->mtu);
}

static void handle_write_suggested_default_data_length(ble_evt_suggest_data_length_set_t *p_data_len_param) {
    printf("BLE default data length status=%u\n", p_data_len_param->status);
}

static void handle_data_length_change(ble_evt_data_length_change_t *p_data_len_param) {
    printf("BLE data length changed, ID=%u\n", p_data_len_param->host_id);
}

static void handle_disconn_complete(ble_evt_gap_disconn_complete_t *p_disconnect_param) {
    if (p_disconnect_param->status != BLE_HCI_ERR_CODE_SUCCESS) {
        printf("BLE disconnect failed: 0x%02x\n", p_disconnect_param->status);
        return;
    }

    ble_fota_disconnect();
    ble_app_link_info[p_disconnect_param->host_id].state = STATE_STANDBY;
    printf("BLE disconnected, ID=%u reason=0x%02x\n", p_disconnect_param->host_id, p_disconnect_param->reason);
    (void)ble_app_request_set(p_disconnect_param->host_id, APP_REQUEST_ADV_START, false);
}

static void ble_evt_handler(ble_evt_param_t *p_param) {
    switch (p_param->event) {
    case BLE_ADV_EVT_SET_ENABLE:
        handle_adv_set_enable(&p_param->event_param.ble_evt_adv.param.evt_set_adv_enable);
        break;

    case BLE_GAP_EVT_CONN_COMPLETE:
        handle_conn_complete(&p_param->event_param.ble_evt_gap.param.evt_conn_complete);
        break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        handle_conn_param_update(&p_param->event_param.ble_evt_gap.param.evt_conn_param_update);
        break;

    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
        handle_phy_update(&p_param->event_param.ble_evt_gap.param.evt_phy);
        break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
        handle_mtu_exchange(&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu);
        break;

    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
        handle_write_suggested_default_data_length(&p_param->event_param.ble_evt_att_gatt.param.ble_evt_suggest_data_length_set);
        break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
        handle_data_length_change(&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change);
        break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
        handle_disconn_complete(&p_param->event_param.ble_evt_gap.param.evt_disconn_complete);
        break;

    default:
        break;
    }
}

static void app_peripheral_handler(app_req_param_t *p_param) {
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[p_param->host_id].profile_info;

    switch (p_param->app_req) {
    case APP_REQUEST_ADV_START:
        svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);
        svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.server_info.data);
        svcs_fota_data_init(&p_profile_info->svcs_info_fotas.server_info.data);

        status = ble_cmd_default_mtu_size_set(p_param->host_id, DEFAULT_MTU);
        if (status != BLE_ERR_OK) {
            printf("BLE default mtu set failed: %d\n", status);
            return;
        }

        status = adv_init();
        if (status != BLE_ERR_OK) {
            printf("BLE adv init failed: %d\n", status);
            return;
        }

        status = adv_enable(p_param->host_id);
        if (status != BLE_ERR_OK) {
            printf("BLE adv enable failed: %d\n", status);
            return;
        }

        g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
        break;

    case APP_REQUEST_TRSPS_DATA_SEND:
        if ((p_profile_info->svcs_info_trsps.server_info.data.udatni01_cccd & BLEGATT_CCCD_NOTIFICATION) != 0) {
            status = ble_svcs_trsps_server_send(p_param->host_id,
                                                BLEGATT_CCCD_NOTIFICATION,
                                                p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                                &g_rx_buffer[g_uart_transmit_index][0],
                                                g_rx_buffer_length[g_uart_transmit_index]);
        } else if ((p_profile_info->svcs_info_trsps.server_info.data.udatni01_cccd & BLEGATT_CCCD_INDICATION) != 0) {
            status = ble_svcs_trsps_server_send(p_param->host_id,
                                                BLEGATT_CCCD_INDICATION,
                                                p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                                &g_rx_buffer[g_uart_transmit_index][0],
                                                g_rx_buffer_length[g_uart_transmit_index]);
        }

        if (status == BLE_ERR_OK) {
            g_data_send_pending = false;
            g_uart_transmit_index++;
            if (g_uart_transmit_index == UART_BUFF_DEPTH) {
                g_uart_transmit_index = 0;
            }
        } else {
            g_data_send_pending = true;
            (void)ble_app_request_set(p_param->host_id, APP_REQUEST_TRSPS_DATA_SEND, false);
        }
        break;

    case APP_REQUEST_FOTA_TIMER_EXPIRY:
        ble_fota_timerexpiry_handler(p_param->host_id);
        break;

    default:
        break;
    }
}

static void ble_app_main(app_req_param_t *p_param) {
    app_peripheral_handler(p_param);
}

static ble_err_t adv_init(void) {
    ble_err_t status;
    ble_adv_param_t adv_param;
    ble_adv_data_param_t adv_data_param;
    ble_adv_data_param_t adv_scan_data_param;
    ble_gap_addr_t addr_param;
    const uint8_t scan_rsp_ad_length = 1 + strlen((const char *)DEVICE_NAME_STR);
    uint8_t adv_data[] = {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
    };
    uint8_t adv_scan_rsp_data[2 + sizeof(DEVICE_NAME_STR) - 1];

    adv_scan_rsp_data[0] = scan_rsp_ad_length;
    adv_scan_rsp_data[1] = GAP_AD_TYPE_LOCAL_NAME_COMPLETE;
    memcpy(&adv_scan_rsp_data[2], DEVICE_NAME_STR, sizeof(DEVICE_NAME_STR) - 1);

    status = ble_cmd_device_addr_get(&addr_param);
    if (status != BLE_ERR_OK) {
        return status;
    }

    adv_param.adv_type = ADV_TYPE_ADV_IND;
    adv_param.own_addr_type = addr_param.addr_type;
    adv_param.adv_interval_min = APP_ADV_INTERVAL_MIN;
    adv_param.adv_interval_max = APP_ADV_INTERVAL_MAX;
    adv_param.adv_channel_map = ADV_CHANNEL_ALL;
    adv_param.adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL;

    status = ble_cmd_adv_param_set(&adv_param);
    if (status != BLE_ERR_OK) {
        return status;
    }

    adv_data_param.length = sizeof(adv_data);
    memcpy(adv_data_param.data, adv_data, sizeof(adv_data));
    status = ble_cmd_adv_data_set(&adv_data_param);
    if (status != BLE_ERR_OK) {
        return status;
    }

    adv_scan_data_param.length = sizeof(adv_scan_rsp_data);
    memcpy(adv_scan_data_param.data, adv_scan_rsp_data, sizeof(adv_scan_rsp_data));
    return ble_cmd_adv_scan_rsp_set(&adv_scan_data_param);
}

static ble_err_t adv_enable(uint8_t host_id) {
    g_advertising_host_id = host_id;
    return ble_cmd_adv_enable(host_id);
}

static bool fota_sw_timer_start(void) {
    if (g_fota_timer == NULL) {
        return false;
    }

    if (xTimerIsTimerActive(g_fota_timer) == pdFALSE) {
        if (xTimerStart(g_fota_timer, 0) != pdTRUE) {
            return false;
        }
    }

    return true;
}

static void trsp_data_send_from_isr(uint8_t *p_data, uint8_t length) {
    if ((length == 0U) || (g_data_send_pending == true)) {
        return;
    }

    g_rx_buffer_length[g_uart_index] = length;
    memcpy(&g_rx_buffer[g_uart_index][0], p_data, length);

    g_uart_index++;
    if (g_uart_index == UART_BUFF_DEPTH) {
        g_uart_index = 0;
    }

    (void)ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_TRSPS_DATA_SEND, true);
}

static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr) {
    app_queue_t p_app_q;

    p_app_q.event = 0;
    p_app_q.param_type = QUEUE_TYPE_APP_REQ;
    p_app_q.param.app_req.host_id = host_id;
    p_app_q.param.app_req.app_req = request;

    if (!from_isr) {
        if (xSemaphoreTake(semaphore_app, 0) != pdTRUE) {
            return false;
        }

        p_app_q.from_isr = false;
        if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE) {
            xSemaphoreGive(semaphore_app);
            return false;
        }
        return true;
    }

    BaseType_t context_switch = pdFALSE;
    if (xSemaphoreTakeFromISR(semaphore_isr, &context_switch) != pdTRUE) {
        return false;
    }

    p_app_q.from_isr = true;
    if (xQueueSendToBackFromISR(g_app_msg_q, &p_app_q, &context_switch) != pdTRUE) {
        xSemaphoreGiveFromISR(semaphore_isr, &context_switch);
        return false;
    }

    return true;
}

static ble_err_t ble_app_event_cb(void *p_param) {
    ble_err_t status = BLE_ERR_OK;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    size_t alloc_size = sizeof(ble_tlv_t) + sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length;

    if (xSemaphoreTake(semaphore_cb, 0) != pdTRUE) {
        return BLE_BUSY;
    }

    p_tlv = pvPortMalloc(alloc_size);
    if (p_tlv == NULL) {
        xSemaphoreGive(semaphore_cb);
        return BLE_ERR_DATA_MALLOC_FAIL;
    }

    p_app_q.param_type = QUEUE_TYPE_OTHERS;
    p_app_q.param.pt_tlv = p_tlv;
    p_tlv->type = APP_GENERAL_EVENT;
    memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);

    if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE) {
        status = BLE_BUSY;
        vPortFree(p_tlv);
        xSemaphoreGive(semaphore_cb);
    }

    return status;
}

static ble_err_t ble_service_data_cb(void *p_param) {
    ble_err_t status = BLE_ERR_OK;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_evt_att_param_t *p_evt_att = p_param;
    size_t alloc_size = sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length;

    if (xSemaphoreTake(semaphore_cb, 0) != pdTRUE) {
        return BLE_BUSY;
    }

    p_tlv = pvPortMalloc(alloc_size);
    if (p_tlv == NULL) {
        xSemaphoreGive(semaphore_cb);
        return BLE_ERR_DATA_MALLOC_FAIL;
    }

    p_app_q.param_type = QUEUE_TYPE_OTHERS;
    p_app_q.param.pt_tlv = p_tlv;
    p_tlv->type = APP_SERVICE_EVENT;
    memcpy(p_tlv->value, p_param, sizeof(ble_evt_att_param_t) + p_evt_att->length);

    if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE) {
        status = BLE_BUSY;
        vPortFree(p_tlv);
        xSemaphoreGive(semaphore_cb);
    }

    return status;
}

static ble_err_t ble_l2cap_data_cb(void *p_param) {
    ble_err_t status = BLE_ERR_OK;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_l2cap_evt_param_t *p_evt_l2cap = p_param;
    size_t alloc_size = sizeof(ble_tlv_t) + sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length;

    if (xSemaphoreTake(semaphore_cb, 0) != pdTRUE) {
        return BLE_BUSY;
    }

    p_tlv = pvPortMalloc(alloc_size);
    if (p_tlv == NULL) {
        xSemaphoreGive(semaphore_cb);
        return BLE_ERR_DATA_MALLOC_FAIL;
    }

    p_app_q.param_type = QUEUE_TYPE_OTHERS;
    p_app_q.param.pt_tlv = p_tlv;
    p_tlv->type = APP_L2CAP_DATA_EVENT;
    memcpy(p_tlv->value, p_param, sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);

    if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE) {
        status = BLE_BUSY;
        vPortFree(p_tlv);
        xSemaphoreGive(semaphore_cb);
    }

    return status;
}

static ble_err_t server_profile_init(uint8_t host_id) {
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    ble_app_link_info[host_id].state = STATE_STANDBY;

    status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_SERVER, &p_profile_info->svcs_info_gaps, NULL);
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_svcs_gaps_device_name_set((uint8_t *)DEVICE_NAME_STR, strlen((const char *)DEVICE_NAME_STR));
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_svcs_gatts_init(host_id, BLE_GATT_ROLE_SERVER, &p_profile_info->svcs_info_gatts, NULL);
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_SERVER, &p_profile_info->svcs_info_dis, NULL);
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_svcs_trsps_init(host_id, BLE_GATT_ROLE_SERVER, &p_profile_info->svcs_info_trsps, ble_svcs_trsps_evt_handler);
    if (status != BLE_ERR_OK) {
        return status;
    }

    return ble_svcs_fotas_init(host_id, BLE_GATT_ROLE_SERVER, &p_profile_info->svcs_info_fotas, ble_svcs_fotas_evt_handler);
}

static ble_err_t ble_init(void) {
    ble_err_t status = BLE_ERR_OK;
    ble_unique_code_format_t unique_code_param;
    ble_gap_addr_t device_addr_param;

    status = ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb);
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb);
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_host_callback_set(APP_L2CAP_DATA_EVENT, ble_l2cap_data_cb);
    if (status != BLE_ERR_OK) {
        return status;
    }

    status = ble_cmd_phy_controller_init();
    if (status != BLE_ERR_OK) {
        return status;
    }
    ble_hci_cmd_step_delay();
    if (!ble_wait_hci_cmd_queue_ready(pdMS_TO_TICKS(200))) {
        printf("BLE init wait phy_controller_init queue drain timeout\n");
    }

    status = ble_cmd_read_unique_code(&unique_code_param);
    if (status == BLE_ERR_OK) {
        device_addr_param.addr_type = unique_code_param.addr_type;
        memcpy(&device_addr_param.addr, &unique_code_param.ble_addr, 6);
        status = ble_cmd_device_addr_set(&device_addr_param);
        if (status != BLE_ERR_OK) {
            return status;
        }
        ble_hci_cmd_step_delay();
        if (!ble_wait_hci_cmd_queue_ready(pdMS_TO_TICKS(200))) {
            printf("BLE init wait device_addr_set queue drain timeout\n");
        }

        status = ble_cmd_write_identity_resolving_key((ble_sm_irk_param_t *)&unique_code_param.ble_irk[0]);
        if (status != BLE_ERR_OK) {
            return status;
        }
    } else {
        status = ble_cmd_device_addr_set((ble_gap_addr_t *)&DEVICE_ADDR);
        if (status != BLE_ERR_OK) {
            return status;
        }
        ble_hci_cmd_step_delay();
        if (!ble_wait_hci_cmd_queue_ready(pdMS_TO_TICKS(200))) {
            printf("BLE init wait fallback device_addr_set queue drain timeout\n");
        }
    }

    ble_hci_cmd_step_delay();
    if (!ble_wait_hci_cmd_queue_ready(pdMS_TO_TICKS(200))) {
        printf("BLE init wait pre resolvable_address_init queue drain timeout\n");
    }
    status = ble_cmd_resolvable_address_init();
    if (status != BLE_ERR_OK) {
        return status;
    }
    ble_hci_cmd_step_delay();
    if (!ble_wait_hci_cmd_queue_ready(pdMS_TO_TICKS(200))) {
        printf("BLE init wait resolvable_address_init queue drain timeout\n");
    }

    status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
    if (status != BLE_ERR_OK) {
        return status;
    }
    ble_hci_cmd_step_delay();

    status = server_profile_init(APP_TRSP_P_HOST_ID);
    if (status != BLE_ERR_OK) {
        return status;
    }

    ble_fota_fw_buffer_flash_check();
    ble_fota_init();

    return BLE_ERR_OK;
}

static void app_uart_init(void) {
    hosal_uart_init(&trsp_uart_dev);
    hosal_uart_callback_set(&trsp_uart_dev, HOSAL_UART_RX_CALLBACK, uart0_rx_callback, &trsp_uart_dev);
    hosal_uart_callback_set(&trsp_uart_dev, HOSAL_UART_TX_DMA_CALLBACK, NULL, &trsp_uart_dev);
    hosal_uart_ioctl(&trsp_uart_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);
}

static void ble_svcs_fotas_evt_handler(ble_evt_att_param_t *p_param) {
    if (p_param->gatt_role != BLE_GATT_ROLE_SERVER) {
        return;
    }

    switch (p_param->event) {
    case BLESERVICE_FOTAS_DATA_WRITE_WITHOUT_RSP_EVENT:
        ble_fota_data(p_param->host_id, p_param->length, p_param->data);
        (void)fota_sw_timer_start();
        break;

    case BLESERVICE_FOTAS_COMMAND_WRITE_EVENT:
        ble_fota_cmd(p_param->host_id, p_param->length, p_param->data);
        (void)fota_sw_timer_start();
        break;

    default:
        break;
    }
}

static void ble_trsp_task(void *parameters_ptr) {
    app_queue_t p_app_q;
    ble_err_t status = ble_init();
    (void)parameters_ptr;

    if (status != BLE_ERR_OK) {
        printf("BLE init failed: %d\n", status);
        vTaskDelete(NULL);
        return;
    }

    g_uart_index = 0;
    g_uart_transmit_index = 0;
    g_data_send_pending = false;
    if (!ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_ADV_START, false)) {
        printf("BLE APP_REQUEST_ADV_START enqueue failed\n");
    }

    for (;;) {
        if (xQueueReceive(g_app_msg_q, &p_app_q, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        switch (p_app_q.param_type) {
        case QUEUE_TYPE_APP_REQ:
            if (p_app_q.from_isr) {
                xSemaphoreGive(semaphore_isr);
            } else {
                xSemaphoreGive(semaphore_app);
            }
            ble_app_main(&p_app_q.param.app_req);
            break;

        case QUEUE_TYPE_OTHERS:
            if (p_app_q.param.pt_tlv == NULL) {
                break;
            }

            switch (p_app_q.param.pt_tlv->type) {
            case APP_GENERAL_EVENT:
                ble_evt_handler((ble_evt_param_t *)p_app_q.param.pt_tlv->value);
                break;

            case APP_SERVICE_EVENT: {
                ble_evt_att_param_t *p_svcs_param = (ble_evt_att_param_t *)p_app_q.param.pt_tlv->value;

                switch (p_svcs_param->gatt_role) {
                case BLE_GATT_ROLE_CLIENT:
                    att_db_link[p_svcs_param->host_id].p_client_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                    break;

                case BLE_GATT_ROLE_SERVER:
                    att_db_link[p_svcs_param->host_id].p_server_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                    break;

                default:
                    break;
                }
                break;
            }

            case APP_L2CAP_DATA_EVENT:
            default:
                break;
            }

            vPortFree(p_app_q.param.pt_tlv);
            xSemaphoreGive(semaphore_cb);
            break;

        default:
            break;
        }
    }
}

void ble_trsp_app_init(void) {
    ble_task_priority_t ble_task_level;

    g_app_msg_q = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
    semaphore_cb = xSemaphoreCreateCounting(BLE_APP_CB_QUEUE_SIZE, BLE_APP_CB_QUEUE_SIZE);
    semaphore_isr = xSemaphoreCreateCounting(APP_ISR_QUEUE_SIZE, APP_ISR_QUEUE_SIZE);
    semaphore_app = xSemaphoreCreateCounting(APP_REQ_QUEUE_SIZE, APP_REQ_QUEUE_SIZE);

    if (g_app_msg_q == NULL) {
        printf("BLE app queue create failed\n");
        return;
    }

    if (semaphore_cb == NULL) {
        printf("BLE semaphore_cb create failed\n");
        return;
    }

    if (semaphore_isr == NULL) {
        printf("BLE semaphore_isr create failed\n");
        return;
    }

    if (semaphore_app == NULL) {
        printf("BLE semaphore_app create failed\n");
        return;
    }

    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;
    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    if (ble_host_stack_init(&ble_task_level) != 0) {
        printf("BLE host stack init failed\n");
        return;
    }

    app_uart_init();
    g_fota_timer = xTimerCreate("fota-tmr", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, fota_timer_handler);
    if (g_fota_timer == NULL) {
        printf("BLE FOTA timer create failed\n");
        return;
    }

    if (xTaskCreate(ble_trsp_task,
                    "app-ble",
                    BLE_TRSP_TASK_STACK_SIZE,
                    NULL,
                    BLE_TRSP_TASK_PRIORITY,
                    NULL) != pdPASS) {
        printf("BLE app task create failed\n");
    }
}
