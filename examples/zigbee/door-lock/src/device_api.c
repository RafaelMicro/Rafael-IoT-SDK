/**
 * @file device_api.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "device_api.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "zigbee_api.h"

//=============================================================================
//                Global variables
//=============================================================================
//=============================================================================
//                Function
//=============================================================================
void set_led_onoff(uint8_t led, uint8_t on) {
    hosal_gpio_pin_write(led, !on);
}

static void button_cb(uint32_t pin, void* isr_param) 
{
    uint32_t pin_value;

    hosal_gpio_pin_get(pin, &pin_value);
    log_info("GPIO%d=%d\r\n", pin, pin_value);
    switch (pin)
    {
    case 0:
        ZIGBEE_APP_NOTIFY_ISR(ZB_APP_EVENT_TOGGLE);
        break;
    case 1:
    case 2:
    case 3:
        break;
    case 4:
        ZIGBEE_APP_NOTIFY_ISR(ZB_APP_EVENT_FACTORY_RESET);
        break;

    default:
        break;
    }

    return;
}

void button_init(void)
{
    uint8_t i = 0;
    hosal_gpio_input_config_t pin_cfg;
    /* gpio0 pin setting */
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg.usr_cb = button_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    NVIC_SetPriority(Gpio_IRQn, 7);
    for (i = 0; i < 5; i++) {
        hosal_pin_set_pullopt(i, HOSAL_PULL_UP_100K);
        hosal_gpio_cfg_input(i, pin_cfg);
        hosal_gpio_debounce_enable(i);
        hosal_gpio_int_enable(i);
        NVIC_EnableIRQ(Gpio_IRQn);
    }
}


