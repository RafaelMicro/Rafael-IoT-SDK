/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet_uart_bypass.c
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
/**************************************************************************************************
*    INCLUDES
*************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "rf_mcu.h"
#include "rfet.h"
#include "rfet_uart_bypass.h"
#include "ruci.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
uint8_t* g_rx_buf_ptr;
uint8_t g_hci_acl_data_q_num = 0;
uint16_t g_data_len_to_spi = 0;
uint16_t g_data_len_from_spi = 0;

RFET_UART_BYPASS_CTRL g_bypass_ctrl = RFET_UART_BYPASS_CTRL_NONE;

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
RFET_UART_BYPASS_STS rfet_uart_bypass_send_spi_cmd_q(uint8_t* send_spi_buf,
                                                     uint16_t send_len) {
    RF_MCU_TX_CMDQ_ERROR tx_err = RF_MCU_TX_CMDQ_ERR_INIT;

    /* to check state before issue cmd send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03) {
        return RFET_UART_BYPASS_STS_SLEEP;
    }

    tx_err = RfMcu_CmdQueueSend(send_spi_buf, send_len);

    return (tx_err != RF_MCU_TX_CMDQ_SET_SUCCESS)
               ? RFET_UART_BYPASS_STS_FAIL
               : RFET_UART_BYPASS_STS_SUCCESS;
}

RFET_UART_BYPASS_STS rfet_uart_bypass_send_spi_data_q(uint8_t qId,
                                                      uint8_t* send_spi_buf,
                                                      uint16_t send_len) {
    RF_MCU_TXQ_ERROR tx_err = RF_MCU_TXQ_ERR_INIT;

    /* to check state before issue TX send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03) {
        return RFET_UART_BYPASS_STS_SLEEP;
    }

    tx_err = RfMcu_TxQueueSendById(qId, send_spi_buf, send_len);

    return (tx_err != RF_MCU_TXQ_SET_SUCCESS) ? RFET_UART_BYPASS_STS_FAIL
                                              : RFET_UART_BYPASS_STS_SUCCESS;
}

RFET_UART_BYPASS_STS rfet_uart_bypass_read_spi_cmd_q(uint16_t* tx_len) {
    uint16_t rx_len;
    RF_MCU_RX_CMDQ_ERROR rx_err = RF_MCU_RX_CMDQ_ERR_INIT;

    /* read cmd queue */
    rx_len = RfMcu_EvtQueueRead(g_uart_tx_buffer, &rx_err);

    if (rx_err == RF_MCU_RX_CMDQ_GET_SUCCESS) {
        *tx_len = rx_len;
    }

    return (rx_err != RF_MCU_RX_CMDQ_GET_SUCCESS)
               ? RFET_UART_BYPASS_STS_FAIL
               : RFET_UART_BYPASS_STS_SUCCESS;
}

RFET_UART_BYPASS_STS rfet_uart_bypass_read_spi_data_q(uint16_t* tx_len) {
    uint16_t rx_len;
    RF_MCU_RXQ_ERROR rx_err = RF_MCU_RXQ_ERR_INIT;

    /* get data from rx data Q*/
    rx_len = RfMcu_RxQueueRead(g_uart_tx_buffer, &rx_err);

    if (rx_err == RF_MCU_RXQ_GET_SUCCESS) {
        *tx_len = rx_len;
    }

    return (rx_err != RF_MCU_RXQ_GET_SUCCESS) ? RFET_UART_BYPASS_STS_FAIL
                                              : RFET_UART_BYPASS_STS_SUCCESS;
}

void rfet_uart_bypass_isr(uint8_t intStatus) {
    //uint8_t readSfr;

    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03) {
        /* FOR LEVEL TRIGGER ONLY, the MCU will keep entering INT if status not cleared */
        return;
    }

    // CMD --------------------------------------------------------------------
    if (intStatus & 0x01) {
        /* read cmd q */
        g_bypass_ctrl |= RFET_UART_BYPASS_CTRL_READ_CMDQ;

#if 0
        readSfr = RfMcu_SpiSfrGet(3);
        if (!(readSfr & 0x2))
        {
            //testIsr = intStatus;
            BREAK();
        }
#endif
    }

    // Status -----------------------------------------------------------------
    if (intStatus & 0x02) {
        g_bypass_ctrl |= RFET_UART_BYPASS_CTRL_READ_MCU_STATE;
        //gUasbMcuState = (uint8_t)RfMcu_McuStateRead();
    }

    // RX packet --------------------------------------------------------------
    if (intStatus & 0x20) {
        /* read RX Q */
        g_bypass_ctrl |= RFET_UART_BYPASS_CTRL_READ_DATAQ;

#if 0
        readSfr = RfMcu_SpiSfrGet(3);
        if (!(readSfr & 0x1))
        {
            //testIsr = intStatus;
            BREAK();
        }
#endif
    }

    RfMcu_InterruptClear(intStatus);
}

