/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file cpc_uart.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-02
 * 
 * 
 */

#include <sys/errno.h>

#include <stdint.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <hosal_gpio.h>
#include <hosal_uart.h>
#include <hosal_wdt.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <task.h>
#include <timers.h>
#include "log.h"
#include "queue.h"

#include <cpc.h>
#include <cpc_user_interface.h>
#include "cpc_api.h"
#include "cpc_crc.h"
#include "cpc_drv.h"
#include "cpc_hdlc.h"
// #include "cpc_internal.h"
#include "cpc_system_common.h"

#include "cli.h"
#include "hosal_rf.h"
#include "hosal_swi.h"
#include "lmac15p4.h"
#include "shell.h"
#include "util_string.h"

#include "app_syslog.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
#ifndef CONFIG_APP_EZMESH_OPERATION_UART_PORT
#define CONFIG_APP_EZMESH_OPERATION_UART_PORT 1
#endif // !CONFIG_APP_EZMESH_OPERATION_UART_PORT

#ifndef CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN
#define CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN 28
#endif // !CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN

#ifndef CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN
#define CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN 29
#endif // !CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN

#ifndef CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE
#define CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE 2000000
#endif // !CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE

#if (CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_EZMESH_OPERATION_UART_PORT,
                    CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
                    CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_115200)

#elif (CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE == 38400)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_EZMESH_OPERATION_UART_PORT,
                    CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
                    CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_38400)

#elif (CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE == 57600)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_EZMESH_OPERATION_UART_PORT,
                    CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
                    CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_57600)
#elif (CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_EZMESH_OPERATION_UART_PORT,
                    CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
                    CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_500000)
#elif (CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_EZMESH_OPERATION_UART_PORT,
                    CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
                    CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_1000000)
#elif (CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_EZMESH_OPERATION_UART_PORT,
                    CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
                    CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_2000000)
#endif // CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE

#define CPC_UART_NOTIFY_HANDEL g_cpc_uart_evt_var
#define CPC_UART_NOTIFY_SIGNAL __cpc_uart_signal()
#define CPC_UART_NOTIFY_ISR(ebit)                                              \
    (CPC_UART_NOTIFY_HANDEL |= ebit);                                          \
    CPC_UART_NOTIFY_SIGNAL;
#define CPC_UART_NOTIFY(ebit)                                                  \
    MCU_ENTER_CRITICAL();                                                      \
    CPC_UART_NOTIFY_HANDEL |= ebit;                                            \
    MCU_EXIT_CRITICAL();                                                       \
    CPC_UART_NOTIFY_SIGNAL;
#define CPC_UART_GET_NOTIFY(ebit)                                              \
    MCU_ATOMIC_SECTION({                                                       \
        ebit = CPC_UART_NOTIFY_HANDEL;                                         \
        CPC_UART_NOTIFY_HANDEL = CPC_UART_EVENT_NONE;                          \
    });
//=============================================================================
//                  Macro Definition
//=============================================================================
#define CPC_UART_RAW_BUFFER_SIZE   (CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH + 16)
#define CPC_UART_CACHE_BUFFER_SIZE (CPC_UART_RAW_BUFFER_SIZE * 1)

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef enum {
    CPC_UART_EVENT_NONE = 0,
    CPC_UART_EVENT_TRIGGER = 0x00000001,
    CPC_UART_EVENT_UART_IN = 0x00000002,
    CPC_UART_EVENT_UART_CTS_STATE = 0x00000004,
    CPC_UART_EVENT_UART_TIMEOUT = 0x00000010,

    CPC_UART_EVENT_CPC_IN = 0x00000100,

    CPC_UART_EVENT_ALL = 0xffffffff,
} cpc_uart_event_t;

typedef struct cpc_uart {
    /* data */
    volatile uint32_t wr_idx;
    volatile uint32_t rd_idx;
    volatile uint32_t cts_state;
    uint8_t rx_cache[CPC_UART_RAW_BUFFER_SIZE];
} cpc_uart_io_t;

typedef enum { PROC_HEADER, PROC_DATA } PROC_STATE;

typedef struct {
    hosal_rf_tx_power_ch_comp_t power;
    hosal_rf_tx_power_comp_seg_t segment;
} cpc_power_t;

