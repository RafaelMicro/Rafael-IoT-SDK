/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2021 DSR Corporation, Denver CO, USA.
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
/*  PURPOSE: NCP High level protocol definitions used by both dev and host
*/
#ifndef NCP_HL_PROTO_COMMON_H
#define NCP_HL_PROTO_COMMON_H 1


#define NCP_HL_CATEGORY_INTERVAL (0x100U)

/**
 * @name NCP HL packet types
 * @anchor hl_pkt_type
 *
 * Note: These values were members of `enum ncp_hl_pkt_type_e` type but were
 * converted to a set of macros due to MISRA violations.
 */
/** @{ */
#define NCP_HL_REQUEST    0U
#define NCP_HL_RESPONSE   1U
#define NCP_HL_INDICATION 2U
/** @} */

/**
 * @name NCP HL call category
 * @anchor hl_call_category
 *
 * Note: These values were members of `enum ncp_hl_call_category_e` type but were
 * converted to a set of macros due to MISRA violations.
 */
/** @{ */
#define NCP_HL_CATEGORY_CONFIGURATION (0U)
#define NCP_HL_CATEGORY_AF            (NCP_HL_CATEGORY_INTERVAL * 1U)
#define NCP_HL_CATEGORY_ZDO           (NCP_HL_CATEGORY_INTERVAL * 2U)
#define NCP_HL_CATEGORY_APS           (NCP_HL_CATEGORY_INTERVAL * 3U)
#define NCP_HL_CATEGORY_NWKMGMT       (NCP_HL_CATEGORY_INTERVAL * 4U)
#define NCP_HL_CATEGORY_SECUR         (NCP_HL_CATEGORY_INTERVAL * 5U)
#define NCP_HL_CATEGORY_MANUF_TEST    (NCP_HL_CATEGORY_INTERVAL * 6U)
#define NCP_HL_CATEGORY_OTA           (NCP_HL_CATEGORY_INTERVAL * 7U)
#define NCP_HL_CATEGORY_CUSTOM_COMMANDS (NCP_HL_CATEGORY_INTERVAL * 8U)
#define NCP_HL_CATEGORY_INTRP         (NCP_HL_CATEGORY_INTERVAL * 9U)
/** @} */

#define NCP_HL_CALL_CATEGORY(code) ((code) / NCP_HL_CATEGORY_INTERVAL * NCP_HL_CATEGORY_INTERVAL)

/**
 * @name NCP HL Call codes
 * @anchor hl_call_codes
 */
