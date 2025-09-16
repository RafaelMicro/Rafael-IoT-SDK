#include "main.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>
#include "cli.h"
#include "hosal_dma.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
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

#define SUBG_RX_ON_RADIO_SLEEP_TIME 1000

#define SUBG_FREQ_PIN_MAX 7

typedef enum {
    APP_BUTTON_EVT,
    APP_TX_DONE_EVT,
    APP_RX_DONE_EVT,
} app_evt_t;

typedef struct {
    uint32_t event;
    uint32_t data;
} app_queue_t;

static uint16_t g_rx_time = 0;

xQueueHandle app_msg_q;
static SemaphoreHandle_t appSemHandle = NULL;

static subg_ctrl_modulation_t modem_type = SUBG_CTRL_MODU_FSK;

/* Supported frequency lists for gpio 31, 30, 29, 28, 23, 14, 9
   User can modify content of g_freq_support to change supported frequency. */
uint32_t g_freq_support[SUBG_FREQ_PIN_MAX] = {
    903000, 907000, 911000, 915000, 919000, 923000, 927000}; //Unit: kHz

uint8_t g_freq_gpio[SUBG_FREQ_PIN_MAX] = {31, 30, 29, 28, 23, 14, 9};

static bool RF_Rx_Switch = false;

static void subg_mac_tx_done(uint32_t tx_status);

static void subg_mac_rx_done(uint16_t packet_length, uint8_t* rx_data_address,
                             uint8_t crc_status, uint8_t rssi, uint8_t snr);

static void test_auto_state_set(bool rxOnWhenIdle) {
    lmac15p4_auto_state_set(rxOnWhenIdle);
    RF_Rx_Switch = rxOnWhenIdle;
}

void led_on(uint32_t led) { hosal_gpio_pin_write(led, 0); }

void led_off(uint32_t led) { hosal_gpio_pin_write(led, 1); }

uint32_t gpio_frequency_get() {
    uint32_t freq;
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
        freq = g_freq_support[0];
    } else if (pin30_value == 0) {
        freq = g_freq_support[1];
    } else if (pin29_value == 0) {
        freq = g_freq_support[2];
    } else if (pin28_value == 0) {
        freq = g_freq_support[3];
    } else if (pin23_value == 0) {
        freq = g_freq_support[4];
    } else if (pin14_value == 0) {
        freq = g_freq_support[5];
    } else if (pin9_value == 0) {
        freq = g_freq_support[6];
    } else {
        freq = g_freq_support[0];
    }
    return freq;
}

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
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[0]);
    } else if (pin30_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[1]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[1]);
    } else if (pin29_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[2]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[2]);
    } else if (pin28_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[3]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[3]);
    } else if (pin23_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[4]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[4]);
    } else if (pin14_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[5]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[5]);
    } else if (pin9_value == 0) {
        subg_ctrl_frequency_set(g_freq_support[6]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[6]);
    } else {
        subg_ctrl_frequency_set(g_freq_support[0]);
        log_info("RF Frequency is %d kHz\r\n", g_freq_support[0]);
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

        subg_ctrl_modem_config_set(mode, data_rate, 0);

        subg_ctrl_mac_set(mode, SUBG_CTRL_CRC_TYPE_16,
                          SUBG_CTRL_WHITEN_DISABLE);
    }

    gpio_frequency_chek();
}

#if !CONFIG_HOSAL_SOC_IDLE_SLEEP
static void subg_data_transmission(uint16_t rx_on_radio_time) {
    uint8_t tx_control = 0;
    uint8_t dsn = 0;
    uint8_t* mac_tx_data = pvPortMalloc(MAX_DATA_SIZE);
    uint16_t mac_tx_data_lens = 0;
    char wakeup_data[] = "RAFAEL_WAKEUP";
    log_info("mac tx start \r\n");
    if (mac_tx_data) {
        uint32_t curr = xTaskGetTickCount();
        while ((curr + (rx_on_radio_time * 1000)) > xTaskGetTickCount()) {
            led_on(GPIO_LED_1);
            /* Generate IEEE802.15.4 MAC Header and append data */
            subg_mac_broadcast_hdr_gen(mac_tx_data, &mac_tx_data_lens, dsn);
            memcpy(&mac_tx_data[mac_tx_data_lens], wakeup_data,
                   sizeof(wakeup_data));
            mac_tx_data_lens += sizeof(wakeup_data);
            lmac15p4_tx_data_send(0, mac_tx_data, mac_tx_data_lens, tx_control,
                                  dsn);
            dsn++;
            led_off(GPIO_LED_1);
            vTaskDelay(5);
        }
        vPortFree(mac_tx_data);
    }
    log_info("mac tx end\r\n");
}
#endif

