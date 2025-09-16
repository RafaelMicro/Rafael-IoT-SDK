#include "main.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include "cli.h"
#include "hosal_dma.h"
#include "hosal_gpio.h"
#include "hosal_rf.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "hosal_uart.h"
#include "lmac15p4.h"
#include "log.h"
#include "mac_frame_gen.h"

/*subg use*/
#include "subg_ctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"

#define GPIO_LED_0 20
#define GPIO_LED_1 21
#define GPIO_LED_2 22

#define BUTTON_EVENT_0    0
#define BUTTON_EVENT_1    1
#define BUTTON_EVENT_2    2
#define BUTTON_EVENT_3    3
#define BUTTON_EVENT_4    4
#define BUTTON_EVENT_NONE 0xFF

#define RX_BUF_SIZE   128
#define TX_BUF_SIZE   128
#define GPIO_LED      22
#define GPIO_SWITCH_0 17
#define GPIO_SWITCH_1 21

#define SUBG_MAC (true)

#define RUCI_HEADER_LENGTH     (1)
#define RUCI_SUB_HEADER_LENGTH (1)
#define RUCI_LENGTH            (2)
#define RUCI_PHY_STATUS_LENGTH (3)
#define RX_CONTROL_FIELD_LENGTH                                                \
    (RUCI_HEADER_LENGTH + RUCI_SUB_HEADER_LENGTH + RUCI_LENGTH                 \
     + RUCI_PHY_STATUS_LENGTH)
#define RX_STATUS_LENGTH       (5)
#define FSK_PHR_LENGTH         (2)
#define OQPSK_PHR_LENGTH       (1)
#define CRC16_LENGTH           (2)
#define CRC32_LENGTH           (4)
#define FSK_MAX_RF_LEN         2063 //2047+16
#define OQPSK_MAX_RF_LEN       142  //127+15
#define FSK_RX_HEADER_LENGTH   (RX_CONTROL_FIELD_LENGTH + FSK_PHR_LENGTH)
#define OQPSK_RX_HEADER_LENGTH (RX_CONTROL_FIELD_LENGTH + OQPSK_PHR_LENGTH)
#define RX_APPEND_LENGTH       (RX_STATUS_LENGTH + CRC16_LENGTH)
#define FSK_RX_LENGTH                                                          \
    (FSK_MAX_RF_LEN - FSK_RX_HEADER_LENGTH - RX_APPEND_LENGTH) //2047
#define OQPSK_RX_LENGTH                                                        \
    (OQPSK_MAX_RF_LEN - OQPSK_RX_HEADER_LENGTH - RX_APPEND_LENGTH) //127

#if SUBG_MAC
#define SUBG_PHY_TURNAROUND_TIMER  1000
#define SUBG_PHY_CCA_DETECTED_TIME 640 // 8 symbols for 50 kbps-data rate
#define SUBG_PHY_CCA_DETECT_MODE   0
#define SUBG_PHY_CCA_THRESHOLD     65

#define SUBG_MAC_UNIT_BACKOFF_PERIOD           320
#define SUBG_MAC_MAC_ACK_WAIT_DURATION         16000
#define SUBG_MAC_MAC_MAX_BE                    5
#define SUBG_MAC_MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define SUBG_MAC_MAC_MAX_FRAME_RETRIES         3
#define SUBG_MAC_MAC_MAX_CSMACA_BACKOFFS       4
#define SUBG_MAC_MAC_MIN_BE                    3
#endif

typedef enum {
    SUBG_TRANSFER_TX_MODE = 0x01,
    SUBG_TRANSFER_SLEEP_TX_MODE = 0x02,
    SUBG_TRANSFER_RX_MODE = 0x03
} subg_transfer_mode_t;

typedef enum {
    APP_BUTTON_EVT,
    APP_TX_DONE_EVT,
    APP_RX_DONE_EVT,
    APP_TX_TIMER_EVT,
    APP_RX_TIMER_EVT
} app_evt_t;

typedef struct {
    uint32_t event;
    uint32_t data;
} app_queue_t;

xQueueHandle app_msg_q;
static TimerHandle_t tx_timer;
static TimerHandle_t rx_timer;

