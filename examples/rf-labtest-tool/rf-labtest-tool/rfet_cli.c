/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet_cli.c
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
#include "rf_common_init.h"
#include "dtm_mode.h"
#include "rfet.h"
#include "rfet_uart_bypass.h"
//#include "retarget.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    Internal Functions
 *************************************************************************************************/
/* prompt of mode select */
void rfet_cli_rf_test_rf_mode_select_prompt(void);
/* prompt of channel select */
void rfet_cli_rf_test_channel_select_prompt(void);
/* prompt of data rate select */
void rfet_cli_rf_test_data_rate_select_prompt(void);
/* prompt of tx power adjust */
void rfet_cli_rf_test_tx_power_adjust_prompt(void);
/* prompt of sync word change */
void rfet_cli_rf_test_sync_word_change_prompt(void);
/* prompt of TX control select */
void rfet_cli_rf_test_tx_control_prompt(void);
/* prompt of RX control select */
void rfet_cli_rf_test_rx_control_prompt(void);

/* Layer rf_test, option 1 */
void rfet_cli_rf_test_rf_mode_select(uint8_t *para_ptr);
/* Layer rf_test, option 2 */
void rfet_cli_rf_test_channel_select(uint8_t *para_ptr);
/* Layer rf_test, option 3 */
void rfet_cli_rf_test_data_rate_select(uint8_t *para_ptr);
/* Layer rf_test, option 4 */
void rfet_cli_rf_test_tx_power_adjust(uint8_t *para_ptr);
/* Layer rf_test, option 5 */
void rfet_cli_rf_test_sync_word_change(uint8_t *para_ptr);
/* Layer rf_test, option 6 */
void rfet_cli_rf_test_tx_control(uint8_t *para_ptr);
/* Layer rf_test, option 7 */
void rfet_cli_rf_test_rx_control(uint8_t *para_ptr);
/* Layer 0, option 2 */
void rfet_cli_enter_rf_test(uint8_t *para_ptr);
/* Layer 0, option 1 */
void rfet_cli_enter_dtm_check(uint8_t *para_ptr);

extern RFET_MODE_SWITCH    g_rfet_mode_control;
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef void (*RFET_CLI_HDLR)(uint8_t *para);
typedef void (*RFET_CLI_PROMPT)(void);

typedef struct _rfet_cli_desc_
{
    uint8_t cmd_option;
    RFET_CLI_HDLR cmd_hdlr;
    RFET_CLI_PROMPT prompt_func;
} s_rfet_cli_cmd_t, *p_rfet_cli_cmd_t;


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
/* Maximum char number of input */
#define RFET_CLI_STRING_MAX_SIZE                            (10)

/* Flag to show get char */
#define RFET_CLI_SHOW_CH                                    (true)
#define RFET_CLI_NO_SHOW_CH                                 (false)

/* CLI command layers boundary */
#define RFET_CLI_CMD_LAYER_0                                (0)
#define RFET_CLI_CMD_LAYER_1                                (1)
#define RFET_CLI_CMD_LAYER_END                              (0xFF)

/* RF channel boundary */
#define RFET_CLI_CMD_RF_CHANNEL_UP_LIMIT                    (39)
#define RFET_CLI_CMD_RF_CHANNEL_DOWN_LIMIT                  (0)
#define RFET_CLI_CMD_RF_ZIGBEE_CHANNEL_UP_LIMIT             (26)
#define RFET_CLI_CMD_RF_ZIGBEE_CHANNEL_DOWN_LIMIT           (11)
/*RF WISUN channel boundary which frequency range is between 868MHz to 870MHz*/
#define RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_UP_LIMIT          (33)
#define RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_DOWN_LIMIT        (25)
#define RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_UP_LIMIT         (16)
#define RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_DOWN_LIMIT       (12)
#define RFET_CLI_CMD_RF_WISUN_50K_CENTER_FREQ               (863125)
#define RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_SPACING           (200)
#define RFET_CLI_CMD_RF_WISUN_100K_CENTER_FREQ              (863225)
#define RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_SPACING          (400)

/* RX auto report interval boundary */
#define RFET_CLI_CMD_RX_CONTROL_AUTO_REPORT_UP_LIMIT        (10)
#define RFET_CLI_CMD_RX_CONTROL_AUTO_REPORT_DOWN_LIMIT      (1)

/* TX payload boundary */
#define RFET_CLI_CMD_TX_CONTROL_BLE_PAYLOAD_LEN_UP_LIMIT            (255)
#define RFET_CLI_CMD_TX_CONTROL_ZIGBEE_PAYLOAD_LEN_UP_LIMIT         (125)
#define RFET_CLI_CMD_TX_CONTROL_WISUN_50K_PAYLOAD_LEN_UP_LIMIT      (291)
#define RFET_CLI_CMD_TX_CONTROL_WISUN_100K_PAYLOAD_LEN_UP_LIMIT     (597)
#define RFET_CLI_CMD_TX_CONTROL_PAYLOAD_LEN_DOWN_LIMIT      (1)

/* TX count boundary */
#define RFET_CLI_CMD_TX_CONTROL_COUNT_UP_LIMIT              (0xFFFF)
#define RFET_CLI_CMD_TX_CONTROL_COUNT_DOWN_LIMIT            (1)

/* TX timing control */
#define RFET_CLI_CMD_TX_MISC_OFFSET                         (130)

/* BLE data rate 4M enable */
#define BLE_4M_EN                                           (0)

/* command group define */
typedef uint8_t RFET_CLI_CMD_GROUP;
#define RFET_CLI_CMD_GROUP_LAYER_0                          ((RFET_CLI_CMD_GROUP)(0x00))
#define RFET_CLI_CMD_GROUP_RF_TEST                          ((RFET_CLI_CMD_GROUP)(0x01))
#define RFET_CLI_CMD_GROUP_RF_TEST_MODE                     ((RFET_CLI_CMD_GROUP)(0x01))
#define RFET_CLI_CMD_GROUP_RF_TEST_CHANNEL                  ((RFET_CLI_CMD_GROUP)(0x02))
#define RFET_CLI_CMD_GROUP_RF_TEST_DATA_RATE                ((RFET_CLI_CMD_GROUP)(0x03))
#define RFET_CLI_CMD_GROUP_RF_TEST_TX_POWER                 ((RFET_CLI_CMD_GROUP)(0x04))
#define RFET_CLI_CMD_GROUP_RF_TEST_SYNC_WORD_CHANGE         ((RFET_CLI_CMD_GROUP)(0x05))
#define RFET_CLI_CMD_GROUP_RF_TEST_TX_CONTROL               ((RFET_CLI_CMD_GROUP)(0x06))
#define RFET_CLI_CMD_GROUP_RF_TEST_RX_CONTROL               ((RFET_CLI_CMD_GROUP)(0x07))
#define RFET_CLI_CMD_GROUP_FUNC_DONE                        ((RFET_CLI_CMD_GROUP)(0xFF))

/* mode select */
typedef uint8_t RFET_CLI_CMD_RF_TEST_MODE;
#define RFET_CLI_CMD_RF_TEST_MODE_BLE                       ((RFET_CLI_CMD_RF_TEST_MODE)(0x00))
#define RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE                    ((RFET_CLI_CMD_RF_TEST_MODE)(0x01))
#define RFET_CLI_CMD_RF_TEST_MODE_WISUN                     ((RFET_CLI_CMD_RF_TEST_MODE)(0x02))

/* data rate */
typedef uint8_t RFET_CLI_CMD_RF_TEST_DATA_RATE;
#define RFET_CLI_CMD_RF_TEST_DATA_RATE_1M                   ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x00))
#define RFET_CLI_CMD_RF_TEST_DATA_RATE_2M                   ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x01))
#define RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S2             ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x02))
#define RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S8             ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x03))
#if (BLE_4M_EN)
#define RFET_CLI_CMD_RF_TEST_DATA_RATE_4M                   ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x04))
#endif

/* data rate */
typedef uint8_t RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE;
#define RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K            ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x00))
#define RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K           ((RFET_CLI_CMD_RF_TEST_DATA_RATE)(0x01))

/* tx power */
typedef uint8_t RFET_CLI_CMD_RF_TEST_TX_POWER;

/* rx control */
typedef uint8_t RFET_CLI_CMD_RF_TEST_RX_CONTROL;
#define RFET_CLI_CMD_RF_TEST_RX_CONTROL_END_REPORT          ((RFET_CLI_CMD_RF_TEST_RX_CONTROL)(0x00))
#define RFET_CLI_CMD_RF_TEST_RX_CONTROL_AUTO_REPORT         ((RFET_CLI_CMD_RF_TEST_RX_CONTROL)(0x01))

