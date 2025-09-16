/**
 * @file uart_handler.c
 * @brief
 * @version 0.1
 * @date 2023-08-28
 * 
 */

//=============================================================================
//                Include
//=============================================================================
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_uart.h"
#include "log.h"
#include "queue.h"
#include "task.h"
#include "uart_handler.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================
#ifndef CONFIG_APP_GW_OPERATION_UART_PORT
#define CONFIG_APP_GW_OPERATION_UART_PORT 1
#endif // !CONFIG_APP_GW_OPERATION_UART_PORT

#ifndef CONFIG_APP_GW_OPERATION_UART_TX_PIN
#define CONFIG_APP_GW_OPERATION_UART_TX_PIN 28
#endif // !CONFIG_APP_GW_OPERATION_UART_TX_PIN

#ifndef CONFIG_APP_GW_OPERATION_UART_RX_PIN
#define CONFIG_APP_GW_OPERATION_UART_RX_PIN 29
#endif // !CONFIG_APP_GW_OPERATION_UART_RX_PIN

#ifndef CONFIG_APP_GW_OPERATION_UART_BAUDRATE
#define CONFIG_APP_GW_OPERATION_UART_BAUDRATE 115200
#endif // !CONFIG_APP_GW_OPERATION_UART_BAUDRATE

#if (CONFIG_APP_GW_OPERATION_UART_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(app_uart_dev, CONFIG_APP_GW_OPERATION_UART_PORT,
                    CONFIG_APP_GW_OPERATION_UART_TX_PIN,
                    CONFIG_APP_GW_OPERATION_UART_RX_PIN, UART_BAUDRATE_115200)

#elif (CONFIG_APP_GW_OPERATION_UART_BAUDRATE == 38400)
HOSAL_UART_DEV_DECL(app_uart_dev, CONFIG_APP_GW_OPERATION_UART_PORT,
                    CONFIG_APP_GW_OPERATION_UART_TX_PIN,
                    CONFIG_APP_GW_OPERATION_UART_RX_PIN, UART_BAUDRATE_38400)

#elif (CONFIG_APP_GW_OPERATION_UART_BAUDRATE == 57600)
HOSAL_UART_DEV_DECL(app_uart_dev, CONFIG_APP_GW_OPERATION_UART_PORT,
                    CONFIG_APP_GW_OPERATION_UART_TX_PIN,
                    CONFIG_APP_GW_OPERATION_UART_RX_PIN, UART_BAUDRATE_57600)
#elif (CONFIG_APP_GW_OPERATION_UART_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(app_uart_dev, CONFIG_APP_GW_OPERATION_UART_PORT,
                    CONFIG_APP_GW_OPERATION_UART_TX_PIN,
                    CONFIG_APP_GW_OPERATION_UART_RX_PIN, UART_BAUDRATE_500000)
#elif (CONFIG_APP_GW_OPERATION_UART_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(app_uart_dev, CONFIG_APP_GW_OPERATION_UART_PORT,
                    CONFIG_APP_GW_OPERATION_UART_TX_PIN,
                    CONFIG_APP_GW_OPERATION_UART_RX_PIN, UART_BAUDRATE_1000000)
#elif (CONFIG_APP_GW_OPERATION_UART_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(app_uart_dev, CONFIG_APP_GW_OPERATION_UART_PORT,
                    CONFIG_APP_GW_OPERATION_UART_TX_PIN,
                    CONFIG_APP_GW_OPERATION_UART_RX_PIN, UART_BAUDRATE_2000000)
#endif // CONFIG_APP_GW_OPERATION_UART_BAUDRATE

#define UART_HANDLER_RX_CACHE_SIZE 256
#define MAX_UART_BUFFER_SIZE       512

//=============================================================================
//                Private ENUM
//=============================================================================

//=============================================================================
//                Private Struct
//=============================================================================
typedef struct __attribute__((packed)) uart_io {
    uint16_t start;
    uint16_t end;

    uint32_t recvLen;
    uint8_t uart_cache[UART_HANDLER_RX_CACHE_SIZE];
} uart_io_t;

//=============================================================================
//                Private Global Variables
//=============================================================================
static TaskHandle_t uart_taskHandle = NULL;
static uart_handler_param_t uart_handler_param;
static uart_io_t g_uart_rx_io = {
    .start = 0,
    .end = 0,
};
static uint8_t g_uart_buffer[MAX_UART_BUFFER_SIZE];

//=============================================================================
//                Functions
//=============================================================================