typedef struct {
    cpc_power_t bt;
    cpc_power_t zb;
} cpc_power_setting_t;

typedef struct {
    uint16_t dlen;
    uint8_t* pdata;
} _cpc_uart_data_t;

typedef struct {
    uint32_t cnt_rx_bytes;
    uint32_t cnt_tx_bytes;
    uint32_t cnt_uart_breack;
    uint32_t cnt_uart_frame_error;
    uint32_t cnt_uart_parity_error;
    uint32_t cnt_uart_rx_overrun;
} cpc_uart_info_cnt_t;

//=============================================================================
//                  Global Data Definition
//=============================================================================
static int _uart_cnt(int argc, char** argv, cb_shell_out_t log_out,
                     void* pExtra);
TaskHandle_t uart_taskHandle = NULL;
TaskHandle_t cpc_sys_taskHandle = NULL;
static QueueHandle_t cpc_uart_handle;
static uint8_t g_tx_buf[CPC_UART_RAW_BUFFER_SIZE];
static uint8_t g_rx_buf[32];
static volatile cpc_uart_event_t g_cpc_uart_evt_var;

static TimerHandle_t s15p4tx_timer;
static volatile uint32_t g15p4_tx_count = 0, g15p4_tx_delay = 0;
static StreamBufferHandle_t xStreamBuffer;
static cpc_uart_io_t g_uart_io = {
    .wr_idx = 0,
};

volatile uint32_t g_uart_tx_done, g_uart_rx_done;

static volatile uint32_t g_uart_tx_compelete = 1, g_uart_tx_start = 0;

static hosal_uart_dma_cfg_t txdam_cfg = {
    .dma_buf = g_tx_buf,
    .dma_buf_size = sizeof(g_tx_buf),
};
static cpc_uart_info_cnt_t g_uart_err_cnt = {0};

const sh_cmd_t g_cli_cmd_uart_cnt STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "ue",
    .cmd_exec = _uart_cnt,
    .pDescription = "ue\n"
                    "  usage: ue\n"
                    "    e.g. ue",
};

//=============================================================================
//                  Private Function Definition
//=============================================================================

static int _uart_cnt(int argc, char** argv, cb_shell_out_t log_out,
                     void* pExtra) {

    do {
        if (argc > 1) {
            memset(&g_uart_err_cnt, 0, sizeof(g_uart_err_cnt));
        } else {

            log_out("uart-%d ", CONFIG_APP_EZMESH_OPERATION_UART_PORT);
            log_out("tx:%d rx:%d ", g_uart_err_cnt.cnt_tx_bytes,
                    g_uart_err_cnt.cnt_rx_bytes);

            if (g_uart_err_cnt.cnt_uart_breack)
                log_out("bi:%d ", g_uart_err_cnt.cnt_uart_breack);

            if (g_uart_err_cnt.cnt_uart_frame_error)
                log_out("fe:%d ", g_uart_err_cnt.cnt_uart_frame_error);

            if (g_uart_err_cnt.cnt_uart_parity_error)
                log_out("pe:%d ", g_uart_err_cnt.cnt_uart_parity_error);

            if (g_uart_err_cnt.cnt_uart_rx_overrun)
                log_out("oe:%d ", g_uart_err_cnt.cnt_uart_rx_overrun);

            log_out("\r\n");
        }

    } while (0);
    return 0;
}

static void __cpc_uart_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(uart_taskHandle, &pxHigherPriorityTaskWoken);
    } else
        xTaskNotifyGive(uart_taskHandle);
}

static int __uart_tx_callback(void* p_arg) {
    (void)p_arg;

    hosal_swi_trigger(SWI_ID_0, HOSAL_TRIG_0);
    g_uart_tx_compelete = 1;

    return 0;
}

