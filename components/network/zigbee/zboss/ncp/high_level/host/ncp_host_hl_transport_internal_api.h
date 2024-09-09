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
/*  PURPOSE: NCP High level transport implementation for the host side.
 *  API of the NCP High level transport, which is not public and used only by adapter layer */

#ifndef NCP_HOST_HL_TRANSPORT_INTERNAL_API_H
#define NCP_HOST_HL_TRANSPORT_INTERNAL_API_H 1

#include "zb_common.h"
#include "zb_types.h"

#include "zb_aps.h"
#include "zb_zdo_globals.h"
#include "ncp/ncp_host_api.h"

#define NCP_TSN_RESERVED 0xFFu


typedef ZB_PACKED_PRE struct ncp_proto_network_descriptor_s
{
  zb_ieee_addr_t extended_pan_id;
  zb_uint16_t    pan_id;
  zb_uint8_t     nwk_update_id;
  zb_uint8_t     channel_page;
  zb_uint8_t     channel;
  zb_uint8_t     flags;
} ZB_PACKED_STRUCT ncp_proto_network_descriptor_t;

typedef ZB_PACKED_PRE struct ncp_host_zb_apsde_data_req_s
{
  zb_addr_u   dst_addr;
  zb_uint16_t profileid;
  zb_uint16_t clusterid;
  zb_uint8_t  dst_endpoint;
  zb_uint8_t  src_endpoint;
  zb_uint8_t  radius;
  zb_uint8_t  addr_mode;
  zb_uint8_t  tx_options;
  zb_uint8_t  use_alias;
  zb_uint16_t alias_src_addr;
  zb_uint8_t  alias_seq_num;
} ZB_PACKED_STRUCT ncp_host_zb_apsde_data_req_t;

typedef ZB_PACKED_PRE struct ncp_host_zb_zdo_binding_table_record_s
{
  zb_ieee_addr_t src_address;
  zb_uint8_t src_endp;
  zb_uint16_t cluster_id;
  zb_uint8_t dst_addr_mode;
  zb_addr_u dst_address;
  zb_uint8_t dst_endp;
} ZB_PACKED_STRUCT ncp_host_zb_zdo_binding_table_record_t;

typedef ZB_PACKED_PRE struct ncp_host_zb_zdo_neighbor_table_record_s
{
  zb_ext_pan_id_t ext_pan_id;
  zb_ieee_addr_t  ext_addr;
  zb_uint16_t network_addr;
  zb_uint8_t type_flags;
  zb_uint8_t permit_join;
  zb_uint8_t depth;
  zb_uint8_t lqi;
} ZB_PACKED_STRUCT ncp_host_zb_zdo_neighbor_table_record_t;

#ifdef ZB_ENABLE_INTER_PAN_NON_DEFAULT_CHANNEL

typedef ZB_PACKED_PRE struct ncp_host_zb_intrp_data_req_s
{
  zb_uint8_t  dst_addr_mode;     /*!< The addressing mode for the destination address used in this primitive. */
  zb_uint16_t dst_pan_id;        /*!< The 16-bit Pan identifier of the entity or entities to which the ASDU is being transferred or the broadcast Pan ID 0xffff. */
  zb_addr_u   dst_addr;          /*!< The address of the entity or entities to which the ASDU is being transferred. */
  zb_uint16_t profile_id;        /*!< The identifier of the application profile for which this frame is intended. */
  zb_uint16_t cluster_id;        /*!< The identifier of the cluster, within the profile specified by the ProfileId parameter, which defines the application semantics of the ASDU. */
  zb_uint8_t  asdu_handle;       /*!< An integer handle associated with the ASDU to be transmitted. */
  zb_uint32_t channel_page_mask; /*!< Channel Page structure - binary encoded channel page and channels mask as list of channels to send packet at. */
  zb_uint32_t chan_wait_ms;      /*!< Time in miliseconds to wait at channel after the packet is sent. */
} ZB_PACKED_STRUCT ncp_host_zb_intrp_data_req_t;

typedef ZB_PACKED_PRE struct ncp_host_zb_intrp_data_res_s
{
  zb_ret_t    status;                   /*!< The status of corresponding request. */
  zb_uint32_t channel_status_page_mask; /*!< Channel Page structure - binary encoded channel page and channels mask as list of channels on which channels the packet was successfully sent. */
  zb_uint8_t  asdu_handle;              /*!< An integer handle associated with the transmitted frame. */
} ZB_PACKED_STRUCT ncp_host_zb_intrp_data_res_t;

typedef ZB_PACKED_PRE struct ncp_host_zb_intrp_data_ind_s
{
  zb_uint8_t     dst_addr_mode; /*!< Destination address mode. */
  zb_uint16_t    dst_pan_id;    /*!< Destination Pan identifier. */
  zb_addr_u      dst_addr;      /*!< Destination address. */
  zb_uint16_t    src_pan_id;    /*!< Source Pan identifier. */
  zb_ieee_addr_t src_addr;      /*!< Source device address. */
  zb_uint16_t    profile_id;    /*!< Profile identifier. */
  zb_uint16_t    cluster_id;    /*!< Cluster identifier. */
  zb_uint8_t     link_quality;  /*!< The link quality observed during the reception of the ASDU. */
  zb_uint8_t     rssi;          /*!< RSSI observed during the reception of the ASDU. */
  zb_uint8_t     page;          /*!< Channel page at which packet was received. */
  zb_uint8_t     channel;       /*!< Channel at which packet was received. */
} ZB_PACKED_STRUCT ncp_host_zb_intrp_data_ind_t;


void ncp_host_intrp_adapter_init_ctx(void);

zb_ret_t ncp_host_intrp_data_request(ncp_host_zb_intrp_data_req_t *req, zb_uint8_t param_len,
                                     zb_uint16_t data_len, zb_uint8_t *data_ptr,
                                     zb_uint8_t *tsn);

void ncp_host_handle_intrp_data_indication(ncp_host_zb_intrp_data_ind_t *ncp_ind,
                                           zb_uint8_t *data_ptr,
                                           zb_uint8_t params_len, zb_uint16_t data_len);

void ncp_host_handle_intrp_data_response(ncp_host_zb_intrp_data_res_t *conf, zb_uint8_t tsn);

#endif /* ZB_ENABLE_INTER_PAN_NON_DEFAULT_CHANNEL */

/* NCP High level transport functions, called from Adapters layer */
zb_ret_t ncp_host_apsde_data_request(ncp_host_zb_apsde_data_req_t *req, zb_uint8_t param_len,
                                     zb_uint16_t data_len, zb_uint8_t *data_ptr,
                                     zb_uint8_t *tsn);

