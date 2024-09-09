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
/* PURPOSE: ZR Thermostat sample for HA profile
*/

#ifndef _THERMOSTAT_ZC_H_
#define _THERMOSTAT_ZC_H_

#include "zboss_api.h"

/* Thermostat used channel */
#define ZB_THERMOSTAT_CHANNEL_MASK (1l << 21)
/* Used endpoint */
#define SRC_ENDPOINT  5

/* Handler for specific zcl commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);
/* Device user application callback */
void thermostat_device_interface_cb(zb_uint8_t param);

zb_uint8_t test_device_scenes_get_entry(zb_uint16_t group_id, zb_uint8_t scene_id);
void test_device_scenes_remove_entries_by_group(zb_uint16_t group_id);
void test_device_scenes_table_init();

typedef struct test_device_scenes_table_entry_s
{
  zb_zcl_scene_table_record_fixed_t common;
  zb_int16_t occupied_cooling_setpoint;
  zb_int16_t occupied_heating_setpoint;
  zb_uint8_t system_mode;
}
test_device_scenes_table_entry_t;

typedef struct resp_info_s
{
  zb_zcl_parsed_hdr_t cmd_info;
  zb_zcl_scenes_view_scene_req_t view_scene_req;
  zb_zcl_scenes_get_scene_membership_req_t get_scene_membership_req;
} resp_info_t;

#define TEST_DEVICE_SCENES_TABLE_SIZE 3


#endif /* #ifndef _THERMOSTAT_ZC_H_ */