/** @{ */
#define NCP_HL_NO_COMMAND                   0U
#define NCP_HL_GET_MODULE_VERSION           (NCP_HL_CATEGORY_CONFIGURATION + 1U)
#define NCP_HL_NCP_RESET                    (NCP_HL_CATEGORY_CONFIGURATION + 2U) /* resp to req or unsolicited resp */
#define NCP_HL_NCP_FACTORY_RESET            (NCP_HL_CATEGORY_CONFIGURATION + 3U)
#define NCP_HL_GET_ZIGBEE_ROLE              (NCP_HL_CATEGORY_CONFIGURATION + 4U)
#define NCP_HL_SET_ZIGBEE_ROLE              (NCP_HL_CATEGORY_CONFIGURATION + 5U)
#define NCP_HL_GET_ZIGBEE_CHANNEL_MASK      (NCP_HL_CATEGORY_CONFIGURATION + 6U)
#define NCP_HL_SET_ZIGBEE_CHANNEL_MASK      (NCP_HL_CATEGORY_CONFIGURATION + 7U)
#define NCP_HL_GET_ZIGBEE_CHANNEL           (NCP_HL_CATEGORY_CONFIGURATION + 8U)
#define NCP_HL_GET_PAN_ID                   (NCP_HL_CATEGORY_CONFIGURATION + 9U)
#define NCP_HL_SET_PAN_ID                   (NCP_HL_CATEGORY_CONFIGURATION + 10U)
#define NCP_HL_GET_LOCAL_IEEE_ADDR          (NCP_HL_CATEGORY_CONFIGURATION + 11U)
#define NCP_HL_SET_LOCAL_IEEE_ADDR          (NCP_HL_CATEGORY_CONFIGURATION + 12U)
#define NCP_HL_SET_TRACE                    (NCP_HL_CATEGORY_CONFIGURATION + 13U)
#define NCP_HL_RESERVED_1                   (NCP_HL_CATEGORY_CONFIGURATION + 14U) /* NCP_HL_GET_KEEPALIVE_TIMEOUT */
#define NCP_HL_RESERVED_2                   (NCP_HL_CATEGORY_CONFIGURATION + 15U) /* NCP_HL_SET_KEEPALIVE_TIMEOUT */
#define NCP_HL_GET_TX_POWER                 (NCP_HL_CATEGORY_CONFIGURATION + 16U)
#define NCP_HL_SET_TX_POWER                 (NCP_HL_CATEGORY_CONFIGURATION + 17U)
#define NCP_HL_GET_RX_ON_WHEN_IDLE          (NCP_HL_CATEGORY_CONFIGURATION + 18U)
#define NCP_HL_SET_RX_ON_WHEN_IDLE          (NCP_HL_CATEGORY_CONFIGURATION + 19U)
#define NCP_HL_GET_JOINED                   (NCP_HL_CATEGORY_CONFIGURATION + 20U)
#define NCP_HL_GET_AUTHENTICATED            (NCP_HL_CATEGORY_CONFIGURATION + 21U)
#define NCP_HL_GET_ED_TIMEOUT               (NCP_HL_CATEGORY_CONFIGURATION + 22U)
#define NCP_HL_SET_ED_TIMEOUT               (NCP_HL_CATEGORY_CONFIGURATION + 23U)
#define NCP_HL_ADD_VISIBLE_DEV              (NCP_HL_CATEGORY_CONFIGURATION + 24U)
#define NCP_HL_ADD_INVISIBLE_SHORT          (NCP_HL_CATEGORY_CONFIGURATION + 25U)
#define NCP_HL_RM_INVISIBLE_SHORT           (NCP_HL_CATEGORY_CONFIGURATION + 26U)
#define NCP_HL_SET_NWK_KEY                  (NCP_HL_CATEGORY_CONFIGURATION + 27U)
#define NCP_HL_GET_SERIAL_NUMBER            (NCP_HL_CATEGORY_CONFIGURATION + 28U)
#define NCP_HL_GET_VENDOR_DATA              (NCP_HL_CATEGORY_CONFIGURATION + 29U)
#define NCP_HL_GET_NWK_KEYS                 (NCP_HL_CATEGORY_CONFIGURATION + 30U)
#define NCP_HL_GET_APS_KEY_BY_IEEE          (NCP_HL_CATEGORY_CONFIGURATION + 31U)
#define NCP_HL_BIG_PKT_TO_NCP               (NCP_HL_CATEGORY_CONFIGURATION + 32U)
#define NCP_HL_BIG_PKT_FROM_NCP             (NCP_HL_CATEGORY_CONFIGURATION + 33U)
#define NCP_HL_GET_PARENT_ADDRESS           (NCP_HL_CATEGORY_CONFIGURATION + 34U)
#define NCP_HL_GET_EXTENDED_PAN_ID          (NCP_HL_CATEGORY_CONFIGURATION + 35U)
#define NCP_HL_GET_COORDINATOR_VERSION      (NCP_HL_CATEGORY_CONFIGURATION + 36U)
#define NCP_HL_GET_SHORT_ADDRESS            (NCP_HL_CATEGORY_CONFIGURATION + 37U)
#define NCP_HL_GET_TRUST_CENTER_ADDRESS     (NCP_HL_CATEGORY_CONFIGURATION + 38U)
#define NCP_HL_DEBUG_WRITE                  (NCP_HL_CATEGORY_CONFIGURATION + 39U)
#define NCP_HL_GET_CONFIG_PARAMETER         (NCP_HL_CATEGORY_CONFIGURATION + 40U)
#define NCP_HL_GET_LOCK_STATUS              (NCP_HL_CATEGORY_CONFIGURATION + 41U)
#define NCP_HL_GET_TRACE                    (NCP_HL_CATEGORY_CONFIGURATION + 42U)
#define NCP_HL_NCP_RESET_IND                (NCP_HL_CATEGORY_CONFIGURATION + 43U)
#define NCP_HL_SET_NWK_LEAVE_ALLOWED        (NCP_HL_CATEGORY_CONFIGURATION + 44U)
#define NCP_HL_GET_NWK_LEAVE_ALLOWED        (NCP_HL_CATEGORY_CONFIGURATION + 45U)
#define NCP_HL_NVRAM_WRITE                  (NCP_HL_CATEGORY_CONFIGURATION + 46U)
#define NCP_HL_NVRAM_READ                   (NCP_HL_CATEGORY_CONFIGURATION + 47U)
#define NCP_HL_NVRAM_ERASE                  (NCP_HL_CATEGORY_CONFIGURATION + 48U)
#define NCP_HL_NVRAM_CLEAR                  (NCP_HL_CATEGORY_CONFIGURATION + 49U)
#define NCP_HL_SET_TC_POLICY                (NCP_HL_CATEGORY_CONFIGURATION + 50U)
#define NCP_HL_SET_EXTENDED_PAN_ID          (NCP_HL_CATEGORY_CONFIGURATION + 51U)
#define NCP_HL_SET_MAX_CHILDREN             (NCP_HL_CATEGORY_CONFIGURATION + 52U)
#define NCP_HL_GET_MAX_CHILDREN             (NCP_HL_CATEGORY_CONFIGURATION + 53U)
#define NCP_HL_SET_ZDO_LEAVE_ALLOWED        (NCP_HL_CATEGORY_CONFIGURATION + 54U)
#define NCP_HL_GET_ZDO_LEAVE_ALLOWED        (NCP_HL_CATEGORY_CONFIGURATION + 55U)
#define NCP_HL_SET_LEAVE_WO_REJOIN_ALLOWED  (NCP_HL_CATEGORY_CONFIGURATION + 56U)
#define NCP_HL_GET_LEAVE_WO_REJOIN_ALLOWED  (NCP_HL_CATEGORY_CONFIGURATION + 57U)
#define NCP_HL_DISABLE_GPPB                 (NCP_HL_CATEGORY_CONFIGURATION + 58U)
#define NCP_HL_GP_SET_SHARED_KEY_TYPE       (NCP_HL_CATEGORY_CONFIGURATION + 59U)
#define NCP_HL_GP_SET_DEFAULT_LINK_KEY      (NCP_HL_CATEGORY_CONFIGURATION + 60U)
#define NCP_HL_PRODUCTION_CONFIG_READ       (NCP_HL_CATEGORY_CONFIGURATION + 61U)

