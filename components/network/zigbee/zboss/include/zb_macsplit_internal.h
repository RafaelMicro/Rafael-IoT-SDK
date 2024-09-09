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
/*  PURPOSE: MAC split internals
*/

#ifndef ZB_MACSPLIT_INTERNAL_H
#define ZB_MACSPLIT_INTERNAL_H 1

#include "zb_types.h"
#include "zb_stack_info.h"

/**
 * Type for call types, see MACSPLIT TRANSPORT PROTOCOL section for possible call types
 */
typedef zb_uint16_t zb_transport_call_type_t;

/* 0xFFU may be interpreted as ZB_MAC_TRANSPORT_TYPE_OTA_PROTOCOL. See hacks in macsplit transport logic */
#define ZB_TRANSPORT_CALL_TYPE_NO_TYPE   0xBBU

typedef enum zb_transport_host_calls_e
{
  ZB_TRANSPORT_CALL_TYPE_HOST_RESET                            = 0x01, /* it is not used now */
  ZB_TRANSPORT_CALL_TYPE_HOST_MCPS_DATA_REQUEST                = 0x02,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_GET_REQUEST                 = 0x03,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_SET_REQUEST                 = 0x04,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_RESET_REQUEST               = 0x05,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_SCAN_REQUEST                = 0x06,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_ASSOCIATE_REQUEST           = 0x07,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_ASSOCIATE_RESPONSE          = 0x08,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_POLL_REQUEST                = 0x09,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_ORPHAN_RESPONSE             = 0x0A,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_START_REQUEST               = 0x0B,
  ZB_TRANSPORT_CALL_TYPE_HOST_SEND_EMPTY_FRAME                 = 0x0C,
  ZB_TRANSPORT_CALL_TYPE_HOST_MCPS_PURGE_INDIRECT_QUEUE_REQUEST = 0x0D,
  ZB_TRANSPORT_CALL_TYPE_HOST_MACSPLIT_GET_VERSION_REQUEST     = 0x0E,
  ZB_TRANSPORT_CALL_TYPE_HOST_SEND_MAC_TX_POWER                = 0x0F,  /* send TX powers array from Host to Radio */
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_ADD_VISIBLE_LONG             = 0x10,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_ADD_INVISIBLE_SHORT          = 0x11,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_REMOVE_INVISIBLE_SHORT       = 0x12,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_CLEAR_FILTERS                = 0x13,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_SYNC_ADDRESS_UPDATE          = 0x14,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_SEND_BEACON_REQUEST          = 0x15,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_SEND_ENHANCED_BEACON_REQUEST = 0x16,
  ZB_TRANSPORT_CALL_TYPE_HOST_MAC_CONFIGURE_FOR_SENDING        = 0x17
#if defined ZB_MAC_POWER_CONTROL
  ,
  ZB_TRANSPORT_CALL_TYPE_HOST_MLME_SET_POWER_INFORMATION_TABLE_REQUEST =0x18
#endif /* defined ZB_MAC_POWER_CONTROL */
} zb_transport_host_calls_t;

typedef enum zb_transport_device_calls_e
{
  ZB_TRANSPORT_CALL_TYPE_DEVICE_BOOT                           = 0x00,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MCPS_DATA_INDICATION           = 0x01,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MCPS_DATA_CONFIRM              = 0x02,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_GET_CONFIRM               = 0x03,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_SET_CONFIRM               = 0x04,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_RESET_CONFIRM             = 0x05,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_SCAN_CONFIRM              = 0x06,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_ASSOCIATE_CONFIRM         = 0x07,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_ASSOCIATE_INDICATION      = 0x08,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_POLL_CONFIRM              = 0x09,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_ORPHAN_INDICATION         = 0x0A,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_START_CONFIRM             = 0x0B,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MCPS_POLL_INDICATION           = 0x0C,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_BEACON_NOTIFY_INDICATION  = 0x0D,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_COMM_STATUS_INDICATION    = 0x0E,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_GP_MCPS_DATA_INDICATION        = 0x0F,

  ZB_TRANSPORT_CALL_TYPE_DEVICE_MCPS_PURGE_INDIRECT_QUEUE_CONFIRM = 0x10,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_DUTY_CYCLE_MODE_INDICATION   = 0x11,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_GET_POWER_INFO_TABLE_CONFIRM = 0x12,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MLME_SET_POWER_INFO_TABLE_CONFIRM = 0x13,
  ZB_TRANSPORT_CALL_TYPE_DEVICE_MACSPLIT_GET_VERSION_CONFIRM        = 0x14
} zb_transport_device_calls_t;


