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
/* PURPOSE: ZDO common functions, both client and server side
*/

#ifndef ZDO_COMMON_H
#define ZDO_COMMON_H 1

#define ZB_TSN_HASH(tsn) ZB_1INT_HASH_FUNC(tsn) % ZDO_TRAN_TABLE_SIZE

zb_uint8_t zdo_send_req_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_callback_t cb,
                                 zb_uint16_t addr, zb_uint8_t resp_counter);
zb_uint8_t zdo_send_req_by_long(zb_uint8_t command_id, zb_uint8_t param, zb_callback_t cb,
                                zb_ieee_addr_t addr);
void zdo_send_resp_by_short(zb_uint16_t command_id, zb_uint8_t param, zb_uint16_t addr);

/*
   Signal user that response came.
   param param - index of buffer with response
*/
zb_ret_t zdo_af_resp(zb_uint8_t param);

void zdo_handle_mgmt_leave_rsp(zb_uint16_t src);


/*! \addtogroup legacy_callbacks */
/*! @{ */

zb_uint8_t* zb_copy_cluster_id(zb_uint8_t *cluster_dst, zb_uint8_t *cluster_src, zb_uint8_t cluster_num);

#endif /* ZDO_COMMON_H */
