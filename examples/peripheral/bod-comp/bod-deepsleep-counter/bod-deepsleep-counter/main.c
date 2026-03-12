/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_bod_comp.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"

void init_bod_comp(void) {
    hosal_bod_comp_config_t cfg = {0};

    cfg.debounce_en = 1;
    cfg.debounce_sel = HOSAL_BOD_SLOW_CLOKC_8;
    cfg.counter_mode_edge = HOSAL_BOD_BOTH_EDGE;
    cfg.counter_mode_en = 1;
    cfg.counter_mode_int_en = 1;
    cfg.counter_mode_threshold = 10;
    cfg.voltage_threshold = 1900;

    hosal_bod_comp_open(cfg, NULL);
}

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/***********************************************/\r\n");
    printf("/****Start Bod Comparator Deep Sleep Counter****/\r\n");
    printf("/***********************************************/\r\n");

    init_bod_comp();
    hosal_bod_comp_deep_sleep_start();
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_BOD_COMP);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_DEEP_SLEEP);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
    hosal_delay_ms(100);
    while (1) {
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }
}
