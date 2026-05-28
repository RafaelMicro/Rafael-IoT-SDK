/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "Dev_Config.h"
#include "EPD_Test.h"
#include "EPD_1in54.h"
#include "hosal_sysctrl.h"
#include "hosal_qspi.h"
#include "hosal_gpio.h"
#include "app_hooks.h"
#include "uart_stdio.h"

HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 6, 7, 8, 9, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);


/*this is pin mux setting*/
void qspi_e_paper_init(void)
{
    hosal_gpio_input_config_t pin_cfg;

    hosal_gpio_pin_set(EPD_RST_PIN);
    hosal_pin_set_mode(EPD_RST_PIN, HOSAL_MODE_GPIO);
    hosal_gpio_cfg_output(EPD_RST_PIN);
    hosal_pin_set_pullopt(EPD_RST_PIN, PULL_NONE);

    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_NOINT;
    pin_cfg.usr_cb = NULL;

    hosal_pin_set_mode(EPD_BUSY_PIN, HOSAL_MODE_GPIO);
    hosal_gpio_cfg_input(EPD_BUSY_PIN, pin_cfg);
    hosal_pin_set_pullopt(EPD_BUSY_PIN, HOSAL_PULL_UP_100K);

    hosal_qspi_status_t       err =   HOSAL_QSPI_SUCCESS;

    hosal_qspi_baudrate_t     baud =  HOSAL_QSPI_BAUDRATE_1M;
    hosal_qspi_mode_t         mode =  HOSAL_QSPI_MASTER_MODE;
    hosal_qspi_bitorder_t     order = HOSAL_QSPI_MSB;
    hosal_qspi_bitsize_t      size =  HOSAL_QSPI_DATASIZE_8;
    hosal_qspi_phase_t        phase = HOSAL_QSPI_PHASE_1EDGE;
    hosal_qspi_polarity_t     pol =   HOSAL_QSPI_POLARITY_LOW;
    hosal_qspi_slave_select_t ss =    HOSAL_QSPI_SELECT_SLAVE_0;
    hosal_qspi_cs_polarity_t  spol =  HOSAL_QSPI_CHIPSEL_ACTIVE_LOW;

    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BAUD_SET,            &baud);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_MODE_SET,            &mode);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_DATAWIDTH_SET,       &size);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BITORDER_SET,        &order);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_PHASE_SET,           &phase);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_POLARITY_SET,        &pol);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_SLAVESELECT_SET,     &ss);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_SLAVE_POLARTITY_SET, &spol);

    qspi_dev0.config.qspi_id  = HOSAL_QSPI_ID_0;
    qspi_dev0.config.data2 = 0;
    qspi_dev0.config.data3 = 0;

    hosal_qspi_init(&qspi_dev0);
    NVIC_DisableIRQ(qspi_dev0.irq_num);    
    
}


int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("\r\n");
    printf("Build Data=%s\r\n",__DATE__);
    printf("Build Time=%s\r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal SPI E-PAPER demo\r\n");
    printf("[QSPI config]\r\n");
    printf(" Speed        : 1M\r\n");
    printf("[QSPI Pin]      CLK Pin      : GPIO6 \r\n");
    printf("                CS Pin       : GPIO7 \r\n");
    printf("                MOSI Pin     : GPIO8 \r\n");
    printf("                MISO(DC) Pin : GPIO9 \r\n");
    printf("[epaper busy Pin]            : GPIO4 \r\n");
    printf("[epaper reset Pin]           : GPIO5 \r\n");
    printf("----------------------------------------------------------------\r\n");
    printf("MH-ET LIVE E-paper Module \r\n");

    qspi_e_paper_init();

    EPD_test();
    while(1){;}

}