/* tx control */
typedef uint8_t RFET_CLI_CMD_RF_TEST_TX_PAYLOAD;
#define RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_PRBS9       ((RFET_CLI_CMD_RF_TEST_TX_PAYLOAD)(0x00))
#define RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_0XF0        ((RFET_CLI_CMD_RF_TEST_TX_PAYLOAD)(0x01))
#define RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_0XAA        ((RFET_CLI_CMD_RF_TEST_TX_PAYLOAD)(0x02))

typedef uint8_t RFET_CLI_CMD_RF_DIRECTION_MODE;
#define RFET_CLI_CMD_RF_DIRECTION_TX_MODE                   ((RFET_CLI_CMD_RF_DIRECTION_MODE)0x01)
#define RFET_CLI_CMD_RF_DIRECTION_RX_MODE                   ((RFET_CLI_CMD_RF_DIRECTION_MODE)0x02)

#define RFET_CLI_CMD_GETCH_EARLY_ABORT                      (0xFF)

#define RFET_CLI_DTM_ENABLE_UART_BRIDGE_CMD                 (0)


/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
bool g_load_fw_flag = false;

bool g_uart_rx_flag = false;
volatile bool g_uart_getch_wait_abort = false;

/* global for UART processing */
uint8_t g_char = 0;
uint8_t g_fw_option = 1;
uint8_t g_rx_string_buf_count = 0;
uint8_t g_rx_string_buf[RFET_CLI_STRING_MAX_SIZE];

/* global for RF control configurations */
RFET_CLI_CMD_RF_TEST_MODE g_rf_test_mode = RFET_CLI_CMD_RF_TEST_MODE_BLE;
RFET_CLI_CMD_RF_TEST_DATA_RATE g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_1M;
RFET_CLI_CMD_RF_TEST_TX_POWER g_rf_test_tx_power;
RFET_CLI_CMD_RF_TEST_RX_CONTROL g_rf_test_rx_control = RFET_CLI_CMD_RF_TEST_RX_CONTROL_END_REPORT;
uint16_t g_rf_test_channel = 0;
uint32_t g_rf_test_sync_word = 0x71764129;

/* global for RX control */
uint8_t g_rf_test_rx_report_interval = 1;
uint16_t g_rf_test_tx_ht_interval = 20;  /* defalut is 10ms, 1ms is for 4Mbps only*/
uint32_t g_rf_test_rx_report_tx_count_for_lost_rate = 0;
uint32_t g_rf_test_rx_total_crc_success = 0;
uint32_t g_rf_test_rx_total_crc_fail = 0;
uint32_t g_rf_test_rx_total_sync_fail = 0;
uint32_t g_rf_test_rx_total_tx_expect_cnt = 0;
uint32_t g_rf_test_rx_interval_cnt = 0;

/* global for TX control */
RFET_CLI_CMD_RF_TEST_TX_PAYLOAD g_rf_test_tx_payload = RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_PRBS9;
uint16_t g_rf_test_tx_payload_len = 10;
uint16_t g_rf_test_tx_count = 1000;
uint16_t g_rf_test_tx_interval = 100;

uint8_t g_rf_test_crc_type;
/* command group rf_test */
s_rfet_cli_cmd_t const c_rfet_cli_rf_test_handler[] =
{
    /* Start */{0, 0, 0},
    {RFET_CLI_CMD_GROUP_RF_TEST_MODE, &rfet_cli_rf_test_rf_mode_select, &rfet_cli_rf_test_rf_mode_select_prompt},
    {RFET_CLI_CMD_GROUP_RF_TEST_CHANNEL, &rfet_cli_rf_test_channel_select, &rfet_cli_rf_test_channel_select_prompt},
    {RFET_CLI_CMD_GROUP_RF_TEST_DATA_RATE, &rfet_cli_rf_test_data_rate_select, &rfet_cli_rf_test_data_rate_select_prompt},
    {RFET_CLI_CMD_GROUP_RF_TEST_TX_POWER, 0, 0},
    {RFET_CLI_CMD_GROUP_RF_TEST_SYNC_WORD_CHANGE, &rfet_cli_rf_test_sync_word_change, &rfet_cli_rf_test_sync_word_change_prompt},
    {RFET_CLI_CMD_GROUP_RF_TEST_TX_CONTROL, &rfet_cli_rf_test_tx_control, &rfet_cli_rf_test_tx_control_prompt},
    {RFET_CLI_CMD_GROUP_RF_TEST_RX_CONTROL, &rfet_cli_rf_test_rx_control, &rfet_cli_rf_test_rx_control_prompt},
    /* End */
    {RFET_CLI_CMD_LAYER_END, 0, 0}

};

/* command table pointer for RF test */
s_rfet_cli_cmd_t const *g_rfet_cli_rf_test_handler = c_rfet_cli_rf_test_handler;

/* command group 0 */
s_rfet_cli_cmd_t const c_rfet_cli_layer_0_handler[] =
{
    /* Start */{0, 0, 0},
#if 0
    {1, &rfet_cli_enter_dtm_check, 0},
    {2, &rfet_cli_enter_rf_test, 0},
#else
    {1, &rfet_cli_enter_rf_test, 0},
#endif
    /* End */{RFET_CLI_CMD_LAYER_END, 0, 0}

};

/* command table pointer for layer 0 */
s_rfet_cli_cmd_t const *g_rfet_cli_layer_0_handler = c_rfet_cli_layer_0_handler;

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* This function is called when platform UART RX interrup raised */
void uart_rx_data_update(uint8_t ch)
{
    g_char = ch;
    g_uart_rx_flag = true;
}

void uart_getch_abort(void)
{
    g_uart_getch_wait_abort = true;
}

/* Single byte input wait and get and display */
uint8_t uart_getch(bool is_show)
{

    if (g_rfet_mode_control == RFET_MODE_CONTROL_GUI)
    {
        while (g_uart_rx_flag == false && g_uart_getch_wait_abort == false);
    }
    else
    {
        g_char = getchar();
    }

    /* early abort */
    if (g_uart_getch_wait_abort == true)
    {
        return RFET_CLI_CMD_GETCH_EARLY_ABORT;
    }

    if (is_show == true)
    {
        printf("%c", g_char);
    }

    g_uart_rx_flag = false;

    return g_char;
}

/* Single byte input get check, check and leave */
bool uart_check_getch(uint8_t *get_ch, bool is_show)
{
    if (g_rfet_mode_control == RFET_MODE_CONTROL_GUI)
    {
        if (g_uart_rx_flag == false)
        {
            *get_ch = 0;
            return false;
        }
    }
    else
    {
#if 0
        /* TBD, replace check_uart0_rx_buffer_data_num */
        if (check_uart0_rx_buffer_data_num() == 0)
        {
            return false;
        }
#endif
        g_char = getchar();
    }

    if (is_show == true)
    {
        printf("%c", g_char);
    }

    *get_ch = g_char;
    g_uart_rx_flag = false;

    return true;

}

/* Transfer ASCII char to 0~9 number in decimal */
uint8_t uart_ch_to_num(uint8_t ch, bool is_hex)
{

    if ((ch >= '0') && (ch <= '9'))
    {
        return (ch - 0x30);
    }

    if (is_hex == true)
    {
        /* check A(a) ~ F(f) */
        /* direct return value from 0x00 ~ 0x0F */
        if ((ch >= 'A') && (ch <= 'F'))
        {
            /* ex: 'A' = 0x41, return 10 */
            return (ch - 55);
        }
        else if ((ch >= 'a') && (ch <= 'f'))
        {
            /* ex: 'a' = 0x61, return 10 */
            return (ch - 87);
        }
    }

    return 0xFF;
}

/* Clear UART RX buf of multi char */
void uart_rx_string_clear(void)
{
    g_rx_string_buf_count = 0;
    memset(g_rx_string_buf, 0, RFET_CLI_STRING_MAX_SIZE);
}

/* Multiple byte input get and display */
uint8_t uart_get_string(bool is_show, bool is_hex)
{

    bool exit = false;

    uart_rx_string_clear();

    while (exit == false)
    {

        if (g_rfet_mode_control == RFET_MODE_CONTROL_GUI)
        {
            /* wait UART RX Interrupt */
            while (g_uart_rx_flag == false);
        }
        else
        {
            g_char = getchar();
        }


        if (is_show == true)
        {
            printf("%c", g_char);
        }

        /* check if Enter key pressed */
        if (g_char == 0x0D)
        {
            exit = true;
        }
        /* only number is allowed, leave if not number key */
        else if (uart_ch_to_num(g_char, is_hex) == 0xFF)
        {
            exit = true;
            g_rx_string_buf_count = 0xFF;
        }
        else
        {

            /* collect string data */
            g_rx_string_buf[g_rx_string_buf_count] = uart_ch_to_num(g_char, is_hex);
            g_rx_string_buf_count++;

            /* force to leave if length exceeds limitation */
            if (g_rx_string_buf_count == RFET_CLI_STRING_MAX_SIZE)
            {
                exit = true;
            }
        }

        g_uart_rx_flag = false;

    }

    return g_rx_string_buf_count;

}