uint16_t rfet_uart_bypass_host_cmd_sfr_read(uint8_t* rx_data_ptr) {

    uint16_t cmdOutLen = RFET_UART_BYPASS_SF_SIZE_CMD_OUT;
    ruci_para_sfr_read_t *pCmdIn, *pCmdOut;

    pCmdIn = (ruci_para_sfr_read_t*)(rx_data_ptr);
    pCmdOut = (ruci_para_sfr_read_t*)(g_uart_tx_buffer);
    pCmdOut->ruci_header.u8 = pCmdIn->ruci_header.u8;
    pCmdOut->sub_header = pCmdIn->sub_header;

    if (pCmdIn->addr > 0xF) {
        *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_FAIL;
        return (cmdOutLen);
    }

    cmdOutLen += 1;
    *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_SUCCESS;
    *((&pCmdOut->sub_header) + 2) = RfMcu_SpiSfrGet(pCmdIn->addr);

    return (cmdOutLen);
}

uint16_t rfet_uart_bypass_host_cmd_sfr_write(uint8_t* rx_data_ptr) {
    uint16_t cmdOutLen = RFET_UART_BYPASS_SF_SIZE_CMD_OUT;
    ruci_para_sfr_write_t *pCmdIn, *pCmdOut;

    pCmdIn = (ruci_para_sfr_write_t*)(rx_data_ptr);
    pCmdOut = (ruci_para_sfr_write_t*)(g_uart_tx_buffer);
    pCmdOut->ruci_header.u8 = pCmdIn->ruci_header.u8;
    pCmdOut->sub_header = pCmdIn->sub_header;

    if (pCmdIn->addr > 0xF) {
        *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_FAIL;
        return (cmdOutLen);
    }

    *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_SUCCESS;
    RfMcu_SpiSfrSet(pCmdIn->addr, pCmdIn->data);

    return (cmdOutLen);
}

uint16_t rfet_uart_bypass_host_cmd_io_read(uint8_t* rx_data_ptr) {
    uint16_t cmdOutLen = RFET_UART_BYPASS_SF_SIZE_CMD_OUT;
    ruci_para_io_read_t *pCmdIn, *pCmdOut;

    pCmdIn = (ruci_para_io_read_t*)(rx_data_ptr);
    pCmdOut = (ruci_para_io_read_t*)(g_uart_tx_buffer);
    pCmdOut->ruci_header.u8 = pCmdIn->ruci_header.u8;
    pCmdOut->sub_header = pCmdIn->sub_header;

    if ((pCmdIn->data_length > 2117)
        || (pCmdIn->data_length > RFET_UART_BUFFER_MAX_SIZE)
        || (pCmdIn->q_idx > 1)) {
        *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_FAIL;
        return (cmdOutLen);
    }

    cmdOutLen += pCmdIn->data_length;
    *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_SUCCESS;
    RfMcu_IoGet(pCmdIn->q_idx, ((&pCmdOut->sub_header) + 2),
                pCmdIn->data_length);

    return (cmdOutLen);
}

uint16_t rfet_uart_bypass_host_cmd_io_write(uint8_t* rx_data_ptr) {
    uint16_t cmdOutLen = RFET_UART_BYPASS_SF_SIZE_CMD_OUT;
    ruci_para_io_write_t *pCmdIn, *pCmdOut;

    pCmdIn = (ruci_para_io_write_t*)(rx_data_ptr);
    pCmdOut = (ruci_para_io_write_t*)(g_uart_tx_buffer);
    pCmdOut->ruci_header.u8 = pCmdIn->ruci_header.u8;
    pCmdOut->sub_header = pCmdIn->sub_header;

    if ((pCmdIn->data_length > 2110)
        || (pCmdIn->data_length > RFET_UART_BUFFER_MAX_SIZE)
        || (pCmdIn->q_idx > 7)) {
        *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_FAIL;
        return (cmdOutLen);
    }

    *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_SUCCESS;
    RfMcu_IoSet(pCmdIn->q_idx, pCmdIn->data, pCmdIn->data_length);

    return (cmdOutLen);
}

