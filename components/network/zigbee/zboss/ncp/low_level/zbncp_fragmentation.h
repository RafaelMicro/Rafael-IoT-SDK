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
/*  PURPOSE: NCP low level fragmentation prototypes
*/
#ifndef ZBNCP_INCLUDE_GUARD_FRAGMENTATION_H
#define ZBNCP_INCLUDE_GUARD_FRAGMENTATION_H 1

#include "zbncp_types.h"
#include "zbncp_ll_proto.h"
#include "zbncp_frag_internal.h"

/**
 * @brief Fragmentation context structure
 */
typedef struct zbncp_frag_ctx_s
{
    zbncp_res_ctx_t res;    /**< Context for packet reassembling */
    zbncp_div_ctx_t div;    /**< Context for packet divider */
}
zbncp_frag_ctx_t;

/**
 * @brief Initialization of fragmentation context
 *
 * @param frag_ctx - context for fragmentation procedure
 */
void zbncp_frag_initialize(zbncp_frag_ctx_t *frag_ctx);

/**
 * @brief Storing a packet for transmission into internal buffer
 *
 * @param frag_ctx - context for fragmentation procedure
 * @param buf - pointer to a transmitted data
 * @param size - size of transmitted data
 * @return 0 if everything OK, otherwise error
 */
zbncp_int32_t zbncp_frag_store_tx_pkt(zbncp_frag_ctx_t *frag_ctx, const void* buf, zbncp_size_t size);

/**
 * @brief Pass place for copying a received packet to user
 *
 * @param frag_ctx - context for fragmentation procedure
 * @param buf - pointer to a memory where a packet will be stored
 * @param size - size of user buffer to not exceeding
 */
void zbncp_frag_set_place_for_rx_pkt(zbncp_frag_ctx_t *frag_ctx, void* buf, zbncp_size_t size);

/**
 * @brief Filling request to pass into low level
 *
 * @param frag_ctx - context for fragmentation procedure
 * @param req - request parameters, where fragments memory parameters are filled
 */
void zbncp_frag_fill_request(const zbncp_frag_ctx_t *frag_ctx, zbncp_ll_quant_req_t *req);

/**
 * @brief Processing TX response from low level
 *
 * @param frag_ctx - context for fragmentation procedure
 * @param rsp - information about received packet and transmitted packet
 * @return NCP LL transmitter readiness status, true - if NCP LL is ready to transmit next packet
 */
zbncp_bool_t zbncp_frag_process_tx_response(zbncp_frag_ctx_t *frag_ctx, const zbncp_ll_quant_res_t *rsp);

/**
 * @brief Processing RX response from low level
 *
 * @param frag_ctx - context for fragmentation procedure
 * @param rsp - information about received packet and transmitted packet
 * @return size of received packet, 0 - if no packet
 */
zbncp_size_t zbncp_frag_process_rx_response(zbncp_frag_ctx_t *frag_ctx, const zbncp_ll_quant_res_t *rsp);

#endif /* ZBNCP_INCLUDE_GUARD_FRAGMENTATION_H */
