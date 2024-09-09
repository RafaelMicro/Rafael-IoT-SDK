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
/*  PURPOSE: NCP High level protocol definitions used by both dev and host
*/
#ifndef NCP_HL_PROTO_COMMON_H
#define NCP_HL_PROTO_COMMON_H 1


#define NCP_HL_CATEGORY_INTERVAL 0x100

enum ncp_hl_pkt_type_e
{
  NCP_HL_REQUEST = 0,
  NCP_HL_RESPONSE,
  NCP_HL_INDICATION,
};

enum ncp_hl_call_category_e
{
  NCP_HL_CATEGORY_CONFIGURATION = 0,
  NCP_HL_CATEGORY_AF            = NCP_HL_CATEGORY_INTERVAL,
  NCP_HL_CATEGORY_ZDO           = NCP_HL_CATEGORY_INTERVAL * 2,
  NCP_HL_CATEGORY_APS           = NCP_HL_CATEGORY_INTERVAL * 3,
  NCP_HL_CATEGORY_NWKMGMT       = NCP_HL_CATEGORY_INTERVAL * 4,
  NCP_HL_CATEGORY_SECUR         = NCP_HL_CATEGORY_INTERVAL * 5,
  NCP_HL_CATEGORY_MANUF_TEST    = NCP_HL_CATEGORY_INTERVAL * 6,
  NCP_HL_CATEGORY_OTA           = NCP_HL_CATEGORY_INTERVAL * 7
};

#define NCP_HL_CALL_CATEGORY(code) ((code) / NCP_HL_CATEGORY_INTERVAL * NCP_HL_CATEGORY_INTERVAL)


typedef enum
{
  NCP_HL_GET_MODULE_VERSION = NCP_HL_CATEGORY_CONFIGURATION + 1,
  NCP_HL_NCP_RESET,             /* resp to req or unsolicited resp*/
  NCP_HL_NCP_FACTORY_RESET,
  NCP_HL_GET_ZIGBEE_ROLE,
  NCP_HL_SET_ZIGBEE_ROLE,
  NCP_HL_GET_ZIGBEE_CHANNEL_MASK,
  NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
  NCP_HL_GET_ZIGBEE_CHANNEL,
  NCP_HL_GET_PAN_ID,
  NCP_HL_SET_PAN_ID,
  NCP_HL_GET_LOCAL_IEEE_ADDR,
  NCP_HL_SET_LOCAL_IEEE_ADDR,
  NCP_HL_SET_TRACE,
  NCP_HL_GET_KEEPALIVE_TIMEOUT,
  NCP_HL_SET_KEEPALIVE_TIMEOUT,
  NCP_HL_GET_TX_POWER,
  NCP_HL_SET_TX_POWER,
  NCP_HL_GET_RX_ON_WHEN_IDLE,
  NCP_HL_SET_RX_ON_WHEN_IDLE,
  NCP_HL_GET_JOINED,
  NCP_HL_GET_AUTHENTICATED,
  NCP_HL_GET_ED_TIMEOUT,
  NCP_HL_SET_ED_TIMEOUT,
  NCP_HL_ADD_VISIBLE_DEV,
  NCP_HL_ADD_INVISIBLE_SHORT,
  NCP_HL_RM_INVISIBLE_SHORT,
  NCP_HL_SET_NWK_KEY,
  NCP_HL_GET_SERIAL_NUMBER,
  NCP_HL_GET_VENDOR_DATA,
  NCP_HL_GET_NWK_KEYS,
  NCP_HL_GET_APS_KEY_BY_IEEE,
  NCP_HL_BIG_PKT_TO_NCP,
  NCP_HL_BIG_PKT_FROM_NCP,

  NCP_HL_AF_SET_SIMPLE_DESC = NCP_HL_CATEGORY_AF + 1,
  NCP_HL_AF_DEL_EP,
  NCP_HL_AF_SET_NODE_DESC,
  NCP_HL_AF_SET_POWER_DESC,

  NCP_HL_ZDO_NWK_ADDR_REQ = NCP_HL_CATEGORY_ZDO + 1,
  NCP_HL_ZDO_IEEE_ADDR_REQ,
  NCP_HL_ZDO_POWER_DESC_REQ,
  NCP_HL_ZDO_NODE_DESC_REQ,
  NCP_HL_ZDO_SIMPLE_DESC_REQ,
  NCP_HL_ZDO_ACTIVE_EP_REQ,
  NCP_HL_ZDO_MATCH_DESC_REQ,
  NCP_HL_ZDO_BIND_REQ,
  NCP_HL_ZDO_UNBIND_REQ,
  NCP_HL_ZDO_MGMT_LEAVE_REQ,
  NCP_HL_ZDO_PERMIT_JOINING_REQ,
   /* indication */
  NCP_HL_ZDO_DEV_ANNCE_IND,
    /* req/resp */
  NCP_HL_ZDO_REJOIN,

  /* req/resp */
  NCP_HL_APSDE_DATA_REQ = NCP_HL_CATEGORY_APS + 1,
  NCP_HL_APSME_BIND,
  NCP_HL_APSME_UNBIND,
  NCP_HL_APSME_ADD_GROUP,
  NCP_HL_APSME_RM_GROUP,
  /* indication */
  NCP_HL_APSDE_DATA_IND,
  /* req/resp */
  NCP_HL_APSME_RM_ALL_GROUPS,

  NCP_HL_NWK_FORMATION = NCP_HL_CATEGORY_NWKMGMT + 1,
  NCP_HL_NWK_DISCOVERY,
  NCP_HL_NWK_NLME_JOIN,
  NCP_HL_NWK_PERMIT_JOINING,
  NCP_HL_NWK_GET_IEEE_BY_SHORT,
  NCP_HL_NWK_GET_SHORT_BY_IEEE,
  NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE,
  /* indications */
  NCP_HL_NWK_STARTED_IND,
  NCP_HL_NWK_JOINED_IND,
  NCP_HL_NWK_JOIN_FAILED_IND,
  NCP_HL_NWK_LEAVE_IND,
  NCP_HL_GET_ED_KEEPALIVE_TIMEOUT,
  NCP_HL_SET_ED_KEEPALIVE_TIMEOUT,
  /* req/resp */
  NCP_HL_PIM_SET_FAST_POLL_INTERVAL,
  NCP_HL_PIM_SET_LONG_POLL_INTERVAL,
  NCP_HL_PIM_START_FAST_POLL,
  NCP_HL_PIM_START_LONG_POLL,
  NCP_HL_PIM_START_POLL,
  NCP_HL_PIM_SET_ADAPTIVE_POLL,
  NCP_HL_PIM_STOP_FAST_POLL,
  NCP_HL_PIM_STOP_POLL,
  NCP_HL_PIM_ENABLE_TURBO_POLL,
  NCP_HL_PIM_DISABLE_TURBO_POLL,
  NCP_HL_NWK_GET_FIRST_NBT_ENTRY,
  NCP_HL_NWK_GET_NEXT_NBT_ENTRY,
  NCP_HL_NWK_PAN_ID_CONFLICT_RESOLVE,
  /* indications */
  NCP_HL_NWK_PAN_ID_CONFLICT_IND,
  NCP_HL_NWK_ADDRESS_UPDATE_IND,
  /* req/resp */
  NCP_HL_NWK_START_WITHOUT_FORMATION,

  NCP_HL_SECUR_SET_LOCAL_IC = NCP_HL_CATEGORY_SECUR + 1,
  NCP_HL_SECUR_ADD_IC,
  NCP_HL_SECUR_DEL_IC,
  NCP_HL_SECUR_ADD_CERT,
  NCP_HL_SECUR_DEL_CERT,
  NCP_HL_SECUR_START_KE,
  NCP_HL_SECUR_START_PARTNER_LK,

  /* indications */
  NCP_HL_SECUR_CHILD_KE_FINISHED_IND,
  NCP_HL_SECUR_PARTNER_LK_FINISHED_IND,

  /* req/resp */
  NCP_HL_SECUR_JOIN_USES_IC,
  NCP_HL_SECUR_GET_IC_BY_IEEE,
  NCP_HL_SECUR_GET_CERT,
  NCP_HL_SECUR_GET_LOCAL_IC,

  /* req/resp */
  NCP_HL_MANUF_MODE_START = NCP_HL_CATEGORY_MANUF_TEST + 1,
  NCP_HL_MANUF_MODE_END,
  NCP_HL_MANUF_SET_CHANNEL,
  NCP_HL_MANUF_GET_CHANNEL,
  NCP_HL_MANUF_SET_POWER,
  NCP_HL_MANUF_GET_POWER,
  NCP_HL_MANUF_START_TONE,
  NCP_HL_MANUF_STOP_TONE,
  NCP_HL_MANUF_START_STREAM_RANDOM,
  NCP_HL_MANUF_STOP_STREAM_RANDOM,
  NCP_HL_MANUF_SEND_SINGLE_PACKET,
  NCP_HL_MANUF_START_TEST_RX,
  NCP_HL_MANUF_STOP_TEST_RX,
  /* indication */
  NCP_HL_MANUF_RX_PACKET_IND,
  /* req/resp */
  NCP_HL_MANUF_CALIBRATION,

  /* req/resp */
  NCP_HL_OTA_RUN_BOOTLOADER = NCP_HL_CATEGORY_OTA + 1

} ncp_hl_call_code_t;

