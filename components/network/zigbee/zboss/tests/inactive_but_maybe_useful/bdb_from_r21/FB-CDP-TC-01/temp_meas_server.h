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
/* PURPOSE: Temperature Measurement server device definition for test
*/

#ifndef TEMP_MEAS_SERVER_H
#define TEMP_MEAS_SERVER_H 1

/******************* Specific definitions for Temperature server device **************************/

#define DEVICE_ID_TEMP_MEAS_SERVER   0x7e01
#define DEVICE_VER_TEMP_MEAS_SERVER  0  /*!< device version */

#define TEMP_MEAS_SERVER_IN_CLUSTER_NUM 1 /*!< IN (server) clusters number */
#define TEMP_MEAS_SERVER_OUT_CLUSTER_NUM 0 /*!<  OUT (client) clusters number */

#define TEMP_MEAS_SERVER_CLUSTER_NUM   (TEMP_MEAS_SERVER_IN_CLUSTER_NUM + \
                                        TEMP_MEAS_SERVER_OUT_CLUSTER_NUM)

/*!
  @brief Declare cluster list for Temperature Measurement server
  @param cluster_list_name - cluster list variable name
  @param temp_attr_list - attribute list for Temperature Measurements cluster
 */
#define DECLARE_TEMP_MEAS_SERVER_CLUSTER_LIST(              \
  cluster_list_name,                                        \
  temp_attr_list)                                           \
  zb_zcl_cluster_desc_t cluster_list_name[] =               \
  {                                                         \
    ZB_ZCL_CLUSTER_DESC(                                    \
      ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                   \
      ZB_ZCL_ARRAY_SIZE(temp_attr_list, zb_zcl_attr_t),     \
      (temp_attr_list),                                     \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
      ZB_ZCL_MANUF_CODE_INVALID                             \
    )                                                       \
  }

#endif /* TEMP_MEAS_SERVER_H */