/* transfer decimal string into decimal value */
uint32_t uart_string_to_value(bool to_hex)
{

    uint8_t i = 0;
    uint32_t ret = 0;
    uint32_t multi = 1;

    /* length */
    /* the length is limited to RFET_CLI_STRING_MAX_SIZE*/

    /* buf ptr */
    /* digis is the last one in the array with valid length */

    if (g_rx_string_buf_count == 0)
    {
        return 0;
    }

    if (to_hex == true && g_rx_string_buf_count > 8)
    {
        /* check if the hex string over 32bit */
        return 0;
    }

    while (i < g_rx_string_buf_count)
    {

        if (to_hex == false)
        {
            ret += (g_rx_string_buf[g_rx_string_buf_count - 1 - i] * multi);
            multi *= 10;
        }
        else
        {
            ret += (g_rx_string_buf[g_rx_string_buf_count - 1 - i] * multi);
            multi *= 16;
        }
        i++;
    }

    return ret;

}

/* prompt of mode select */
void rfet_cli_rf_test_rf_mode_select_prompt(void)
{

    printf("\r\n+-------------------------------------------------+");
    printf("\r\n|                  RF Mode Select                 |");
    printf("\r\n+=================================================+");
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        printf("\r\n| RF Mode Select: Now:[BLE]                       |");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {
        printf("\r\n| RF Mode Select: Now:[ZIGBEE]                    |");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        printf("\r\n| RF Mode Select: Now:[WISUN]                    |");
    }
    else
    {
        printf("\r\n| RF Mode Select: Now:[None]                      |");
    }
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [1]. BLE                                        |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [2]. ZIGBEE                                     |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [3]. WISUN                                      |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [R]. Return                                     |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n:");

}

/* prompt of channel select */
void rfet_cli_rf_test_channel_select_prompt(void)
{
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n|                                      Channel Select                                     |");
        printf("\r\n+=========================================================================================+");
        printf("\r\n| Current Setting: [");
        printf("%4dMHz]                                                              |", (g_rf_test_channel * 2) + 2402);
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Please input channel index to setup channel, ex: 0 = 2402Mhz, 1 = 2404MHz, 39 = 2480MHz |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Channel index from 0 ~ 39 with 2MHz bandwidth.                                          |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Press Enter to use input value or any non-number character to leave                     |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n:");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n|                                      Channel Select                                     |");
        printf("\r\n+=========================================================================================+");
        printf("\r\n| Current Setting: [");
        printf("%4dMHz]                                                              |", (g_rf_test_channel * 5) + 2350);
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Please input channel index to setup channel, ex: 11 = 2405Mhz, ..., 26 = 2480MHz        |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Channel index from 11 ~ 26 with 5MHz bandwidth.                                          |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Press Enter to use input value or any non-number character to leave                     |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n:");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n|                                      Channel Select                                     |");
        printf("\r\n+=========================================================================================+");
        printf("\r\n| Current Setting: [");
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
        {
            printf("%4dMHz]                                                              |", (g_rf_test_channel * (g_rf_test_data_rate * RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_SPACING)) + RFET_CLI_CMD_RF_WISUN_50K_CENTER_FREQ);
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K)
        {
            printf("%4dMHz]                                                              |", (g_rf_test_channel * (g_rf_test_data_rate * RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_SPACING)) + RFET_CLI_CMD_RF_WISUN_100K_CENTER_FREQ);
        }
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Please input channel index to setup channel                                             |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Channel index  for 50kHz is 25 ~ 33 with 200kHz bandwidth. (868125kHz ~ 869725kHz)      |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Channel index  for 100kHz is 12 ~ 16 with 400kHz bandwidth. (868025kHz ~ 869625kHz)     |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n| Press Enter to use input value or any non-number character to leave                     |");
        printf("\r\n+-----------------------------------------------------------------------------------------+");
        printf("\r\n:");
    }
}

void rfet_cli_rf_test_data_rate_current_setting_prompt(void)
{

    switch (g_rf_test_data_rate)
    {
    case RFET_CLI_CMD_RF_TEST_DATA_RATE_1M:
        printf("\r\n| Current Setting: [1Mbps]                        |");
        break;
    case RFET_CLI_CMD_RF_TEST_DATA_RATE_2M:
        printf("\r\n| Current Setting: [2Mbps]                        |");
        break;
    case RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S2:
        printf("\r\n| Current Setting: [Coded S2]                     |");
        break;
    case RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S8:
        printf("\r\n| Current Setting: [Coded S8]                     |");
        break;
#if (BLE_4M_EN)
    case RFET_CLI_CMD_RF_TEST_DATA_RATE_4M:
        printf("\r\n| Current Setting: [4Mbps]                        |");
        break;
#endif
    default:
        printf("\r\n| Current Setting: [Unknown]                      |");
        break;
    }
}

