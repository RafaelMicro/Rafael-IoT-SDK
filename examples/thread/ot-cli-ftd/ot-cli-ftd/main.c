/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config/platform.h>
#include "EnhancedFlashDataset.h"
#include "FreeRTOS.h"
#include "cli.h"
#include "hosal_rf.h"
#include "hosal_sysctrl.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "mcu.h"
#include "openthread_port.h"
#include "task.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "dump_boot_info.h"

#define PHY_PIB_TURNAROUND_TIMER                192
#define PHY_PIB_CCA_DETECTED_TIME               128 // 8 symbols
#define PHY_PIB_CCA_DETECT_MODE                 0
#define PHY_PIB_CCA_THRESHOLD                   85
#define MAC_PIB_UNIT_BACKOFF_PERIOD             320
#define MAC_PIB_MAC_ACK_WAIT_DURATION           544 // non-beacon mode; 864 for beacon mode
#define MAC_PIB_MAC_MAX_BE                      8
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME   16416
#define MAC_PIB_MAC_MAX_FRAME_RETRIES           4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS         10
#define MAC_PIB_MAC_MIN_BE                      5

/**
 * @brief Initializes the pin multiplexing.
 *
 * This function sets all GPIO pins to GPIO mode, except for GPIO16 and GPIO17,
 * which are reserved for specific functions.
 *
 * @return void This function does not return a value.
 */
static void pin_mux_init(void) 
{
    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (int i = 0; i < 32; i++) {
        if (i == 16 || i == 17) {
            continue; // Skip GPIO16 and GPIO17
        }
        hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
    }
}

/**
 * @brief Main entry point for the application.
 *
 * This function initializes the RF module, starts the application initialization,
 * and enters an infinite loop to keep the application running.
 *
 * @param pvParameters Pointer to parameters passed to the task (not used).
 * @return void This function does not return a value.
 */
static void app_main_entry(void* pvParameters)
{
    enhanced_flash_dataset_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    log_info("Thread 2.4G\r\n");
    lmac15p4_init(LMAC15P4_2P4G_OQPSK, 0);
    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, PHY_PIB_CCA_DETECTED_TIME);

    lmac15p4_mac_pib_set(MAC_PIB_UNIT_BACKOFF_PERIOD,
                         MAC_PIB_MAC_ACK_WAIT_DURATION, MAC_PIB_MAC_MAX_BE,
                         MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME,
                         MAC_PIB_MAC_MAX_FRAME_RETRIES, MAC_PIB_MAC_MIN_BE);

    cli_init();
    // for (uint32_t pin = 0 ; pin < 8 ; pin++)
    // {
    //     pin_set_mode(pin, MODE_DBG0 + pin);
    // }

    // pin_set_mode(28, MODE_DBG2);
    // pin_set_mode(29, MODE_DBG3);
    // SYSCTRL->sys_test.bit.dbg_out_sel = 25;
    otrStart();
    while (1);
}

/**
 * @brief Application entry point.
 *
 * Initializes hardware, configures peripherals, and starts the main application task.
 *
 * @return int Not used.
 */
int main(void) {
    pin_mux_init();
    uart_stdio_init();
    vHeapRegionsInt();
    _dump_boot_info();

    if (xTaskCreate(app_main_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }
    puts("[OS] Starting OS Scheduler...\r\n");
    puts("\r\n");
    vTaskStartScheduler();
    while (1) {};

    return 0;
}

extern void otAppCliTnit(otInstance* instance);

void otrInitUser(otInstance* instance) {
    otAppCliInit((otInstance*)instance);
}


