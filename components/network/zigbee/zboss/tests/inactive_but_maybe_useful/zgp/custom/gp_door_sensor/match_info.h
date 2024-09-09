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
/* PURPOSE: Provides example of matching table declaration for ZGPS sink
*/

#ifndef MATCH_INFO_H
#define MATCH_INFO_H 1

#include "zb_common.h"
#include "zgp/zb_zgp.h"
#include "zgp/zb_zgpd_cmd.h"

zgps_manuf_spec_match_rec_t g_manuf_spec_tbl[] =
{
  {
    ZB_ZCL_CLUSTER_ID_IAS_ZONE,
    ZB_ZCL_CLUSTER_CLIENT_ROLE,
    {ZB_GPDF_CMD_ATTR_REPORT, ZB_GPDF_CMD_MULTI_CLUSTER_ATTR_REPORT, 0},
    ZB_ZGPD_DEF_MANUFACTURER_ID,
    ZB_ZGP_MS_DOOR_SENSOR_DEV_ID,
  }
};


zgp_to_zb_cmd_mapping_t g_cmd_mapping[] =
{
  {ZB_GPDF_CMD_ATTR_REPORT, ZB_ZCL_CMD_REPORT_ATTRIB},
};


zb_zgps_match_info_t  g_zgps_match_info = 
{
  0,
  NULL,
  ZB_ARRAY_SIZE(g_cmd_mapping),
  g_cmd_mapping,
  ZB_ARRAY_SIZE(g_manuf_spec_tbl),
  g_manuf_spec_tbl
};

#endif //MATCH_INFO_H
