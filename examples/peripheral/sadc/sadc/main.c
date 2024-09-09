#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_sadc.h"
#include "log.h"
#include "task.h"


volatile hosal_sadc_convert_state_t sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
volatile uint32_t sadc_convert_channel;
volatile hosal_sadc_value_t sadc_convert_value;

void init_default_pin_mux(void) {
    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++) {
        if ((i != 16) && (i != 17)) {
            pin_set_mode(i, MODE_GPIO);
        }
    }

    return;
}

void sadc_int_callback_handler(hosal_sadc_cb_t* p_cb) {
    if (p_cb->type == SADC_CB_SAMPLE) {
        sadc_convert_channel = p_cb->data.sample.channel;
        sadc_convert_value = p_cb->data.sample.value;
        sadc_convert_status = SADC_CONVERT_DONE;

        printf("ADC CH%d: adc = %x, comp = %x, cal = %x, value = %x\r\n",
               p_cb->data.sample.channel, p_cb->raw.conversion_value,
               p_cb->raw.compensation_value, p_cb->raw.calibration_value,
               p_cb->data.sample.value);
    }
}

int main(void) {
    hosal_sadc_config_t sadc_set;
    hosal_sadc_channel_config_t read_ch;

    /*we should set pinmux here or in SystemInit */
    init_default_pin_mux();

    puts("/*******Start SADC2314********/\r\n");

    sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
    read_ch.channel = HOSAL_SADC_CH_AIN7;
    sadc_set.oversample = HOSAL_SADC_OVERSAMPLE_256;
    sadc_set.resolution = HOSAL_SADC_RES_12BIT;

    hosal_sadc_config_enable(sadc_set, sadc_int_callback_handler);

    hosal_sadc_compensation_init(1);

    while (1) {
        if (sadc_convert_status == HOSAL_SADC_CONVERT_IDLE) {
            sadc_convert_status = HOSAL_SADC_CONVERT_START;

            if (hosal_sadc_channel_read(read_ch) != STATUS_SUCCESS) {
                sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
            }
        }

        if (sadc_convert_status == HOSAL_SADC_CONVERT_DONE) {

            switch (read_ch.channel) {
                case HOSAL_SADC_CH_AIN7:
                    printf("AIO7 ADC = %dmv\r\n", sadc_convert_value);
                    read_ch.channel = HOSAL_SADC_CH_VBAT;
                    break;

                case HOSAL_SADC_CH_VBAT:
                    printf("VBAT ADC = %dmv\r\n", sadc_convert_value);
                    read_ch.channel = HOSAL_SADC_CH_AIN6;
                    break;

                case HOSAL_SADC_CH_AIN6:
                    printf("AIO6 ADC = %dmv\r\n", sadc_convert_value);
                    puts("\r\n");
                    puts("\r\n");
                    read_ch.channel = HOSAL_SADC_CH_AIN7;
                    break;

                default: break;
            }

            vTaskDelay(300);
            sadc_convert_status = HOSAL_SADC_CONVERT_IDLE;
        }
    }

    while (1) {}
}