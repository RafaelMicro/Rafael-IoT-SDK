#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "gpio.h"
#include "rf_mcu_ahb.h"
#include "comm_subsystem_drv.h"
#include "uart_stdio.h"
#include "hosal_lpm.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"


#define PWR_LEVEL3_KEY   GPIO3
#define PWR_LEVEL2_KEY   GPIO2
#define PWR_LEVEL1_KEY   GPIO1
#define PWR_LEVEL0_KEY   GPIO0
#define WAKEUP_LED       GPIO20
#define WAKEUP_KEY       GPIO4
#define LPM_WAKEUP_KEY   HOSAL_LOW_POWER_WAKEUP_GPIO4


#define LPM_SRAM0_RETAIN 0x1E

uint32_t enter_sleep_mode = FALSE;
uint32_t get_power_level;

/**
 * \brief Wake up Interrupt Callback function
 * \param[in] pin  GPIO wakeup pin number
 * \param[in] isr_param NULL
 * \return None
 */
void wakeup_int_callback(uint32_t pin, void *isr_param)
{
    return;
}

/**
 * \brief Power Level_3 Interrupt Callback function
 * \param[in] pin GPIO wakeup pin number
 * \param[in] isr_param NULL
 * \return None
 */
#if defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583)
void pwr_level3_int_callback(uint32_t pin, void* isr_param) {

    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP3);
    enter_sleep_mode = TRUE;
    return;
}
#endif
/**
 * \brief Power Level_2 Interrupt Callback function
 * \param[in] pin GPIO wakeup pin number
 * \param[in] isr_param NULL
 * \return None
 */
void pwr_level2_int_callback(uint32_t pin, void* isr_param) {


#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || \
    defined(CONFIG_RT584_NONE_OS)
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_POWER_DOWN);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP2);
#endif
    enter_sleep_mode = TRUE;

    return;
}
/**
 * \brief Power Level_1 Interrupt Callback function
 * \param[in] pin GPIO wakeup pin number
 * \param[in] isr_param NULL
 * \return None
 */
void pwr_level1_int_callback(uint32_t pin, void* isr_param) {
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || \
    defined(CONFIG_RT584_NONE_OS)
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_DEEP_SLEEP);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP1);
#endif

    enter_sleep_mode = TRUE;

    return;
}
/**
 * \brief Power Level_0 Interrupt Callback function
 * \param[in] pin GPIO wakeup pin number
 * \param[in] isr_param NULL
 * \return None
 */
void pwr_level0_int_callback(uint32_t pin, void* isr_param) {
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || \
    defined(CONFIG_RT584_NONE_OS)
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP0);
#endif

    enter_sleep_mode = TRUE;
    return;
}

/**
 * \brief Initinal GPIO Pin for enter sleep
 */