uint16_t rfet_uart_bypass_host_cmd_mem_read(uint8_t* rx_data_ptr) {
    uint16_t cmdOutLen = RFET_UART_BYPASS_SF_SIZE_CMD_OUT;
    ruci_para_mem_read_t *pCmdIn, *pCmdOut;

    pCmdIn = (ruci_para_mem_read_t*)(rx_data_ptr);
    pCmdOut = (ruci_para_mem_read_t*)(g_uart_tx_buffer);
    pCmdOut->ruci_header.u8 = pCmdIn->ruci_header.u8;
    pCmdOut->sub_header = pCmdIn->sub_header;

    if ((pCmdIn->data_length > 2048)
        || (pCmdIn->data_length > RFET_UART_BUFFER_MAX_SIZE)) {
        *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_FAIL;
        return (cmdOutLen);
    }

    cmdOutLen += pCmdIn->data_length;
    *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_SUCCESS;
    RfMcu_MemoryGet(pCmdIn->addr, ((&pCmdOut->sub_header) + 2),
                    pCmdIn->data_length);

    return (cmdOutLen);
}

uint16_t rfet_uart_bypass_host_cmd_mem_write(uint8_t* rx_data_ptr) {
    uint16_t cmdOutLen = RFET_UART_BYPASS_SF_SIZE_CMD_OUT;
    ruci_para_mem_write_t *pCmdIn, *pCmdOut;

    pCmdIn = (ruci_para_mem_write_t*)(rx_data_ptr);
    pCmdOut = (ruci_para_mem_write_t*)(g_uart_tx_buffer);
    pCmdOut->ruci_header.u8 = pCmdIn->ruci_header.u8;
    pCmdOut->sub_header = pCmdIn->sub_header;

    if ((pCmdIn->data_length > 2048)
        || (pCmdIn->data_length > RFET_UART_BUFFER_MAX_SIZE)) {
        *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_FAIL;
        return (cmdOutLen);
    }

    *((&pCmdOut->sub_header) + 1) = RFET_UART_BYPASS_SF_CMD_SUCCESS;

    RfMcu_MemorySet(pCmdIn->addr, pCmdIn->data, pCmdIn->data_length);

    return (cmdOutLen);
}

bool rfet_uart_bypass_host_cmd(uint16_t* cmd_out_len) {
    *cmd_out_len = 0;

    /* Command parser */
    switch (g_rx_buf_ptr[1]) {
        case (RUCI_CODE_SFR_READ):
            /* SFR read */
            *cmd_out_len = rfet_uart_bypass_host_cmd_sfr_read(g_rx_buf_ptr);
            break;
        case (RUCI_CODE_SFR_WRITE):
            /* SFR write */
            *cmd_out_len = rfet_uart_bypass_host_cmd_sfr_write(g_rx_buf_ptr);
            break;
        case (RUCI_CODE_IO_READ):
            /* IO read */
            *cmd_out_len = rfet_uart_bypass_host_cmd_io_read(g_rx_buf_ptr);
            break;
        case (RUCI_CODE_IO_WRITE):
            /* IO write */
            *cmd_out_len = rfet_uart_bypass_host_cmd_io_write(g_rx_buf_ptr);
            break;
        case (RUCI_CODE_MEM_READ):
            /* Memory read */
            *cmd_out_len = rfet_uart_bypass_host_cmd_mem_read(g_rx_buf_ptr);
            break;
        case (RUCI_CODE_MEM_WRITE):
            /* Memory write */
            *cmd_out_len = rfet_uart_bypass_host_cmd_mem_write(g_rx_buf_ptr);
            break;
        case (RUCI_CODE_REG_READ):
        //break;
        case (RUCI_CODE_REG_WRITE):
        //break;
        default: return false;
    }

    return true;
}

