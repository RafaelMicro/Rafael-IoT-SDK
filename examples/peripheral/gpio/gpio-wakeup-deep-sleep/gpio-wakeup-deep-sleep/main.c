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
#if defined(CONFIG_RF1301) || defined(CONFIG_RT584H) ||  defined(CONFIG_RT584HA4) || defined(CONFIG_RT584L)
#include "hosal_dpd.h"
#endif
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"


#if defined(CONFIG_RF1301) || defined(CONFIG_RT584H) ||  defined(CONFIG_RT584HA4) || defined(CONFIG_RT584L)
#else
/*wakeup io init */
void wakeupio_init(void)
{
    hosal_gpio_input_config_t pin_cfg0;

    /* gpio0 pin setting */
    pin_cfg0.param = NULL;
    pin_cfg0.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg0.usr_cb = NULL;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_pin_set_pullopt(GPIO0, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input_parameters(GPIO0, pin_cfg0, true);
    
    return;
}
#endif

int main(void) {
    uint32_t reset_cause = 0, reset_by_deep_sleep = 0;

    uart_stdio_init();
    vHeapRegionsInt();
    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);
           
    hosal_gpio_cfg_output(1);
    hosal_gpio_pin_clear(1);
    hosal_gpio_cfg_output(5);
    hosal_gpio_pin_set(5);
    hosal_gpio_cfg_output(4);
    
    printf("/********************************************/\r\n");
    printf("/*****START GPIO WAKE UP FROM DEEP SLEEP*****/\r\n");
    printf("/********************************************/\r\n");
#if defined(CONFIG_RF1301) || defined(CONFIG_RT584H) ||  defined(CONFIG_RT584HA4) || defined(CONFIG_RT584L)
    hosal_get_all_reset_cause(&reset_cause);
    printf("Deep sleep wake up,%.8lx\r\n", reset_cause);
    hosal_reset_by_deep_sleep(&reset_by_deep_sleep);
    if ( reset_by_deep_sleep ) {
        hosal_clear_reset_cause();
    }
    hosal_gpio_setup_deep_sleep_io(GPIO0, HOSAL_GPIO_LEVEL_LOW);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
#else
    wakeupio_init();
#endif
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO0);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_DEEP_SLEEP);

    printf("Enter deep sleep\r\n");
    hosal_delay_ms(100);
    while (1) {
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);

    }
}
