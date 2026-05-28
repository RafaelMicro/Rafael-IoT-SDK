/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet.c
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
#include "stdio.h"
#include "string.h"

#include "mcu.h"
//#include "project_config.h"

#include "hosal_gpio.h"
#include "hosal_uart.h"
#include "hosal_sysctrl.h"
#include "lpm.h"
#include "sysctrl.h"
//#include "uart_drv.h"
//#include "retarget.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "rf_common_init.h"
#include "rf_mcu.h"

#include "ruci.h"
//#include "uart_bridge.h"
#include "dtm_mode.h"
#include "rfet.h"
#include "rfet_cli.h"
#include "rfet_gui.h"
#if (RFET_HOST_ENABLE == 1)
#include "rfet_host.h"
#endif
#include "rfet_uart_bypass.h"
#include "rf_tx_comp.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    Internal Functions
 *************************************************************************************************/
void rfet_command_mode_switch(uint8_t* rx_data_ptr, uint16_t rx_len);
void rfet_command_uart_bridge(uint8_t* rx_data_ptr);

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
// <o> RFET_UART_RX_TASK_PHASE
// <0=> 0: Disable
// <1=> 1: Enable
#define RFET_UART_RX_TASK_PHASE (1)

/*
 * Remark: UART_BAUDRATE_115200 is not 115200...Please don't use 115200 directly
 * Please use macro define  UART_BAUDRATE_XXXXXX
 */

#define PRINTF_BAUDRATE UART_BAUDRATE_115200
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
extern TaskHandle_t xrf_rfet_taskHandle;
extern TaskHandle_t xrf_dtm_taskHandle;
/* RFET Version */
// <o> RFET Version
uint32_t g_rfet_version = 140;

/* UART Buffer */
bool volatile g_uart_tx_start = false;
uint8_t g_uart_tx_buffer
    [RFET_UART_BUFFER_MAX_SIZE]; /* Buffer with maximum UART TX length */
uint8_t* g_uart_tx_buf;

uint8_t g_rfet_guart_byte_data;

uint16_t g_uart_tx_idx = 0;
uint16_t g_uart_tx_size = 0;

uint8_t g_uart_rx_buffer
    [RFET_UART_BUFFER_MAX_SIZE]; /* Buffer with maximum UART RX length */
uint16_t g_uart_rx_idx;
uint16_t g_uart_rx_length;

uint8_t g_uart_event_buffer
    [RFET_UART_EVENT_BUFFER_MAX_SIZE]; /* Buffer with maximum UART RX length */

uint16_t g_uart_check_gui_cnt = 0;
uint16_t g_uart_check_gui_last_rx_len = 0;

RFET_UART_RX_CTRL g_urat_rx_ctrl = RFET_UART_RX_CTRL_NONE;

/* RFET Control Global Variables */
RFET_MODE_SWITCH g_rfet_mode_control;
//RFET_FW_SWITCH      g_reft_fw_control = RFET_FW_CONTROL_RUCI_CMD;

/* command group 0 */
s_rfet_cmd_t const c_rfet_command_handler[] = {
    /* Start */
    /* Command Handler */
    {RFET_CMD_MODE_SWITCH_GUI, RFET_CMD_LEN_MODE_SWITCH,
     &rfet_command_mode_switch},
    {RFET_CMD_MODE_SWITCH_UART_BRIDGE, RFET_CMD_LEN_MODE_SWITCH,
     &rfet_command_mode_switch},
    {RFET_CMD_MODE_SWITCH_HOST, RFET_CMD_LEN_MODE_SWITCH,
     &rfet_command_mode_switch},
    /* the GUI command should directly pass to GUI task to handle */
    {RFET_CMD_GUI_CONTROL_DTM_CTRL, RFET_CMD_LEN_GUI_CONTROL_DTM_CTRL,
     &rfet_gui_command_control},
    {RFET_CMD_GUI_CONTROL_RF_FW_SWITCH, RFET_CMD_LEN_GUI_CONTROL_RF_FW_SWITCH,
     &rfet_gui_command_control},
    {RFET_CMD_GUI_CONTROL_GET_CHIP_INFO, RFET_CMD_LEN_GUI_CONTROL_GET_CHIP_INFO,
     &rfet_gui_command_control},
    {RFET_CMD_GUI_CONTROL_RUCI_CMD, 0, &rfet_gui_command_control},
    {RFET_CMD_GUI_CUSTOMER_CMD, 0, &rfet_gui_command_control},
#if (RFET_HOST_ENABLE == 1)
    {RFET_CMD_HOST_CONTROL_CMD, 0, &rfet_host_command_control},
#endif
    /* Event Handler */
    {RFET_EVENT, 0, 0},
    /* End */
    {RFET_CMD_HDLR_END, 0, 0}};

