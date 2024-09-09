#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "main.h"
#include "mcu.h"
#include "openthread_port.h"

#include "hosal_uart.h"

#include "FreeRTOS.h"
#include "task.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_PORT
#define CONFIG_APP_OT_NCP_OPERATION_UART_PORT 0
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_PORT

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN
#define CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN 28
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN
#define CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN 29
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE
#define CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE 500000
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE

#if (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_115200)
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_500000)
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_1000000)
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_2000000)
#endif // CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE

#define OT_NCP_UART_NOTIFY_ISR(ebit)                                           \
    (g_ot_ncp_uart_evt_var |= ebit);                                           \
    ot_ncp_uart_signal()
#define OT_NCP_UART_NOTIFY(ebit)                                               \
    enter_critical_section();                                                  \
    g_ot_ncp_uart_evt_var |= ebit;                                             \
    leave_critical_section();                                                  \
    ot_ncp_uart_signal()
#define OT_NCP_UART_GET_NOTIFY(ebit)                                           \
    enter_critical_section();                                                  \
    ebit = g_ot_ncp_uart_evt_var;                                              \
    g_ot_ncp_uart_evt_var = OT_NCP_UART_EVENT_NONE;                            \
    leave_critical_section()

#define OT_NCP_UART_DATA_CACHE_SIZE 512

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef enum {
    OT_NCP_UART_EVENT_NONE = 0,
    OT_NCP_UART_EVENT_TRIGGER = 0x00000001,
    OT_NCP_UART_EVENT_UART_IN = 0x00000002,
    OT_NCP_UART_EVENT_UART_TIMEOUT = 0x00000010,

    OT_NCP_UART_EVENT_ALL = 0xffffffff,
} ot_ncp_uart_event_t;

typedef struct ncp_uart {
    /* data */
    volatile uint32_t wr_idx;
    volatile uint32_t rd_idx;
    uint8_t rx_cache[OT_NCP_UART_DATA_CACHE_SIZE];
} ot_ncp_uart_io_t;

static ot_ncp_uart_event_t g_ot_ncp_uart_evt_var;

static ot_ncp_uart_io_t g_uart_io = {
    .wr_idx = 0,
};

static uint8_t g_tx_buf[OT_NCP_UART_DATA_CACHE_SIZE];
static uint8_t g_rx_buf[32];

static hosal_uart_dma_cfg_t txdam_cfg = {
    .dma_buf = g_tx_buf,
    .dma_buf_size = sizeof(g_tx_buf),
};

static TaskHandle_t ot_ncp_utask_handle = NULL;

//=============================================================================
//                  Private Function Definition
//=============================================================================
static void _uart_sw_isr_cb(uint32_t sw_id) {
    if (sw_id == 0) {
        OT_NCP_UART_NOTIFY_ISR(OT_NCP_UART_EVENT_TRIGGER);
    }
    if (sw_id == 1) {
        OT_NCP_UART_NOTIFY_ISR(OT_NCP_UART_EVENT_UART_IN);
    }
}
#if (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 1)
void uart1_handler(void) {
    uint32_t wr_pos = g_uart_io.wr_idx;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t pos = 0, cnt;
    uint8_t value[8] = {0};
    UART_T* uart = UART1;
    uint32_t iir;
    uint32_t tx = 0;

    NVIC_ClearPendingIRQ(Uart1_IRQn);

    do {
        /* code */
        iir = uart->IIR & IIR_INTID_MSK;

        if ((uart->xDMA_INT_STATUS & xDMA_ISR_TX) == xDMA_ISR_TX) {
            uart->xDMA_INT_STATUS = xDMA_ISR_TX;
            uart->xDMA_TX_ENABLE = xDMA_Stop;
            swi_int_trigger(0);
        }
        /* Triger receiver FIFO exceeds and in last 4 character times
         didn't read from the FIFO. */
        if ((iir == IIR_INTID_RDA) || (iir == IIR_INTID_CTI)) {
            if (uart->LSR & UART_LSR_DR) {
                cnt = hosal_uart_receive(&cpc_uart_dev, value, 8);

                if (cnt > 0) {
                    pos = (wr_pos + cnt) % OT_NCP_UART_DATA_CACHE_SIZE;
                    if (pos == rd_pos) {
                        swi_int_trigger(1);
                        break;
                    }

                    for (int i = 0; i < cnt; i++)
                        g_uart_io.rx_cache[((wr_pos + i)
                                            % OT_NCP_UART_DATA_CACHE_SIZE)] =
                            value[i];
                    g_uart_io.wr_idx = pos;
                    swi_int_trigger(1);
                }
            }
        }
    } while (0);
}
#endif

