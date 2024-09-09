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
/* PURPOSE: ZCL Basic attr list
*/
#ifndef ZCL_BASIC_ATTR_LIST_H
#define ZCL_BASIC_ATTR_LIST_H 1

/* [BASIC_CLUSTER_COMMON] */
/* Basic cluster attributes data */
zb_uint8_t g_attr_basic_zcl_version = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
zb_uint8_t g_attr_basic_application_version = ZB_ZCL_BASIC_APPLICATION_VERSION_DEFAULT_VALUE;
zb_uint8_t g_attr_basic_stack_version = ZB_ZCL_BASIC_STACK_VERSION_DEFAULT_VALUE;
zb_uint8_t g_attr_basic_hw_version = ZB_ZCL_BASIC_HW_VERSION_DEFAULT_VALUE;
zb_char_t g_attr_basic_manufacturer_name[] = ZB_ZCL_BASIC_MANUFACTURER_NAME_DEFAULT_VALUE;
zb_char_t g_attr_basic_model_identifier[] = ZB_ZCL_BASIC_MODEL_IDENTIFIER_DEFAULT_VALUE;
zb_char_t g_attr_basic_date_code[] = ZB_ZCL_BASIC_DATE_CODE_DEFAULT_VALUE;
zb_uint8_t g_attr_basic_power_source = ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;
zb_char_t g_attr_basic_location_description[] = ZB_ZCL_BASIC_LOCATION_DESCRIPTION_DEFAULT_VALUE;
zb_uint8_t g_attr_basic_physical_environment = ZB_ZCL_BASIC_PHYSICAL_ENVIRONMENT_DEFAULT_VALUE;
zb_char_t g_attr_sw_build_id[] = ZB_ZCL_BASIC_SW_BUILD_ID_DEFAULT_VALUE;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list, &g_attr_basic_zcl_version, &g_attr_basic_application_version, &g_attr_basic_stack_version, &g_attr_basic_hw_version, &g_attr_basic_manufacturer_name, &g_attr_basic_model_identifier, &g_attr_basic_date_code, &g_attr_basic_power_source, &g_attr_basic_location_description, &g_attr_basic_physical_environment, &g_attr_sw_build_id);
/* [BASIC_CLUSTER_COMMON] */

#endif /* ZCL_BASIC_ATTR_LIST_H */
