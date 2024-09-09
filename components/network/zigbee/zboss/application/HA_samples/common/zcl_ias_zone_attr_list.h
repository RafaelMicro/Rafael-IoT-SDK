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
/* PURPOSE: ZCL IAS Zone attr list
*/
#ifndef ZCL_IAS_ZONE_ATTR_LIST_H
#define ZCL_IAS_ZONE_ATTR_LIST_H 1

/* [IAS_ZONE_CLUSTER_COMMON] */
/* IAS Zone cluster attributes data */
zb_uint8_t g_attr_ias_zone_zonestate = ZB_ZCL_IAS_ZONE_ZONESTATE_DEF_VALUE;
zb_uint16_t g_attr_ias_zone_zonetype = 0;
zb_uint16_t g_attr_ias_zone_zonestatus = ZB_ZCL_IAS_ZONE_ZONE_STATUS_DEF_VALUE;
zb_uint8_t g_attr_ias_zone_number_of_zone_sensitivity_levels_supported = ZB_ZCL_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_DEFAULT_VALUE;
zb_uint8_t g_attr_ias_zone_current_zone_sensitivity_level = ZB_ZCL_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_DEFAULT_VALUE;
zb_ieee_addr_t g_attr_ias_zone_ias_cie_address = {0, 0, 0, 0, 0, 0, 0, 0};
zb_uint8_t g_attr_ias_zone_zoneid_ha = ZB_ZCL_IAS_ZONEID_ID_DEF_VALUE;
zb_uint16_t cie_short_addr = 0;
zb_uint8_t cie_ep = 0;
ZB_ZCL_DECLARE_IAS_ZONE_ATTRIB_LIST_EXT(ias_zone_attr_list, &g_attr_ias_zone_zonestate, &g_attr_ias_zone_zonetype, &g_attr_ias_zone_zonestatus, &g_attr_ias_zone_number_of_zone_sensitivity_levels_supported, &g_attr_ias_zone_current_zone_sensitivity_level, &g_attr_ias_zone_ias_cie_address, &g_attr_ias_zone_zoneid_ha, &cie_short_addr, &cie_ep);
/* [IAS_ZONE_CLUSTER_COMMON] */

#endif /* ZCL_IAS_ZONE_ATTR_LIST_H */