#define NCP_HL_AF_SET_SIMPLE_DESC               (NCP_HL_CATEGORY_AF + 1U)
#define NCP_HL_AF_DEL_SIMPLE_DESC               (NCP_HL_CATEGORY_AF + 2U)
#define NCP_HL_AF_SET_NODE_DESC                 (NCP_HL_CATEGORY_AF + 3U)
#define NCP_HL_AF_SET_POWER_DESC                (NCP_HL_CATEGORY_AF + 4U)
#define NCP_HL_AF_SUBGHZ_SUSPEND_IND            (NCP_HL_CATEGORY_AF + 5U)

#define NCP_HL_ZDO_NWK_ADDR_REQ                 (NCP_HL_CATEGORY_ZDO + 1U)
#define NCP_HL_ZDO_IEEE_ADDR_REQ                (NCP_HL_CATEGORY_ZDO + 2U)
#define NCP_HL_ZDO_POWER_DESC_REQ               (NCP_HL_CATEGORY_ZDO + 3U)
#define NCP_HL_ZDO_NODE_DESC_REQ                (NCP_HL_CATEGORY_ZDO + 4U)
#define NCP_HL_ZDO_SIMPLE_DESC_REQ              (NCP_HL_CATEGORY_ZDO + 5U)
#define NCP_HL_ZDO_ACTIVE_EP_REQ                (NCP_HL_CATEGORY_ZDO + 6U)
#define NCP_HL_ZDO_MATCH_DESC_REQ               (NCP_HL_CATEGORY_ZDO + 7U)
#define NCP_HL_ZDO_BIND_REQ                     (NCP_HL_CATEGORY_ZDO + 8U)
#define NCP_HL_ZDO_UNBIND_REQ                   (NCP_HL_CATEGORY_ZDO + 9U)
#define NCP_HL_ZDO_MGMT_LEAVE_REQ               (NCP_HL_CATEGORY_ZDO + 10U)
#define NCP_HL_ZDO_PERMIT_JOINING_REQ           (NCP_HL_CATEGORY_ZDO + 11U)
/* indication */
#define NCP_HL_ZDO_DEV_ANNCE_IND                (NCP_HL_CATEGORY_ZDO + 12U)
/* req/resp */
#define NCP_HL_ZDO_REJOIN                       (NCP_HL_CATEGORY_ZDO + 13U)
#define NCP_HL_ZDO_SYSTEM_SRV_DISCOVERY_REQ     (NCP_HL_CATEGORY_ZDO + 14U)
#define NCP_HL_ZDO_MGMT_BIND_REQ                (NCP_HL_CATEGORY_ZDO + 15U)
#define NCP_HL_ZDO_MGMT_LQI_REQ                 (NCP_HL_CATEGORY_ZDO + 16U)
#define NCP_HL_ZDO_MGMT_NWK_UPDATE_REQ          (NCP_HL_CATEGORY_ZDO + 17U)
/* indication */
#define NCP_HL_ZDO_REMOTE_CMD_IND               (NCP_HL_CATEGORY_ZDO + 18U)
#define NCP_HL_ZDO_GET_STATS                    (NCP_HL_CATEGORY_ZDO + 19U)

/* indication */
#define NCP_HL_ZDO_DEV_AUTHORIZED_IND           (NCP_HL_CATEGORY_ZDO + 20U)
#define NCP_HL_ZDO_DEV_UPDATE_IND               (NCP_HL_CATEGORY_ZDO + 21U)
/* req/resp */
#define NCP_HL_ZDO_SET_NODE_DESC_MANUF_CODE_REQ (NCP_HL_CATEGORY_ZDO + 22U)