zb_ret_t ncp_host_apsme_bind_unbind_request(zb_ieee_addr_t src_addr, zb_uint8_t src_endpoint,
                                            zb_uint16_t cluster_id, zb_uint8_t addr_mode,
                                            zb_addr_u dst_addr, zb_uint8_t dst_endpoint,
                                            zb_bool_t is_bind, zb_uint8_t *tsn);

zb_ret_t ncp_host_apsme_unbind_all_request(void);

zb_ret_t ncp_host_apsme_add_group_request(zb_apsme_add_group_req_t *req, zb_uint8_t *tsn);

zb_ret_t ncp_host_apsme_remove_group_request(zb_apsme_remove_group_req_t *req, zb_uint8_t *tsn);

zb_ret_t ncp_host_apsme_remove_all_groups_request(zb_apsme_remove_all_groups_req_t *req, zb_uint8_t *tsn);

zb_ret_t ncp_host_aps_check_binding_request(zb_aps_check_binding_req_t *req, zb_uint8_t *tsn);

zb_ret_t ncp_host_aps_get_group_table_request(void);

zb_ret_t ncp_host_nwk_formation(zb_uint32_t *scan_channels_list, zb_uint8_t scan_duration,
                                zb_uint8_t distributed_network, zb_uint16_t distributed_network_address,
                                zb_ext_pan_id_t extended_pan_id);

zb_ret_t ncp_host_nwk_start_without_formation(void);

zb_ret_t ncp_host_nwk_discovery(zb_uint32_t *scan_channels_list, zb_uint8_t scan_duration);

zb_ret_t ncp_host_nwk_nlme_join(zb_ext_pan_id_t extended_pan_id, zb_uint8_t rejoin_network,
                                zb_uint32_t *scan_channels_list, zb_uint8_t scan_duration,
                                zb_uint8_t mac_capabilities, zb_uint8_t security_enable);

zb_ret_t ncp_host_nwk_permit_joining(zb_uint8_t permit_duration);

zb_ret_t ncp_host_zdo_ieee_addr_request(zb_uint8_t *ncp_tsn, zb_zdo_ieee_addr_req_param_t *req_param);

zb_ret_t ncp_host_zdo_nwk_addr_request(zb_uint8_t *ncp_tsn, zb_zdo_nwk_addr_req_param_t *req_param);

zb_ret_t ncp_host_zdo_power_desc_request(zb_uint8_t *ncp_tsn, zb_uint16_t nwk_addr);

zb_ret_t ncp_host_zdo_node_desc_request(zb_uint8_t *ncp_tsn, zb_uint16_t nwk_addr);

zb_ret_t ncp_host_zdo_simple_desc_request(zb_uint8_t *ncp_tsn, zb_uint16_t nwk_addr, zb_uint8_t endpoint);

zb_ret_t ncp_host_zdo_active_ep_request(zb_uint8_t *ncp_tsn, zb_uint16_t nwk_addr);

zb_ret_t ncp_host_zdo_match_desc_request(zb_uint8_t *ncp_tsn, zb_zdo_match_desc_param_t *req);

zb_ret_t ncp_host_zdo_bind_unbind_request(zb_uint8_t *ncp_tsn, zb_zdo_bind_req_param_t *bind_param,
                                          zb_bool_t is_bind);

zb_ret_t ncp_host_zdo_permit_joining_request(zb_uint8_t *ncp_tsn, zb_uint16_t short_addr,
                                             zb_uint8_t permit_duration, zb_uint8_t tc_significance);

zb_ret_t ncp_host_zdo_mgmt_leave_request(zb_uint8_t *ncp_tsn, zb_zdo_mgmt_leave_param_t *req);

zb_ret_t ncp_host_zdo_mgmt_bind_request(zb_uint8_t *ncp_tsn, zb_uint16_t dst_addr, zb_uint8_t start_index);

zb_ret_t ncp_host_zdo_mgmt_lqi_request(zb_uint8_t *ncp_tsn, zb_uint16_t dst_addr, zb_uint8_t start_index);

zb_ret_t ncp_host_zdo_mgmt_nwk_update_request(zb_uint8_t *ncp_tsn, zb_uint32_t scan_channels,
  zb_uint8_t scan_duration, zb_uint8_t scan_count,
  zb_uint16_t manager_addr, zb_uint16_t dst_addr);

zb_ret_t ncp_host_zdo_rejoin_request(zb_uint8_t *ncp_tsn,
                                     zb_ext_pan_id_t ext_pan_id,
                                     zb_channel_page_t *channels_list,
                                     zb_uint8_t secure_rejoin);

zb_ret_t ncp_host_zdo_system_server_discovery_request(zb_uint8_t *ncp_tsn, zb_uint16_t server_mask);

zb_ret_t ncp_host_zdo_diagnostics_get_stats_request(zb_uint8_t *ncp_tsn, zb_uint8_t pib_attr);

zb_ret_t ncp_host_nwk_nlme_start_router_request(zb_uint8_t beacon_order, zb_uint8_t superframe_order,
                                                zb_uint8_t battery_life_extension);

/* Functions to be called from NCP High level transport */
void ncp_host_handle_apsde_data_indication(zb_apsde_data_indication_t *ind,
                                           zb_uint8_t *data_ptr,
                                           zb_uint8_t params_len, zb_uint16_t data_len);

void ncp_host_handle_apsde_data_response(zb_apsde_data_confirm_t *conf, zb_uint8_t tsn);

void ncp_host_handle_apsme_bind_response(zb_ret_t status, zb_uint8_t tsn);

void ncp_host_handle_apsme_unbind_response(zb_ret_t status, zb_uint8_t tsn);

void ncp_host_handle_apsme_unbind_all_response(zb_ret_t status);

void ncp_host_handle_apsme_add_group_response(zb_ret_t status, zb_uint8_t tsn);
void ncp_host_handle_apsme_remove_group_response(zb_ret_t status, zb_uint8_t tsn);
void ncp_host_handle_apsme_remove_all_groups_response(zb_ret_t status, zb_uint8_t tsn);
void ncp_host_handle_aps_check_binding_response(zb_ret_t status, zb_bool_t exists, zb_uint8_t tsn);
void ncp_host_handle_aps_get_group_table_response(zb_ret_t status_code,

                                                  zb_aps_group_table_ent_t* table,
                                                  zb_size_t entry_cnt);

void ncp_host_handle_nwk_formation_response(zb_ret_t status,
                                            zb_uint16_t short_address,
                                            zb_uint16_t pan_id,
                                            zb_uint8_t current_page,
                                            zb_uint8_t current_channel);

void ncp_host_handle_nwk_start_without_formation_response(zb_ret_t status);

void ncp_host_handle_nwk_discovery_response(zb_ret_t status, zb_uint8_t network_count,
                                            ncp_proto_network_descriptor_t *network_descriptors);

