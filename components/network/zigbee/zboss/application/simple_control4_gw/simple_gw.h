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
/* PURPOSE: Header file for Simple GW
*/

#ifndef SIMPLE_GW_H
#define SIMPLE_GW_H 1

#include "zboss_api.h"
#include "zb_led_button.h"

/* CONFIG SECTION */
#define IAS_CIE_ENABLED 1

#if defined IAS_CIE_ENABLED
#include "ias_cie_addon.h"
#endif

/*  */
#define SIMPLE_GW_ENDPOINT 1

#undef ZB_USE_BUTTONS

#define SIMPLE_GW_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define SIMPLE_GW_CHANNEL_MASK (1l<<21)

#define MATCH_DESC_REQ_TIMEOUT (10*ZB_TIME_ONE_SECOND)

#define SIMPLE_GW_INVALID_DEV_INDEX 0xff

#define SIMPLE_GW_DEV_NUMBER 20

#define SIMPLE_GW_TOGGLE_ITER_TIMEOUT (5*ZB_TIME_ONE_SECOND)
#define SIMPLE_GW_RANDOM_TIMEOUT_VAL (15)
#define SIMPLE_GW_COMMUNICATION_PROBLEMS_TIMEOUT (30) /* 300 sec */
#define SIMPLE_GW_TOGGLE_TIMEOUT (ZB_RANDOM_VALUE(SIMPLE_GW_RANDOM_TIMEOUT_VAL)) /* * ZB_TIME_ONE_SECOND */

#define SIMPLE_GW_REPORTING_MIN_INTERVAL 30
#define SIMPLE_GW_REPORTING_MAX_INTERVAL(dev_idx) ((90 * ((dev_idx) + 1)))

enum simple_gw_device_state_e
{
  NO_DEVICE,
  MATCH_DESC_DISCOVERY,
  IEEE_ADDR_DISCOVERY,
  CONFIGURE_BINDING,
  CONFIGURE_REPORTING,
  COMPLETED,
  COMPLETED_NO_TOGGLE,
#if defined IAS_CIE_ENABLED
  WRITE_CIE_ADDR,
#endif
};

typedef ZB_PACKED_PRE struct simple_gw_device_params_s
{
  zb_uint8_t dev_state;
  zb_uint8_t endpoint;
  zb_uint16_t short_addr;
  zb_ieee_addr_t ieee_addr;
  zb_uint8_t pending_toggle;
} simple_gw_device_params_t;

typedef ZB_PACKED_PRE struct simple_gw_device_ctx_s
{
  simple_gw_device_params_t devices[SIMPLE_GW_DEV_NUMBER];
} ZB_PACKED_STRUCT simple_gw_device_ctx_t;

typedef simple_gw_device_ctx_t simple_gw_device_nvram_dataset_t;

void find_onoff_device(zb_uint8_t param, zb_uint16_t dev_idx);
void find_onoff_device_delayed(zb_uint8_t param);
void find_onoff_device_cb(zb_uint8_t param);
zb_uint16_t simple_gw_associate_cb(zb_uint16_t short_addr);
void simple_gw_dev_annce_cb(zb_uint16_t short_addr);
void simple_gw_leave_indication(zb_ieee_addr_t dev_addr);

void device_ieee_addr_req(zb_uint8_t param, zb_uint16_t dev_idx);
void device_ieee_addr_req_cb(zb_uint8_t param);

void bind_device(zb_uint8_t param, zb_uint16_t dev_idx);
void bind_device_cb(zb_uint8_t param);

void configure_reporting(zb_uint8_t param, zb_uint16_t dev_idx);
void configure_reporting_cb(zb_uint8_t param);

zb_uint16_t simple_gw_get_nvram_data_size();
void simple_gw_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length);
zb_ret_t simple_gw_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos);

/* Handler for specific zcl commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);

zb_uint8_t simple_gw_get_dev_index_by_state(zb_uint8_t dev_state);
zb_uint8_t simple_gw_get_dev_index_by_short_addr(zb_uint16_t short_addr);
zb_uint8_t simple_gw_get_dev_index_by_ieee_addr(zb_ieee_addr_t ieee_addr);

void find_onoff_device_tmo(zb_uint8_t param);


/*************************************************************************/

#ifdef GW_CONTROL4_COMPATIBLE
/* Control4 Network Cluster attributes */
typedef struct simple_control4_gw_attr_s
{
  zb_uint16_t access_point_node_ID;
  zb_ieee_addr_t access_point_long_ID;
  zb_uint8_t  access_point_cost;
}
simple_control4_gw_attr_t;

/** @brief Declare attribute list for C4 Network Controller cluster
    @param attr_list - attribute list name
    @param access_point_node_ID - pointer to variable to store Access Point Node ID attribute value
    @param access_point_long_ID - pointer to variable to store Access Point Long ID attribute value
    @param access_point_cost - pointer to variable to store Access Point Cost ID attribute value
*/
#define ZB_ZCL_DECLARE_CONTROL4_NETWORKING_CONTROLLER_ATTRIB_LIST(attr_list, access_point_node_ID,        \
  access_point_long_ID, access_point_cost)                                                                \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                           \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_ID, (access_point_node_ID)) \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_ID, (access_point_long_ID)) \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_COST_ID, (access_point_cost))       \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** Macro for sending Control4 immediate announce command */
#define ZB_ZCL_C4_IMMEDIATE_ANNOUNCE_REQ(                                 \
  buffer, addr, dst_addr_mode, dst_ep, dis_default_resp, cb)              \
    ZB_ZCL_SEND_CMD(buffer,                                               \
                    addr,                                                 \
                    dst_addr_mode,                                        \
                    dst_ep,                                               \
                    ZB_CONTROL4_NETWORK_ENDPOINT,                         \
                    ZB_AF_CONTROL4_PROFILE_ID,                            \
                    dis_default_resp,                                     \
                    ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING,                \
                    ZB_ZCL_CMD_CONTROL4_NETWORKING_IMMEDIATE_ANNOUNCE_ID, \
                    cb                                                    \
    );

#endif /* GW_CONTROL4_COMPATIBLE */

#endif /* SIMPLE_GW_H */