/* req/resp */
#define NCP_HL_APSDE_DATA_REQ      (NCP_HL_CATEGORY_APS + 1U)
#define NCP_HL_APSME_BIND          (NCP_HL_CATEGORY_APS + 2U)
#define NCP_HL_APSME_UNBIND        (NCP_HL_CATEGORY_APS + 3U)
#define NCP_HL_APSME_ADD_GROUP     (NCP_HL_CATEGORY_APS + 4U)
#define NCP_HL_APSME_RM_GROUP      (NCP_HL_CATEGORY_APS + 5U)
/* indication */
#define NCP_HL_APSDE_DATA_IND      (NCP_HL_CATEGORY_APS + 6U)
/* req/resp */
#define NCP_HL_APSME_RM_ALL_GROUPS (NCP_HL_CATEGORY_APS + 7U)
#define NCP_HL_APS_CHECK_BINDING   (NCP_HL_CATEGORY_APS + 8U)
#define NCP_HL_APS_GET_GROUP_TABLE (NCP_HL_CATEGORY_APS + 9U)
#define NCP_HL_APSME_UNBIND_ALL    (NCP_HL_CATEGORY_APS + 10U)

#define NCP_HL_APSME_GET_BIND_ENTRY_BY_ID     (NCP_HL_CATEGORY_APS + 11U)
#define NCP_HL_APSME_RM_BIND_ENTRY_BY_ID      (NCP_HL_CATEGORY_APS + 12U)
#define NCP_HL_APSME_CLEAR_BIND_TABLE         (NCP_HL_CATEGORY_APS + 13U)

  /* indication */
#define NCP_HL_APSME_REMOTE_BIND_IND          (NCP_HL_CATEGORY_APS + 14U)
#define NCP_HL_APSME_REMOTE_UNBIND_IND        (NCP_HL_CATEGORY_APS + 15U)

#define NCP_HL_SET_REMOTE_BIND_OFFSET         (NCP_HL_CATEGORY_APS + 16U)
#define NCP_HL_GET_REMOTE_BIND_OFFSET         (NCP_HL_CATEGORY_APS + 17U)
#define INTERNAL_APSDE_DATA_REQ_BY_BIND_TBL_ID  (NCP_HL_CATEGORY_APS + 18U)

#define NCP_HL_NWK_FORMATION                   (NCP_HL_CATEGORY_NWKMGMT + 1U)
#define NCP_HL_NWK_DISCOVERY                   (NCP_HL_CATEGORY_NWKMGMT + 2U)
#define NCP_HL_NWK_NLME_JOIN                   (NCP_HL_CATEGORY_NWKMGMT + 3U)
#define NCP_HL_NWK_PERMIT_JOINING              (NCP_HL_CATEGORY_NWKMGMT + 4U)
#define NCP_HL_NWK_GET_IEEE_BY_SHORT           (NCP_HL_CATEGORY_NWKMGMT + 5U)
#define NCP_HL_NWK_GET_SHORT_BY_IEEE           (NCP_HL_CATEGORY_NWKMGMT + 6U)
#define NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE        (NCP_HL_CATEGORY_NWKMGMT + 7U)
/* indications */
#define NCP_HL_NWK_STARTED_IND                 (NCP_HL_CATEGORY_NWKMGMT + 8U)
#define NCP_HL_NWK_REJOINED_IND                (NCP_HL_CATEGORY_NWKMGMT + 9U)
#define NCP_HL_NWK_REJOIN_FAILED_IND           (NCP_HL_CATEGORY_NWKMGMT + 10U)
#define NCP_HL_NWK_LEAVE_IND                   (NCP_HL_CATEGORY_NWKMGMT + 11U)
#define NCP_HL_GET_ED_KEEPALIVE_TIMEOUT        (NCP_HL_CATEGORY_NWKMGMT + 12U)
#define NCP_HL_SET_ED_KEEPALIVE_TIMEOUT        (NCP_HL_CATEGORY_NWKMGMT + 13U)
/* req/resp */
#define NCP_HL_PIM_SET_FAST_POLL_INTERVAL      (NCP_HL_CATEGORY_NWKMGMT + 14U)
#define NCP_HL_PIM_SET_LONG_POLL_INTERVAL      (NCP_HL_CATEGORY_NWKMGMT + 15U)
#define NCP_HL_PIM_START_FAST_POLL             (NCP_HL_CATEGORY_NWKMGMT + 16U)
#define NCP_HL_PIM_START_LONG_POLL             (NCP_HL_CATEGORY_NWKMGMT + 17U)
#define NCP_HL_PIM_START_POLL                  (NCP_HL_CATEGORY_NWKMGMT + 18U)
#define NCP_HL_PIM_SET_ADAPTIVE_POLL           (NCP_HL_CATEGORY_NWKMGMT + 19U)
#define NCP_HL_PIM_STOP_FAST_POLL              (NCP_HL_CATEGORY_NWKMGMT + 20U)
#define NCP_HL_PIM_STOP_POLL                   (NCP_HL_CATEGORY_NWKMGMT + 21U)
#define NCP_HL_PIM_ENABLE_TURBO_POLL           (NCP_HL_CATEGORY_NWKMGMT + 22U)
#define NCP_HL_PIM_DISABLE_TURBO_POLL          (NCP_HL_CATEGORY_NWKMGMT + 23U)
#define NCP_HL_NWK_GET_FIRST_NBT_ENTRY         (NCP_HL_CATEGORY_NWKMGMT + 24U)
#define NCP_HL_NWK_GET_NEXT_NBT_ENTRY          (NCP_HL_CATEGORY_NWKMGMT + 25U)
#define NCP_HL_NWK_PAN_ID_CONFLICT_RESOLVE     (NCP_HL_CATEGORY_NWKMGMT + 26U)
/* indications */
#define NCP_HL_NWK_PAN_ID_CONFLICT_IND         (NCP_HL_CATEGORY_NWKMGMT + 27U)
#define NCP_HL_NWK_ADDRESS_UPDATE_IND          (NCP_HL_CATEGORY_NWKMGMT + 28U)

