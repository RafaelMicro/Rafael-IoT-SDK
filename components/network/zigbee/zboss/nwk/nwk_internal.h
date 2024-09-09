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
/* PURPOSE: NWK layer internals
*/

#ifndef NWK_INTERNAL_H
#define NWK_INTERNAL_H 1

#include "zb_nwk.h"

/*! \addtogroup ZB_NWK */
/*! @{ */

#define NWK_RETRANSMIT_COUNTDOWN_QUANT 1U

/**
  Check parameter on error
 */
#define CHECK_PARAM_RET_ON_ERROR(param)             \
do                                                  \
{                                                   \
  if ( param == NULL )                              \
  {                                                 \
    TRACE_MSG(TRACE_NWK1, "<< ret invalid param", (FMT__0));  \
    ZB_ASSERT(0);                                   \
    return;                                         \
  }                                                 \
} while (0)


#ifndef ZB_LITE_NO_NLME_ROUTE_DISCOVERY
/**
   Common confirm status routine
 */
void nwk_route_discovery_confirm(zb_bufid_t buf, zb_uint8_t cfm_status);
#endif

/**
  Return pointer to free entry from array and increment used elements counter
  */
#define NWK_ARRAY_GET_ENT(array, ent, cnt) do                            \
{                                                                        \
  zb_ushort_t ii;                                                        \
  (ent) = NULL;                                                          \
  for (ii = 0; ii < ZB_ARRAY_SIZE(array); ii++)                          \
  {                                                                      \
    if ( !ZB_U2B(array[ii].used) )                              \
    {                                                                    \
      (ent) = &(array)[ii];                                              \
      (cnt)++;                                                           \
      (array)[ii].used = ZB_TRUE_U;                       \
      break;                                                             \
    }                                                                    \
  }                                                                      \
} while (0)

/**
  Mark array entry as free and decrements used element counter
  */
#define NWK_ARRAY_PUT_ENT(array, ent, cnt) do     \
{                                                 \
  if ( ent != NULL )                              \
  {                                               \
    (ent)->used = ZB_FALSE_U;                              \
    (cnt)--;                                      \
  }                                               \
} while(0)

/**
  Find used entry with specified parameters in array
*/
#define NWK_ARRAY_FIND_ENT(array, array_size, ent, condition) do        \
{                                                                       \
  zb_ushort_t ii;                                                       \
  (ent) = NULL;                                                         \
  for (ii = 0; ii < (array_size); ii++)                                 \
  {                                                                     \
    (ent) = &(array)[ii];                                               \
    if (ZB_U2B((ent)->used)                                             \
         && (condition) )                                               \
    {                                                                   \
      break;                                                            \
    }                                                                   \
  }                                                                     \
  if (ii >= (array_size))                                               \
  {                                                                     \
    (ent) = NULL;                                                       \
  }                                                                     \
} while (0)

/**
  Free all array entries
*/
#define NWK_ARRAY_CLEAR(array, array_size, cnt) do                      \
{                                                                       \
  zb_ushort_t ii;                                                       \
  for (ii = 0 ; ii < (array_size); ii++)                                \
  {                                                                     \
    (array)[ii].used = ZB_FALSE_U;                                \
  }                                                                     \
  (cnt) = 0U;                                                           \
} while(0)


/* Get some fields from nwk header which is just at buffer begin.
   Note that nwk header is packed, so it is safe to ignore alignment.
 */
#define NWK_HDR_FROM_BUF(param) ((zb_nwk_hdr_t *)zb_buf_begin(param))
#define NWK_SRC_FROM_BUF(param) NWK_HDR_FROM_BUF(param)->src_addr
#define NWK_DST_FROM_BUF(param) NWK_HDR_FROM_BUF(param)->dst_addr
#define NWK_SEQ_FROM_BUF(param) NWK_HDR_FROM_BUF(param)->seq_num

/**
   Searches for next channel page and channel mask to scan.

   This function iterates over all MAC iface table records starting with
   passed iface_start_idx, sequentially iterates over scan_channel_list
   starting with channel_page_start_idx matching the channel mask of iface
   supported channels and channel mask in scan_channel_list for corresponding
   channel page until the first non-zero resulting scan channel mask is found.
   Resulting channel mask considers channel mask supported by MAC iface.

   NOTE: this function also may be used for validation of scan_channel_list
   (ScanChannelListStructure) according to r22 3.2.2.2.2 "Validating the List
   of Channels" if iface_start_idx and channel_page_start_idx are set to 0:
     - RET_OK means validation passed;
     - RET_NOT_FOUND means validation failed (no non-zero resulting channel mask
       found).

   @param iface_start_idx - index of record in MAC iface table to start iteration
   @param channel_page_start_idx - index of record in scan_channels_list to start
                                   iteration for start MAC iface.
   @param scan_channels_list - ScanChannelListStructure for scan
   @param iface_idx [out] - resulting index of record in MAC iface table
   @param channel_page [out] - channel page to scan
   @param channel_mask [out] - resulting channel mask to scan (considering
                               channels supported by MAC iface)

   @return RET_OK if non-zero resulting channel mask found
           RET_NOT_FOUND if no non-zero resulting channel mask found
 */