void ncp_host_handle_nwk_permit_joining_response(zb_ret_t status);

void ncp_host_handle_nwk_nlme_join_response(zb_ret_t status, zb_uint16_t short_addr, zb_ext_pan_id_t extended_pan_id,
                                            zb_uint8_t channel_page, zb_uint8_t logical_channel,
                                            zb_uint8_t enhanced_beacon, zb_uint8_t mac_interface);

void ncp_host_handle_nwk_joined_indication(zb_uint16_t nwk_addr, zb_ext_pan_id_t ext_pan_id,
                                           zb_uint8_t channel_page, zb_uint8_t channel,
                                           zb_uint8_t beacon_type, zb_uint8_t mac_interface);

void ncp_host_handle_nwk_join_failed_indication(zb_uint8_t status_category, zb_uint8_t status_code);

void ncp_host_handle_nwk_leave_indication(zb_ieee_addr_t device_addr, zb_uint8_t rejoin);


void ncp_host_handle_nwk_address_update_indication(zb_uint16_t nwk_addr);


/**
 * Handles NWK PAN ID Conflict Indication
 *
 * @param pan_id_count count of PAN IDs
 * @param pan_ids PAN IDs values
 */
void ncp_host_handle_nwk_pan_id_conflict_indication(zb_uint16_t pan_id_count, zb_uint16_t *pan_ids);


void ncp_host_handle_nwk_parent_lost_indication(void);

#ifdef ZB_APSDE_REQ_ROUTING_FEATURES
void ncp_host_handle_nwk_route_reply_indication(zb_uint16_t nwk_addr);

void ncp_host_handle_nwk_route_request_send_indication(zb_uint16_t nwk_addr);

void ncp_host_handle_nwk_route_record_send_indication(zb_bufid_t buf);
#endif

void ncp_host_handle_nwk_start_pan_id_conflict_resolution_response(zb_ret_t status);

void ncp_host_handle_nwk_enable_pan_id_conflict_resolution_response(zb_ret_t status);

void ncp_host_handle_nwk_enable_auto_pan_id_conflict_resolution_response(zb_ret_t status);

void ncp_host_handle_mac_add_invisible_short_response(zb_ret_t status);

void ncp_host_handle_mac_add_visible_long_response(zb_ret_t status);


void ncp_host_handle_zdo_ieee_addr_response(zb_ret_t status, zb_uint8_t ncp_tsn,
                                            zb_zdo_ieee_addr_resp_t *resp,
                                            zb_bool_t is_extended,
                                            zb_zdo_ieee_addr_resp_ext_t *resp_ext,
                                            zb_zdo_ieee_addr_resp_ext2_t *resp_ext2,
                                            zb_uint16_t *addr_list);

void ncp_host_handle_zdo_nwk_addr_response(zb_ret_t status, zb_uint8_t ncp_tsn,
                                           zb_zdo_nwk_addr_resp_head_t *resp,
                                           zb_bool_t is_extended,
                                           zb_zdo_nwk_addr_resp_ext_t *resp_ext,
                                           zb_zdo_nwk_addr_resp_ext2_t *resp_ext2,
                                           zb_uint16_t *addr_list);

void ncp_host_handle_zdo_power_descriptor_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint16_t nwk_addr,
                                                   zb_uint16_t power_descriptor);

void ncp_host_handle_zdo_node_descriptor_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint16_t nwk_addr,
                                                  zb_af_node_desc_t *node_desc);

void ncp_host_handle_zdo_simple_descriptor_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint16_t nwk_addr,
                                                    zb_af_simple_desc_1_1_t *simple_desc,
                                                    zb_uint16_t *app_cluster_list_ptr);

void ncp_host_handle_zdo_active_ep_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint16_t nwk_addr,
                                            zb_uint8_t ep_count, zb_uint8_t *ep_list);

void ncp_host_handle_zdo_match_desc_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint16_t nwk_addr,
                                             zb_uint8_t match_ep_count, zb_uint8_t *match_ep_list);

void ncp_host_handle_zdo_diagnostics_get_stats_response(zb_ret_t status, zb_uint8_t ncp_tsn, zdo_diagnostics_full_stats_t *full_stats);

void ncp_host_handle_zdo_bind_response(zb_ret_t status, zb_uint8_t ncp_tsn);

void ncp_host_handle_zdo_unbind_response(zb_ret_t status, zb_uint8_t ncp_tsn);

void ncp_host_handle_zdo_permit_joining_response(zb_ret_t status, zb_uint8_t ncp_tsn);

void ncp_host_handle_zdo_mgmt_leave_response(zb_ret_t status, zb_uint8_t ncp_tsn);

void ncp_host_handle_zdo_mgmt_bind_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint8_t binding_table_entries,
  zb_uint8_t start_index, zb_uint8_t binding_table_list_count, ncp_host_zb_zdo_binding_table_record_t *binding_table);

void ncp_host_handle_zdo_mgmt_lqi_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint8_t neighbor_table_entries,
  zb_uint8_t start_index, zb_uint8_t neighbor_table_list_count, ncp_host_zb_zdo_neighbor_table_record_t *neighbor_table);

void ncp_host_handle_zdo_mgmt_nwk_update_response(zb_ret_t status, zb_uint8_t ncp_tsn,
  zb_uint32_t scanned_channels, zb_uint16_t total_transmissions, zb_uint16_t transmission_failures,
  zb_uint8_t scanned_channels_list_count, zb_uint8_t *energy_values);

void ncp_host_handle_zdo_rejoin_response(zb_ret_t status);

void ncp_host_handle_zdo_system_server_discovery_response(zb_ret_t status, zb_uint8_t ncp_tsn,
                                                          zb_uint16_t server_mask);

void ncp_host_handle_nwk_nlme_router_start_response(zb_ret_t status);

void ncp_host_handle_zdo_device_annce_indication(zb_uint16_t nwk_addr, zb_ieee_addr_t ieee_addr,
                                                 zb_uint8_t capability);

void ncp_host_handle_zdo_device_authorized_indication(zb_ieee_addr_t ieee_addr,
                                                      zb_uint16_t nwk_addr,
                                                      zb_uint8_t authorization_type,
                                                      zb_uint8_t authorization_status);

void ncp_host_handle_zdo_device_update_indication(zb_ieee_addr_t ieee_addr,
                                                  zb_uint16_t nwk_addr,
                                                  zb_uint8_t status);

void ncp_host_handle_secur_tclk_indication(zb_ret_t status,
                                           zb_ieee_addr_t trust_center_address,
                                           zb_uint8_t key_type);



/**
 * Force NCP module reboot
 *
 */
zb_ret_t ncp_host_reset(void);

