#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcu.h"
#include "hosal_qspi.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"

#define BUFSIZE 4096

HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 6, 7, 8, 9, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);
HOSAL_QSPI_DEV_DECL(qspi_dev1, QSPI1, 28, 29, 30, 31, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_SLAVE_MODE);

uint8_t master_tx_buf[BUFSIZE];
uint8_t master_rx_buf[BUFSIZE];

uint8_t slave_tx_buf[BUFSIZE];
uint8_t slave_rx_buf[BUFSIZE];

volatile bool qspi0_flag, qspi1_flag;

void qspi0_transfer_cb(void *arg) {  
    hosal_qspi_dev_t *ptr = arg;

    qspi0_flag = true;
}

void qspi1_transfer_cb(void *arg) {
    hosal_qspi_dev_t *ptr = arg;

    qspi1_flag = true;
}

int main(void) {

    hosal_qspi_status_t   err1, err2;

    hosal_qspi_baudrate_t master_baud = HOSAL_QSPI_BAUDRATE_1M;
    hosal_qspi_mode_t     master_mode = HOSAL_QSPI_MASTER_MODE;

    hosal_qspi_baudrate_t slave_baud = HOSAL_QSPI_BAUDRATE_1M;
    hosal_qspi_mode_t     slave_mode = HOSAL_QSPI_SLAVE_MODE;

    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal QSPI dma loopback demo\r\n");
    printf("[QSPI0 config]\r\n");
    printf(" Speed        : 1M\r\n"); 
    printf(" Role         : Master\r\n"); 
    printf(" Bit Order    : MSB\r\n"); 
    printf(" POL          : HOSAL_QSPI_POLARITY_LOW\r\n");
    printf(" PHA          : HOSAL_QSPI_PHASE_1EDGE\r\n");
    printf(" data_width   : HOSAL_QSPI_DATASIZE_8\r\n\n");
    printf(" cs_polarity  : HOSAL_QSPI_CHIPSEL_ACTIVE_LOW\r\n\n");
    printf("[QSPI1 config]\r\n");
    printf(" Speed        : 1M\r\n"); 
    printf(" Role         : Slave\r\n"); 
    printf(" Bit Order    : MSB\r\n"); 
    printf(" POL          : HOSAL_QSPI_POLARITY_LOW\r\n");
    printf(" PHA          : HOSAL_QSPI_PHASE_1EDGE\r\n");
    printf(" data_width   : HOSAL_QSPI_DATASIZE_8\r\n\n");
    printf(" cs_polarity  : HOSAL_QSPI_CHIPSEL_ACTIVE_LOW\r\n\n");
    printf("[QSPI0 Pin]     CS Pin       : GPIO7 \r\n");
    printf("                CLK Pin      : GPIO6 \r\n\n");
    printf("                MOSI Pin     : GPIO8 \r\n\n");
    printf("                MISO Pin     : GPIO9 \r\n\n");
    printf("[QSPI1 Pin]     CS Pin       : GPIO29 \r\n");
    printf("                CLK Pin      : GPIO28 \r\n\n");
    printf("                MOSI Pin     : GPIO30 \r\n\n");
    printf("                MOSI Pin     : GPIO31 \r\n\n"); 
    printf("QSPI0 CS connect QSPI1 CS\r\n");
    printf("QSPI0 CLK connect QSPI1 CLK\r\n");
    printf("QSPI0 MOSI connect QSPI1 MOSI\r\n");
    printf("QSPI0 MISO connect QSPI1 MISO\r\n");
    printf("----------------------------------------------------------------\r\n");



    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BAUD_SET, &master_baud);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_MODE_SET, &master_mode);
    hosal_qspi_callback_register(&qspi_dev0, HOSAL_QSPI_TRANSFER_DMA, qspi0_transfer_cb, &qspi_dev0);
    hosal_qspi_init(&qspi_dev0);
    NVIC_SetPriority(qspi_dev0.irq_num, 2);
    NVIC_EnableIRQ(qspi_dev0.irq_num);

    hosal_qspi_ioctl(&qspi_dev1, HOSAL_QSPI_BAUD_SET, &slave_baud);
    hosal_qspi_ioctl(&qspi_dev1, HOSAL_QSPI_MODE_SET, &slave_mode);
    hosal_qspi_callback_register(&qspi_dev1, HOSAL_QSPI_TRANSFER_DMA, qspi1_transfer_cb, &qspi_dev1);
    hosal_qspi_init(&qspi_dev1);
    NVIC_SetPriority(qspi_dev1.irq_num, 2);
    NVIC_EnableIRQ(qspi_dev1.irq_num); 

    for (uint32_t i = 0; i < sizeof(master_tx_buf); i++) {
        master_tx_buf[i] = i;
        slave_tx_buf[i] = i;
    }

    while (1) {
        err1 = hosal_qspi_transfer_dma(&qspi_dev1, slave_tx_buf, slave_rx_buf, sizeof(slave_tx_buf));
        err2 = hosal_qspi_transfer_dma(&qspi_dev0, master_tx_buf, master_rx_buf, sizeof(master_tx_buf));
        while (!qspi0_flag) {}
        while (!qspi1_flag) {}
        
        for (uint32_t i = 0; i < sizeof(master_tx_buf); i++) {
            printf("%02x, %02x\r\n", master_rx_buf[i], slave_tx_buf[i]);
            if (master_rx_buf[i] != slave_tx_buf[i]) {
                printf("Error\r\n");
            }
        }
        memset(master_rx_buf, 0, sizeof(master_rx_buf));
        memset(slave_rx_buf, 0, sizeof(slave_rx_buf));
        qspi0_flag = false;
        qspi1_flag = false;
        hosal_delay_ms(500);
    }
 

    printf("\r\n\r\n");
    printf("hosal QSPI dma loopback finish\r\n");
 
    return 0;
}