/* req/resp */
#define NCP_HL_NWK_START_WITHOUT_FORMATION     (NCP_HL_CATEGORY_NWKMGMT + 29U)
#define NCP_HL_NWK_NLME_ROUTER_START           (NCP_HL_CATEGORY_NWKMGMT + 30U)
#define NCP_HL_PIM_SINGLE_POLL                 (NCP_HL_CATEGORY_NWKMGMT + 31U)

#define NCP_HL_PARENT_LOST_IND                 (NCP_HL_CATEGORY_NWKMGMT + 32U)

/* indications */
#define NCP_HL_NWK_ROUTE_REPLY_IND             (NCP_HL_CATEGORY_NWKMGMT + 33U)
#define NCP_HL_NWK_ROUTE_REQUEST_SEND_IND      (NCP_HL_CATEGORY_NWKMGMT + 34U)
#define NCP_HL_NWK_ROUTE_RECORD_SEND_IND       (NCP_HL_CATEGORY_NWKMGMT + 35U)

#define NCP_HL_PIM_START_TURBO_POLL_PACKETS    (NCP_HL_CATEGORY_NWKMGMT + 36U)
#define NCP_HL_PIM_START_TURBO_POLL_CONTINUOUS (NCP_HL_CATEGORY_NWKMGMT + 37U)
#define NCP_HL_PIM_TURBO_POLL_CONTINUOUS_LEAVE (NCP_HL_CATEGORY_NWKMGMT + 38U)
#define NCP_HL_PIM_TURBO_POLL_PACKETS_LEAVE    (NCP_HL_CATEGORY_NWKMGMT + 39U)
#define NCP_HL_PIM_PERMIT_TURBO_POLL           (NCP_HL_CATEGORY_NWKMGMT + 40U)
#define NCP_HL_PIM_SET_FAST_POLL_TIMEOUT       (NCP_HL_CATEGORY_NWKMGMT + 41U)
#define NCP_HL_PIM_GET_LONG_POLL_INTERVAL      (NCP_HL_CATEGORY_NWKMGMT + 42U)
#define NCP_HL_PIM_GET_IN_FAST_POLL_FLAG       (NCP_HL_CATEGORY_NWKMGMT + 43U)
#define NCP_HL_SET_KEEPALIVE_MODE              (NCP_HL_CATEGORY_NWKMGMT + 44U)
#define NCP_HL_START_CONCENTRATOR_MODE         (NCP_HL_CATEGORY_NWKMGMT + 45U)
#define NCP_HL_STOP_CONCENTRATOR_MODE          (NCP_HL_CATEGORY_NWKMGMT + 46U)

#define NCP_HL_NWK_ENABLE_PAN_ID_CONFLICT_RESOLUTION      (NCP_HL_CATEGORY_NWKMGMT + 47U)
#define NCP_HL_NWK_ENABLE_AUTO_PAN_ID_CONFLICT_RESOLUTION (NCP_HL_CATEGORY_NWKMGMT + 48U)

#define NCP_HL_PIM_TURBO_POLL_CANCEL_PACKET    (NCP_HL_CATEGORY_NWKMGMT + 49U)

