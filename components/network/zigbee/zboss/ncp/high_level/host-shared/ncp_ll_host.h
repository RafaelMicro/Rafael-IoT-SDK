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
/*  PURPOSE: NCP Low level protocol definitions for the host side
*/
#ifndef NCP_LL_HOST_H
#define NCP_LL_HOST_H 1

#include "ncp_common.h"

/**
 *  @addtogroup NCP_HOST_API
 * @{
 */


/**
   @page ll_api_descr LL protocol API for higher layer at Host
   @{
   This is description of C implementation of ZBOSS Low Level NCP protocol.
   LL protocol must be integrated into that main loop. LL level can't have its own internal thread/task.
   I/o level which is under LL works using interrupts or some OS API (select() in Linux).
   Most Ll logic works in the customer's main loop when customer's application gives a time quant to LL.

   LL can ask for time quant from the user's Host application by
   calling a callback.  HL can give a quant to LL by calling its main
   entry potint. Same call is used to initiate transmit from HL, to
   pass received packet from Ll to HL, to ask HL to call LL entry
   point after pre-defined timeout.

   @}
 */

/**
   LL calls this callback from the interrupt context when it wants
   high level to call Ll in the main loop context.
 */
typedef void (*ncp_ll_call_me_cb_t)(void);


/**
   Initialize LL protocol and i/o layer below LL protocol.

   Call of call_me_cb means "LL wants HL to call ncp_host_ll_quant() from the main loop context".
   In most cases call_me_cb is called from ISR, so HL must limit its actions done from that callback.

   Call of call_me_cb asks for immediate call of ncp_host_ll_quant and
   canceling of LL alarm (call to ncp_host_ll_quant after timeout).

   @param call_me_cb - HL callback to be called by LL from the interrupt context.
   @return 0 if ok, -1 if error
 */
int ncp_host_ll_proto_init(ncp_ll_call_me_cb_t call_me_cb);

/**
   LL protocol entry point.

   To be called from HL in the main execution loop context.
   ncp_host_ll_quant() does following:
   - schedules HL packet transmission if tx_size is not zero
   - passes next received packet up to HL if rx_buf_size is not zero and received packet exists
   - asks HL to call ncp_host_ll_quant after timeout.
   ncp_host_ll_quant() never blocks.

Since at first call data is put into the internal queue, but not transmitted yet, it returns ZBNCP_RET_BLOCKED.
ZBNCP_RET_OK returned if previous transmit is done.
ZBNCP_RET_OK will be returned at some of next calls of ncp_host_ll_quant.
If you need to know that exactly that packet tx is complete, send packets one-by-one - that is, send the next one only after ncp_host_ll_quant return ZBNCP_RET_OK.
ZBNCP_RET_TX_FAILED returned if transport layer failed to transmit a packet.
ZBNCP_RET_NO_ACK returned if opposite side hasn't acknowledged transmitted packet.

Return code reports only TX state.
RX is indicated by assigning received_bytes.

 You may provide NULL instead of RX buf if you don't want to get data from LL library. Received packet will be buffered in LL library in such case.

   @param tx_buf             - (in) pointer to HL packet to send. NULL if nothing to send
   @param tx_size            - (in) size of HL packets to send. 0 if nothing to send
   @param rx_buf             - (in) pointer to HL's buffer for next received packet. NULL if HL don't want to receive next packet.
   @param rx_buf_size        - (in) size of rx packet buffer. 0 if HL don't want to receive next packet.
   @param received_bytes     - (out) size of the packet passed to HL. If no packet received, *received_bytes is 0.
   @param alarm_timeout_ms   - (out) delay to next call to ncp_host_ll_quant(), in ms. 0 if LL requests to call it immediately.

   @return ZBNCP_RET_OK - ok, can do next tx, ZBNCP_RET_BLOCKED - ok, can't do next tx, ZBNCP_RET_BUSY - can't do that tx

   @snippet main.c ncp_host_ll_quant

There are two reserved values for timeout:
ZBNCP_LL_TIMEOUT_INFINITE (-1) - no any pending action in NCP library - no need to call it if no call_me_cb called of Host wants to send something
ZBNCP_LL_TIMEOUT_NOW (0) - please, call ncp_host_ll_quant() again immediately

 */
int ncp_host_ll_quant(
  void *tx_buf,
  zb_uint_t tx_size,
  void *rx_buf,
  zb_uint_t rx_buf_size,
  zb_uint_t *received_bytes,
  zb_uint_t *alarm_timeout_ms);

/**
   @}
*/

#endif /* NCP_LL_HOST_H */