s_rfet_cmd_t const* g_rfet_command_handler = c_rfet_command_handler;

#if (CHOOSE_RFET_UART == UART_0)
#define RFET_UART_TX_IO 17
#define RFET_UART_RX_IO 16
#elif (CHOOSE_RFET_UART == UART_1)
#define RFET_UART_TX_IO 28
#define RFET_UART_RX_IO 29
#endif
HOSAL_UART_DEV_DECL(uart_dev_rfet, CHOOSE_RFET_UART, RFET_UART_TX_IO,
                    RFET_UART_RX_IO, UART_BAUDRATE_115200)

/**************************************************************************************************
 *    Internal Functions
 *************************************************************************************************/
uint8_t rfet_command_validation_check(RFET_CMD_HEADER in_cmd_header,
                                      uint8_t* rx_data_ptr);
RFET_UART_RX_DISPATCH rfet_uart_rx_data_dispatch(uint8_t rx_data);
void rfet_command_crc_add(uint8_t* rx_data_ptr, uint16_t buf_len);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
#if 0
void rfet_uart_isr(uint32_t event, void *p_context)
{
    /*Notice:
        UART_EVENT_TX_DONE  is for asynchronous mode send
        UART_EVENT_RX_DONE  is for synchronous  mode receive

        if system wants to use p_context as parameter, it can cast
        the type of p_context to original type.  like

        uint32_t  phandle;

        phandle = (uint32_t *) p_context;

     */
    uint8_t uart_rx_data;

    if (event & UART_EVENT_TX_DONE)
    {
        /*if you use multi-tasking, signal the waiting task here.*/
        g_uart_tx_start = 0;
        rfet_gui_mutex_unlock();
    }

    if (event & UART_EVENT_RX_DONE)
    {

        uart_rx(CHOOSE_RFET_UART, &g_rfet_guart_byte_data, 1);
        uart_rx_data = g_rfet_guart_byte_data;
        /*if you use multi-tasking, signal the waiting task here.*/
        //g_uart_rx_buffer[g_uart_rx_idx++] = guart_byte_data;
        if (rfet_uart_rx_data_dispatch(uart_rx_data) == RFET_UART_RX_DISPATCH_CLI_CH)
        {
            uart_rx_data_update(uart_rx_data);
        }




        //uasb_data_rx_parse();
        //gpio_pin_toggle(22);

    }

    if (event & (UART_EVENT_RX_OVERFLOW | UART_EVENT_RX_BREAK |
                 UART_EVENT_RX_FRAMING_ERROR | UART_EVENT_RX_PARITY_ERROR))
    {

        //it's almost impossible for those error case.
        //do something ...
        if (event & UART_EVENT_RX_FRAMING_ERROR)
        {

        }
        else
        {
            //wait WDT
            //while(1);
        }
    }

}
#else
int rfet_uart_rx_callback(void* p_arg) {
    uint8_t uart_rx_data;

    hosal_uart_receive(&uart_dev_rfet, &g_rfet_guart_byte_data, 1);
    //uart_rx(CHOOSE_RFET_UART, &g_rfet_guart_byte_data, 1);

    uart_rx_data = g_rfet_guart_byte_data;
    /*if you use multi-tasking, signal the waiting task here.*/
    //g_uart_rx_buffer[g_uart_rx_idx++] = guart_byte_data;
    if (rfet_uart_rx_data_dispatch(uart_rx_data)
        == RFET_UART_RX_DISPATCH_CLI_CH) {
        uart_rx_data_update(uart_rx_data);
    }

    hosal_gpio_pin_toggle(22);
    return 0;
}

