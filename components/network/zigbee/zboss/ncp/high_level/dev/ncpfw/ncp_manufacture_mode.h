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
/*  PURPOSE: NCP utilities for manufacture mode
*/
#ifndef NCP_MANUFACTURE_MODE_H
#define NCP_MANUFACTURE_MODE_H 1

#include "zb_common.h"

zb_bool_t zb_ncp_manuf_check_request(zb_uint16_t call_id);
void zb_ncp_manuf_init(zb_uint8_t page, zb_uint8_t channel);
void zb_ncp_manuf_init_cont(zb_uint8_t param);
zb_ret_t zb_ncp_manuf_set_page_and_channel(zb_uint8_t page, zb_uint8_t channel);
zb_uint8_t zb_ncp_manuf_get_page(void);
zb_uint8_t zb_ncp_manuf_get_channel(void);
void zb_ncp_manuf_set_power(zb_int8_t power_dbm);
zb_int8_t zb_ncp_manuf_get_power(void);
zb_ret_t zb_ncp_manuf_start_tone(void);
void zb_ncp_manuf_stop_tone_or_stream(void);
zb_ret_t zb_ncp_manuf_start_stream(zb_uint16_t tx_word);
zb_ret_t zb_ncp_manuf_send_packet(zb_bufid_t buf, zb_callback_t cb);
zb_ret_t zb_ncp_manuf_start_rx(zb_callback_t cb);
void zb_ncp_manuf_stop_rx(void);

#endif /* NCP_MANUFACTURE_MODE_H */