void rfet_cli_rf_test_wisun_data_rate_current_setting_prompt(void)
{

    switch (g_rf_test_data_rate)
    {
    case RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K:
        printf("\r\n| Current Setting: [50kbps]                       |");
        break;
    case RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K:
        printf("\r\n| Current Setting: [100kbps]                      |");
        break;
    default:
        printf("\r\n| Current Setting: [Unknown]                      |");
        break;
    }
}
/* prompt of data rate select */
void rfet_cli_rf_test_data_rate_select_prompt(void)
{
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {

        printf("\r\n+-------------------------------------------------+");
        printf("\r\n|               RF Data Rate Select               |");
        printf("\r\n+=================================================+");
        rfet_cli_rf_test_data_rate_current_setting_prompt();
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [1]. 1Mbps                                      |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [2]. 2Mbps                                      |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [3]. Coded S2                                   |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [4]. Coded S8                                   |");
#if (BLE_4M_EN)
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [5]. 4Mbps                                      |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [6]. 4Mbps with 2ms TX interval                 |");
#endif
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [R]. Return                                     |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n:");

    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {

        printf("\r\n+-------------------------------------------------+");
        printf("\r\n|               RF Data Rate Select               |");
        printf("\r\n+=================================================+");
        printf("\r\n| Current Setting: [250kbps]                      |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [R]. Return                                     |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n:");

    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {

        printf("\r\n+-------------------------------------------------+");
        printf("\r\n|               RF Data Rate Select               |");
        printf("\r\n+=================================================+");
        rfet_cli_rf_test_wisun_data_rate_current_setting_prompt();
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [1]. 50kbps                                     |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [2]. 100kb                                      |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n| [R]. Return                                     |");
        printf("\r\n+-------------------------------------------------+");
        printf("\r\n:");

    }


}

/* TX power adjustment is not supported in T3ASIC */
void rfet_cli_rf_test_tx_power_adjust_prompt(void)
{
    ;//
}

/* prompt of sync word change */
void rfet_cli_rf_test_sync_word_change_prompt(void)
{
    printf("\r\n+---------------------------------------------------------------------------+");
    printf("\r\n|                        Synchronization Word Change                        |");
    printf("\r\n+===========================================================================+");
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        printf("\r\n| Current Setting:[0x%8X]                                              |", g_rf_test_sync_word);
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n| Please input synchronization word in hexadecimal to change, ex:0xAABBCCDD |");
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n| Press Enter to use input value or any character not hexadecimal to leave. |");
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n:0x");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        printf("\r\n| Current Setting:[0x%8X]                                              |", g_rf_test_sync_word);
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n| Please input 2bytes sync word in hexadecimal to change, ex:0xABCD         |");
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n| Press Enter to use input value or any character not hexadecimal to leave. |");
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n:0x");
    }
    else
    {
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n| Not supported in Zigbee Mode                                              |");
        printf("\r\n| Press [R] to leave.                                                       |");
        printf("\r\n+---------------------------------------------------------------------------+");
        printf("\r\n:");
    }

}

void rfet_cli_rf_test_tx_payload_prompt(void)
{
    printf("\r\n+-------------------------+");
    printf("\r\n|    TX Payload Select    |");
    printf("\r\n+=========================+");
    printf("\r\n| [1]. Payload PRBS9      |");
    printf("\r\n+-------------------------+");
    printf("\r\n| [2]. Payload 0b11110000 |");
    printf("\r\n+-------------------------+");
    printf("\r\n| [3]. Paylaod 0b10101010 |");
    printf("\r\n+-------------------------+");
    printf("\r\n| [R]. Return             |");
    printf("\r\n+-------------------------+");
    printf("\r\n:");
}

void rfet_cli_rf_test_tx_length_prompt(void)
{
    printf("\r\n+---------------------------------------------------------------------+");
    printf("\r\n|                           TX Length Input                           |");
    printf("\r\n+=====================================================================+");
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        printf("\r\n| Please input the TX length from 1 ~ 255 bytes                       |");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {
        printf("\r\n| Please input the TX length from 1 ~ 125 bytes                       |");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
        {
            printf("\r\n| Please input the TX length from 1 ~ %d bytes                       |", RFET_CLI_CMD_TX_CONTROL_WISUN_50K_PAYLOAD_LEN_UP_LIMIT);
        }
        else
        {
            printf("\r\n| Please input the TX length from 1 ~ %d bytes                      |", RFET_CLI_CMD_TX_CONTROL_WISUN_100K_PAYLOAD_LEN_UP_LIMIT);
        }
    }
    printf("\r\n+---------------------------------------------------------------------+");
    printf("\r\n| Press Enter to use input value or any character not number to leave |");
    printf("\r\n+---------------------------------------------------------------------+");
    printf("\r\n:");
}

void rfet_cli_rf_test_tx_count_prompt(void)
{
    printf("\r\n+------------------------------------------------------------------------+");
    printf("\r\n|                             TX Count Input                             |");
    printf("\r\n+========================================================================+");
    printf("\r\n| Please input the TX count from 0 ~ 65535, 65535 = non-stop TX          |");
    printf("\r\n+------------------------------------------------------------------------+");
    printf("\r\n| Press Enter to use input value or any character not number to leave    |");
    printf("\r\n+------------------------------------------------------------------------+");
    printf("\r\n:");
}

void rfet_cli_rf_test_tx_done_prompt(uint16_t tx_done_cnt)
{
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n|                    TX Status                    |");
    printf("\r\n+=================================================+");
    printf("\r\n| TX [%5d] packet is done                       |", tx_done_cnt);
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n");
    printf("\r\n");
}

/* prompt of TX control select */
void rfet_cli_rf_test_tx_control_prompt(void)
{
    printf("\r\n+-----------------------------------------------------------+");
    printf("\r\n|                         TX Control                        |");
    printf("\r\n+===========================================================+");
    printf("\r\n| [Data Payload]:|");
    if (g_rf_test_tx_payload == RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_PRBS9)
    {
        printf("[PRBS9]                                   |");
    }
    else if (g_rf_test_tx_payload == RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_0XF0)
    {
        printf("[ 0xF0]                                   |");
    }
    else if (g_rf_test_tx_payload == RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_0XAA)
    {
        printf("[ 0xAA]                                   |");
    }
    else
    {
        printf("[ None]                                   |");
    }
    printf("\r\n+-----------------------------------------------------------+");
    printf("\r\n| [Payload Length]:[%3d]                                    |", g_rf_test_tx_payload_len);
    printf("\r\n+-----------------------------------------------------------+");

    if (g_rf_test_tx_count != 0xFFFF)
    {
        printf("\r\n| [TX Count]:[  %5d]                                      |", g_rf_test_tx_count);
    }
    else
    {
        printf("\r\n| [TX Count]:[NO-STOP]                                      |");
    }
    printf("\r\n+-----------------------------------------------------------+");
    printf("\r\n| Please input the related parameter with following prompt, |");
    printf("\r\n| Press 'R' to leave TX Control at anytime.                 |");
    printf("\r\n| Press [Enter] to use current setting for TX.              |");
    printf("\r\n+-----------------------------------------------------------+");

}

void rfet_cli_rf_test_rx_control_auto_report_interval_prompt(void)
{
    printf("\r\n+----------------------------------------------------------------------------------+");
    printf("\r\n|                              RX Auto Report Interval                             |");
    printf("\r\n+==================================================================================+");
    printf("\r\n| Please input the RX Auto report interval from 1 ~ 10, unit = 1s, MAX is 10s.     |");
    printf("\r\n| Ex: 10 = 10s report interval.                                                    |");
    printf("\r\n+----------------------------------------------------------------------------------+");
    printf("\r\n| Press Enter to use input value or any character not number to leave              |");
    printf("\r\n+----------------------------------------------------------------------------------+");
    printf("\r\n:");
}

void rfet_cli_rf_test_rx_status_prompt(uint32_t crc_success_cnt, uint32_t crc_fail_cnt, uint32_t sync_fail_cnt)
{
    uint32_t lost_rate = 0;
    uint8_t digi_val = 0;
    uint8_t point_val = 0;

    if (g_rf_test_rx_report_tx_count_for_lost_rate > 0)
    {
        if (g_rf_test_rx_report_tx_count_for_lost_rate < crc_success_cnt)
        {
#if 0
            /* calculation failure */
            digi_val = 100;
#endif
            lost_rate = 0;
        }
        else
        {
            lost_rate = (g_rf_test_rx_report_tx_count_for_lost_rate - crc_success_cnt) * 10000;
        }
        digi_val = (uint8_t)((lost_rate / g_rf_test_rx_report_tx_count_for_lost_rate) / 100);
        point_val = (uint8_t)(((lost_rate / g_rf_test_rx_report_tx_count_for_lost_rate) / 10) % 10);
    }

    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n|                        RX Status                       |");
    printf("\r\n|                                                        |");
    printf("\r\n|                                         No.[%10d]|", g_rf_test_rx_interval_cnt);
    printf("\r\n+========================================================+");
    printf("\r\n| RX CRC Pass Count: [%10d]                        |", crc_success_cnt);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| RX CRC Fail Count: [%10d]                        |", crc_fail_cnt);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| RX Sync Fail Count:[%10d]                        |", sync_fail_cnt);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| RX Lost Rate:      [    %3d.%1d%%]                        |", digi_val, point_val);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| Total RX CRC Pass Count: [%10d]                  |", g_rf_test_rx_total_crc_success);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| Total RX CRC Fail Count: [%10d]                  |", g_rf_test_rx_total_crc_fail);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| Total RX Sync Fail Count:[%10d]                  |", g_rf_test_rx_total_sync_fail);
    printf("\r\n+--------------------------------------------------------+");
    printf("\r\n| Total TX Expect Packet:  [%10d]                  |", g_rf_test_rx_total_tx_expect_cnt);
    printf("\r\n+--------------------------------------------------------+");
    lost_rate = (g_rf_test_rx_total_tx_expect_cnt - g_rf_test_rx_total_crc_success) * 10000;
    digi_val = (uint8_t)((lost_rate / g_rf_test_rx_total_tx_expect_cnt) / 100);
    point_val = (uint8_t)(((lost_rate / g_rf_test_rx_total_tx_expect_cnt) / 10) % 10);
    printf("\r\n| Total RX Lost Rate:      [    %3d.%1d%%]                  |", digi_val, point_val);
    printf("\r\n+--------------------------------------------------------+");
    if (g_rf_test_rx_control == RFET_CLI_CMD_RF_TEST_RX_CONTROL_END_REPORT)
    {
        printf("\r\nPress any key to leave");
    }
    printf("\r\n:");

}

/* prompt of RX control select */
void rfet_cli_rf_test_rx_control_prompt(void)
{

    printf("\r\n+-------------------------------------------------------------+");
    printf("\r\n|                          RX Control                         |");
    printf("\r\n+=============================================================+");
    printf("\r\n| Enable RX and Set Status Report Mode:                       |");
    printf("\r\n+-------------------------------------------------------------+");
    printf("\r\n| [1]. Enable RX and report status at RX End                  |");
    printf("\r\n+-------------------------------------------------------------+");
    printf("\r\n| [2]. Enable RX and set auto report mode with given interval |");
    printf("\r\n+-------------------------------------------------------------+");
    printf("\r\n| [R]. Return                                                 |");
    printf("\r\n+-------------------------------------------------------------+");
    printf("\r\n:");

}

/* prompt of RF Test main options */
void rfet_cli_rf_test_prompt(void)
{

    printf("\r\n+-------------------------------------------------+");
    printf("\r\n|                                                 |");
    printf("\r\n|        RF Transmitting and Receiving Test       |");
    printf("\r\n|                                                 |");
    printf("\r\n+=================================================+");
    printf("\r\n| Current Setting:                                |");
    printf("\r\n+------------------------+------------------------+");
    printf("\r\n| RF Mode:[");

    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        printf("BLE]          | ");
        printf("Channel:[%4dMHz]      |", g_rf_test_channel * 2 + 2402);
        printf("\r\n+------------------------+------------------------+");
        printf("\r\n| Data Rate:[");

        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_1M)
        {
            printf("1Mbps]      | ");
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_2M)
        {
            printf("2Mbps]      | ");
        }
#if (BLE_4M_EN)
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_4M)
        {
            printf("4Mbps]      | ");
        }
