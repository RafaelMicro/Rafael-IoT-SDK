/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet_uart_bypass.h
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
#ifndef _RFET_UART_BYPASS_H_
#define _RFET_UART_BYPASS_H_

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "rf_common_init.h"
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
typedef uint8_t RFET_UART_BYPASS_STS;
#define RFET_UART_BYPASS_STS_SUCCESS                ((RFET_UART_BYPASS_STS)(0x00))
#define RFET_UART_BYPASS_STS_FAIL                   ((RFET_UART_BYPASS_STS)(0x01))
#define RFET_UART_BYPASS_STS_SLEEP                  ((RFET_UART_BYPASS_STS)(0x10))

typedef uint8_t RFET_UART_BYPASS_CTRL;
#define RFET_UART_BYPASS_CTRL_NONE                  ((RFET_UART_BYPASS_CTRL)(0x00))
#define RFET_UART_BYPASS_CTRL_READ_CMDQ             ((RFET_UART_BYPASS_CTRL)(0x01))
#define RFET_UART_BYPASS_CTRL_READ_DATAQ            ((RFET_UART_BYPASS_CTRL)(0x02))
#define RFET_UART_BYPASS_CTRL_READ_MCU_STATE        ((RFET_UART_BYPASS_CTRL)(0x04))
#define RFET_UART_BYPASS_CTRL_ACCESS_HOST_CMD       ((RFET_UART_BYPASS_CTRL)(0x80)) /* Open or NOT, TBD */

typedef uint8_t RFET_UART_BYPASS_TYPE;
#define RFET_UART_BYPASS_TYPE_HCI_COMMAND           ((RFET_UART_BYPASS_TYPE)(0x01))
#define RFET_UART_BYPASS_TYPE_HCI_ACL_DATA          ((RFET_UART_BYPASS_TYPE)(0x02))
#define RFET_UART_BYPASS_TYPE_PCI_COMMON_COMMAND    ((RFET_UART_BYPASS_TYPE)(0x10))
#define RFET_UART_BYPASS_TYPE_PCI_FSK_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x11))
#define RFET_UART_BYPASS_TYPE_PCI_BLE_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x12))
#define RFET_UART_BYPASS_TYPE_PCI_15P4MAC_COMMAND   ((RFET_UART_BYPASS_TYPE)(0x13))
#define RFET_UART_BYPASS_TYPE_PCI_OQPSK_COMMAND     ((RFET_UART_BYPASS_TYPE)(0x14))
#define RFET_UART_BYPASS_TYPE_PCI_DATA              ((RFET_UART_BYPASS_TYPE)(0x17))

#define RFET_UART_BYPASS_TYPE_APCI_RF_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x20))
#define RFET_UART_BYPASS_TYPE_APCI_TX_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x21))
#define RFET_UART_BYPASS_TYPE_APCI_RX_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x22))
#define RFET_UART_BYPASS_TYPE_HOST_CMD              ((RFET_UART_BYPASS_TYPE)(0xF0))
#define RFET_UART_BYPASS_TYPE_HOST_EVENT            ((RFET_UART_BYPASS_TYPE)(0xF1))

#define RFET_UART_BYPASS_TYPE_CMM_SYS_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x30))
#define RFET_UART_BYPASS_TYPE_CMM_HAL_COMMAND       ((RFET_UART_BYPASS_TYPE)(0x31))
#define RFET_UART_BYPASS_TYPE_HCI_COMMAND_HDR_SIZE  (4) /* tranport(1B) + opcode(2B) + length(1B) */
#define RFET_UART_BYPASS_TYPE_HCI_ACL_DATA_HDR_SIZE (5) /* tranport(1B) + handle2B) + length(2B) */

#define RFET_UART_BYPASS_TYPE_PCI_COMMAND_HDR_SIZE  (3) /* RUCI Header(1B) + Sub-Header (1B) + length(1B)  */
#define RFET_UART_BYPASS_TYPE_PCI_DATA_HDR_SIZE     (4) /* RUCI Header(1B) + Sub-Header (1B) + length(2B)  */
#define RFET_UART_BYPASS_TYPE_APCI_COMMAND_HDR_SIZE (3) /* RUCI Header(1B) + Sub-Header (1B) + length(1B)  */
#define RFET_UART_BYPASS_TYPE_HOST_COMMAND_HDR_SIZE (4) /* RUCI Header(1B) + Sub-Header (1B) + length(2B) */


#define RFET_UART_BYPASS_SF_CMD_FAIL                (0)
#define RFET_UART_BYPASS_SF_CMD_SUCCESS             (1)
#define RFET_UART_BYPASS_SF_SIZE_HEADER             (sizeof(ruci_head_t))
#define RFET_UART_BYPASS_SF_SIZE_SUBHEADER          (1)
#define RFET_UART_BYPASS_SF_SIZE_RET_VALUE          (1)
#define RFET_UART_BYPASS_SF_SIZE_CMD_OUT             (RFET_UART_BYPASS_SF_SIZE_HEADER + RFET_UART_BYPASS_SF_SIZE_SUBHEADER + RFET_UART_BYPASS_SF_SIZE_RET_VALUE)

/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/

/**************************************************************************************************
 *    Global Prototypes
 *************************************************************************************************/
bool rfet_uart_bypass_init(RF_FW_LOAD_SELECT fw_select);
void rfet_uart_bypass_entry(uint8_t *rx_data_ptr, uint16_t rx_len);
bool rfet_uart_bypass_is_busy(void);
void rfet_uart_bypass_task(void);


#endif  //_RFET_UART_BYPASS_H_

