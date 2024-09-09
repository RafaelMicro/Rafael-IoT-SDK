#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_qspi.h"
#include "cli.h"
#include "log.h"

HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 6, 7, 8, 9, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);

#define BUFSIZE 4096

uint8_t tx_buf[BUFSIZE];
uint8_t rx_buf[BUFSIZE];

int main(void) {
    hosal_qspi_status_t       err =   HOSAL_QSPI_SUCCESS;

    hosal_qspi_baudrate_t     baud =  HOSAL_QSPI_BAUDRATE_16M;
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

    // printf("baud  : %d\r\n", (hosal_qspi_baudrate_t)baud);
    // printf("mode  : %d\r\n", (hosal_qspi_mode_t)mode);
    // printf("size  : %d\r\n", (hosal_qspi_bitsize_t)size);
    // printf("order : %d\r\n", (hosal_qspi_bitorder_t)order);
    // printf("phase : %d\r\n", (hosal_qspi_phase_t)phase);
    // printf("pol   : %d\r\n", (hosal_qspi_polarity_t)pol);
    // printf("ss    : %d\r\n", (hosal_qspi_slave_select_t)ss);
    // printf("spol  : %d\r\n", (hosal_qspi_cs_polarity_t)spol);

    // hosal_qspi_baudrate_t     baud1;
    // hosal_qspi_mode_t         mode1;
    // hosal_qspi_bitorder_t     order1;
    // hosal_qspi_bitsize_t      size1;
    // hosal_qspi_phase_t        phase1;
    // hosal_qspi_polarity_t     pol1;
    // hosal_qspi_slave_select_t ss1;
    // hosal_qspi_cs_polarity_t  spol1; 

    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BAUD_GET,            &baud1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_MODE_GET,            &mode1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_DATAWIDTH_GET,       &size1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BITORDER_GET,        &order1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_PHASE_GET,           &phase1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_POLARITY_GET,        &pol1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_SLAVESELECT_GET,     &ss1);
    // hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_SLAVE_POLARTITY_GET, &spol1);

    // printf("baud  : %d\r\n", (hosal_qspi_baudrate_t)baud1);
    // printf("mode  : %d\r\n", (hosal_qspi_mode_t)mode1);
    // printf("size  : %d\r\n", (hosal_qspi_bitsize_t)size1);
    // printf("order : %d\r\n", (hosal_qspi_bitorder_t)order1);
    // printf("phase : %d\r\n", (hosal_qspi_phase_t)phase1);
    // printf("pol   : %d\r\n", (hosal_qspi_polarity_t)pol1);
    // printf("ss    : %d\r\n", (hosal_qspi_slave_select_t)ss1);
    // printf("spol  : %d\r\n", (hosal_qspi_cs_polarity_t)spol1);

    hosal_qspi_init(&qspi_dev0);

    for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
        tx_buf[i] = i;
    }

    printf("Start...\r\n");
    while (1) {
        err = hosal_qspi_transfer_pio(&qspi_dev0, tx_buf, rx_buf, sizeof(tx_buf), 1000);
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
        Delay_ms(1000);
    }
 
    return 0;
}
