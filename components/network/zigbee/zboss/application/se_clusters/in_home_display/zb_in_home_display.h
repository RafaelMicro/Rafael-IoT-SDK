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
/* PURPOSE: Smart Energy in-home display device definition
*/

#ifndef ZB_IH_HOME_DISPLAY_H
#define ZB_IH_HOME_DISPLAY_H 1

#include "zcl/zb_zcl_calendar.h"
#include "zcl/zb_zcl_identify.h"
#include "zcl/zb_zcl_el_measurement.h"
#include "zcl/zb_zcl_meter_identification.h"
#include "zcl/zb_zcl_price.h"

/* #if defined ZB_DEFINE_DEVICE_IN_HOME_DISPLAY || defined DOXYGEN */

/** @cond DOXYGEN_SECTION */

/**
 *  @defgroup in_home_display In-home Display
 *  @ingroup ZB_ERL_SAMPLES
 *  @details The In-Home Display device will relay energy consumption data to the user by way of a
 *  graphical or text display. The display may or may not be an interactive device. At a minimum at
 *  least one of the following should be displayed: current energy usage, a history over
 *  selectable periods, pricing information, or text messages. As an interactive device, it can be
 *  used for returning simple messages for interpretation by the recipient (e.g. “Button A was
 *  pressed). The display may also show critical pricing information to advise the customer when
 *  peaks are due to occur so that they can take appropriate action.
 *
 *  @par Supported clusters
 *  In-home Display Device has X clusters: \n
 *  <table>
 *    <tr>
 *      <th>Server roles</th> <th>Client roles</th>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_BASIC </td> <td>@ref ZB_ZCL_METERING </td>
 *    </tr>
 *    <tr>
 *      <td>@ref ZB_ZCL_IDENTIFY</td> <td>@ref ZB_ZCL_CALENDAR </td>
 *    </tr>
 *    <tr>
 *      <td></td> <td>@ref ZB_ZCL_PRICE </td>
 *    </tr>
 *    <tr>
 *      <td></td> <td>@ref ZB_ZCL_IDENTIFY </td>
 *    </tr>
 *    <tr>
 *      <td></td> <td>@ref ZB_ZCL_MESSAGING </td>
 *    </tr>
 *  </table>
 *
 *  @par Example
 *  @n Declare clusters
 *  @snippet ihd_zr.c DECLARE_CLUSTERS
 *  @n Register device context
 *  @snippet ihd_zr.c REGISTER_DEVICE_CTX
 *  @n Command handler example
 *  @snippet ihd_zr.c COMMAND_HANDLER
 *  @n Signal handler example
 *  @snippet ihd_zr.c SIGNAL_HANDLER
 *  @{
 */

#define ZB_IN_HOME_DISPLAY_DEVICE_ID 0x0502

/** @def ZB_DEVICE_VER_IN_HOME_DISPLAY
 *  @brief In-Home Display device version
 */
#define ZB_DEVICE_VER_IN_HOME_DISPLAY      0

/** @cond internal */

/** @def ZB_IN_HOME_DISPLAY_IN_CLUSTER_NUM
 *  @brief In-Home Display IN clusters number
 */
#define ZB_IN_HOME_DISPLAY_IN_CLUSTER_NUM  2


/** @def ZB_IN_HOME_DISPLAY_OUT_CLUSTER_NUM
 *  @brief In-Home Display OUT clusters number
 */
/* 2017/08/22 NK:MAJOR Why 9? Found only 7 client clusters below. */
#define ZB_IN_HOME_DISPLAY_OUT_CLUSTER_NUM 5


/** @def ZB_IN_HOME_DISPLAY_CLUSTER_NUM
 *  @brief Total number of clusters for In-Home Display SE device
 */
#define ZB_IN_HOME_DISPLAY_CLUSTER_NUM \
  (ZB_IN_HOME_DISPLAY_IN_CLUSTER_NUM + ZB_IN_HOME_DISPLAY_OUT_CLUSTER_NUM)


/** @def ZB_IN_HOME_DISPLAY_REPORT_ATTR_COUNT
 *  @brief Number of attributes for reporting on In-Home Display device
 */
