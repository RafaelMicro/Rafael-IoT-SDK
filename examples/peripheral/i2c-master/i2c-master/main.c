#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "hosal_i2c_master.h"
#include "log.h"
#include "task.h"

#define I2CM0_SCL  22
#define I2CM0_SDA  23

#define TIMER0_ID 0
/* Write ATEML I2C flash in 3.3V needs 5ms. */
#define END_TICK  5000 

volatile uint8_t g_transfer_done, timeout;
volatile uint32_t g_i2c_status;

void i2c_finish(uint32_t status) {
    g_i2c_status = status;
    g_transfer_done = 1;
}

void do_i2c_master_test(void) {
    hosal_i2c_slave_data_t dev_cfg2;
    hosal_timer_tick_config_t tick_cfg0;

    uint32_t status;
    uint32_t i, temp, read_bytes;

    tick_cfg0.timeload_ticks = END_TICK;
    tick_cfg0.timeout_ticks = 0;

    /* Please notice: I2C flash maximum write sector is 64 bytes...*/
    uint8_t data[64] = {0x55, 0xAA, 0xC4, 0x37, 0x12, 0x46,
                                      0xC9, 0x33, 0x18, 0xFF, 0x38};
    uint8_t rdata[64];

    hosal_i2c_init(0, I2C_CLOCK_400K);

    /* generate some test pattern to write.. this is simple loop to generate random number */
    for (i = 11; i < 64; i++) {
        temp = ((data[i - 10] * data[i - 9]) - data[i - 7]);
        temp = ((temp & 0x00FF0000) >> 16) + ((temp & 0x0000FF00) >> 8)
               + (temp & 0xFF);
        data[i] = temp & 0xFF;
    }

    puts("write 64 bytes data to ATMEL EEPROM \r\n");

    /* this is I2C write for ATMEL I2C */
    dev_cfg2.dev_addr = 0x50;
    dev_cfg2.reg_addr = 0x00;
    dev_cfg2.bFlag_16bits = 0x01;
    dev_cfg2.i2c_usr_isr = i2c_finish;
    g_transfer_done = 0;

    status = hosal_i2c_write(0, &dev_cfg2, data, 64);

    while (g_transfer_done == 0) {}

    if (g_i2c_status == I2C_STATUS_ERR_NOACK) {
        printf("Why I2C NO ACK ERROR?\r\n");
    }

    /* 
     * According to spec, ATEML I2C EEPROM
     * write should wait a period for write
     */

    hosal_timer_start(TIMER0_ID, tick_cfg0);
    while (!timeout) {}
    hosal_timer_stop(TIMER0_ID);

    puts("read 1~64 bytes from ATMEL I2C EEPROM \r\n");

    dev_cfg2.dev_addr = 0x50;
    dev_cfg2.reg_addr = 0x0000;
    dev_cfg2.bFlag_16bits = 0x01;
    dev_cfg2.i2c_usr_isr = i2c_finish;

    for (i = 1; i < 64; i++) {
        memset(rdata, 0, 64);
        read_bytes = i;

        g_transfer_done = FALSE;
        /* read/write bytes can NOT be zero!! */
        status = hosal_i2c_read(0, &dev_cfg2, rdata, read_bytes);
        if (status != STATUS_SUCCESS) {
            printf("i2c_read error why \n");
        }

        while (g_transfer_done == FALSE) {
            /*wait I2C finish transfer*/
        }

        if (g_i2c_status == I2C_STATUS_ERR_NOACK) {
            printf("Why I2C NO ACK ERROR?\r\n");
        }

        /*compare data */
        for (temp = 0; temp < read_bytes; ++temp) {
            if (rdata[temp] != data[temp]) {
                printf("data mismatch \r\n");
            }
        }
    }
    puts("I2C read write verify End \r\n");
}

void init_default_pin_mux(void) {
    /* set I2C GPIO22 SCL, GPIO23 SDA */
    hosal_i2c_preinit(I2CM0_SCL, I2CM0_SDA);
    return;
}

void timer0_cb(uint32_t timer_id) {
    timeout = 1;
    return;
}

void init_timer(void) {
    hosal_timer_config_t cfg0;
    hosal_timer_tick_config_t tick_cfg0;

    cfg0.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg0.int_enable = HOSAL_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_TIMER_FREERUN_MODE;
    cfg0.one_shot_mode = HOSAL_TIMER_ONE_SHOT_ENABLE;
    cfg0.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg0.user_prescale = 0;

    tick_cfg0.timeload_ticks = END_TICK;
    tick_cfg0.timeout_ticks = 0;

    timeout = 0;

    hosal_timer_init(TIMER0_ID, cfg0, timer0_cb);
    hosal_timer_start(TIMER0_ID, tick_cfg0);
}

int main(void) {

    init_default_pin_mux();

    puts("/*******Start I2C Master *******/\r\n");

    puts("I2C master0 => Please connect I2C device in SCL--GPIO22 and "
         "SDA--GPIO23 \r\n");
    puts("We use AT24C EEPROM I2C address is 0x50 \r\n");

    init_timer();

    do_i2c_master_test();

    while (1) { }
}