int rfet_uart_tx_callback(void* p_arg) {
    g_uart_tx_start = 0;
    rfet_gui_mutex_unlock();
    return 0;
}
#endif

void rfet_uart_tx(uint16_t length) //UASB_Uart_Tx(uint16_t length)
{
    g_uart_tx_start = 1;
    hosal_uart_send(&uart_dev_rfet, g_uart_tx_buffer, length);
}

void rfet_uart_init(void) {
#if 0
    static uint32_t  handle;

    uart_config_t  uart1_drv_config;

    uart1_drv_config.baudrate = UART_BAUDRATE_115200;
    uart1_drv_config.databits = UART_DATA_BITS_8;
    uart1_drv_config.hwfc     = UART_HWFC_DISABLED;
    uart1_drv_config.parity   = UART_PARITY_NONE;

    /* Important: p_contex will be the second parameter in uart callback.
     * In this example, we do NOT use p_context, (So we just use handle for sample)
     * but you can use it for whaterever you want. (It can be NULL, too)
     */
    handle = 0;
    uart1_drv_config.p_context = (void *) &handle;

    uart1_drv_config.stopbit  = UART_STOPBIT_ONE;
    uart1_drv_config.interrupt_priority = IRQ_PRIORITY_NORMAL;

    /* init uart 1*/
    uart_init(CHOOSE_RFET_UART, &uart1_drv_config, rfet_uart_isr);

    /* set uart 1 DMA RX buf */
    uart_rx(CHOOSE_RFET_UART, &g_rfet_guart_byte_data, 1);
#else
    hosal_uart_init(&uart_dev_rfet);

    hosal_uart_callback_set(&uart_dev_rfet, HOSAL_UART_RX_CALLBACK,
                            rfet_uart_rx_callback, &uart_dev_rfet);
    hosal_uart_callback_set(&uart_dev_rfet, HOSAL_UART_TX_CALLBACK,
                            rfet_uart_tx_callback, &uart_dev_rfet);

    hosal_uart_ioctl(&uart_dev_rfet, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT);
#endif
}

void rfet_uart_tx_buf(uint16_t tx_len, bool crc_en) {
    uint16_t tx_add_len;
    g_uart_tx_start = true;
    g_uart_tx_size = tx_len;
    g_uart_tx_idx = 1;
    g_uart_tx_buf = g_uart_tx_buffer;

    if (crc_en == true) {

        /* add one more byte if CRC enabled */
        tx_add_len = (uint16_t)(g_uart_tx_buffer[1]
                                | (g_uart_tx_buffer[2] << 8));
        tx_add_len += 1;

        g_uart_tx_buffer[1] = (uint8_t)(tx_add_len & 0xFF);
        g_uart_tx_buffer[2] = (uint8_t)(tx_add_len >> 8);

        /* CRC generate */
        rfet_command_crc_add(g_uart_tx_buffer, tx_len);
        g_uart_tx_size += 1;
    }

    //UART1_TX(g_uart_tx_buffer);
    rfet_uart_tx(g_uart_tx_size);
}

void rfet_uart_tx_dtm_end_event(void) {
    g_uart_tx_buffer[0] = 0;
    g_uart_tx_buffer[1] = 0;

    rfet_uart_tx(2);
}

/* Init global UART RX buffer */
void rfet_uart_rx_buffer_init(void) {
    memset(g_uart_rx_buffer, 0, RFET_UART_BUFFER_MAX_SIZE);
    //memset(g_uart_tx_buffer, 0, RFET_UART_BUFFER_MAX_SIZE);

    g_uart_tx_start = FALSE;
    g_uart_tx_idx = 0;
    g_uart_rx_length = 0;
}

