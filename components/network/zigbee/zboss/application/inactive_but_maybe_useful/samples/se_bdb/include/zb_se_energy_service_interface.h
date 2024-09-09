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
/* PURPOSE: Smart Energy - Energy Service Interface (ESI) device definition
*/

#ifndef ZB_SE_ENERGY_SERVICE_INTERFACE_H
#define ZB_SE_ENERGY_SERVICE_INTERFACE_H 1

#include "zboss_api.h"

#if defined ZB_SE_DEFINE_DEVICE_ENERGY_SERVICE_INTERFACE || defined DOXYGEN

/** @cond DOXYGEN_SE_SECTION */

/**
 *  @defgroup se_esi Energy Service Interface (ESI)
 *  @ingroup se_devices
 *  @details The Energy Service Interface connects the energy supply company communication
 *  network to the metering and energy management devices within the home. It routes messages
 *  to and from the relevant end points. It may be installed within a meter, thermostat, or In-
 *  Home Display, or may be a standalone device, and it will contain another non-Zigbee
 *  communication module (e.g. power-line carrier, RF, GPRS, broadband Internet connection).
 *
 *  @par Supported clusters
 *  ESI Device has X clusters (see SE spec 1.4 subclauses 6.1, 6.3.2.1): \n
 *  <table>
 *    <tr>
 *      <th>Server roles</th> <th>Client roles</th>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_BASIC</td> <td>@ref ZB_ZCL_KEC</td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_KEC</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_MESSAGING</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_PRICE</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_DRLC</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_TIME</td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_KEEP_ALIVE <i>(optional)</i></td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_CALENDAR <i>(optional)</i></td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_TUNNELING <i>(optional)</i></td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_MDU_PAIRING <i>(optional)</i></td> <td></td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_SUBGHZ <i>(optional)</i></td> <td></td>
 *    </tr>
 *  </table>
 *  @par Example
 *  @n Declare clusters
 *  @snippet DOXYGEN_SNIPPETS.dox DECLARE_CLUSTERS_samples_se_energy_service_interface_se_esi_zc_c
 *  @n Register device context
 *  @snippet DOXYGEN_SNIPPETS.dox REGISTER_DEVICE_CTX_samples_se_energy_service_interface_se_esi_zc_c
 *  @n Signal handler example
 *  @snippet DOXYGEN_SNIPPETS.dox SIGNAL_HANDLER_samples_se_energy_service_interface_se_esi_zc_c
 *  @{
 */

/** @def ZB_SE_DEVICE_VER_ENERGY_SERVICE_INTERFACE
 *  @brief ESI device version
 */
#define ZB_SE_DEVICE_VER_ENERGY_SERVICE_INTERFACE          0
#define ZB_BDB_DEVICE_VER_ENERGY_SERVICE_INTERFACE         1

/** @cond internal */

/** @def ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM
 *  @brief ESI IN clusters number
 */
#define ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM  11

/** @def ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM
 *  @brief ESI OUT clusters number
 */
#define ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM 3

/** @def ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_CLUSTER_NUM
 *  @brief Number of clusters for ESI SE device
 */
#define ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_CLUSTER_NUM \
  (ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM + ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM)

#define ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM  3
#define ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM 0
#define ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_CLUSTER_NUM \
  (ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM + ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM)


/** @def ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT
 *  @brief Number of attributes for reporting on ESI device
 */
#define ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT (ZB_ZCL_METERING_REPORT_ATTR_COUNT)
#define ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT (ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT)


#ifdef ZB_SUBGHZ_BAND_ENABLED

#define ZB_ZCL_SUBGHZ_CLUSTER_DESC_SERVER(attr_list) \
  ZB_ZCL_CLUSTER_DESC(                               \
    ZB_ZCL_CLUSTER_ID_SUB_GHZ,                       \
    ZB_ZCL_ARRAY_SIZE(attr_list, zb_zcl_attr_t),     \
    (attr_list),                                     \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                      \
    ZB_ZCL_MANUF_CODE_INVALID),

