/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <FreeRTOS.h>
#include <task.h>
#include "app_led.h"
#include "app_miu_config.h"
#include "app_task.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "miu_port.h"
#include "subg_ctrl.h"
#include "util_string.h"

#define PHY_PIB_TURNAROUND_TIMER  1000
#define PHY_PIB_CCA_DETECTED_TIME 640 // 8 symbols for 50 kbps-data rate
#define PHY_PIB_CCA_DETECT_MODE   0
#define PHY_PIB_CCA_THRESHOLD     65
#define MAC_PIB_UNIT_BACKOFF_PERIOD                                            \
    1640 // PHY_PIB_TURNAROUND_TIMER + PHY_PIB_CCA_DETECTED_TIME
#define MAC_PIB_MAC_ACK_WAIT_DURATION         2000  // oqpsk neee more then 2000
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 82080 // for 50 kbps-data rate
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       2
#define MAC_PIB_MAC_MAX_BE                    5
#define MAC_PIB_MAC_MIN_BE                    4
static uint16_t cca_duration_table[] = {0, 0, 0, 380, 700, 1140, 260, 480, 600};
static uint16_t sensitivity_table[] = {100, 100, 100, 103, 106,
                                       109, 101, 104, 106};

enum {
    TX_POWER_20DBM,
    TX_POWER_14DBM,
    TX_POWER_0DBM,
};

static const char* const tx_power_str[] = {"20dBm", "14dBm", "0dBm"};

/* power index per chip, provided by Rafael. 0: keep chip default */
#if defined(CONFIG_RT581)
static const uint8_t tx_power_index[] = {30, 24, 0};
#elif defined(CONFIG_RF1301)
static const uint8_t tx_power_index[] = {0x7C, 0x3F, 0};
#else
static const uint8_t tx_power_index[] = {0, 0, 0};
#endif

#if defined(CONFIG_RF_POWER_14DBM)
static uint8_t sPhyTxPower = TX_POWER_14DBM;
#elif defined(CONFIG_RF_POWER_0DBM)
static uint8_t sPhyTxPower = TX_POWER_0DBM;
#else
static uint8_t sPhyTxPower = TX_POWER_20DBM;
#endif

static const char* const data_rate_str[] = {
    "2M", "1M", "500K", "200K", "100K", "50K", "300K", "150K", "75K"};
static const char* const band_str[] = {"SubG_915M", "2P4G",      "SubG_868M",
                                       "SubG_433M", "SubG_315M", "SubG_470M"};
uint16_t cca_duration = 0;
uint32_t frame_total_wait_time = 0;
uint32_t backof_period = 0;
/*set subg frequencydatarate*/
#if CONFIG_SUBG_FREQUENCY_BAND_915
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_915M;
#elif CONFIG_SUBG_FREQUENCY_BAND_868
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_868M;
#elif CONFIG_SUBG_FREQUENCY_BAND_470
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_470M;
#elif CONFIG_SUBG_FREQUENCY_BAND_433
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_433M;
#else
#error "SUBG frequency band not selected or not supported"
#endif
/*set subg datarate*/
#if CONFIG_SUBG_DATA_RATE_FSK_300K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_300K;
#elif CONFIG_SUBG_DATA_RATE_FSK_200K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_200K;
#elif CONFIG_SUBG_DATA_RATE_FSK_150K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_150K;
#elif CONFIG_SUBG_DATA_RATE_FSK_100K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_100K;
#elif CONFIG_SUBG_DATA_RATE_FSK_75K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_75K;
#elif CONFIG_SUBG_DATA_RATE_FSK_50K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_50K;
#elif CONFIG_SUBG_DATA_RATE_OQPSK_25K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_25K;
#else
#error "SUBG data rate not selected or not supported"
#endif

void otrInitUser(otInstance* instance) {
    app_led_pin_init();
    otAppCliInit((otInstance*)instance);
    otAppNcpInit((otInstance*)instance);
    ncpInitUser();
}

void app_task(void) {}

void app_common_init() {

    /*phy init*/
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    log_info("Mesh It Up Sniffer");
    log_info("Band               : %s", band_str[sPhyFrequencyBand]);
    log_info("Data Rate          : %s", data_rate_str[sPhyDataRate]);
    log_info("miu_port lib ver   : %s", miu_port_lib_version());

    /*subg phy parameter setting*/
    cca_duration = cca_duration_table[sPhyDataRate];
    frame_total_wait_time = 0;
    backof_period = PHY_PIB_TURNAROUND_TIMER + cca_duration;

#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
    lmac15p4_init(LMAC15P4_SUBG_OQPSK, sPhyFrequencyBand);
#else
    lmac15p4_init(LMAC15P4_SUBG_FSK, sPhyFrequencyBand);
#endif

    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, cca_duration);

    lmac15p4_mac_pib_set(backof_period, MAC_PIB_MAC_ACK_WAIT_DURATION,
                         MAC_PIB_MAC_MAX_BE, MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         frame_total_wait_time, MAC_PIB_MAC_MAX_FRAME_RETRIES,
                         MAC_PIB_MAC_MIN_BE);

    subg_ctrl_sleep_set(false);
    subg_ctrl_idle_set();
#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
    subg_ctrl_modem_config_set(LMAC15P4_SUBG_OQPSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(LMAC15P4_SUBG_OQPSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(LMAC15P4_SUBG_OQPSK, 8);
    subg_ctrl_sfd_set(LMAC15P4_SUBG_OQPSK, 0x00007209);
    subg_ctrl_filter_set(LMAC15P4_SUBG_OQPSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#else
    subg_ctrl_modem_config_set(SUBG_CTRL_MODU_FSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(SUBG_CTRL_MODU_FSK, 8);
    subg_ctrl_sfd_set(SUBG_CTRL_MODU_FSK, 0x00007209);
    subg_ctrl_filter_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#endif

    /*subg tx power setting*/
    if (tx_power_index[sPhyTxPower]) {
        hosal_rf_tx_power_t tx_power;
        tx_power.band_type = sPhyFrequencyBand;
#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
        tx_power.modem = HOSAL_RF_MODEM_SUBG_OQPSK;
#else
        tx_power.modem = HOSAL_RF_MODEM_FSK;
#endif
        tx_power.power_index = tx_power_index[sPhyTxPower];
        hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_SET, &tx_power);
        log_info("Tx Power           : %s", tx_power_str[sPhyTxPower]);
    } else {
        log_info("Tx Power           : chip default");
    }

    /*subg frequency range setting*/
    log_info("Channel Range     : %d ~ %d",
             OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN,
             OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX);
    log_info("Channel Frequency : %d ", OPENTHREAD_CONFIG_CHANNEL_FREQUENCY);
    log_info("Channel Spacing   : %d ", OPENTHREAD_CONFIG_CHANNEL_SPACING);
    otRadioChRange_t radiochrange;
    radiochrange.minChannel =
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN;
    radiochrange.maxChannel =
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX;
    radiochrange.frequencyBase = OPENTHREAD_CONFIG_CHANNEL_FREQUENCY;
    radiochrange.frequencySpacing = OPENTHREAD_CONFIG_CHANNEL_SPACING;
    otPlatRadioSetChannelRange(radiochrange);
    otPlatRadioSetReceiveSensitivity(sensitivity_table[sPhyDataRate]);
    log_info("Rx Sensitivity    : -%d dBm", sensitivity_table[sPhyDataRate]);

    /*mesh it up task start*/
    miuStart();
}