typedef ZB_PACKED_PRE struct zb_macsplit_sync_ieee_addr_update_s
{
  zb_ieee_addr_t ieee_address;
  zb_uint16_t short_address;
  zb_bool_t lock;
} ZB_PACKED_STRUCT
zb_macsplit_sync_ieee_addr_update_t;

typedef void (*zb_device_stack_information_callback)(zb_stack_information_t*);

/**
 * Get stack information from device.
 *
 * @param cb - called when device send information aboit it's stack
 *
 * @return nothing
 */
void zb_macsplit_get_device_stack_information(zb_device_stack_information_callback cb);

/**
 * Function for handling outcoming call.
 *
 * @param ref - ref to buffer to be handling
 *
 * @return nothing
 */
void zb_mcps_data_confirm(zb_bufid_t param);
void zb_mcps_data_indication(zb_bufid_t param);
void zb_mcps_poll_indication(zb_bufid_t param);
void zb_mlme_get_confirm(zb_bufid_t param);
void zb_mlme_set_confirm(zb_bufid_t param);
void zb_mlme_reset_confirm(zb_bufid_t param);
void zb_mlme_scan_confirm(zb_bufid_t param);
void zb_mlme_beacon_notify_indication(zb_bufid_t param);
void zb_mlme_associate_indication(zb_bufid_t param);
void zb_mlme_associate_confirm(zb_bufid_t param);
void zb_mlme_comm_status_indication(zb_bufid_t param);
void zb_mlme_orphan_indication(zb_bufid_t param);
void zb_mcps_data_request(zb_bufid_t param);
void zb_mlme_get_request(zb_bufid_t param);
void zb_mlme_set_request(zb_bufid_t param);
void zb_mlme_reset_request(zb_bufid_t param);
void zb_mlme_scan_request(zb_bufid_t param);
void zb_mlme_associate_request(zb_bufid_t param);
void zb_mlme_associate_response(zb_bufid_t param);
void zb_mlme_poll_request(zb_bufid_t param);
void zb_mcps_purge_indirect_queue_request(zb_bufid_t param);
void macsplit_indicate_boot(void);
void zb_mlme_dev_reset_conf(zb_bufid_t param);
void zb_mlme_dev_send_mac_tx_power(zb_bufid_t param);
void zb_macsplit_transport_handle_device_boot_call(zb_bufid_t param);
void zb_macsplit_get_device_version_request(zb_bufid_t param);
void zb_macsplit_get_device_version_confirm(zb_bufid_t param);
void zb_macsplit_send_device_stack_information(zb_bufid_t param);
void zb_macsplit_push_tx_queue(void);
/**
 * Function for handling incoming call.
 *
 * @param ref - ref to buffer to be handling
 * @param call_type - incoming call type
 *
 * @return nothing
 */
void zb_macsplit_handle_call(zb_bufid_t param, zb_transport_call_type_t call_type);

zb_bool_t zb_macsplit_call_is_conf(zb_transport_call_type_t call_type);

#if defined ZB_MACSPLIT_DEVICE
void zb_macsplit_transport_put_trace_bytes(zb_uint8_t *buf, zb_short_t len);
void zb_macsplit_transport_send_trace(zb_uint8_t *buf, zb_short_t len);
void zb_macsplit_transport_flush_trace(void);
#endif

#if defined ZB_MACSPLIT_HOST
zb_ret_t zb_macsplit_transport_secure_frame(zb_bufid_t param);
#endif

#endif /* ZB_MACSPLIT_INTERNAL_H */
