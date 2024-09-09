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
/* PURPOSE: ERL Interface device definition
*/

#ifndef ZB_ERL_INTERFACE_H
#define ZB_ERL_INTERFACE_H 1

#include "zboss_api.h"

/** @cond DOXYGEN_SECTION */

/**
 *  @defgroup erl_interface Emetteur Radio Local
 *  @ingroup ZB_ERL_SAMPLES
 *  @details Emetteur Radio Local
 *
 *  @par Supported clusters
 *  ERL Device has X clusters (see Zigbee ERL Interface Device Specification Version 0.1, section 6.2): \n
 *  <table>
 *    <tr>
 *      <th>Server roles</th> <th>Client roles</th>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_BASIC</td> <td>@ref ZB_ZCL_IDENTIFY</td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_MESSAGING</td> <td>@ref ZB_ZCL_TIME </td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_PRICE</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_TIME</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_CALENDAR </td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_METER_IDENTIFICATION</td> <td> </td>
 *    </tr>
 *    <tr>
*       <td>@ref ZB_ZCL_ELECTRICAL_MEASUREMENT</td> <td> </td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_DIAGNOSTICS</td> <td> </td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_IDENTIFY</td> <td> </td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_METERING</td> <td> </td>
 *    </tr>
 *  </table>
 *  @par Example
 *  @n Declare clusters
 *  @snippet erl_interface_zc.c DECLARE_CLUSTERS
 *  @n Register device context
 *  @snippet erl_interface_zc.c REGISTER_DEVICE_CTX
 *  @n Signal handler example
 *  @snippet erl_interface_zc.c SIGNAL_HANDLER
 *  @{
 */

/** @defgroup ERL_CALENDAR_CLUSTER_DEFINITIONS Common Calendar cluster attribute defintions
 *  @defgroup ERL_IDENTIFY_CLUSTER_DEFINITIONS Common Identify cluster attribute defintions
 *  @defgroup ERL_METER_IDENTIFICATION_CLUSTER_DEFINITIONS Common Meter Identification cluster attribute defintions
 *  @defgroup ERL_PRICE_CLUSTER_DEFINITIONS Common Price cluster attribute defintions
 *  @defgroup ERL_METERING_CLUSTER_DEFINITIONS Common Metering cluster attribute defintions
 *  @defgroup ERL_ELECTRICAL_MEASUREMENT_CLUSTER_DEFINITIONS Common Electrical Measurement cluster attribute defintions
 *  @defgroup ERL_BASIC_CLUSTER_DEFINITIONS Common Basic cluster attribute defintions
 *  @defgroup ERL_DIAGNOSTICS_CLUSTER_DEFINITIONS Common Diagnostics cluster attribute defintions
 *  @defgroup ERL_TIME_CLUSTER_DEFINITIONS Common Time cluster attribute defintions
 */

#define ZB_ERL_INTERFACE_DEVICE_ID 0x0509
/* FIXME: Use proper Device ID */
#define ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEVICE_ID 0x0510

/** @def ZB_DEVICE_VER_ERL_INTERFACE
 *  @brief ERL device version
 */
#define ZB_DEVICE_VER_ERL_INTERFACE          0

/** @cond internal */

/** @def ZB_ERL_INTERFACE_DEV_IN_CLUSTER_NUM
 *  @brief ERL IN clusters number
 */
#define ZB_ERL_INTERFACE_DEV_IN_CLUSTER_NUM  10

/** @def ZB_ERL_INTERFACE_DEV_OUT_CLUSTER_NUM
 *  @brief ERL OUT clusters number
 */
#define ZB_ERL_INTERFACE_DEV_OUT_CLUSTER_NUM 2

/** @def ZB_ERL_INTERFACE_DEV_CLUSTER_NUM
 *  @brief Number of clusters for ERL device
 */
#define ZB_ERL_INTERFACE_DEV_CLUSTER_NUM \
  (ZB_ERL_INTERFACE_DEV_IN_CLUSTER_NUM + ZB_ERL_INTERFACE_DEV_OUT_CLUSTER_NUM)

/** @def ZB_ERL_INTERFACE_DEV_REPORT_ATTR_COUNT
 *  @brief Number of attributes for reporting on ERL device
 */
