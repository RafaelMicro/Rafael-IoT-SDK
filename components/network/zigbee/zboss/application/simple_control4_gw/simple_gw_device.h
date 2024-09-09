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
/* PURPOSE: Simple GW device definition
*/
#ifndef SIMPLE_GW_DEVICE_H
#define SIMPLE_GW_DEVICE_H 1

/* ALL_DEVICE_SUPPORT or following clusters should be defined:
#define ZB_ZCL_SUPPORT_CLUSTER_BASIC    1
#define ZB_ZCL_SUPPORT_CLUSTER_SCENES   1
#define ZB_ZCL_SUPPORT_CLUSTER_GROUPS   1
#define ZB_ZCL_SUPPORT_CLUSTER_IDENTIFY 1
#define ZB_ZCL_SUPPORT_CLUSTER_ON_OFF   1
#define ZB_ZCL_SUPPORT_CLUSTER_ON_OFF_SWITCH_CONFIG 1
*/

#define ZB_HA_DEVICE_VER_SIMPLE_GW 0  /*!< Simple GW device version */
#define ZB_HA_SIMPLE_GW_DEVICE_ID 0

#define ZB_HA_SIMPLE_GW_IN_CLUSTER_NUM 3  /*!< @internal Simple GW IN clusters number */
#define ZB_HA_SIMPLE_GW_OUT_CLUSTER_NUM 5 /*!< @internal Simple GW OUT clusters number */

#define ZB_HA_SIMPLE_GW_CLUSTER_NUM                                      \
  (ZB_HA_SIMPLE_GW_IN_CLUSTER_NUM + ZB_HA_SIMPLE_GW_OUT_CLUSTER_NUM)

/*! @internal Number of attribute for reporting on Simple GW device */
#define ZB_HA_SIMPLE_GW_REPORT_ATTR_COUNT \
  (ZB_ZCL_ON_OFF_SWITCH_CONFIG_REPORT_ATTR_COUNT)

/** @brief Declare cluster list for Simple GW device
    @param cluster_list_name - cluster list variable name
    @param on_off_switch_config_attr_list - attribute list for On/off switch configuration cluster
    @param basic_attr_list - attribute list for Basic cluster
    @param identify_attr_list - attribute list for Identify cluster
 */
#define ZB_HA_DECLARE_SIMPLE_GW_CLUSTER_LIST(                           \
      cluster_list_name,                                                    \
      on_off_switch_config_attr_list,                                       \
      basic_attr_list,                                                      \
      identify_attr_list)                                                   \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,                           \
          ZB_ZCL_ARRAY_SIZE(on_off_switch_config_attr_list, zb_zcl_attr_t), \
          (on_off_switch_config_attr_list),                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
          (identify_attr_list),                                             \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
          (basic_attr_list),                                                \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_SCENES,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                            \
          ZB_ZCL_CLUSTER_ID_IAS_ZONE,                                   \
          0,                                                            \
          NULL,                                                         \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                   \
          ZB_ZCL_MANUF_CODE_INVALID                                     \
          ),                                                            \
        ZB_ZCL_CLUSTER_DESC(                                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                   \
          0,                                                            \
          NULL,                                                         \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                   \
          ZB_ZCL_MANUF_CODE_INVALID                                     \
        )                                                               \
    }


/** @internal @brief Declare simple descriptor for Simple GW device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param in_clust_num - number of supported input clusters
    @param out_clust_num - number of supported output clusters
    @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
    definitions, because these values are used to form simple descriptor type name
*/
#define ZB_ZCL_DECLARE_SIMPLE_GW_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                        \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                 \
  {                                                                                           \
    ep_id,                                                                                    \
    ZB_AF_HA_PROFILE_ID,                                                                      \
    ZB_HA_SIMPLE_GW_DEVICE_ID,                                                            \
    ZB_HA_DEVICE_VER_SIMPLE_GW,                                                           \
    0,                                                                                        \
    in_clust_num,                                                                             \
    out_clust_num,                                                                            \
    {                                                                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                             \
      ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,                                                 \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                               \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                               \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                         \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE,                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
    }                                                                                         \
  }

/** @brief Declare endpoint for Simple GW device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param cluster_list - endpoint cluster list
 */
#define ZB_HA_DECLARE_SIMPLE_GW_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_SIMPLE_GW_SIMPLE_DESC(                          \
      ep_name,                                                       \
      ep_id,                                                         \
      ZB_HA_SIMPLE_GW_IN_CLUSTER_NUM,                            \
      ZB_HA_SIMPLE_GW_OUT_CLUSTER_NUM);                          \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                              ep_id,                                                  \
      ZB_AF_HA_PROFILE_ID,                                           \
      0,                                                             \
      NULL,                                                          \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),        \
      cluster_list,                                                  \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,       \
                              0, NULL, /* No reporting ctx */                         \
                              0, NULL)

/** @brief Declare Simple GW device context.
    @param device_ctx - device context variable name.
    @param ep_name - endpoint variable name.
*/
#ifdef GW_CONTROL4_COMPATIBLE
#define ZB_HA_DECLARE_SIMPLE_GW_CTX(device_ctx, ep1_name, ep2_name)     \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, ep1_name, ep2_name)
#else
#define ZB_HA_DECLARE_SIMPLE_GW_CTX(device_ctx, ep_name)                \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)
#endif
 /* No CVC ctx */

/*! @} */

#endif /* SIMPLE_GW_DEVICE_H */
