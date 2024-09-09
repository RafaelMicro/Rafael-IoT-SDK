#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_uart.h"


uint32_t rx_finish, tx_finish, buf_index;
static uint8_t   sendbuf[1024], recvbuf[1024];


int main(void) {

    hosal_uart_dev_t uart_dev;
    uint32_t i, index = 0;

    printf("hosal uart_1 polling loop back examples %s %s \r\n", __DATE__, __TIME__);


    for (i = 0; i < 1024; i++) {
        sendbuf[i] = (i + 0x31);
        recvbuf[i] = 0xFF;
    }

    uart_dev.config.uart_id = 1;//UART 1
    uart_dev.config.tx_pin = 28;
    uart_dev.config.rx_pin = 29;
    uart_dev.config.uart_cfg.baud_rate = UART_BAUDRATE_2000000;
    uart_dev.config.uart_cfg.data_width = UART_DATA_BITS_8;
    uart_dev.config.uart_cfg.parity = UART_PARITY_NONE;
    uart_dev.config.uart_cfg.stop_bits = UART_STOPBIT_ONE;
    uart_dev.config.uart_cfg.flow_control = UART_HWFC_DISABLED;

    hosal_uart_init(&uart_dev);

    ///* Configure UART to polling mode */
    hosal_uart_ioctl(&uart_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_POLL);

    for (i = 0; i < 1024; i++) {
        printf(".");

        if (i % 63 == 0 && i != 0) {
            printf("\r\n");
        }

        while (!hosal_uart_tx_ready(&uart_dev)) {;}
        hosal_uart_tx_pol(&uart_dev, &sendbuf[i]);

        while (!hosal_uart_rx_ready(&uart_dev)) {;}
        hosal_uart_rx_pol(&uart_dev, &recvbuf[i]);


        if (sendbuf[i] != recvbuf[i]) {
            printf("error %d %x\r\n", i, recvbuf[i]);
            while (1);
        }
    }

    printf("\r\nuart1 demo finish! \n");


    while (1);
}

