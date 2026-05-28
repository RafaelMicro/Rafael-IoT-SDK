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
#include "hosal_aux_comp.h"
#include "app_hooks.h"
#include "uart_stdio.h"

/*Aux Callback */
void aux_callback(uint32_t status) {
    uint32_t cnt = 0;

    hosal_get_aux_comp_counter_count(&cnt);
    printf("Aux_Callback:%ld\r\n", cnt);
    hosal_clear_aux_comp_counter_count();
    return;
}

void init_aux_comp(void) {
    hosal_aux_comp_config_t cfg = {0};

    cfg.debounce_en = 1;
    cfg.debounce_sel = HOSAL_AUX_SLOW_CLOKC_8;
    cfg.counter_mode_edge = HOSAL_AUX_BOTH_EDGE;
    cfg.counter_mode_en = 1;
    cfg.counter_mode_int_en = 1;
    cfg.counter_mode_threshold = 10;
    cfg.voltage_threshold = 1900;

    hosal_aux_comp_open(cfg, aux_callback);
    NVIC_EnableIRQ(Aux_Comp_IRQn);
}

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/************************************************/\r\n");
    printf("/******Start Aux Comparator Normal Counter*******/\r\n");
    printf("/************************************************/\r\n");

    init_aux_comp();
    aux_comp_normal_start();
    while (1) {
    }
}

