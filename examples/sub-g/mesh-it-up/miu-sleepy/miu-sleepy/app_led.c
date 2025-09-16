#include "FreeRTOS.h"
#include "hosal_gpio.h"
#include "main.h"
#include "timers.h"

#define MIU_LED0_PIN 20
#define MIU_LED1_PIN 21
/*EVK hardware not support GPIO-22 (need wire connection)*/
#define MIU_LED2_PIN 22

static TimerHandle_t led_timerHandle = NULL;

void app_set_led0_on(void) { hosal_gpio_pin_write(MIU_LED0_PIN, 0); }

void app_set_led0_off(void) { hosal_gpio_pin_write(MIU_LED0_PIN, 1); }

void app_set_led0_toggle(void) { hosal_gpio_pin_toggle(MIU_LED0_PIN); }

static void app_led_timer_handler(TimerHandle_t xTimer) {
    app_set_led0_off();
    //clear timer point
    xTimerDelete(led_timerHandle, 0);
    led_timerHandle = NULL;
}

void app_set_led0_flash(void) {
    if (NULL == led_timerHandle) {
        app_set_led0_on();
        led_timerHandle = xTimerCreate("led_timer", 100, pdFALSE, NULL,
                                       app_led_timer_handler);
        xTimerStart(led_timerHandle, 0);
    }
}

void app_set_led1_on(void) { hosal_gpio_pin_write(MIU_LED1_PIN, 0); }

void app_set_led1_off(void) { hosal_gpio_pin_write(MIU_LED1_PIN, 1); }

void app_set_led1_toggle(void) { hosal_gpio_pin_toggle(MIU_LED1_PIN); }

void app_led_pin_init(void) {
    hosal_gpio_cfg_output(MIU_LED0_PIN);
    hosal_gpio_cfg_output(MIU_LED1_PIN);
    /*EVK hardware not support GPIO-22 (need wire connection)*/
    // hosal_gpio_cfg_output(MIU_LED2_PIN);

    hosal_gpio_pin_write(MIU_LED0_PIN, 1);
    hosal_gpio_pin_write(MIU_LED1_PIN, 1);
    /*EVK hardware not support GPIO-22 (need wire connection)*/
    // hosal_gpio_pin_write(MIU_LED2_PIN, 0);
}