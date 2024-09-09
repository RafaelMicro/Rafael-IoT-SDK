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
/* PURPOSE: MAC pending bit source matching
*/

#ifndef MAC_SOURCE_MATCHING_INCLUDED
#define MAC_SOURCE_MATCHING_INCLUDED 1

#include "zb_common.h"
/*! \addtogroup ZB_MAC */
/*! @{ */

#ifdef ZB_MAC_PENDING_BIT_SOURCE_MATCHING

#if (defined ZB_MAC_SOFTWARE_PB_MATCHING)
typedef enum source_matching_search_action_e
{
  SEARCH_ACTION_NO_ACTION,
  SEARCH_ACTION_ALLOW_CREATE,
  SEARCH_ACTION_DELETE_IF_EXIST
} source_matching_search_action_t;

zb_bool_t mac_child_hash_table_search(zb_uint16_t short_addr,
                                      zb_uint32_t *ref_p,
                                      zb_bool_t   called_from_interrupt,
                                      source_matching_search_action_t action);
zb_uint8_t mac_software_src_match_add_short_addr(zb_uint16_t short_addr);
zb_uint8_t mac_software_src_match_delete_short_addr(zb_uint16_t short_addr);
void mac_software_src_match_tbl_drop(void);
void mac_software_src_match_short_set_pending_bit(zb_uint16_t short_addr, zb_uint8_t pending);

/* Call functions via macro to choose between SW and HW implementations of source address & pending bit matching */

#define ZB_SRC_MATCH_ADD_SHORT_ADDR(index, short_addr)  \
  mac_software_src_match_add_short_addr(short_addr)
#define ZB_SRC_MATCH_DELETE_SHORT_ADDR(index, short_addr) \
  mac_software_src_match_delete_short_addr(short_addr)
#define ZB_SRC_MATCH_TBL_DROP() \
  mac_software_src_match_tbl_drop()
#define ZB_SRC_MATCH_SHORT_SET_PENDING_BIT(short_addr, pending) \
  mac_software_src_match_short_set_pending_bit(short_addr, pending)
#define ZB_SRC_MATCH_ADD_IEEE_ADDR(index, ieee_addr) (MAC_SUCCESS)
#define ZB_SRC_MATCH_DELETE_IEEE_ADDR(index, ieee_addr) (MAC_SUCCESS)
#define ZB_SRC_MATCH_IEEE_SET_PENDING_BIT(ieee_addr, pending)

#ifdef ZB_MAC_POLL_INDICATION_CALLS_REDUCED
void mac_software_src_match_update_poll_ind_call_timeout(zb_uint16_t short_addr, zb_uint16_t poll_timeout);

#define ZB_MAC_UPDATE_POLL_IND_CALL_TIMEOUT(index, short_addr, tmo)     \
  mac_software_src_match_update_poll_ind_call_timeout(short_addr, tmo)
#endif /* ZB_MAC_POLL_INDICATION_CALLS_REDUCED */

#elif (defined ZB_MAC_STICKY_PENDING_BIT) /* #if !ZB_MAC_SOFTWARE_PB_MATCHING */

#define ZB_SRC_MATCH_ADD_SHORT_ADDR(index, short_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_ADD_SHORT_ADDR(index, short_addr)
#define ZB_SRC_MATCH_DELETE_SHORT_ADDR(index, short_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_DELETE_SHORT_ADDR(index, short_addr)
#define ZB_SRC_MATCH_TBL_DROP() \
  ZB_TRANSCEIVER_SRC_MATCH_TBL_DROP()
#define ZB_SRC_MATCH_SHORT_SET_PENDING_BIT(short_addr, pending) \
  ZB_TRANSCEIVER_SRC_MATCH_SHORT_SET_PENDING_BIT(short_addr, pending)
#define ZB_SRC_MATCH_ADD_IEEE_ADDR(index, ieee_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_ADD_IEEE_ADDR(index, ieee_addr)
#define ZB_SRC_MATCH_DELETE_IEEE_ADDR(index, ieee_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_DELETE_IEEE_ADDR(index, ieee_addr)
#define ZB_SRC_MATCH_IEEE_SET_PENDING_BIT(ieee_addr, pending) \
  ZB_TRANSCEIVER_SRC_MATCH_IEEE_SET_PENDING_BIT(ieee_addr, pending)

#elif (defined ZB_MAC_HARDWARE_PB_MATCHING)

#define ZB_SRC_MATCH_ADD_SHORT_ADDR(index, short_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_ADD_SHORT_ADDR(index, short_addr)
#define ZB_SRC_MATCH_DELETE_SHORT_ADDR(index, short_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_DELETE_SHORT_ADDR(index, short_addr)
#define ZB_SRC_MATCH_TBL_DROP() \
  ZB_TRANSCEIVER_SRC_MATCH_TBL_DROP()
#define ZB_SRC_MATCH_SHORT_SET_PENDING_BIT(short_addr, pending) \
  ZB_TRANSCEIVER_SRC_MATCH_SHORT_SET_PENDING_BIT(short_addr, pending)
#define ZB_SRC_MATCH_ADD_IEEE_ADDR(index, ieee_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_ADD_IEEE_ADDR(index, ieee_addr)
#define ZB_SRC_MATCH_DELETE_IEEE_ADDR(index, ieee_addr) \
  ZB_TRANSCEIVER_SRC_MATCH_DELETE_IEEE_ADDR(index, ieee_addr)
#define ZB_SRC_MATCH_IEEE_SET_PENDING_BIT(ieee_addr, pending) \
  ZB_TRANSCEIVER_SRC_MATCH_IEEE_SET_PENDING_BIT(ieee_addr, pending)

#endif /* ZB_MAC_SOFTWARE_PB_MATCHING */

#endif /* ZB_MAC_PENDING_BIT_SOURCE_MATCHING */

#endif /* MAC_SOURCE_MATCHING_INCLUDED */
