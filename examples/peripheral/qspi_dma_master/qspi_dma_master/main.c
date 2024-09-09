#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_qspi.h"
#include "cli.h"
#include "log.h"

#define BUFSIZE 4096

HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 6, 7, 8, 9, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);

uint8_t tx_buf[BUFSIZE];
uint8_t rx_buf[BUFSIZE];

volatile bool qspi0_flag;

void qspi0_transfer_cb(void *arg) {
    hosal_qspi_dev_t *ptr = arg;

    qspi0_flag = true;
}

int main(void) {
    hosal_qspi_status_t   err = HOSAL_QSPI_SUCCESS;
    hosal_qspi_baudrate_t baud = HOSAL_QSPI_BAUDRATE_4M;
    hosal_qspi_mode_t     mode = HOSAL_QSPI_MASTER_MODE;

    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BAUD_SET, &baud);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_MODE_SET, &mode);
    hosal_qspi_callback_register(&qspi_dev0, HOSAL_QSPI_TRANSFER_DMA, qspi0_transfer_cb, &qspi_dev0);
    hosal_qspi_init(&qspi_dev0);
    NVIC_SetPriority(qspi_dev0.irq_num, 0);
    NVIC_EnableIRQ(qspi_dev0.irq_num);

    for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
        tx_buf[i] = i;
    }

    while (1) {
        err = hosal_qspi_transfer_dma(&qspi_dev0, tx_buf, rx_buf, sizeof(tx_buf));

        while (!qspi0_flag) {}

        switch(err) {
        case HOSAL_QSPI_SUCCESS: {
            for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
                printf("%02x, %02x ", tx_buf[i], rx_buf[i]);
                if (tx_buf[i] != rx_buf[i]) {
                    printf("Validation failure");
                }
                printf("\r\n");
            }
            memset(rx_buf, 0, sizeof(rx_buf));
        }
        break;
        default:
        break;
        }
        qspi0_flag = false;
        Delay_ms(100);
    }
 
    return 0;
}