void rfet_uart_get_ruci_hdr_len(uint8_t cmd_type, uint8_t* hdr_len,
                                bool* is_word_len) {
    switch (cmd_type) {
        case RFET_UART_BYPASS_TYPE_HCI_COMMAND:

            *hdr_len = RFET_UART_BYPASS_TYPE_HCI_COMMAND_HDR_SIZE;

            break;
        case RFET_UART_BYPASS_TYPE_HCI_ACL_DATA:

            *hdr_len = RFET_UART_BYPASS_TYPE_HCI_ACL_DATA_HDR_SIZE;
            *is_word_len = true;

            break;
        case RFET_UART_BYPASS_TYPE_PCI_COMMON_COMMAND:
        case RFET_UART_BYPASS_TYPE_PCI_FSK_COMMAND:
        case RFET_UART_BYPASS_TYPE_PCI_BLE_COMMAND:
        case RFET_UART_BYPASS_TYPE_PCI_15P4MAC_COMMAND:
        case RFET_UART_BYPASS_TYPE_PCI_OQPSK_COMMAND:
        case RFET_UART_BYPASS_TYPE_CMM_HAL_COMMAND:
            *hdr_len = RFET_UART_BYPASS_TYPE_PCI_COMMAND_HDR_SIZE;

            break;
        case RFET_UART_BYPASS_TYPE_PCI_DATA:

            *hdr_len = RFET_UART_BYPASS_TYPE_PCI_DATA_HDR_SIZE;
            *is_word_len = true;

            break;
        case RFET_UART_BYPASS_TYPE_APCI_RF_COMMAND:
        case RFET_UART_BYPASS_TYPE_APCI_TX_COMMAND:
        case RFET_UART_BYPASS_TYPE_APCI_RX_COMMAND:

            *hdr_len = RFET_UART_BYPASS_TYPE_APCI_COMMAND_HDR_SIZE;

            break;
        case RFET_UART_BYPASS_TYPE_HOST_CMD:

            *hdr_len = RFET_UART_BYPASS_TYPE_HOST_COMMAND_HDR_SIZE;
            *is_word_len = true;

            break;
        default:
            *hdr_len = 0xFF; /* header length error */
            break;
    }
}

bool rfet_uart_check_gui_hdr(uint8_t cmd_type) {
    switch (cmd_type) {
        case RFET_CMD_MODE_SWITCH_GUI:
        case RFET_CMD_MODE_SWITCH_UART_BRIDGE:
        case RFET_CMD_MODE_SWITCH_HOST:
        case RFET_CMD_GUI_CONTROL_DTM_CTRL:
        case RFET_CMD_GUI_CONTROL_RF_FW_SWITCH:
        case RFET_CMD_GUI_CONTROL_GET_CHIP_INFO:
        case RFET_CMD_GUI_CONTROL_RUCI_CMD:
        case RFET_CMD_GUI_CUSTOMER_CMD:
        case RFET_CMD_DTM_TEST_END_CMD: return true;
    }

    return false;
}

bool rfet_uart_check_host_hdr(uint8_t cmd_type) {
    switch (cmd_type) {
        case RFET_CMD_HOST_CONTROL_CMD: return true;
    }

    return false;
}

bool rfet_uart_check_dtm_end(void) {
    if ((g_uart_rx_buffer[0] == 0xC1) && (g_uart_rx_buffer[1] == 0xAB)) {
        return true;
    }
    return false;
}

bool rfet_uart_check_ruci_hdr_len(uint8_t cmd_type, uint16_t len) {
    /* To Be Finished */
    if (len > (RFET_UART_BUFFER_MAX_SIZE - RFET_CMD_COMMON_OVERHEAD_SIZE
               - RFET_CMD_COMMON_CRC_LEN_SIZE)) {
        return false;
    }
    return true;
}