#if (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 0)
void uart0_handler(void) {

    uint32_t wr_pos = g_uart_io.wr_idx;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t pos = 0, cnt;
    uint8_t value[8] = {0};
    UART_T* uart = UART0;
    uint32_t iir;

    NVIC_ClearPendingIRQ(Uart0_IRQn);

    do {
        /* code */
        iir = uart->IIR & IIR_INTID_MSK;

        if ((uart->xDMA_INT_STATUS & xDMA_ISR_TX) == xDMA_ISR_TX) {
            uart->xDMA_INT_STATUS = xDMA_ISR_TX;
            uart->xDMA_TX_ENABLE = xDMA_Stop;
            swi_int_trigger(0);
        }

        if ((iir == IIR_INTID_RDA) || (iir == IIR_INTID_CTI)) {
            if (uart->LSR & UART_LSR_DR) {
                cnt = hosal_uart_receive(&cpc_uart_dev, value, 8);

                if (cnt > 0) {
                    pos = (wr_pos + cnt) % OT_NCP_UART_DATA_CACHE_SIZE;
                    if (pos == rd_pos)
                        break;

                    for (int i = 0; i < cnt; i++)
                        g_uart_io.rx_cache[((wr_pos + i)
                                            % OT_NCP_UART_DATA_CACHE_SIZE)] =
                            value[i];
                    g_uart_io.wr_idx = pos;
                    swi_int_trigger(1);
                }
            }
        }
    } while (0);

    return 0;
}
#endif
static size_t _uart_data_read(uint8_t* pbuf, int length) {
    size_t byte_cnt = 0;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t wr_pos = g_uart_io.wr_idx;

    do {
        /* code */
        while (1) {
            if (rd_pos == wr_pos)
                break;
            if (length == byte_cnt)
                break;

            pbuf[byte_cnt++] = g_uart_io.rx_cache[rd_pos];
            rd_pos = (rd_pos + 1) % OT_NCP_UART_DATA_CACHE_SIZE;
        }
        hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DISABLE_INTERRUPT, NULL);
        g_uart_io.rd_idx = rd_pos;
        hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_ENABLE_INTERRUPT, NULL);
    } while (0);

    return byte_cnt;
}

void ot_ncp_uart_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(ot_ncp_utask_handle, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(ot_ncp_utask_handle);
    }
}

static void ot_ncp_task_loop(void* parameters_ptr) {
    ot_ncp_uart_event_t sevent = OT_NCP_UART_EVENT_NONE;
    static uint8_t uart_packet[OT_NCP_UART_DATA_CACHE_SIZE];
    uint32_t data_len = 0;

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);

    if (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 0)
        NVIC_SetPriority(Uart0_IRQn, 0);
    else if (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 1)
        NVIC_SetPriority(Uart1_IRQn, 0);

    memset(uart_packet, 0x00, sizeof(uart_packet));

    for (;;) {
        OT_NCP_UART_GET_NOTIFY(sevent);
        if (OT_NCP_UART_EVENT_TRIGGER & sevent) {
            otNcpHdlcSendDone();
        }

        if (OT_NCP_UART_EVENT_UART_IN & sevent) {
            data_len = _uart_data_read(uart_packet,
                                       OT_NCP_UART_DATA_CACHE_SIZE);
            otNcpHdlcReceive(uart_packet, data_len);
        }
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

static int NcpSend(const uint8_t* aBuf, uint16_t aBufLength) {
    txdam_cfg.dma_buf_size = aBufLength;
    memcpy(txdam_cfg.dma_buf, aBuf, aBufLength);

    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DMA_TX_START, &txdam_cfg);
    return aBufLength;
}

void otAppNcpInit(otInstance* aInstance) { otNcpHdlcInit(aInstance, NcpSend); }

void otrInitUser(otInstance* instance) {
    BaseType_t xReturned;
    /*Init UART In the first place*/
    hosal_uart_init(&cpc_uart_dev);

    swi_int_enable(SWI_ID_0, _uart_sw_isr_cb);
    swi_int_enable(SWI_ID_1, _uart_sw_isr_cb);

    NVIC_SetPriority(Soft_IRQn, 3);

    xReturned = xTaskCreate(ot_ncp_task_loop, "ncp_uart", 512, NULL,
                            (configMAX_PRIORITIES - 4), &ot_ncp_utask_handle);
    if (xReturned != pdPASS) {
        log_error("task create fail\n");
    }
    otAppCliInit((otInstance*)instance);
    otAppNcpInit((otInstance*)instance);
}
