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
#include "hosal_pwm.h"
#include "zigbee_api.h"

#ifndef CONFIG_APP_LIGHTING_PWM_ID
#define CONFIG_APP_LIGHTING_PWM_ID 0
#endif // !CONFIG_APP_LIGHTING_PWN_ID

#ifndef CONFIG_APP_LIGHTING_PWM_PIN
#define CONFIG_APP_LIGHTING_PWM_PIN 20
#endif // !CONFIG_APP_LIGHTING_PWM_PIN
//=============================================================================
//                Global variables
//=============================================================================
//=============================================================================
//                Function
//=============================================================================
void pwm_ctl_init(void) {
    hosal_pwm_dev_t pwm_dev;

    pwm_dev.config.id = CONFIG_APP_LIGHTING_PWM_ID;
    pwm_dev.config.frequency = 16000;//16K
    pwm_dev.config.pin_out = CONFIG_APP_LIGHTING_PWM_PIN;	
    pwm_dev.config.count_end_val = 3000;
	
    hosal_pwm_init_fmt0(&pwm_dev);
}

void pwm_ctl_set_level(uint8_t level) {
    uint8_t duty;    
    duty = level * 100 / 254;
    if(level > 0 && duty == 0) {
        duty = 1;
    }
    /* Inverted for rt58x evk */
    duty = 100 - duty;
    hosal_pwm_fmt0_duty(CONFIG_APP_LIGHTING_PWM_ID, duty);
}