/* Handling UART RX command, CLI character or GUI command */
RFET_UART_RX_DISPATCH rfet_uart_rx_data_dispatch(uint8_t rx_data) {
    uint16_t rx_len;
    uint16_t rx_remaining_len;
    uint8_t hdr_len;
#if (RFET_UART_RX_TASK_PHASE == 0)
    uint8_t cmd_idx;
#endif
    bool is_word_len = false;
    bool ignore_bypass_check = false;

    /* no check, just directly pass to SPI */
    if ((g_rfet_mode_control == RFET_MODE_CONTROL_UART_BRIDGE)
        || (dtm_sys_is_enable() == true)) {

        if (g_rfet_mode_control == RFET_MODE_CONTROL_CLI) {

            /* check exit character 'R' when DTM enabled by CLI mode, to support exit char check for DTM + UART Bridge */
            if (g_uart_rx_length == 0 && (rx_data == 'r' || rx_data == 'R')) {

                ignore_bypass_check = true;
            }
        } else if ((g_rfet_mode_control == RFET_MODE_CONTROL_GUI)
                   || (g_rfet_mode_control == RFET_MODE_CONTROL_HOST)) {

            /* check if cmd type belongs to RFET command range */
            if (g_uart_rx_length == 0) {
                g_uart_rx_buffer[0] = rx_data;
            }

            ignore_bypass_check =
                (g_rfet_mode_control == RFET_MODE_CONTROL_GUI)
                    ? rfet_uart_check_gui_hdr(g_uart_rx_buffer[0])
                    : rfet_uart_check_host_hdr(g_uart_rx_buffer[0]);
        }

        /* and then check if the command belongs to RFB FW usage */
        if (ignore_bypass_check == false) {

            /* busy handling */
            if (rfet_uart_bypass_is_busy() == false) {

                g_uart_rx_buffer[g_uart_rx_length] = rx_data;
                g_uart_rx_length++;
                rx_remaining_len = 0xFFFF;

                rfet_uart_get_ruci_hdr_len(g_uart_rx_buffer[0], &hdr_len,
                                           &is_word_len);

                /* just check the first byte if belonging to RUCI or HCI group */
                if (hdr_len == 0xFF) {
                    rfet_uart_rx_buffer_init();
                } else {
                    if (g_uart_rx_length >= hdr_len) {
                        if (is_word_len == true) {
                            /* 2byte length */
                            rx_len = g_uart_rx_buffer[hdr_len - 2];
                            rx_len |= (g_uart_rx_buffer[hdr_len - 1] << 8);
                        } else {
                            rx_len = g_uart_rx_buffer[hdr_len - 1];
                        }

                        /* it is safer to compare length and command ID */
                        if (rfet_uart_check_ruci_hdr_len(g_uart_rx_buffer[0],
                                                         rx_len)
                            == true) {
                            rx_remaining_len = rx_len
                                               - (g_uart_rx_length - hdr_len);
                        } else {
                            rfet_uart_rx_buffer_init();
                        }
                    }
                    if (rx_remaining_len == 0) {
                        /* uart bypass handling */
                        rfet_uart_bypass_entry(g_uart_rx_buffer,
                                               g_uart_rx_length);

                        /* UART performance issue, if there are too much RX data,
                        the UART HW might be overflow since the handling cost too much time */

                        /* the RX buffer data must be kept for waiting uart bypass task to handle */
                        rfet_uart_rx_buffer_init();
                    }
                }
            }

            /* early abort */
            return RFET_UART_RX_DISPATCH_CMD_MODE;
        }
    }

    /* Handle ASCII 1 byte data or combined command data */
    if ((g_uart_rx_length == 0)
        && (rx_data < RFET_UART_CH_CHECK_THRESHOLD)) /* ASCII Input */
    {

        /* Deliver 1 Byte character to CLI task */
        return RFET_UART_RX_DISPATCH_CLI_CH;

    } else /* RFET COMMANDS (Includes GUI commands and HOST commands) */
    {

        g_uart_rx_buffer[g_uart_rx_length] = rx_data;
        g_uart_rx_length++;
        rx_remaining_len = 0xFFFF;

        /* check RFET header, call error handling if header is invalid */
        if ((rfet_uart_check_gui_hdr(g_uart_rx_buffer[0])) == false
            && (rfet_uart_check_host_hdr(g_uart_rx_buffer[0]) == false)) {
            /* how many following data will come in is unknown, */
            rfet_uart_rx_buffer_init();
        }

        /* check if received UART data is enough */
        if (g_uart_rx_length >= (RFET_CMD_COMMON_HEADER_SIZE
                                 + RFET_CMD_COMMON_HEADER_LEN_SIZE)) {

            rx_len = (uint16_t)(g_uart_rx_buffer[1]
                                | (g_uart_rx_buffer[2] << 8));

            g_urat_rx_ctrl = (g_urat_rx_ctrl == RFET_UART_RX_CTRL_NONE)
                                 ? RFET_UART_RX_CTRL_WAIT_DATA
                                 : g_urat_rx_ctrl;

            /* check how many length should wait */
            if ((rx_len < (g_uart_rx_length - RFET_CMD_COMMON_HEADER_SIZE
                           - RFET_CMD_COMMON_HEADER_LEN_SIZE))
                || (rx_len
                    > (RFET_UART_BUFFER_MAX_SIZE - RFET_CMD_COMMON_OVERHEAD_SIZE
                       - RFET_CMD_COMMON_CRC_LEN_SIZE))) {

                /* ERROR 1, received more than expected, no multi command accepted, so this should not be happened */
                rfet_uart_rx_buffer_init();
                g_urat_rx_ctrl = RFET_UART_RX_CTRL_NONE;

            } else {

                rx_remaining_len = rx_len
                                   - (g_uart_rx_length
                                      - RFET_CMD_COMMON_HEADER_SIZE
                                      - RFET_CMD_COMMON_HEADER_LEN_SIZE);

                /* check command unitl no remaining data to be recied */
                if (rx_remaining_len == 0) {

                    g_urat_rx_ctrl = RFET_UART_RX_CTRL_CMD_EXECUTE;

                    /* Ready to abort CLI under this situation */
                    if (rfet_command_validation_check(
                            (RFET_CMD_HEADER)g_uart_rx_buffer[0],
                            g_uart_rx_buffer)
                        != RFET_CMD_HDLR_END) {
                        uart_getch_abort();
                    }

#if (RFET_UART_RX_TASK_PHASE == 0)
                    cmd_idx = rfet_command_validation_check(
                        (RFET_CMD_HEADER)g_uart_rx_buffer[0], g_uart_rx_buffer);

                    /* handle command */
                    if (cmd_idx != RFET_CMD_HDLR_END) {
                        g_rfet_command_handler[cmd_idx].cmd_hdlr(
                            g_uart_rx_buffer, g_uart_rx_length);
                    }

                    /* clear UART buffer no matter we get the cmd_idx or not */
                    rfet_uart_rx_buffer_init();
#endif
                }
            }

        } else if (g_uart_rx_length == RFET_CMD_DTM_TEST_END_SIZE) {
            if (rfet_uart_check_dtm_end() == true) {
                rfet_uart_rx_buffer_init();
                rfet_uart_tx_dtm_end_event();
            }
        }

        return RFET_UART_RX_DISPATCH_CMD_MODE;
    }
}

