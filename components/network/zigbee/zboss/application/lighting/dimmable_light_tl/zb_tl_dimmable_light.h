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
/* PURPOSE: Touchlink Dimmable Light device definition
*/

#ifndef ZB_TL_DIMMABLE_LIGHT_H
#define ZB_TL_DIMMABLE_LIGHT_H 1

/** @cond touchlink */

#include "zboss_api_tl.h"

/**
 *  @defgroup ZB_TL_DIMMABLE_LIGHT Dimmable light device.
 *  @ingroup ZB_TL_DEVICES
 *  @addtogroup ZB_TL_DIMMABLE_LIGHT
 *  Touchlink Dimmable light device.
 *  @{
    @details
    Dimmable light device has 6 clusters (see spec 5.2.3): \n
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_IDENTIFY \n
        - @ref ZB_ZCL_GROUPS \n
        - @ref ZB_ZCL_SCENES \n
        - @ref ZB_ZCL_ON_OFF \n
        - @ref ZB_ZCL_LEVEL_CONTROL
*/

#define ZB_TL_DEVICE_VER_DIMMABLE_LIGHT 0      /**< Device version */

#define ZB_TL_DIMMABLE_LIGHT_DEVICE_ID 0x0100  /**< Touchlink Dimmable light device ID */

#define ZB_TL_DIMMABLE_LIGHT_IN_CLUSTER_NUM  7 /**< Dimmable light input clusters number. */
#define ZB_TL_DIMMABLE_LIGHT_OUT_CLUSTER_NUM 1 /**< Dimmable light output clusters number. */

/*! @internal Number of attribute for reporting on Dimmable light device */
#define ZB_TL_DIMMABLE_LIGHT_REPORT_ATTR_COUNT \
  (ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT + ZB_ZCL_LEVEL_CONTROL_REPORT_ATTR_COUNT)

#define ZB_TL_DIMMABLE_LIGHT_CVC_ATTR_COUNT 1

/**
 *  @brief Declare cluster list for Dimmable light device.
 *  @param cluster_list_name [IN] - cluster list variable name.
 *  @param basic_attr_list [IN] - attribute list for Basic cluster.
 *  @param identify_attr_list [IN] - attribute list for Identify cluster.
 *  @param groups_attr_list [IN] - attribute list for Groups cluster.
 *  @param scenes_attr_list [IN] - attribute list for Scenes cluster.
 *  @param on_off_attr_list [IN] - attribute list for On/Off cluster.
 *  @param level_control_attr_list [IN] - attribute list for Level Control cluster.
 *  @param commissioning_declare_both [IN] - determines Commissioning cluster role: ZB_TRUE implies
 *  both client and server, and ZB_FALSE implies server role only.
 */
#define ZB_TL_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(                                              \
  cluster_list_name,                                                                            \
  basic_attr_list,                                                                              \
  identify_attr_list,                                                                           \
  groups_attr_list,                                                                             \
  scenes_attr_list,                                                                             \
  on_off_attr_list,                                                                             \
  level_control_attr_list,                                                                      \
  commissioning_declare_both)                                                                   \
      zb_zcl_cluster_desc_t cluster_list_name[] =                                               \
      {                                                                                         \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                                    \
          (basic_attr_list),                                                                    \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
        ),                                                                                      \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                           \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),                                 \
          (identify_attr_list),                                                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
        ),                                                                                      \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                                             \
          ZB_ZCL_ARRAY_SIZE(groups_attr_list, zb_zcl_attr_t),                                   \
          (groups_attr_list),                                                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
        ),                                                                                      \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_SCENES,                                                             \
          ZB_ZCL_ARRAY_SIZE(scenes_attr_list, zb_zcl_attr_t),                                   \
          (scenes_attr_list),                                                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
        ),                                                                                      \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                                             \
          ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),                                   \
          (on_off_attr_list),                                                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
        ),                                                                                      \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                      \
          ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t),                            \
          (level_control_attr_list),                                                            \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
        ),                                                                                      \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                                            \
          0,                                                                                    \
          NULL,                                                                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
          ),                                                                                    \
        ZB_ZCL_CLUSTER_DESC(                                                                    \
          ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                                            \
          0,                                                                                    \
          NULL,                                                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                           \
          ZB_ZCL_MANUF_CODE_INVALID                                                             \
          )                                                                                     \
      }

/**
 *  @brief Declare simple descriptor for Dimmable light device.
 *  @param ep_name - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param in_clust_num [IN] - number of supported input clusters.
 *  @param out_clust_num [IN] - number of supported output clusters.
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name.
 */
#define ZB_ZCL_DECLARE_TL_DIMMABLE_LIGHT_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)  \
      ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                    \
      ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name =            \
      {                                                                                       \
        ep_id,                                                                                \
        ZB_AF_ZLL_PROFILE_ID,                                                                 \
        ZB_TL_DIMMABLE_LIGHT_DEVICE_ID,                                                       \
        ZB_TL_DEVICE_VER_DIMMABLE_LIGHT,                                                      \
        0,                                                                                    \
        in_clust_num,                                                                         \
        out_clust_num,                                                                        \
        {                                                                                     \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                                           \
          ZB_ZCL_CLUSTER_ID_SCENES,                                                           \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                                           \
          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                    \
          ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                                          \
          ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING                                           \
        }                                                                                     \
      }

/**
 *  @brief Declare endpoint for dimmable light device.
 *  @param ep_name [IN] - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param cluster_list [IN] - endpoint cluster list.
 */
#define ZB_TL_DECLARE_DIMMABLE_LIGHT_EP(ep_name, ep_id, cluster_list)              \
      ZB_ZCL_DECLARE_TL_DIMMABLE_LIGHT_SIMPLE_DESC(                                \
          ep_name,                                                                 \
          ep_id,                                                                   \
          ZB_TL_DIMMABLE_LIGHT_IN_CLUSTER_NUM,                                     \
          ZB_TL_DIMMABLE_LIGHT_OUT_CLUSTER_NUM);                                   \
      ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,         \
                                         ZB_TL_DIMMABLE_LIGHT_REPORT_ATTR_COUNT);  \
      ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info## device_ctx_name,     \
                                             ZB_TL_DIMMABLE_LIGHT_CVC_ATTR_COUNT); \
      ZB_AF_DECLARE_ENDPOINT_DESC(                                                 \
        ep_name,                                                                   \
        ep_id,                                                                     \
        ZB_AF_ZLL_PROFILE_ID,                                                       \
        0,     \
        NULL,                            \
        ZB_ZCL_ARRAY_SIZE(                                                         \
          cluster_list,                                                            \
          zb_zcl_cluster_desc_t),                                                  \
        cluster_list,                                                              \
        (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
        ZB_TL_DIMMABLE_LIGHT_REPORT_ATTR_COUNT, reporting_info## device_ctx_name,  \
        ZB_TL_DIMMABLE_LIGHT_CVC_ATTR_COUNT, cvc_alarm_info## device_ctx_name)

#define ZB_TL_DECLARE_DIMMABLE_LIGHT_CTX(device_ctx, ep_name)  \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

/**
 *  @}
 */

/** @endcond */ /* touchlink */

#endif /* ZB_TL_DIMMABLE_LIGHT_H */