static int __uart_rx_callback(void* p_arg) {
    (void)p_arg;

    uint32_t wr_pos = g_uart_io.wr_idx;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint8_t value[32] = {0};
    uint32_t cnt = hosal_uart_receive(&cpc_uart_dev, value, sizeof(value));

    if (cnt <= 0)
        return 0;

    g_uart_err_cnt.cnt_rx_bytes += cnt;

    uint32_t pos = (wr_pos + cnt) % CPC_UART_RAW_BUFFER_SIZE;
    if (pos == rd_pos) {
        hosal_swi_trigger(HOSAL_SWI0_ID, HOSAL_TRIG_1);
        return 0;
    }

    for (uint32_t i = 0; i < cnt; i++)
        g_uart_io.rx_cache[(wr_pos + i) % CPC_UART_RAW_BUFFER_SIZE] = value[i];

    g_uart_io.wr_idx = pos;
    hosal_swi_trigger(HOSAL_SWI0_ID, HOSAL_TRIG_1);

    return 0;
}

static int __uart_rx_line_status_callback(void* p_arg) {
    (void)p_arg;

    uint32_t lsr = hosal_uart_get_lsr(&cpc_uart_dev);

    if (lsr & UART_LSR_BI)
        g_uart_err_cnt.cnt_uart_breack++;
    if (lsr & UART_LSR_OE)
        g_uart_err_cnt.cnt_uart_rx_overrun++;
    if (lsr & UART_LSR_PE)
        g_uart_err_cnt.cnt_uart_parity_error++;
    if (lsr & UART_LSR_FE)
        g_uart_err_cnt.cnt_uart_frame_error++;

    return 0;
}

#if CONFIG_APP_EZMESH_OPERATION_UART_FLOW_CONTROL
static int __uart_mode_callback(void* p_arg) {
    (void)p_arg;

    g_uart_io.cts_state = gpio_pin_get(CONFIG_APP_EZMESH_OPERATION_UART_RTS_PIN);
    hosal_swi_trigger(HOSAL_SWI0_ID, HOSAL_TRIG_2);

    return 0;
}
#endif

static void _uart_sw_isr_cb(uint32_t sw_id) {
    switch (sw_id) {
        case HOSAL_TRIG_0: {
            CPC_UART_NOTIFY_ISR(CPC_UART_EVENT_TRIGGER);
            break;
        }
        case HOSAL_TRIG_1: {
            CPC_UART_NOTIFY_ISR(CPC_UART_EVENT_UART_IN);
            break;
        }
        case HOSAL_TRIG_2: {
            CPC_UART_NOTIFY_ISR(CPC_UART_EVENT_UART_CTS_STATE);
            break;
        }
        default: break;
    }
}

static size_t _uart_data_read(uint8_t* pbuf, int length) {
    size_t byte_cnt = 0;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t wr_pos = g_uart_io.wr_idx;

    while (rd_pos != wr_pos && byte_cnt < length) {
        pbuf[byte_cnt++] = g_uart_io.rx_cache[rd_pos];
        rd_pos = (rd_pos + 1) % CPC_UART_RAW_BUFFER_SIZE;
    }

    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DISABLE_INTERRUPT, NULL);
    g_uart_io.rd_idx = rd_pos;
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_ENABLE_INTERRUPT, NULL);
    return byte_cnt;
}

static void _tx_power_setting(cpc_power_setting_t setting) {
    hosal_rf_tx_power_ch_comp_t tx_pwr_ch_comp_ctrl;
    hosal_rf_tx_power_comp_seg_t tx_pwr_comp_seg_ctrl;

    // BLE
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET,
                   (void*)&setting.bt.power.modem_type);
    tx_pwr_ch_comp_ctrl = (hosal_rf_tx_power_ch_comp_t){
        .tx_power_stage0 = setting.bt.power.tx_power_stage0,
        .tx_power_stage1 = setting.bt.power.tx_power_stage1,
        .tx_power_stage2 = setting.bt.power.tx_power_stage2,
        .tx_power_stage3 = setting.bt.power.tx_power_stage3,
        .modem_type = setting.bt.power.modem_type};
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET,
                   (void*)&tx_pwr_ch_comp_ctrl);

    tx_pwr_comp_seg_ctrl = (hosal_rf_tx_power_comp_seg_t){
        .segmentA = setting.bt.segment.segmentA,
        .segmentB = setting.bt.segment.segmentB,
        .segmentC = setting.bt.segment.segmentC,
        .modem_type = setting.bt.segment.modem_type};
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void*)&tx_pwr_comp_seg_ctrl);

    // ZIGBEE
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET,
                   (void*)&setting.zb.segment.modem_type);
    tx_pwr_ch_comp_ctrl = (hosal_rf_tx_power_ch_comp_t){
        .tx_power_stage0 = setting.zb.power.tx_power_stage0,
        .tx_power_stage1 = setting.zb.power.tx_power_stage1,
        .tx_power_stage2 = setting.zb.power.tx_power_stage2,
        .tx_power_stage3 = setting.zb.power.tx_power_stage3,
        .modem_type = setting.zb.segment.modem_type};
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET,
                   (void*)&tx_pwr_ch_comp_ctrl);

    tx_pwr_comp_seg_ctrl = (hosal_rf_tx_power_comp_seg_t){
        .segmentA = setting.zb.segment.segmentA,
        .segmentB = setting.zb.segment.segmentB,
        .segmentC = setting.zb.segment.segmentC,
        .modem_type = setting.zb.segment.modem_type};
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void*)&tx_pwr_comp_seg_ctrl);
}

