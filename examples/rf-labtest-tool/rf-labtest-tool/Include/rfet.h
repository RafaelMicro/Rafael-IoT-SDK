/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet.h
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
#ifndef _RFET_H_
#define _RFET_H_

#include "rfet_api.h"

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/


/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/
typedef uint8_t RFET_CMD_HEADER;
typedef uint8_t RFET_UART_RX_DISPATCH;
typedef uint8_t RFET_UART_RX_CTRL;
typedef uint8_t RFET_MODE_SWITCH;
typedef uint8_t RFET_FW_SWITCH;

typedef struct _rfet_command_desc_
{
    RFET_CMD_HEADER cmd_header;
    uint16_t cmd_length;
    RFET_HDLR cmd_hdlr;
} s_rfet_cmd_t, *p_rfet_cmd_t;

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define UART_0 (0)
#define UART_1 (1)
#define CHOOSE_RFET_UART (UART_0)

#if (defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583))
#define GPIO_TEST           (2)
#else
#define GPIO_TEST           (1)
#endif

#define CLI_EN              (0)

#define RFET_UART_BUFFER_MAX_SIZE               (2200)//(2048)
#define RFET_UART_EVENT_BUFFER_MAX_SIZE         (256)

#define RFET_UART_CH_CHECK_THRESHOLD            (0x80)

#define RFET_UART_RX_CTRL_NONE                  ((RFET_UART_RX_CTRL)0x00)
#define RFET_UART_RX_CTRL_WAIT_DATA             ((RFET_UART_RX_CTRL)0x01)
#define RFET_UART_RX_CTRL_CMD_EXECUTE           ((RFET_UART_RX_CTRL)0x02)

#define RFET_CMD_MODE_SWITCH_GUI                ((RFET_CMD_HEADER)(0x81))
#define RFET_CMD_MODE_SWITCH_UART_BRIDGE        ((RFET_CMD_HEADER)(0x82))
#define RFET_CMD_MODE_SWITCH_HOST               ((RFET_CMD_HEADER)(0x83))
#define RFET_CMD_GUI_CONTROL_DTM_CTRL           ((RFET_CMD_HEADER)(0x91))
#define RFET_CMD_GUI_CONTROL_RF_FW_SWITCH       ((RFET_CMD_HEADER)(0x92))
#define RFET_CMD_GUI_CONTROL_UART_BYPASS        ((RFET_CMD_HEADER)(0x93))
#define RFET_CMD_GUI_CONTROL_RUCI_CMD           ((RFET_CMD_HEADER)(0x93))
#define RFET_CMD_GUI_CONTROL_GET_CHIP_INFO      ((RFET_CMD_HEADER)(0x94))
#define RFET_CMD_GUI_CUSTOMER_CMD               ((RFET_CMD_HEADER)(0x9C))
#define RFET_CMD_HOST_CONTROL_CMD               ((RFET_CMD_HEADER)(0xA0))
#define RFET_CMD_DTM_TEST_END_CMD               ((RFET_CMD_HEADER)(0xC1))

#define RFET_EVENT                              ((RFET_CMD_HEADER)(0xE0))
#define RFET_CMD_HDLR_END                       ((RFET_CMD_HEADER)(0xFF))

#define RFET_UART_RX_DISPATCH_CLI_CH            ((RFET_UART_RX_DISPATCH)(0x01))
#define RFET_UART_RX_DISPATCH_CMD_MODE          ((RFET_UART_RX_DISPATCH)(0x02))

#define RFET_CMD_COMMON_HEADER_SIZE                     (1)     /* Header(1B) */
#define RFET_CMD_COMMON_HEADER_LEN_SIZE                 (2)     /* Length(2B) */
#define RFET_CMD_COMMON_CRC_LEN_SIZE                    (1)     /* 1B */
#define RFET_CMD_COMMON_OVERHEAD_SIZE                   (RFET_CMD_COMMON_HEADER_SIZE+RFET_CMD_COMMON_HEADER_LEN_SIZE)
#define RFET_CMD_LEN_MODE_SWITCH                        (2)
#define RFET_CMD_LEN_GUI_CONTROL_DTM_CTRL               (2)
#define RFET_CMD_LEN_GUI_CONTROL_RF_FW_SWITCH           (2)
#define RFET_CMD_LEN_GUI_CONTROL_GET_CHIP_INFO          (1)
#define RFET_CMD_LEN_GUI_CONTROL_EVENT_HEADER_SIZE      (4)     /* Header(1B) + Length(2B) + Parameter Byte 0(1B) */
#define RFET_CMD_LEN_GUI_CONTROL_EVENT_OVERHEAD         (2)     /* Parameter Byte 0 & CRC(1B) */
#define RFET_CMD_DTM_TEST_END_SIZE                      (2)

#define RFET_MODE_CONTROL_CLI                   ((RFET_MODE_SWITCH)(0x00))
#define RFET_MODE_CONTROL_GUI                   ((RFET_MODE_SWITCH)(0x01))
#define RFET_MODE_CONTROL_UART_BRIDGE           ((RFET_MODE_SWITCH)(0x02))
#define RFET_MODE_CONTROL_HOST                  ((RFET_MODE_SWITCH)(0x03))

#define RFET_GUI_DTM_MODE_ENABLE                ((RFET_FW_SWITCH)0x01)
#define RFET_GUI_DTM_MODE_DISABLE               ((RFET_FW_SWITCH)0x02)

#define RFET_FW_CONTROL_PARA_NONE               ((RFET_FW_SWITCH)0x00)
#define RFET_FW_CONTROL_RUCI_CMD                ((RFET_FW_SWITCH)0x01)
#define RFET_FW_CONTROL_BLE_CONTROLLER          ((RFET_FW_SWITCH)0x02)

/**************************************************************************************************
 *    Global Prototypes
 *************************************************************************************************/
extern uint8_t      g_uart_tx_buffer[RFET_UART_BUFFER_MAX_SIZE];
extern uint32_t     g_rfet_version;
extern RFET_HDLR    gRfet_CustomerHandle;
extern bool         g_gui_enable_gpio_sleep;
/* define for command handler */
void rfet_uart_tx_buf(uint16_t tx_len, bool crc_en);
void rfet_init(void);

#endif  //_RFET_H_