#define NCP_HL_SECUR_SET_LOCAL_IC             (NCP_HL_CATEGORY_SECUR + 1U)
#define NCP_HL_SECUR_ADD_IC                   (NCP_HL_CATEGORY_SECUR + 2U)
#define NCP_HL_SECUR_DEL_IC                   (NCP_HL_CATEGORY_SECUR + 3U)
#define NCP_HL_SECUR_ADD_CERT                 (NCP_HL_CATEGORY_SECUR + 4U)
#define NCP_HL_SECUR_DEL_CERT                 (NCP_HL_CATEGORY_SECUR + 5U)
#define NCP_HL_SECUR_START_KE                 (NCP_HL_CATEGORY_SECUR + 6U)
#define NCP_HL_SECUR_START_PARTNER_LK         (NCP_HL_CATEGORY_SECUR + 7U)

/* indications */
#define NCP_HL_SECUR_CBKE_SRV_FINISHED_IND    (NCP_HL_CATEGORY_SECUR + 8U)
#define NCP_HL_SECUR_PARTNER_LK_FINISHED_IND  (NCP_HL_CATEGORY_SECUR + 9U)

/* req/resp */
#define NCP_HL_SECUR_JOIN_USES_IC             (NCP_HL_CATEGORY_SECUR + 10U)
#define NCP_HL_SECUR_GET_IC_BY_IEEE           (NCP_HL_CATEGORY_SECUR + 11U)
#define NCP_HL_SECUR_GET_CERT                 (NCP_HL_CATEGORY_SECUR + 12U)
#define NCP_HL_SECUR_GET_LOCAL_IC             (NCP_HL_CATEGORY_SECUR + 13U)

#define NCP_HL_SECUR_TCLK_IND                 (NCP_HL_CATEGORY_SECUR + 14U)
#define NCP_HL_SECUR_TCLK_EXCHANGE_FAILED_IND (NCP_HL_CATEGORY_SECUR + 15U)

/*req*/
#define NCP_HL_SECUR_KE_WHITELIST_ADD         (NCP_HL_CATEGORY_SECUR + 16U)
#define NCP_HL_SECUR_KE_WHITELIST_DEL         (NCP_HL_CATEGORY_SECUR + 17U)
#define NCP_HL_SECUR_KE_WHITELIST_DEL_ALL     (NCP_HL_CATEGORY_SECUR + 18U)

/* req/resp */
#define NCP_HL_SECUR_GET_KEY_IDX              (NCP_HL_CATEGORY_SECUR + 19U)
#define NCP_HL_SECUR_GET_KEY                  (NCP_HL_CATEGORY_SECUR + 20U)
#define NCP_HL_SECUR_ERASE_KEY                (NCP_HL_CATEGORY_SECUR + 21U)
#define NCP_HL_SECUR_CLEAR_KEY_TABLE          (NCP_HL_CATEGORY_SECUR + 22U)

#define NCP_HL_SECUR_NWK_INITIATE_KEY_SWITCH_PROCEDURE (NCP_HL_CATEGORY_SECUR + 23U)
#define NCP_HL_SECUR_GET_IC_LIST              (NCP_HL_CATEGORY_SECUR + 24U)
#define NCP_HL_SECUR_GET_IC_BY_IDX            (NCP_HL_CATEGORY_SECUR + 25U)
#define NCP_HL_SECUR_REMOVE_ALL_IC            (NCP_HL_CATEGORY_SECUR + 26U)
#define NCP_HL_SECUR_PARTNER_LK_ENABLE        (NCP_HL_CATEGORY_SECUR + 27U)

/* req/resp */
#define NCP_HL_MANUF_MODE_START               (NCP_HL_CATEGORY_MANUF_TEST + 1U)
#define NCP_HL_MANUF_MODE_END                 (NCP_HL_CATEGORY_MANUF_TEST + 2U)
#define NCP_HL_MANUF_SET_PAGE_AND_CHANNEL     (NCP_HL_CATEGORY_MANUF_TEST + 3U)
#define NCP_HL_MANUF_GET_PAGE_AND_CHANNEL     (NCP_HL_CATEGORY_MANUF_TEST + 4U)
#define NCP_HL_MANUF_SET_POWER                (NCP_HL_CATEGORY_MANUF_TEST + 5U)
#define NCP_HL_MANUF_GET_POWER                (NCP_HL_CATEGORY_MANUF_TEST + 6U)
#define NCP_HL_MANUF_START_TONE               (NCP_HL_CATEGORY_MANUF_TEST + 7U)
#define NCP_HL_MANUF_STOP_TONE                (NCP_HL_CATEGORY_MANUF_TEST + 8U)
#define NCP_HL_MANUF_START_STREAM_RANDOM      (NCP_HL_CATEGORY_MANUF_TEST + 9U)
#define NCP_HL_MANUF_STOP_STREAM_RANDOM       (NCP_HL_CATEGORY_MANUF_TEST + 10U)
#define NCP_HL_MANUF_SEND_SINGLE_PACKET       (NCP_HL_CATEGORY_MANUF_TEST + 11U)
#define NCP_HL_MANUF_START_TEST_RX            (NCP_HL_CATEGORY_MANUF_TEST + 12U)
#define NCP_HL_MANUF_STOP_TEST_RX             (NCP_HL_CATEGORY_MANUF_TEST + 13U)
/* indication */
#define NCP_HL_MANUF_RX_PACKET_IND            (NCP_HL_CATEGORY_MANUF_TEST + 14U)
/* req/resp */
#define NCP_HL_MANUF_CALIBRATION              (NCP_HL_CATEGORY_MANUF_TEST + 15U)

