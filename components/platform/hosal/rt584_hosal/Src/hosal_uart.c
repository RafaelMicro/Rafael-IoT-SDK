/** @file hosal_uart.c
 * @license
 * @description
 */

#include <stdint.h>
#include "hosal_uart.h"
#include "mcu.h"
#include "stdio.h"
#include "sysctrl.h"

#define MAX_NUMBER_OF_UART  3
#define UART0_BASE_CLK      0

#define UART_ReadLineStatus(pReg) ((pReg)->lsr)

typedef struct {
    UART_T* uart;      /*based hardware address*/
    IRQn_Type irq_num; // UART IRQ Number

    hosal_uart_callback_t rx_cb;
    void* rx_cb_arg;

    hosal_uart_callback_t tx_cb;
    void* tx_cb_arg;

    hosal_uart_callback_t rxdma_cb;
    void* rxdma_cb_arg;

    hosal_uart_callback_t txdma_cb;
    void* txdma_cb_arg;

} uart_handle_t;

static uart_handle_t g_uart_handle[MAX_NUMBER_OF_UART] = {
    {
        .uart = UART0,
        .irq_num = Uart0_IRQn,
    },
    {
        .uart = UART1,
        .irq_num = Uart1_IRQn,
    },
    {
        .uart = UART2,
        .irq_num = Uart2_IRQn,
    },
};

int hosal_uart_finalize(hosal_uart_dev_t* uart_dev) {


    int rval = 0;
    hosal_uart_config_t* cfg = & uart_dev->config;
    UART_T* uart;
    uart = g_uart_handle[cfg->uart_id].uart;

    NVIC_DisableIRQ(g_uart_handle[cfg->uart_id].irq_num);

    uart->isr = 0; /*disable xDMA interrupt*/

    uart->xdma_rx_enable = xDMA_Stop;
    uart->xdma_tx_enable = xDMA_Stop;

    uart->isr = ISR_DMA_RX_INTR | ISR_DMA_TX_INTR; /*write 1 to clear*/

    /*reset uart FIFO*/
    uart->fcr = 0;
    uart->fcr = FCR_CLEAR_XMIT | FCR_CLEAR_RCVR;

    // Disable UART peripheral clock
    disable_perclk((UART0_CLK + cfg->uart_id));

    // Clear pending UART interrupts in NVIC
    NVIC_ClearPendingIRQ(g_uart_handle[cfg->uart_id].irq_num);

    return rval;

}