#define SUB_GHZ_CLUSTER_ENABLED_ID ZB_ZCL_CLUSTER_ID_SUB_GHZ,

#undef ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM
#define ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM  12

#else
#define SUB_GHZ_CLUSTER_ENABLED_ID
#define ZB_ZCL_SUBGHZ_CLUSTER_DESC_SERVER(attr_list)
#endif

/** @endcond */ /* internal */

/** @def ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_CLUSTER_LIST
 *  @brief Declare cluster list for ESI device.
 *  @param cluster_list_name - cluster list variable name
 *  @param basic_attr_list - attribute list for Basic cluster
 *  @param kec_attr_list - attribute liste for Key Establishment cluster
 *  @param price_attr_list - attribute list for Price cluster
 *  @param time_attr_list - attribute list for Time cluster
 *  @param keep_alive_attr_list - attribute list for Keep-Alive cluster
 *  @param tunneling_attr_list - attribute list for Tunneling cluster
 *  @param sub_ghz_attr_list - attribute list for Sub-GHz cluster
 */
#define ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_CLUSTER_LIST(cluster_list_name,    \
                                                                basic_attr_list,      \
                                                                kec_attr_list,        \
                                                                price_attr_list,      \
                                                                time_attr_list,       \
                                                                keep_alive_attr_list, \
                                                                tunneling_attr_list,  \
                                                                sub_ghz_attr_list,    \
                                                                metering_attr_list)   \
  zb_zcl_cluster_desc_t cluster_list_name[] =                                         \
  {                                                                                   \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                              \
      (basic_attr_list),                                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_CALENDAR,                                                     \
      0,                                                                              \
      NULL,                                                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_PRICE,                                                        \
      ZB_ZCL_ARRAY_SIZE(price_attr_list, zb_zcl_attr_t),                              \
      (price_attr_list),                                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_TIME,                                                         \
      ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),                               \
      (time_attr_list),                                                               \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                                   \
      ZB_ZCL_ARRAY_SIZE(keep_alive_attr_list, zb_zcl_attr_t),                         \
      (keep_alive_attr_list),                                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_DRLC,                                                         \
      0,                                                                              \
      NULL,                                                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_TUNNELING,                                                    \
      ZB_ZCL_ARRAY_SIZE(tunneling_attr_list, zb_zcl_attr_t),                          \
      (tunneling_attr_list),                                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_MESSAGING,                                                    \
      0,                                                                              \
      NULL,                                                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_MDU_PAIRING,                                                  \
      0,                                                                              \
      NULL,                                                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
    ),                                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_METERING,                                                     \
      ZB_ZCL_ARRAY_SIZE(metering_attr_list, zb_zcl_attr_t),                           \
      (metering_attr_list),                                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
    ),                                                                                \
    ZB_ZCL_SUBGHZ_CLUSTER_DESC_SERVER(sub_ghz_attr_list)                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                                \
      (kec_attr_list),                                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                                \
      (kec_attr_list),                                                                \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                                   \
      0, NULL,                                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_TIME,                                                         \
      0, NULL,                                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
  }

#define ZB_BDB_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_CLUSTER_LIST(cluster_list_name,  \
                                                                 basic_attr_list, \
                                                                 identify_attr_list, \
                                                                 on_off_attr_list) \
  zb_zcl_cluster_desc_t cluster_list_name[] =                         \
  {                                                                   \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_BASIC,                                        \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),              \
      (basic_attr_list),                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),              \
      (identify_attr_list),                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
    ),                                                                  \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                     \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),              \
      (on_off_attr_list),                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
  }

/** @cond internals_doc */

/** @def ZB_ZCL_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_SIMPLE_DESC
 *  @brief Declare simple descriptor for ESI device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param in_clust_num   - number of supported input clusters
 *  @param out_clust_num  - number of supported output clusters
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name
 */