/* req/resp */
#define NCP_HL_OTA_RUN_BOOTLOADER (NCP_HL_CATEGORY_OTA + 1U)

/* req/resp */
#define NCP_HL_READ_NVRAM_RESERVED  (NCP_HL_CATEGORY_CUSTOM_COMMANDS + 1U)
#define NCP_HL_WRITE_NVRAM_RESERVED (NCP_HL_CATEGORY_CUSTOM_COMMANDS + 2U)

/* req/resp */
#define NCP_HL_INTRP_DATA_REQ      (NCP_HL_CATEGORY_INTRP + 1U)
/* indication */
#define NCP_HL_INTRP_DATA_IND      (NCP_HL_CATEGORY_INTRP + 2U)
/** @} */

/**
 * @brief Type for possible values of NCP HL call codes.
 *
 * Holds one of @ref hl_call_code. Kept only for backward
 * compatibility as @ref hl_call_code were declared previously as enum. Can be
 * removed in future releases.
 */
typedef zb_uint_t ncp_hl_call_code_t;


/**
 * @name NCP configuration parameters ID's for NCP_HL_GET_CONFIG_PARAMETER call code
 * @anchor ncp_hl_config_parameter
 */
/** @{ */
#define NCP_CP_IEEE_ADDR_TABLE_SIZE           1U
#define NCP_CP_NEIGHBOR_TABLE_SIZE            2U
#define NCP_CP_APS_SRC_BINDING_TABLE_SIZE     3U
#define NCP_CP_APS_GROUP_TABLE_SIZE           4U
#define NCP_CP_NWK_ROUTING_TABLE_SIZE         5U
#define NCP_CP_NWK_ROUTE_DISCOVERY_TABLE_SIZE 6U
#define NCP_CP_IOBUF_POOL_SIZE                7U
#define NCP_CP_PANID_TABLE_SIZE               8U
#define NCP_CP_APS_DUPS_TABLE_SIZE            9U
#define NCP_CP_APS_BIND_TRANS_TABLE_SIZE      10U
#define NCP_CP_N_APS_RETRANS_ENTRIES          11U
#define NCP_CP_NWK_MAX_HOPS                   12U
#define NCP_CP_NIB_MAX_CHILDREN               13U
#define NCP_CP_N_APS_KEY_PAIR_ARR_MAX_SIZE    14U
#define NCP_CP_NWK_MAX_SRC_ROUTES             15U
#define NCP_CP_APS_MAX_WINDOW_SIZE            16U
#define NCP_CP_APS_INTERFRAME_DELAY           17U /*!< in milliseconds */
#define NCP_CP_ZDO_ED_BIND_TIMEOUT            18U /*!< in seconds */
#define NCP_CP_NIB_PASSIVE_ASK_TIMEOUT        19U /*!< 16 bit, in milliseconds */
#define NCP_CP_APS_ACK_TIMEOUTS               20U /*!< 2 x 16 bit, in milliseconds */
#define NCP_CP_MAC_BEACON_JITTER              21U /*!< 2 x 16 bit, in milliseconds */
#define NCP_CP_TX_POWER                       22U /*!< 2 x 8 bit, signed */
#define NCP_CP_ZLL_DEFAULT_RSSI_THRESHOLD     23U /*!< 8 bit */
#define NCP_CP_NIB_MTORR                      24U /*!< 2 x 8 bit + 1 x 16 bit (in seconds) */
/** @} */

/**
 * @brief Type for NCP configuration parameters ID's for NCP_HL_GET_CONFIG_PARAMETER call code.
 *
 * Holds one of @ref ncp_hl_config_parameter. Kept only for backward compatibility as
 * @ref ncp_hl_config_parameter were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t ncp_hl_config_parameter_t;

typedef ZB_PACKED_PRE struct
{
  zb_uint8_t    version;
  zb_uint8_t    control;
  zb_uint16_t   call_id;
} ZB_PACKED_STRUCT
ncp_hl_header_t;

typedef ncp_hl_header_t ncp_hl_ind_header_t;

#define NCP_HL_PROTO_VERSION 0U

#define NCP_HL_GET_PKT_TYPE(control) ((control) & 3U)

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

typedef ZB_PACKED_PRE struct ncp_hl_response_partner_lk_s
{
  zb_uint8_t      key[ZB_CCM_KEY_SIZE];
  zb_uint8_t      aps_link_key_type;
  zb_uint8_t      key_source;
  zb_uint8_t      key_attributes;
  zb_uint32_t     outgoing_frame_counter;
  zb_uint32_t     incoming_frame_counter;
  zb_ieee_addr_t  partner_address;

} ZB_PACKED_STRUCT
ncp_hl_response_partner_lk_t;

/**
 * @name Binding types
 * @anchor bind_type
 */