#define ZB_ERL_INTERFACE_DEV_REPORT_ATTR_COUNT (ZB_ZCL_METERING_REPORT_ATTR_COUNT)

/** @def ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_IN_CLUSTER_NUM
 *  @brief ERL IN clusters number
 */
#define ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_IN_CLUSTER_NUM  3

/** @def ZB_ERL_INTERFACE_DEV_OUT_CLUSTER_NUM
 *  @brief ERL OUT clusters number
 */
#define ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_OUT_CLUSTER_NUM 1

/** @def ZB_ERL_INTERFACE_DEV_CLUSTER_NUM
 *  @brief Number of clusters for ERL device
 */
#define ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_CLUSTER_NUM \
  (ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_IN_CLUSTER_NUM + \
   ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_OUT_CLUSTER_NUM)

/** @def ZB_ERL_INTERFACE_DEV_REPORT_ATTR_COUNT
 *  @brief Number of attributes for reporting on ERL device
 */
#define ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_REPORT_ATTR_COUNT \
  (ZB_ZCL_METERING_REPORT_ATTR_COUNT)

/** @endcond */ /* internal */

/** @def ZB_DECLARE_ERL_INTERFACE_DEV_CLUSTER_LIST
 *  @brief Declare cluster list for ERL device.
 *  @param cluster_list_name - cluster list variable name
 *  @param basic_attr_list - attribute list for Basic cluster
 *  @param price_attr_list - attribute list for Price cluster
 *  @param time_attr_list - attribute list for Time cluster
 */
