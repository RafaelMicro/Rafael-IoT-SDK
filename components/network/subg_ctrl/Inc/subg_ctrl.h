/**
 * @file subg_ctrl.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __SUBG_CTRL_H__
#define __SUBG_CTRL_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FSK_MAX_RF_LEN 2063 //2047+16
#define OQPSK_MAX_RF_LEN 142 //127+15
#define MAX_RF_LEN FSK_MAX_RF_LEN

typedef enum {
    SUBG_CTRL_MODU_FSK = 0,
    SUBG_CTRL_MODU_OPQSK,
} subg_ctrl_modulation_t;

typedef enum {
    SUBG_CTRL_DATA_RATE_2M = 0,
    SUBG_CTRL_DATA_RATE_1M,
    SUBG_CTRL_DATA_RATE_500K,
    SUBG_CTRL_DATA_RATE_200K,
    SUBG_CTRL_DATA_RATE_100K,
    SUBG_CTRL_DATA_RATE_50K,
    SUBG_CTRL_DATA_RATE_300K,
    SUBG_CTRL_DATA_RATE_150K,
    SUBG_CTRL_DATA_RATE_75K,
} subg_ctrl_data_rate_t;

typedef enum {
    SUBG_CTRL_DATA_RATE_25K = 3,
    SUBG_CTRL_DATA_RATE_12P5K = 4,
    SUBG_CTRL_DATA_RATE_6P25K = 5,
} subg_ctrl_oqpsk_data_rate_t;

typedef enum {
    SUBG_CTRL_CRC_TYPE_16 = 0,
    SUBG_CTRL_CRC_TYPE_32,
} subg_ctrl_crc_type_t;

typedef enum {
    SUBG_CTRL_WHITEN_DISABLE = 0,
    SUBG_CTRL_WHITEN_ENABLE
} subg_ctrl_whiten_t;

typedef enum {
    SUBG_CTRL_FSK_MOD_0P5 = 0,
    SUBG_CTRL_FSK_MOD_1,
    SUBG_CTRL_FSK_MOD_UNDEF
} subg_ctrl_fsk_mod_t;

typedef enum {
    SUBG_CTRL_FILTER_TYPE_FSK = 0,
    SUBG_CTRL_FILTER_TYPE_GFSK,
    SUBG_CTRL_FILTER_TYPE_OQPSK
} subg_ctrl_filter_type_t;

void subg_ctrl_modem_config_set(subg_ctrl_modulation_t modulation,
                                subg_ctrl_data_rate_t data_rate,
                                uint8_t modulation_index);

void subg_ctrl_mac_set(subg_ctrl_modulation_t modulation,
                       subg_ctrl_crc_type_t crc_type,
                       subg_ctrl_whiten_t whiten_enable);

void subg_ctrl_preamble_set(subg_ctrl_modulation_t modulation,
                            uint32_t preamble_len);

void subg_ctrl_frequency_set(uint32_t frequency);

void subg_ctrl_sfd_set(subg_ctrl_modulation_t modulation, uint32_t sfd);

void subg_ctrl_filter_set(subg_ctrl_modulation_t modulation, uint32_t filter);

void subg_ctrl_sleep_set(uint32_t enable);

void subg_ctrl_idle_set(void);

void subg_ctrl_rx_start_set(uint32_t rx_timeout);

#endif
