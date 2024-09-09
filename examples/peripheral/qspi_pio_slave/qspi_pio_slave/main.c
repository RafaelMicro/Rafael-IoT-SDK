#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_qspi.h"
#include "cli.h"
#include "log.h"

// #define Master

#ifdef Master
HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 6, 7, 8, 9, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);
#else
HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 6, 7, 8, 9, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_SLAVE_MODE);
#endif

#define BUFSIZE 4096

uint8_t tx_buf[BUFSIZE];
uint8_t rx_buf[BUFSIZE];

int main(void) {
    hosal_qspi_status_t   err = HOSAL_QSPI_SUCCESS;
    hosal_qspi_baudrate_t baud = HOSAL_QSPI_BAUDRATE_1M;

#ifdef Master
    hosal_qspi_mode_t     mode = HOSAL_QSPI_MASTER_MODE;
#else
    hosal_qspi_mode_t     mode = HOSAL_QSPI_SLAVE_MODE;
#endif
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BAUD_SET, &baud);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_MODE_SET, &mode);
    hosal_qspi_init(&qspi_dev0);

    for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
        tx_buf[i] = i;
    }

    printf("Start...\r\n");
    while (1) {
        err = hosal_qspi_transfer_pio(&qspi_dev0, tx_buf, rx_buf, sizeof(tx_buf), 2000);
        switch (err) {
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
            case HOSAL_QSPI_TIMEOUT:
                printf("Timeout\r\n");
            break;
            case HOSAL_QSPI_INVALID_REQUEST:
                printf("Invalid request\r\n");
                break;
            default:
            break;
        }
    #ifdef Master
        Delay_ms(100);
    #endif
    }
 
    return 0;
}