typedef ZB_PACKED_PRE struct
{
  zb_uint8_t    version;
  zb_uint8_t    control;
  zb_uint16_t   call_id;
} ZB_PACKED_STRUCT
ncp_hl_header_t;

typedef ncp_hl_header_t ncp_hl_ind_header_t;

#define NCP_HL_PROTO_VERSION 0

#define NCP_HL_GET_PKT_TYPE(control) ((control) & 3)

typedef ZB_PACKED_PRE struct
{
  ncp_hl_header_t hdr;
  zb_uint8_t      tsn;
} ZB_PACKED_STRUCT
ncp_hl_request_header_t;


typedef ZB_PACKED_PRE struct
{
  ncp_hl_header_t hdr;
  zb_uint8_t      tsn;
  zb_uint8_t      status_category;
  zb_uint8_t      status_code;
} ZB_PACKED_STRUCT
ncp_hl_response_header_t;

typedef ZB_PACKED_PRE struct ncp_hl_response_neighbor_by_ieee_s
{
  zb_ieee_addr_t  ieee_addr;
  zb_uint16_t     short_addr;
  zb_uint8_t      device_type;
  zb_uint8_t      rx_on_when_idle;
  zb_uint16_t     ed_config;
  zb_uint32_t     timeout_counter;
  zb_uint32_t     device_timeout;
  zb_uint8_t      relationship;
  zb_uint8_t      transmit_failure_cnt;
  zb_uint8_t      lqi;
  zb_uint8_t      outgoing_cost;
  zb_uint8_t      age;
  zb_uint8_t      keepalive_received;
  zb_uint8_t      mac_iface_idx;
} ZB_PACKED_STRUCT
ncp_hl_response_neighbor_by_ieee_t;

#endif /* NCP_HL_PROTO_COMMON_H */
