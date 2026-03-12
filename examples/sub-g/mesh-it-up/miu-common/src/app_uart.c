#include "app_uart.h"
#include <string.h>
#include "FreeRTOS.h"
#include "cli.h"
#include "hosal_uart.h"
#include "log.h"
#include "main.h"
#include "ota_download_cmd_handler.h"
#include "task.h"
#include "uart_stdio.h"
#include "util_queue.h"

#define UART_HANDLER_RX_CACHE_SIZE 128
#define RX_BUFF_SIZE               484
#define MAX_UART_BUFFER_SIZE       384

typedef struct uart_io {
    uint16_t start;
    uint16_t end;

    uint32_t recvLen;
    uint8_t uart_cache[RX_BUFF_SIZE];
} uart_io_t;

static uart_io_t g_uart1_rx_io = {.start = 0, .end = 0, .recvLen = 0};
HOSAL_UART_DEV_DECL(uart1_dev, 1, 28, 29, UART_BAUDRATE_Baud115200)

static uint8_t uart_buf[MAX_UART_BUFFER_SIZE] = {0};
static uint8_t g_tmp_buff[RX_BUFF_SIZE];

static TaskHandle_t app_task_handle = NULL;

/*uart 1 use*/
static int __uart1_read(uint8_t* p_data, uint32_t p_data_len) {
    if (p_data == NULL || p_data_len == 0) {
        return -1; // Prevent invalid reads
    }

    uint32_t available_data =
        g_uart1_rx_io.recvLen; // Readable data in the ring buffer
    if (available_data == 0) {
        return 0; // No readable data
    }

    uint32_t read_len = (p_data_len > available_data) ? available_data
                                                      : p_data_len;

    // Ring buffer reads data
    if (g_uart1_rx_io.end + read_len < RX_BUFF_SIZE) {
        // Read directly from end
        memcpy(p_data, g_uart1_rx_io.uart_cache + g_uart1_rx_io.end, read_len);
        g_uart1_rx_io.end += read_len;
    } else {
        // Read the tail part first
        uint32_t tail_len = RX_BUFF_SIZE - g_uart1_rx_io.end;
        memcpy(p_data, g_uart1_rx_io.uart_cache + g_uart1_rx_io.end, tail_len);

        // Then read the rest from the beginning
        uint32_t head_len = read_len - tail_len;
        memcpy(p_data + tail_len, g_uart1_rx_io.uart_cache, head_len);

        g_uart1_rx_io.end = head_len; // Update `end` position
    }

    // Update `recvLen`
    g_uart1_rx_io.recvLen -= read_len;

    return read_len;
}

static int __uart1_rx_callback(void* p_arg) {
    uint32_t new_data_len = 0;

    // Ring buffer writes (make sure not to exceed RX_BUFF_SIZE)
    if (g_uart1_rx_io.start >= g_uart1_rx_io.end) {
        new_data_len = hosal_uart_receive(
            p_arg, g_uart1_rx_io.uart_cache + g_uart1_rx_io.start,
            RX_BUFF_SIZE - g_uart1_rx_io.start);
        g_uart1_rx_io.start = (g_uart1_rx_io.start + new_data_len)
                              % RX_BUFF_SIZE;
    } else if (((g_uart1_rx_io.start + 1) % RX_BUFF_SIZE)
               != g_uart1_rx_io.end) {
        new_data_len = hosal_uart_receive(
            p_arg, g_uart1_rx_io.uart_cache + g_uart1_rx_io.start,
            g_uart1_rx_io.end - g_uart1_rx_io.start - 1);
        g_uart1_rx_io.start = (g_uart1_rx_io.start + new_data_len)
                              % RX_BUFF_SIZE;
    }

    // Calculate the length of readable data in the ring buffer
    uint32_t len = 0;
    if (g_uart1_rx_io.start >= g_uart1_rx_io.end) {
        len = g_uart1_rx_io.start - g_uart1_rx_io.end;
    } else {
        len = RX_BUFF_SIZE - g_uart1_rx_io.end + g_uart1_rx_io.start;
    }

    if (g_uart1_rx_io.recvLen != len) {
        g_uart1_rx_io.recvLen = len;
    }

    return 0;
}

static int __uart1_break_callback(void* p_arg) {
    /*can't printf*/
    return 0;
}

void app_uart1_recv() {
    static uint8_t tmp_buf[UART_HANDLER_RX_CACHE_SIZE] = {0};
    int len = 0;

    len = __uart1_read(tmp_buf, UART_HANDLER_RX_CACHE_SIZE);
    if (len > 0) {
        log_info_hexdump("uar1 rx", tmp_buf, len);
    }
}

void app_uart1_data_recv(uint8_t* data, uint16_t lens) {
    ota_download_cmd_proc(data, lens);
    return;
}

void app_uart0_hex_recv() {
    static uint16_t total_len = 0;
    static uint16_t offset = 0;
    static uint8_t rx_buf[UART_HANDLER_RX_CACHE_SIZE] = {0};
    int len = 0;

    uint16_t msgbufflen = 0;
    uint32_t parser_status = 0;
    int i = 0;

    do {
        if (total_len >= MAX_UART_BUFFER_SIZE) {
            total_len = 0;
        }
        len = uart_stdio_read(rx_buf, UART_HANDLER_RX_CACHE_SIZE);
        if (len > 0) {
            log_info("Adding to total_len: current %d, read %d ", total_len,
                     len);

            uint32_t space_left = MAX_UART_BUFFER_SIZE - total_len;
            uint32_t data_to_copy = (len > space_left) ? space_left : len;

            memcpy(uart_buf + total_len, rx_buf, data_to_copy);
            total_len += data_to_copy;
#if CONFIG_APP_TASK_OTA_ENABLE
            parser_status = ota_download_cmd_parser(uart_buf, total_len,
                                                    &msgbufflen, &offset);
            if (parser_status == UART_DATA_VALID) {
                ota_download_cmd_proc((uart_buf + offset), msgbufflen);
            }
#endif
            if (msgbufflen > 0) {
                if (total_len > msgbufflen) {
                    total_len -= msgbufflen;
                    if (total_len > offset) {
                        memcpy((uart_buf + offset),
                               (uart_buf + msgbufflen + offset),
                               total_len - offset);
                    }
                } else {
                    total_len = 0;
                }
            }

            offset = 0;
            msgbufflen = 0;
        }

    } while (0);
}

static void app_uart_task(void* pvParameters) {
    uint8_t cnt = 0;
    while (1) {
        if (cli_mode_get_function() == UART0_MODE_HEX_RX) {
            app_uart0_hex_recv();
        }
        app_uart1_recv();
        vTaskDelay(10);
    }
}

int app_uart_data_send(uint8_t u_port, uint8_t* p_data, uint16_t data_len) {
    hosal_uart_send(&uart1_dev, p_data, data_len);
    return 0;
}

void app_uart_init() {
    /*Init UART In the first place*/
    hosal_uart_init(&uart1_dev);
    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uart1_dev, HOSAL_UART_RX_CALLBACK,
                            __uart1_rx_callback, &uart1_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart1_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);

    __NVIC_SetPriority(Uart1_IRQn, 2);

    BaseType_t xReturned;
    xReturned = xTaskCreate(app_uart_task, "app_uart", 512, NULL,
                            (configMAX_PRIORITIES - 4), &app_task_handle);
    if (xReturned != pdPASS) {
        log_error("task create fail");
    }
}
