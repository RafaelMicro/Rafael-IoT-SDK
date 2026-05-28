/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet_gui.c
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
#include "rf_common_init.h"
#include "dtm_mode.h"
#include "rfet_uart_bypass.h"
#include "rfet_gui.h"
#include "FreeRTOS.h"
#include "task.h"


/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
// <o> RFET_GUI_DTM_ENABLE_UART_BRIDGE_CMD
// <0=> 0: Disable
// <1=> 1: Enable
#define RFET_GUI_DTM_ENABLE_UART_BRIDGE_CMD (1)

#define BMU_TX_Q_ID_MAX                     (6)
#define RFET_CC_DEFAULT_EVT_PARA_LEN        (2)
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
volatile RFET_GUI_CTRL g_gui_ctrl = RFET_GUI_CTRL_NONE;
extern RF_FW_LOAD_SELECT g_fw_select;
volatile bool        g_gui_uart_tx_mutex = false;
extern bool g_gui_dtm_task;
bool        g_gui_command_event_tx_flag = false;
uint8_t     g_gui_hci_acl_data_q_id = 0;
volatile uint8_t     g_gui_read_event_cnt = 0;
volatile uint8_t     g_gui_read_data_cnt = 0;
extern TaskHandle_t xrf_rfet_taskHandle;
RFET_HDLR   gRfet_CustomerHandle = rfet_gui_cc_default_entry;


/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
RFET_GUI_STS rfet_gui_send_spi_cmd_q(uint8_t *send_spi_buf, uint16_t send_len)
{
    RF_MCU_TX_CMDQ_ERROR   tx_err = RF_MCU_TX_CMDQ_ERR_INIT;

    /* to check state before issue cmd send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03)
    {
        return RFET_GUI_STS_SLEEP;
    }

    tx_err = RfMcu_CmdQueueSend(send_spi_buf, send_len);

    return (tx_err != RF_MCU_TX_CMDQ_SET_SUCCESS) ? RFET_GUI_STS_FAIL : RFET_GUI_STS_SUCCESS;
}


RFET_GUI_STS rfet_gui_send_spi_data_q(uint8_t qId, uint8_t *send_spi_buf, uint16_t send_len)
{
    RF_MCU_TXQ_ERROR    tx_err = RF_MCU_TXQ_ERR_INIT;

    /* to check state before issue TX send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03)
    {
        return RFET_GUI_STS_SLEEP;
    }

    tx_err = RfMcu_TxQueueSendById(qId, send_spi_buf, send_len);

    return (tx_err != RF_MCU_TXQ_SET_SUCCESS) ? RFET_GUI_STS_FAIL : RFET_GUI_STS_SUCCESS;

}

RFET_GUI_STS rfet_gui_read_spi_cmd_q(uint16_t *tx_len)
{
    uint16_t        rx_len;
    RF_MCU_RX_CMDQ_ERROR   rx_err = RF_MCU_RX_CMDQ_ERR_INIT;

    /* read cmd queue to uart tx buffer after RFET Header */
    rx_len = RfMcu_EvtQueueRead(g_uart_tx_buffer + RFET_CMD_LEN_GUI_CONTROL_EVENT_HEADER_SIZE, &rx_err);

    if (rx_err == RF_MCU_RX_CMDQ_GET_SUCCESS)
    {
        *tx_len = rx_len;
    }

    return (rx_err != RF_MCU_RX_CMDQ_GET_SUCCESS) ? RFET_GUI_STS_FAIL : RFET_GUI_STS_SUCCESS;

}

RFET_GUI_STS rfet_gui_read_spi_data_q(uint16_t *tx_len)
{
    uint16_t        rx_len;
    RF_MCU_RXQ_ERROR    rx_err = RF_MCU_RXQ_ERR_INIT;

    /* get data from rx data Q to uart tx buffer after RFET Header */
    rx_len = RfMcu_RxQueueRead(g_uart_tx_buffer + RFET_CMD_LEN_GUI_CONTROL_EVENT_HEADER_SIZE, &rx_err);

    if (rx_err == RF_MCU_RXQ_GET_SUCCESS)
    {
        *tx_len = rx_len;
    }

    return (rx_err != RF_MCU_RXQ_GET_SUCCESS) ? RFET_GUI_STS_FAIL : RFET_GUI_STS_SUCCESS;

}