/* g_rx_total_count = g_crc_success_count + g_crc_fail_count*/
uint32_t g_crc_success_count;
uint32_t g_crc_fail_count;
uint32_t g_rx_total_count;
uint32_t g_rx_total_count_last; // last rx count
uint32_t g_rx_timeout_count;
/* g_tx_total_count = g_tx_success_Count + g_tx_fail_Count*/
uint16_t g_tx_total_count;
uint16_t g_tx_fail_Count;
uint16_t g_tx_success_Count;
uint32_t g_tx_csmaca_fail_cnt;
uint32_t g_tx_no_ack_cnt;
uint32_t g_tx_fail_cnt;

/* Use PRBS9 as data content*/
const uint8_t g_Prbs9Content[] = PRBS9_CONTENT;

/* Rx buffer to store data from RFB*/
uint8_t g_prbs9_buf[FSK_RX_LENGTH];

/* TX length for TX transmit test*/
uint16_t g_tx_len;

static subg_ctrl_modulation_t modem_type = SUBG_CTRL_MODU_FSK;

/* Supported frequency lists for gpio 31, 30, 29, 28, 23, 14, 9
   User can modify content of g_freq_support to change supported frequency. */
uint32_t g_freq_support[7] = {903000, 907000, 911000, 915000,
                              919000, 923000, 927000}; //Unit: kHz

static bool RF_Rx_Switch = false;

xQueueHandle app_msg_q;

uint8_t keyevent = BUTTON_EVENT_NONE;
static void subg_mac_tx_done(uint32_t tx_status);

static void subg_mac_rx_done(uint16_t packet_length, uint8_t* rx_data_address,
                             uint8_t crc_status, uint8_t rssi, uint8_t snr);

static subg_transfer_mode_t transfer_mode_get() {
    /* 
    1. SUBG_TRANSFER_TX_MODE: Tester sends a certain number of packets
    2. SUBG_TRANSFER_RX_MODE: Tester receives and verify packets
    */
    subg_transfer_mode_t transfer_mode;
    uint32_t pin_value; 

    hosal_gpio_pin_get(15, &pin_value);
    if (pin_value == 0) {
        transfer_mode = SUBG_TRANSFER_RX_MODE;
    } else {
        transfer_mode = SUBG_TRANSFER_TX_MODE;
    }
    return transfer_mode;
}

static void test_auto_state_set(bool rxOnWhenIdle) {
    lmac15p4_auto_state_set(rxOnWhenIdle);
    RF_Rx_Switch = rxOnWhenIdle;
}

void led_on(uint32_t led) { hosal_gpio_pin_write(led, 0); }

void led_off(uint32_t led) { hosal_gpio_pin_write(led, 1); }

void gpio_frequency_chek() {
    uint32_t pin31_value, pin30_value, pin29_value, pin28_value, 
             pin23_value, pin14_value, pin9_value;

    hosal_gpio_pin_get(31, &pin31_value);
    hosal_gpio_pin_get(30, &pin30_value);
    hosal_gpio_pin_get(29, &pin29_value);
    hosal_gpio_pin_get(28, &pin28_value);
    hosal_gpio_pin_get(23, &pin23_value);
    hosal_gpio_pin_get(14, &pin14_value);
    hosal_gpio_pin_get(9, &pin9_value);
    if (pin31_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[0]);
        printf("RF Frequency is 903MHz\r\n");
    } else if (pin30_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[1]);
        printf("RF Frequency is 907MHz\r\n");
    } else if (pin29_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[2]);
        printf("RF Frequency is 911MHz\r\n");
    } else if (pin28_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[3]);
        printf("RF Frequency is 915MHz\r\n");
    } else if (pin23_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[4]);
        printf("RF Frequency is 919MHz\r\n");
    } else if (pin14_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[5]);
        printf("RF Frequency is 923MHz\r\n");
    } else if (pin9_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[6]);
        printf("RF Frequency is 927MHz\r\n");
    } else {
        subg_ctrl_frequency_set(g_freq_support[0]);
        printf("RF Frequency is 903MHz\r\n");
    }
}

