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
#include "cli.h"
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

/*subg use*/
#include "subg_ctrl.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#define SUBG_MAC (true)

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define RUCI_HEADER_LENGTH      (1)
#define RUCI_SUB_HEADER_LENGTH  (1)
#define RUCI_LENGTH             (2)
#define RUCI_PHY_STATUS_LENGTH  (3)
#define RX_CONTROL_FIELD_LENGTH (RUCI_HEADER_LENGTH+RUCI_SUB_HEADER_LENGTH+RUCI_LENGTH+RUCI_PHY_STATUS_LENGTH)
#define RX_STATUS_LENGTH        (5)
#define FSK_PHR_LENGTH          (2)
#define OQPSK_PHR_LENGTH        (1)
#define CRC16_LENGTH            (2)
#define CRC32_LENGTH            (4)
#define FSK_RX_HEADER_LENGTH    (RX_CONTROL_FIELD_LENGTH + FSK_PHR_LENGTH)
#define OQPSK_RX_HEADER_LENGTH  (RX_CONTROL_FIELD_LENGTH + OQPSK_PHR_LENGTH)
#define RX_APPEND_LENGTH        (RX_STATUS_LENGTH + CRC16_LENGTH)
#define FSK_RX_LENGTH           (FSK_MAX_RF_LEN - FSK_RX_HEADER_LENGTH - RX_APPEND_LENGTH)  //2047
#define OQPSK_RX_LENGTH         (OQPSK_MAX_RF_LEN - OQPSK_RX_HEADER_LENGTH - RX_APPEND_LENGTH)  //127
#define PHY_MIN_LENGTH          (3)
#define PRBS9_LENGTH            (255)
#if (SUBG_MAC)
#define A_TURNAROUND_TIMR             1000;
#define A_UNIT_BACKOFF_PERIOD         320;
#define MAC_ACK_WAIT_DURATION         16000 // For OQPSK mode; FSK: 2000 non-beacon mode; 864 for beacon mode
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

/* Use PRBS9 as data content*/
const uint8_t        g_Prbs9Content[] = PRBS9_CONTENT;

/* Rx buffer to store data from RFB*/
uint8_t              g_prbs9_buf[FSK_RX_LENGTH];

/* TX length for TX transmit test*/
uint16_t             g_tx_len;

static subg_ctrl_modulation_t modem_type = SUBG_CTRL_MODU_FSK;
static hosal_rf_tx_power_t sPhyPowerStage = {
    .band_type = HOSAL_RF_BAND_SUBG_915M,
    .power_index = 30,
};

/* frequency lists*/
uint32_t             g_freq_support[10] = {920000, 920500, 921000, 921500, 922000, 922500, 923000, 923500, 924000, 924500};

static rtc_time_t current_time, alarm_tm;
static uint32_t alarm_mode;
/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static void subg_tx_done(uint32_t tx_status)
{
    g_tx_total_count++;

#if (SUBG_MAC)
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
#else
        printf("TX (len:%d) done total:%d Fail:%d\n", g_tx_len, g_tx_total_count, g_tx_fail_Count);
#endif
    xTimerStart(tx_timer, 0);
}

static void subg_rx_done(uint16_t ruci_packet_length, uint8_t *rx_data_address, uint8_t crc_status, uint8_t rssi, uint8_t snr)
{
#if (!SUBG_MAC)
    uint16_t i;
    uint8_t header_length = (modem_type == SUBG_CTRL_MODU_FSK) ? FSK_RX_HEADER_LENGTH : OQPSK_RX_HEADER_LENGTH;
#endif
    uint16_t rx_data_len;
    uint8_t phr_length = (modem_type == SUBG_CTRL_MODU_FSK) ? FSK_PHR_LENGTH : OQPSK_PHR_LENGTH;

    g_rx_total_count++;
    rx_data_len = ruci_packet_length - (RUCI_PHY_STATUS_LENGTH + phr_length + RX_APPEND_LENGTH);

    if (crc_status == 0)
    {
#if (!SUBG_MAC)
        /* Verify data content*/
        for (i = 0; i < rx_data_len; i++)
        {
            if (g_prbs9_buf[i] != *(rx_data_address + header_length + i))
            {
                printf("[E] data content error %d/%d\n", i, rx_data_len);
            }
        }
        printf("\n");
#endif
        g_crc_success_count ++;
    }
    else
    {
        g_crc_fail_count ++;
    }
    printf("RX (len:%d) done, Success:%d Fail:%d\n", rx_data_len, g_crc_success_count, g_crc_fail_count);
}

