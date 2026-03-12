/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hosal_dma.h"
#include "hosal_status.h"
#include "hosal_uart.h"
#include "log.h"
#include "main.h"
#include "mcu.h"
#include "miu_port.h"
#include "uart_stdio.h"

#include <timers.h>
#include "FreeRTOS.h"
#include "task.h"

#include <openthread/ncp.h>

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_PORT
#define CONFIG_APP_OT_NCP_OPERATION_UART_PORT 1
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_PORT

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN
#define CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN 28
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN
#define CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN 29
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN

#ifndef CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE
#define CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE 115200
#endif // !CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE

#if (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(ncp_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_115200)
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(ncp_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_500000)
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(ncp_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_1000000)
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(ncp_uart_dev, CONFIG_APP_OT_NCP_OPERATION_UART_PORT,
                    CONFIG_APP_OT_NCP_OPERATION_UART_TX_PIN,
                    CONFIG_APP_OT_NCP_OPERATION_UART_RX_PIN,
                    UART_BAUDRATE_2000000)
#endif // CONFIG_APP_OT_NCP_OPERATION_UART_BAUDRATE

#if (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 0)
#define OT_NCP_UART_IRQn Uart0_IRQn
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 1)
#define OT_NCP_UART_IRQn Uart1_IRQn
#elif (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 2)
#define OT_NCP_UART_IRQn Uart2_IRQn
#else
#error "Invalid CONFIG_APP_OT_NCP_OPERATION_UART_PORT"
#endif

#define OT_NCP_UART_DATA_CACHE_SIZE 128
#define OT_NCP_UART_RX_BUFF_SIZE    484

static TimerHandle_t app_ncp_rx_cb_time = NULL;

typedef struct uart_io {
    uint16_t start;
    uint16_t end;

    uint32_t recvLen;
    uint8_t uart_cache[OT_NCP_UART_RX_BUFF_SIZE];
} uart_io_t;

static uart_io_t g_ncp_uart_rx_io = {.start = 0, .end = 0, .recvLen = 0};

static uint8_t g_tx_buf[OT_NCP_UART_DATA_CACHE_SIZE];

static hosal_uart_dma_cfg_t ncp_uart_dam_tx_cfg = {
    .dma_buf = g_tx_buf,
    .dma_buf_size = sizeof(g_tx_buf),
};

static TaskHandle_t ot_ncp_utask_handle = NULL;

static void ot_ncp_uart_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(ot_ncp_utask_handle, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(ot_ncp_utask_handle);
    }
}

static int ncp_uart_rx_read(uint8_t* p_data, uint32_t p_data_len) {
    if (p_data == NULL || p_data_len == 0) {
        return -1; // Prevent invalid reads
    }
    uint32_t available_data = 0;

    taskENTER_CRITICAL();
    if (g_ncp_uart_rx_io.start >= g_ncp_uart_rx_io.end) {
        available_data = g_ncp_uart_rx_io.start - g_ncp_uart_rx_io.end;
    } else {
        available_data = OT_NCP_UART_RX_BUFF_SIZE - g_ncp_uart_rx_io.end
                         + g_ncp_uart_rx_io.start;
    }
    taskEXIT_CRITICAL();

    if (available_data == 0) {
        return 0; // No readable data
    }

    uint32_t read_len = (p_data_len > available_data) ? available_data
                                                      : p_data_len;

    // Ring buffer reads data
    if (g_ncp_uart_rx_io.end + read_len < OT_NCP_UART_RX_BUFF_SIZE) {
        // Read directly from end
        memcpy(p_data, g_ncp_uart_rx_io.uart_cache + g_ncp_uart_rx_io.end,
               read_len);
        taskENTER_CRITICAL();
        g_ncp_uart_rx_io.end += read_len;
        taskEXIT_CRITICAL();
    } else {
        // Read the tail part first
        uint32_t tail_len = OT_NCP_UART_RX_BUFF_SIZE - g_ncp_uart_rx_io.end;
        memcpy(p_data, g_ncp_uart_rx_io.uart_cache + g_ncp_uart_rx_io.end,
               tail_len);

        // Then read the rest from the beginning
        uint32_t head_len = read_len - tail_len;
        memcpy(p_data + tail_len, g_ncp_uart_rx_io.uart_cache, head_len);
        taskENTER_CRITICAL();
        g_ncp_uart_rx_io.end = head_len; // Update `end` position
        taskEXIT_CRITICAL();
    }

    // Update `recvLen`
    taskENTER_CRITICAL();
    g_ncp_uart_rx_io.recvLen -= read_len;
    taskEXIT_CRITICAL();

    return read_len;
}