/** @{ */
#define NCP_HL_UNUSED_BINDING    0x00
#define NCP_HL_UNICAST_BINDING   0x01
/** @} */



typedef ZB_PACKED_PRE struct ncp_hl_bind_entry_s
{
  zb_uint8_t      src_endpoint;
  zb_uint16_t     cluster_id;
  zb_uint8_t      dst_addr_mode;
  ZB_PACKED_PRE union
  {
    zb_uint16_t     short_addr;
    zb_ieee_addr_t  long_addr;
  } u;

  zb_uint8_t dst_endpoint;
  zb_uint8_t id;
  zb_uint8_t bind_type;

} ZB_PACKED_STRUCT
ncp_hl_bind_entry_t;


/**
 * @name NCP HL reset options
 * @anchor ncp_hl_reset_opt
 */
/** @{ */
#define NO_OPTIONS         0U
#define NVRAM_ERASE        1U
#define FACTORY_RESET      2U
#define BLOCK_READING_KEYS 3U
/** @} */

/**
 * @brief Type for NCP HL reset options.
 *
 * Holds one of @ref ncp_hl_reset_opt. Kept only for backward compatibility as
 * @ref ncp_hl_reset_opt were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t ncp_hl_reset_opt_t;


#define NCP_HL_ZDO_LEAVE_FLAG_REMOVE_CHILDREN 6U
#define NCP_HL_ZDO_LEAVE_FLAG_REJOIN          7U

#define NCP_HL_GET_JOINED_FLAG_JOINED         (1U << 0U)
#define NCP_HL_GET_JOINED_FLAG_PARENT_LOST    (1U << 1U)

/**
 * @name NCP LL trace options
 * @anchor ll_trace
 *
 * Note: These values were members of `enum ncp_ll_trace_e` type but were
 * converted to a set of macros due to MISRA violations.
 */
/** @{ */
#define NCP_TRACE_TRAFFIC  1U
#define NCP_TRACE_COMMON   2U
#define NCP_TRACE_LL_PROTO 4U
#define NCP_TRACE_HOST_INT 8U /* HOST_INT line activity */
#define NCP_TRACE_SLEEP    16U
/** @} */

typedef ZB_PACKED_PRE struct ncp_hl_nvram_write_req_hdr_s
{
  zb_uint8_t dataset_qnt;
} ZB_PACKED_STRUCT
ncp_hl_nvram_write_req_hdr_t;

typedef ZB_PACKED_PRE struct ncp_hl_nvram_write_req_ds_hdr_s
{
  zb_uint16_t type;
  zb_uint16_t version;
  zb_uint16_t len;
} ZB_PACKED_STRUCT
ncp_hl_nvram_write_req_ds_hdr_t;

typedef ZB_PACKED_PRE struct ncp_hl_nvram_read_resp_ds_hdr_s
{
  zb_uint16_t nvram_version;
  zb_uint16_t type;
  zb_uint16_t version;
  zb_uint16_t len;
} ZB_PACKED_STRUCT
ncp_hl_nvram_read_resp_ds_hdr_t;


#define NCP_HL_TC_POLICY_TC_LINK_KEYS_REQUIRED           0x0000U
#define NCP_HL_TC_POLICY_IC_REQUIRED                     0x0001U
#define NCP_HL_TC_POLICY_TC_REJOIN_ENABLED               0x0002U
#define NCP_HL_TC_POLICY_IGNORE_TC_REJOIN                0x0003U
#define NCP_HL_TC_POLICY_APS_INSECURE_JOIN               0x0004U
#define NCP_HL_TC_POLICY_DISABLE_NWK_MGMT_CHANNEL_UPDATE 0x0005U

#define ZB_NCP_SERIAL_NUMBER_LENGTH 16U
#define ZB_NCP_MAX_VENDOR_DATA_SIZE 64U

#ifdef ZB_HAVE_CALIBRATION
void ncp_perform_calibration(void);
#endif

zb_uint16_t
ncp_hl_fill_resp_hdr(
  ncp_hl_response_header_t **rh,
  zb_uint_t resp_code,
  zb_uint8_t tsn,
  zb_ret_t status,
  zb_uint_t body_size);

#endif /* NCP_HL_PROTO_COMMON_H */
