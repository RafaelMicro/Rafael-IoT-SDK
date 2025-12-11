/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_dpd.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "app_hooks.h"
#include "uart_stdio.h"


int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    
    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/*********************************************/\r\n");
    printf("/***START GPIO WAKE UP FROM DEEP POWER DOWN***/\r\n");
    printf("/*********************************************/\r\n");

    printf("Deep power down wake up,%.8lx\r\n", hosal_get_all_reset_cause());
    if ( hosal_reset_by_deep_power_down() ) {
        hosal_clear_reset_cause();
    }
    hosal_gpio_setup_deep_powerdown_io(GPIO0, HOSAL_GPIO_LEVEL_LOW);
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO0);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_POWER_DOWN);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
    printf("Enter deep power down\r\n");
    hosal_delay_ms(100);
    while (1) {
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }
}
