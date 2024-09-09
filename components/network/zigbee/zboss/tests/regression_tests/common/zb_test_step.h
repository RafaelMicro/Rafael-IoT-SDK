/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
/* PURPOSE:
*/

#ifndef _ZB_TEST_STEP
#define _ZB_TEST_STEP

/* Test step storage */

#define ZB_TEST_STEP_MAX_COUNT 30

typedef struct zb_test_step_s
{
    zb_callback_t callback;
    zb_uint8_t param;
    zb_uint32_t step_time;
} zb_test_step_info_t;

/**
 * Test step registration. Must be called for each test step before the test_control
 * state machine is started.
 *
 * Steps are numerated by consecutive integers starting with 0.
 *
 * @param cb - a callback that implements the test step
 * @param param - parameter that must be passed to the callback
 * @param step_time - time (in beacon intervals) that the step takes to complete
 */
void test_step_register(zb_callback_t cb, zb_uint8_t param, zb_uint32_t step_time);

/**
 * Returns the number of added steps that have been registered.
 * @return The step count.
 */
zb_uint8_t test_step_count();

/**
 * Get test step details by the step number.
 * @param step_no
 * @return An pointer to the corresponding zb_test_step_info_t structure
 */
zb_test_step_info_t *test_step_get_info(zb_uint8_t step_no);


/* State machine */

typedef enum zb_test_control_mode_e
{
    ZB_TEST_CONTROL_ALARMS = 0,
    ZB_TEST_CONTROL_BUTTONS,
    ZB_TEST_CONTROL_UNUSED
} zb_test_control_mode_t;

/**
 * Schedules next test step.
 * @return RET_OK - successfully handled
 * @return RET_ERROR - no more test steps available
 */
zb_ret_t test_control_force_next();

/**
 * Schedules test step by the step index.
 * @param idx - step number starting with 0
 * @return RET_OK - successfully handled
 * @return RET_ERROR - no more test steps available
 */
zb_ret_t test_control_force_step(zb_uint8_t idx);

/**
 * Setts action which will be performed before step execution. (Used only for buttons now)
 * @param callback - a callback that implements action
 * @return nothing
 */
void test_control_set_action_on_steps(zb_callback_t callback);

/**
 * Starts test. Expected to be called from ZDO_STARTUP_COMPLETE
 * @param method - one of the methods from zb_test_control_mode_t
 * @param timeout - timeout for the first test step
 * @return nothing
 */
void test_control_start(zb_test_control_mode_t method, zb_uint32_t timeout);

#endif /* _ZB_TEST_STEP */
