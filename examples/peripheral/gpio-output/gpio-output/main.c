#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "log.h"
#include "task.h"

#define GPIO_OUT  5
#define GPIO_IN   4
#define RT_TIMER0 0

volatile uint8_t timeout = 0;
volatile uint8_t timeout_cnt = 0;

void pin_init(void) {
    hosal_gpio_input_config_t pin_cfg;

    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++) {
        if ((i != 16) && (i != 17) && (i != 10) && (i != 11)) {
            hosal_pin_set_mode(i, MODE_GPIO);
        }
    }

    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_NOINT;
    pin_cfg.usr_cb = NULL;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(GPIO_IN);
    hosal_pin_set_pullopt(GPIO_IN, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(GPIO_IN, pin_cfg);

    hosal_gpio_cfg_output(GPIO_OUT);
    hosal_gpio_pin_clear(GPIO_OUT);
    return;
}

/* TIMER0 interrupt */
void timer0_callback(uint32_t timer_id) {
    timeout = 1;
    timeout_cnt = timeout_cnt + 1;
    if ((timeout_cnt % 2) == 0) {
        hosal_gpio_pin_toggle(GPIO_OUT);
    }
    return;
}

void timer_periodic_init(void) {
    hosal_timer_config_t cfg0;
    hosal_timer_tick_config_t tick_cfg0;

    /* TIMER0 */
    cfg0.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg0.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg0.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg0.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg0.user_prescale = 0;

    tick_cfg0.timeload_ticks = 1000000;
    tick_cfg0.timeout_ticks = 0;

    hosal_timer_init(RT_TIMER0, cfg0, timer0_callback);
    NVIC_EnableIRQ(Timer0_IRQn);
    hosal_timer_start(RT_TIMER0, tick_cfg0);
}

int32_t main(void) {

    pin_init();

    puts("/*****Start Gpio Output******/\r\n");
    puts("GPIO 5 is output and will high low toggle\r\n");
    puts("GPIO 4 is input to know gpio5 output value\r\n");

    timer_periodic_init();
    while (1) {
        timeout = 0;
        printf("GPIO_4=%d\r\n", hosal_gpio_pin_get(GPIO_IN));
        while (timeout == 0) {}
    }
}
