/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#ifndef UARTCONFIG_H_
#define UARTCONFIG_H_

#include <stdio.h>
#include <stdint.h> 
#include <stdbool.h>

typedef struct scan_config {
    bool dirty;
    uint8_t power_stage;
    uint8_t scan_number;
    uint8_t scan_table[20];
    uint16_t network_data_poll_period;
    uint16_t thread_channel_scan_timeout;
    uint16_t thread_channel_scan_retry;
    uint16_t thread_long_deeplseep_wakeup_time;
    uint16_t thread_short_deeplseep_wakeup_time;
} scan_config_t;

void thread_config_set(scan_config_t *config);
void thread_config_get(scan_config_t *config);
void thread_config_reset(void);
#endif