#endif
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S2)
        {
            printf("Coded S2]   | ");
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S8)
        {
            printf("Coded S8]   | ");
        }
        else
        {
            printf("None]       | ");
        }

    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {
        printf("ZIGBEE]       | ");

        printf("Channel:[%4dMHz]      |", g_rf_test_channel * 5 + 2350);
        printf("\r\n+------------------------+------------------------+");
        printf("\r\n| Data Rate:[");
        printf("250kbps]    | ");
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        printf("WISUN]        | ");
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
        {
            printf("Channel:[%4dMHz]    |", (g_rf_test_channel * RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_SPACING + RFET_CLI_CMD_RF_WISUN_50K_CENTER_FREQ));
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K)
        {
            printf("Channel:[%4dMHz]    |", (g_rf_test_channel * RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_SPACING + RFET_CLI_CMD_RF_WISUN_100K_CENTER_FREQ));
        }
        printf("\r\n+------------------------+------------------------+");
        printf("\r\n| Data Rate:[");

        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
        {
            printf("50kbps]     | ");
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K)
        {
            printf("100kbps]    | ");
        }
        else
        {
            printf("None]       | ");
        }
    }
    else
    {
        printf("NONE]         | ");
    }

    printf("TX Power:[7dBm]        |");
    printf("\r\n+------------------------+------------------------+");
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        printf("\r\n| Sync Word:[0x%X] |                        |", g_rf_test_sync_word);
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        printf("\r\n| Sync Word:[0x%X]     |                        |", g_rf_test_sync_word);
    }
    printf("\r\n+------------------------+------------------------+");

    printf("\r\n| [1]. RF Mode Select                             |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [2]. Channel Select                             |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [3]. Data Rate Select                           |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [4]. TX Power Adjust(Not Supported Yet)         |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [5]. Sync Word Change                           |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [6]. TX Control                                 |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [7]. RX Control                                 |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| [R]. Return                                     |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n:");

}

/* prompt of CLI default options */
void rfet_cli_layer0_control_prompt(void)
{
    printf("\r\n:");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n| RafaelMicro Evaluation Tool Suite (ver ");
    printf("AR_%d.%d.%d)|", g_rfet_version / 100, ((g_rfet_version / 10) % 10), g_rfet_version % 10);
    printf("\r\n|                                                 |");
    printf("\r\n|          Copyright 2021 RafaelMicro.inc         |");
    printf("\r\n+=================================================+");
    printf("\r\n| [1]. Enable RF Test                             |");
    printf("\r\n+-------------------------------------------------+");
    printf("\r\n:");
}

/* stop TX or RX activities */
bool rfet_cli_rf_trx_stop(uint8_t mode)
{
    bool ret = true;

    /* stop TX */
    if (mode == RFET_CLI_CMD_RF_DIRECTION_TX_MODE)
    {
        ret = dtm_set_tx_disable_cmd();
    }
    else     /* stop RX */
    {
        ret = dtm_set_rx_disable_cmd();
    }

    return ret;
}

/* start TX or RX activities */
bool rfet_cli_rf_trx_start(uint8_t mode)
{
    bool ret = true;
    uint8_t data_rate;
#if 0
    /* DEBUG MODE START */
    if (g_rf_test_mode == 0xCC)
    {
        printf("\r\nTest Set Clock Command");
        return dtm_set_clock_cmd(true);
    }
#endif

    if (g_load_fw_flag == false)
    {
        /* init RF */
        dtm_sys_common_init();
    }

    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {
        /* init mode */
        ret = dtm_ble_init_cmd();

        /* init freq */
        ret = dtm_set_rf_freq_cmd(2402 + (2 * g_rf_test_channel));

        /* init data rate and clock*/
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_1M)
        {
            ret = dtm_set_clock_cmd(RADIO_CLOCK_16MHZ);
            ret = dtm_set_phy_cmd(LE_PHY_1M);
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_2M)
        {
            ret = dtm_set_clock_cmd(RADIO_CLOCK_16MHZ);
            ret = dtm_set_phy_cmd(LE_PHY_2M);
#if (BLE_4M_EN)
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_4M)
        {
            ret = dtm_set_clock_cmd(RADIO_CLOCK_32MHZ);
            ret = dtm_set_phy_cmd(LE_PHY_2M);
#endif
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S2)
        {
            ret = dtm_set_clock_cmd(RADIO_CLOCK_16MHZ);
            ret = dtm_set_phy_cmd(LE_PHY_LE_CODED_S2);
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S8)
        {
            ret = dtm_set_clock_cmd(RADIO_CLOCK_16MHZ);
            ret = dtm_set_phy_cmd(LE_PHY_LE_CODED_S8);
        }
        else
        {
            ret = false;
        }

        /* init sync_word */
        ret = dtm_set_sfd_cmd(g_rf_test_sync_word);
    }
    /* init Zigbee*/
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {
        /* init zigbee mode */
        ret = dtm_zigbee_init_cmd();

        /* init freq */
        ret = dtm_set_rf_freq_cmd(2350 + (5 * g_rf_test_channel));
    }
    /* init WiSUN*/
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {
        /* DTM RF WiSUN Mode init */
        dtm_wisun_init_cmd();

        /* init freq */
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
        {
            ret = dtm_set_rf_freq_cmd((g_rf_test_channel * RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_SPACING) + RFET_CLI_CMD_RF_WISUN_50K_CENTER_FREQ);
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K)
        {
            ret = dtm_set_rf_freq_cmd((g_rf_test_channel * RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_SPACING) + RFET_CLI_CMD_RF_WISUN_100K_CENTER_FREQ);
        }

        /* DTM RF WiSUN Modem set */
        data_rate = (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K) ? RADIO_PHY_FSK_50K : RADIO_PHY_FSK_100K;
        dtm_wisun_set_modem_cmd(data_rate, MOD_1);

        /* DTM RF WiSUN Mac set */
        dtm_wisun_set_mac_cmd(g_rf_test_crc_type, WHITEN_DISABLE);

        /* DTM RF WiSUN preamble set */
        dtm_wisun_set_preamble_cmd(8);

        /* DTM RF WiSUN SFD set */
        dtm_wisun_set_sfd_cmd(g_rf_test_sync_word);
    }

#if 0    /* Enanle RFB debug port*/
    outp32(0x40800010, 0x77777777);
    outp32(0x4080003C, ((inp32(0x4080003C) & 0xF0FFFFFF) | 0x07000000)); //P0
    //outp32(0x4080003C, ((inp32(0x4080003C) & 0xF0FFFFFF) | 0x01000000)); //modem
#endif




    if (mode == RFET_CLI_CMD_RF_DIRECTION_TX_MODE)
    {

        ret = dtm_set_tx_enable_cmd(g_rf_test_tx_payload, g_rf_test_tx_payload, g_rf_test_tx_interval,
                                    g_rf_test_tx_payload_len, g_rf_test_tx_count);
    }
    else
    {
        g_rf_test_rx_total_crc_success = 0;
        g_rf_test_rx_total_crc_fail = 0;
        g_rf_test_rx_total_sync_fail = 0;
        g_rf_test_rx_interval_cnt = 0;
        g_rf_test_rx_total_tx_expect_cnt = 0;
        ret = dtm_set_rx_enable_cmd(g_rf_test_rx_report_interval * 10);
    }

    if (g_load_fw_flag == false)
    {
        g_load_fw_flag = true;
    }


    return ret;
}


/* Layer rf_test, option 1 */
void rfet_cli_rf_test_rf_mode_select(uint8_t *para_ptr)
{
    uint8_t ch;
    ch = uart_getch(RFET_CLI_SHOW_CH);

    *para_ptr = RFET_CLI_CMD_GROUP_RF_TEST_MODE;

    if (ch == 'R' || ch == 'r' || ch == RFET_CLI_CMD_GETCH_EARLY_ABORT)
    {
        /* to exit mode select */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
    }
    else
    {
        switch (ch)
        {
        case '1':
            g_rf_test_mode = RFET_CLI_CMD_RF_TEST_MODE_BLE;
            g_rf_test_channel = 0;
            g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_1M;
            g_rf_test_tx_ht_interval = 20;
            g_rf_test_tx_payload_len = 10;
            g_rf_test_tx_count = 1000;
            g_rf_test_sync_word = 0x71764129;
            /* to exit mode select */
            *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
            break;
        case '2':
            g_rf_test_mode = RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE;
            g_rf_test_channel = 11;
            g_rf_test_tx_payload_len = 10;
            g_rf_test_tx_ht_interval = 20;
            g_rf_test_tx_count = 1000;
            /* to exit mode select */
            *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
            break;
        case '3':
            g_rf_test_mode = RFET_CLI_CMD_RF_TEST_MODE_WISUN;
            g_rf_test_channel = RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_DOWN_LIMIT;
            g_rf_test_data_rate = RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K;
            g_rf_test_crc_type = FSK_CRC_16;
            g_rf_test_tx_payload_len = 10;
            g_rf_test_tx_ht_interval = 50;
            g_rf_test_tx_count = 1000;
            g_rf_test_sync_word = 0x00007209;
            /* to exit mode select */
            *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
            break;
        }
    }

}

