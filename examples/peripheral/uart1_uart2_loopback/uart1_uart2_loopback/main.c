#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_uart.h"


uint32_t rx1_finish, tx1_finish, buf_index;
uint32_t rx2_finish, tx2_finish, buf_index;
static uint8_t   sendbuf1[1024], recvbuf1[1024];
static uint8_t   sendbuf2[1024], recvbuf2[1024];


int uart1_dma_tx_callback(void *param) {
    uint32_t *event = param;

    if (event[0] & UART_EVENT_DMA_TX_DONE) {
        tx1_finish = 1;
    }

    return STATUS_SUCCESS;
}

int uart1_dma_rx_callback(void *param) {

    uint32_t *event = param;

    if (event[0] & UART_EVENT_DMA_RX_DONE) {
        rx1_finish = 1;
    }

    return STATUS_SUCCESS;
}

int uart2_dma_tx_callback(void *param) {
    uint32_t *event = param;

    if (event[0] & UART_EVENT_DMA_TX_DONE) {
        tx2_finish = 1;
    }

    return STATUS_SUCCESS;
}

int uart2_dma_rx_callback(void *param) {

    uint32_t *event = param;

    if (event[0] & UART_EVENT_DMA_RX_DONE) {
        rx2_finish = 1;
    }

    return STATUS_SUCCESS;
}

int main(void) {

    hosal_uart_dev_t uart1_dev, uart2_dev;
    uint32_t i, j, index = 0;

    printf("hosal uart_1/uart_2 dma loopback examples %s %s \r\n", __DATE__, __TIME__);

    for (i = 0; i < 1024; i++) {
        sendbuf1[i] = (i + 0x31);
        recvbuf1[i] = 0xFF;

        sendbuf2[i] = (i + 0x31);
        recvbuf2[i] = 0xFF;
    }

    uart1_dev.config.uart_id = 1;//UART 1
    uart1_dev.config.tx_pin = 28;
    uart1_dev.config.rx_pin = 29;
    uart1_dev.config.uart_cfg.baud_rate = UART_BAUDRATE_2000000;
    uart1_dev.config.uart_cfg.data_width = UART_DATA_BITS_8;
    uart1_dev.config.uart_cfg.parity = UART_PARITY_NONE;
    uart1_dev.config.uart_cfg.stop_bits = UART_STOPBIT_ONE;
    uart1_dev.config.uart_cfg.flow_control = UART_HWFC_DISABLED;

    hosal_uart_init(&uart1_dev);

    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_TX_DMA_CALLBACK, uart1_dma_tx_callback, &uart1_dev);

    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_RX_DMA_CALLBACK, uart1_dma_rx_callback, &uart1_dev);

    //* Configure UART to dma interrupt mode */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_DMA);


    uart2_dev.config.uart_id = 2;//UART 2
    uart2_dev.config.tx_pin = 30;
    uart2_dev.config.rx_pin = 31;
    uart2_dev.config.uart_cfg.baud_rate = UART_BAUDRATE_2000000;
    uart2_dev.config.uart_cfg.data_width = UART_DATA_BITS_8;
    uart2_dev.config.uart_cfg.parity = UART_PARITY_NONE;
    uart2_dev.config.uart_cfg.stop_bits = UART_STOPBIT_ONE;
    uart2_dev.config.uart_cfg.flow_control = UART_HWFC_DISABLED;

    hosal_uart_init(&uart2_dev);

    hosal_uart_callback_set(&uart2_dev, HOSAL_UART_TX_DMA_CALLBACK, uart2_dma_tx_callback, &uart2_dev);

    hosal_uart_callback_set(&uart2_dev, HOSAL_UART_RX_DMA_CALLBACK, uart2_dma_rx_callback, &uart2_dev);

    //* Configure UART to dma interrupt mode */
    hosal_uart_ioctl(&uart2_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_DMA);

    tx1_finish = 0; tx2_finish = 0;
    rx1_finish = 0; rx2_finish = 0;

    for (i = 1; i < 1024; i++) {
        printf(".");

        if (i % 63 == 0 && i != 0) {
            printf("\r\n");
        }

        hosal_uart_rx_dma(&uart1_dev, recvbuf1, i);
        hosal_uart_rx_dma(&uart2_dev, recvbuf2, i);

        hosal_uart_tx_dma(&uart1_dev, sendbuf1, i);
        hosal_uart_tx_dma(&uart2_dev, sendbuf2, i);

        vTaskDelay(10);

        while (tx1_finish == 0) {;}
        while (rx1_finish == 0) {;}
        while (tx2_finish == 0) {;}
        while (rx2_finish == 0) {;}

        tx1_finish = 0;
        rx1_finish = 0;

        tx2_finish = 0;
        rx2_finish = 0;

        for (j = 0; j < i; j++) {
            if (sendbuf1[j] != recvbuf2[j]) {
                printf("error %d %x\r\n", j, recvbuf2[j]);
                while (1);
            }

            if (sendbuf2[j] != recvbuf1[j]) {
                printf("error %d %x\r\n", j, recvbuf2[j]);
                while (1);
            }

            recvbuf1[i] = 0xFF;
            recvbuf2[i] = 0xFF;
        }
    }

    printf("\r\nuart1/uart2 demo finish! \n");


    while (1);
}