/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfb_sample.c
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
/**************************************************************************************************
*    INCLUDES
*************************************************************************************************/
#include "rfb_sample.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include "hosal_dma.h"
#include "hosal_gpio.h"
#include "hosal_rf.h"
#include "hosal_rtc.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "hosal_uart.h"
#include "lmac15p4.h"
#include "log.h"
#include "mac_frame_gen.h"
#include "lpm.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#define MAC_15P4 (false)

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define RUCI_PHY_STATUS_LENGTH  (3)
#define RX_CONTROL_FIELD_LENGTH (7)
#define RX_STATUS_LENGTH  (5)
#define OQPSK_PHR_LENGTH   (1)
#define CRC16_LENGTH (2)
#define OQPSK_RX_HEADER_LENGTH (RX_CONTROL_FIELD_LENGTH + OQPSK_PHR_LENGTH)
#define OQPSK_RX_APPEND_LENGTH (RX_STATUS_LENGTH + CRC16_LENGTH)
#define RX_LENGTH (MAX_RF_LEN+OQPSK_RX_HEADER_LENGTH+OQPSK_RX_APPEND_LENGTH)
#define PHY_MIN_LENGTH          (3)

#if (MAC_15P4)
#define A_TURNAROUND_TIMR             192
#define A_UNIT_BACKOFF_PERIOD         320
#define MAC_ACK_WAIT_DURATION         544
#define MAC_MAX_BE                    5
#define MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define MAC_MAX_FRAME_RETRIES         3
#define MAC_MAX_CSMACA_BACKOFFS       4
#define MAC_MIN_BE                    3
#endif
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
/* g_rx_total_count = g_crc_success_count + g_crc_fail_count*/
uint32_t             g_crc_success_count;
uint32_t             g_crc_fail_count;
uint32_t             g_rx_total_count;
uint32_t             g_rx_total_count_last; // last rx count
uint32_t             g_rx_timeout_count;
/* g_tx_total_count = g_tx_success_Count + g_tx_fail_Count*/
uint16_t             g_tx_total_count;
uint16_t             g_tx_fail_Count;
uint16_t             g_tx_success_Count;
uint32_t             g_tx_csmaca_fail_cnt;
uint32_t             g_tx_no_ack_cnt;
uint32_t             g_tx_fail_cnt;

/* Burst TX test target*/
uint16_t             g_tx_count_target;

/* TX length for TX transmit test*/
uint16_t             g_tx_len;

/* TX buffer */
uint8_t              g_tx_buf[OQPSK_MAX_DATA_SIZE];

static hosal_rf_tx_power_t sPhyPowerStage = {
    .modem = LMAC15P4_2P4G_OQPSK,
    .band_type = HOSAL_RF_BAND_2P4G,
    .power_index = 127,
};
/* TX power_index
For RT584H, 
[74 ~ 127]: 20dBm Table (-14 dBm to 20 dBm)
[13 ~ 63]: 0dBm Table (-27 dBm to 0 dBm).
For RT584L, 
[74 ~ 127]: 10dBm Table (-20 dBm to 10 dBm)
[10 ~ 63]: 0dBm Table (-35 dBm to 0 dBm).
*/
static rtc_time_t current_time, alarm_tm;
static uint32_t alarm_mode;
/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static void rfb_tx_done(uint32_t tx_status)
{
    g_tx_total_count++;

    /* tx_status =
    0x00: TX success
    0x40: TX success and ACK is received
    0x80: TX success, ACK is received, and frame pending is true
    */
    if ((tx_status != 0) && (tx_status != 0x40) && (tx_status != 0x80))
    {
        g_tx_fail_Count ++;

        if (tx_status == 0x10)
        {
            g_tx_csmaca_fail_cnt++;
        }
        else if (tx_status == 0x20)
        {
            g_tx_no_ack_cnt++;
        }
        else if (tx_status == 0x08)
        {
            g_tx_fail_cnt++;
        }
    }

    printf("Tx (len:%d)done total:%d Fail:%d CaFail:%d NoAck:%d TxFail%d \n", g_tx_len, g_tx_total_count, g_tx_fail_Count, g_tx_csmaca_fail_cnt, g_tx_no_ack_cnt, g_tx_fail_cnt);
    xTimerStart(tx_timer, 0);
}

static void rfb_rx_done(uint16_t ruci_packet_length, uint8_t *rx_data_address, uint8_t crc_status, uint8_t rssi, uint8_t snr)
{
    uint16_t i;
    uint16_t rx_data_len;
    uint8_t header_length = OQPSK_RX_HEADER_LENGTH;
    uint8_t phr_length = OQPSK_PHR_LENGTH;

    g_rx_total_count++;
    rx_data_len = ruci_packet_length - (RUCI_PHY_STATUS_LENGTH + phr_length + OQPSK_RX_APPEND_LENGTH);

    if (crc_status == 0)
    {
        printf("RX packet:");
        for (i = 0; i < rx_data_len; i++)
        {
            printf(" %x", *(rx_data_address + header_length + i));
        }
        printf("\n");
        g_crc_success_count ++;
    }
    else
    {
        g_crc_fail_count ++;
    }
    printf("RX (len:%d) done, Success:%d Fail:%d\n", rx_data_len, g_crc_success_count, g_crc_fail_count);
}

/* TRX proccess and related function */
bool burst_tx_abort(void)
{
    if (g_tx_total_count != g_tx_count_target)
    {
        return false;
    }
    return true;
}