void rfet_uart_rx_data_cmd_handle(void) {

    uint8_t cmd_idx;

    if (g_urat_rx_ctrl == RFET_UART_RX_CTRL_CMD_EXECUTE) {

        g_urat_rx_ctrl = RFET_UART_RX_CTRL_NONE;

        cmd_idx = rfet_command_validation_check(
            (RFET_CMD_HEADER)g_uart_rx_buffer[0], g_uart_rx_buffer);

        /* handle command */
        if (cmd_idx != RFET_CMD_HDLR_END) {
            g_rfet_command_handler[cmd_idx].cmd_hdlr(g_uart_rx_buffer,
                                                     g_uart_rx_length);
        }

        /* clear UART buffer no matter we get the cmd_idx or not */
        rfet_uart_rx_buffer_init();
    }
}

extern bool g_gui_dtm_task;

void rfet_command_event_pacakge(uint8_t para_len, uint8_t* para) {
    g_uart_tx_buffer[0] = 0xE0;
    g_uart_tx_buffer[1] = para_len; /* LSB with CRC */
    g_uart_tx_buffer[2] = 0;        /* MSB */
    memcpy(g_uart_tx_buffer + 3, para, para_len);

    rfet_uart_tx_buf(para_len + RFET_CMD_COMMON_OVERHEAD_SIZE, true);
}