/* Layer rf_test, option 2 */
void rfet_cli_rf_test_channel_select(uint8_t *para_ptr)
{
    uint8_t get_string_len;
    uint32_t get_value;

    get_string_len = uart_get_string(true, false);

    *para_ptr = RFET_CLI_CMD_GROUP_RF_TEST_CHANNEL;

    if (get_string_len == 0xFF)
    {
        /* to exit channel select */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
    }
    else
    {

        get_value = uart_string_to_value(false);

        if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
        {
            if ((get_value > RFET_CLI_CMD_RF_CHANNEL_UP_LIMIT))
            {
                /* wrong channel range */
                /* do nothing to input the channel again */
            }
            else
            {
                g_rf_test_channel = (uint16_t)(get_value & 0xFF);
                printf("\r\nChannel changed:%4dMHz", (g_rf_test_channel * 2 + 2402));

                /* to exit channel select */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
            }
        }
        else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
        {
            if ((get_value < RFET_CLI_CMD_RF_ZIGBEE_CHANNEL_DOWN_LIMIT) || (get_value > RFET_CLI_CMD_RF_ZIGBEE_CHANNEL_UP_LIMIT))
            {
                /* wrong channel range */
                /* do nothing to input the channel again */
            }
            else
            {
                g_rf_test_channel = (uint16_t)(get_value & 0xFF);
                printf("\r\nChannel changed:%4dMHz", (g_rf_test_channel * 5 + 2350));

                /* to exit channel select */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
            }
        }
        else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
        {

            if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K)
            {

                if ((get_value < RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_DOWN_LIMIT) || (get_value > RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_UP_LIMIT))
                {
                    /* wrong channel range */
                    /* do nothing to input the channel again */
                }
                else
                {
                    g_rf_test_channel = (uint16_t)(get_value & 0xFF);
                    printf("\r\nChannel changed:%4dMHz", (g_rf_test_channel * RFET_CLI_CMD_RF_WISUN_100K_CHANNEL_SPACING + RFET_CLI_CMD_RF_WISUN_100K_CENTER_FREQ));

                    /* to exit channel select */
                    *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                }
            }
            else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
            {

                if ((get_value < RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_DOWN_LIMIT) || (get_value > RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_UP_LIMIT))
                {
                    /* wrong channel range */
                    /* do nothing to input the channel again */
                }
                else
                {
                    g_rf_test_channel = (uint16_t)(get_value & 0xFF);
                    printf("\r\nChannel changed:%4dMHz", (g_rf_test_channel * RFET_CLI_CMD_RF_WISUN_50K_CHANNEL_SPACING + RFET_CLI_CMD_RF_WISUN_50K_CENTER_FREQ));

                    /* to exit channel select */
                    *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                }
            }

        }
    }

}

/* Layer rf_test, option 3 */
void rfet_cli_rf_test_data_rate_select(uint8_t *para_ptr)
{
    uint8_t ch;
    ch = uart_getch(RFET_CLI_SHOW_CH);

    *para_ptr = RFET_CLI_CMD_GROUP_RF_TEST_DATA_RATE;

    if (ch == 'R' || ch == 'r' || ch == RFET_CLI_CMD_GETCH_EARLY_ABORT)
    {
        /* to exit tx power adjust */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
    }
    else
    {
        /* revert TX interval to 10ms */
        //g_rf_test_tx_ht_interval = 20;

        if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
        {
            switch (ch)
            {
            case '1':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_1M;
                /* to exit channel select */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
            case '2':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_2M;
                /* to exit tx power adjust */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
            case '3':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S2;
                /* to exit tx power adjust */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
            case '4':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S8;
                /* to exit tx power adjust */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
#if (BLE_4M_EN)
            case '5':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_4M;
                /* to exit tx power adjust */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
            case '6':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_TEST_DATA_RATE_4M;
                g_rf_test_tx_ht_interval = 2;
                /* to exit tx power adjust */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
#endif
            }
        }
        else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
        {
            switch (ch)
            {
            case '1':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K;
                /* to exit channel select */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
            case '2':
                g_rf_test_data_rate = RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K;
                /* to exit tx power adjust */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                break;
            }
        }
    }
}

/* Layer rf_test, option 4 */
/* TX power adjustment is not supported in this version */
void rfet_cli_rf_test_tx_power_adjust(uint8_t *para_ptr)
{
    ;//
}

/* Layer rf_test, option 5 */
void rfet_cli_rf_test_sync_word_change(uint8_t *para_ptr)
{

    uint8_t get_string_len;
    uint32_t get_value;

    get_string_len = uart_get_string(true, true);

    *para_ptr = RFET_CLI_CMD_GROUP_RF_TEST_SYNC_WORD_CHANGE;

    if (get_string_len == 0xFF)
    {
        /* to exit channel select */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
    }
    else
    {
        get_value = uart_string_to_value(true);

        if (get_value != 0)
        {

            g_rf_test_sync_word = get_value;

            printf("\r\nSynchronization word changed:0x%8X", g_rf_test_sync_word);

            /* to exit access address change */
            *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
        }
    }

}

/* To count the appropriate TX interval */
uint16_t rfet_cli_rf_test_tx_interval_count(uint16_t interval_ms)
{
    uint16_t interval_us;
    uint16_t ret;

    /* input is ms*/
    if (interval_ms > 65)
    {
        /* Maximum value */
        interval_us = 65000;
    }
#if 0
    else if (interval_ms < 1)
    {
        interval_us = 1000;
    }
#endif
    else
    {
        interval_us = (interval_ms * 1000);
    }

    /* count packet time */
    ret = g_rf_test_tx_payload_len * 8;

    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {

        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_2M)
        {
            ret += 88; /* 88 = (preamble 2 +access address 4 + PHR 2 + CRC 3)*8 */
            ret /= 2;  /* 2MHz*/
#if (BLE_4M_EN)
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_4M)
        {
            ret += 88; /* 88 = (preamble 2 +access address 4 + PHR 2 + CRC 3)*8 */
            ret /= 4; /* 4MHz*/
#endif
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_1M)
        {
            ret += 80; /* 80 = (preamble 1 +access address 4 + PHR 2 + CRC 3)*8 */
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S2)
        {
            ret += 43; /* CRC+ TERM2*/
            ret *= 2; /* 500kHz*/
            ret += 376; /* preamble 80 + FEC block1 296*/
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_CODED_S8)
        {
            ret += 43; /* CRC+ TERM2+ PHR*/
            ret *= 8; /* 125kHz*/
            ret += 376; /* preamble 80 + FEC block1 296*/
        }
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {
        ret += 64; /* 64 = (Preamble 4+ SFD 1+ PHR 1+ CRC 2)*8*/
        ret *= 4; /* 250kHz*/
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)     //TBD
    {
        ret += 112; /* 112 = (Preamble 8+ SFD 2+ PHR 2+ CRC 2)*8*/
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K)
        {
            ret *= 20; /*50kHz*/
            ret += 94; /* HW delay*/
        }
        else if (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_100K)
        {
            ret *= 10; /* 100kHz*/
            ret += 45; /* HW delay*/
        }

    }
    ret += 15; /* duration from packet end to ISR*/

    if (ret > interval_us)
    {
        printf("[E] TX interval(%d) is exceed target (%d)\n", ret, interval_us);
        while (1);
    }

    /* return is microsecond*/
    return interval_us - ret;

}

bool rfet_cli_check_tx_length(uint32_t get_value)
{
    uint16_t wisun_tx_length_upbound = (g_rf_test_data_rate == RFET_CLI_CMD_RF_WISUN_TEST_DATA_RATE_50K) ? RFET_CLI_CMD_TX_CONTROL_WISUN_50K_PAYLOAD_LEN_UP_LIMIT : RFET_CLI_CMD_TX_CONTROL_WISUN_100K_PAYLOAD_LEN_UP_LIMIT;
    if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_BLE)
    {

        if ((get_value > RFET_CLI_CMD_TX_CONTROL_BLE_PAYLOAD_LEN_UP_LIMIT) ||
                (get_value < RFET_CLI_CMD_TX_CONTROL_PAYLOAD_LEN_DOWN_LIMIT))
        {
            return false;
        }

    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_ZIGBEE)
    {

        if ((get_value > RFET_CLI_CMD_TX_CONTROL_ZIGBEE_PAYLOAD_LEN_UP_LIMIT) ||
                (get_value < RFET_CLI_CMD_TX_CONTROL_PAYLOAD_LEN_DOWN_LIMIT))
        {
            return false;
        }
    }
    else if (g_rf_test_mode == RFET_CLI_CMD_RF_TEST_MODE_WISUN)
    {

        if ((get_value > wisun_tx_length_upbound) ||
                (get_value < RFET_CLI_CMD_TX_CONTROL_PAYLOAD_LEN_DOWN_LIMIT))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}


