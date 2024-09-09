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
/*  PURPOSE: NCP High level transport (adaptors layer) implementation for the host side: APS
 *  category */

#ifndef NCP_HOST_HL_ADAPTOR_H
#define NCP_HOST_HL_ADAPTOR_H

#include "zb_common.h"
#include "zb_types.h"
#include "ncp/ncp_host_api.h"

#include "zb_aps.h"

/* Functions to be called after upper layer, to write data to transport buffers */
zb_ret_t ncp_host_apsde_data_request_transport(zb_apsde_data_req_t *req, zb_uint8_t param_len,
                                               zb_uint16_t data_len, zb_uint8_t *data_ptr,
                                               zb_uint8_t *tsn);

zb_ret_t ncp_host_nwk_formation_transport(zb_nlme_network_formation_request_t *req);

zb_ret_t ncp_host_nwk_permit_joining_transport(zb_nlme_permit_joining_request_t *req);

/* Functions to be called before upper layer, to read data from transport buffers */
void adaptor_handle_apsde_data_indication(zb_apsde_data_indication_t *ind,
                                          zb_uint8_t *data_ptr,
                                          zb_uint8_t params_len, zb_uint16_t data_len);

void adaptor_handle_apsde_data_request_response(zb_apsde_data_confirm_t *conf);

void adaptor_handle_nwk_formation_response(zb_ret_t status);

void adaptor_handle_nwk_permit_joining_response(zb_ret_t status);

#endif /* NCP_HOST_HL_ADAPTOR_H */
