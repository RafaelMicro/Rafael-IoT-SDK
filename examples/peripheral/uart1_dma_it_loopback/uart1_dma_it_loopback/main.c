#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cli.h"
#include "log.h"
#include "hosal_uart.h"


uint32_t rx_finish, tx_finish, buf_index;
static uint8_t   sendbuf[1024], recvbuf[1024];
SemaphoreHandle_t xSemaphore = NULL;

#define UART1_HWFL_ENABLE       1
#define UART1_HWFL_DISABLE      0

int uart_dma_tx_callback(void *param) {
    uint32_t *event = param;

    if (event[0] & UART_EVENT_DMA_TX_DONE) {
        tx_finish = 1;
    }

    return STATUS_SUCCESS;
}

int uart_dma_rx_callback(void *param) {

    uint32_t *event = param;

    if (event[0] & UART_EVENT_DMA_RX_DONE) {
        rx_finish = 1;
    }

    return STATUS_SUCCESS;
}

int main(void) {

    hosal_uart_dev_t uart_dev;
    uint32_t i, j, index = 0;

    printf("hosal uart_1 dma interrupt loop back examples %s %s \r\n", __DATE__, __TIME__);


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

    hosal_uart_callback_set(&uart_dev, HOSAL_UART_TX_DMA_CALLBACK, uart_dma_tx_callback, &uart_dev);

    hosal_uart_callback_set(&uart_dev, HOSAL_UART_RX_DMA_CALLBACK, uart_dma_rx_callback, &uart_dev);

    //* Configure UART to dma interrupt mode */
    hosal_uart_ioctl(&uart_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_DMA);

    tx_finish = 0;
    rx_finish = 0;

    for (i = 1; i < 1024; i++) {
        printf(".");

        if (i % 63 == 0 && i != 0) {
            printf("\r\n");
        }

        hosal_uart_rx_dma(&uart_dev, recvbuf, i);
        hosal_uart_tx_dma(&uart_dev, sendbuf, i);

        vTaskDelay(10);

        while (rx_finish == 0) {;}
        while (tx_finish == 0) {;}

        tx_finish = 0;
        rx_finish = 0;

        for (j = 0; j < i; j++) {
            if (sendbuf[j] != recvbuf[j]) {
                printf("error %d %x\r\n", j, recvbuf[j]);
                while (1);
            }
        }
    }

    printf("\r\nuart1 demo finish! \n");


    while (1);
}