/**
 * Force NCP module reboot.
 * Cleans NCP module internal state and removes from NVRAM all info except NWK packets counter.
 */
zb_ret_t ncp_host_factory_reset(void);

/**
 * Force NCP module reboot.
 * Cleans NCP module internal state, erases all NVRAM content including NWK packet counter.
 *
 * @warning Use for debugging purposes only
 */
zb_ret_t ncp_host_reset_nvram_erase(void);

/**
 * Sends Get Module Version request.
 *
 */
zb_ret_t ncp_host_get_module_version(void);

/**
 * Sends Get Join Status request.
 *
 */
zb_ret_t ncp_host_get_join_status(void);

/**
 * Sends Get Authentication Status request.
 *
 */
zb_ret_t ncp_host_get_authentication_status(void);

/**
 * Requests device local IEEE address
 *
 * @param interface_num - Number of MAC interface. Always 0 for 2.4GHz-only build
 *
 */
zb_ret_t ncp_host_get_local_ieee_addr(zb_uint8_t interface_num);

/**
 * Requests production config from SoC
 */
zb_ret_t ncp_host_request_production_config(void);

/**
 * Sets device local IEEE address
 *
 * @param interface_num - Number of MAC interface. Always 0 for 2.4GHz-only build
 * @param addr - IEEE address that is set on the device
 */
zb_ret_t ncp_host_set_local_ieee_addr(zb_uint8_t interface_num, const zb_ieee_addr_t addr);

/**
 * Requests RxOnWhenIdle PIB attribute
 */
zb_ret_t ncp_host_get_rx_on_when_idle(void);


/**
 * Sets RxOnWhenIdle PIB attribute
 *
 * @param rx_on_when_idle - RxOnWhenIdle PIB attribute value
 */
zb_ret_t ncp_host_set_rx_on_when_idle(zb_bool_t rx_on_when_idle);


/**
 * Requests Keepalive Timeout
 */
zb_ret_t ncp_host_get_keepalive_timeout(void);


/**
 * Sets Keepalive Timeout value
 *
 * @param keepalive_timeout - Keepalive Timeout value
 */
zb_ret_t ncp_host_set_keepalive_timeout(zb_uint32_t keepalive_timeout);


/**
 * Requests End device Timeout
 */
zb_ret_t ncp_host_get_end_device_timeout(void);


/**
 * Sets End Device Timeout value
 *
 * @param timeout - End Device Timeout value
 */
zb_ret_t ncp_host_set_end_device_timeout(zb_uint8_t timeout);


/**
 * Sets keepalive mode
 *
 * @param mode - keepalive mode value
 */
zb_ret_t ncp_host_nwk_set_keepalive_mode(zb_uint8_t mode);


/**
 * Enables Concentrator mode for the device
 *
 * @param radius - the hop count radius for concentrator route discoveries
 * @param disc_time - the time in seconds between concentrator route discoveries
 */
zb_ret_t ncp_host_nwk_start_concentrator_mode(zb_uint8_t radius, zb_uint32_t disc_time);


/**
 * Disables Concentrator mode for the device
 */
zb_ret_t ncp_host_nwk_stop_concentrator_mode(void);


/**
 * Requests long poll interval value
 */
zb_ret_t ncp_host_nwk_get_long_poll_interval(zb_uint8_t *ncp_tsn);


/**
 * Requests "in fast poll" flag value
 */
zb_ret_t ncp_host_nwk_get_in_fast_poll_flag(zb_uint8_t *ncp_tsn);


/**
 * Sets fast poll timeout value
 *
 * @param timeout - timeout value
 */
zb_ret_t ncp_host_nwk_set_fast_poll_timeout(zb_time_t timeout);


/**
 * Starts PAN ID conflict resolution
 *
 * @param pan_id_count - length of the pan_ids array
 * @param pan_ids - array with PAN ids values
 */
zb_ret_t ncp_host_nwk_start_pan_id_conflict_resolution(zb_uint16_t pan_id_count, zb_uint16_t *pan_ids);


/**
 * Enables PAN ID conflict resolution
 *
 * @param enable - flag to indicate whether PAN ID conflict resolution should be enabled
 */
zb_ret_t ncp_host_nwk_enable_pan_id_conflict_resolution(zb_bool_t enable);


/**
 * Enables PAN ID conflict resolution
 *
 * @param enable - flag to indicate whether PAN ID conflict auto resolution should be enabled
 */
zb_ret_t ncp_host_nwk_enable_auto_pan_id_conflict_resolution(zb_bool_t enable);


/**
 * Marks short address as invisible on MAC level
 *
 * @param invisible_address - short address to mark as invisible
 */
zb_ret_t ncp_host_mac_add_invisible_short(zb_uint16_t invisible_address);

/**
 * Marks long address as visible on MAC level
 *
 * @param long_address - long address to mark as visible
 */
zb_ret_t ncp_host_mac_add_visible_long(const zb_ieee_addr_t long_address);


/**
 * Permits turbo poll
 *
 * @param permit_status - permit status value
 */
zb_ret_t ncp_host_nwk_permit_turbo_poll(zb_bool_t permit_status);


/**
 * Sets Zigbee channel mask.
 *
 * @param page
 * @param channel_mask
 *
 */
zb_ret_t ncp_host_set_zigbee_channel_mask(zb_uint8_t page, zb_uint32_t channel_mask);

/**
 * Gets Zigbee channel mask.
 *
 */
zb_ret_t ncp_host_get_zigbee_channel_mask(void);


/**
 * Gets Current Zigbee Channel
 *
 */
zb_ret_t ncp_host_get_zigbee_channel(void);

/**
 * Sets Zigbee PAN ID
 *
 * @note It is not allowed to set Zigbee PAN ID if device is already joined to the network.
 *       In this case NCP returns status code INVALID_STATE.
 *
 * @param pan_id
 *
 */
zb_ret_t ncp_host_set_zigbee_pan_id(zb_uint16_t pan_id);

/**
 * Gets Zigbee PAN ID
 *
 */
zb_ret_t ncp_host_get_zigbee_pan_id(void);

/**
 * Sets Zigbee Extended PAN ID
 *
 * @note It is not allowed to set Zigbee Extended PAN ID if device is already joined to the network.
 *       In this case NCP returns status code INVALID_STATE.
 *
 * @param extended_pan_id
 *
 */
zb_ret_t ncp_host_set_zigbee_extended_pan_id(const zb_ext_pan_id_t extended_pan_id);

/**
 * Sets Zigbee Role
 *
 * @note It is impossible to change NCP role after it is already commissioned.
 *
 * @param zigbee_role
 *
 */
zb_ret_t ncp_host_set_zigbee_role(zb_uint8_t zigbee_role);

