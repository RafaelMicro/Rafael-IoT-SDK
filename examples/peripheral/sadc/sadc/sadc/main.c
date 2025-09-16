#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_gpio.h"
#include "hosal_sadc.h"
#include "hosal_status.h"
#include "hosal_sysctrl.h"
#include "task.h"
#include "app_hooks.h"
#include "uart_stdio.h"


volatile uint32_t sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
volatile uint32_t sadc_convert_channel;
volatile hosal_sadc_value_t sadc_convert_value;




void sadc_int_callback_handler(hosal_sadc_cb_t* p_cb) {
    if (p_cb->type == HOSAL_SADC_CB_SAMPLE) {
        sadc_convert_channel = p_cb->data.sample.channel;
        sadc_convert_value = p_cb->data.sample.value;
        sadc_convert_status = HOSAL_SADC_CONVERT_DONE;
        /*
        printf("ADC CH%d: adc = %x, comp = %x, cal = %x, value = %x\r\n",
               p_cb->data.sample.channel, p_cb->raw.conversion_value,
               p_cb->raw.compensation_value, p_cb->raw.calibration_value,
               p_cb->data.sample.value);
        */
    }
}

void init_default_pin_mux(void) {
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    /*
     * AIO 0 ->  GPIO 21
     * AIO 1 ->  GPIO 22
     * AIO 2 ->  GPIO 23
     * AIO 4 ->  GPIO 28
     * AIO 5 ->  GPIO 29
     * AIO 6 ->  GPIO 30
     * AIO 7 ->  GPIO 31
     */
    hosal_sadc_aio_enable(7);
    hosal_pin_set_pullopt(GPIO31, HOSAL_PULL_NONE);
#else
#endif
    return;
}

int32_t main(void) {

    hosal_sadc_config_t sadc_set;
    hosal_sadc_channel_config_t read_ch;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    init_default_pin_mux();
    
    printf("/*******Start SADC********/\r\n");

    sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
    read_ch.channel = HOSAL_SADC_CH_AIN7;
    sadc_set.oversample = HOSAL_SADC_OVERSAMPLE_256;
    sadc_set.resolution = HOSAL_SADC_RES_12BIT;

    hosal_sadc_config_enable(sadc_set, sadc_int_callback_handler);
    NVIC_EnableIRQ((IRQn_Type)(Sadc_IRQn));

#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
#else
    hosal_sadc_compensation_init(1);
#endif
    while (1) {
        if (sadc_convert_status == HOSAL_SADC_CONVERT_IDLE) {
            sadc_convert_status = HOSAL_SADC_CONVERT_START;

            if (hosal_sadc_channel_read(read_ch) != HOSAL_STATUS_SUCCESS) {
                sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
            }
        }

        if (sadc_convert_status == HOSAL_SADC_CONVERT_DONE) {

            switch (read_ch.channel) {
                case HOSAL_SADC_CH_AIN7:
                    printf("AIO%d ADC = %dmv\r\n", read_ch.channel, sadc_convert_value);
                    read_ch.channel = HOSAL_SADC_CH_VBAT;
                    break;

                case HOSAL_SADC_CH_VBAT:
                    printf("VBAT ADC = %dmv\r\n", sadc_convert_value);
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
                    read_ch.channel = HOSAL_SADC_CH_TEMPERATURE;
                    break;

                case HOSAL_SADC_CH_TEMPERATURE:
                    printf("Temp ADC = %dmv\r\n", sadc_convert_value);
                    printf("\r\n");
                    printf("\r\n");
                    read_ch.channel = HOSAL_SADC_CH_AIN7;
                    break;
#else
                    read_ch.channel = HOSAL_SADC_CH_AIN6;
                    break;

                case HOSAL_SADC_CH_AIN6:
                    printf("AIO%d ADC = %dmv\r\n", read_ch.channel, sadc_convert_value);
                    printf("\r\n");
                    printf("\r\n");
                    read_ch.channel = HOSAL_SADC_CH_AIN7;
                    break;
#endif
                default: break;
            }

            hosal_delay_ms(300);
            sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
        }
    }

    while (1) {}
}