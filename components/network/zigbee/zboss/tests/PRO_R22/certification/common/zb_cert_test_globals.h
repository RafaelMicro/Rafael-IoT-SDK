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
/* PURPOSE: Definitions common to all R22 certification tests
*/
#include "zb_test_step.h"

#ifndef _ZB_CERT_TEST_GLOBALS
#define _ZB_CERT_TEST_GLOBALS

#include "zb_common.h"
#include "../../../multitest_common/zb_multitest.h"
#include "zb_test_profile.h"

#define ZB_CERT_TEST_DEAULT_PAGE    0
#define ZB_CERT_TEST_DEAULT_CHANNEL 15
#define ZB_CERT_TEST_DEAULT_MODE ZB_TEST_CONTROL_ALARMS

#ifdef ZB_MULTI_TEST

#ifndef TEST_PAGE
#define TEST_PAGE        (zb_multitest_get_test_ctx()->page)
#endif /* ifndef TEST_PAGE */

#ifndef TEST_CHANNEL
#define TEST_CHANNEL     (zb_multitest_get_test_ctx()->channel)
#endif /* ifndef TEST_CHANNEL */

#ifndef TEST_MODE
#define TEST_MODE        (zb_multitest_get_test_ctx()->mode)
#endif /* ifndef TEST_MODE */

#else /* ifdef ZB_MULTI_TEST */

#ifndef TEST_PAGE
#define TEST_PAGE        ZB_CERT_TEST_DEAULT_PAGE
#endif /* ifndef TEST_PAGE */

#ifndef TEST_CHANNEL
#define TEST_CHANNEL     ZB_CERT_TEST_DEAULT_CHANNEL
#endif /* ifndef TEST_CHANNEL */

#ifndef TEST_MODE
#define TEST_MODE        ZB_CERT_TEST_DEAULT_MODE
#endif /* ifndef TEST_MODE */

#endif /* ifdef ZB_MULTI_TEST */


void zb_cert_test_set_init_globals(void);
void zb_cert_test_set_common_channel_settings(void);
void zb_cert_test_set_zc_role(void);
void zb_cert_test_set_zr_role(void);
void zb_cert_test_set_zed_role(void);

void zb_cert_test_set_security_level(zb_uint8_t level);

zb_uint16_t zb_cert_test_get_network_addr(void);
void zb_cert_test_set_network_addr(zb_uint16_t addr);

zb_uint8_t zb_cert_test_get_nib_seq_number(void);
void zb_cert_test_inc_nib_seq_number(void);
void zb_cert_test_set_nib_seq_number(zb_uint8_t seq_number);

zb_uint8_t zb_cert_test_get_zdo_tsn(void);
zb_uint8_t zb_cert_test_inc_zdo_tsn(void);

zb_uint8_t zb_cert_test_get_aps_counter(void);
void zb_cert_test_inc_aps_counter(void);

zb_uint16_t zb_cert_test_get_parent_short_addr(void);
void zb_cert_test_get_parent_ieee_addr(zb_ieee_addr_t addr);

void zb_cert_test_set_device_type(zb_nwk_device_type_t type);

zb_uint8_t zb_cert_test_nib_get_max_depth(void);

void zb_cert_test_set_keepalive_mode(nwk_keepalive_supported_method_t mode);
void zb_cert_test_set_permit_join_duration(zb_uint8_t permit_join_duration);

//JJ
void zb_cert_test_set_nwk_state(zb_uint8_t state);

zb_uint8_t zb_cert_test_nib_get_update_id(void);

void zb_cert_test_nib_set_disable_rejoin(zb_uint8_t disable_rejoin);
void zb_cert_test_nib_set_outgoing_frame_counter(zb_uint32_t frame_counter);


void zb_cert_test_set_aib_always_rejoin(zb_uint8_t always_rejoin);
void zb_cert_test_set_aib_allow_tc_rejoins(zb_uint8_t allow_tc_rejoins);

zb_uint8_t zb_cert_test_get_mac_tsn(void);
void zb_cert_test_inc_mac_tsn(void);

void zb_cert_test_set_aps_use_nvram(void);

/* Add stubs for some functions those are not available on NCP */
#ifdef NCP_MODE_HOST

#include "zb_aps.h"