void subg_cfg_set(subg_ctrl_modulation_t mode, uint8_t data_rate) {
    modem_type = mode;
    subg_ctrl_idle_set();
    if (mode == SUBG_CTRL_MODU_FSK) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_915M);

        lmac15p4_callback_t mac_cb;
        mac_cb.rx_cb = subg_mac_rx_done;
        mac_cb.tx_cb = subg_mac_tx_done;
        lmac15p4_cb_set(0, &mac_cb);

        subg_ctrl_modem_config_set(mode, data_rate, SUBG_CTRL_FSK_MOD_1);

        subg_ctrl_mac_set(mode, SUBG_CTRL_CRC_TYPE_16,
                          SUBG_CTRL_WHITEN_DISABLE);

        subg_ctrl_preamble_set(mode, 8);

        subg_ctrl_sfd_set(mode, 0x00007209);

        subg_ctrl_filter_set(mode, SUBG_CTRL_FILTER_TYPE_GFSK);
    } else {
        lmac15p4_init(LMAC15P4_SUBG_OQPSK, HOSAL_RF_BAND_SUBG_915M);

        lmac15p4_callback_t mac_cb;
        mac_cb.rx_cb = subg_mac_rx_done;
        mac_cb.tx_cb = subg_mac_tx_done;
        lmac15p4_cb_set(0, &mac_cb);

        subg_ctrl_modem_config_set(mode, data_rate, SUBG_CTRL_FSK_MOD_1);

        subg_ctrl_mac_set(mode, SUBG_CTRL_CRC_TYPE_16,
                          SUBG_CTRL_WHITEN_DISABLE);
    }

    gpio_frequency_chek();
}

void fsk_data_gen(uint8_t* pbuf, uint16_t len) {
    uint8_t idx;
    for (idx = 0; idx < (len >> 8); idx++) {
        memcpy(pbuf + (idx << 8), &(g_Prbs9Content[0]), 0x100);
    }
    if (len & 0xFF) {
        memcpy(pbuf + (idx << 8), &(g_Prbs9Content[0]), (len & 0xFF));
    }
}

void subg_data_gen(MacBuffer_t* MacBuf, uint8_t* tx_control, uint8_t* Dsn) {
    uint16_t mac_data_len = 0;
    uint16_t max_length = ((modem_type == SUBG_CTRL_MODU_FSK) ? 2045 : 125);

    mac_data_len = 100;
    if (mac_data_len > max_length) {
        mac_data_len = max_length;
    }
    *Dsn = (uint8_t)((g_tx_total_count) & 0x7F);
    Rfb_MacFrameGen(modem_type, MacBuf, tx_control, *Dsn, mac_data_len);

    g_tx_len = MacBuf->len;
}

static void button_cb(uint32_t pin, void* isr_param) {
    uint32_t pin_value;

    hosal_gpio_pin_get(pin, &pin_value);
    printf("GPIO%d=%d\r\r\n", pin, pin_value);
    BaseType_t context_switch;
    app_queue_t t_app_q;

    switch (pin) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            t_app_q.event = APP_BUTTON_EVT;
            t_app_q.data = pin;

            xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
            break;

        default: break;
    }

    return;
}