static int uart_handler_rx_cb(void* p_arg) {
    uint32_t len = 0;
    if (g_uart_rx_io.start >= g_uart_rx_io.end) {
        g_uart_rx_io.start += hosal_uart_receive(
            p_arg, g_uart_rx_io.uart_cache + g_uart_rx_io.start,
            UART_HANDLER_RX_CACHE_SIZE - g_uart_rx_io.start - 1);
        if (g_uart_rx_io.start == (UART_HANDLER_RX_CACHE_SIZE - 1)) {
            g_uart_rx_io.start = hosal_uart_receive(
                p_arg, g_uart_rx_io.uart_cache,
                (UART_HANDLER_RX_CACHE_SIZE + g_uart_rx_io.end - 1)
                    % UART_HANDLER_RX_CACHE_SIZE);
        }
    } else if (((g_uart_rx_io.start + 1) % UART_HANDLER_RX_CACHE_SIZE)
               != g_uart_rx_io.end) {
        g_uart_rx_io.start += hosal_uart_receive(
            p_arg, g_uart_rx_io.uart_cache,
            g_uart_rx_io.end - g_uart_rx_io.start - 1);
    }

    if (g_uart_rx_io.start != g_uart_rx_io.end) {

        len = (g_uart_rx_io.start + UART_HANDLER_RX_CACHE_SIZE
               - g_uart_rx_io.end)
              % UART_HANDLER_RX_CACHE_SIZE;
        if (g_uart_rx_io.recvLen != len) {
            g_uart_rx_io.recvLen = len;
        }
    }
    return 0;
}

static int uart_handler_data_read(uint8_t* p_data) {
    uint32_t byte_cnt = 0;
    enter_critical_section();

    if (g_uart_rx_io.start != g_uart_rx_io.end) {
        if (g_uart_rx_io.start > g_uart_rx_io.end) {
            memcpy(p_data, g_uart_rx_io.uart_cache + g_uart_rx_io.end,
                   g_uart_rx_io.start - g_uart_rx_io.end);
            g_uart_rx_io.end = g_uart_rx_io.start;
        } else {
            memcpy(p_data, g_uart_rx_io.uart_cache + g_uart_rx_io.end,
                   UART_HANDLER_RX_CACHE_SIZE - g_uart_rx_io.end);
            g_uart_rx_io.end = UART_HANDLER_RX_CACHE_SIZE - 1;
            if (g_uart_rx_io.start) {
                memcpy(p_data, g_uart_rx_io.uart_cache, g_uart_rx_io.start);
                g_uart_rx_io.end = (UART_HANDLER_RX_CACHE_SIZE
                                    + g_uart_rx_io.start - 1)
                                   % UART_HANDLER_RX_CACHE_SIZE;
            }
        }
    }

    byte_cnt = g_uart_rx_io.recvLen;

    g_uart_rx_io.start = g_uart_rx_io.end = 0;
    g_uart_rx_io.recvLen = 0;
    leave_critical_section();
    return byte_cnt;
}

static void uart_handler_task(void* pvParameters) {
    static uint16_t offset = 0, total_len = 0;
    uart_handler_parser_status_t status;

    uint8_t temp_buffer[UART_HANDLER_RX_CACHE_SIZE];
    uint16_t uart_len = 0;
    uint16_t data_len = 0;
    uint8_t i = 0;

    while (1) {
        uart_len = uart_handler_data_read(temp_buffer);
        if (uart_len == 0) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }
        // log_info_hexdump("UART", temp_buffer, uart_len);
        memcpy(g_uart_buffer + total_len, temp_buffer, uart_len);
        total_len += uart_len;

        for (i = 0; i < UART_HANDLER_PARSER_CB_NUM; i++) {
            if (uart_handler_param.parser_cb[i] == NULL) {
                continue;
            }

            status = uart_handler_param.parser_cb[i](g_uart_buffer, total_len,
                                                     &data_len, &offset);
            if (status == UART_HANDLER_PARSER_VALID
                || status == UART_HANDLER_PARSER_VALID_CRC_OK) {
                if (uart_handler_param.rx_cb[i] == NULL) {
                    continue;
                }

                uart_handler_param.rx_cb[i](g_uart_buffer + offset, data_len);

                if (status == UART_HANDLER_PARSER_VALID_CRC_OK) {
                    total_len -= (data_len + offset);
                } else {
                    total_len -= data_len;
                }
                if (total_len > 0) {
                    memmove(g_uart_buffer, g_uart_buffer + data_len + offset,
                            total_len);
                }
                break;
            } else if (status == UART_HANDLER_PARSER_INVALID_CRC_ERR) {
                total_len = 0;
                break;
            }
        }
    }
}

int uart_handler_data_send(uint8_t* pdata, uint16_t len) {
    return hosal_uart_send(&app_uart_dev, pdata, len);
}

void uart_handler_init(uart_handler_param_t* param) {
    memcpy(&uart_handler_param, param, sizeof(uart_handler_param_t));
    hosal_uart_init(&app_uart_dev);

    log_info("GW UART Port %d (%d,%d), Baudrate %d",
             CONFIG_APP_GW_OPERATION_UART_PORT,
             CONFIG_APP_GW_OPERATION_UART_RX_PIN,
             CONFIG_APP_GW_OPERATION_UART_TX_PIN,
             CONFIG_APP_GW_OPERATION_UART_BAUDRATE);

    if (xTaskCreate(uart_handler_task, "uart_handler_task", 1024, NULL,
                    E_TASK_PRIORITY_HIGHEST, &uart_taskHandle)
        != pdPASS) {
        log_error("uart_handler_task create failed");
    }

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&app_uart_dev, HOSAL_UART_RX_CALLBACK,
                            uart_handler_rx_cb, &app_uart_dev);

    hosal_uart_ioctl(&app_uart_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);

    __NVIC_SetPriority(Uart1_IRQn, 4);
}