/**
 * Gets Zigbee Role
 *
 */
zb_ret_t ncp_host_get_zigbee_role(void);

/**
 * Sets Zigbee Role
 *
 * @note It is impossible to change max children count after it is already commissioned.
 *
 * @param zigbee role
 *
 */
zb_ret_t ncp_host_set_max_children(zb_uint8_t max_children);

/**
 * Sets device install code
 *
 * @param ic_type
 * @param ic
 *
 */
zb_ret_t ncp_host_secur_set_local_ic(zb_uint8_t ic_type, zb_uint8_t *ic);


/**
 * Gets device install code
 */
zb_ret_t ncp_host_secur_get_local_ic(void);



/**
 * Starts NWK key switching procedure
 */
zb_ret_t ncp_host_secur_nwk_initiate_key_switch_procedure(void);

/**
 * Gets registered install codes list
 *
 * @param start_index
 * @param tsn
 */
zb_ret_t ncp_host_secur_get_ic_list_request(zb_uint8_t start_index, zb_uint8_t *tsn);

/**
 * Gets registered install code by index
 *
 * @param ic_index
 * @param tsn
 */
zb_ret_t ncp_host_secur_get_ic_by_idx_request(zb_uint8_t ic_index, zb_uint8_t *tsn);

/**
 * Removes all registered install codes
 *
 * @param tsn
 */
zb_ret_t ncp_host_secur_remove_all_ic_request(zb_uint8_t *tsn);

/**
 * Removes registered install code by device IEEE addr
 *
 * @param ieee_addr - IEEE address of device to remove installcode by
 * @param tsn - NCP TSN value
 */
zb_ret_t ncp_host_secur_remove_ic_request(zb_ieee_addr_t ieee_addr, zb_uint8_t *tsn);

/**
 *  Gets APS key by IEEE address
 *
 */
zb_ret_t ncp_host_get_aps_key_by_ieee(zb_ieee_addr_t ieee_addr);

/**
 * Sets NWK key
 *
 * @param key
 * @param key_number
 */
zb_ret_t ncp_host_set_nwk_key(zb_uint8_t *key, zb_uint8_t key_number);

/**
 * Requests NWK keys list
 *
 */
zb_ret_t ncp_host_get_nwk_keys(void);


/**
 * Gets current parent address
 */
zb_ret_t ncp_host_get_parent_address(void);

/**
 * Gets current extended PAN ID
 */
zb_ret_t ncp_host_get_extended_pan_id(void);

/**
 * Gets current coordinator version
 */
zb_ret_t ncp_host_get_coordinator_version(void);

/**
 * Gets current short address
 */
zb_ret_t ncp_host_get_short_address(void);

/**
 * Gets current trust center address
 */
zb_ret_t ncp_host_get_trust_center_address(void);

/**
 * Writes NVRAM dataset on the SoC
 */
zb_ret_t ncp_host_nvram_write_request(zb_uint8_t* request, zb_size_t len);

/**
 * Reads NVRAM dataset on the SoC
 */
zb_ret_t ncp_host_nvram_read_request(zb_uint16_t dataset_type);

/**
 * Clears NVRAM on the SoC
 */
zb_ret_t ncp_host_nvram_clear_request(void);

/**
 * Erases NVRAM on the SoC
 */
zb_ret_t ncp_host_nvram_erase_request(void);

/**
 * Sets a TC Policy
 *
 * @param policy_type - a type of a policy to set
 * @param value - a value to set
 *
 * @return status of operation
 */
zb_ret_t ncp_host_set_tc_policy(zb_uint16_t policy_type, zb_uint8_t value);


/**
 * Disables GPPB functionality on a SoC.
 */
zb_ret_t ncp_host_disable_gppb(void);

/**
 * Sets GP Shared Key Type on a SoC.
 */
zb_ret_t ncp_host_gp_set_shared_key_type(zb_uint8_t gp_security_key_type);

/**
 * Sets GP Default Link Key on a SoC.
 */
zb_ret_t ncp_host_gp_set_default_link_key(zb_uint8_t* gp_link_key);

/**
 * Gets IEEE address of the device by short address in the local address translation table
 * @ref zb_address_ieee_by_short()
 *
 * @param short_addr
 */
zb_ret_t ncp_host_nwk_get_ieee_by_short(zb_uint16_t short_addr);

/**
 * Gets entry from neighbor table by IEEE
 * @ref zb_nwk_neighbor_get_by_ieee()
 *
 * @param ieee_addr
 */
zb_ret_t ncp_host_nwk_get_neighbor_by_ieee(zb_ieee_addr_t ieee_addr);


/**
 * Sets Fast Poll Interval PIM attribute
 *
 * @param fast_poll_interval - Fast Poll Interval PIB attribute value
 */
zb_ret_t ncp_host_nwk_set_fast_poll_interval(zb_uint16_t fast_poll_interval);


/**
 * Sets Long Poll Interval PIM attribute
 *
 * @param long_poll_interval - Long Poll Interval PIB attribute value
 */
zb_ret_t ncp_host_nwk_set_long_poll_interval(zb_uint32_t long_poll_interval);


/**
 * Starts poll with the Fast Poll Interval specified by PIM attribute
 */
zb_ret_t ncp_host_nwk_start_fast_poll(void);


/**
 * Starts poll with the Long Poll Interval specified by PIM attribute
 */
zb_ret_t ncp_host_nwk_start_poll(void);


/**
 * Stops fast poll and restart it with the Long Poll Interval
 *
 * @param ncp_tsn
 */
zb_ret_t ncp_host_nwk_stop_fast_poll(zb_uint8_t *ncp_tsn);


/**
 * Stops poll
 */
zb_ret_t ncp_host_nwk_stop_poll(void);


/**
 * Enables turbo poll for given amount of time
 *
 * @param time - Turbo poll active time in milliseconds
 */
zb_ret_t ncp_host_nwk_enable_turbo_poll(zb_uint32_t time);


/**
 * Disables turbo poll
 */
zb_ret_t ncp_host_nwk_disable_turbo_poll(void);


/**
 * Gets short address from the local address translation table by IEEE
 * @ref zb_address_short_by_ieee()
 *
 * @param ieee_addr
 */
zb_ret_t ncp_host_nwk_get_short_by_ieee(zb_ieee_addr_t ieee_addr);


/**
 * Starts Turbo Poll for the specified packets count
 *
 * @param packets_count
 */
zb_ret_t ncp_host_nwk_start_turbo_poll_packets(zb_uint8_t packets_count);


/**
 * Decreases the number of packets to turbo poll by one.
 */
zb_ret_t ncp_host_nwk_turbo_poll_cancel_packet(void);


/**
 * Starts Turbo Poll for the specified time interval in milliseconds
 *
 * @param turbo_poll_timeout_ms
 */
