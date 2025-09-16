#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_uart.h"
#include "hosal_dma.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"


uint32_t rx_finish, tx_finish;
uint8_t  sendbuf[1024], recvbuf[1024];

hosal_uart_dma_cfg_t uart1_dam_tx;
hosal_uart_dma_cfg_t uart1_dam_rx;
hosal_uart_dev_t uart1_dev;

void uart_init()
{
    uart1_dev.config.uart_id = HOSAL_UART1_ID;
    uart1_dev.config.tx_pin = 29;
    uart1_dev.config.rx_pin = 28;
    uart1_dev.config.baud_rate = UART_BAUDRATE_115200;
    uart1_dev.config.data_width = UART_DATA_BITS_8;
    uart1_dev.config.parity = UART_PARITY_NONE;
    uart1_dev.config.stop_bits = UART_STOPBIT_ONE;
    uart1_dev.port = HOSAL_UART1_ID;
    uart1_dev.config.flow_control = UART_HWFC_DISABLED;

    hosal_uart_finalize(&uart1_dev);

    /*Init UART In the first place*/
    hosal_uart_init(&uart1_dev);

    /* Configure UART dam to interrupt mode */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODE_SET, (void*)HOSAL_UART_DMA_MODE_POLL);

    //hosal_uart_ioctl(&uart1_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE, NULL);
}


int main(void) {
   
    uint32_t i = 0,j, dma_status=0;

    uart_stdio_init();
    hosal_dma_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : uart1 dma loopback demo\r\n");
    printf("[Uart config]\r\n");
    printf(" BuadRate     : 115200\r\n"); 
    printf(" Data Bit     : 8\r\n"); 
    printf(" Parity Bit   : None\r\n"); 
    printf(" Stop Bits    : One\r\n");
    printf(" Flow Control : Disable\r\n\n");
    printf("[Uart1 Pin]     Tx Pin       : GPIO29 \r\n");
    printf("                Rx Pin       : GPIO28 \r\n\n");
    printf("Uart1 TX connect Uart1 RX\r\n");
    printf("----------------------------------------------------------------\r\n");

    uart_init();

    j = 0;
    for(i=0;i<1024;i++)
    {
       sendbuf[i] = i+1;
       recvbuf[i] = 0xFF;
    }
    tx_finish = 0;
    rx_finish = 0;

        
    for (i = 1; i < 1024; i++) {

        printf(".");

        if (i % 63 == 0 && i != 0) {
            printf("\r\n");
        }

        uart1_dam_tx.dma_buf_size = i;
        uart1_dam_tx.dma_buf = (uint8_t*)sendbuf;

        uart1_dam_rx.dma_buf_size = i;
        uart1_dam_rx.dma_buf = (uint8_t*)recvbuf;

        hosal_uart_ioctl(&uart1_dev, HOSAL_UART_DMA_POL_RX_START,&uart1_dam_rx);
        hosal_uart_ioctl(&uart1_dev, HOSAL_UART_DMA_POL_TX_START,&uart1_dam_tx);
        
        do
        {
           dma_status = hosal_uart_get_dma_int_status(&uart1_dev);
           
        } while ((dma_status&HOSAL_UART_DMA_TX_STATUS)==0 || (dma_status&HOSAL_UART_DMA_RX_STATUS)==0);

         hosal_uart_ioctl(&uart1_dev, HOSAL_UART_CLEAR_DMA_STATUS,(void*)(HOSAL_UART_DMA_RX_STATUS|HOSAL_UART_DMA_TX_STATUS));

        hosal_delay_ms(10);

        for (j = 0; j < i; j++) {
            if (sendbuf[j] != recvbuf[j]) {
                printf("error %d %x\r\n", j, recvbuf[j]);
                while (1);
            }
            recvbuf[j]= 0xFF;
        }

    }

    printf("\r\n\r\n");
    printf("hosal uart1 dma loopback demo finish\r\n");
    while (1);
}

