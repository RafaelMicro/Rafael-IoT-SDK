#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_uart.h"
#include "hosal_timer.h"
#include "hosal_dma.h"
#include "uart_stdio.h"
#include "hosal_sysctrl.h"
#include "hosal_status.h"
#include "gpio.h"

#define RT_TIMER0     0

volatile uint32_t rx_finish, tx_finish;
volatile uint32_t counter;
uint8_t  sendbuf[1024], recvbuf[1024];

static hosal_uart_dma_cfg_t uart1_dam_tx;
static hosal_uart_dma_cfg_t uart1_dam_rx;
static hosal_uart_dev_t uart1_dev;
volatile uint32_t  test_count = 0;


int uart_mode_callback(void *param) {

    uint32_t *event = param;

    return HOSAL_STATUS_SUCCESS;
}

int uart_dma_tx_callback(void *param) {
    uint32_t *event = param;

    tx_finish = 1;
  
    return HOSAL_STATUS_SUCCESS;
}

int uart_dma_rx_callback(void *param) {

    uint32_t *event = param;

    rx_finish = 1; 

    return HOSAL_STATUS_SUCCESS;
}


/* timer0 callback function */
void timer0_cb(uint32_t timer_id) {

    uint32_t value;
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODEM_RTS_SET, (void*)(test_count & 1));
    test_count++;
    return;
}


void timer_periodic_mode(void) {
    hosal_timer_config_t cfg;
    hosal_timer_tick_config_t tick_cfg;

    /* This setting is 500ms timeout */
    cfg.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg.user_prescale = 0;

    /*
    32000000/32 = 1000000Mhz
    1/2000000  = 1 us
    122343*1us = 122.343ms
    */
    tick_cfg.timeload_ticks = 122343;
    tick_cfg.timeout_ticks = 0;

    hosal_timer_init(RT_TIMER0, cfg, timer0_cb);
    NVIC_EnableIRQ((IRQn_Type)(Timer0_IRQn));
    hosal_timer_start(RT_TIMER0, tick_cfg);

}


void uart_init()
{

    uart1_dev.config.uart_id = HOSAL_UART1_ID;
    uart1_dev.config.tx_pin = GPIO29;
    uart1_dev.config.rx_pin = GPIO28;
    uart1_dev.config.rts_pin = GPIO20;
    uart1_dev.config.cts_pin = GPIO21;   
    uart1_dev.config.baud_rate = UART_BAUDRATE_115200;
    uart1_dev.config.data_width = UART_DATA_BITS_8;
    uart1_dev.config.parity = UART_PARITY_NONE;
    uart1_dev.config.stop_bits = UART_STOPBIT_ONE;
    uart1_dev.port = HOSAL_UART1_ID;
    uart1_dev.config.flow_control = UART_HWFC_ENABLED;

    hosal_uart_finalize(&uart1_dev);

    /*Init UART In the first place*/
    hosal_uart_init(&uart1_dev);

            /* clear UART dam interrupt status */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_CLEAR_DMA_STATUS, (void*)(HOSAL_UART_DMA_TX_STATUS|HOSAL_UART_DMA_RX_STATUS));
    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_TX_DMA_CALLBACK,uart_dma_tx_callback, &uart1_dev);

    /* Configure UART break interrupt callback function */
    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_RX_DMA_CALLBACK,uart_dma_rx_callback, &uart1_dev);

    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_MODE_CALLBACK,uart_mode_callback, &uart1_dev);

    /* Configure UART dam to interrupt mode */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODE_SET, (void*)HOSAL_UART_DMA_MODE_INT_TX);
     /* Configure UART dam to interrupt mode */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODE_SET, (void*)HOSAL_UART_DMA_MODE_INT_RX);   

    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_FLOWMODE_SET, (void*)TRUE);

}


int main(void) {
    

    uint32_t i = 0,j;
    
    uart_stdio_init();
    hosal_dma_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : uart1 hardware flow control demo\r\n");
    printf("[Uart config]\r\n");
    printf(" BuadRate     : 115200\r\n"); 
    printf(" Data Bit     : 8\r\n"); 
    printf(" Parity Bit   : None\r\n"); 
    printf(" Stop Bits    : One\r\n");
    printf(" Flow Control : Enable\r\n\n");
    printf("[Uart1 Pin]     Tx Pin       : GPIO29 \r\n");
    printf("                Rx Pin       : GPIO28 \r\n");
    printf("                Cts Pin      : GPIO21 \r\n");
    printf("                Rts Pin      : GPIO20 \r\n\n");   
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
       
    uart1_dam_tx.dma_buf = (uint8_t*)sendbuf;
    uart1_dam_rx.dma_buf = (uint8_t*)recvbuf;

    timer_periodic_mode();    

    for (i = 1; i < 1024; i++) {

        printf(".");

        if (i % 63 == 0 && i != 0) {
            printf("\r\n");
        }

        uart1_dam_tx.dma_buf_size = i;
        uart1_dam_rx.dma_buf_size = i;

        hosal_uart_ioctl(&uart1_dev, HOSAL_UART_DMA_RX_START,&uart1_dam_rx); 
        hosal_uart_ioctl(&uart1_dev, HOSAL_UART_DMA_TX_START,&uart1_dam_tx);

        while ((tx_finish == 0) || (rx_finish == 0)) {;}

        tx_finish = 0;
        rx_finish = 0;

        for (j = 0; j < i; j++) {
            if (sendbuf[j] != recvbuf[j]) {
                printf("error %d %x\r\n", j, recvbuf[j]);
                while (1);
            }
            recvbuf[j]= 0xFF;
        } 
    }

    timer_stop(RT_TIMER0);
    printf("\r\n\r\n");
    printf("hosal uart1 hardware flow control demo finish!\r\n");

    while (1);
}