#define ZB_IN_HOME_DISPLAY_REPORT_ATTR_COUNT 0

/** @endcond */


/** @def ZB_DECLARE_IN_HOME_DISPLAY_CLUSTER_LIST
 *  @brief Declare cluster list for In-Home Display device.
 *  @param cluster_list_name - cluster list variable name
 *  @param basic_attr_list - attribute list for Basic cluster
 */
#define ZB_DECLARE_IN_HOME_DISPLAY_CLUSTER_LIST(cluster_list_name, \
                                                basic_attr_list,   \
                                                identify_attr_list) \
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
        ZB_ZCL_CLUSTER_ID_IDENTIFY,                                     \
        ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),           \
        (identify_attr_list),                                           \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
        ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                  \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_CALENDAR,                                     \
      0, NULL,                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_METERING,                                     \
      0, NULL,                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_PRICE,                                        \
      0, NULL,                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_MESSAGING,                                    \
      0, NULL,                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                     \
      0,                                                              \
      NULL,                                                           \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
  }

/** @cond internals_doc */

/** @def ZB_ZCL_DECLARE_IN_HOME_DISPLAY_SIMPLE_DESC
 *  @brief Declare simple descriptor for In-Home Display device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param in_clust_num   - number of supported input clusters
 *  @param out_clust_num  - number of supported output clusters
 *
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name
 */
#define ZB_ZCL_DECLARE_IN_HOME_DISPLAY_SIMPLE_DESC(ep_name,                    \
                                                   ep_id,                      \
                                                   in_clust_num,               \
                                                   out_clust_num)              \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                            \
    ep_id,                                                                     \
    ZB_AF_HA_PROFILE_ID,                                                       \
    ZB_IN_HOME_DISPLAY_DEVICE_ID,                                           \
    ZB_DEVICE_VER_IN_HOME_DISPLAY,                                          \
    0,                                                                         \
    in_clust_num,                                                              \
    out_clust_num,                                                             \
    {                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                          \
      ZB_ZCL_CLUSTER_ID_CALENDAR,                                              \
      ZB_ZCL_CLUSTER_ID_METERING,                                              \
      ZB_ZCL_CLUSTER_ID_PRICE,                                                 \
      ZB_ZCL_CLUSTER_ID_MESSAGING,                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                              \
    }                                                                          \
  }


/** @endcond */

/** @def ZB_DECLARE_IN_HOME_DISPLAY_EP
 *  @brief Declare endpoint for In-Home Display device
 *  @param ep_name - endpoint variable name
 *  @param ep_id - endpoint ID
 *  @param cluster_list - endpoint cluster list
 */
#define ZB_DECLARE_IN_HOME_DISPLAY_EP(ep_name, ep_id, cluster_list)                  \
  ZB_ZCL_DECLARE_IN_HOME_DISPLAY_SIMPLE_DESC(ep_name,                                   \
                                             ep_id,                                     \
                                             ZB_IN_HOME_DISPLAY_IN_CLUSTER_NUM,      \
                                             ZB_IN_HOME_DISPLAY_OUT_CLUSTER_NUM);    \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                  \
                              ep_id,                                    \
                          ZB_AF_HA_PROFILE_ID,                                          \
                          0,                                                            \
                          NULL,                                                         \
                          ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),       \
                          cluster_list,                                                 \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name, \
                              ZB_IN_HOME_DISPLAY_REPORT_ATTR_COUNT, NULL, \
                           0, NULL)

/** @def ZB_DECLARE_IN_HOME_DISPLAY_CTX
 *  @brief Declares In-Home Display device context.
 *  @param device_ctx - device context variable name.
 *  @param ep_name - endpoint variable name.
 */
#define ZB_DECLARE_IN_HOME_DISPLAY_CTX(device_ctx_name, ep_name)     \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx_name, ep_name)

/** @} */

/** @endcond */ /* DOXYGEN_SECTION */

/* #endif /\* ZB_DEFINE_DEVICE_IN_HOME_DISPLAY *\/ */

#endif /* ZB_IH_HOME_DISPLAY_H */