void cpc_set_secondary_rf_cert_band(cpc_system_rf_band_t band) {
    log_info("cert band %d", band);
    cpc_power_setting_t power;
    switch (band) {
        case CERT_BAND_FCC: {
            power = (cpc_power_setting_t){
                .bt = {.power = {HOSAL_RF_MODEM_BLE, 31, 28, 28, 24},
                       .segment = {HOSAL_RF_MODEM_BLE, 38, 38, 39}},
                .zb = {.power = {HOSAL_RF_MODEM_2P4G_OQPSK, 31, 31, 31, 18},
                       .segment = {HOSAL_RF_MODEM_2P4G_OQPSK, 8, 26, 39}}};
            return _tx_power_setting(power);
        }
        case CERT_BAND_CE: {
            power = (cpc_power_setting_t){
                .bt = {.power = {HOSAL_RF_MODEM_BLE, 29, 30, 30, 30},
                       .segment = {HOSAL_RF_MODEM_BLE, 1, 38, 39}},
                .zb = {.power = {HOSAL_RF_MODEM_2P4G_OQPSK, 28, 29, 29, 29},
                       .segment = {HOSAL_RF_MODEM_2P4G_OQPSK, 8, 26, 39}}};
            return _tx_power_setting(power);
        }
        case CERT_BAND_JP: {
            power = (cpc_power_setting_t){
                .bt = {.power = {HOSAL_RF_MODEM_BLE, 31, 28, 25, 19},
                       .segment = {HOSAL_RF_MODEM_BLE, 35, 38, 39}},
                .zb = {.power = {HOSAL_RF_MODEM_2P4G_OQPSK, 31, 31, 31, 10},
                       .segment = {HOSAL_RF_MODEM_2P4G_OQPSK, 13, 26, 34}}};
            return _tx_power_setting(power);
        }
        case CERT_BNAD_DEFAULT:
        default: break;
    }
}

static bool __hdlc_header_validate(uint8_t* hdr) {
    uint16_t hcs = 0;
    if (hdr[CPC_HDLC_FLAG_POS] != CPC_HDLC_FLAG_VAL)
        return false;
    hcs = (uint16_t)(hdr[CPC_HDLC_HCS_POS] | (hdr[CPC_HDLC_HCS_POS + 1] << 8));
    return cpc_validate_crc_sw(hdr, CPC_HDLC_HCS_POS, hcs);
}

static bool __sync_header(uint8_t* buffer, size_t* pos) {
    if (*pos < CPC_HDLC_HEADER_RAW_SIZE)
        return false;

    size_t tmp_pos = *pos;
    size_t num_header_combination = (tmp_pos - CPC_HDLC_HEADER_RAW_SIZE) + 1;

    for (size_t i = 0; i < num_header_combination; i++) {
        if (__hdlc_header_validate(&buffer[i])) {
            if (i != 0 && tmp_pos - i > 0) {
                memmove(&buffer[0], &buffer[i], tmp_pos - i);
            }
            *pos = tmp_pos - i;
            return true;
        }
    }

    *pos = 0;
    // log_warn("i %d", num_header_combination);
    return false;
}

