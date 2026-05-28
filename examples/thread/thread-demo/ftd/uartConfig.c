/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include "uartConfig.h"
#include <string.h>
#include "EnhancedFlashDataset.h"

static scan_config_t thread_config = {
    .scan_number = 5,
    .scan_table = {20, 11, 3, 16, 5, 12, 7, 11, 19, 10, 
        8, 6, 13, 2, 15, 4, 17, 18, 9, 1},
};

void thread_config_set(scan_config_t *config) {
    memcpy(&thread_config, config, sizeof(thread_config));
    thread_config.dirty = true;
    efd_set_env_blob("threadConfig", &thread_config, sizeof(thread_config));
}

void thread_config_get(scan_config_t *config) {
    size_t configLength;

    efd_get_env_blob("threadConfig", &thread_config, sizeof(thread_config), &configLength);
    memcpy(config, &thread_config, sizeof(thread_config));
}

void thread_config_reset(void) {
    memset(&thread_config, 0, sizeof(thread_config));
    efd_set_env_blob("threadConfig", &thread_config, sizeof(thread_config));
}