#define ZB_DECLARE_ERL_INTERFACE_DEV_CLUSTER_LIST(cluster_list_name,    \
                                                  basic_attr_list,      \
                                                  identify_attr_list,   \
                                                  time_attr_list,       \
                                                  meter_identification_attr_list, \
                                                  el_measurement_attr_list, \
                                                  diagnostics_attr_list, \
                                                  price_attr_list,      \
                                                  metering_attr_list,   \
                                                  calendar_attr_list)   \
  zb_zcl_cluster_desc_t cluster_list_name[] =                           \
  {                                                                     \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
      (basic_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_PRICE,                                          \
      ZB_ZCL_ARRAY_SIZE(price_attr_list, zb_zcl_attr_t),                \
      (price_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_TIME,                                           \
      ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),                 \
      (time_attr_list),                                                 \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
      (identify_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,                           \
      ZB_ZCL_ARRAY_SIZE(meter_identification_attr_list, zb_zcl_attr_t), \
      (meter_identification_attr_list),                                 \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,                         \
      ZB_ZCL_ARRAY_SIZE(el_measurement_attr_list, zb_zcl_attr_t),       \
      (el_measurement_attr_list),                                       \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                                    \
      ZB_ZCL_ARRAY_SIZE(diagnostics_attr_list, zb_zcl_attr_t),          \
      (diagnostics_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_METERING,                                       \
      ZB_ZCL_ARRAY_SIZE(metering_attr_list, zb_zcl_attr_t),             \
      (metering_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_CALENDAR,                                       \
      ZB_ZCL_ARRAY_SIZE(calendar_attr_list, zb_zcl_attr_t),             \
      (calendar_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_MESSAGING,                                      \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_TIME,                                           \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
  }

/** @cond internals_doc */

/** @def ZB_ZCL_DECLARE_ERL_INTERFACE_DEV_SIMPLE_DESC
 *  @brief Declare simple descriptor for ERL device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param in_clust_num   - number of supported input clusters
 *  @param out_clust_num  - number of supported output clusters
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name
 */
#define ZB_ZCL_DECLARE_ERL_INTERFACE_DEV_SIMPLE_DESC(ep_name,           \
                                                     ep_id,             \
                                                     in_clust_num,      \
                                                     out_clust_num)     \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                  \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                     \
    ep_id,                                                              \
    ZB_AF_HA_PROFILE_ID,                                                \
    ZB_ERL_INTERFACE_DEVICE_ID,                                         \
    ZB_DEVICE_VER_ERL_INTERFACE,                                        \
    0,                                                                  \
    in_clust_num,                                                       \
    out_clust_num,                                                      \
    {                                                                   \
      /* Server clusters */                                             \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_CLUSTER_ID_TIME,                                           \
      ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,                           \
      ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,                         \
      ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                                    \
      ZB_ZCL_CLUSTER_ID_PRICE,                                          \
      ZB_ZCL_CLUSTER_ID_METERING,                                       \
      ZB_ZCL_CLUSTER_ID_MESSAGING,                                      \
      ZB_ZCL_CLUSTER_ID_CALENDAR,                                       \
      /* Client clusters */                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_CLUSTER_ID_TIME,                                           \
    }                                                                   \
  }

/** @endcond */

/** @def ZB_DECLARE_ERL_INTERFACE_DEV_EP
 *  @brief Declare endpoint for ERL device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param cluster_list - endpoint cluster list
 */
#define ZB_DECLARE_ERL_INTERFACE_DEV_EP(ep_name, ep_id, cluster_list)                 \
  ZB_ZCL_DECLARE_ERL_INTERFACE_DEV_SIMPLE_DESC(ep_name,                               \
                                               ep_id,                                 \
                                               ZB_ERL_INTERFACE_DEV_IN_CLUSTER_NUM,   \
                                               ZB_ERL_INTERFACE_DEV_OUT_CLUSTER_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##device_ctx_name,                 \
                                     ZB_ERL_INTERFACE_DEV_REPORT_ATTR_COUNT);         \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                              ep_id,                                                  \
                              ZB_AF_HA_PROFILE_ID,                                    \
                              0,                                                      \
                              NULL,                                                   \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                                           \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,       \
                              ZB_ERL_INTERFACE_DEV_REPORT_ATTR_COUNT,                 \
                              reporting_info##device_ctx_name,                        \
                              0, NULL)

#define ZB_DECLARE_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_CLUSTER_LIST( \
  cluster_list_name,                                                    \
  basic_attr_list,                                                      \
  identify_attr_list,                                                   \
  metering_attr_list)                                                   \
  zb_zcl_cluster_desc_t cluster_list_name[] =                           \
  {                                                                     \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
      (basic_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
      (identify_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_METERING,                                       \
      ZB_ZCL_ARRAY_SIZE(metering_attr_list, zb_zcl_attr_t),             \
      (metering_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
  }

ZB_DECLARE_SIMPLE_DESC(3, 1);

#define ZB_ZCL_DECLARE_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_SIMPLE_DESC( \
  ep_name,                                                              \
  ep_id,                                                                \
  in_clust_num,                                                         \
  out_clust_num)                                                        \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                     \
    ep_id,                                                              \
    ZB_AF_HA_PROFILE_ID,                                                \
    ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEVICE_ID,                  \
    ZB_DEVICE_VER_ERL_INTERFACE,                                        \
    0,                                                                  \
    in_clust_num,                                                       \
    out_clust_num,                                                      \
    {                                                                   \
      /* Server clusters */                                             \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_CLUSTER_ID_METERING,                                       \
      /* Client clusters */                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
    }                                                                   \
  }

#define ZB_DECLARE_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_SIMPLE_DESC(                       \
    ep_name,                                                                                 \
    ep_id,                                                                                   \
    ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_IN_CLUSTER_NUM,                              \
    ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_OUT_CLUSTER_NUM);                            \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                                                        \
    reporting_info##device_ctx_name,                                                         \
    ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_REPORT_ATTR_COUNT);                          \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                       \
                              ep_id,                                                         \
                              ZB_AF_HA_PROFILE_ID,                                           \
                              0,                                                             \
                              NULL,                                                          \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),        \
                              cluster_list,                                                  \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,              \
                              ZB_ERL_INTERFACE_PHASE_METERING_ELEMENT_DEV_REPORT_ATTR_COUNT, \
                              reporting_info##device_ctx_name,                               \
                              0, NULL)

/** @def ZB_DECLARE_ERL_INTERFACE_DEV_CTX
 *  @brief Declare ERL device context.
 *  @param device_ctx - device context variable name.
 *  @param ep_name - endpoint variable name.
 */
#define ZB_DECLARE_ERL_INTERFACE_DEV_CTX(device_ctx_name, ep1_name, ep2_name, ep3_name, ep4_name) \
  ZBOSS_DECLARE_DEVICE_CTX_4_EP(device_ctx_name, ep1_name, ep2_name, ep3_name, ep4_name)

/** @} */

/** @endcond */ /* DOXYGEN_SECTION */

#endif /* ZB_ERL_INTERFACE_H */
