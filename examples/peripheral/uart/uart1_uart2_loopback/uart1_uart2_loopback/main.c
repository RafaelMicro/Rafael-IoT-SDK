#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "mcu.h"
#include "hosal_status.h"
#include "sysctrl.h"
#include "hosal_uart.h"
#include "hosal_dma.h"
#include "uart_stdio.h"
#include "hosal_sysctrl.h"

uint32_t rx1_finish, tx1_finish;
uint32_t rx2_finish, tx2_finish;
uint8_t  sendbuf1[1024], sendbuf2[1024];
uint8_t  recvbuf1[1024], recvbuf2[1024];

hosal_uart_dma_cfg_t uart1_dam_tx,uart2_dam_tx;
hosal_uart_dma_cfg_t uart1_dam_rx,uart2_dam_rx;

static hosal_uart_dev_t uart1_dev;
static hosal_uart_dev_t uart2_dev;


int uart1_dma_tx_callback(void *param) {

    uint32_t *event = param;
    
    tx1_finish = 1;
   
    return HOSAL_STATUS_SUCCESS;
}

int uart1_dma_rx_callback(void *param) {

    uint32_t *event = param;

    rx1_finish = 1; 
   
    return HOSAL_STATUS_SUCCESS;
}

int uart2_dma_tx_callback(void *param) {
    uint32_t *event = param;

    tx2_finish = 1;
   

    return HOSAL_STATUS_SUCCESS;
}

int uart2_dma_rx_callback(void *param) {

    uint32_t *event = param;

    rx2_finish = 1; 
  
    return HOSAL_STATUS_SUCCESS;
}

void uart_init()
{
    uart1_dev.config.uart_id = HOSAL_UART1_ID;
    uart1_dev.config.tx_pin = GPIO28;
    uart1_dev.config.rx_pin = GPIO29;
    uart1_dev.config.baud_rate = UART_BAUDRATE_115200;
    uart1_dev.config.data_width = UART_DATA_BITS_8;
    uart1_dev.config.parity = UART_PARITY_NONE;
    uart1_dev.config.stop_bits = UART_STOPBIT_ONE;
    uart1_dev.port = HOSAL_UART1_ID;
    uart1_dev.config.flow_control = UART_HWFC_DISABLED;

    hosal_uart_finalize(&uart1_dev);

    /*Init UART In the first place*/
    hosal_uart_init(&uart1_dev);

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_TX_DMA_CALLBACK,uart1_dma_tx_callback, &uart1_dev);

    /* Configure UART break interrupt callback function */
    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_RX_DMA_CALLBACK,uart1_dma_rx_callback, &uart1_dev);

    /* Configure UART dam to interrupt mode */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODE_SET, (void*)HOSAL_UART_DMA_MODE_INT);


    uart2_dev.config.uart_id = HOSAL_UART2_ID;
    uart2_dev.config.tx_pin = GPIO30;
    uart2_dev.config.rx_pin = GPIO31;
    uart2_dev.config.baud_rate = UART_BAUDRATE_115200;
    uart2_dev.config.data_width = UART_DATA_BITS_8;
    uart2_dev.config.parity = UART_PARITY_NONE;
    uart2_dev.config.stop_bits = UART_STOPBIT_ONE;
    uart2_dev.port = HOSAL_UART2_ID;
    uart2_dev.config.flow_control = UART_HWFC_DISABLED;

    hosal_uart_finalize(&uart2_dev);

    /*Init UART In the first place*/
    hosal_uart_init(&uart2_dev);

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uart2_dev, HOSAL_UART_TX_DMA_CALLBACK,uart2_dma_tx_callback, &uart2_dev);

    /* Configure UART break interrupt callback function */
    hosal_uart_callback_set(&uart2_dev, HOSAL_UART_RX_DMA_CALLBACK,uart2_dma_rx_callback, &uart2_dev);

    /* Configure UART dam to interrupt mode */
    hosal_uart_ioctl(&uart2_dev, HOSAL_UART_MODE_SET, (void*)HOSAL_UART_DMA_MODE_INT);


}


