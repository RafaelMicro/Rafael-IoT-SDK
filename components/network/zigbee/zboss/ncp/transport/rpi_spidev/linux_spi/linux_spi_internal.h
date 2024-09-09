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
/*  PURPOSE: Linux SPI access internal declarations.
*/
#ifndef LINUX_SPI_INTERNAL_H
#define LINUX_SPI_INTERNAL_H 1

#if defined ZB_CONFIG_SGW_GENIATECH_JUNA || defined ZB_CONFIG_SGW_GENIATECH_RADIO_NORDIC
#include "gw_vendor.h"
#include "zboss_api.h"
#endif

#include <linux/spi/spidev.h>
#include "linux_spi_transport.h"
#include "linux_spi_internal_types.h"

/* Default pins assignment are for RPi 2,3 Number n is BCMn at pinout description */

#ifndef SPI_HOST_INT_PIN
#define SPI_HOST_INT_PIN        25
#endif

#ifndef SPI_CS_PIN
#define SPI_CS_PIN              8 /* BCM 8, pin 24, SPI CE0 */
#endif

#ifndef SPI_RESET_PIN
#define SPI_RESET_PIN           24
#endif
#ifndef SPI_RESET_ACTIVE
#define SPI_RESET_ACTIVE        0
#endif
#ifndef SPI_RESET_DISABLED
#define SPI_RESET_DISABLED      1
#endif

#ifndef SPI_DEVICE
#define SPI_DEVICE              "/dev/spidev0.0"
#endif
#ifndef SPI_TRANSPORT_MODE
#define SPI_TRANSPORT_MODE      (SPI_MODE_1)
#endif
#ifndef SPI_TRANSPORT_BIT_RATE
#define SPI_TRANSPORT_BIT_RATE  (1000000u)/*(15200u)*/
#endif
/* #define SPI_TRANSPORT_BIT_RATE  (15200u) */

#define SPI_SEND_DATA_SEM	"/ncp_spi_send_sem"

#define SPI_RX_OBJS     3

typedef enum spi_ini_state_e
{
  SPI_NOT_INITIALIZED,
  SPI_INITIALIZED
}
spi_init_state_t;

typedef struct linux_spi_ctx_s
{
  spi_recv_data_cb_t recv_cb;
  spi_callback_t     send_cb;
  spi_callback_t     init_cb;
  int                spi_fd;
}
linux_spi_ctx_t;

typedef struct recv_obj_s
{
  uint8_t* buf;
  size_t remain;
  size_t offset;
}
recv_obj_t;

typedef struct rx_spi_ctx_s
{
  recv_obj_t recv_obj[SPI_RX_OBJS];
  uint8_t drv_index;
  uint8_t* user_buf;
  size_t user_len;
  size_t user_offset;
  uint8_t user_index;
  spi_bool_t is_user_requested;
  uint8_t empty_obj_cnt;
}
rx_spi_ctx_t;

typedef struct tx_spi_ctx_s
{
  uint8_t *buf;
  size_t len;
}
tx_spi_ctx_t;

#endif /* LINUX_SPI_INTERNAL_H */
