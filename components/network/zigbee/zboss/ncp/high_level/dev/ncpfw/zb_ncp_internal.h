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
/*  PURPOSE: Internal header for NCP low level protocol
*/
#ifndef ZB_NCP_LL_H
#define ZB_NCP_LL_H 1

#include "ncp/zb_ncp_ll_dev.h"
//#include "ncp_ll_host.h"
#include "ncp_prod_config.h"

/**
 * @name NCP call types
 * @anchor ncp_qcall_type
 */
/** @{ */
#define ZBOSS_SIGNAL        0U
#define NCP_SIGNAL          1U
#define NCP_SE_SIGNAL       2U
#define NCP_DATA_OR_ZDO_IND 3U
#define NCP_DATA_REQ        4U
/** @} */

/**
 * @brief Type for NCP call types.
 *
 * Holds one of @ref ncp_qcall_type. Kept only for backward compatibility as
 * @ref ncp_qcall_type were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t ncp_qcall_type_t;

ncp_ll_packet_received_cb_t ncp_hl_proto_init(void);

/* API for NCP app */
void ncp_hl_mark_ffd_just_boot(void);
void ncp_hl_send_reset_resp(zb_ret_t status);
void ncp_hl_send_reset_ind(zb_uint8_t rst_src);
void ncp_hl_reset_notify_ncp(zb_ret_t status);
void ncp_hl_rx_device_annce(zb_zdo_signal_device_annce_params_t *params);
void ncp_hl_data_or_zdo_ind(zb_bool_t is_zdo, zb_uint8_t param);
void ncp_hl_data_or_zdo_ind_exec(zb_bool_t is_zdo, zb_uint8_t param);
zb_uint8_t ncp_hl_data_or_zdo_cmd_ind(zb_uint8_t param);
void ncp_hl_nwk_leave_itself(zb_zdo_signal_leave_params_t *leave_params);
void ncp_hl_nwk_leave_ind(zb_zdo_signal_leave_indication_params_t *leave_ind_params);
void ncp_hl_nwk_pan_id_conflict_ind(zb_pan_id_conflict_info_t *pan_id_params);
zb_bool_t ncp_hl_is_manufacture_mode(void);
void ncp_hl_manuf_init_cont(zb_uint8_t param);
#ifndef SNCP_MODE
void ncp_hl_nwk_device_authorized_ind(zb_zdo_signal_device_authorized_params_t *params);
void ncp_hl_nwk_device_update_ind(zb_zdo_signal_device_update_params_t *params);
#endif

zb_ret_t ncp_sync_kec_subg_for_ep(zb_af_simple_desc_1_1_t *dsc);
void ncp_kec_tc_init(void);
zb_bool_t ncp_have_kec(void);
zb_bool_t ncp_partner_lk_inprogress(void);
void ncp_hl_partner_lk_rsp(zb_ret_t status);
void ncp_hl_partner_lk_ind(zb_uint8_t *peer_addr);

void ncp_set_kec_suite(zb_uint16_t suite);
void ncp_hl_set_prod_config(const ncp_app_production_config_t *prod_cfg);

void ncp_enqueue_buf(ncp_qcall_type_t call_type, zb_uint8_t param, zb_bufid_t buf);
void ncp_send_packet(void *data, zb_uint32_t len);
zb_bool_t ncp_exec_blocked(void);

#if defined TC_SWAPOUT && !defined ZB_COORDINATOR_ONLY
void ncp_hl_tc_swapped_signal(zb_uint8_t param);
#endif

#ifdef ZB_HAVE_CALIBRATION
#define ZBS_NCP_RESERV_CRYSTAL_CAL_ID  0x01U           /* Crystal Calibration ID */
zb_ret_t ncp_res_flash_read_by_id(zb_uint8_t id, zb_uint32_t *out_value);
#endif

#ifdef ZB_ZCL_SUPPORT_CLUSTER_SUBGHZ
void ncp_hl_subghz_duty_cycle_msg(zb_uint8_t time_mins);
#endif

void ncp_comm_commissioning_force_link(void);
void ncp_comm_se_link_key_refresh_alarm(zb_bufid_t param);

#ifdef ZB_ENABLE_INTER_PAN_NON_DEFAULT_CHANNEL
zb_uint8_t ncp_hl_intrp_cmd_ind(zb_uint8_t param, zb_uint8_t current_page, zb_uint8_t current_channel);
#endif /* ZB_ENABLE_INTER_PAN_NON_DEFAULT_CHANNEL */

#endif /* ZB_NCP_LL_H */