static void app_button_process(uint32_t pin) {
    led_off(GPIO_LED_0);
    led_off(GPIO_LED_1);
    led_off(GPIO_LED_2);
    switch (pin) {
        case 0:
            if (keyevent == BUTTON_EVENT_NONE) {
                /*first press */
                keyevent = BUTTON_EVENT_0;
                subg_cfg_set(SUBG_CTRL_MODU_OPQSK, SUBG_CTRL_DATA_RATE_6P25K);
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    /*close tx timer*/
                    xTimerStop(tx_timer, 0);
                    printf("Tx: OQPSK_6.25K\r\n");
                } else {
                    /*close rx timer*/
                    xTimerStop(rx_timer, 0);
                    /* Disable RX*/
                    test_auto_state_set(false);
                    printf("Rx: OQPSK_6.25K\r\n");
                }
            } else if (keyevent == BUTTON_EVENT_0) {
                /*second press*/
                keyevent = BUTTON_EVENT_NONE;
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    g_tx_no_ack_cnt = 0;
                    g_tx_total_count = 0;
                    xTimerStart(tx_timer, 0);
                    printf("Tx: OQPSK_6.25K, Transmission Start\r\n");
                } else {
                    g_crc_success_count = 0;
                    g_rx_total_count = 0;
                    led_on(GPIO_LED_1);
                    /* Enable RX*/
                    test_auto_state_set(true);
                    xTimerStart(rx_timer, 0);
                    printf("Rx: OQPSK_6.25K, Receiving start\r\n");
                }
            } else {
                xTimerStop(tx_timer, 0);
                xTimerStop(rx_timer, 0);
                keyevent = BUTTON_EVENT_NONE;
            }
            break;
        case 1:
            if (keyevent == BUTTON_EVENT_NONE) {
                /*first press */
                keyevent = BUTTON_EVENT_1;
                subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_50K);
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    /*close tx timer*/
                    xTimerStop(tx_timer, 0);
                    printf("Tx: SUBG_CTRL_DATA_RATE_50K\r\n");
                } else {
                    /*close rx timer*/
                    xTimerStop(rx_timer, 0);
                    /* Disable RX*/
                    test_auto_state_set(false);
                    printf("Rx: SUBG_CTRL_DATA_RATE_50K\r\n");
                }
            } else if (keyevent == BUTTON_EVENT_1) {
                /*second press*/
                keyevent = BUTTON_EVENT_NONE;
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    g_tx_no_ack_cnt = 0;
                    g_tx_total_count = 0;
                    xTimerStart(tx_timer, 0);
                    printf(
                        "Tx: SUBG_CTRL_DATA_RATE_50K, Transmission Start\r\n");
                } else {
                    g_crc_success_count = 0;
                    g_rx_total_count = 0;
                    led_on(GPIO_LED_1);
                    /* Enable RX*/
                    test_auto_state_set(true);
                    xTimerStart(rx_timer, 0);
                    printf("Rx: SUBG_CTRL_DATA_RATE_50K, Receiving start\r\n");
                }
            } else {
                xTimerStop(tx_timer, 0);
                xTimerStop(rx_timer, 0);
                keyevent = BUTTON_EVENT_NONE;
            }
            break;
        case 2:
            if (keyevent == BUTTON_EVENT_NONE) {
                /*first press */
                keyevent = BUTTON_EVENT_2;
                subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_100K);
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    /*close tx timer*/
                    xTimerStop(tx_timer, 0);
                    printf("Tx: SUBG_CTRL_DATA_RATE_100K\r\n");
                } else {
                    /*close rx timer*/
                    xTimerStop(rx_timer, 0);
                    /* Disable RX*/
                    test_auto_state_set(false);
                    printf("Rx: SUBG_CTRL_DATA_RATE_100K\r\n");
                }
            } else if (keyevent == BUTTON_EVENT_2) {
                /*second press*/
                keyevent = BUTTON_EVENT_NONE;
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    g_tx_no_ack_cnt = 0;
                    g_tx_total_count = 0;
                    xTimerStart(tx_timer, 0);
                    printf(
                        "Tx: SUBG_CTRL_DATA_RATE_100K, Transmission Start\r\n");
                } else {
                    g_crc_success_count = 0;
                    g_rx_total_count = 0;
                    led_on(GPIO_LED_1);
                    /* Enable RX*/
                    test_auto_state_set(true);
                    xTimerStart(rx_timer, 0);
                    printf("Rx: SUBG_CTRL_DATA_RATE_100K, Receiving start\r\n");
                }
            } else {
                xTimerStop(tx_timer, 0);
                xTimerStop(rx_timer, 0);
                keyevent = BUTTON_EVENT_NONE;
            }
            break;
        case 3:
            if (keyevent == BUTTON_EVENT_NONE) {
                /*first press */
                keyevent = BUTTON_EVENT_3;
                subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_200K);
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    /*close tx timer*/
                    xTimerStop(tx_timer, 0);
                    printf("Tx: SUBG_CTRL_DATA_RATE_200K\r\n");
                } else {
                    /*close rx timer*/
                    xTimerStop(rx_timer, 0);
                    /* Disable RX*/
                    test_auto_state_set(false);
                    printf("Rx: SUBG_CTRL_DATA_RATE_200K\r\n");
                }
            } else if (keyevent == BUTTON_EVENT_3) {
                /*second press*/
                keyevent = BUTTON_EVENT_NONE;
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    g_tx_no_ack_cnt = 0;
                    g_tx_total_count = 0;
                    xTimerStart(tx_timer, 0);
                    printf(
                        "Tx: SUBG_CTRL_DATA_RATE_200K, Transmission Start\r\n");
                } else {
                    g_crc_success_count = 0;
                    g_rx_total_count = 0;
                    led_on(GPIO_LED_1);
                    /* Enable RX*/
                    test_auto_state_set(true);
                    xTimerStart(rx_timer, 0);
                    printf("Rx: SUBG_CTRL_DATA_RATE_200K, Receiving start\r\n");
                }
            } else {
                xTimerStop(tx_timer, 0);
                xTimerStop(rx_timer, 0);
                keyevent = BUTTON_EVENT_NONE;
            }
            break;
        case 4:
            if (keyevent == BUTTON_EVENT_NONE) {
                /*first press */
                keyevent = BUTTON_EVENT_4;
                subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_300K);
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    /*close tx timer*/
                    xTimerStop(tx_timer, 0);
                    printf("Tx: SUBG_CTRL_DATA_RATE_300K\r\n");
                } else {
                    /*close rx timer*/
                    xTimerStop(rx_timer, 0);
                    /* Disable RX*/
                    test_auto_state_set(false);
                    printf("Rx: SUBG_CTRL_DATA_RATE_300K\r\n");
                }
            } else if (keyevent == BUTTON_EVENT_4) {
                /*second press*/
                keyevent = BUTTON_EVENT_NONE;
                if (transfer_mode_get() == SUBG_TRANSFER_TX_MODE) {
                    g_tx_no_ack_cnt = 0;
                    g_tx_total_count = 0;
                    xTimerStart(tx_timer, 0);
                    printf(
                        "Tx: SUBG_CTRL_DATA_RATE_300K, Transmission Start\r\n");
                } else {
                    g_crc_success_count = 0;
                    g_rx_total_count = 0;
                    led_on(GPIO_LED_1);
                    /* Enable RX*/
                    test_auto_state_set(true);
                    xTimerStart(rx_timer, 0);
                    printf("Rx: SUBG_CTRL_DATA_RATE_300K, Receiving start\r\n");
                }
            } else {
                xTimerStop(tx_timer, 0);
                xTimerStop(rx_timer, 0);
                keyevent = BUTTON_EVENT_NONE;
            }
            break;

        default: break;
    }
}