static bool __push_valid_hdlc_frame(uint8_t* buffer, size_t* pos) {
    if (*pos < CPC_HDLC_HEADER_RAW_SIZE)
        return false;

    uint16_t payload_len = (uint16_t)(buffer[CPC_HDLC_LENGTH_POS]
                                      | (buffer[CPC_HDLC_LENGTH_POS + 1] << 8));
    size_t frame_size = payload_len + CPC_HDLC_HEADER_RAW_SIZE;
    if (frame_size > *pos)
        return false;

    MCU_ATOMIC_SECTION({
        cpc_drv_uart_push_header(buffer);
        if (payload_len)
            cpc_drv_uart_push_data(buffer + CPC_HDLC_HEADER_RAW_SIZE,
                                   payload_len);
    });
    // log_info_hexdump("RH", buffer, CPC_HDLC_HEADER_RAW_SIZE);
    // if (payload_len)
    //     log_info_hexdump("RD", buffer + CPC_HDLC_HEADER_RAW_SIZE, payload_len);

    size_t remaining = *pos - frame_size;
    if (remaining > 0)
        memmove(buffer, &buffer[frame_size], remaining);
    *pos = remaining;
    return true;
}

static size_t cpc_uart_get_data(uint8_t* buffer, size_t pos, size_t size) {
    return _uart_data_read(&buffer[pos], size - pos - 1);
}

static void _uart_proces(uint8_t* packet) {
    static size_t pos = 0;
    static PROC_STATE state = PROC_HEADER;
    size_t available_space = CPC_UART_CACHE_BUFFER_SIZE - pos - 1;
    pos += _uart_data_read(&packet[pos], available_space);
    if (pos <= 0)
        return;
    APP_SYSLOG_INSERT(APP_SYSLOG_UART, APP_SYSLOG_UART_RECV, (uint32_t)packet,
                      pos);
    while (1) {
        /* Sepcial pattern for reset */
        // uint8_t isp_pattern[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
        // if(pos >= 7) {
        //     if(memcmp(packet, isp_pattern, 7) == 0) cpc_system_reset(0);
        // }
        switch (state) {
            case PROC_HEADER: {
                if (__sync_header(packet, &pos)) {
                    state = PROC_DATA;
                    break;
                }
                return;
            }
            case PROC_DATA: {
                if (__push_valid_hdlc_frame(packet, &pos)) {
                    state = PROC_HEADER;
                    break;
                }
                return;
            }
            default: return; break;
        }
    };
}

static void _uart_task(void* parameters_ptr) {
    _cpc_uart_data_t* puart_data;
    cpc_buffer_handle_t* buffer_handle;
    cpc_uart_event_t sevent = CPC_UART_EVENT_NONE;
    TickType_t wait_time = portMAX_DELAY;
    static uint8_t uart_packet[CPC_UART_CACHE_BUFFER_SIZE];

    /* Use HOSAL's generic IRQ handler and register app-specific callbacks. */
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_RX_CALLBACK,
                            __uart_rx_callback, &cpc_uart_dev);
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_TX_DMA_CALLBACK,
                            __uart_tx_callback, &cpc_uart_dev);
    hosal_uart_callback_set(&cpc_uart_dev,
                            HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK,
                            __uart_rx_line_status_callback, &cpc_uart_dev);
#if CONFIG_APP_EZMESH_OPERATION_UART_FLOW_CONTROL
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_MODE_CALLBACK,
                            __uart_mode_callback, &cpc_uart_dev);
#endif

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE,
                     NULL);

#if CONFIG_APP_EZMESH_OPERATION_UART_FLOW_CONTROL
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_FLOWMODE_SET, (void*)true);
    vTaskDelay(pdMS_TO_TICKS(100));
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_MODEM_RTS_SET, (void*)false);
    vTaskDelay(pdMS_TO_TICKS(100));
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_MODEM_RTS_SET, (void*)true);

