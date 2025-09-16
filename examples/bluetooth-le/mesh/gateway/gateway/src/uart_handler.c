/**
 * @file uart_handler.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-03-24
 *
 * @copyright Copyright (c) 2022
 *
 */



//=============================================================================
//                Include
//=============================================================================
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart_handler.h"
#include "hosal_uart.h"
#include "mcu.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define UART_HANDLER_RX_CACHE_SIZE          16
#define MAX_UART_BUFFER_SIZE                256
//=============================================================================
//                Private ENUM
//=============================================================================

//=============================================================================
//                Private Struct
//=============================================================================
typedef struct
{
    uint8_t *pdata;
    uint32_t len;
} uart_msg_t;

typedef struct
{
    uint8_t len;
    uint8_t data[8];
} uart_rx_msg_t;
//=============================================================================
//                Private Function Declaration
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================
static uart_handler_parm_t uart_parm;
static uint8_t uart_buf[MAX_UART_BUFFER_SIZE] = { 0 };
static QueueHandle_t uart_msg_q;
static QueueHandle_t uart_rx_q;
//=============================================================================
//                Public Global Variables
//=============================================================================

//=============================================================================
//                Private Definition of Compare/Operation/Inline function/
//=============================================================================

HOSAL_UART_DEV_DECL(app_uartstdio, 1, 28, 29, UART_BAUDRATE_115200)
//=============================================================================
//                Functions
//=============================================================================
static void _uart_handler_send(void)
{
    uart_msg_t uart_msg;

    if (xQueueReceive(uart_msg_q, &uart_msg, 0) == pdTRUE)
    {

        hosal_uart_send(&app_uartstdio, uart_msg.pdata, uart_msg.len);
        if (uart_msg.pdata)
        {
            vPortFree(uart_msg.pdata);
        }
    }
}
static void _uart_handler_recv(void)
{
    /*  */
    static uint16_t total_len = 0;
    static uint16_t offset = 0;
    uint8_t rx_buf[UART_HANDLER_RX_CACHE_SIZE] = { 0 };
    int len;
    uart_rx_msg_t uart_rx;
    uint16_t msgbufflen = 0;
    uint32_t parser_status = 0;
    int i = 0;
    /*  */
    do
    {
        if (total_len > MAX_UART_BUFFER_SIZE)
        {
            total_len = 0;
        }
        //len = hosal_uart_receive(&app_uartstdio, rx_buf, sizeof(rx_buf)); 
        if (xQueueReceive(uart_rx_q, &uart_rx, 2) == pdTRUE)
        {

            len = uart_rx.len;
            //uart_buf[total_len] = uart_rx.data;
            memcpy(uart_buf + total_len, uart_rx.data, len);
            total_len += len;
            for (i = 0; i < UART_HANDLER_PARSER_CB_NUM; i++)
            {
                if (uart_parm.UartParserCB[i] == NULL)
                {
                    continue;
                }
                parser_status = uart_parm.UartParserCB[i](uart_buf, total_len, &msgbufflen, &offset);
                if ((parser_status == UART_DATA_VALID) || (parser_status == UART_DATA_VALID_CRC_OK))
                {
                    if (uart_parm.UartRecvCB[i] == NULL)
                    {
                        break;
                    }
                    mesh_tlv_t *pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + msgbufflen);

                    if (!pt_tlv)
                    {
                        break;
                    }
                    memcpy(pt_tlv->value, uart_buf + offset, msgbufflen);
                    pt_tlv->length = msgbufflen;
                    uart_parm.UartRecvCB[i](pt_tlv);
                    if (parser_status == UART_DATA_VALID_CRC_OK)
                    {
                        total_len -= (msgbufflen + offset) ;
                    }
                    else
                    {
                        total_len -= msgbufflen;
                    }

                    if (total_len > 0)
                    {
                        uint8_t *remainingdata = pvPortMalloc(total_len);
                        if (!remainingdata)
                        {
                            break;
                        }
                        memcpy(remainingdata, uart_buf + offset, total_len);
                        memcpy(uart_buf, remainingdata, total_len);

                        if (remainingdata)
                        {
                            vPortFree(remainingdata);
                        }
                        break;
                    }
                    break;
                }
                else if (parser_status == UART_DATA_CS_ERROR)
                {
                    total_len = 0;
                    //msgbufflen = 0;
                }
                else
                {
                    //offset = 0;
                    msgbufflen = 0;
                }
            }
        }
        
    } while (0);
}

static void _uart_handler_task(void *arg)
{
    for (;;)
    {
        _uart_handler_send();
        _uart_handler_recv();
        //vTaskDelay(2);
    }
}

static int uart1_rx_callback(void *p_arg)
{
    BaseType_t context_switch;
    char ch;
    uart_rx_msg_t uart_rx;

    uart_rx.len = hosal_uart_receive(&app_uartstdio, uart_rx.data, 8);
    xQueueSendToBackFromISR(uart_rx_q, &uart_rx, &context_switch);

    return 0;
}

void uart_handler_init(uart_handler_parm_t *param, uint8_t priority)
{
    TaskHandle_t t_thread;

    hosal_uart_init(&app_uartstdio);
    hosal_uart_callback_set(&app_uartstdio, HOSAL_UART_RX_CALLBACK, uart1_rx_callback, &app_uartstdio);
    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&app_uartstdio, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);
    uart_msg_q = xQueueCreate(20, sizeof(uart_msg_t));

    uart_rx_q = xQueueCreate(10, sizeof(uart_rx_msg_t));
    xTaskCreate(_uart_handler_task, "uart", 256, NULL, priority, &t_thread);

    memcpy(&uart_parm, param, sizeof(uart_handler_parm_t));
}

void uart_handler_send(uint8_t *pdata, uint32_t len)
{
    uart_msg_t uart_msg;

    uart_msg.pdata = pdata;
    uart_msg.len = len;

    xQueueSendToBack(uart_msg_q, &uart_msg, 5);
}