void rfb_rx_timeout(void)
{
    g_rx_timeout_count ++;
    printf("RX timeout:%d\n", g_rx_timeout_count);
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
    uint16_t mac_data_len = 0;
    uint16_t max_length = (modem_type == SUBG_CTRL_MODU_FSK) ? 2045 : 125;

    mac_data_len = (uint16_t)((g_tx_total_count) & 0x7FF);

    if (mac_data_len > max_length)
    {
        mac_data_len = max_length;
    }

    *Dsn = (uint8_t)((g_tx_total_count) & 0x7F);
    Rfb_MacFrameGen(modem_type, MacBuf, tx_control, *Dsn, mac_data_len);

    g_tx_len = MacBuf->len;
}

static void rtc_callback(uint32_t rtc_status) {}

static void set_wakeup_sleep(size_t wakeup_time) {
    printf("sleep for %d sec\n", wakeup_time);
    hosal_rtc_set_clk(40000);

    current_time.tm_year = 0;
    current_time.tm_mon = 0;
    current_time.tm_day = 0;
    current_time.tm_hour = 0;
    current_time.tm_min = 0;
    current_time.tm_sec = 0;
    hosal_rtc_set_time(&current_time);

    alarm_tm.tm_year = 0;
    alarm_tm.tm_mon = 0;
    alarm_tm.tm_day = 0;
    alarm_tm.tm_hour = 0;
    alarm_tm.tm_min = 0;
    alarm_tm.tm_sec = wakeup_time;
    alarm_mode = RTC_MODE_HOUR_EVENT_INTERRUPT | RTC_MODE_EVENT_INTERRUPT;
    hosal_rtc_set_alarm(&alarm_tm, alarm_mode, rtc_callback);

    lpm_low_power_unmask(LOW_POWER_MASK_BIT_RESERVED13);
    lpm_set_low_power_level(LOW_POWER_LEVEL_SLEEP0);
    lpm_enable_low_power_wakeup(LOW_POWER_WAKEUP_SLOW_TIMER);
    lpm_enter_low_power_mode();
}

