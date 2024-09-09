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
/*  PURPOSE: NCP High level protocol definitions
*/
#ifndef NCP_PROD_CONFIG_H
#define NCP_PROD_CONFIG_H 1

#include "../../common/ncp_hl_proto.h"

typedef ZB_PACKED_PRE struct
{
  char serial_number[ZB_NCP_SERIAL_NUMBER_LENGTH];
  zb_uint8_t vendor_data_size;
  zb_uint8_t vendor_data[ZB_NCP_MAX_VENDOR_DATA_SIZE];
}
ZB_PACKED_STRUCT ncp_app_production_config_t;

#endif /* NCP_PROD_CONFIG_H */
