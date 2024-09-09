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
/*  PURPOSE: NCP low level fragmentation internal structures and prototypes
*/
#ifndef ZBNCP_INCLUDE_GUARD_FRAG_INTERNAL_H
#define ZBNCP_INCLUDE_GUARD_FRAG_INTERNAL_H 1

#include "zbncp_types.h"
#include "zbncp_mem.h"
#include "zbncp_ll_proto.h"

typedef struct zbncp_res_ctx_s
{
  void *user_buf;
  zbncp_size_t max_size;
  zbncp_size_t offset;
  zbncp_memref_t mem;
  zbncp_uint8_t big_buf[ZBNCP_BIG_BUF_SIZE];
}
zbncp_res_ctx_t;

typedef struct zbncp_div_ctx_s
{
  zbncp_size_t offset;
  zbncp_size_t pkt_size;
  zbncp_ll_tx_pkt_t tx_pkt;
  zbncp_uint8_t big_buf[ZBNCP_BIG_BUF_SIZE];
}
zbncp_div_ctx_t;

void zbncp_res_initialize(zbncp_res_ctx_t *res);
void zbncp_div_initialize(zbncp_div_ctx_t *div);

zbncp_int32_t zbncp_div_store_tx_pkt(zbncp_div_ctx_t *div, const void* buf, zbncp_size_t size);
zbncp_ll_tx_pkt_t zbncp_div_request_fragment(const zbncp_div_ctx_t *div);
zbncp_bool_t zbncp_div_process_response(zbncp_div_ctx_t *div, const zbncp_ll_quant_res_t *rsp);

void zbncp_res_set_place_for_rx_pkt(zbncp_res_ctx_t *res, void* buf, zbncp_size_t size);
zbncp_memref_t zbncp_res_get_place_for_fragment(const zbncp_res_ctx_t *res);
zbncp_size_t zbncp_res_process_response(zbncp_res_ctx_t *res, const zbncp_ll_rx_info_t *info);

#endif /* ZBNCP_INCLUDE_GUARD_FRAG_INTERNAL_H */