zb_uint8_t zb_zdo_mgmt_bind_req(zb_uint8_t param, zb_callback_t cb);
void fill_nldereq(zb_uint8_t param, zb_uint16_t addr, zb_uint8_t secure);
void zb_nlde_data_request(zb_uint8_t param);
void zb_set_installcode_policy(zb_bool_t allow_ic_only);
zb_uint8_t zb_zdo_mgmt_lqi_req(zb_uint8_t param, zb_callback_t cb);
void zb_nwk_change_me_addr(zb_uint8_t param);
void mac_add_invisible_short(zb_uint16_t addr);
void mac_remove_invisible_short(zb_uint16_t addr);
void mac_add_visible_device(zb_ieee_addr_t long_addr);
zb_bool_t zb_zdo_rejoin_backoff_is_running();
void zb_nwk_set_address_assignment_cb(zb_addr_assignment_cb_t cb);
void zb_zdo_rejoin_backoff_cancel();
zb_ret_t zb_zdo_rejoin_backoff_start(zb_bool_t insecure_rejoin);
void zb_nvram_register_app1_write_cb(zb_nvram_write_app_data_t wcb, zb_nvram_get_app_data_size_t gcb);
void zb_nvram_register_app1_read_cb(zb_nvram_read_app_data_t cb);
void zb_secur_apsme_request_key(zb_uint8_t param);
zb_ret_t zb_osif_nvram_write(zb_uint8_t page, zb_uint32_t pos, void *buf, zb_uint16_t len);
zb_ret_t zb_osif_nvram_read(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len);
void zb_tp_buffer_test_request(zb_uint8_t param, zb_callback_t cb);
void zb_zdo_register_device_annce_cb(zb_device_annce_cb_t cb);
void aps_data_hdr_fill_datareq(zb_uint8_t fc, zb_apsde_data_req_t *req, zb_bufid_t param);
void zb_set_node_descriptor_manufacturer_code_req(zb_uint16_t manuf_code, zb_set_manufacturer_code_cb_t cb);
void zb_zdo_pim_set_long_poll_interval(zb_time_t ms);

void zb_set_use_extended_pan_id(const zb_ext_pan_id_t ext_pan_id);
void zb_get_use_extended_pan_id(zb_ext_pan_id_t ext_pan_id);

void zb_set_input_cluster_id(zb_af_simple_desc_1_1_t *simple_desc, zb_uint8_t cluster_number, zb_uint16_t cluster_id);
void zb_set_output_cluster_id(zb_af_simple_desc_1_1_t *simple_desc, zb_uint8_t cluster_number, zb_uint16_t cluster_id);
zb_ret_t zb_add_simple_descriptor(zb_af_simple_desc_1_1_t *simple_desc);
void zb_set_simple_descriptor(zb_af_simple_desc_1_1_t *simple_desc,
                              zb_uint8_t  endpoint,
                              zb_uint16_t app_profile_id,
                              zb_uint16_t app_device_id,
                              zb_bitfield_t app_device_version,
                              zb_uint8_t app_input_cluster_count,
                              zb_uint8_t app_output_cluster_count);

zb_ret_t zb_nwk_neighbor_get_by_ieee(zb_ieee_addr_t long_addr, zb_neighbor_tbl_ent_t **nbt);

void zb_zdo_set_aps_unsecure_join(zb_bool_t insecure_join);

void zb_nwk_forward(zb_uint8_t param);
zb_uint8_t zb_zdo_nwk_addr_req(zb_uint8_t param, zb_callback_t cb);
void zb_zdo_pim_permit_turbo_poll(zb_bool_t permit);
void *nwk_alloc_and_fill_cmd(zb_bufid_t buf, zb_uint8_t cmd, zb_uint8_t cmd_size);
zb_ret_t zb_nwk_neighbor_get_by_short(zb_uint16_t short_addr, zb_neighbor_tbl_ent_t **nbt);

zb_aps_device_key_pair_set_t * zb_secur_update_key_pair(zb_ieee_addr_t address,
                                                        zb_uint8_t* key,
                                                        zb_uint8_t key_type,
                                                        zb_uint8_t key_attr,
                                                        zb_uint8_t key_source);

void tp_send_req_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_uint16_t profile_id, zb_uint16_t addr, zb_uint8_t addr_mode, zb_uint8_t src_ep, zb_uint8_t dst_ep, zb_uint8_t tx_options, zb_uint8_t radius);

zb_nwk_hdr_t *nwk_alloc_and_fill_hdr(zb_bufid_t buf,
                                     zb_uint16_t src_addr, zb_uint16_t dst_addr,
                                     zb_bool_t is_multicast, zb_bool_t is_secured, zb_bool_t is_cmd_frame, zb_bool_t force_long);

zb_uint8_t zb_nwk_get_nbr_dvc_type_by_ieee(zb_ieee_addr_t addr);
void zb_nlme_permit_joining_request(zb_uint8_t param);
zb_uint8_t *secur_nwk_key_by_seq(zb_ushort_t key_seq_number);
void zb_secur_switch_nwk_key_br(zb_uint8_t param);
void zb_apsme_transport_key_request(zb_uint8_t param);