zb_ret_t nwk_scan_find_next_channel_mask(zb_uint8_t iface_start_idx,
                                         zb_uint8_t channel_page_start_idx,
                                         zb_channel_list_t scan_channels_list,
                                         zb_uint8_t *iface_idx,
                                         zb_uint8_t *channel_page,
                                         zb_uint32_t *channel_mask);

/**
   Builds and initiates MLME scan request.

   @param param - buffer for further operations
   @param scan_type - scan type
   @param scan_duration - scan duration (saved in NWK handle)
   @param scan_iface_idx - index of iface in MAC iface table (saved in NWK handle)
   @param scan_channel_page - channel page to scan
   @param scan_channel_mask - channel mask to scan
 */
void nlme_scan_request(zb_uint8_t  param,
                       zb_uint8_t  scan_type,
                       zb_uint8_t  scan_duration,
                       zb_uint8_t  scan_iface_idx,
                       zb_uint8_t  scan_channel_page,
                       zb_uint32_t scan_channel_mask);


#ifdef ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN

/**
   zb_nwk_daa_calc_cskip

   See 3.6.1.6
   Calculate Cskip value based on the node depth. This value wiil be used to
   determine the address of any joined device and also determining whether to
   route up or down in tree based routing. This function called once after
   nwk_formation or nwk_join

   @param depth - depth of the node inside tree
   @return Cskip value
 */
zb_uint16_t zb_nwk_daa_calc_cskip(zb_uint8_t depth );


/**
   Calculate address for child router. See 3.6.1.6
 */
#define ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN_ROUTER_ADDRESS() ( ZB_PIBCACHE_NETWORK_ADDRESS() + ZB_NIB().router_child_num*ZB_NIB().cskip + 1 )

/**
   Calculate address for child ed. See 3.6.1.6
 */
#define ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN_ED_ADDRESS() ( ZB_PIBCACHE_NETWORK_ADDRESS() + ZB_NIB().max_routers*ZB_NIB().cskip + ZB_NIB().ed_child_num + 1 )


/**
   Calculate address for child router. See 3.6.1.6
 */
#define ZB_NWK_DISTRIBUTED_ROUTER_ADDRESS_VERIFY(addr) (                \
(addr) > ZB_PIBCACHE_NETWORK_ADDRESS()                                  \
&&                                                                      \
(addr) < ZB_PIBCACHE_NETWORK_ADDRESS() + ZB_NIB().max_routers * ZB_NIB().cskip + 1U)

/**
   Calculate address for child ed. See 3.6.1.6
 */
#define ZB_NWK_DISTRIBUTED_ED_ADDRESS_VERIFY(addr) (                        \
(addr) > ZB_PIBCACHE_NETWORK_ADDRESS()                                  \
&&                                                                      \
(addr) < ZB_PIBCACHE_NETWORK_ADDRESS() + ZB_NIB().max_children * ZB_NIB().cskip + ZB_NIB().ed_child_num + 1U)


#else
//  #error Implement Stochastic address assign mechanism

/*
  zb_nwk_get_stoch_addr - Stochastic Address Assignment
  See 3.6.1.7

  Generate new short address.
  New address not equal current or one of known address (ex. not equal coordinator address and
  has not in address map)

  @return new address
*/
zb_uint16_t zb_nwk_get_stoch_addr(void);


/**
   Get address for child router
 */
#define ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN_ROUTER_ADDRESS() zb_nwk_get_stoch_addr()

/**
   Get address for child ed
 */

#define ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN_ED_ADDRESS()  zb_nwk_get_stoch_addr()



#endif /* ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN */


#if defined(MAC_CERT_TEST_HACKS)
#define ZB_NWK_ROUTER_ADDRESS_ASSIGN()  ZB_PREDEFINED_ROUTER_ADDR
#define ZB_NWK_ED_ADDRESS_ASSIGN()      ZB_PREDEFINED_ED_ADDR
#else
#define ZB_NWK_ROUTER_ADDRESS_ASSIGN    ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN_ROUTER_ADDRESS
#define ZB_NWK_ED_ADDRESS_ASSIGN        ZB_NWK_DISTRIBUTED_ADDRESS_ASSIGN_ED_ADDRESS
#endif  /* MAC_CERT_TEST_HACKS */

