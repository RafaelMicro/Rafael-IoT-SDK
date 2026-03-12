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
    uint8_t repeat_cnt;
    uint32_t delay = 0, loop_cnt = 0;
    uint32_t full_flag, empty_flag;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    init_default_pin_mux();

    printf("/******************************/\r\n");
    printf("/*****Start NEC Protocol ******/\r\n");
    printf("/******************************/\r\n");

    init_ir();

    while (1) {
        tmp_cmd = 0x59 + loop_cnt;
        tmp_address = 0x16 + loop_cnt;

        irm_cmd = tmp_cmd | ((~tmp_cmd) << 8);
        irm_address = tmp_address | ((~tmp_address) << 8);

        hosal_ir_carrier_config(HOSAL_NEC_CARRIER_HIGH_CNT, HOSAL_NEC_CARRIER_LOW_CNT, HOSAL_NEC_CARRIER_BASEMENT_CNT);
        hosal_ir_nec_encoder( irm_cmd, irm_address);
        printf("NEC irm_cmd:%x, irm_address:%x \r\n", irm_cmd, irm_address);

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

        ir_done = 0;
        hosal_ir_start();
        while (!ir_done) {}
        for (delay = 0; delay < 100000; delay++);

        repeat_cnt = 5;

        while (repeat_cnt != 0) {
            printf("NEC repeat remain cnt:%d\r\n", repeat_cnt);
            hosal_ir_nec_repeat_encoder();
            hosal_ir_enable();
            hosal_ir_buffer_repeat_fill_in();
            ir_done = false;
            hosal_ir_start();
            while (!ir_done);

            repeat_cnt--;
            for (delay = 0; delay < 10000; delay++);
        }

        hosal_ir_stop();
        loop_cnt++;
        for (delay = 0; delay < 320000; delay++);

    }
}