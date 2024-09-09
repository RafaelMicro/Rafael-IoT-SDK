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
/*  PURPOSE: NCP host SoC state cache
*/

#ifndef NCP_HOST_SOC_STATE_H
#define NCP_HOST_SOC_STATE_H 1

#include "zb_common.h"
#include "ncp/ncp_host_api.h"


void ncp_host_state_init(void);


zb_ret_t ncp_host_state_validate_start_state(void);


zb_bool_t ncp_host_state_get_nvram_erase(void);


void ncp_host_state_get_long_addr(zb_ieee_addr_t address);


void ncp_host_state_set_long_addr(const zb_ieee_addr_t address);


zb_bool_t ncp_host_state_get_nwk_key(zb_uint8_t *key, zb_uint8_t i);


void ncp_host_state_get_use_extended_pan_id(zb_ext_pan_id_t ext_pan_id);


zb_uint16_t ncp_host_state_get_pan_id(void);


zb_nwk_device_type_t ncp_host_state_get_device_type(void);


zb_uint8_t ncp_host_state_get_max_children(void);


zb_uint8_t* ncp_host_state_get_ic(void);


zb_uint8_t ncp_host_state_get_ic_type(void);


void ncp_host_state_set_ic(zb_uint8_t ic_type, const zb_uint8_t *ic);


zb_bool_t ncp_host_state_get_allow_ic_only(void);

zb_bool_t ncp_host_state_get_tc_policy_tc_link_keys_required(void);

void ncp_host_state_get_channel_list(zb_channel_list_t channel_list);


void ncp_host_state_set_channel_list(zb_channel_list_t channel_list);


zb_bool_t ncp_host_state_get_rx_on_when_idle(void);


zb_uint16_t *ncp_host_state_get_mac_invisible_shorts(zb_uint8_t *addresses_count);


zb_ieee_addr_t *ncp_host_state_get_mac_visible_longs(zb_uint8_t *addresses_count);


zb_bool_t ncp_host_state_get_permit_control4_network(void);


void ncp_host_state_set_keepalive_timeout(zb_uint_t timeout);


zb_uint_t ncp_host_state_get_keepalive_timeout(void);


zb_bool_t ncp_host_state_keepalive_timeout_is_set(void);


void ncp_host_state_set_ed_timeout(zb_uint_t timeout);


zb_uint_t ncp_host_state_get_ed_timeout(void);


zb_bool_t ncp_host_state_ed_timeout_is_set(void);


void ncp_host_state_set_keepalive_mode(nwk_keepalive_supported_method_t mode);


nwk_keepalive_supported_method_t ncp_host_state_get_keepalive_mode(void);


zb_bool_t ncp_host_state_keepalive_mode_is_set(void);


zb_bool_t ncp_host_state_get_zboss_started(void);


zb_bool_t ncp_host_state_get_joined(void);


void ncp_host_state_set_zboss_started(zb_bool_t started);


void ncp_host_state_set_joined(zb_bool_t joined);


void ncp_host_state_set_authenticated(zb_bool_t authenticated);


void ncp_host_state_set_tclk_valid(zb_bool_t tclk_valid);


void ncp_host_state_set_waiting_for_tclk(zb_bool_t waiting_for_tclk);


void ncp_host_state_set_extended_pan_id(const zb_ext_pan_id_t ext_pan_id);


void ncp_host_state_set_current_page(zb_uint8_t page);


void ncp_host_state_set_current_channel(zb_uint8_t channel);


void ncp_host_state_set_short_address(zb_uint16_t address);


void ncp_host_state_set_parent_short_address(zb_uint16_t address);


void ncp_host_state_set_pan_id(zb_uint16_t pan_id);


void ncp_host_state_set_coordinator_version(zb_uint8_t version);


zb_aps_group_table_t* ncp_host_state_get_group_table(void);


void ncp_host_state_set_rejoin_active(zb_bool_t is_active);

void ncp_host_state_set_module_version(zb_uint32_t fw_version,
                                       zb_uint32_t stack_version,
                                       zb_uint32_t ncp_version);

zb_uint32_t ncp_host_state_get_stack_version(void);

zb_uint32_t ncp_host_state_get_ncp_protocol_version(void);

#endif /* NCP_HOST_SOC_STATE_H */
