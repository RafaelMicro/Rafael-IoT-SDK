/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfb_sample.h
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
#ifndef _RFB_SAMPLE_H_
#define _RFB_SAMPLE_H_
/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <timers.h>
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/
typedef enum {
    MAC_TX_CONTINUOUS_WAVE_TEST = 0x01,
    MAC_TX_PACKETS_TEST = 0x02,
    MAC_RX_TEST = 0x03
} mac_test_case_t;

typedef enum {
    APP_TX_DONE_EVT,
    APP_RX_DONE_EVT,
    APP_TX_TIMER_EVT,
    APP_RX_TIMER_EVT
} app_evt_t;

typedef struct {
    uint32_t event;
    uint32_t data;
} app_queue_t;

xQueueHandle app_msg_q;
TimerHandle_t tx_timer;
TimerHandle_t rx_timer;
/**************************************************************************************************
 *    Global Prototypes
 *************************************************************************************************/
void rfb_sample_init(uint8_t RfbPciTestCase);
void app_tx_process(uint32_t rfb_pci_test_case);
void app_rx_process(void);
#endif

