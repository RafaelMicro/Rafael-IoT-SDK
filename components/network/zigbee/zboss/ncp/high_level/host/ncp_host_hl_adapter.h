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
/*  PURPOSE: NCP High level transport (adapters layer) implementation for the host side */

#ifndef NCP_HOST_HL_ADAPTER_H
#define NCP_HOST_HL_ADAPTER_H 1

#include "zb_common.h"
#include "zb_types.h"

/* Inits all adapter contexts, calls _init_ctx functions from each part of the adapter layer */
void ncp_host_adapter_init_ctx(void);
/* Inits ZDO adapter context */
void ncp_host_zdo_adapter_init_ctx(void);
/* Inits APS adapter context */
void ncp_host_aps_adapter_init_ctx(void);
/* Inits NVRAM adapter context */
void ncp_host_nvram_adapter_init_ctx(void);
/* Inits Secur adapter context */
void ncp_host_secur_adapter_init_ctx(void);


/* checks whether the dataset type is supported for the NCP host */
zb_bool_t ncp_host_nvram_dataset_is_supported(zb_nvram_dataset_types_t t);

zb_ret_t ncp_host_nvram_read_dataset(zb_nvram_dataset_types_t type,
                                     zb_uint8_t* buf, zb_uint16_t ds_len,
                                     zb_uint16_t ds_ver, zb_nvram_ver_t nvram_ver);

#endif /* NCP_HOST_HL_ADAPTER_H */