#ifdef ZB_NWK_TREE_ROUTING
/**
   zb_nwk_tree_routing_init

   Initialize tree route subsystem.

   @return nothing
 */
void zb_nwk_tree_routing_init();

/**
   zb_nwk_tree_routing_route

   Find next hop to destination

   @param dest_address - address of the destination device
   @return Neighbor table entry to route packet to, NULL on error
 */
zb_neighbor_tbl_ent_t *zb_nwk_tree_routing_route(zb_uint16_t dest_address);
#endif /* ZB_NWK_TREE_ROUTING */

#ifdef ZB_NWK_MESH_ROUTING
//#if 1
/**
   zb_nwk_mesh_routing_init

   Initialize mesh route subsystem.

   @return nothing
 */
void zb_nwk_mesh_routing_init(void);

/**
   zb_nwk_mesh_routing_deinit

   Deinitialize mesh route subsystem.

   @return nothing
 */
void zb_nwk_mesh_routing_deinit(void);

/**
   zb_nwk_mesh_route_discovery

   Sets up new route discovery operation. This device will be route request
   originator.

   @param cbuf - output buffer
   @param dest_address - address of the destination device
   @param radius - route discovery radius, 0 - use default radius
   @param rreq_type - route request type (@see @ref nwk_rreq_type)
   @return nothing
 */
void zb_nwk_mesh_route_discovery(zb_bufid_t cbuf, zb_uint16_t dest_addr, zb_uint8_t radius, zb_uint8_t rreq_type);


/**
   zb_nwk_mesh_rreq_handler

   Process an incoming route request and decide if it needs to be forwarded
   or a route reply needs to be generated

   @param buf - buffer with incoming route request packet
   @param nwk_hdr - pointer to the network header
   @param nwk_cmd_rreq - pointer to the network route request header
   @return nothing
 */
void zb_nwk_mesh_rreq_handler(zb_bufid_t buf, zb_nwk_hdr_t *nwk_hdr, zb_nwk_cmd_rreq_t *nwk_cmd_rreq);

/**
   zb_nwk_mesh_rrep_handler

   Process an incoming route reply

   @param buf - buffer with incoming route request packet
   @param nwk_hdr - pointer to the network header
   @param nwk_cmd_rrep - pointer to the network route reply header
   @return nothing
 */
void zb_nwk_mesh_rrep_handler(zb_bufid_t buf, zb_nwk_hdr_t *nwk_hdr, zb_nwk_cmd_rrep_t *nwk_cmd_rrep);

/**
   zb_nwk_mesh_find_route

   Search for the route

   @param dest_addr - packet destination address
   @param is_multicast - packet type if multicast - 1
   @return routing table entry if route exists, NULL - otherwise (pro stack version)
 */
zb_nwk_routing_t *zb_nwk_mesh_find_route(zb_uint16_t dest_addr);

/**
   zb_nwk_mesh_find_route_discovery_entry

   Search for the route discovery entry

   @param dest_addr - packet destination address
   @return route discovery table entry if exists, NULL - otherwise
 */
zb_nwk_route_discovery_t *zb_nwk_mesh_find_route_discovery_entry(zb_uint16_t dest_addr);

/**
   zb_nwk_mesh_add_buf_to_pending

   Add buffer to the pending list, to send it after route will be found

   @param buf - packet buffer
   @param handle - nsdu packet handle
   @return RET_OK on success, error code otherwise
 */
zb_ret_t zb_nwk_mesh_add_buf_to_pending(zb_uint8_t param, zb_uint8_t handle);

/**
  zb_nwk_calc_path_cost

  Calculate cost from src_addr to this device

  @param src_addr - or me or neighbor short address
  @return path cost from src_addr
 */
zb_uint8_t zb_nwk_calc_path_cost(zb_uint16_t src_addr);

/**
  zb_nwk_calc_path_cost

  Delete address entry from routing table

  @param addr - destintatin address, whose entry to be deleted
  @return path cost from src_addr
 */
void zb_nwk_mesh_delete_route(zb_uint16_t addr);

#endif /* ZB_NWK_MESH_ROUTING */

/**
   zb_nwk_forward

   @param param - buffer with data for forward to MAC layer
   @return nothing
 */
void zb_nwk_forward(zb_uint8_t param);

/**
   zb_nlme_rejoin_scan_confirm

   @param param - buffer with scan result
   @return nothing
 */