static void app_tx_done_process(uint32_t tx_status) {

#if (SUBG_MAC)
    /* tx_status =
    0x00: TX success
    0x40: TX success and ACK is received
    0x80: TX success, ACK is received, and frame pending is true
    */
    if ((tx_status != 0) && (tx_status != 0x40) && (tx_status != 0x80)) {
        g_tx_fail_Count++;
        //printf("Tx done Fail:%d Status:%X\r\n", g_tx_fail_Count, tx_status);
        if (tx_status == 0x10) {
            g_tx_csmaca_fail_cnt++;
        } else if (tx_status == 0x20) {
            g_tx_no_ack_cnt++;
        } else if (tx_status == 0x08) {
            g_tx_fail_cnt++;
        }
    }
#endif

    g_tx_total_count++;
    if (g_tx_total_count >= 300) {
        led_off(GPIO_LED_0);
        if ((g_tx_total_count - g_tx_no_ack_cnt) < (g_tx_total_count * 0.8)) {
            led_on(GPIO_LED_1);
        } else {
            led_on(GPIO_LED_2);
        }
    } else {
        if (keyevent == BUTTON_EVENT_NONE) {
            xTimerStart(tx_timer, 0);
        }
    }

#if (SUBG_MAC)
    printf("Tx (len: %d)done total: %d Fail: %d CCaFail: %d NoAck: %d TxFail: "
           "%d \r\n",
           g_tx_len, g_tx_total_count, g_tx_fail_Count, g_tx_csmaca_fail_cnt,
           g_tx_no_ack_cnt, g_tx_fail_cnt);
#else
    printf("TX (len:%d) done total:%d Fail:%d\r\n", g_tx_len, g_tx_total_count,
           g_tx_fail_Count);
#endif
}

static void app_tx_process() {
#if (SUBG_MAC)
    uint8_t tx_control = 0;
    uint8_t Dsn = 0;
    static MacBuffer_t MacBuf;
#else
    uint16_t max_length = ((modem_type == SUBG_CTRL_MODU_FSK) ? 2047 : 127);
#endif
    led_on(GPIO_LED_0);
#if (SUBG_MAC)
    /* Generate IEEE802.15.4 MAC Header and append data */
    subg_data_gen(&MacBuf, &tx_control, &Dsn);
    int ret = lmac15p4_tx_data_send(0, MacBuf.dptr, MacBuf.len, tx_control,
                                    Dsn);
    g_tx_len = MacBuf.len;
#else
    /* Determine TX packet length*/
    g_tx_len++;
    if (g_tx_len > (max_length - FSK_CRC16_LENGTH)) {
        g_tx_len = PHY_MIN_LENGTH;
    }
    /* Send data */
    lmac15p4_tx_data_send(0, &g_prbs9_buf[0], g_tx_len, 0, 0);
#endif
}

