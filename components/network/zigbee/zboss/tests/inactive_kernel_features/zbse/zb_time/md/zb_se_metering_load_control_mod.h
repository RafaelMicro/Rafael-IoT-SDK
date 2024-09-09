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
/* PURPOSE: Smart Energy Metering & Load Control (MLC) device definition
*/

#ifndef ZB_SE_METERING_LOAD_CONTROL_MOD_H
#define ZB_SE_METERING_LOAD_CONTROL_MOD_H 1

#if 1

/** @cond DOXYGEN_SE_SECTION */

/** @defgroup mlc_device Metering Load Control device
 *  @ingroup se_devices
 * @{
 */

/** @def ZB_SE_DEVICE_VER_MLC
 *  @brief Device version
 */
#define ZB_SE_DEVICE_VER_MLC      0

/** @cond internal */

#define ZB_SE_MLC_MOD_PHYSICAL_DEVICE_IN_CLUSTER_NUM  2
#define ZB_SE_MLC_MOD_PHYSICAL_DEVICE_OUT_CLUSTER_NUM  1
#define ZB_SE_MLC_MOD_PHYSICAL_DEVICE_CLUSTER_NUM \
  (ZB_SE_MLC_MOD_PHYSICAL_DEVICE_IN_CLUSTER_NUM + ZB_SE_MLC_MOD_PHYSICAL_DEVICE_OUT_CLUSTER_NUM)

#define ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_IN_CLUSTER_NUM  2
#define ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_OUT_CLUSTER_NUM  0
#define ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_CLUSTER_NUM \
  (ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_IN_CLUSTER_NUM + ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_OUT_CLUSTER_NUM)

#define ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_IN_CLUSTER_NUM  0
#define ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_OUT_CLUSTER_NUM  2
#define ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_CLUSTER_NUM \
  (ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_IN_CLUSTER_NUM + ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_OUT_CLUSTER_NUM)

/** @def ZB_SE_MLC_MOD_REPORT_ATTR_COUNT
 *  @brief Number of attributes for reporting on Metering Load Control device
 */
#define ZB_SE_MLC_MOD_REPORT_ATTR_COUNT (ZB_ZCL_METERING_REPORT_ATTR_COUNT)

/** @endcond */


/** @def ZB_SE_DECLARE_MLC_MOD_PHYSICAL_DEVICE_CLUSTER_LIST
 *  @brief Declare cluster list for Metering Load Control device.
 *  @param cluster_list_name - cluster list variable name
 *  @param basic_attr_list - attribute list for Basic cluster
 *  @param kec_attr_list - attribute list for Key Establishment cluster
 */
#define ZB_SE_DECLARE_MLC_MOD_PHYSICAL_DEVICE_CLUSTER_LIST(cluster_list_name, \
                                                          basic_attr_list,    \
                                                          kec_attr_list) \
  zb_zcl_cluster_desc_t cluster_list_name[] =                          \
  {                                                                   \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_BASIC,                                        \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),              \
      (basic_attr_list),                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                \
      (kec_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                \
      (kec_attr_list),                                                \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
  }


#define ZB_SE_DECLARE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_CLUSTER_LIST(cluster_list_name,  \
                                                                       metering_attr_list, \
                                                                       time_attr_list)     \
  zb_zcl_cluster_desc_t cluster_list_name[] =                                              \
  {                                                                                        \
    ZB_ZCL_CLUSTER_DESC(                                                                   \
      ZB_ZCL_CLUSTER_ID_TIME,                                                              \
      ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),                                    \
      (time_attr_list),                                                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                          \
      ZB_ZCL_MANUF_CODE_INVALID                                                            \
    ),                                                                                     \
    ZB_ZCL_CLUSTER_DESC(                                                                   \
      ZB_ZCL_CLUSTER_ID_METERING,                                                          \
      ZB_ZCL_ARRAY_SIZE(metering_attr_list, zb_zcl_attr_t),                                \
      (metering_attr_list),                                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                          \
      ZB_ZCL_MANUF_CODE_INVALID                                                            \
    ),                                                                                     \
  }


#define ZB_SE_DECLARE_MLC_MOD_LOAD_CONTROL_DEVICE_CLUSTER_LIST(cluster_list_name, \
                                                       drlc_attr_list)            \
  zb_zcl_cluster_desc_t cluster_list_name[] =                                     \
  {                                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                          \
      ZB_ZCL_CLUSTER_ID_DRLC,                                                     \
      ZB_ZCL_ARRAY_SIZE(drlc_attr_list, zb_zcl_attr_t),                           \
      (drlc_attr_list),                                                           \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                                   \
      ),                                                                          \
    ZB_ZCL_CLUSTER_DESC(                                                          \
      ZB_ZCL_CLUSTER_ID_TIME,                                                     \
      0,                                                                          \
      NULL,                                                                       \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                                   \
      ),                                                                          \
  }

/** @cond internals_doc */

/** @def ZB_ZCL_DECLARE_MLC_MOD_SIMPLE_DESC
 *  @brief Declare simple descriptor for Metering Load Control device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param in_clust_num   - number of supported input clusters
 *  @param out_clust_num  - number of supported output clusters
 *
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name
 */
