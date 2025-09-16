/**
 * @file subg_ctrl.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include "hosal_rf.h"
#include "subg_ctrl.h"
static uint32_t g_now_frequency = 0xFFFFFFFF;

void subg_ctrl_modem_config_set(subg_ctrl_modulation_t modulation,
                                subg_ctrl_data_rate_t data_rate,
                                uint8_t modulation_index) {
    hosal_rf_modem_config_set_t config;

    config.data_rate = data_rate;
    config.modulation_index = modulation_index;

    if (modulation == SUBG_CTRL_MODU_FSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_FSK_MODEM_CONFIG_SET,
                       (void*)&config);
    } else if (modulation == SUBG_CTRL_MODU_OPQSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_OQPSK_DATA_RATE_SET,
                       (void*)&data_rate);
    }
}

void subg_ctrl_mac_set(subg_ctrl_modulation_t modulation,
                       subg_ctrl_crc_type_t crc_type,
                       subg_ctrl_whiten_t whiten_enable) {
    hosal_rf_mac_set_t mac_set;

    mac_set.crc_type = crc_type;
    mac_set.whiten_enable = whiten_enable;

    if (modulation == SUBG_CTRL_MODU_FSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_FSK_PREAMBLE_SET, (void*)&mac_set);
    } else if (modulation == SUBG_CTRL_MODU_OPQSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_OQPSK_MAC_SET, (void*)&mac_set);
    }
}

void subg_ctrl_preamble_set(subg_ctrl_modulation_t modulation,
                            uint32_t preamble_len) {
    if (modulation == SUBG_CTRL_MODU_FSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_FSK_PREAMBLE_SET,
                       (void*)preamble_len);
    } else if (modulation == SUBG_CTRL_MODU_OPQSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_OQPSK_PREAMBLE_SET,
                       (void*)preamble_len);
    }
}

void subg_ctrl_sfd_set(subg_ctrl_modulation_t modulation, uint32_t sfd) {
    if (modulation == SUBG_CTRL_MODU_FSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_FSK_SFD_SET, (void*)sfd);
    }
}

void subg_ctrl_filter_set(subg_ctrl_modulation_t modulation, uint32_t filter) {
    if (modulation == SUBG_CTRL_MODU_FSK) {
        hosal_rf_ioctl(HOSAL_RF_IOCTL_SUBG_FSK_FILTER_SET, (void*)filter);
    }
}

void subg_ctrl_frequency_set(uint32_t frequency) {
    if (g_now_frequency != frequency) {
        g_now_frequency = frequency;
        hosal_rf_ioctl(HOSAL_RF_IOCTL_FREQUENCY_SET, (void*)frequency);
    }
}

void subg_ctrl_sleep_set(uint32_t enable) {
    hosal_rf_ioctl(HOSAL_RF_IOCTL_SLEEP_SET, (void*)enable);
}

void subg_ctrl_idle_set(void) {
    hosal_rf_ioctl(HOSAL_RF_IOCTL_IDLE_SET, (void*)NULL);
}

void subg_ctrl_rx_start_set(uint32_t rx_timeout) {
    hosal_rf_ioctl(HOSAL_RF_IOCTL_RX_ENABLE_SET, (void*)rx_timeout);
}
