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
/*  PURPOSE: NCP low level protocol declarations.
*/
#ifndef ZBNCP_INCLUDE_GUARD_LL_IMPL_H
#define ZBNCP_INCLUDE_GUARD_LL_IMPL_H 1

#include "zbncp_types.h"
#include "zbncp_debug.h"
#include "zbncp_mem.h"
#include "zbncp_iobuf.h"
#include "zbncp_fifo.h"
#include "zbncp_utils.h"
#include "zbncp_tr.h"
#include "zbncp_ll_proto.h"
#include "zbncp_ll_pkt.h"
#include "zbncp_ll_pktbuf.h"
#include "zbncp_ll_fifo.h"
#include "zbncp_ll_time.h"
#include "zbncp_ll_alarm.h"

#define ZBNCP_LL_TX_FIFO_SIZE     2u    /**< LL protocol TX FIFO size */
#define ZBNCP_LL_PKT_SEND_TIMEOUT 700u  /**< LL protocol packet send timeout */
#define ZBNCP_LL_ACK_WAIT_TIMEOUT 500u  /**< LL protocol acknowledge wait timeout */

#define ZBNCP_LL_WAIT_ACK_RETRY   3u    /**< Retry counter for missing acknowledgement retransmissions */
#define ZBNCP_LL_TX_ATTEMPTS      1u    /**< Retry counter for failed transmission attempts */

/** @brief LL protocol receive states */
typedef enum zbncp_ll_rx_state_e
{
  ZBNCP_LL_RX_STATE_IDLE,               /**< Ready to receive next packet */
  ZBNCP_LL_RX_STATE_RECEIVING_HDR,      /**< Receiving header */
  ZBNCP_LL_RX_STATE_HDR_RECEIVED,       /**< Header was received */
  ZBNCP_LL_RX_STATE_HDR_VALIDATED,      /**< Header is valid - may proceed to receiving body */
  ZBNCP_LL_RX_STATE_RECEIVING_BODY,     /**< Receiving body */
  ZBNCP_LL_RX_STATE_BODY_RECEIVED,      /**< Body was received */
  ZBNCP_LL_RX_STATE_SEND_ACK,           /**< Need to send acknowledge on received packet */
  ZBNCP_LL_RX_STATE_SENDING_ACK,        /**< Sending acknowledge on received packet */
  ZBNCP_LL_RX_STATE_RESYNC,             /**< Invalid signature or header received - resynchronize with the sender */
  ZBNCP_LL_RX_STATE_ERROR,              /**< Unrecoverable error during receive */
}
zbncp_ll_rx_state_t;

/** @brief LL protocol transmit states */
typedef enum zbncp_ll_tx_state_e
{
  ZBNCP_LL_TX_STATE_IDLE,               /**< Ready to send next packet */
  ZBNCP_LL_TX_STATE_SENDING_ACK,        /**< Sending acknowledge on received packet */
  ZBNCP_LL_TX_STATE_ACK_SENT,           /**< Acknowledge on received packet was sent */
  ZBNCP_LL_TX_STATE_SENDING_PKT,        /**< Sending regular packet */
  ZBNCP_LL_TX_STATE_PKT_SENT,           /**< Regular packet was sent */
  ZBNCP_LL_TX_STATE_WAITING_ACK,        /**< Waiting acknowledge on sent packet */
  ZBNCP_LL_TX_STATE_ACK_RECEIVED,       /**< Acknowledge on sent packet was received */
  ZBNCP_LL_TX_STATE_ERROR,              /**< Unrecoverable error during transmit */
}
zbncp_ll_tx_state_t;

/** @brief LL protocol receive state context */
typedef struct zbncp_ll_rx_s
{
  zbncp_ll_rx_state_t state;            /**< Current receive state */
  zbncp_uint8_t frameno_handled_pkt;    /**< Frame number for last handled and acknowledged packet. Used for checking duplicates */
  zbncp_uint8_t frameno_ack;            /**< Frame number for acknowledging or asking for retransmission */
  zbncp_uint8_t ackflags;               /**< Received acknowledge flags */
  zbncp_size_t received;                /**< Received byte count */
  zbncp_ll_pktbuf_t pkt;                /**< Received packet buffer */
}
zbncp_ll_rx_t;

/** @brief LL protocol transmit state context */
typedef struct zbncp_ll_tx_s
{
  zbncp_ll_tx_state_t state;            /**< Current transmit state */
  zbncp_uint8_t frameno;                /**< Next frame number to transmit */
  zbncp_uint8_t tx_attempts_cnt;        /**< Transmission attempt counter */
  zbncp_uint8_t missing_ack_cnt;        /**< Missing ACK counter */
  zbncp_ll_fifo_t fifo;                 /**< Transmit packet buffer FIFO */
  zbncp_ll_pktbuf_t ack;                /**< Packet buffer for out-of-order acknowledge to be sent with high priority */
  zbncp_ll_pktbuf_t pool[ZBNCP_LL_TX_FIFO_SIZE];
                                        /**< Transmit packet buffers pool for the FIFO */
}
zbncp_ll_tx_t;

/** @brief LL protocol state context */
struct zbncp_ll_proto_s
{
  zbncp_transport_t *tr;                /**< Pointer to the transport proxy */
  zbncp_ll_time_t time;                 /**< Current time for timeout calculation */
  zbncp_ll_proto_cb_t cb;               /**< Callbacks provided by the protocol user */
  zbncp_ll_alarm_t alarm_sending;       /**< Alarm object implementing timeout calculation for transmission of any data */
  zbncp_ll_alarm_t alarm_wait_ack;      /**< Alarm object implementing timeout calculation for waiting for ACK */
  zbncp_bool_t service;                 /**< Flag showing that LL state machine need time quantum right now */
  zbncp_int32_t status;                 /**< Status of LL state machines for RX/TX */
  zbncp_ll_rx_t rx;                     /**< Receive state context */
  zbncp_ll_tx_t tx;                     /**< Transmit state context */
};

/**
 * @brief Construct NCP low-level protocol implementation object.
 *
 * @param ll - pointer to the LL protocol state context
 * @param tr - pointer to the transport proxy
 *
 * @return nothing
 */
void zbncp_ll_construct(zbncp_ll_proto_t *ll, zbncp_transport_t *tr);

#endif /* ZBNCP_INCLUDE_GUARD_LL_IMPL_H */