static void button_cb(uint32_t pin, void* isr_param) {
    uint32_t pin_value;

    hosal_gpio_pin_get(pin, &pin_value);
    log_info("GPIO%d=%d\r\r\n", pin, pin_value);
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
            xSemaphoreGiveFromISR(appSemHandle, &context_switch);
            break;

        default: break;
    }

    return;
}

static void app_button_process(uint32_t pin) {
    led_off(GPIO_LED_0);
    led_off(GPIO_LED_1);
    led_off(GPIO_LED_2);
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
    hosal_rf_wake_on_radio_t wake_on_radio;
    wake_on_radio.frequency = gpio_frequency_get();
    wake_on_radio.sleep_time = SUBG_RX_ON_RADIO_SLEEP_TIME;
#endif
    switch (pin) {
        case 0:
            subg_cfg_set(SUBG_CTRL_MODU_OPQSK, SUBG_CTRL_DATA_RATE_6P25K);
            g_rx_time = 35;
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
            log_info("6.25K Wake on radio start: sleep %d ms ,rx %d ms \r\n",
                     wake_on_radio.sleep_time, g_rx_time);
#else
            log_info("6.25K TX start %d ms \r\n", g_rx_time);
#endif
            break;
        case 1:
            subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_50K);
            g_rx_time = 8;
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
            log_info("50K Wake on radio start: sleep %d ms ,rx %d ms \r\n",
                     wake_on_radio.sleep_time, g_rx_time);
#else
            log_info("50K TX start %d ms \r\n", g_rx_time);
#endif
            break;
        case 2:
            subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_100K);
            g_rx_time = 6;
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
            log_info("100K Wake on radio start: sleep %d ms ,rx %d ms \r\n",
                     wake_on_radio.sleep_time, g_rx_time);
#else
            log_info("100K TX start %d ms \r\n", g_rx_time);
#endif
            break;
        case 3:
            subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_200K);
            g_rx_time = 4;
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
            log_info("200K Wake on radio start: sleep %d ms ,rx %d ms \r\n",
                     wake_on_radio.sleep_time, g_rx_time);
#else
            log_info("200K TX start %d ms \r\n", g_rx_time);
#endif
            break;
        case 4:
            subg_cfg_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_300K);
            g_rx_time = 3;
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
            log_info("300K Wake on radio start: sleep %d ms ,rx %d ms \r\n",
                     wake_on_radio.sleep_time, g_rx_time);
#else
            log_info("300K TX start %d ms \r\n", g_rx_time);
#endif
            break;

        default: break;
    }
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
    /* Disable RX*/
    test_auto_state_set(false);
    wake_on_radio.rx_on_time = g_rx_time;
    hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_RVD27);
    hosal_rf_ioctl(HOSAL_RF_IOCTL_WAKE_ON_RADIO_SET, &wake_on_radio);
#else
    subg_data_transmission(g_rx_time);
#endif
}

static void app_tx_done_process(uint32_t tx_status) {
    /* tx_status =
    0x00: TX success
    0x40: TX success and ACK is received
    0x80: TX success, ACK is received, and frame pending is true
    */
    if ((tx_status != 0) && (tx_status != 0x40) && (tx_status != 0x80)) {
        log_info("Tx done Status : %X\r\n", tx_status);
    }
}

static void app_rx_done_process(uint32_t rx_data_len) {
    log_info("Wakeup RX done len : %d\r\r\n", rx_data_len);
    led_on(GPIO_LED_0);
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_RVD27);
#endif
}

