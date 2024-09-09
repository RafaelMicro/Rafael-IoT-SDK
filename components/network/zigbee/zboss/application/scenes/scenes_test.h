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
/* PURPOSE: Common definitions for testing Scenes (Server) for HA profile
*/

#ifndef SCENES_TEST_H
#define SCENES_TEST_H 1

/** @brief Timeout in seconds imitating test step 4b */
#define STEP_4B_IMITATION_TIMEOUT 5

#define TEST_GROUP_ID 0x0001
#define TEST_SCENE_ID_1 0x01
#define TEST_SCENE_ID_2 0x02
#define TEST_TRANSITION_TIME_1 0x0010
#define TEST_TRANSITION_TIME_2 0x0020

/**
 *  @brief Declare attribute list for On/Off cluster (extended attribute set).
 *  @param attr_list [IN] - attribute list name being declared by this macro.
 *  @param on_off [IN] - pointer to a boolean variable storing on/off attribute value.
 *  @param global_scene_ctrl [IN] - pointer to a boolean variable storing global scene control attribute value.
 *  @param on_time [IN] - pointer to a unsigned 16-bit integer variable storing on time attribute value.
 *  @param off_wait_time [IN] - pointer to a unsigned 16-bit integer variable storing off wait time attribute value.
 *  @param start_up_on_off [IN] - pointer to a 8-bit enum variable storing start up on off attribute value.
 */
#define ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT_WITH_START_UP_ON_OFF(                             \
    attr_list, on_off, global_scene_ctrl, on_time, off_wait_time, start_up_on_off               \
    )                                                                                           \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, (on_off))                                \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL, (global_scene_ctrl))          \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_TIME, (on_time))                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME, (off_wait_time))                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF, (start_up_on_off))                 \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#endif /* SCENES_TEST_H */
