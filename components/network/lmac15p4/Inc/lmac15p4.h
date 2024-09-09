/**
 * @file lmac15p4.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __LMAC154_H__
#define __LMAC154_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    LMAC15P4_SUBG_FSK   = 1,
    LMAC15P4_2P4G_OQPSK = 2,
    LMAC15P4_SUBG_OQPSK = 4,
} lmac15p4_modem_t;

typedef enum
{
    LMAC154_CHANNEL_11 = 0,
    LMAC154_CHANNEL_12,
    LMAC154_CHANNEL_13,
    LMAC154_CHANNEL_14,
    LMAC154_CHANNEL_15,
    LMAC154_CHANNEL_16,
    LMAC154_CHANNEL_17,
    LMAC154_CHANNEL_18,
    LMAC154_CHANNEL_19,
    LMAC154_CHANNEL_20,
    LMAC154_CHANNEL_21,
    LMAC154_CHANNEL_22,
    LMAC154_CHANNEL_23,
    LMAC154_CHANNEL_24,
    LMAC154_CHANNEL_25,
    LMAC154_CHANNEL_26,
} lmac154_channel_t;
typedef void(pf_rx_done_cb)(uint16_t packet_length, uint8_t *pdata,
                            uint8_t crc_status, uint8_t rssi, uint8_t snr);
typedef void(pf_tx_done_cb)(uint32_t tx_status);
typedef struct
{
    pf_rx_done_cb *rx_cb;
    pf_tx_done_cb *tx_cb;
} lmac15p4_callback_t;


void lmac15p4_RxDoneEvent(uint16_t packet_length, uint8_t *pdata,
                          uint8_t crc_status, uint8_t rssi, uint8_t snr);
void lmac15p4_TxDoneEvent(uint32_t tx_status);
int lmac15p4_init(lmac15p4_modem_t modem, uint8_t band_type);
void lmac15p4_cb_set(uint32_t pan_idx, lmac15p4_callback_t *callback_set);
int lmac15p4_read_rssi(void);
char *lmac15p4_get_version(void);
int lmac15p4_tx_data_send(uint32_t pan_idx, uint8_t *tx_data_address, uint16_t packet_length, uint8_t mac_control, uint8_t mac_dsn);
void lmac15p4_address_filter_set(uint32_t pan_idx, uint8_t mac_promiscuous_mode,
                                 uint16_t short_source_address,
                                 uint32_t long_source_address_0, uint32_t long_source_address_1,
                                 uint16_t pan_id, uint8_t is_coordinator);
void lmac15p4_mac_pib_set(uint32_t a_unit_backoff_period,
                          uint32_t mac_ack_wait_duration, uint8_t mac_max_BE,
                          uint8_t mac_max_CSMA_backoffs, uint32_t mac_max_frame_total_wait_time,
                          uint8_t mac_max_frame_retries, uint8_t mac_min_BE);
void lmac15p4_phy_pib_set(uint16_t a_turnaround_time, uint8_t phy_cca_mode,
                          uint8_t phy_cca_threshold, uint16_t phy_cca_duration);
void lmac15p4_auto_ack_set(uint32_t auto_ack_enable);
void lmac15p4_auto_state_set(uint32_t auto_state_enable);
void lmac15p4_channel_set(lmac154_channel_t ch);
void lmac15p4_ack_pending_bit_set(uint32_t pan_idx, uint32_t pending_bit_enable);
void lmac15p4_src_match_ctrl(uint32_t pan_idx, uint32_t enable);
void lmac15p4_src_match_short_entry(uint8_t control_type, uint8_t *short_addr);
void lmac15p4_src_match_extended_entry(uint8_t control_type, uint8_t *extended_addr);
void lmac15p4_csl_receiver_ctrl(uint8_t csl_receiver_ctrl, uint16_t csl_period);
void lmac15p4_csl_accuracy_get(uint8_t *csl_accuracy);
void lmac15p4_csl_uncertainty_get(uint8_t *csl_uncertainty);
void lmac15p4_csl_sample_time_update(uint32_t csl_sample_time);
uint32_t lmac15p4_read_ack(uint8_t *ack_data, uint8_t *ack_time);
void lmac15p4_key_set(uint32_t pan_idx, uint8_t *key);
void lmac15p4_rtc_time_read(uint32_t *rtc_time);
void lmac15p4_rx_rtc_time_read(uint8_t rx_cnt, uint32_t *rx_rtc_time);
uint32_t  lmac15p4_frame_counter_get(void);
void lmac15p4_2ch_scan_set(uint32_t eanble, lmac154_channel_t ch1, lmac154_channel_t ch2);
#endif