/* RFET Mode Switch Command 0x81 RFET_CMD_MODE_SWITCH_GUI */
void rfet_command_mode_switch(uint8_t* rx_data_ptr, uint16_t rx_len) {
    uint8_t fw_mode;
    uint8_t status = RFET_EVENT_RET_STATUS_SUCCESS;
    uint8_t ret[2];

    ret[0] = rx_data_ptr[0];

    /* check command header */
    switch (rx_data_ptr[0]) {
        case RFET_CMD_MODE_SWITCH_GUI:
            g_rfet_mode_control = RFET_MODE_CONTROL_GUI;
            g_gui_dtm_task = false;
            break;
        case RFET_CMD_MODE_SWITCH_UART_BRIDGE:
            g_rfet_mode_control = RFET_MODE_CONTROL_UART_BRIDGE;
            break;
        case RFET_CMD_MODE_SWITCH_HOST:
            g_rfet_mode_control = RFET_MODE_CONTROL_HOST;
            break;
        default:
            //do nothing or rollback to CLI
            status |= 0x01;
            break;
    }

    if (status == 0) {

        /* check FW mode*/
        switch (rx_data_ptr[RFET_CMD_COMMON_HEADER_SIZE
                            + RFET_CMD_COMMON_HEADER_LEN_SIZE]) {
            case RFET_FW_CONTROL_RUCI_CMD:

#if ((RF_MCU_CONST_LOAD_SUPPORTED == 0)                                        \
     || (RF_MCU_CHIP_MODEL != RF_MCU_CHIP_569M0))
                fw_mode = RF_FW_LOAD_SELECT_RUCI_CMD;
#else
                fw_mode = RF_FW_LOAD_SELECT_INTERNAL_TEST;
#endif

                break;
            case RFET_FW_CONTROL_BLE_CONTROLLER:

                fw_mode = RF_FW_LOAD_SELECT_BLE_CONTROLLER;

                break;
            case RFET_FW_CONTROL_PARA_NONE:
                if (g_rfet_mode_control != RFET_MODE_CONTROL_HOST) {
                    status |= 0x02; /* wrong para */
                }
                break;
            default:
                //do nothing
                status |= 0x02; /* wrong para */
                break;
        }

        if (status == 0) {

            if (g_rfet_mode_control == RFET_MODE_CONTROL_GUI) {
                status = (rfet_gui_init(fw_mode) == true) ? status : status | 4;
            } else if (g_rfet_mode_control == RFET_MODE_CONTROL_UART_BRIDGE) {
                status = (rfet_uart_bypass_init(fw_mode) == true) ? status
                                                                  : status | 4;
            }
#if (RFET_HOST_ENABLE == 1)
            else if (g_rfet_mode_control == RFET_MODE_CONTROL_HOST) {
                status = (rfet_host_mode_init() == true) ? status : status | 4;
            }
#endif

            uart_getch_abort();
        }
    }

    ret[1] = status;

    /* return event */
    rfet_command_event_pacakge(2, ret);
}

/* Handling UART RX command */
bool rfet_command_crc_check(uint8_t* rx_data_ptr) {

    uint16_t i;
    uint16_t count_crc_len;
    uint32_t crc_result = 0;

    count_crc_len = (uint16_t)(rx_data_ptr[1] | (rx_data_ptr[2] << 8));
    count_crc_len += (RFET_CMD_COMMON_HEADER_SIZE
                      + RFET_CMD_COMMON_HEADER_LEN_SIZE - 1);

    /* not inculde CRC */
    for (i = 0; i < count_crc_len; i++) {
        crc_result += rx_data_ptr[i];
    }

    if ((uint8_t)(crc_result & 0xFF) == rx_data_ptr[count_crc_len]) {
        return true;
    }

    return false;
}