#define ZB_ZCL_DECLARE_MLC_MOD_PHYSICAL_DEVICE_SIMPLE_DESC(ep_name,                 \
                                                           ep_id,                   \
                                                           in_cluster_num,          \
                                                           out_cluster_num)         \
  ZB_DECLARE_SIMPLE_DESC(in_cluster_num, out_cluster_num);                          \
  ZB_AF_SIMPLE_DESC_TYPE(in_cluster_num, out_cluster_num)  simple_desc_##ep_name =  \
  {                                                                                 \
    ep_id,                                                                          \
    ZB_AF_SE_PROFILE_ID,                                                            \
    ZB_SE_PHYSICAL_DEVICE_ID,                                                       \
    ZB_SE_DEVICE_VER_MLC,                                                           \
    0,                                                                              \
    in_cluster_num,                                                                 \
    out_cluster_num,                                                                \
    {                                                                               \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                      \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                          \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                          \
    }                                                                               \
  }


#define ZB_ZCL_DECLARE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_SIMPLE_DESC(ep_name,         \
                                                                       ep_id,           \
                                                                       in_cluster_num,  \
                                                                       out_cluster_num) \
  ZB_DECLARE_SIMPLE_DESC(in_cluster_num, out_cluster_num);                              \
  ZB_AF_SIMPLE_DESC_TYPE(in_cluster_num, out_cluster_num)  simple_desc_##ep_name =      \
  {                                                                                     \
    ep_id,                                                                              \
    ZB_AF_SE_PROFILE_ID,                                                                \
    ZB_SE_REMOTE_COMMUNICATIONS_DEVICE_ID,                                              \
    ZB_SE_DEVICE_VER_MLC,                                                               \
    0,                                                                                  \
    in_cluster_num,                                                                     \
    out_cluster_num,                                                                    \
    {                                                                                   \
      ZB_ZCL_CLUSTER_ID_METERING,                                                       \
      ZB_ZCL_CLUSTER_ID_TIME,                                                           \
    }                                                                                   \
  }

#define ZB_ZCL_DECLARE_MLC_MOD_LOAD_CONTROL_DEVICE_SIMPLE_DESC(ep_name,            \
                                                       ep_id,                      \
                                                       in_cluster_num,             \
                                                       out_cluster_num)            \
  ZB_DECLARE_SIMPLE_DESC(in_cluster_num, out_cluster_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_cluster_num, out_cluster_num)  simple_desc_##ep_name = \
  {                                                                                \
    ep_id,                                                                         \
    ZB_AF_SE_PROFILE_ID,                                                           \
    ZB_SE_LOAD_CONTROL_DEVICE_ID,                                                  \
    ZB_SE_DEVICE_VER_MLC,                                                          \
    0,                                                                             \
    in_cluster_num,                                                                \
    out_cluster_num,                                                               \
    {                                                                              \
      ZB_ZCL_CLUSTER_ID_DRLC,                                                      \
      ZB_ZCL_CLUSTER_ID_TIME,                                                      \
    }                                                                              \
  }

/** @endcond */

/** @def ZB_SE_DECLARE_MLC_MOD_PHYSICAL_DEVICE_EP
 *  @brief Declare endpoint for Metering Load Control device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param cluster_list - endpoint cluster list
 */
#define ZB_SE_DECLARE_MLC_MOD_PHYSICAL_DEVICE_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_MLC_MOD_PHYSICAL_DEVICE_SIMPLE_DESC(ep_name,               \
                                                 ep_id,                 \
                                                 ZB_SE_MLC_MOD_PHYSICAL_DEVICE_IN_CLUSTER_NUM,\
                                                 ZB_SE_MLC_MOD_PHYSICAL_DEVICE_OUT_CLUSTER_NUM); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                  \
                              ep_id,                                    \
                              ZB_AF_SE_PROFILE_ID,                      \
                              0,                                        \
                              NULL,                                     \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                             \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name, \
                              0, NULL,                                  \
                              0, NULL)

#define ZB_SE_DECLARE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_EP(ep_name, ep_id, cluster_list)                 \
  ZB_ZCL_DECLARE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_SIMPLE_DESC(ep_name,               \
                                                 ep_id,                 \
                                                 ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_IN_CLUSTER_NUM,\
                                                 ZB_SE_MLC_MOD_REMOTE_COMMUNICATION_DEVICE_OUT_CLUSTER_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##device_ctx_name,                            \
                                     ZB_SE_MLC_MOD_REPORT_ATTR_COUNT);                      \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                  \
                              ep_id,                                    \
                              ZB_AF_SE_PROFILE_ID,                      \
                              0,                                        \
                              NULL,                                     \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                             \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name, \
                              ZB_SE_MLC_MOD_REPORT_ATTR_COUNT, reporting_info##device_ctx_name, \
                              0, NULL)                                  \

#define ZB_SE_DECLARE_MLC_MOD_LOAD_CONTROL_DEVICE_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_MLC_MOD_LOAD_CONTROL_DEVICE_SIMPLE_DESC(ep_name,               \
                                                 ep_id,                 \
                                                 ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_IN_CLUSTER_NUM,\
                                                 ZB_SE_MLC_MOD_LOAD_CONTROL_DEVICE_OUT_CLUSTER_NUM); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                  \
                              ep_id,                                    \
                              ZB_AF_SE_PROFILE_ID,                      \
                              0,                                        \
                              NULL,                                     \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                             \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name, \
                              0, NULL,                                  \
                              0, NULL)

/** @} */ /* mlc_device */

/** @endcond */ /* DOXYGEN_SE_SECTION */

#endif /* ZB_SE_DEFINE_DEVICE_METERING_LOAD_CONTROL */

#endif /* ZB_SE_METERING_LOAD_CONTROL_MOD_H */