int hosal_uart_init(hosal_uart_dev_t* uart_dev) {

    int rval = 0;
    hosal_uart_config_t* cfg = & uart_dev->config;
    UART_T* uart;
    uint32_t cval;

    do {

        uart = g_uart_handle[cfg->uart_id].uart;

#if (RT584_SHUTTLE_IC)
        if (cfg->uart_id == 2)
        {
            pin_set_mode(cfg->rx_pin, MODE_UART2); 
            pin_set_mode(cfg->tx_pin, MODE_UART2); 
        }
        else if (cfg->uart_id == 1)
        {
            pin_set_mode(cfg->rx_pin, MODE_UART1); 
            pin_set_mode(cfg->tx_pin, MODE_UART1); 
        }
        else
#endif
        {
            pin_set_mode(cfg->rx_pin, MODE_UART); /*GPIO16 as UART0 RX*/
            pin_set_mode(cfg->tx_pin, MODE_UART); /*GPIO17 as UART0 TX*/            
        }
        NVIC_DisableIRQ(g_uart_handle[cfg->uart_id].irq_num);
        NVIC_SetPriority(g_uart_handle[cfg->uart_id].irq_num, 4);
        // Clear UART IRQ
        NVIC_ClearPendingIRQ(g_uart_handle[cfg->uart_id].irq_num);

        uart->uart_en = UART_DISABLE;           /*Disablea  UART*/

        //enable_perclk((UART0_CLK + cfg->uart_id));
        /*clear FIFO, REMAKR: FCR is write-only*/
        uart->fcr = 0;
        uart->fcr |= FCR_DEFVAL; /*reset FIFO*/

        // Disable interrupts
        uart->ier = 0;

        /*set baudrate*/
        uart->dlx = cfg->baud_rate & 0xFFFF ;
        uart->fdl = (cfg->baud_rate >> UART_BR_FRCT_SHIFT) & 0xFF;
        uart->lsm = 0;

        /*bits mode only use two bits.*/
        cval = cfg->data_width & 0x03;

        /*set stop bits*/
        if (cfg->stop_bits == UART_STOPBIT_TWO) {
            cval |= LCR_STOP;
        }

        /*set parity*/
        if (cfg->parity & PARENB) {
            cval |= LCR_PARITY;
        }
        if (!(cfg->parity & PARODD)) {
            cval |= LCR_EPAR;
        }
        if (cfg->parity & CMSPAR) {
            cval |= LCR_SPAR;
        }

        uart->lcr = cval;


        uart->mcr = 0; /*Initial default modem control register.*/
        /*init native DMA architecture setting*/
        uart->ier = 0; /*disable xDMA interrupt*/

        uart->xdma_rx_enable = xDMA_Stop;
        uart->xdma_tx_enable = xDMA_Stop;

        uart->xdma_tx_len = 0;
        uart->xdma_rx_len = 0;

        uart->isr |= (ISR_RDA_INTR | ISR_THRE_INTR | ISR_RX_TIMEOUT | ISR_DMA_RX_INTR |
                      ISR_DMA_TX_INTR);/*write 1 to clear*/
        uart->lsr |= UART_LSR_BRK_ERROR_BITS;

        uart->fcr |= (FCR_TRIGGER_8 | FCR_DMA_SELECT);
        uart->ier |= UART_IER_RLSI;

        uart->uart_en = UART_ENABLE;


    } while (0);

    return rval;

}

int hosal_uart_send(hosal_uart_dev_t* uart_dev, const void* data,
                uint32_t size) {

    uint32_t i = 0;
    hosal_uart_config_t* cfg = & uart_dev->config;
    UART_T* uart;
    uart = g_uart_handle[cfg->uart_id].uart;
    while (i < size) {
        while ((UART_ReadLineStatus(uart) & UART_LSR_THRE) == 0) {}
        uart->thr = ((uint8_t*)data)[i];
        i++;
    }


    return 0;
}

inline int hosal_uart_receive(hosal_uart_dev_t* uart_dev, void* data,
                   uint32_t expect_size) {

    uint32_t counter = 0;
    hosal_uart_config_t* cfg = & uart_dev->config;

    UART_T* uart;
    uart = g_uart_handle[cfg->uart_id].uart;

    while (counter < expect_size) {
        if (uart->lsr & UART_LSR_DR) {
            ((uint8_t*)data)[counter] = uart->rbr & 0xFF;
        } else {
            break;
        }
        counter++;
    }

    return counter;


}