void rfet_gui_isr(uint8_t intStatus)
{
    //uint8_t readSfr;

    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03)
    {
        /* FOR LEVEL TRIGGER ONLY, the MCU will keep entering INT if status not cleared */
        return;
    }

    // CMD --------------------------------------------------------------------
    if (intStatus & 0x01)
    {
        /* read cmd q */
        g_gui_ctrl |= RFET_GUI_CTRL_READ_CMDQ;
        g_gui_read_event_cnt++;
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
    if (intStatus & 0x02)
    {
        g_gui_ctrl |= RFET_GUI_CTRL_READ_MCU_STATE;
        //gUasbMcuState = (uint8_t)RfMcu_McuStateRead();
    }

    // RX packet --------------------------------------------------------------
    if (intStatus & 0x20)
    {
        /* read RX Q */
        g_gui_ctrl |= RFET_GUI_CTRL_READ_DATAQ;
        g_gui_read_data_cnt++;

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

    vTaskResume(xrf_rfet_taskHandle);

}

void rfet_gui_enter_dtm_task(void)
{
    /* init DTM mode */
    dtm_sys_init(RF_FW_LOAD_SELECT_RUCI_CMD, true);

#if (RFET_GUI_DTM_ENABLE_UART_BRIDGE_CMD == 1)
    dtm_sys_set_enable(true);
#endif

    /* hijack GUI task */
    while (g_gui_dtm_task == true)
    {

        /* Run DTM task */
        /* dtm_task(); TO DO for RTOS task */

#if (RFET_GUI_DTM_ENABLE_UART_BRIDGE_CMD == 1)
        /* Run UART Bypass task */
        if (dtm_sys_is_enable() == true)
        {
            rfet_uart_bypass_task();
        }
#endif

        /* exit by GUI control */
    }

#if (RFET_GUI_DTM_ENABLE_UART_BRIDGE_CMD == 1)
    dtm_sys_set_enable(false);
#endif


}

void rfet_gui_uart_bypass_cmd(uint8_t *rx_data_ptr, uint16_t rx_len)
{
    uint8_t q_id = 0;
    uint8_t q_type = rx_data_ptr[0];
    RFET_UART_BYPASS_STS bypass_sts = RFET_GUI_STS_SUCCESS;

    /* data Q or command Q */
    switch (q_type)
    {
        case RFET_UART_BYPASS_TYPE_HCI_ACL_DATA:
            /* Determine Q id*/
            q_id = g_gui_hci_acl_data_q_id;

            /* Increment Q ID, if TX Q ID exceed MAX, wrap around it */
            g_gui_hci_acl_data_q_id++;
            if (g_gui_hci_acl_data_q_id > BMU_TX_Q_ID_MAX)
            {
                g_gui_hci_acl_data_q_id = 0;
            }
            do
            {
                bypass_sts = rfet_gui_send_spi_data_q(q_id, rx_data_ptr, rx_len);
            } while (bypass_sts == RFET_GUI_STS_SLEEP);
            break;

        case RFET_UART_BYPASS_TYPE_PCI_DATA:
            do
            {
                bypass_sts = rfet_gui_send_spi_data_q(q_id, rx_data_ptr, rx_len);
            } while (bypass_sts == RFET_GUI_STS_SLEEP);
            break;

        default:
            do
            {
                bypass_sts = rfet_gui_send_spi_cmd_q(rx_data_ptr, rx_len);
            } while (bypass_sts == RFET_GUI_STS_SLEEP);
            break;
    }
}

/*
+--------+--------+------------------+-------------------------------------------+--------+
| 1 Byte | 2 Byte |      1 Byte      |                   N Byte                  | 1 Byte |
+========+========+==================+===========================================+========+
| Header | Length | Parameter Byte 0 |            Parameter[RUCI CMD]            |   CRC  |
+--------+--------+------------------+-------------------------------------------+--------+
|  0xE0  |        |       0x93       | [Header][Sub Header][Para. Length][Para.] |        |
+--------+--------+------------------+-------------------------------------------+--------+
*/

void reft_gui_add_event_header(uint16_t len)
{
    g_uart_tx_buffer[0] = RFET_EVENT;
    g_uart_tx_buffer[1] = (uint8_t)(len & 0xFF);
    g_uart_tx_buffer[2] = (uint8_t)(len >> 8);
    g_uart_tx_buffer[3] = RFET_CMD_GUI_CONTROL_RUCI_CMD;
}

void reft_gui_command_event_package(uint8_t *event_ptr, uint16_t event_len)
{
    reft_gui_add_event_header(event_len);
    memcpy(g_uart_tx_buffer + RFET_CMD_COMMON_OVERHEAD_SIZE, event_ptr, event_len);
}

RF_FW_LOAD_SELECT rfet_gui_ruci_hdr_check(uint8_t cmd_ruci_header)
{
    switch (cmd_ruci_header)
    {
    case RFET_UART_BYPASS_TYPE_HCI_COMMAND:
    case RFET_UART_BYPASS_TYPE_HCI_ACL_DATA:
        return RF_FW_LOAD_SELECT_BLE_CONTROLLER;
    case RFET_UART_BYPASS_TYPE_PCI_COMMON_COMMAND:
    case RFET_UART_BYPASS_TYPE_PCI_FSK_COMMAND:
    case RFET_UART_BYPASS_TYPE_PCI_BLE_COMMAND:
    case RFET_UART_BYPASS_TYPE_PCI_15P4MAC_COMMAND:
    case RFET_UART_BYPASS_TYPE_PCI_OQPSK_COMMAND:
    case RFET_UART_BYPASS_TYPE_PCI_DATA:
    case RFET_UART_BYPASS_TYPE_CMM_SYS_COMMAND:
    case RFET_UART_BYPASS_TYPE_CMM_HAL_COMMAND:
#if ((RF_MCU_CONST_LOAD_SUPPORTED == 0) || (RF_MCU_CHIP_MODEL != RF_MCU_CHIP_569M0))
        return RF_FW_LOAD_SELECT_RUCI_CMD;
#else
        return RF_FW_LOAD_SELECT_INTERNAL_TEST;
#endif
    case RFET_UART_BYPASS_TYPE_APCI_RF_COMMAND:
    case RFET_UART_BYPASS_TYPE_APCI_TX_COMMAND:
    case RFET_UART_BYPASS_TYPE_APCI_RX_COMMAND:
    case RFET_UART_BYPASS_TYPE_HOST_CMD:
        return RF_FW_LOAD_SELECT_UNSUPPORTED_CMD;
    }

    return RF_FW_LOAD_SELECT_UNSUPPORTED_CMD;

}


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/*
+--------+--------+-------------------------------------------+--------+
| 1 Byte | 2 Byte |                   N Byte                  | 1 Byte |
+========+========+===========================================+========+
| Header | Length |            Parameter[RUCI CMD]            |   CRC  |
+--------+--------+-------------------------------------------+--------+
|  0x93  |        | [Header][Sub Header][Para. Length][Para.] |        |
+--------+--------+-------------------------------------------+--------+
*/
/* RFET UART BYPASS 0x93  */
void rfet_gui_command_control(uint8_t *rx_data_ptr, uint16_t rx_len)
{
    uint8_t cmd_type = rx_data_ptr[0];
    uint8_t cmd_event_ret[4];
    uint8_t cmd_ruci_header = rx_data_ptr[3];
    RF_FW_LOAD_SELECT fw_check;

    switch (cmd_type)
    {
    case RFET_CMD_GUI_CONTROL_DTM_CTRL:

        cmd_event_ret[0] = RFET_CMD_GUI_CONTROL_DTM_CTRL;

        if (g_fw_select == RF_FW_LOAD_SELECT_RUCI_CMD)
        {

            /* Enable DTM handle task */
            if (rx_data_ptr[3] == RFET_GUI_DTM_MODE_ENABLE)
            {
                g_gui_dtm_task = true;
            }
            else if (rx_data_ptr[3] == RFET_GUI_DTM_MODE_DISABLE)
            {
                g_gui_dtm_task = false;
            }

            cmd_event_ret[1] = RFET_EVENT_RET_STATUS_SUCCESS;
        }
        else
        {
            cmd_event_ret[1] = RFET_EVENT_RET_STATUS_FAIL_WRONG_FW;
        }

        reft_gui_command_event_package(cmd_event_ret, 2);
        g_gui_command_event_tx_flag = true;
        break;

    case RFET_CMD_GUI_CONTROL_RF_FW_SWITCH:

        cmd_event_ret[0] = RFET_CMD_GUI_CONTROL_RF_FW_SWITCH;
        cmd_event_ret[1] = RFET_EVENT_RET_STATUS_SUCCESS;

        /* Switch  FW */
        /* Init Different FW */
        if (rx_data_ptr[3] <= RF_FW_LOAD_SELECT_BLE_CONTROLLER)
        {
#if ((RF_MCU_CONST_LOAD_SUPPORTED == 0) || (RF_MCU_CHIP_MODEL != RF_MCU_CHIP_569M0))
            if (rfet_gui_init(rx_data_ptr[3]) == false)
#else
            if (rfet_gui_init(RF_FW_LOAD_SELECT_INTERNAL_TEST) == false)
#endif
            {
                cmd_event_ret[1] = RFET_EVENT_RET_STATUS_FAIL;
                ;//init FW error handling
            }
        }
        reft_gui_command_event_package(cmd_event_ret, 2);
        g_gui_command_event_tx_flag = true;

        /*force to stop in order to send event to host  */
        g_gui_dtm_task = false;
        break;

    case RFET_CMD_GUI_CONTROL_UART_BYPASS:

        fw_check = rfet_gui_ruci_hdr_check(cmd_ruci_header);

        cmd_event_ret[0] = RFET_CMD_GUI_CONTROL_RUCI_CMD;

        if (fw_check == RF_FW_LOAD_SELECT_UNSUPPORTED_CMD)
        {
            cmd_event_ret[1] = RFET_EVENT_RET_STATUS_FAIL;
        }
        else if (fw_check != g_fw_select)
        {
            cmd_event_ret[1] = RFET_EVENT_RET_STATUS_FAIL_WRONG_FW;
        }
        else
        {
            /* RUCI and HCI Commands */
            /* dirtect transfer the command to RFB */
            /* Ignore Header and 2B length */
            rfet_gui_uart_bypass_cmd(rx_data_ptr + RFET_CMD_COMMON_OVERHEAD_SIZE, rx_len - 1 - RFET_CMD_COMMON_OVERHEAD_SIZE);
        }

        /* check if FW and commnad matched */
        if (cmd_event_ret[1] != RFET_EVENT_RET_STATUS_SUCCESS)
        {
            reft_gui_command_event_package(cmd_event_ret, 2);
            g_gui_command_event_tx_flag = true;
        }
        break;

    case RFET_CMD_GUI_CONTROL_GET_CHIP_INFO:
        cmd_event_ret[0] = RFET_CMD_GUI_CONTROL_GET_CHIP_INFO;
        cmd_event_ret[1] = RFET_EVENT_RET_STATUS_SUCCESS;
        /* Check GetOtpVersion when ready */
        cmd_event_ret[2] = '4';
        /* Change when chip define for RT584z/RT584h/RT584l/RF1301/RT584c are ready */
#if defined (CONFIG_RT584H) || (CONFIG_RT584HA4)
        cmd_event_ret[3] = 'H';
#elif defined(CONFIG_RT584L)
        cmd_event_ret[3] = 'L';
#elif defined(CONFIG_RF1301)
        cmd_event_ret[3] = 'S';
#elif defined(CONFIG_RT584C)
        cmd_event_ret[3] = 'C';
#else
        cmd_event_ret[3] = '?';
#endif

        reft_gui_command_event_package(cmd_event_ret, 4);
        g_gui_command_event_tx_flag = true;
        break;

    case RFET_CMD_GUI_CUSTOMER_CMD:
        gRfet_CustomerHandle(rx_data_ptr, rx_len);
        break;

    default:
        break;
    }

}

void rfet_gui_cc_event_gen(uint8_t *evt_para, uint16_t para_len)
{
    reft_gui_command_event_package(evt_para, para_len);
    g_gui_command_event_tx_flag = true;
}

void rfet_gui_cc_default_entry(uint8_t *rx_data_ptr, uint16_t rx_len)
{
    uint8_t cmd_type = rx_data_ptr[0];
    uint8_t cmd_ruci_header = rx_data_ptr[3];
    uint8_t event_data[RFET_CC_DEFAULT_EVT_PARA_LEN];

    /* Do nothing and reply event */
    UNUSED(cmd_type);
    UNUSED(cmd_ruci_header);

    event_data[0] = RFET_CMD_GUI_CUSTOMER_CMD;
    event_data[1] = RFET_EVENT_RET_STATUS_SUCCESS;

    rfet_gui_cc_event_gen(event_data, RFET_CC_DEFAULT_EVT_PARA_LEN);
}

void rfet_gui_cc_entry_registration(RFET_HDLR rfet_hdlr)
{
    if (rfet_hdlr)
    {
        gRfet_CustomerHandle = rfet_hdlr;
    }
}

bool rfet_gui_init(RF_FW_LOAD_SELECT fw_select)
{
    if (rf_common_init_by_fw(fw_select, rfet_gui_isr) == true)
    {
        g_fw_select = fw_select;
        return true;
    }
    else
    {
        return false;
    }
}

bool rfet_gui_is_busy(void)
{
    return (g_gui_ctrl) ? true : false;
}

void rfet_gui_mutex_unlock(void)
{
    g_gui_uart_tx_mutex = false;
}

/* GUI main task */
void rfet_gui_task(void)
{
    uint16_t spi_to_uart_len = 0;
    uint16_t event_len;

    if (g_gui_command_event_tx_flag == true)
    {

        event_len = (uint16_t)(g_uart_tx_buffer[1] | (g_uart_tx_buffer[2] << 8));

        /* return GUI command event, always 6 byte */
        rfet_uart_tx_buf(event_len + RFET_CMD_COMMON_OVERHEAD_SIZE, true);

        g_gui_command_event_tx_flag = false;

    }
    /* remove GUI command header and try to run */
    else if (g_gui_ctrl && (g_gui_uart_tx_mutex == false))
    {
        if ((g_gui_ctrl & RFET_GUI_CTRL_READ_CMDQ) == RFET_GUI_CTRL_READ_CMDQ)
        {

            /* read command q */
            if (rfet_gui_read_spi_cmd_q(&spi_to_uart_len) == RFET_GUI_STS_SUCCESS)
            {

                g_gui_uart_tx_mutex = true;

                /* add RFET header and CRC for returning event */
                reft_gui_add_event_header(spi_to_uart_len + 1);

                /* UART TX */
                rfet_uart_tx_buf(spi_to_uart_len + RFET_CMD_LEN_GUI_CONTROL_EVENT_HEADER_SIZE, true);
            }
            enter_critical_section();
            if (--g_gui_read_event_cnt == 0)
            {
                g_gui_ctrl &= (~RFET_GUI_CTRL_READ_CMDQ);
            }
            leave_critical_section();
        }
        else if ((g_gui_ctrl & RFET_GUI_CTRL_READ_DATAQ) == RFET_GUI_CTRL_READ_DATAQ)
        {

            /* read data q */
            if (rfet_gui_read_spi_data_q(&spi_to_uart_len) == RFET_GUI_STS_SUCCESS)
            {

                g_gui_uart_tx_mutex = true;

                /* add RFET header and CRC for returning event */
                reft_gui_add_event_header(spi_to_uart_len + 1);

                /* UART TX */
                rfet_uart_tx_buf(spi_to_uart_len + RFET_CMD_LEN_GUI_CONTROL_EVENT_HEADER_SIZE, true);
            }

            if (--g_gui_read_data_cnt == 0)
            {
                g_gui_ctrl &= (~RFET_GUI_CTRL_READ_DATAQ);
            }
        }
        else if ((g_gui_ctrl & RFET_GUI_CTRL_READ_MCU_STATE) == RFET_GUI_CTRL_READ_MCU_STATE)
        {
            /* read MCU state */
            g_gui_ctrl &= (~RFET_GUI_CTRL_READ_MCU_STATE);
        }
    }
    else
    {

        if ((g_gui_dtm_task == true) && (g_gui_command_event_tx_flag == false))
        {
            /* stay in this task until dtm control is disabled */
            rfet_gui_enter_dtm_task();
        }

    }

}