void rfet_command_crc_add(uint8_t* rx_data_ptr, uint16_t buf_len) {
    uint8_t crc = 0;
    uint16_t i;

    for (i = 0; i < buf_len; i++) {
        crc += rx_data_ptr[i];
    }

    rx_data_ptr[buf_len] = crc;
}

uint8_t rfet_command_validation_check(RFET_CMD_HEADER in_cmd_header,
                                      uint8_t* rx_data_ptr) {
    uint8_t idx = 0;
    uint16_t rx_len;

    while (g_rfet_command_handler[idx].cmd_header != RFET_CMD_HDLR_END) {
        if (in_cmd_header == g_rfet_command_handler[idx].cmd_header) {

            /* check length */
            /*
            * if the length in table is 0, it represents the length is variable
            * only check the CRC if valid
            */
            if (g_rfet_command_handler[idx].cmd_length != 0) {
                rx_len = (uint16_t)(rx_data_ptr[1] | (rx_data_ptr[2] << 8));
                if (rx_len != g_rfet_command_handler[idx].cmd_length) {
                    /* wrong length */
                    /* ERROR 1, received more than expected */
                    /* Error Handling, need to search next header???? */
                    break;
                }
            }

            /* check CRC */
            if (rfet_command_crc_check(rx_data_ptr) != true) {
                /* wrong CRC */
                break;
            }

            /* return index of callback function */
            return idx;
        }

        idx++;
    }

    return RFET_CMD_HDLR_END;
}

void rfet_disable_sysyick_isr(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    NVIC_DisableIRQ(SysTick_IRQn);
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
}

void rfet_enter_sleep_by_io(void) {
    uint32_t pin_value;
    gpio_pin_get(GPIO_TEST, &pin_value);
    if (g_gui_enable_gpio_sleep && (pin_value == SLEEP_TRIGGER_VALUE)) {
#if (RF_TX_POWER_COMP)
        Tx_Power_Compensation_Deinit();
        sadc_disable();
#endif
        rfet_disable_sysyick_isr();
        pin_set_pullopt(GPIO_TEST, HOSAL_PULL_DOWN_100K);
        lpm_set_low_power_level(LOW_POWER_LEVEL_SLEEP0);
        lpm_low_power_unmask(LOW_POWER_MASK_BIT_RESERVED31);
        /* suspend until no more event */
        vTaskSuspend(xrf_rfet_taskHandle);
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
static void rfet_task(void* parameters_ptr) {
    while (1) {
#if (RFET_UART_RX_TASK_PHASE == 1)
        /* check UART RX command */
        rfet_uart_rx_data_cmd_handle();
#endif

        if (g_rfet_mode_control == RFET_MODE_CONTROL_CLI) {
            /* default run CLI task */
            rfet_cli_task();
        } else if (g_rfet_mode_control == RFET_MODE_CONTROL_GUI) {
            rfet_gui_task();
        } else if (g_rfet_mode_control == RFET_MODE_CONTROL_UART_BRIDGE) {
            rfet_uart_bypass_task();
        }
#if (RFET_HOST_ENABLE == 1)
        else if (g_rfet_mode_control == RFET_MODE_CONTROL_HOST) {
            rfet_host_task();
        }
#endif
        else {
            ; // do nothing
        }

        rfet_enter_sleep_by_io();
    }
}

void rfet_init(void) {
    if (g_rfet_mode_control == RFET_MODE_CONTROL_GUI) {
        rfet_uart_rx_buffer_init();
        //uart_uninit(CHOOSE_RFET_UART); //equivalence of hosal_uart.c?
        dtm_uart_deinit();
        //printf("rfet_init");
        rfet_uart_init();
    } else {
        //console_drv_init(PRINTF_BAUDRATE);
    }

    xTaskCreate(rfet_task, "TASK_RFET", 1024, NULL, E_TASK_PRIORITY_APP,
                &xrf_rfet_taskHandle);
    vTaskSuspend(xrf_dtm_taskHandle);
}