static void app_main_task(void) {
    app_queue_t app_q;
    appSemHandle = xSemaphoreCreateBinary();

    for (;;) {
        xSemaphoreTake(appSemHandle, portMAX_DELAY);
        while (xQueueReceive(app_msg_q, &app_q, 0) == pdTRUE) {
            switch (app_q.event) {
                case APP_BUTTON_EVT: app_button_process(app_q.data); break;
                case APP_TX_DONE_EVT: app_tx_done_process(app_q.data); break;
                case APP_RX_DONE_EVT: app_rx_done_process(app_q.data); break;
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

static void subg_mac_tx_done(uint32_t tx_status) {
    app_queue_t t_app_q;
    BaseType_t context_switch;

    t_app_q.event = APP_TX_DONE_EVT;
    t_app_q.data = tx_status;

    xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
    xSemaphoreGiveFromISR(appSemHandle, &context_switch);
}

static void subg_mac_rx_done(uint16_t packet_length, uint8_t* rx_data_address,
                             uint8_t crc_status, uint8_t rssi, uint8_t snr) {
    led_on(GPIO_LED_0);

    app_queue_t t_app_q;
    BaseType_t context_switch;

    uint16_t rx_data_len = 0;
    uint8_t phr_length = ((modem_type == SUBG_CTRL_MODU_FSK)
                              ? FSK_PHR_LENGTH
                              : OQPSK_PHR_LENGTH);
    if (crc_status == 0) {
        /* Calculate PHY payload length*/
        rx_data_len = packet_length
                      - (RUCI_PHY_STATUS_LENGTH + phr_length
                         + RX_APPEND_LENGTH);

        t_app_q.event = APP_RX_DONE_EVT;
        t_app_q.data = rx_data_len;
        xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
        xSemaphoreGiveFromISR(appSemHandle, &context_switch);
    }
}

void subg_config_init() {
    /* RF system priority set */
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

    uint16_t short_addr = SUBG_MAC_SHORT_ADDR;

    uint32_t long_addr_0 = (SUBG_MAC_LONG_ADDR >> 32);

    uint32_t long_addr_1 = SUBG_MAC_LONG_ADDR & 0xFFFFFFFF;

    uint16_t pnaid = 0x1AAA;

    lmac15p4_address_filter_set(0, false, short_addr, long_addr_0, long_addr_1,
                                pnaid, true);

    /* AUTO ACK Enable Flag */
    lmac15p4_auto_ack_set(true);

    /* Frame Pending Bit */
    lmac15p4_ack_pending_bit_set(0, true);

    /* Auto State */
    lmac15p4_auto_state_set(false);

    lmac15p4_src_match_ctrl(0, true);

    gpio_frequency_chek();

#if CONFIG_HOSAL_SOC_IDLE_SLEEP
    hosal_rf_wake_on_radio_t wake_on_radio;

    g_rx_time = 3;
    wake_on_radio.frequency = gpio_frequency_get();
    wake_on_radio.rx_on_time = g_rx_time;
    wake_on_radio.sleep_time = SUBG_RX_ON_RADIO_SLEEP_TIME;
    hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_RVD27);
    hosal_rf_ioctl(HOSAL_RF_IOCTL_WAKE_ON_RADIO_SET, &wake_on_radio);
    log_info("300K RX on radio start: sleep %d ms ,rx %d ms \r\n",
             wake_on_radio.sleep_time, wake_on_radio.rx_on_time);
#endif
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

    /* initil cli*/
#if CONFIG_HOSAL_SOC_IDLE_SLEEP
    log_info("SubG Wakeup on radio Sleep\r\n");
#else
    log_info("SubG Wakeup on radio Tx\r\n");
    cli_init();
#endif

    /* initil Button*/
    button_init();

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);

    /* event queue*/
    app_msg_q = xQueueCreate(5, sizeof(app_queue_t));

    log_printk("GPIO : Frequency (kHz) : ");
    for (int i = 0; i < SUBG_FREQ_PIN_MAX; i++) {
        log_printk("%d : %d(khz), ", g_freq_gpio[i], g_freq_support[i]);
    }
    log_printk("\r\n");
    log_info("Buttion : Data Rate (Kbps) : 6.25Kpbs(0), 50Kpbs(1), 100Kpbs(2), "
             "200Kpbs(3), 300Kpbs(4) ");

    if (xTaskCreate(app_main_entry, (char*)"main",
                    256, NULL, E_TASK_PRIORITY_APP, NULL) != pdPASS) {
        printf("Task create fail....\r\n");
    }

    vTaskStartScheduler();
    return 0;
}
