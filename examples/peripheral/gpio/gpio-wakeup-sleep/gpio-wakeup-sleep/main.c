#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"


/*GPIO0 Handler */
void gpio0_handler(uint32_t pin, void *isr_param) {
    printf("GPIO%ld, wake up\r\n", pin);
    return;
}

/*wakeup io init */
void wakeupio_init(void) {
    hosal_gpio_input_config_t pin_cfg0;

    /* gpio0 pin setting */
    pin_cfg0.param = NULL;
    pin_cfg0.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg0.usr_cb = gpio0_handler;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(GPIO0);
    hosal_pin_set_pullopt(GPIO0, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(GPIO0, pin_cfg0);
    hosal_gpio_int_enable(GPIO0);
    NVIC_EnableIRQ(Gpio_IRQn);
    return;
}


int32_t main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);
    printf("/*******************************************/\r\n");
    printf("/*******START GPIO WAKE UP FROM SLEEP*******/\r\n");
    printf("/*******************************************/\r\n");

    wakeupio_init();

#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO0);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO0);
#endif
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);

    hosal_delay_ms(100);
    while (1) {
        printf("Enter sleep\r\n");
        hosal_delay_ms(10);
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }
}