int hosal_uart_ioctl(hosal_uart_dev_t* uart_dev, int ctl, void* p_arg) {

    hosal_uart_dma_cfg_t* dma_cfg;
    UART_T* uart;
    uint32_t tmp = 0;
    uart = g_uart_handle[uart_dev->config.uart_id].uart;

    switch (ctl) {
        case HOSAL_UART_BAUD_SET:
            uart_dev->config.baud_rate = (uint32_t)p_arg;
            break;
        case HOSAL_UART_BAUD_GET:
            if (p_arg) {
                * (uint32_t*)p_arg = uart_dev->config.baud_rate;
            }
            break;
        case HOSAL_UART_DATA_WIDTH_SET:
            uart_dev->config.data_width = *(hosal_uart_databits_t*)p_arg;
            break;
        case HOSAL_UART_DATA_WIDTH_GET:
            if (p_arg) {
                * (hosal_uart_databits_t*)p_arg = uart_dev->config.data_width;
            }
            break;
        case HOSAL_UART_STOP_BITS_SET:
            uart_dev->config.stop_bits = *(hosal_uart_stopbit_t*)p_arg;
            break;
        case HOSAL_UART_STOP_BITS_GET:
            if (p_arg) {
                * (hosal_uart_stopbit_t*)p_arg = uart_dev->config.stop_bits;
            }
            break;
        case HOSAL_UART_FLOWMODE_SET:
            uart_dev->config.flow_control = (hosal_uart_hwfc_t)p_arg;
            break;
        case HOSAL_UART_FLOWSTAT_GET:
            if (p_arg) {
                *(hosal_uart_hwfc_t*)p_arg = uart_dev->config.flow_control;
            }
            break;
        case HOSAL_UART_PARITY_SET:
            uart_dev->config.parity = (hosal_uart_parity_t)p_arg;
            break;
        case HOSAL_UART_PARITY_GET:
            if (p_arg) {
                *(hosal_uart_parity_t*)p_arg = uart_dev->config.parity;
            }
            break;
        case HOSAL_UART_MODE_SET:
            NVIC_ClearPendingIRQ(
                g_uart_handle[uart_dev->config.uart_id].irq_num);
            uart->ier = 0;
            uart_dev->config.mode = (hosal_uart_mode_t)p_arg;
            
            if (uart_dev->config.mode == HOSAL_UART_MODE_POLL) {
                uart->ier = 0;
            } else if (uart_dev->config.mode == HOSAL_UART_MODE_INT_TX) {
                uart->ier |= UART_IER_THRI;
                NVIC_EnableIRQ(g_uart_handle[uart_dev->config.uart_id].irq_num);
            } else if (uart_dev->config.mode == HOSAL_UART_MODE_INT_RX) {
                uart->ier |= UART_IER_RDI;
                NVIC_EnableIRQ(g_uart_handle[uart_dev->config.uart_id].irq_num);
            } else if (uart_dev->config.mode == HOSAL_UART_MODE_INT) {
                uart->ier |= (UART_IER_THRI | UART_IER_RLSI | UART_IER_RDI | UART_IER_RX_TIMEOUT);
                NVIC_EnableIRQ(g_uart_handle[uart_dev->config.uart_id].irq_num);
            }

            break;
        case HOSAL_UART_MODE_GET:
            if (p_arg) {
                * (hosal_uart_mode_t*)p_arg = uart_dev->config.mode;
            }
            break;

        case HOSAL_UART_TX_TRIGGER_ON: break;
        case HOSAL_UART_TX_TRIGGER_OFF: break;
        case HOSAL_UART_DMA_TX_START:
            dma_cfg = (hosal_uart_dma_cfg_t*)p_arg;
            uart->xdma_tx_addr = (uint32_t)dma_cfg->dma_buf;
            uart->xdma_tx_len = dma_cfg->dma_buf_size;
            uart->ier = UART_IER_DMA_TX_INTR;
            uart->xdma_tx_enable = xDMA_Enable;
            break;
        case HOSAL_UART_DMA_RX_START:
            break;

        case HOSAL_UART_DISABLE_INTERRUPT:
            NVIC_DisableIRQ(g_uart_handle[uart_dev->config.uart_id].irq_num);
            break;

        case HOSAL_UART_ENABLE_INTERRUPT:
            NVIC_EnableIRQ(g_uart_handle[uart_dev->config.uart_id].irq_num);
            break;

        case HOSAL_UART_CLEAR_FIFO:
            tmp = uart->ier;
            uart->ier = 0;
            uart->fcr = 0;
            uart->fcr = FCR_DEFVAL; /*reset FIFO*/
            uart->ier = tmp;
            break;
        default: return -1;
    }

    return 0;
}