#endif
    if (CONFIG_APP_EZMESH_OPERATION_UART_PORT == 0)
        NVIC_SetPriority(Uart0_IRQn, 0);
    else if (CONFIG_APP_EZMESH_OPERATION_UART_PORT == 1)
        NVIC_SetPriority(Uart1_IRQn, 0);

    memset(uart_packet, 0x00, sizeof(uart_packet));

    while (1) {

        if (ulTaskNotifyTake(pdFALSE, wait_time) != 0) {

            CPC_UART_GET_NOTIFY(sevent);

            if (CPC_UART_EVENT_UART_IN & sevent) {
                // log_info("ur");
                _uart_proces(uart_packet);
                // log_info("ub");
            }
            if (CPC_UART_EVENT_UART_CTS_STATE & sevent) {
                log_info("CTS pin change");
            }

            if (g_uart_tx_compelete == 1) {
                if (g_uart_tx_done == 0) {
                    hosal_gpio_pin_set(20);
                    cpc_drv_trnsmit_complete();
                    // log_info("UTD");
                    g_uart_tx_done = 1;
                }
                if (xQueueReceive(cpc_uart_handle, (void*)&puart_data, 0)
                    == pdPASS) {
                    hosal_gpio_pin_clear(20);
                    g_uart_tx_compelete = 0;
                    if (puart_data->dlen > CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH) {
                        log_error("!");
                        return;
                    }
                    buffer_handle = (cpc_buffer_handle_t*)puart_data->pdata;
                    if (puart_data->dlen > 0) {
                        txdam_cfg.dma_buf_size = CPC_HDLC_HEADER_RAW_SIZE
                                                 + puart_data->dlen + 2;

                        memcpy(txdam_cfg.dma_buf, buffer_handle->hdlc_header,
                               CPC_HDLC_HEADER_RAW_SIZE);
                        memcpy(&txdam_cfg.dma_buf[7], buffer_handle->data,
                               puart_data->dlen);

                        memcpy(&txdam_cfg.dma_buf[7 + puart_data->dlen],
                               buffer_handle->fcs, 2);
                    } else {
                        txdam_cfg.dma_buf_size = CPC_HDLC_HEADER_RAW_SIZE;
                        memcpy(txdam_cfg.dma_buf, buffer_handle->hdlc_header,
                               CPC_HDLC_HEADER_RAW_SIZE);
                    }
                    // log_info("UT");
                    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DMA_TX_START,
                                     &txdam_cfg);
                    g_uart_err_cnt.cnt_tx_bytes += txdam_cfg.dma_buf_size;
                    g_uart_tx_done = 0;
                    if (puart_data) {
                        vPortFree(puart_data);
                        puart_data = NULL;
                    }
                }
            }
            if (uxQueueSpacesAvailable(cpc_uart_handle) != 16)
                wait_time = 2;
            else
                wait_time = portMAX_DELAY;

            hosal_wdt_kick();
        }
    }
}

static void __cpc_sys_task(void* parameters_ptr) {
    if (cpc_init() == 0) {
        log_info("cpc_init success");
    } else {
        log_error("cpc init fail!");
    }

    while (1) {
        if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0)
            cpc_process_action();
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // ulTaskNotifyTake(pdTRUE, 30);
    }
}

const char* cpc_secondary_app_version(void) { return CONFIG_PROJECT_VERSION; }

int cpc_uart_data_send(cpc_buffer_handle_t* buffer_handle,
                       uint16_t payload_tx_len) {

    _cpc_uart_data_t* puart_data;

    puart_data = (_cpc_uart_data_t*)pvPortMalloc(sizeof(_cpc_uart_data_t));

    puart_data->pdata = (uint8_t*)buffer_handle;
    puart_data->dlen = payload_tx_len;

    // log_info_hexdump("TH", buffer_handle->hdlc_header,
    //                  CPC_HDLC_HEADER_RAW_SIZE);
    // if (payload_tx_len > 0)
    //     log_info_hexdump("TD", buffer_handle->data, payload_tx_len);

    if (xQueueSend(cpc_uart_handle, (void*)&puart_data, portMAX_DELAY)
        == pdPASS) {
        CPC_UART_NOTIFY(CPC_UART_EVENT_CPC_IN);
    } else {
        log_error("Send failed");
    }

    hosal_wdt_kick();
#if 0
    if (payload_tx_len > CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH) {
        MCU_ATOMIC_SECTION(cpc_drv_trnsmit_complete(););
        return -1;
    }

    txdam_cfg.dma_buf_size = CPC_HDLC_HEADER_RAW_SIZE
                             + (payload_tx_len > 0 ? payload_tx_len + 2 : 0);
    memcpy(txdam_cfg.dma_buf, buffer_handle->hdlc_header,
           CPC_HDLC_HEADER_RAW_SIZE);

    if (payload_tx_len > 0) {
        memcpy(&txdam_cfg.dma_buf[CPC_HDLC_HEADER_RAW_SIZE],
               buffer_handle->data, payload_tx_len);
        memcpy(&txdam_cfg.dma_buf[CPC_HDLC_HEADER_RAW_SIZE + payload_tx_len],
               buffer_handle->fcs, 2);
    }

    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DMA_TX_START, &txdam_cfg);
    hosal_gpio_pin_clear(20);

    APP_SYSLOG_INSERT(APP_SYSLOG_UART, APP_SYSLOG_UART_SEND,
                      (uint32_t)txdam_cfg.dma_buf,
                      (uint32_t)txdam_cfg.dma_buf_size);
