/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
/* PURPOSE:
*/

#define ZB_TRANSCEIVER_START_CHANNEL_NUMBER 11
#define ZB_TRANSCEIVER_MAX_CHANNEL_NUMBER   26

#include "zboss_api.h"
#include "zb_types.h"
#include "lmac15p4.h"
#ifdef __cplusplus
extern "C" {
#endif


#define ZB_MAC_RX_FLUSH()
#define ZB_MAC_CLEAR_TRANS_INT()
#define ZB_MAC_GET_TRANS_INT_FLAG() 0
#define ZB_MAC_READ_INT_STATUS_REG()
#define ZB_MAC_GET_RX_INT_STATUS_BIT() TRANS_CTX().int_status_0  
#define ZB_MAC_GET_TX_INT_STATUS_BIT() TRANS_CTX().int_status_1
#define ZB_MAC_CLEAR_RX_INT_STATUS_BIT()                        \
{                                                               \
  TRANS_CTX().int_status_0 = 0;                                 \
}
#define ZB_MAC_CLEAR_TX_INT_STATUS_BIT()                        \
{                                                               \
  TRANS_CTX().int_status_1 = 0;                                 \
  TRANS_CTX().tx_status = 0;                                    \
}

#define ZB_MAC_SET_RX_INT_STATUS_BIT() (TRANS_CTX().int_status_0 = 1)
#define ZB_MAC_SET_TX_INT_STATUS_BIT() (TRANS_CTX().int_status_1 = 1)
#define ZB_MAC_TRANS_CLEAR_PENDING_BIT() zb_radioClearPendingBit()
#define ZB_MAC_TRANS_SET_PENDING_BIT() zb_radioSetPendingBit()
#define ZB_MAC_TRANS_PENDING_BIT()  zb_radioPendBit()

#define ZB_TRANSCEIVER_INIT_RADIO() 
#define ZB_TRANS_CHECK_CHANNEL_BUSY_ERROR() TRANS_CTX().tx_status
#define ZB_TRANS_CHECK_TX_RETRY_COUNT_EXCEEDED_ERROR() 0
#define ZB_TRANS_CHECK_NO_ACK() 0


#define ZB_TRANSCEIVER_DEINIT_RADIO()

#define ZB_TRANSCEIVER_SET_COORD_SHORT_ADDR(addr)
#define ZB_TRANSCEIVER_SET_COORD_EXT_ADDR(addr)
#define ZB_TRANSCEIVER_SET_PAN_COORDINATOR(pan_coord)

#define ZB_TRANSCEIVER_SET_PAN_ID(pan_id) \
    MAC_PIB().mac_pan_id = pan_id; \
    zb_radioUpdateAddressFilter();

#define ZB_TRANSCEIVER_SET_CHANNEL(page, channel_number) zb_radioSetChannel(channel_number)
#define ZB_TRANSCEIVER_SET_RX_ON_OFF(_rx_on) lmac15p4_auto_state_set(_rx_on)

#define ZB_TRANSCEIVER_UPDATE_SHORT_ADDR()    zb_radioUpdateAddressFilter()
#define ZB_TRANSCEIVER_UPDATE_PAN_ID()        zb_radioUpdateAddressFilter()
#define ZB_TRANSCEIVER_UPDATE_LONGMAC()       zb_radioUpdateAddressFilter()

#define ZB_TRANSCEIVER_GET_ENERGY_LEVEL(rssi) \
  *rssi = lmac15p4_read_rssi();

#define ZB_TRANS_GET_TX_TIMESTAMP()
#define ZB_TRANSCEIVER_GET_RX_ON_OFF()

#define ZB_TRANSCEIVER_START_GET_RSSI(_scan_duration_bi) 

#define ZB_TRANS_SEND_FRAME(header_length, buf, wait_type) \
    zb_radioSendPacket(buf, wait_type);
#define ZB_TRANS_REPEAT_SEND_FRAME(header_length, buf, wait_type)       

#define ZB_TRANSCEIVER_ENABLE_AUTO_ACK()

#define ZB_TRANS_CUT_SPECIFIC_HEADER(zb_buffer)

#define ZB_RX_FLUSH_TIMEOUT (60 * ZB_TIME_ONE_SECOND)

#define ZB_IS_TRANSPORT_BUSY() 0


typedef struct zb_transceiver_ctx_s
{
  zb_time_t  timer_ov_period;
  zb_time_t  rx_timestamp;
  zb_uint16_t int_status_0;
  zb_uint16_t int_status_1;
  zb_uint16_t tx_status;
  zb_uint8_t err;
  zb_uint8_t interrupt_flag;
  zb_uint8_t recv_buf_full;
  zb_uint8_t csma_backoffs;
  zb_uint8_t csma_be;
  zb_uint8_t csma_enable;
  zb_uint8_t len;

  zb_uint_t  in_recv;
  zb_uint_t  rx_fifo_total;
  zb_uint_t  rx_error;
  zb_uint_t  tx_autoack_inprogress;
  zb_uint_t  auto_ack_enabled;

  zb_uint32_t t1, t2;

  zb_time_t high_precision_timer;

  zb_uint16_t total_tx_retries;
  zb_uint16_t failed_tx;
  zb_time_t rx_flush_wd_timer;


}
zb_transceiver_ctx_t;

void zb_radioSetPendingBit(void);
void zb_radioClearPendingBit(void);
void zb_radioUpdateAddressFilter(void);
zb_bool_t zb_radioPendBit(void);
void zb_radioSetChannel(uint8_t channel);
void zb_radioSendPacket(uint8_t buf, zb_uint8_t wait_type);