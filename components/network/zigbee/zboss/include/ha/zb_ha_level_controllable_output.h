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
/* PURPOSE: Level controllable output for HA profile
*/

#ifndef ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_H
#define ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_H 1

#if defined ZB_HA_DEFINE_DEVICE_LEVEL_CONTROLLABLE_OUTPUT || defined DOXYGEN

/** @cond DOXYGEN_HA_SECTION */

/**
 *  @defgroup ha_level_controllable_output Level Controllable Output
 *  @ingroup ZB_HA_DEVICES
    @{
    @details
    Level Control device has 6 clusters (see spec 7.4.4): \n
        - @ref ZB_ZCL_IDENTIFY \n
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_SCENES \n
        - @ref ZB_ZCL_GROUPS \n
        - @ref ZB_ZCL_ON_OFF \n
        - @ref ZB_ZCL_LEVEL_CONTROL

    Sample use Level Control device

    @par Example

    - Declaring clusters:

        @snippet HA_samples/common/zcl_basic_attr_list.h BASIC_CLUSTER_COMMON
        @snippet HA_samples/level_controllable_output/sample_zc.c COMMON_DECLARATION

    - Registering device list:

        @snippet HA_samples/level_controllable_output/sample_zc.c REGISTER

    - Example of command handler:

        @snippet HA_samples/level_controllable_output/sample_zc.c COMMAND_HANDLER
        @snippet HA_samples/level_controllable_output/sample_zc.c ZCL_COMMAND_HANDLER

    @par

    @cond ZBOSS_SAMPLES_API_DOC_LINE
    The complete set of Level Control device samples can be found in /application/HA_samples/level_control_output
    @endcond
*/

/*!< Level Controllable Output device version */
#define ZB_HA_DEVICE_VER_LEVEL_CONTROLLABLE_OUTPUT 0

/*!< Level Controllable Output IN (server) clusters number */
#define ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_IN_CLUSTER_NUM  6

/*!< Level Controllable Output OUT (client) clusters number */
#define ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_OUT_CLUSTER_NUM 0

#define ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_CLUSTER_NUM    \
  (   ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_IN_CLUSTER_NUM   \
   +  ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_OUT_CLUSTER_NUM)

/** @internal @brief Number of attribute for reporting on Level Controllable Output device */
#define ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_REPORT_ATTR_COUNT \
  (ZB_ZCL_LEVEL_CONTROL_REPORT_ATTR_COUNT + ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT)

#define ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_CVC_ATTR_COUNT 1

/** @brief Declare cluster list for Level Controllable Output device
    @param cluster_list_name - cluster list variable name
    @param basic_attr_list - attribute list for Basic cluster
    @param identify_attr_list - attribute list for Identify cluster
    @param scenes_attr_list - attribute list for Scenes cluster
    @param groups_attr_list - attribute list for Groups cluster
    @param on_off_attr_list - attribute list for On/Off cluster
    @param level_control_attr_list - attribute list for Level Control cluster
 */
#define ZB_HA_DECLARE_LEVEL_CONTROLLABLE_OUTPUT_CLUSTER_LIST(  \
    cluster_list_name,                                         \
    basic_attr_list,                                           \
    identify_attr_list,                                        \
    scenes_attr_list,                                          \
    groups_attr_list,                                          \
    on_off_attr_list,                                          \
    level_control_attr_list)                                   \
zb_zcl_cluster_desc_t cluster_list_name[] =                    \
{                                                              \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),      \
    (identify_attr_list),                                      \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_BASIC,                                   \
    ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),         \
    (basic_attr_list),                                         \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_SCENES,                                  \
    ZB_ZCL_ARRAY_SIZE(scenes_attr_list, zb_zcl_attr_t),        \
    (scenes_attr_list),                                        \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_GROUPS,                                  \
    ZB_ZCL_ARRAY_SIZE(groups_attr_list, zb_zcl_attr_t),        \
    (groups_attr_list),                                        \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_ON_OFF,                                  \
    ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),        \
    (on_off_attr_list),                                        \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                           \
    ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t), \
    (level_control_attr_list),                                 \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  )                                                            \
}


/** @internal @brief Declare simple descriptor for Level Controllable Output device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param in_clust_num - number of supported input clusters
    @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_LEVEL_CONTROLLABLE_OUTPUT_SIMPLE_DESC(                 \
  ep_name, ep_id, in_clust_num, out_clust_num)                                \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                        \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name = \
  {                                                                           \
    ep_id,                                                                    \
    ZB_AF_HA_PROFILE_ID,                                                      \
    ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_DEVICE_ID,                                \
    ZB_HA_DEVICE_VER_LEVEL_CONTROLLABLE_OUTPUT,                               \
    0,                                                                        \
    in_clust_num,                                                             \
    out_clust_num,                                                            \
    {                                                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                             \
      ZB_ZCL_CLUSTER_ID_SCENES,                                               \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                               \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                               \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL                                         \
    }                                                                         \
  }

/** @brief Declare endpoint for Level Controllable Output device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param cluster_list - endpoint cluster list
 */
#define ZB_HA_DECLARE_LEVEL_CONTROLLABLE_OUTPUT_EP(ep_name, ep_id, cluster_list)                 \
  ZB_ZCL_DECLARE_LEVEL_CONTROLLABLE_OUTPUT_SIMPLE_DESC(ep_name,                                  \
   ep_id,                                                                                        \
   ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_IN_CLUSTER_NUM,                                               \
   ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_OUT_CLUSTER_NUM);                                             \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,                           \
                                     ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_REPORT_ATTR_COUNT);         \
  ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info## device_ctx_name,                       \
                                         ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_CVC_ATTR_COUNT);        \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                               \
                              0,                                        \
                              NULL,                                     \
                          ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,  \
                          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                      \
                          ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_REPORT_ATTR_COUNT,                     \
                          reporting_info## device_ctx_name,                                      \
                          ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_CVC_ATTR_COUNT,                        \
                          cvc_alarm_info## device_ctx_name)

/*!
  @brief Declare application's device context for Level Control Switch device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/

#define ZB_HA_DECLARE_LEVEL_CONTROLLABLE_OUTPUT_CTX(device_ctx, ep_name)                                                    \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

/*! @} */

/** @endcond */ /* DOXYGEN_HA_SECTION */

#endif /* ZB_HA_DEFINE_DEVICE_LEVEL_CONTROLLABLE_OUTPUT */

#endif /* ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_H */