/* Layer rf_test, option 6, TX control function */
void rfet_cli_rf_test_tx_control(uint8_t *para_ptr)
{
    bool tx_start = false;
    uint8_t ch;
    uint8_t get_string_len;
    uint16_t stupid_delay = 0;
    uint32_t get_value;

    *para_ptr = RFET_CLI_CMD_GROUP_RF_TEST_TX_CONTROL;

    /* select TX Payload */
    rfet_cli_rf_test_tx_payload_prompt();

    ch = uart_getch(RFET_CLI_SHOW_CH);

    if (ch == 'R' || ch == 'r' || ch == RFET_CLI_CMD_GETCH_EARLY_ABORT)
    {
        /* to exit rx control */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
    }
    else
    {

        switch (ch)
        {
        case '1':
            g_rf_test_tx_payload = RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_PRBS9;
            break;
        case '2':
            g_rf_test_tx_payload = RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_0XF0;
            break;
        case '3':
            g_rf_test_tx_payload = RFET_CLI_CMD_RF_TEST_TX_CONTROL_PAYLOAD_0XAA;
            break;
        case 0x0D:
            /* to use current setting for TX */
            tx_start = true;
            break;
        default:
            /* Do nothing to keep stay in TX Control function */
            return;

        }

        if (tx_start == false)
        {

            /* tx length */
            rfet_cli_rf_test_tx_length_prompt();

            /* check input TX payload length */
            get_string_len = uart_get_string(true, false);

            if (get_string_len == 0xFF)
            {
                /* Do nothing to keep stay in TX Control */
            }
            else
            {

                get_value = uart_string_to_value(false);

                if (rfet_cli_check_tx_length(get_value) == false)
                {

                    printf("\r\n[ERROR]:Input is out of range");
                    /* Do nothing to keep stay in TX Control */
                }
                else
                {

                    g_rf_test_tx_payload_len = (uint16_t)(get_value & 0xFFFF);

                    printf("\r\nTX payload length is set to %d", g_rf_test_tx_payload_len);

                    /* to get tx count */
                    rfet_cli_rf_test_tx_count_prompt();

                    /* check input TX payload length */
                    get_string_len = uart_get_string(true, false);

                    if (get_string_len == 0xFF)
                    {
                        /* to exit TX control */
                        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                    }
                    else
                    {

                        get_value = uart_string_to_value(false);

                        if ((get_value > RFET_CLI_CMD_TX_CONTROL_COUNT_UP_LIMIT) ||
                                (get_value < RFET_CLI_CMD_TX_CONTROL_COUNT_DOWN_LIMIT))
                        {
                            printf("\r\n[ERROR]:Input is out of range");
                            /* Do nothing to keep stay in TX Control */
                        }
                        else
                        {

                            g_rf_test_tx_count = (uint16_t)(get_value & 0xFFFF);
                            tx_start = true;

                            printf("\r\nTX count is set to %d", g_rf_test_tx_count);
                        }
                    }
                }
            }
        }
    }

    if (tx_start == true)
    {

        /* enable TX */
        printf("\r\nTX Started\r\nPress R to leave TX:");

        g_rf_test_tx_interval = rfet_cli_rf_test_tx_interval_count(g_rf_test_tx_ht_interval);

#if 0
        if (g_rf_test_data_rate == RFET_CLI_CMD_RF_TEST_DATA_RATE_4M)
        {
            /* calculate TX interval with 1ms */
            g_rf_test_tx_interval = rfet_cli_rf_test_tx_interval_count(1);
        }
        else
        {
            /* calculate TX interval with 10ms */
            g_rf_test_tx_interval = rfet_cli_rf_test_tx_interval_count(2);
        }
#endif

        /* start TX */
        rfet_cli_rf_trx_start(RFET_CLI_CMD_RF_DIRECTION_TX_MODE);

        if (g_rf_test_tx_count != 0xFFFF)
        {
            dtm_set_check_event_flag();
        }

        do
        {
            if (uart_check_getch(&ch, RFET_CLI_SHOW_CH) == false)
            {

#if 1
                if (g_rf_test_tx_count == 0xFFFF)
                {
                    if (((stupid_delay++) % 50000) == 0)
                    {
                        printf(".");
                    }
                }
                else
                {
                    if (dtm_check_tx_burst_done_event() == true)
                    {
                        /* handle TX done event */
                        /* exit TX loop */
                        rfet_cli_rf_test_tx_done_prompt(g_rf_test_tx_count);
                        break;
                    }
                    if (((stupid_delay++) % 2000) == 0)
                    {
                        printf(".");
                    }
                }
#else
                if ((g_rf_test_tx_count != 0xFFFF) && (dtm_check_tx_burst_done_event() == true))
                {
                    /* handle TX done event */
                    /* exit TX loop */
                    rfet_cli_rf_test_tx_done_prompt(g_rf_test_tx_count);
                    break;
                }

                if (((stupid_delay++) % 5000) == 0)
                {
                    printf(".");
                }
#endif

            }
        } while (ch != 'R' && ch != 'r');

        /* stop TX */
        rfet_cli_rf_trx_stop(RFET_CLI_CMD_RF_DIRECTION_TX_MODE);

        /* to exit TX control */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;

    }

}

