/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/*  PURPOSE: Linux SPI access declarations.
*/
#ifndef LINUX_SPI_TRANSPORT_H
#define LINUX_SPI_TRANSPORT_H 1

#include "stdint.h"

/*! \addtogroup NCP_TRANSPORT_API */
/*! @{ */

#define SPI_MAX_SIZE      ZBNCP_LL_PKT_SIZE_MAX  /**< Maximum buffer size that can be transferred in one operation */

#define SPI_SUCCESS       0     /**< Sending over SPI succeeded */
#define SPI_BUSY          1     /**< SPI is busy and can not send right now */
#define SPI_FAIL          2     /**< Failed to send over SPI */

/**
   Type of callback called by Linux SPI when it received some data.

   A user first calls linux_spi_recv_data() to pass a buffer to the transport,
   then one receives notification callback on receving completion with the
   recevied data buffer pointer and with recevied byte count.

   @param buf - pointer to data received
   @param len - length actually received
 */
typedef void (*spi_recv_data_cb_t)(uint8_t *buf, uint16_t len);

/**
   Type of callback called by Linux SPI when it completed initialization or sendign data.

   After a user calls linux_spi_init(), then one receives notification
   callback on initialization completion.

   After a user calls linux_spi_send_data() to pass a buffer to the transport,
   then one receives notification callback on sending completion with a
   status of the operation.

   @param param - parameter to the callback
                  * unused for initialization completion callback
                  * status of the operation for sending completion calback
 */
typedef void (*spi_callback_t)(uint8_t param);

/**
  Initialize Linux SPI.

  @param cb - initialization completion callback
 */
void linux_spi_init(spi_callback_t cb);

/**
  Receive data from Linux SPI.

  @param buf - pointer to the buffer for received data
  @param len - length of the receive buffer
 */
void linux_spi_recv_data(uint8_t *buf, uint16_t len);

/**
  Setup Linux SPI callback on receving completion.

  @param cb - receiving completion callback
 */
void linux_spi_set_cb_recv_data(spi_recv_data_cb_t cb);

/**
  Send data over SPI.

  @param buf - pointer to the buffer with data to send
  @param len - length of the send buffer
 */
void linux_spi_send_data(uint8_t *buf, uint16_t len);

/**
  Setup Linux SPI callback on sending completion.

  @param cb - sending completion callback
 */
void linux_spi_set_cb_send_data(spi_callback_t cb);

/**
  Reset NCP.

 */
void linux_spi_reset_ncp(void);

/**
   @}
*/

#endif /* LINUX_SPI_TRANSPORT_H */
