#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"
#include "hosal_sysctrl.h"
#include "hosal_gpio.h"

#define GPIO0     0
#define GPIO1     1
#define GPIO2     2


void gpio_cb(uint32_t pin, void* isr_param) {
    printf("GPIO%d=%d\r\n", pin, hosal_gpio_pin_get(pin));
    return;
}

void pin_init(void) {
    hosal_gpio_input_config_t pin_cfg0, pin_cfg1, pin_cfg2;

    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++) {
        if ((i != 16) && (i != 17) && (i != 10) && (i != 11)) {
            hosal_pin_set_mode(i, MODE_GPIO);
        }
    }

    /* gpio0 pin setting */
    pin_cfg0.param = NULL;
    pin_cfg0.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg0.usr_cb = gpio_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(GPIO0);
    hosal_pin_set_pullopt(GPIO0, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(GPIO0, pin_cfg0);

    /* gpio1 pin setting */
    pin_cfg1.param = NULL;
    pin_cfg1.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg1.usr_cb = gpio_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(GPIO1);
    hosal_pin_set_pullopt(GPIO1, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(GPIO1, pin_cfg1);

    /* gpio2 pin setting */
    pin_cfg2.param = NULL;
    pin_cfg2.pin_int_mode = HOSAL_GPIO_PIN_INT_LEVEL_LOW;
    pin_cfg2.usr_cb = gpio_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(GPIO2);
    hosal_pin_set_pullopt(GPIO2, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(GPIO2, pin_cfg2);

    hosal_gpio_int_enable(GPIO0);
    hosal_gpio_int_enable(GPIO1);
    hosal_gpio_int_enable(GPIO2);

    NVIC_EnableIRQ(Gpio_IRQn);

    return;
}




int32_t main(void) {

    pin_init();

    puts("/*****Start Gpio Interrupt******/\r\n");
    puts("GPIO0 rising edge trigger interrupt\r\n");
    puts("GPIO1 falling edge trigger interrupt\r\n");
    puts("GPIO2 low level trigger interrupt\r\n");

    while (1) {}
}