void gpio_init(void) {

    hosal_gpio_input_config_t pin_cfg0, pin_cfg1, pin_cfg2, pin_cfg3, pin_cfg4;

    hosal_gpio_pin_clear(WAKEUP_LED);
    hosal_gpio_cfg_output(WAKEUP_LED);               /*Configure WAKEUP_LED output low*/
    hosal_pin_set_pullopt(WAKEUP_LED,PULL_NONE);    /*Configure WAKEUP_LED no-pull*/

#if defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583)

    pin_cfg3.param = NULL;
    pin_cfg3.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg3.usr_cb = pwr_level3_int_callback;

    hosal_pin_set_mode(PWR_LEVEL3_KEY,HOSAL_MODE_GPIO);                      /*Select PWR_LEVEL3_KEY as GPIO mode*/
    hosal_pin_set_pullopt(PWR_LEVEL3_KEY,HOSAL_PULL_UP_100K);                /*Configure PWR_LEVEL3_KEY 100K pull-up*/
    hosal_gpio_cfg_input(PWR_LEVEL3_KEY, pin_cfg3);
    hosal_gpio_int_enable(PWR_LEVEL3_KEY);                                  /*enable PWR_LEVEL3_KEY pin for interrupt source*/
    hosal_gpio_debounce_enable(PWR_LEVEL3_KEY);
#endif

    pin_cfg2.param = NULL;
    pin_cfg2.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg2.usr_cb = pwr_level2_int_callback;
    hosal_pin_set_mode(PWR_LEVEL2_KEY,HOSAL_MODE_GPIO);                      /*Select PWR_LEVEL2_KEY as GPIO mode*/
    hosal_pin_set_pullopt(PWR_LEVEL2_KEY,HOSAL_PULL_UP_100K);                /*Configure PWR_LEVEL2_KEY 100K pull-up*/
    hosal_gpio_cfg_input(PWR_LEVEL2_KEY, pin_cfg2);
    hosal_gpio_int_enable(PWR_LEVEL2_KEY);                                  /*enable PWR_LEVEL2_KEY pin for interrupt source*/
    hosal_gpio_debounce_enable(PWR_LEVEL2_KEY);

    pin_cfg1.param = NULL;
    pin_cfg1.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg1.usr_cb = pwr_level1_int_callback;
    hosal_pin_set_mode(PWR_LEVEL1_KEY,HOSAL_MODE_GPIO);                      /*Select PWR_LEVEL1_KEY as GPIO mode*/
    hosal_pin_set_pullopt(PWR_LEVEL1_KEY,HOSAL_PULL_UP_100K);                /*Configure PWR_LEVEL1_KEY 100K pull-up*/
    hosal_gpio_cfg_input(PWR_LEVEL1_KEY, pin_cfg1);
    hosal_gpio_int_enable(PWR_LEVEL1_KEY);                                  /*enable PWR_LEVEL1_KEY pin for interrupt source*/
    hosal_gpio_debounce_enable(PWR_LEVEL1_KEY);


    pin_cfg0.param = NULL;
    pin_cfg0.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg0.usr_cb = pwr_level0_int_callback;
    hosal_pin_set_mode(PWR_LEVEL0_KEY,HOSAL_MODE_GPIO);                      /*Select PWR_LEVEL0_KEY as GPIO mode*/
    hosal_pin_set_pullopt(PWR_LEVEL0_KEY,HOSAL_PULL_UP_100K);                /*Configure PWR_LEVEL0_KEY 100K pull-up*/
    hosal_gpio_cfg_input(PWR_LEVEL0_KEY, pin_cfg0);
    hosal_gpio_int_enable(PWR_LEVEL0_KEY);                              /*enable PWR_LEVEL0_KEY pin for interrupt source*/
    hosal_gpio_debounce_enable(PWR_LEVEL0_KEY);

    pin_cfg4.param = NULL;
    pin_cfg4.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    pin_cfg4.usr_cb = wakeup_int_callback;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(WAKEUP_KEY);
    hosal_pin_set_pullopt(WAKEUP_KEY, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(WAKEUP_KEY, pin_cfg4);
    hosal_gpio_int_enable(WAKEUP_KEY);
    NVIC_EnableIRQ(Gpio_IRQn);

   #if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || \
    defined(CONFIG_RT584_NONE_OS)
    hosal_gpio_setup_deep_sleep_io(WAKEUP_KEY, HOSAL_GPIO_LEVEL_LOW);
    hosal_gpio_setup_deep_powerdown_io(WAKEUP_KEY, HOSAL_GPIO_LEVEL_LOW);
    #endif  

}


int main(void) {

    uart_stdio_init();
    hosal_dma_init();

    printf("----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Sleep Example\r\n");
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || defined(CONFIG_RT584_NONE_OS)
    printf("press PWR_LEVEL2_KEY to enter power down mode\r\n");
    printf("press PWR_LEVEL1_KEY to enter deep sleep mode\r\n");
    printf("press PWR_LEVEL0_KEY to enter sleep mode \r\n");
    printf("press WAKEUP_KEY can wake up these power saving modes\r\n");
#else
    printf("press PWR_LEVEL3_KEY to enter sleep mode 3\r\n");
    printf("press PWR_LEVEL2_KEY to enter sleep mode 2\r\n");
    printf("press PWR_LEVEL1_KEY to enter sleep mode 1\r\n");
    printf("press PWR_LEVEL0_KEY to enter sleep mode 0\r\n");
    printf("press WAKEUP_KEY can wake up these power saving modes\r\n");
#endif


    gpio_init();
    hosal_lpm_init();

#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || \
    defined(CONFIG_RT584_NONE_OS)
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_SRAM_DEEP_SLEEP_INIT,NULL);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_DISABLE_LDO_MODE,HOSAL_LPM_PARAM_NONE); //if no load 569 FW, need to disable ldo mode.
    hosal_lpm_ioctrl(HOSAL_LPM_SRAM0_RETAIN, LPM_SRAM0_RETAIN);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP0);
#endif

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, LPM_WAKEUP_KEY);
    /*
     * In this example,
     * press PWR_LEVEL3_KEY to enter sleep mode 3,
     * press PWR_LEVEL2_KEY to enter sleep mode 2,
     * press PWR_LEVEL1_KEY to enter sleep mode 1,
     * press PWR_LEVEL0_KEY to enter sleep mode 0,
     * and press WAKEUP_KEY can wake up these power saving modes.
     */
    while (1) {

        hosal_gpio_pin_toggle(WAKEUP_LED);

        if (enter_sleep_mode == TRUE) {
            enter_sleep_mode = FALSE;
            
            #if defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583)

            hosal_get_lpm_ioctrl(HOSAL_LPM_GET_POWER_LEVEL, &get_power_level);

            if(get_power_level==HOSAL_LOW_POWER_LEVEL_SLEEP1) {
                   
                    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                    HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP);
                    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP0);
            }
            else if(get_power_level==HOSAL_LOW_POWER_LEVEL_SLEEP2) {
                    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                    HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
                    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP0);
            }   

            #endif
                      
            hosal_gpio_pin_set(WAKEUP_LED);
            hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
        }

        vTaskDelay(1000);
    }
}

/** @} */ /* end of examples group */
