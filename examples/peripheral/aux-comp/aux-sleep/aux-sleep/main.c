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
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"


/*Aux Callback */
void aux_callback(uint32_t status) {
    printf("Aux_Callback\r\n");
    return;
}

void init_aux_comp(void) {
    hosal_aux_comp_config_t cfg = {0};

    cfg.debounce_en = 1;
    cfg.debounce_sel = HOSAL_AUX_SLOW_CLOKC_8;
    cfg.rising_edge_int_en = 1;
    cfg.falling_edge_int_en = 1;

    hosal_aux_comp_open(cfg, aux_callback);
    NVIC_EnableIRQ(Aux_Comp_IRQn);
}

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/**************************************/\r\n");
    printf("/*****Start Aux Comparator Sleep*******/\r\n");
    printf("/**************************************/\r\n");

    init_aux_comp();
    hosal_aux_comp_sleep_start();
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_AUX_COMP);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP);
    hosal_delay_ms(50);
    while (1) {
        printf("sleep\r\n");
        hosal_delay_ms(5);
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }
}
