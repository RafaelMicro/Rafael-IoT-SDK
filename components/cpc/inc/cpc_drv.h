/**
 * @file cpc_drv.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CPC_DRV_H
#define CPC_DRV_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include "cpc.h"
#include "cpc_api.h"

// -----------------------------------------------------------------------------
// SPI Default configuration value
#ifndef CPC_DRV_SPI_FRAME_LENGTH
#define CPC_DRV_SPI_FRAME_LENGTH 8
#endif
#ifndef CPC_DRV_SPI_BIT_ORDER
#define CPC_DRV_SPI_BIT_ORDER spidrvBitOrderMsbFirst
#endif
#ifndef CPC_DRV_SPI_BIT_ORCPC_DRV_SPI_CLOCK_MODEDER
#define CPC_DRV_SPI_CLOCK_MODE spidrvClockMode0
#endif

typedef struct {
    bool use_raw_rx_buffer;
    bool preprocess_hdlc_header;
    bool uart_flowcontrol;
} cpc_drv_capabilities_t;

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Core to driver commands

/***************************************************************************/ /**
 * Initializes CPC driver.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
status_t cpc_drv_init(void);

/***************************************************************************/ /**
 * Gets CPC driver capabilities.
 *
 * @param capabilities Pointer to structure that will receive the driver
 *                     capabilities.
 ******************************************************************************/
void cpc_drv_get_capabilities(cpc_drv_capabilities_t* capabilities);

/***************************************************************************/ /**
 * Reads data from driver.
 *
 * @param buffer_handle Pointer to variable that will receive receive the buffer handle.
 *
 * @param payload_rx_len Pointer to variable that will receive the received
 *                       payload length, in bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
status_t cpc_drv_read_data(cpc_buffer_handle_t** buffer_handle,
                           uint16_t* payload_rx_len);

/***************************************************************************/ /**
 * Checks if driver is ready to transmit.
 *
 * @return true if driver is ready to accept a tx. false otherwise.
 ******************************************************************************/
bool cpc_drv_is_transmit_ready(void);

/***************************************************************************/ /**
 * Transmits data.
 *
 * @param buffer_handle Pointer to the buffer handle.
 *
 * @param payload_tx_len Payload length to transmit, in bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
status_t cpc_drv_transmit_data(cpc_buffer_handle_t* buffer_handle,
                               uint16_t payload_tx_len);

// -----------------------------------------------------------------------------
// Driver to core notifications

/***************************************************************************/ /**
 * Notifies core of tx completion.
 *
 * @param buffer_handle Pointer to the buffer handle that was transmitted.
 ******************************************************************************/
void cpc_drv_notify_tx_complete(cpc_buffer_handle_t* buffer_handle);

/***************************************************************************/ /**
 * Notifies core of rx completion.
 ******************************************************************************/
void cpc_drv_notify_rx_data(void);

/***************************************************************************/ /**
 * Check if the driver is out of RX buffers
 ******************************************************************************/
bool cpc_drv_is_out_of_rx_buffers(void);

/***************************************************************************/ /**
 * Get currently configured bus speed
 ******************************************************************************/
uint32_t cpc_drv_get_bus_speed(void);

void cpc_drv_trnsmit_complete(void);

uint32_t cpc_drv_uart_push_header(uint8_t* pHeader);

void cpc_drv_uart_push_data(uint8_t* p_data, uint16_t length);

/** @} (end addtogroup cpc) */

#ifdef __cplusplus
}
#endif

#endif // CPC_DRV_H