int main(void) {

    uint32_t i = 0,j;
    uart_stdio_init();
    hosal_dma_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : uart1 uart2 dma loopback\r\n");
    printf("[Uart config]\r\n");
    printf(" BuadRate     : 115200\r\n"); 
    printf(" Data Bit     : 8\r\n"); 
    printf(" Parity Bit   : None\r\n"); 
    printf(" Stop Bits    : One\r\n");
    printf(" Flow Control : Disable\r\n\n");
    printf("[Uart1 Pin]     Tx Pin       : GPIO29 \r\n");
    printf("                Rx Pin       : GPIO28 \r\n\n");
    printf("[Uart2 Pin]     Tx Pin       : GPIO31 \r\n");
    printf("                Rx Pin       : GPIO30 \r\n\n");
    printf("Uart1 TX connect Uart2 RX\r\n");
    printf("Uart2 TX connect Uart1 RX\r\n");
    printf("----------------------------------------------------------------\r\n");

    uart_init();

    j = 0;

    for(i=0;i<1024;i++) {
       sendbuf1[i] = i+1;
       recvbuf1[i] = 0xFF;
       sendbuf2[i] = i+1;
       recvbuf2[i] = 0xFF;       
    }

        rx1_finish = 0; rx2_finish = 0;
        tx1_finish = 0; tx2_finish = 0;


        uart1_dam_tx.dma_buf_size = i;
        uart1_dam_tx.dma_buf = (uint8_t*)sendbuf1;

        uart1_dam_rx.dma_buf_size = i;
        uart1_dam_rx.dma_buf = (uint8_t*)recvbuf1;

        uart2_dam_tx.dma_buf_size = i;
        uart2_dam_tx.dma_buf = (uint8_t*)sendbuf2;

        uart2_dam_rx.dma_buf_size = i;
        uart2_dam_rx.dma_buf = (uint8_t*)recvbuf2;   

    for (i = 1; i < 1024; i++) {

        printf(".");

        if (i % 64 == 0 && i != 0) {
            printf("\r\n");
        }

        uart1_dam_tx.dma_buf_size = i;
        uart1_dam_rx.dma_buf_size = i;
        
        uart2_dam_tx.dma_buf_size = i;
        uart2_dam_rx.dma_buf_size = i;

        hosal_uart_ioctl(&uart1_dev, HOSAL_UART_DMA_RX_START,&uart1_dam_rx);
        hosal_uart_ioctl(&uart2_dev, HOSAL_UART_DMA_TX_START,&uart2_dam_tx);
        hosal_delay_ms(100);


        hosal_uart_ioctl(&uart2_dev, HOSAL_UART_DMA_RX_START,&uart2_dam_rx);
        hosal_uart_ioctl(&uart1_dev, HOSAL_UART_DMA_TX_START,&uart1_dam_tx);
        hosal_delay_ms(100);


        while (tx1_finish == 0 || tx2_finish==0) {;} //printf("tx finsih \r\n");
        while (rx1_finish == 0 || rx2_finish==0) {;} //printf("rx finsih \r\n");

        rx1_finish = 0; rx2_finish = 0;
        tx1_finish = 0; tx2_finish = 0;

        for (j = 0; j < i; j++) {

            if (sendbuf1[j] != recvbuf2[j]) {
                printf("error %d %x\r\n", j, recvbuf2[j]);
                while (1);
            }

            if (sendbuf2[j] != recvbuf1[j]) {
                printf("error %d %x\r\n", j, recvbuf1[j]);
                while (1);
            }            
            recvbuf1[j]= 0xFF;
            recvbuf2[j]= 0xFF;
        }

    }

    printf("\r\n\r\n");
    printf("hosal uart1/uart2 loopback finish\r\n");

    while (1);
}