void app_tx_process(uint32_t rfb_pci_test_case) {
#if (SUBG_MAC)
    uint8_t tx_control = 0;
    uint8_t Dsn = 0;
    static MacBuffer_t MacBuf;
#else
    uint16_t max_length = (modem_type == SUBG_CTRL_MODU_FSK) ? 2047 : 127;
#endif
    /* Abort test if TX count is reached in burst tx test */
    if (burst_tx_abort())
    {
        return;
    }

    switch (rfb_pci_test_case)
    {
    case SUBG_BURST_TX_TEST:
#if (SUBG_MAC)
        /* Generate IEEE802.15.4 MAC Header and append data */
        mac_data_gen(&MacBuf, &tx_control, &Dsn);
        lmac15p4_tx_data_send(0, MacBuf.dptr, MacBuf.len, tx_control, Dsn);
        g_tx_len = MacBuf.len;
#else
        /* Determine TX packet length*/
        g_tx_len ++;
        if (g_tx_len > (max_length - CRC16_LENGTH))
        {
            g_tx_len = PHY_MIN_LENGTH;
        }
        /* Send data */
        lmac15p4_tx_data_send(0, &g_prbs9_buf[0], g_tx_len, 0, 0);
#endif
        /* Add delay to increase TX interval*/
        delay_us(30000);
        break;

    case SUBG_SLEEP_TX_TEST:
#if (SUBG_MAC)
        lmac15p4_auto_state_set(true);

        /* Generate IEEE802.15.4 MAC Header and append data */
        mac_data_gen(&MacBuf, &tx_control, &Dsn);
        lmac15p4_tx_data_send(0, MacBuf.dptr, MacBuf.len, tx_control, Dsn);
        g_tx_len = MacBuf.len;
#else
        subg_ctrl_idle_set();

        /* Determine TX packet length*/
        g_tx_len ++;
        if (g_tx_len > (max_length - CRC16_LENGTH))
        {
            g_tx_len = PHY_MIN_LENGTH;
        }

        /* Send data */
        lmac15p4_tx_data_send(0, &g_prbs9_buf[0], g_tx_len, 0, 0);
#endif
        /* Wait TX finish */
        //while (tx_done_check() == false);

        /*Set RF State to SLEEP*/
#if (SUBG_MAC)
        lmac15p4_auto_state_set(false);
#else
        subg_ctrl_sleep_set(true);
#endif
        set_wakeup_sleep(10);
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

/* TRX initialization */
void rfb_trx_init(uint32_t rx_timeout_timer, bool rx_continuous)
{
    /*Set RF State to Idle*/
    subg_ctrl_idle_set();

    /*Set TX config*/
    subg_ctrl_modem_config_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_DATA_RATE_100K, SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_CRC_TYPE_16, SUBG_CTRL_WHITEN_ENABLE);
    subg_ctrl_preamble_set(SUBG_CTRL_MODU_FSK, 8);
    subg_ctrl_sfd_set(SUBG_CTRL_MODU_FSK, 0x00007209);
    subg_ctrl_filter_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_FILTER_TYPE_GFSK);

    /*
    * Set channel frequency :
    * For band is subg, units is kHz
    * For band is 2.4g, units is mHz
    */

    subg_ctrl_frequency_set(g_freq_support[0]);

    sPhyPowerStage.modem = (modem_type == SUBG_CTRL_MODU_FSK) ? HOSAL_RF_MODEM_FSK : HOSAL_RF_MODEM_SUBG_OQPSK;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_SET, &sPhyPowerStage);

    g_tx_len = PHY_MIN_LENGTH;
}

void data_gen(uint8_t *pbuf, uint16_t len)
{
    uint8_t  idx ;
    for (idx = 0; idx < (len >> 8); idx++)
    {
        memcpy(pbuf + (idx << 8), &(g_Prbs9Content[0]), 0x100);
    }
    if (len & 0xFF)
    {
        memcpy(pbuf + (idx << 8), &(g_Prbs9Content[0]), (len & 0xFF));
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void rfb_sample_init(uint8_t RfbPciTestCase)
{
    uint32_t FwVer;
#if (SUBG_MAC)
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
    uint8_t phy_cca_mode = ENERGY_DETECTION_OR_CHARRIER_SENSING;
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

    lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_915M);

    /* Register rfb interrupt event */
    lmac15p4_callback_t mac_cb;
    mac_cb.rx_cb = subg_rx_done;
    mac_cb.tx_cb = subg_tx_done;
    lmac15p4_cb_set(0, &mac_cb);

#if (SUBG_MAC)
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
#endif

    /* Init test counters*/
    g_crc_success_count = 0;
    g_crc_fail_count = 0;
    g_rx_total_count = 0;
    g_tx_total_count = 0;
    g_tx_count_target = 100;

    data_gen(&g_prbs9_buf[0], FSK_RX_LENGTH);

    /* Set test parameters*/
    rfb_trx_init(0, true);
    if (RfbPciTestCase == SUBG_RX_TEST)
    {
#if (SUBG_MAC)
        lmac15p4_auto_state_set(true);
#else
        subg_ctrl_rx_start_set(0xffffffff);
#endif
        xTimerStart(rx_timer, 0);
    }
    else
    {
        xTimerStart(tx_timer, 0);
    }
}