zb_ret_t ncp_host_nwk_start_turbo_poll_continuous(zb_time_t turbo_poll_timeout_ms);

/**
 * Leaves packets turbo poll process
 */
zb_ret_t ncp_host_nwk_turbo_poll_packets_leave(void);


/**
 * Leaves continuous turbo poll process
 */
zb_ret_t ncp_host_nwk_turbo_poll_continuous_leave(void);

/**
 * Adds the install code for the device with specified long address
 * @note This function is valid only for Trust Center (ZC)
 * @ref zb_secur_ic_add()
 *
 * @param address - long address of the device to add the install code
 * @param ic_type - install code type as enumerated in
 * @param ic - pointer to the install code buffer
 * @param tsn - NCP TSN value
 */
zb_ret_t ncp_host_secur_add_ic(zb_ieee_addr_t address, zb_uint8_t ic_type, zb_uint8_t *ic, zb_uint8_t *tsn);



/* AF API */
/**
 * Sets Simple Descriptor for the device
 *
 * @param endpoint - Endpoint
 * @param app_profile_id - Application profile identifier
 * @param app_device_id - Application device identifier
 * @param app_device_version - Application device version
 * @param app_input_cluster_count - Application input cluster count
 * @param app_output_cluster_count - Application output cluster count
 * @param app_input_cluster_list - List of application input clusters
 * @param app_output_cluster_list - List of application output clusters
 */
zb_ret_t ncp_host_af_set_simple_descriptor(zb_uint8_t  endpoint, zb_uint16_t app_profile_id,
                                           zb_uint16_t app_device_id, zb_bitfield_t app_device_version,
                                           zb_uint8_t app_input_cluster_count,
                                           zb_uint8_t app_output_cluster_count,
                                           zb_uint16_t *app_input_cluster_list,
                                           zb_uint16_t *app_output_cluster_list);

/**
 * Deletes endpoint of the device
 *
 * @param endpoint - Number of the endpoint to delete
 */
zb_ret_t ncp_host_af_delete_endpoint(zb_uint8_t endpoint);

/**
 * Sets Node Descriptor
 *
 * @param device_type - Device type: ZC, ZR or ZED
 * @param mac_capabilities - MAC Capabilities bitfield
 * @param manufacturer_code - Manufacturer code
 */
zb_ret_t ncp_host_af_set_node_descriptor(zb_uint8_t device_type, zb_uint8_t mac_capabilities,
                                         zb_uint16_t manufacturer_code);

/**
 * Sets Power Descriptor
 * @ref zb_set_node_power_descriptor()
 *
 */
zb_ret_t ncp_host_af_set_power_descriptor(zb_uint8_t current_power_mode,
                                          zb_uint8_t available_power_sources,
                                          zb_uint8_t current_power_source,
                                          zb_uint8_t current_power_source_level);

/**
 * Sets Node Descriptor Manufacturer Code
 *
 * @param manuf_code manufacturer code
 * @param ncp_tsn - NCP TSN value
 */
zb_ret_t ncp_host_zdo_set_node_desc_manuf_code(zb_uint16_t manuf_code, zb_uint8_t *ncp_tsn);


/**
 * Requests device serial number from Production Config data
 */
zb_ret_t ncp_host_get_serial_number(void);


/**
 * Requests vendor-specific data from Production Config data
 */
zb_ret_t ncp_host_get_vendor_data(void);


/**
 * Handles reset indication.
 *
 * @param reset_src - reset source
 */
void ncp_host_handle_reset_indication(zb_uint8_t reset_src);

/**
 * Handles reset response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_reset_response(zb_ret_t status_code, zb_bool_t is_solicited);

/**
 * Handles Get Version response.
 *
 * @param status_code - status of operation
 * @param version - received NCP module version
 *
 */
void ncp_host_handle_get_module_version_response(zb_ret_t status_code, zb_uint32_t fw_version,
                                                 zb_uint32_t stack_version, zb_uint32_t ncp_version);

/**
 * Handles Get Join Status response
 *
 * @param status_code - status of operation
 * @param joined - 0 means devices is not joined, 1 means device is joined
 */
void ncp_host_handle_get_join_status_response(zb_ret_t status_code, zb_bool_t joined);

/**
 * Handles Get Authentication Status response
 *
 * @param status_code - status of operation
 * @param authenticated - 0 means devices is not authenticated, 1 means device is authenticated
 */
void ncp_host_handle_get_authentication_status_response(zb_ret_t status_code, zb_bool_t authenticated);

/**
 * Handles Get Local IEEE Address response.
 *
 * @param status_code - status of operation
 * @param addr - received IEEE address
 *
 */
void ncp_host_handle_get_local_ieee_addr_response(zb_ret_t status_code, zb_ieee_addr_t addr,
                                                  zb_uint8_t interface_num);

#if defined ZB_PRODUCTION_CONFIG && defined ZB_NCP_PRODUCTION_CONFIG_ON_HOST
/**
 * Handles Request Production Config response.
 *
 * @param status_code - status of operation
 * @param production_config_data_len - length of production config data block
 * @param production_config_data - pointer to raw production config data
 */
void ncp_host_handle_request_production_config_response(zb_ret_t status_code,
                                                        zb_uint16_t production_config_data_len,
                                                        zb_uint8_t *production_config_data);
#endif /* ZB_PRODUCTION_CONFIG && ZB_NCP_PRODUCTION_CONFIG_ON_HOST */

/**
 * Handles Set Local IEEE Address response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_set_local_ieee_addr_response(zb_ret_t status_code);


/**
 * Handles Set RxOnWhenIdle PIB attribute response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_set_rx_on_when_idle_response(zb_ret_t status_code);


/**
 * Handles Get RxOnWhenIdle PIB attribute response.
 *
 * @param status_code - status of operation
 * @param rx_on_when_idle - RxOnWhenIdle PIB attribute value
 */
void ncp_host_handle_get_rx_on_when_idle_response(zb_ret_t status_code, zb_bool_t rx_on_when_idle);


/**
 * Handles Set Keepalive Timeout response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_set_keepalive_timeout_response(zb_ret_t status_code);


/**
 * Handles Get Keepalive Timeout response.
 *
 * @param status_code - status of operation
 * @param keepalive_timeout - Keepalive Timeout value
 */
void ncp_host_handle_get_keepalive_timeout_response(zb_ret_t status_code, zb_uint32_t keepalive_timeout);


/**
 * Handles Set End Device Timeout response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_set_end_device_timeout_response(zb_ret_t status_code);


/**
 * Handles Get End Device Timeout response.
 *
 * @param status_code - status of operation
 * @param timeout - End Device Timeout value
 */