void zb_nlme_rejoin_scan_confirm(zb_uint8_t param);

/**
   zb_nlme_rejoin_response

   @param param - buffer with rejoin response data
   @return nothing
 */
void zb_nlme_rejoin_response(zb_uint8_t param);

/**
   zb_nlme_rejoin_response_timeout

   @param param - buffer
   @return nothing
 */
void zb_nlme_rejoin_response_timeout(zb_uint8_t param);

/**
   handle incoming zb_nlme_rejoin_request

   @param param - buffer
   @return nothing
 */
void zb_nlme_rejoin_request_handler(zb_uint8_t param);

/**
   zb_nlme_rejoin_resp_sent

   @param param - buffer
   @return nothing
 */
void zb_nlme_rejoin_resp_sent(zb_uint8_t param);

/**
   zb_nlme_orphan_scan_confirm

   @param param - buffer
   @return nothing
 */
void zb_nlme_orphan_scan_confirm(zb_uint8_t param);

/* This handler is used to data transmission with call nlde-data-confirm */
#define ZB_NWK_NON_INTERNAL_NSDU_HANDLE                      0x00U
/* This handles are used to transfer service nwk data. No confirm should be called
 * under NWK layer */
#define ZB_NWK_INTERNAL_NSDU_HANDLE         0xFFU
#define ZB_NWK_INTERNAL_REJOIN_CMD_HANDLE   0xFAU
#define ZB_NWK_INTERNAL_REJOIN_CMD_RESPONSE 0xFDU
/*!< When got data.confirm, do actual leave, then call leave.indication and, maybe, rejoin  */
#define ZB_NWK_INTERNAL_LEAVE_IND_AT_DATA_CONFIRM_HANDLE     0xFCU
/*!< When got data.confirm, send responce to mgmt_leave command. Maybe, leave later  */
#define ZB_NWK_INTERNAL_LEAVE_CONFIRM_AT_DATA_CONFIRM_HANDLE   0xFBU
/* This handle is used to transfer OMM status. */
#define ZB_NWK_INTERNAL_OMM_STATUS_CONFIRM_HANDLE            0xF9U
/* Detect delivery status of relayed frames */
#define ZB_NWK_INTERNAL_RELAYED_UNICAST_FRAME_CONFIRM_HANDLE 0xF8U
/* Detect delivery of NWK End Device Timeout Request frame */
#define ZB_NWK_INTERNAL_ED_TIMEOUT_REQ_FRAME_COFIRM_HANDLE   0xF7U
#define ZB_NWK_MINIMAL_INTERNAL_HANDLE ZB_NWK_INTERNAL_ED_TIMEOUT_REQ_FRAME_COFIRM_HANDLE

/**
   remove_parent_from_potential_parents
   Remove parent from potential parents list

   @param parent - parent for remove

   @return nothing
 */
void remove_parent_from_potential_parents(zb_ext_neighbor_tbl_ent_t *parent);

void zb_panid_conflict_network_update(zb_uint8_t param);

/**
   zb_panid_conflict_send_nwk_update

   @return nothing
 */
void zb_panid_conflict_send_nwk_update(zb_uint8_t param);


/* VP: zb_nwk_addr_conflict_compare is removed */

void zb_nwk_address_conflict_start_process(zb_uint8_t addr_ref_param);
void zb_nwk_address_conflict_send_report(zb_uint8_t param, zb_uint16_t addr_ref_param);
void zb_nwk_change_me_addr(zb_uint8_t param);
void zb_nwk_change_my_child_addr(zb_uint8_t param, zb_neighbor_tbl_ent_t *nbt);
void zb_nwk_change_my_child_addr_process(zb_uint8_t param);
zb_ret_t zb_nwk_address_conflict_resolve(zb_uint8_t param, zb_uint16_t addr);

/*
   Test all input message on Conflict Address
 */
zb_ret_t zb_nwk_test_addresses(zb_uint8_t param);

/*
 * Link Status command frame
 */
#if defined ZB_PRO_STACK && defined ZB_ROUTER_ROLE    /* Zigbee pro and router */

void zb_nwk_link_status_alarm(zb_uint8_t param);
void zb_nwk_add_age_neighbor(void);
void zb_nwk_receive_link_status_command(zb_bufid_t buf, zb_nwk_hdr_t *nwk_hdr, zb_uint8_t hdr_size);
zb_uint8_t zb_nwk_prepare_link_status_command(zb_uint8_t *dt, zb_uint8_t max_count);
void zb_nwk_send_link_status_command(zb_uint8_t param);

#endif /* Zigbee pro */