int hosal_uart_callback_set(hosal_uart_dev_t* uart_dev, int callback_type,
                        hosal_uart_callback_t pfn_callback, void* arg) {

    hosal_uart_config_t* cfg = & uart_dev->config;

    if (callback_type == HOSAL_UART_TX_CALLBACK) {
        uart_dev->tx_cb = pfn_callback;
        uart_dev->p_tx_arg = arg;
        g_uart_handle[cfg->uart_id].tx_cb = pfn_callback;
        g_uart_handle[cfg->uart_id].tx_cb_arg = arg;

    } else if (callback_type == HOSAL_UART_RX_CALLBACK) {
        uart_dev->rx_cb = pfn_callback;
        uart_dev->p_rx_arg = arg;
        g_uart_handle[cfg->uart_id].rx_cb = pfn_callback;
        g_uart_handle[cfg->uart_id].rx_cb_arg = arg;

    } else if (callback_type == HOSAL_UART_TX_DMA_CALLBACK) {
        uart_dev->txdma_cb = pfn_callback;
        uart_dev->p_txdma_arg = arg;
        g_uart_handle[cfg->uart_id].txdma_cb = pfn_callback;
        g_uart_handle[cfg->uart_id].txdma_cb_arg = arg;

    } else if (callback_type == HOSAL_UART_RX_DMA_CALLBACK) {
        uart_dev->rxdma_cb = pfn_callback;
        uart_dev->p_rxdma_arg = arg;
        g_uart_handle[cfg->uart_id].rxdma_cb = pfn_callback;
        g_uart_handle[cfg->uart_id].rxdma_cb_arg = arg;
    }
    return 0;
}

static inline void __uart_generic_notify_handler(uint8_t id) {

    UART_T* uart = g_uart_handle[id].uart;
    uint32_t iir;

    iir = uart->isr;
    //if ((iir & IIR_INTSTATUS) == 0)
    if ((iir == ISR_RDA_INTR) || (iir == ISR_RX_TIMEOUT)) {
        if (uart->lsr & UART_LSR_DR) {
            if (g_uart_handle[id].rx_cb != NULL) {
                g_uart_handle[id].rx_cb(g_uart_handle[id].rx_cb_arg);
            }
        }
        uart->isr = ISR_RDA_INTR;
        uart->isr = ISR_RX_TIMEOUT;
    }

    if (iir == ISR_THRE_INTR) {
        if (uart->lsr & UART_LSR_THRE) {
            if (g_uart_handle[id].tx_cb != NULL) {
                g_uart_handle[id].tx_cb(g_uart_handle[id].tx_cb_arg);
            }
        }
        uart->isr = ISR_THRE_INTR;
    }

    if ((uart->isr & ISR_DMA_TX_INTR) == ISR_DMA_TX_INTR) {
        uart->isr = ISR_DMA_TX_INTR;
        uart->xdma_tx_enable = xDMA_Stop;
        if (g_uart_handle[id].txdma_cb != NULL) {
            g_uart_handle[id].txdma_cb(g_uart_handle[id].txdma_cb_arg);
        }
    }
    if ((uart->isr & ISR_DMA_RX_INTR) == ISR_DMA_RX_INTR) {
        uart->isr = ISR_DMA_RX_INTR;
        uart->xdma_rx_enable = xDMA_Stop;
        if (g_uart_handle[id].rxdma_cb != NULL) {
            g_uart_handle[id].rxdma_cb(g_uart_handle[id].rxdma_cb_arg);
        }
    }

}

__WEAK void UART0_Handler(void) {
    __uart_generic_notify_handler(0);
    NVIC_ClearPendingIRQ(Uart0_IRQn);
}

__WEAK void UART1_Handler(void) {
    __uart_generic_notify_handler(1);
    NVIC_ClearPendingIRQ(Uart1_IRQn);
}

__WEAK void UART2_Handler(void) {
    __uart_generic_notify_handler(2);
    NVIC_ClearPendingIRQ(Uart2_IRQn);
}