#define ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_SIMPLE_DESC(ep_name,       \
                                                ep_id,                         \
                                                in_clust_num,                  \
                                                out_clust_num, sub_ghz_token)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                            \
    ep_id,                                                                     \
    ZB_AF_HA_PROFILE_ID,                                                       \
    ZB_SE_ENERGY_SERVICE_INTERFACE_DEVICE_ID,                                  \
    ZB_SE_DEVICE_VER_ENERGY_SERVICE_INTERFACE,                                 \
    0,                                                                         \
    in_clust_num,                                                              \
    out_clust_num,                                                             \
    {                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
      ZB_ZCL_CLUSTER_ID_CALENDAR,                                              \
      ZB_ZCL_CLUSTER_ID_PRICE,                                                 \
      ZB_ZCL_CLUSTER_ID_TIME,                                                  \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                            \
      ZB_ZCL_CLUSTER_ID_DRLC,                                                  \
      ZB_ZCL_CLUSTER_ID_TUNNELING,                                             \
      ZB_ZCL_CLUSTER_ID_MESSAGING,                                             \
      ZB_ZCL_CLUSTER_ID_MDU_PAIRING,                                           \
      ZB_ZCL_CLUSTER_ID_METERING,                                              \
      sub_ghz_token                                                            \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                     \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                     \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                            \
      ZB_ZCL_CLUSTER_ID_TIME                                                   \
    }                                                                          \
  }

#define ZB_BDB_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_SIMPLE_DESC(ep_name,       \
                                                ep_id,                         \
                                                in_clust_num,                  \
                                                out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                            \
    ep_id,                                                                     \
    ZB_AF_HA_PROFILE_ID,                                                       \
    ZB_SE_ENERGY_SERVICE_INTERFACE_DEVICE_ID,                                  \
    ZB_BDB_DEVICE_VER_ENERGY_SERVICE_INTERFACE,                                 \
    0,                                                                         \
    in_clust_num,                                                              \
    out_clust_num,                                                             \
    {                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                           \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                       \
    }                                                                          \
  }

/** @endcond */

/** @def ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_EP
 *  @brief Declare endpoint for ESI device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param cluster_list - endpoint cluster list
 */
#define ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_EP(ep_name, ep_id, cluster_list)           \
  ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_SIMPLE_DESC(ep_name,                            \
                                          ep_id,                                              \
                                          ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM,  \
                                          ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM, \
                                          SUB_GHZ_CLUSTER_ENABLED_ID);                        \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(se_reporting_info##device_ctx_name,                      \
                                     ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT);   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                        \
                              ep_id,                                                          \
                              ZB_AF_HA_PROFILE_ID,                                            \
                              0,                                                              \
                              NULL,                                                           \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),         \
                              cluster_list,                                                   \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,               \
                              ZB_SE_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT,           \
                              se_reporting_info##device_ctx_name,                             \
                              0, NULL)

#define ZB_BDB_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_EP(ep_name, ep_id, cluster_list)           \
  ZB_BDB_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_SIMPLE_DESC(ep_name,                            \
                                          ep_id,                                              \
                                          ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_IN_CLUSTER_NUM,  \
                                          ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_OUT_CLUSTER_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(bdb_reporting_info##device_ctx_name,                         \
                                     ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT);   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                        \
                              ep_id,                                                          \
                              ZB_AF_HA_PROFILE_ID,                                            \
                              0,                                                              \
                              NULL,                                                           \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),         \
                              cluster_list,                                                   \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,               \
                              ZB_BDB_ENERGY_SERVICE_INTERFACE_DEV_REPORT_ATTR_COUNT,           \
                              bdb_reporting_info##device_ctx_name,                                \
                              0, NULL)

/** @def ZB_SE_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_CTX
 *  @brief Declare ESI device context.
 *  @param device_ctx_name - device context variable name.
 *  @param ep_name - endpoint variable name.
 */
#define ZB_DECLARE_ENERGY_SERVICE_INTERFACE_DEV_CTX(device_ctx_name, ep1_name, ep2_name) \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx_name, ep1_name, ep2_name)

/** @} */

/** @endcond */ /* DOXYGEN_SE_SECTION */

#endif /* ZB_SE_DEFINE_DEVICE_ENERGY_SERVICE_INTERFACE */

#endif /* ZB_SE_ESI_H */