void mac_data_gen(MacBuffer_t *MacBuf, uint8_t *tx_control, uint8_t *Dsn)
{
    uint8_t mac_data_len = 0;

    mac_data_len = (uint8_t)((g_tx_total_count) & 0x7F);
    if (mac_data_len > 100)
    {
        mac_data_len = 100;
    }
    *Dsn = (uint8_t)((g_tx_total_count) & 0x7F);
    Rfb_MacFrameGen(MacBuf, tx_control, *Dsn, mac_data_len);

    g_tx_len = MacBuf->len;
}

void app_tx_process(uint32_t rfb_pci_test_case) {
#if (MAC_15P4)
    uint8_t tx_control = 0;
    uint8_t Dsn = 0;
    static MacBuffer_t MacBuf;
#endif
    /* Abort test if TX count is reached in burst tx test */
    if (burst_tx_abort())
    {
        return;
    }

    switch (rfb_pci_test_case)
    {
    case MAC_TX_PACKETS_TEST:
#if (MAC_15P4)
        /* Generate IEEE802.15.4 MAC Header and append data */
        mac_data_gen(&MacBuf, &tx_control, &Dsn);
        lmac15p4_tx_data_send(0, MacBuf.dptr, MacBuf.len, tx_control, Dsn);
        g_tx_len = MacBuf.len;
#else
        g_tx_len ++;
        /* Determine TX packet length*/
        if (g_tx_len > OQPSK_MAX_DATA_SIZE)
        {
            g_tx_len = OQPSK_MAX_DATA_SIZE;
        }
        else
        {
            g_tx_buf[(g_tx_len - 1)] = g_tx_len;
        }
        /* Send data */
        lmac15p4_tx_data_send(0, &g_tx_buf[0], g_tx_len, 0, 0);
#endif
        break;

    case MAC_TX_CONTINUOUS_WAVE_TEST:
        lmac15p4_tx_continuous_wave_send(1);
        break;
    }
}

void app_rx_process(void) {
    /* Check whether RX data is comming during certain interval */
    if (g_rx_total_count_last == g_rx_total_count) {
        printf("[E] No RX data in this period\r\n");
    }
    xTimerStart(rx_timer, 0);
    g_rx_total_count_last = g_rx_total_count;
}
/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void rfb_sample_init(uint8_t RfbPciTestCase)
{
    uint32_t FwVer;

#if (MAC_15P4)
    /* MAC PIB Parameters */
    uint32_t a_unit_backoff_period = A_UNIT_BACKOFF_PERIOD;
    uint32_t mac_ack_wait_duration = MAC_ACK_WAIT_DURATION;
    uint8_t mac_max_BE = MAC_MAX_BE;
    uint8_t mac_max_CSMA_backoffs = MAC_MAX_CSMACA_BACKOFFS;
    uint32_t mac_max_frame_total_wait_time = MAC_MAX_FRAME_TOTAL_WAIT_TIME;
    uint8_t mac_max_frame_retries = MAC_MAX_FRAME_RETRIES;
    uint8_t mac_min_BE = MAC_MIN_BE;

    /* PHY PIB Parameters */
    uint16_t a_turnaround_time = A_TURNAROUND_TIMR;
    uint8_t phy_cca_mode = ENERGY_DETECTION_OR_CARRIER_SENSING;
    uint8_t phy_cca_threshold = 80;
    uint16_t phy_cca_duration = A_TURNAROUND_TIMR;

    /* AUTO ACK Enable Flag */
    uint8_t auto_ack_enable = true;

    /* Frame Pending Bit */
    uint8_t frame_pending_bit = true;

    /* Address Filter Set */
    uint16_t short_addr = 0x1234;
    uint32_t long_addr_0 = 0x11223333;
    uint32_t long_addr_1 = 0x55667788;
    uint16_t pan_id = 0x1AAA;
    uint8_t is_coordinator = true;
    uint8_t mac_promiscuous_mode = false;
#endif

    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);
    lmac15p4_init(LMAC15P4_2P4G_OQPSK, HOSAL_RF_BAND_2P4G);

    /* Register rfb interrupt event */
    lmac15p4_callback_t mac_cb;
    mac_cb.rx_cb = rfb_rx_done;
    mac_cb.tx_cb = rfb_tx_done;
    lmac15p4_cb_set(0, &mac_cb);

#if (MAC_15P4)
    /* PHY PIB Parameters */
    lmac15p4_phy_pib_set(a_turnaround_time, phy_cca_mode,
                         phy_cca_threshold, phy_cca_duration);

    /* MAC PIB Parameters */
    lmac15p4_mac_pib_set(a_unit_backoff_period, mac_ack_wait_duration, mac_max_BE, 
                         mac_max_CSMA_backoffs, mac_max_frame_total_wait_time, 
                         mac_max_frame_retries, mac_min_BE);

    lmac15p4_address_filter_set(0, mac_promiscuous_mode, short_addr, long_addr_0,
                                long_addr_1, pan_id, is_coordinator);

    /* AUTO ACK Enable Flag */
    lmac15p4_auto_ack_set(auto_ack_enable);

    lmac15p4_ack_pending_bit_set(0, frame_pending_bit);
#else
    /* TX data setting example */
    g_tx_buf[0] = 0xA;
    g_tx_buf[1] = 0xB;
    g_tx_buf[2] = 0xC;
#endif
    /* Init test counters*/
    g_crc_success_count = 0;
    g_crc_fail_count = 0;
    g_rx_total_count = 0;
    g_tx_total_count = 0;
    g_tx_count_target = 100;

    lmac15p4_channel_set(0);

    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_SET, &sPhyPowerStage);

    g_tx_len = PHY_MIN_LENGTH;

    if (RfbPciTestCase == MAC_RX_TEST)
    {
        lmac15p4_auto_state_set(true);
        xTimerStart(rx_timer, 0);
    }
    else
    {
        xTimerStart(tx_timer, 0);
    }
}