/*
  Alloc and fill nwk hdr, return pointer to the allocated hdr
*/
zb_nwk_hdr_t *nwk_alloc_and_fill_hdr(zb_bufid_t buf,
                                     zb_uint16_t src_addr,
                                     zb_uint16_t dst_addr,
                                     zb_bool_t is_multicast, zb_bool_t is_secured,
                                     zb_bool_t is_cmd_frame, zb_bool_t force_long);

/*
  Get source ieee address from nwk header
*/
zb_ieee_addr_t *zb_nwk_get_src_long_from_hdr(zb_nwk_hdr_t *nwhdr);

/**
  Alloc and fill place for nwk command, return pointer to the command payload.
  'cmd' is a value from @ref nwk_cmd.
*/
void *nwk_alloc_and_fill_cmd(zb_bufid_t buf, zb_uint8_t cmd, zb_uint8_t cmd_size);

void nwk_next_rejoin_poll(zb_uint8_t param);

zb_bool_t zb_nwk_check_assigned_short_addr(zb_uint16_t short_addr);

/**
  Fill packet parameters (that will be passed to apsde) with default values and assign handle;
  params here is pointer
 */
zb_apsde_data_ind_params_t * zb_nwk_init_apsde_data_ind_params(zb_bufid_t buf, zb_uint8_t handle);

/* Load NWK settings into PIB */
void zb_nwk_load_pib_stm(zb_uint8_t param);
#if !(defined ZB_ED_ROLE && defined ZB_ED_RX_OFF_WHEN_IDLE)
/*
  Clear Broadcast transaction records for the address
*/
void zb_nwk_clear_btr_for_address(zb_address_ieee_ref_t addr_ref);
#define ZB_NWK_CLEAR_BTR_FOR_ADDRESS(addr_ref) zb_nwk_clear_btr_for_address(addr_ref)
#else
#define ZB_NWK_CLEAR_BTR_FOR_ADDRESS(addr_ref)
#endif /* !(defined ZB_ED_ROLE && defined ZB_ED_RX_OFF_WHEN_IDLE) */

#define ZB_NWK_RANDOM_TIME_PERIOD(base, random_jtr) ((base) + ZB_RANDOM_JTR(random_jtr))

#if defined ZB_MAC_POWER_CONTROL
/* Send NWK Link Power Delta command */
void zb_nwk_link_power_delta_alarm(zb_uint8_t param);

/* Handle received Link Power Delta command */
void zb_nwk_handle_link_power_delta_command(zb_bufid_t buf,
                                            zb_nwk_hdr_t *nwk_hdr,
                                            zb_uint8_t hdr_size);
#endif /* ZB_MAC_POWER_CONTROL */

void zb_nwk_ed_timeout_req_frame_confirm(zb_uint8_t param);

void nwk_call_nlde_data_confirm(zb_bufid_t param, zb_nwk_status_t status, zb_bool_t has_nwk_hdr);

zb_uint16_t nwk_get_pkt_mac_source(zb_bufid_t b);

#if !(defined ZB_ED_ROLE && defined ZB_ED_RX_OFF_WHEN_IDLE)
/**
 * Adds an entry to the BTT table.
 * Note that this function does not check whether such entry already exists.
 * @return ZB_TRUE if the entry was successfully added
 */
zb_bool_t zb_nwk_broadcasting_add_btr(zb_nwk_hdr_t *nwk_hdr);

/**
 * Finds a BTR entry corresponding to the NWK header.
 * @return apointer to the BTR entry or NULL.
 */
zb_nwk_btr_t *zb_nwk_broadcasting_find_btr(const zb_nwk_hdr_t *nwk_hdr);
#endif

#ifdef ZB_ROUTER_ROLE
/**
 * Start the process of broadcast transmission.
 */
void zb_nwk_broadcasting_transmit(zb_uint8_t param);
/**
 * This function keeps track of the number of passive acks for the broadcast transmission.
 */
void zb_nwk_broadcasting_mark_passive_ack(zb_uint16_t mac_src_addr, zb_nwk_hdr_t *nwk_hdr);

#endif

/**
 * Clear BTT table and stop all ongoing broadcasting proceses.
 */
void zb_nwk_broadcasting_clear(void);

/**
 * Clear BTT table and stop all ongoing broadcasting proceses.
 * Clear pending table and routing table
 */
void nwk_internals_clear(void);

zb_nwk_routing_t *new_routing_table_ent(void);

/* GP add, start */
zb_uint8_t zb_nwk_routing_table_size(void);
/* GP add, end */

/*! @} */

#endif /* NWK_INTERNAL_H */
