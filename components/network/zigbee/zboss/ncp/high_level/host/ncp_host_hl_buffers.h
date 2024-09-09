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
/*  PURPOSE: NCP High level transport buffers declarations for the host side
*/

#ifndef NCP_HOST_HL_BUFFERS_H
#define NCP_HOST_HL_BUFFERS_H 1

#include "zb_common.h"
#include "zb_types.h"
#include "ncp_hl_proto.h"

typedef struct ncp_host_hl_rx_buf_handle_s
{
  zb_uint8_t *ptr;
  zb_size_t len;
  zb_size_t pos;
} ncp_host_hl_rx_buf_handle_t;

typedef struct ncp_host_hl_tx_buf_handle_s
{
  zb_uint8_t *ptr;
  zb_size_t len;
  zb_size_t pos;
  zb_uint8_t buf_id;
} ncp_host_hl_tx_buf_handle_t;

zb_ret_t ncp_host_hl_init_indication_body(ncp_hl_ind_header_t *hdr,
                                          zb_uint16_t len,
                                          ncp_host_hl_rx_buf_handle_t *handle);

zb_ret_t ncp_host_hl_init_response_body(ncp_hl_response_header_t *hdr,
                                        zb_uint16_t len,
                                        ncp_host_hl_rx_buf_handle_t *handle);

zb_ret_t ncp_host_hl_init_request_body(zb_uint8_t buf_id,
                                       ncp_hl_request_header_t *hdr,
                                       zb_uint16_t max_len,
                                       ncp_host_hl_tx_buf_handle_t *handle);


void ncp_host_hl_buf_get_u8(ncp_host_hl_rx_buf_handle_t *buf, zb_uint8_t *val);

void ncp_host_hl_buf_get_u16(ncp_host_hl_rx_buf_handle_t *buf, zb_uint16_t *val);

void ncp_host_hl_buf_get_u32(ncp_host_hl_rx_buf_handle_t *buf, zb_uint32_t *val);

void ncp_host_hl_buf_get_u64(ncp_host_hl_rx_buf_handle_t *buf, zb_uint64_t *val);

void ncp_host_hl_buf_get_u64addr(ncp_host_hl_rx_buf_handle_t *buf, zb_64bit_addr_t addr);

void ncp_host_hl_buf_get_u128key(ncp_host_hl_rx_buf_handle_t *buf, zb_uint8_t *key);

void ncp_host_hl_buf_get_array(ncp_host_hl_rx_buf_handle_t *buf, zb_uint8_t *data_ptr, zb_size_t size);

void ncp_host_hl_buf_get_ptr(ncp_host_hl_rx_buf_handle_t *buf, zb_uint8_t **data_ptr, zb_size_t size);


void ncp_host_hl_buf_put_u8(ncp_host_hl_tx_buf_handle_t *buf, zb_uint8_t val);

void ncp_host_hl_buf_put_u16(ncp_host_hl_tx_buf_handle_t *buf, zb_uint16_t val);

void ncp_host_hl_buf_put_u32(ncp_host_hl_tx_buf_handle_t *buf, zb_uint32_t val);

void ncp_host_hl_buf_put_u64(ncp_host_hl_tx_buf_handle_t *buf, zb_uint64_t val);

void ncp_host_hl_buf_put_u64addr(ncp_host_hl_tx_buf_handle_t *buf, const zb_64bit_addr_t addr);

void ncp_host_hl_buf_put_u128key(ncp_host_hl_tx_buf_handle_t *buf, zb_uint8_t *key);

void ncp_host_hl_buf_put_array(ncp_host_hl_tx_buf_handle_t *buf, zb_uint8_t *data_ptr, zb_size_t size);


zb_ret_t ncp_host_hl_buf_check_len(const ncp_host_hl_tx_buf_handle_t *buf, zb_size_t expected);

zb_uint16_t ncp_host_hl_tx_buf_get_len(ncp_host_hl_tx_buf_handle_t *handle);

#endif /* NCP_HOST_HL_BUFFERS_H */