void zb_start_concentrator_mode(zb_uint8_t radius, zb_uint32_t disc_time);
void zb_nwk_pib_set(zb_uint8_t param, zb_uint8_t attr, void *value,
                    zb_ushort_t value_size, zb_callback_t cb);
void zb_nlme_send_status(zb_uint8_t param);
void zb_mac_send_frame(zb_bufid_t buf, zb_uint8_t mhr_len);
void zb_aps_send_command(zb_uint8_t param, zb_uint16_t dest_addr, zb_uint8_t command, zb_bool_t secure,
                         zb_uint8_t secure_aps_i);
void zb_secur_apsme_remove_device(zb_uint8_t param);
void zb_set_default_ffd_descriptor_values(zb_logical_type_t device_type);

void zb_mcps_data_request(zb_uint8_t param);
void zb_mcps_build_data_request(zb_bufid_t buf, zb_uint16_t src_addr_param, zb_uint16_t dst_addr_param,
  zb_uint8_t tx_options_param, zb_uint8_t msdu_hande_param);
zb_uint8_t zdo_send_req_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_callback_t cb, zb_uint16_t addr,
                           zb_uint8_t resp_counter);

void zb_fcf_set_src_addressing_mode(zb_uint8_t *p_fcf, zb_uint8_t addr_mode);
void zb_tp_transmit_counted_packets_req_ext(zb_uint8_t param, zb_callback_t cb);
void zb_tp_transmit_counted_packets_req(zb_uint8_t param, zb_callback_t cb);
void zdo_send_resp_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_uint16_t addr);
void zb_nlme_route_discovery_request(zb_uint8_t param);
zb_bool_t zb_tp_send_link_status_command(zb_uint8_t param, zb_uint16_t short_addr);

void zb_mlme_start_request(zb_uint8_t param);
void zb_enable_auto_pan_id_conflict_resolution(zb_bool_t status);
zb_uint8_t zb_zdo_mgmt_nwk_update_req(zb_uint8_t param, zb_callback_t cb);

void zb_init_ed_aging(zb_neighbor_tbl_ent_t *nbt, zb_uint8_t timeout, zb_bool_t run_aging);
zb_ret_t zb_nwk_send_rrec(zb_bufid_t cbuf, zb_uint16_t src_addr, zb_uint16_t dst_addr);
void zb_tp_device_announce(zb_uint8_t param);
void zb_zdo_do_set_channel(zb_uint8_t channel);
void zb_secur_send_nwk_key_update_br(zb_uint8_t param);
void zb_fcf_set_dst_addressing_mode(zb_uint8_t *p_fcf, zb_uint8_t addr_mode);

void zdo_send_device_annce(zb_uint8_t param);
zb_uint8_t zb_nwk_get_nbr_rel_by_short(zb_uint16_t addr);
zb_ret_t zb_nwk_delete_neighbor_by_short(zb_uint16_t addr);
zb_nwk_routing_t *new_routing_table_ent();

/*#ifdef ZB_PRO_STACK
extern zb_nwk_routing_t *zb_nwk_mesh_find_route(zb_uint16_t dest_addr, zb_uint8_t is_multicast);
#else
zb_nwk_routing_t *zb_nwk_mesh_find_route(zb_uint16_t dest_addr);
#endif*/

void zb_set_node_power_descriptor(zb_current_power_mode_t current_power_mode, zb_uint8_t available_power_sources,
                                  zb_uint8_t current_power_source, zb_power_source_level_t current_power_source_level);

void zdo_set_aging_timeout(zb_uint8_t timeout);
void zb_zdo_pim_start_poll(zb_uint8_t param);
void zb_zdo_pim_stop_poll(zb_uint8_t param);
zb_ret_t zb_zdo_set_lpd_cmd_mode(zb_uint8_t mode);
void zb_zdo_set_lpd_cmd_timeout(zb_uint8_t timeout);

void zb_start_get_peer_short_addr(zb_address_ieee_ref_t dst_addr_ref, zb_callback_t cb, zb_uint8_t param);
void zb_set_default_ed_descriptor_values();

#ifdef ZB_TRANSCEIVER_SET_RX_ON_OFF
#undef ZB_TRANSCEIVER_SET_RX_ON_OFF
#define ZB_TRANSCEIVER_SET_RX_ON_OFF(param) ((void)param)
#endif

#endif /* NCP_MODE_HOST */

#endif /* _ZB_CERT_TEST_GLOBALS */