static void app_rx_process() {
    /* Check whether RX data is comming during certain interval */
    if ((RF_Rx_Switch == true) && (g_rx_total_count_last == g_rx_total_count)) {
        printf("[E] No RX data in this period\r\n");
        led_off(GPIO_LED_0);
    }
    xTimerStart(rx_timer, 0);
    g_rx_total_count_last = g_rx_total_count;
}

static void app_main_task(void) {
    app_queue_t app_q;
    for (;;) {
        if (xQueueReceive(app_msg_q, &app_q, 0) == pdTRUE) {
            switch (app_q.event) {
                case APP_BUTTON_EVT: app_button_process(app_q.data); break;
                case APP_TX_DONE_EVT: app_tx_done_process(app_q.data); break;
                case APP_RX_DONE_EVT: break;
                case APP_TX_TIMER_EVT: app_tx_process(); break;
                case APP_RX_TIMER_EVT: app_rx_process(); break;
                default: break;
            }
        }
    }
}

void button_init(void) {
    uint8_t i = 0;
    hosal_gpio_input_config_t pin_cfg;
    /* gpio0 pin setting */
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg.usr_cb = button_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    NVIC_SetPriority(Gpio_IRQn, 7);
    NVIC_EnableIRQ(Gpio_IRQn);
    for (i = 0; i < 5; i++) {
        hosal_gpio_debounce_enable(i);
        hosal_pin_set_pullopt(i, HOSAL_PULL_UP_100K);
        hosal_gpio_cfg_input(i, pin_cfg);
        hosal_gpio_int_enable(i);
    }
}

static void tx_timer_timeout() {
    app_queue_t t_app_q;
    BaseType_t context_switch;

    t_app_q.event = APP_TX_TIMER_EVT;
    t_app_q.data = 0;
    xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
}

static void rx_timer_timeout() {
    app_queue_t t_app_q;
    BaseType_t context_switch;

    t_app_q.event = APP_RX_TIMER_EVT;
    t_app_q.data = 0;
    xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
}

static void subg_mac_tx_done(uint32_t tx_status) {
    app_queue_t t_app_q;
    BaseType_t context_switch;

    t_app_q.event = APP_TX_DONE_EVT;
    t_app_q.data = tx_status;

    xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
}

static void subg_mac_rx_done(uint16_t packet_length, uint8_t* rx_data_address,
                             uint8_t crc_status, uint8_t rssi, uint8_t snr) {
    led_on(GPIO_LED_0);

#if (!SUBG_MAC)
    uint16_t i;
    uint8_t header_length = ((modem_type == SUBG_CTRL_MODU_FSK)
                                 ? FSK_RX_HEADER_LENGTH
                                 : OQPSK_RX_HEADER_LENGTH);
#endif
    uint16_t rx_data_len = 0;
    uint8_t phr_length = ((modem_type == SUBG_CTRL_MODU_FSK)
                              ? FSK_PHR_LENGTH
                              : OQPSK_PHR_LENGTH);
    g_rx_total_count++;
    if (crc_status == 0) {
        /* Calculate PHY payload length*/
        rx_data_len = packet_length
                      - (RUCI_PHY_STATUS_LENGTH + phr_length
                         + RX_APPEND_LENGTH);
#if (!SUBG_MAC)
        /* Verify data content*/
        for (i = 0; i < rx_data_len; i++) {
            if (g_prbs9_buf[i] != *(rx_data_address + header_length + i)) {
                printf("[E] data content error\r\n");
            }
        }
        printf("\r\n");
#endif
        g_crc_success_count++;
    } else {
        g_crc_fail_count++;
    }

    printf("RX (len:%d) done, Success:%d Fail:%d \r\r\n", rx_data_len,
           g_crc_success_count, g_crc_fail_count);
}

void subg_config_init() {
    /* RF system priority set */
    NVIC_SetPriority(Uart0_IRQn, 0x01);
    NVIC_SetPriority(CommSubsystem_IRQn, 0x00);

    /*rf init */
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);
    /*Choose the frequency band you want */
    lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_915M);

    /* Register rfb interrupt event */
    lmac15p4_callback_t mac_cb;
    mac_cb.rx_cb = subg_mac_rx_done;
    mac_cb.tx_cb = subg_mac_tx_done;
    lmac15p4_cb_set(0, &mac_cb);

    subg_ctrl_sleep_set(false);

    subg_ctrl_idle_set();