/*  */
void rfet_uart_bypass_entry(uint8_t* rx_data_ptr, uint16_t rx_len) {

    uint8_t q_id = 0;
    uint8_t q_type = rx_data_ptr[3];
    RFET_UART_BYPASS_STS bypass_sts = RFET_UART_BYPASS_STS_SUCCESS;

#if 0
    g_data_len_to_spi = (uint16_t)(rx_data_ptr[1] | (rx_data_ptr[2] << 8));

    /* wrong length, this is invalid data */
    if (g_data_len_to_spi <= 1)
    {
        return;
    }

    g_data_len_to_spi -= 1;

    /* to make sure the rfet uart bypass task can handle the uart buffer */

    /* data Q or command Q */
    switch (q_type)
    {

    case RFET_UART_BYPASS_TYPE_HCI_ACL_DATA:
        q_id = g_hci_acl_data_q_num;
        g_hci_acl_data_q_num++;
    case RFET_UART_BYPASS_TYPE_PCI_DATA:
        do
        {
            bypass_sts = rfet_uart_bypass_send_spi_data_q(q_id, rx_data_ptr + 3, g_data_len_to_spi);
        } while (bypass_sts == RFET_UART_BYPASS_STS_SLEEP);
        break;
    default:
        do
        {
            bypass_sts = rfet_uart_bypass_send_spi_cmd_q(rx_data_ptr + 3, g_data_len_to_spi);
        } while (bypass_sts == RFET_UART_BYPASS_STS_SLEEP);
        break;

    }

    if (bypass_sts == RFET_UART_BYPASS_STS_FAIL)
    {
        ;// fatal error
    }

#else

    q_type = rx_data_ptr[0];

    /* data Q or command Q */
    switch (q_type) {

        case RFET_UART_BYPASS_TYPE_HCI_ACL_DATA:
            q_id = g_hci_acl_data_q_num;
            g_hci_acl_data_q_num++;
            do {
                bypass_sts = rfet_uart_bypass_send_spi_data_q(q_id, rx_data_ptr,
                                                              rx_len);
            } while (bypass_sts == RFET_UART_BYPASS_STS_SLEEP);
            break;

        case RFET_UART_BYPASS_TYPE_PCI_DATA:
            do {
                bypass_sts = rfet_uart_bypass_send_spi_data_q(q_id, rx_data_ptr,
                                                              rx_len);
            } while (bypass_sts == RFET_UART_BYPASS_STS_SLEEP);
            break;

        case RFET_UART_BYPASS_TYPE_HOST_CMD:
            /* set flag to let loop handle the host command */
            g_bypass_ctrl |= RFET_UART_BYPASS_CTRL_ACCESS_HOST_CMD;

            g_rx_buf_ptr = rx_data_ptr;
            break;

        default:
            do {
                bypass_sts = rfet_uart_bypass_send_spi_cmd_q(rx_data_ptr,
                                                             rx_len);
            } while (bypass_sts == RFET_UART_BYPASS_STS_SLEEP);
            break;
    }

    if (bypass_sts == RFET_UART_BYPASS_STS_FAIL) {
        ; // fatal error
    }

#endif
}

bool rfet_uart_bypass_init(RF_FW_LOAD_SELECT fw_select) {
    if (rf_common_init_by_fw(fw_select, rfet_uart_bypass_isr) == true) {
        //printf("\r\nInit:%d Success",fw_select);
        return true;
    } else {
        //printf("\r\nInit:%d Fail",fw_select);
        return false;
    }
}

bool rfet_uart_bypass_is_busy(void) { return (g_bypass_ctrl) ? true : false; }

/* should be executed if mode is switched to GUI mode */
void rfet_uart_bypass_task(void) {
    uint16_t spi_to_uart_len = 0;

    /* remove GUI command header and try to run */
    if (g_bypass_ctrl) {
        if ((g_bypass_ctrl & RFET_UART_BYPASS_CTRL_READ_CMDQ)
            == RFET_UART_BYPASS_CTRL_READ_CMDQ) {
            /* read command q */
            if (rfet_uart_bypass_read_spi_cmd_q(&spi_to_uart_len)
                == RFET_UART_BYPASS_STS_SUCCESS) {
                /* UART TX */
                rfet_uart_tx_buf(spi_to_uart_len, false);
            }

            g_bypass_ctrl &= (~RFET_UART_BYPASS_CTRL_READ_CMDQ);
        } else if ((g_bypass_ctrl & RFET_UART_BYPASS_CTRL_READ_DATAQ)
                   == RFET_UART_BYPASS_CTRL_READ_DATAQ) {

            /* read data q */
            if (rfet_uart_bypass_read_spi_data_q(&spi_to_uart_len)
                == RFET_UART_BYPASS_STS_SUCCESS) {
                /* UART TX */
                rfet_uart_tx_buf(spi_to_uart_len, false);
            }
            g_bypass_ctrl &= (~RFET_UART_BYPASS_CTRL_READ_DATAQ);

        } else if ((g_bypass_ctrl & RFET_UART_BYPASS_CTRL_READ_MCU_STATE)
                   == RFET_UART_BYPASS_CTRL_READ_MCU_STATE) {
            /* read MCU state */
            g_bypass_ctrl &= (~RFET_UART_BYPASS_CTRL_READ_MCU_STATE);
        }
        /* The access host command flag is not set by interrupt, the action is not notified by RFB ISR */
        else if ((g_bypass_ctrl & RFET_UART_BYPASS_CTRL_ACCESS_HOST_CMD)
                 == RFET_UART_BYPASS_CTRL_ACCESS_HOST_CMD) {
            /* Access Host Command */
            if ((rfet_uart_bypass_host_cmd(&spi_to_uart_len))
                && (spi_to_uart_len > 0)) {
                rfet_uart_tx_buf(spi_to_uart_len, false);
            }
            g_bypass_ctrl &= (~RFET_UART_BYPASS_CTRL_ACCESS_HOST_CMD);
        }
    }
}