static int ncp_uart_rx_callback(void* p_arg) {
    uint32_t new_data_len = 0;

    // Ring buffer writes (make sure not to exceed RX_BUFF_SIZE)
    if (g_ncp_uart_rx_io.start >= g_ncp_uart_rx_io.end) {
        new_data_len = hosal_uart_receive(
            p_arg, g_ncp_uart_rx_io.uart_cache + g_ncp_uart_rx_io.start,
            OT_NCP_UART_RX_BUFF_SIZE - g_ncp_uart_rx_io.start);
        g_ncp_uart_rx_io.start = (g_ncp_uart_rx_io.start + new_data_len)
                                 % OT_NCP_UART_RX_BUFF_SIZE;
    } else if (((g_ncp_uart_rx_io.start + 1) % OT_NCP_UART_RX_BUFF_SIZE)
               != g_ncp_uart_rx_io.end) {
        new_data_len = hosal_uart_receive(
            p_arg, g_ncp_uart_rx_io.uart_cache + g_ncp_uart_rx_io.start,
            g_ncp_uart_rx_io.end - g_ncp_uart_rx_io.start - 1);
        g_ncp_uart_rx_io.start = (g_ncp_uart_rx_io.start + new_data_len)
                                 % OT_NCP_UART_RX_BUFF_SIZE;
    }
    if (new_data_len > 0) {
        if (app_ncp_rx_cb_time) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xTimerResetFromISR(app_ncp_rx_cb_time, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    return 0;
}

void app_ncp_uart_rx_cb_timeout_callback(TimerHandle_t xTimer) {
    ot_ncp_uart_signal();
}

static int NcpSend(const uint8_t* aBuf, uint16_t aBufLength) {
    // log_info_hexdump("send", ncp_uart_dam_tx_cfg.dma_buf,
    //                  ncp_uart_dam_tx_cfg.dma_buf_size);
    hosal_uart_send(&ncp_uart_dev, aBuf, aBufLength);
    otNcpHdlcSendDone();
    return aBufLength;
}

void otAppNcpInit(otInstance* aInstance) { otNcpHdlcInit(aInstance, NcpSend); }

static void ot_ncp_task_loop(void* parameters_ptr) {
    static uint8_t uart_packet[OT_NCP_UART_DATA_CACHE_SIZE];
    uint32_t data_len = 0;

    otInstance* instance = otrGetInstance();
    if (instance) {
        otAppNcpInit(instance);
    }

    for (;;) {
        memset(uart_packet, 0x00, sizeof(uart_packet));
        data_len = ncp_uart_rx_read(uart_packet, OT_NCP_UART_DATA_CACHE_SIZE);
        // log_info_hexdump("read", uart_packet, data_len);
        // log_info("read");
        otNcpHdlcReceive(uart_packet, data_len);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void ncpInitUser() {
    BaseType_t xReturned;
#if (CONFIG_APP_OT_NCP_OPERATION_UART_PORT == 0)
    uart_stdio_deinit();
#endif
    if (app_ncp_rx_cb_time == NULL) {
        app_ncp_rx_cb_time = xTimerCreate(
            "app_ncp_rx_cb_time", 5, pdFALSE, NULL,
            (TimerCallbackFunction_t)app_ncp_uart_rx_cb_timeout_callback);
    }

    xReturned = xTaskCreate(ot_ncp_task_loop, "ncp_uart", 512, NULL,
                            (configMAX_PRIORITIES - 4), &ot_ncp_utask_handle);
    if (xReturned != pdPASS) {
        log_error("task create fail\n");
        return;
    }

    NVIC_SetPriority(OT_NCP_UART_IRQn, 5);
    /*Init UART In the first place*/
    hosal_uart_init(&ncp_uart_dev);

    /* clear UART dam interrupt status */
    hosal_uart_ioctl(&ncp_uart_dev, HOSAL_UART_CLEAR_DMA_STATUS,
                     (void*)(HOSAL_UART_DMA_TX_STATUS));

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&ncp_uart_dev, HOSAL_UART_RX_CALLBACK,
                            ncp_uart_rx_callback, &ncp_uart_dev);

    log_info("ncpInitUser \r\n");
    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&ncp_uart_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);
}