void ncp_host_handle_get_end_device_timeout_response(zb_ret_t status_code, zb_uint8_t timeout);



/**
 * Handles Set Zigbee Channel Mask Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_zigbee_channel_mask_response(zb_ret_t status_code);

/**
 * Handles Get Zigbee Channel Mask Response
 *
 * @param status_code - status of operation
 * @param channel_list_len - count of channel structures
 * @param channel_page - Channel Page
 * @param channel mask - Channel Mask
 *
 */
void ncp_host_handle_get_zigbee_channel_mask_response(zb_ret_t status_code, zb_uint8_t channel_list_len,
                                                      zb_uint8_t channel_page, zb_uint32_t channel_mask);

/**
 * Handles Get Zigbee Current Channel Response
 *
 * @param status_code - status of operation
 * @param page - current page
 * @param channel - current channel
 *
 */
void ncp_host_handle_get_zigbee_channel_response(zb_ret_t status_code, zb_uint8_t page, zb_uint8_t channel);

/**
 * Handles Set Zigbee PAN ID Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_zigbee_pan_id_response(zb_ret_t status_code);

/**
 * Handles Set Zigbee Extended PAN ID Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_zigbee_extended_pan_id_response(zb_ret_t status_code);

/**
 * Handles Get Zigbee PAN ID Response
 *
 * @param status_code - status of operation
 * @param pan_id - status of operation
 *
 */
void ncp_host_handle_get_zigbee_pan_id_response(zb_ret_t status_code, zb_uint16_t pan_id);

/**
 * Handles Set Zigbee Role Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_zigbee_role_response(zb_ret_t status_code);

/**
 * Handles Set Max Children Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_max_children_response(zb_ret_t status_code);

/**
 * Handles Set Local Install Code Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_local_ic_response(zb_ret_t status_code);

/**
 * Handles Get Local Install Code Response
 *
 * @param status_code - status of operation
 * @param ic_type - IC type
 * @param ic - IC data
 *
 */
void ncp_host_handle_secur_get_local_ic_response(zb_ret_t status_code, zb_uint8_t ic_type, const zb_uint8_t *ic);

/**
 * Handles Get Zigbee Role Response
 *
 * @param status_code - status of operation
 * @param zigbee_role - device role in a Zigbee network
 *
 */
void ncp_host_handle_get_zigbee_role_response(zb_ret_t status_code, zb_uint8_t zigbee_role);

/**
 * Handles Set NWK Key Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_set_nwk_key_response(zb_ret_t status_code);

/**
 * Handles Get NWK Keys Response
 *
 * @param status_code - status of operation
 * @param nwk_key1
 * @param key_number1
 * @param nwk_key2
 * @param key_number2
 * @param nwk_key3
 * @param key_number3
 *
 */
void ncp_host_handle_get_nwk_keys_response(zb_ret_t status_code,
                                           zb_uint8_t *nwk_key1, zb_uint8_t key_number1,
                                           zb_uint8_t *nwk_key2, zb_uint8_t key_number2,
                                           zb_uint8_t *nwk_key3, zb_uint8_t key_number3);

/**
 * Handles Get APS Key by IEEE Response
 *
 * @param status_code - status of operation
 * @param aps_key
 */
void ncp_host_handle_get_aps_key_by_ieee_response(zb_ret_t status_code, const zb_uint8_t *aps_key);


/**
 * Handles Get Parent Address Response
 *
 * @param status code - status of operation
 * @param parent_address
 */
void ncp_host_handle_get_parent_address_response(zb_ret_t status_code, zb_uint16_t parent_address);

/**
 * Handles Get Extended PAN ID Response
 *
 * @param status code - status of operation
 * @param ext_pan_id - extended PAN ID
 */
void ncp_host_handle_get_extended_pan_id_response(zb_ret_t status_code,
                                                  const zb_ext_pan_id_t ext_pan_id);

/**
 * Handles Get Coordinator Version Response
 *
 * @param status code - status of operation
 * @param version - coordinator version
 */
void ncp_host_handle_get_coordinator_version_response(zb_ret_t status_code, zb_uint8_t version);

/**
 * Handles Get Short Address Response
 *
 * @param status code - status of operation
 * @param address - current short address of the device
 */
void ncp_host_handle_get_short_address_response(zb_ret_t status_code, zb_uint16_t address);

/**
 * Handles Get Trust Center Address Response
 *
 * @param status code - status of operation
 * @param address - current Trust Center IEEE address
 */
void ncp_host_handle_get_trust_center_address_response(zb_ret_t status_code,
                                                       const zb_ieee_addr_t address);

/**
 * Handles NVRAM Read Response
 *
 * @param status_code - a status of operation
 * @param buf - a buffer with the requested dataset
 * @param len - a length of the buffer
 * @param ds_type - a type of the requested dataset
 * @param ds_version - a version of the requested dataset
 * @param nvram_version - an NVRAM version
 */
void ncp_host_handle_nvram_read_response(zb_ret_t status_code,
                                         zb_uint8_t *buf, zb_uint16_t len,
                                         zb_nvram_dataset_types_t ds_type,
                                         zb_uint16_t ds_version,
                                         zb_nvram_ver_t nvram_version);

/**
 * Handles NVRAM Write Response
 *
 * @param status_code - a status of operation
 */
void ncp_host_handle_nvram_write_response(zb_ret_t status_code);


/**
 * Handles NVRAM Clear Response
 *
 * @param status_code - a status of operation
 */
void ncp_host_handle_nvram_clear_response(zb_ret_t status_code);


/**
 * Handles NVRAM Erase Response
 *
 * @param status_code - a status of operation
 */
void ncp_host_handle_nvram_erase_response(zb_ret_t status_code);

/**
 * Handles Set TC Policy Response
 *
 * @param status_code - status of operation
 *
 */
void ncp_host_handle_set_tc_policy_response(zb_ret_t status_code);


/* NWK API */

/**
 * Handles NWK Get IEEE By Short Response
 *
 * @param status_code - status of operation
 * @param ieee_addr - IEEE address
 */
void ncp_host_handle_nwk_get_ieee_by_short_response(zb_ret_t status_code, zb_ieee_addr_t ieee_addr);

/* TODO: Implement returning of the neighbor table entity.
         We don't use this function now, so we can leave it as is for now or remove it. */
/**
 * Handles NWK Get Neighbor By IEEE Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_get_neighbor_by_ieee_response(zb_ret_t status_code);

/**
 * Handles NWK Get Short By IEEE Response
 *
 * @param status_code - status of operation
 * @param short_addr - short address from the local address translation table
 */
void ncp_host_handle_nwk_get_short_by_ieee_response(zb_ret_t status_code, zb_uint16_t short_addr);

