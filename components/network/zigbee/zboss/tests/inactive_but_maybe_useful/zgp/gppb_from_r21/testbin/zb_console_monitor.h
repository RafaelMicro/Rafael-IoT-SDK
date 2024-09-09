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
/*  PURPOSE:
*/
#ifndef ZB_CONSOLE_MONITOR_H
#define ZB_CONSOLE_MONITOR_H 1

#include "zb_mac_transport.h"

#define ZB_MAC_TRANSPORT_SIGNATURE_SIZE 2
#define ZB_CONSOLE_MONITOR_INVITATION '$'
#define ZB_CONSOLE_MONITOR_INVITATION_SIZE 1

typedef ZB_PACKED_PRE struct zb_console_monitor_hdr_s
{
  zb_uint8_t sig[ZB_MAC_TRANSPORT_SIGNATURE_SIZE];
  zb_mac_transport_hdr_t h;
} ZB_PACKED_STRUCT
zb_console_monitor_hdr_t;

typedef ZB_PACKED_PRE struct zb_console_monitor_data_s
{
  zb_uint8_t payload[ZB_CONSOLE_MONITOR_INVITATION_SIZE];
} ZB_PACKED_STRUCT
zb_console_monitor_data_t;

typedef ZB_PACKED_PRE struct zb_console_monitor_pkt_s
{
  zb_console_monitor_hdr_t hdr;
  zb_console_monitor_data_t data;
} ZB_PACKED_STRUCT
zb_console_monitor_pkt_t;

typedef struct zb_console_monitor_ctx_s
{
  zb_uint8_t *buffer;           /* Data buffer */
  zb_uint8_t max_lenght;        /* Lenght of Data buffer */
  zb_uint8_t pos;               /* Written */
  zb_bool_t rx_ready;           /* Monitor ready to RX */
  volatile zb_bool_t cmd_recvd;
  zb_ret_t status;
} zb_console_monitor_ctx_t;

void zb_console_monitor_init();
void zb_console_monitor_deinit();
zb_uint8_t zb_console_monitor_get_cmd(zb_uint8_t *buffer, zb_uint8_t max_lenght);

#endif /* ZB_CONSOLE_MONITOR_H */
