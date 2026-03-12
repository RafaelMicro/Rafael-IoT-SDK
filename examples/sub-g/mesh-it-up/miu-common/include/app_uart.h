#ifndef APP_UART_H
#define APP_UART_H

typedef enum {
    UART_DATA_VALID = 0,
    UART_DATA_VALID_CRC_OK,
    UART_DATA_INVALID,
    UART_DATA_CS_ERROR,
} uart_handler_data_sts_t;

void app_uart_init();

#endif /* APP_UART_H */