#ifdef ZB_APSDE_REQ_ROUTING_FEATURES
void ncp_host_handle_nwk_route_reply_indication_adapter(zb_bufid_t buf);
void ncp_host_handle_nwk_route_request_send_indication_adapter(zb_bufid_t buf);
void ncp_host_handle_nwk_route_record_send_indication_adapter(zb_bufid_t buf);
#endif
/**
 * Handles Set Fast Poll Interval PIM Attribute Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_set_fast_poll_interval_response(zb_ret_t status_code);


/**
 * Handles Set Long Poll Interval PIM Attribute Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_set_long_poll_interval_response(zb_ret_t status_code);


/**
 * Handles Start Fast Poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_start_fast_poll_response(zb_ret_t status_code);


/**
 * Handles Start Poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_start_poll_response(zb_ret_t status_code);


/**
 * Handles Stop Fast Poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_stop_fast_poll_response(zb_ret_t status_code, zb_uint8_t ncp_tsn,
  zb_stop_fast_poll_result_t stop_result);


/**
 * Handles Stop Poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_stop_poll_response(zb_ret_t status_code);


/**
 * Handles Enable turbo poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_enable_turbo_poll_response(zb_ret_t status_code);


/**
 * Handles Disable turbo poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_disable_turbo_poll_response(zb_ret_t status_code);


/**
 * Handles Turbo Poll Cancel Packet Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_turbo_poll_cancel_packet_response(zb_ret_t status_code);


/**
 * Handles Start Turbo Poll Packets Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_start_turbo_poll_packets_response(zb_ret_t status_code);


/**
 * Handles Start Turbo Poll Continuous Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_start_turbo_poll_continuous_response(zb_ret_t status_code);


/**
 * Handles Permit Turbo Poll Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_permit_turbo_poll_response(zb_ret_t status_code);


/**
 * Handles Set Keepalive Mode Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_set_keepalive_mode_response(zb_ret_t status_code);


/**
 * Handles Start Concentrator Mode Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_start_concentrator_mode_response(zb_ret_t status_code);


/**
 * Handles Stop Concentrator Mode Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_stop_concentrator_mode_response(zb_ret_t status_code);


/**
 * Handles Turbo Poll Continuous Leave Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_turbo_poll_continuous_leave_response(zb_ret_t status_code);


/**
 * Handles Turbo Poll Packets Leave Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_turbo_poll_packets_leave_response(zb_ret_t status_code);


/**
 * Handles Set Fast Poll Timeout Mode Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_nwk_set_fast_poll_timeout_response(zb_ret_t status_code);


/**
 * Handles Get Long Poll Interval Response
 *
 * @param status - status of operation
 * @param ncp_tsn - NCP TSN value
 * @param interval - fast poll interval value
 */
void ncp_host_handle_nwk_get_long_poll_interval_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint32_t interval);


/**
 * Handles Get "In Fast Poll" Flag Response
 *
 * @param status - status of operation
 * @param ncp_tsn - NCP TSN value
 * @param in_fast_poll_status - "In Fast Poll" flag value
 */
void ncp_host_handle_nwk_get_in_fast_poll_flag_response(zb_ret_t status, zb_uint8_t ncp_tsn, zb_uint8_t in_fast_poll_status);


/* Security API */

typedef ZB_PACKED_PRE struct ncp_host_secur_installcode_record_s
{
  zb_ieee_addr_t  device_address;
  zb_uint8_t      options;
  zb_uint8_t      installcode[ZB_CCM_KEY_SIZE+ZB_CCM_KEY_CRC_SIZE];
} ZB_PACKED_STRUCT ncp_host_secur_installcode_record_t;

/**
 * Handles NWK Secur Add Remote Device Installcode Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 */
void ncp_host_handle_secur_add_ic_response(zb_ret_t status_code, zb_uint8_t ncp_tsn);

/**
 * Handles Secur Get Installcodes List Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 * @param ic_table_entries - total number of installcodes
 * @param start_index - start installcode index of the request
 * @param ic_table_list_count - number of intallcodes in the response
 * @param ic_table - pointer to received installcodes table
 */
void ncp_host_handle_secur_get_ic_list_response(zb_ret_t status, zb_uint8_t ncp_tsn,
  zb_uint8_t ic_table_entries, zb_uint8_t start_index, zb_uint8_t ic_table_list_count,
  ncp_host_secur_installcode_record_t *ic_table);

/**
 * Handles Secur Get Installcode By Index Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 * @param ic_record - pointer to received installcode
 */
void ncp_host_handle_secur_get_ic_by_idx_response(zb_ret_t status, zb_uint8_t ncp_tsn,
  ncp_host_secur_installcode_record_t *ic_record);

/**
 * Handles Secur Remove Installcode Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 */
void ncp_host_handle_secur_remove_ic_response(zb_ret_t status, zb_uint8_t ncp_tsn);

/**
 * Handles NWK Initiate Key Switch Procedure Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 */
void ncp_host_handle_nwk_initiate_key_switch_procedure_response(zb_ret_t status);

/**
 * Handles Secur Remove All Installcodes Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 */
void ncp_host_handle_secur_remove_all_ic_response(zb_ret_t status, zb_uint8_t ncp_tsn);

/**
 * Handles Secur Remove Installcode Response
 *
 * @param status_code - status of operation
 * @param ncp_tsn - NCP TSN
 */
void ncp_host_handle_secur_remove_ic_response(zb_ret_t status, zb_uint8_t ncp_tsn);

/* AF API */
/**
 * Handles AF Set Simple Descriptor Response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_af_set_simple_descriptor_response(zb_ret_t status_code);

/**
 * Handles AF Delete Endpoint command response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_af_delete_endpoint_response(zb_ret_t status_code);

/**
 * Handles AF Set Node Descriptor command response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_af_set_node_descriptor_response(zb_ret_t status_code);

/**
 * Handles AF Set Power Descriptor command response
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_af_set_power_descriptor_response(zb_ret_t status_code);

/**
 * Handles Set Node Descriptor Manufacturer Code Response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_zdo_set_node_desc_manuf_code_response(zb_ret_t status_code, zb_uint8_t ncp_tsn);


/**
 * Handles Get Serial Number Response.
 *
 * @param status_code - status of operation
 */
void ncp_host_handle_get_serial_number_response(zb_ret_t status_code, const zb_uint8_t *serial_number);


/**
 * Handles Get Vendor Data Response.
 *
 * @param status_code - status of operation
 * @param vendor_data_size - length of the vendor data
 * @param vendor_data - vendor data raw bytes
 */
void ncp_host_handle_get_vendor_data_response(zb_ret_t status_code, zb_uint8_t vendor_data_size, const zb_uint8_t *vendor_data);


#endif /* NCP_HOST_HL_TRANSPORT_INTERNAL_API_H */