/* Layer rf_test, option 7, RX control function */
void rfet_cli_rf_test_rx_control(uint8_t *para_ptr)
{
    uint8_t ch;
    uint8_t get_string_len;
    uint32_t get_rx_crc_cnt;
    uint32_t get_rx_crc_fail_cnt;
    uint32_t get_rx_sync_fail_cnt;
    uint32_t get_value;

    ch = uart_getch(RFET_CLI_SHOW_CH);

    *para_ptr = RFET_CLI_CMD_GROUP_RF_TEST_RX_CONTROL;

    if (ch == 'R' || ch == 'r' || ch == RFET_CLI_CMD_GETCH_EARLY_ABORT)
    {
        /* to exit rx control */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
    }
    else
    {
        switch (ch)
        {
        case '1':
            g_rf_test_rx_control = RFET_CLI_CMD_RF_TEST_RX_CONTROL_END_REPORT;
            g_rf_test_rx_report_interval = 0;
            g_rf_test_rx_report_tx_count_for_lost_rate = 0;
            break;
        case '2':

            /* get auto report inteval */
            rfet_cli_rf_test_rx_control_auto_report_interval_prompt();

            /* check input auto RX status report interval */
            get_string_len = uart_get_string(true, false);

            if (get_string_len == 0xFF)
            {
                /* to exit rx control */
                *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
            }
            else
            {

                get_value = uart_string_to_value(false);

                if (get_value > RFET_CLI_CMD_RX_CONTROL_AUTO_REPORT_UP_LIMIT)
                {
                    printf("\r\nInput out of range");
                    /* to exit rx control */
                    *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;
                }
                else
                {

                    if (get_value == 0)
                    {
                        g_rf_test_rx_control = RFET_CLI_CMD_RF_TEST_RX_CONTROL_END_REPORT;
                    }
                    else
                    {
                        g_rf_test_rx_report_interval = (uint8_t)(get_value & 0xFF);
                        g_rf_test_rx_control = RFET_CLI_CMD_RF_TEST_RX_CONTROL_AUTO_REPORT;

                        /* calculate with TX interval */
                        g_rf_test_rx_report_tx_count_for_lost_rate = (g_rf_test_rx_report_interval * 1000) / g_rf_test_tx_ht_interval;

                        printf("\r\nRX Auto Report within %ds", g_rf_test_rx_report_interval);
                    }
                }
            }

            break;
        }
    }

    if (*para_ptr != RFET_CLI_CMD_GROUP_FUNC_DONE)
    {

        printf("\r\nRX Enabled, press [R] to leave RX:");

        /* start RX */
        rfet_cli_rf_trx_start(RFET_CLI_CMD_RF_DIRECTION_RX_MODE);

        /* Set check event flag */
        dtm_set_check_event_flag();

        do
        {
            if (uart_check_getch(&ch, RFET_CLI_SHOW_CH) == false)
            {
                if (g_rf_test_rx_control == RFET_CLI_CMD_RF_TEST_RX_CONTROL_AUTO_REPORT)
                {

                    get_rx_crc_cnt = g_rf_test_rx_total_crc_success;
                    get_rx_crc_fail_cnt = g_rf_test_rx_total_crc_fail;

                    /* what we get from event is total CRC status, only reset when RX disabled */
                    if (dtm_check_crc_event(&g_rf_test_rx_total_crc_success, &g_rf_test_rx_total_crc_fail) == true)
                    {

                        get_rx_crc_cnt = g_rf_test_rx_total_crc_success - get_rx_crc_cnt;
                        get_rx_crc_fail_cnt = g_rf_test_rx_total_crc_fail - get_rx_crc_fail_cnt;

#if 1

#if 1
                        if (g_rf_test_tx_ht_interval < 10)
                        {
                            if (get_rx_crc_cnt + get_rx_crc_fail_cnt > 500)
                            {

                                g_rf_test_rx_total_crc_success -= (get_rx_crc_cnt > 500) ? (get_rx_crc_cnt - 500) : 0;

                                if (get_rx_crc_cnt > 500)
                                {
                                    get_rx_crc_cnt = 500;
                                }

                                if (get_rx_crc_cnt <= 500)
                                {
                                    g_rf_test_rx_total_crc_success -= (get_rx_crc_cnt + get_rx_crc_fail_cnt - 500);
                                }

                                get_rx_crc_cnt = 500 - get_rx_crc_fail_cnt;

                            }
                        }
                        else
                        {
                            if (get_rx_crc_cnt + get_rx_crc_fail_cnt > 100)
                            {

                                g_rf_test_rx_total_crc_success -= (get_rx_crc_cnt > 100) ? (get_rx_crc_cnt - 100) : 0;

                                if (get_rx_crc_cnt <= 100)
                                {
                                    g_rf_test_rx_total_crc_success -= (get_rx_crc_cnt + get_rx_crc_fail_cnt - 100);
                                }
                                get_rx_crc_cnt = 100 - get_rx_crc_fail_cnt;

                            }
                        }
#else

                        if (get_rx_crc_cnt + get_rx_crc_fail_cnt > g_rf_test_rx_report_tx_count_for_lost_rate)
                        {

                            printf("\r\n:%d,%d,%d", g_rf_test_rx_total_crc_success, get_rx_crc_cnt, get_rx_crc_fail_cnt);
                            g_rf_test_rx_total_crc_success -= (get_rx_crc_cnt > g_rf_test_rx_report_tx_count_for_lost_rate) ? (get_rx_crc_cnt - g_rf_test_rx_report_tx_count_for_lost_rate) : 0;

                            if (get_rx_crc_cnt <= g_rf_test_rx_report_tx_count_for_lost_rate)
                            {
                                g_rf_test_rx_total_crc_success -= (get_rx_crc_cnt + get_rx_crc_fail_cnt - g_rf_test_rx_report_tx_count_for_lost_rate);
                            }
                            get_rx_crc_cnt = g_rf_test_rx_report_tx_count_for_lost_rate - get_rx_crc_fail_cnt;

                        }
#endif

#endif

                        if (g_rf_test_rx_report_tx_count_for_lost_rate < (get_rx_crc_cnt + get_rx_crc_fail_cnt))
                        {
#if 0
                            /* event vale more than expected, calculation error */
                            get_rx_sync_fail_cnt = g_rf_test_rx_report_tx_count_for_lost_rate;
#else
                            get_rx_sync_fail_cnt = 0;
#endif
                        }
                        else
                        {
                            get_rx_sync_fail_cnt = g_rf_test_rx_report_tx_count_for_lost_rate - get_rx_crc_cnt - get_rx_crc_fail_cnt;
                            g_rf_test_rx_total_sync_fail += get_rx_sync_fail_cnt;
                        }

                        g_rf_test_rx_total_tx_expect_cnt += g_rf_test_rx_report_tx_count_for_lost_rate;

                        g_rf_test_rx_interval_cnt++;

                        rfet_cli_rf_test_rx_status_prompt(get_rx_crc_cnt, get_rx_crc_fail_cnt, get_rx_sync_fail_cnt);

                        /* Set check event flag */
                        dtm_set_check_event_flag();
                    }
                }
            }
        } while (ch != 'R' && ch != 'r');

        rfet_cli_rf_trx_stop(RFET_CLI_CMD_RF_DIRECTION_RX_MODE);

        if (g_rf_test_rx_control == RFET_CLI_CMD_RF_TEST_RX_CONTROL_END_REPORT)
        {

            /* Set check event flag */
            dtm_set_check_event_flag();

            /* get CRC event */
            dtm_get_crc_count(&get_rx_crc_cnt, &get_rx_crc_fail_cnt);

            /* show status */
            rfet_cli_rf_test_rx_status_prompt(get_rx_crc_cnt, get_rx_crc_fail_cnt, 0);

            /* wait any key to leave */
            uart_getch(RFET_CLI_SHOW_CH);

        }
        else if (g_rf_test_rx_control == RFET_CLI_CMD_RF_TEST_RX_CONTROL_AUTO_REPORT)
        {
            /* Try to check if any CRC report event again */
            if (dtm_check_crc_event(&get_rx_crc_cnt, &get_rx_crc_fail_cnt) == true)
            {
                rfet_cli_rf_test_rx_status_prompt(get_rx_crc_cnt, get_rx_crc_fail_cnt, 0);
            }
        }

        /* to exit rx control */
        *para_ptr = RFET_CLI_CMD_GROUP_FUNC_DONE;

    }

}

/* to handle the RF Test command option through command handler */
bool rfet_cli_rf_test_cmd_check(uint8_t ch)
{
    uint8_t cmd_option;
    uint8_t para = 0;

    if (ch == 'R' || ch == 'r' || ch == RFET_CLI_CMD_GETCH_EARLY_ABORT)
    {
        return true;
    }

    cmd_option = uart_ch_to_num(ch, false);

    if ((cmd_option != 0xFF) &&
            (g_rfet_cli_rf_test_handler[cmd_option].cmd_option == cmd_option) &&
            (g_rfet_cli_rf_test_handler[cmd_option].cmd_hdlr != 0))
    {
        do
        {

            /* show prompt */
            g_rfet_cli_rf_test_handler[cmd_option].prompt_func();

            /* run callback function */
            g_rfet_cli_rf_test_handler[cmd_option].cmd_hdlr(&para);

        } while (para != RFET_CLI_CMD_GROUP_FUNC_DONE);
    }

    return false;

}

/* Layer 0, option 2, RF Test check loop */
void rfet_cli_enter_rf_test(uint8_t *para_ptr)
{
    bool exit;

    do
    {

        rfet_cli_rf_test_prompt();
        exit = rfet_cli_rf_test_cmd_check(uart_getch(RFET_CLI_SHOW_CH));

    } while (exit == false);

}

/* Layer 0, option 1, DTM control function */
void rfet_cli_enter_dtm_check(uint8_t *para_ptr)
{
    uint8_t ch;
    bool exit = false;

    do
    {

        printf("\r\nEnable DTM Mode?(y/n):");

        ch = uart_getch(RFET_CLI_SHOW_CH);

        if (ch == 'y' || ch == 'Y')
        {

            printf("\r\nDTM Mode Enabled");
            printf("\r\nPress [R] to disable and leave DTM Mode");

            /* init DTM mode */
            dtm_sys_init(RF_FW_LOAD_SELECT_RUCI_CMD, true);

#if (RFET_CLI_DTM_ENABLE_UART_BRIDGE_CMD == 1)
            dtm_sys_set_enable(true);
#endif

            do
            {
                /* Run DTM task */
                /* dtm_task(); TO DO for RTOS task */

#if (RFET_CLI_DTM_ENABLE_UART_BRIDGE_CMD == 1)
                /* Run UART Bypass task */
                if (dtm_sys_is_enable() == true)
                {
                    rfet_uart_bypass_task();
                }
#endif

                /* check if leave DTM */
                uart_check_getch(&ch, true);

            } while (ch != 'r' && ch != 'R');

#if (RFET_CLI_DTM_ENABLE_UART_BRIDGE_CMD == 1)
            dtm_sys_set_enable(false);
#endif

            printf("\r\nLeave DTM Mode");
            exit = true;

        }
        else if (ch == 'n' || ch == 'N')
        {
            printf("\r\nDTM Mode NOT Enabled");
            exit = true;
        }
        else if (ch == RFET_CLI_CMD_GETCH_EARLY_ABORT)
        {
            exit = true;
        }

    } while (exit == false);

}

/* Layer 0 prompt */
bool rfet_cli_layer0_cmd_check(uint8_t cmd_option)
{
    uint8_t *para = 0;

    if (g_rfet_cli_layer_0_handler[cmd_option].cmd_option == cmd_option &&
            g_rfet_cli_layer_0_handler[cmd_option].cmd_hdlr != 0)
    {
        /* run callback function */
        g_rfet_cli_layer_0_handler[cmd_option].cmd_hdlr(para);
    }
    else
    {
        return false;
    }

    return true;
}

bool rfet_cli_reset(void)
{
    bool ret = true;

    /**/
    dtm_sys_common_init();

    ret = dtm_ble_init_cmd();

    if (ret == true)
    {
        ret = dtm_set_tx_disable_cmd();
    }

    return ret;
}


/* CLI main task */
void rfet_cli_task(void)
{
    bool exit;
    uint8_t ch;

    do
    {

        exit = false;

        rfet_cli_layer0_control_prompt();

        ch = uart_ch_to_num(uart_getch(RFET_CLI_SHOW_CH), false);

        if (ch != RFET_CLI_CMD_GETCH_EARLY_ABORT)
        {
            exit = rfet_cli_layer0_cmd_check(ch);
        }

    } while (exit == false);
}


