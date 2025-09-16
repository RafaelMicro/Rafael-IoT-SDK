/**
 * @file uart_handler.h
 * @brief
 * @version 0.1
 * @date 2023-08-28
 * 
 */

#ifndef __UART_HANDLER_H__
#define __UART_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include
//=============================================================================
#include "stdint.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define UART_HANDLER_PARSER_CB_NUM 3

//=============================================================================
//                Private ENUM
//=============================================================================
typedef enum {
    UART_HANDLER_PARSER_VALID = 0,
    UART_HANDLER_PARSER_VALID_CRC_OK,
    UART_HANDLER_PARSER_INVALID,
    UART_HANDLER_PARSER_INVALID_CRC_ERR,
} uart_handler_parser_status_t;

typedef uart_handler_parser_status_t (*uart_handler_parser_cb_t)(
    uint8_t* pdata, uint16_t len, uint16_t* data_len, uint16_t* offset);

typedef void (*uart_rx_cb)(uint8_t* pdata, uint16_t len);

//=============================================================================
//                Private Struct
//=============================================================================
typedef struct UART_HANDLER_PARAM_T {
    uart_handler_parser_cb_t parser_cb[UART_HANDLER_PARSER_CB_NUM];
    uart_rx_cb rx_cb[UART_HANDLER_PARSER_CB_NUM];
} uart_handler_param_t;

//=============================================================================
//                Functions
//=============================================================================
void uart_handler_init(uart_handler_param_t* param);
int uart_handler_data_send(uint8_t* pdata, uint16_t len);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __UART_HANDLER_H__
