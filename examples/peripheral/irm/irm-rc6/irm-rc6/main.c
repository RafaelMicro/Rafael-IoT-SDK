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
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_irm.h"
#include "hosal_status.h"
#include "task.h"
#include "app_hooks.h"
#include "uart_stdio.h"

volatile uint8_t ir_done = 0;

void init_default_pin_mux(void) {
    hosal_pin_set_mode(GPIO5, HOSAL_MODE_IRM);

    return;
}

void irm_cb(uint32_t status) {

    if (status & ENV_LAST_INT) {
        ir_done = 1;
    }
}

void init_ir(void) {
    hosal_irm_mode_t   irm_cfg;

    printf("To initialize IRM firmware\r\n");

    irm_cfg.op_mode = HOSAL_NORMAL_MODE;
    irm_cfg.ir_out_mode = HOSAL_AND;
    irm_cfg.irm_int_en = IR_ALL_INT_EN;
    irm_cfg.irm_cb_func = irm_cb;

    hosal_irm_open(&irm_cfg);
    NVIC_EnableIRQ(Irm_IRQn);
}

int main(void) {
    uint16_t irm_cmd, irm_address;
    uint8_t tmp_cmd, tmp_address;
    uint32_t delay = 0, loop_cnt = 0;
    static uint8_t rc6_toggle = 0;
    uint32_t full_flag, empty_flag;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    init_default_pin_mux();

    printf("/******************************/\r\n");
    printf("/*****Start RC6 Protocol ******/\r\n");
    printf("/******************************/\r\n");

    init_ir();

    while (1) {
        irm_cmd = 0x59 + loop_cnt;
        irm_address = 0x16 + loop_cnt;

        hosal_ir_carrier_config(HOSAL_RC6_CARRIER_HIGH_CNT, HOSAL_RC6_CARRIER_LOW_CNT, HOSAL_RC6_CARRIER_BASEMENT_CNT);
        hosal_ir_rc6_encoder( hosal_onebyte_bitreverse(irm_cmd), hosal_onebyte_bitreverse(irm_address), rc6_toggle);
        rc6_toggle ++;
        printf("irm_cmd:%x, irm_address:%x\r\n", irm_cmd, irm_address);

        hosal_ir_enable();
        hosal_ir_fifo_empty(&empty_flag);
        if ( !empty_flag ) {
            printf("FIFO must empty\r\n");
            while (1);
        }
        hosal_ir_buffer_fill_in();

        hosal_ir_fifo_full(&full_flag);
        if ( !full_flag ) {
            printf("FIFO must full\r\n");
            while (1);
        }

        ir_done = false;
        hosal_ir_start();
        while (!ir_done) {}
        for (delay = 0; delay < 3200000; delay++);

        hosal_ir_stop();
        loop_cnt++;
    }
}