#endif
    return 0;
}

void cpc_sys_signal(void) {
    if (!xPortIsInsideInterrupt()) {
        xTaskNotifyGive(cpc_sys_taskHandle);
        return;
    }
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
    vTaskNotifyGiveFromISR(cpc_sys_taskHandle, &pxHigherPriorityTaskWoken);
}

void cpc_uart_init(void) {
    /*Init UART In the first place*/
    hosal_uart_init(&cpc_uart_dev);

#if CONFIG_APP_EZMESH_OPERATION_UART_FLOW_CONTROL
    pin_set_mode(CONFIG_APP_EZMESH_OPERATION_UART_CTS_PIN, MODE_UART);
    pin_set_mode(CONFIG_APP_EZMESH_OPERATION_UART_RTS_PIN, MODE_UART);
    hosal_swi_callback_register(HOSAL_SWI0_ID, HOSAL_TRIG_2, _uart_sw_isr_cb);
    log_info("EZMesh port %d (%d,%d), baudrate %d, HWFC Enabled",
             CONFIG_APP_EZMESH_OPERATION_UART_PORT,
             CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
             CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
             CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE);
#else
    log_info("EZMesh port %d (%d,%d), baudrate %d, HWFC Disabled",
             CONFIG_APP_EZMESH_OPERATION_UART_PORT,
             CONFIG_APP_EZMESH_OPERATION_UART_RX_PIN,
             CONFIG_APP_EZMESH_OPERATION_UART_TX_PIN,
             CONFIG_APP_EZMESH_OPERATION_UART_BAUDRATE);
#endif
    APP_SYSLOG_INSERT(APP_SYSLOG_UART, APP_SYSLOG_UART_INIT,
                      cpc_uart_dev.config.uart_id,
                      cpc_uart_dev.config.baud_rate);
    hosal_gpio_cfg_output(20);
    hosal_gpio_cfg_output(15);
    hosal_gpio_cfg_output(14);

    hosal_gpio_pin_clear(14);
    hosal_gpio_pin_clear(15);

    cpc_uart_handle = xQueueCreate(16, sizeof(void*));
    g_uart_tx_done = 1;
    hosal_swi_callback_register(HOSAL_SWI0_ID, HOSAL_TRIG_0, _uart_sw_isr_cb);
    hosal_swi_callback_register(HOSAL_SWI0_ID, HOSAL_TRIG_1, _uart_sw_isr_cb);
    NVIC_SetPriority(Soft_IRQn, 3);
    CPC_TASK_CREATE(_uart_task, "app-uart", 512, NULL, E_TASK_PRIORITY_HIGHEST,
                    &uart_taskHandle);
    CPC_TASK_CREATE(__cpc_sys_task, "ezmesh-thread", 512, NULL,
                    E_TASK_PRIORITY_NORMAL, &cpc_sys_taskHandle);
}

uint32_t cpc_drv_get_bus_speed(void) {
    switch (cpc_uart_dev.config.baud_rate) {
        case UART_BAUDRATE_57600: return 57600;
        case UART_BAUDRATE_500000: return 500000;
        case UART_BAUDRATE_115200: return 115200;
        case UART_BAUDRATE_1000000: return 1000000;
        case UART_BAUDRATE_2000000: return 2000000;
        default: break;
    }
    return -1;
}