#if (SUBG_MAC)
    subg_ctrl_modem_config_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_300K,
                               SUBG_CTRL_FSK_MOD_1);

    subg_ctrl_mac_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);

    subg_ctrl_preamble_set(SUBG_CTRL_MODU_FSK, 8);

    subg_ctrl_sfd_set(SUBG_CTRL_MODU_FSK, 0x00007209);

    subg_ctrl_filter_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_FILTER_TYPE_GFSK);

    /* PHY PIB Parameters */
    lmac15p4_phy_pib_set(SUBG_PHY_TURNAROUND_TIMER, SUBG_PHY_CCA_DETECT_MODE,
                         SUBG_PHY_CCA_THRESHOLD, SUBG_PHY_CCA_DETECTED_TIME);

    /* MAC PIB Parameters */
    lmac15p4_mac_pib_set(SUBG_MAC_UNIT_BACKOFF_PERIOD,
                         SUBG_MAC_MAC_ACK_WAIT_DURATION, SUBG_MAC_MAC_MAX_BE,
                         SUBG_MAC_MAC_MAX_CSMACA_BACKOFFS,
                         SUBG_MAC_MAC_MAX_FRAME_TOTAL_WAIT_TIME,
                         SUBG_MAC_MAC_MAX_FRAME_RETRIES, SUBG_MAC_MAC_MIN_BE);

    uint16_t short_addr = 0x1234;

    uint32_t long_addr_0 = 0x11223344;

    uint32_t long_addr_1 = 0x55667788;

    uint16_t pnaid = 0x1AAA;

    lmac15p4_address_filter_set(0, false, short_addr, long_addr_0, long_addr_1,
                                pnaid, true);

    /* AUTO ACK Enable Flag */
    lmac15p4_auto_ack_set(true);

    /* Frame Pending Bit */
    lmac15p4_ack_pending_bit_set(0, true);

    /* Auto State */
    lmac15p4_auto_state_set(true);

    lmac15p4_src_match_ctrl(0, true);
#endif

    /* Init test counters*/
    g_crc_success_count = 0;
    g_crc_fail_count = 0;
    g_rx_total_count = 0;
    g_tx_total_count = 0;

    fsk_data_gen(&g_prbs9_buf[0], FSK_RX_LENGTH);

    uint32_t freq = g_freq_support[0];
    // uint32_t Fw_ver = lmac15p4_get_version();
    printf("Firmware version: %d\r\n", 0);
    gpio_frequency_chek();
}

static void app_main_entry(void* pvParameters)
{
    /* initil SubG*/
    subg_config_init();
    app_main_task();

    while (1) {
    }
}

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    /* led init */
    hosal_gpio_cfg_output(GPIO_LED_0);
    hosal_gpio_cfg_output(GPIO_LED_1);
    hosal_gpio_cfg_output(GPIO_LED_2);
    led_off(GPIO_LED_0);
    led_off(GPIO_LED_1);
    led_off(GPIO_LED_2);

    /* initil dama*/
    hosal_dma_init();

    /* initil cli*/
    cli_init();

    /* initil Button*/
    button_init();

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);

    /* event queue*/
    app_msg_q = xQueueCreate(5, sizeof(app_queue_t));

    /*tx timer*/
    tx_timer = xTimerCreate("tx_timer", pdMS_TO_TICKS(30), pdFALSE, (void*)0,
                            tx_timer_timeout);

    /*rx timer*/
    rx_timer = xTimerCreate("rx_timer", pdMS_TO_TICKS(1000), pdFALSE, (void*)0,
                            rx_timer_timeout);

    printf("GPIO    : Frequency (MHz) : 903Mhz(31), 907Mhz(30), 911Mhz(29),  "
           "915Mhz(28), 919Mhz(23), 923Mhz(14), 927Mhz(9)\r\n");
    printf("Buttion : Data Rate (Kbps) : 6.25Kpbs(0), 50Kpbs(1), 100Kpbs(2), "
           "200Kpbs(3), 300Kpbs(4)\r\n");
    printf("GPIO 15 : Tx Mode(1), Rx Mode(0) \r\n");

    if (xTaskCreate(app_main_entry, (char*)"main",
                    256, NULL, E_TASK_PRIORITY_APP, NULL) != pdPASS) {
        printf("Task create fail....\r\n");
    }

    vTaskStartScheduler();
    return 0;
}
