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
/* PURPOSE: Header file for APS Unencrypted transport key tests router
*/

#ifndef APS_UNENCRYPTED_TKEY_TESTS_ZR_H
#define APS_UNENCRYPTED_TKEY_TESTS_ZR_H 1

#include "zboss_api.h"

#define APS_UNENCRYPTED_TKEY_TESTS_ZR_CHANNEL_MASK (1l<<17)

#define ZR1_IEEE_ADDRESS {0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb}

void bulb_app_ctx_init();

typedef enum aps_unencrypted_tkey_tests_button_state_s
{
  APS_UNENCRYPTED_TKEY_TESTS_ZR_BUTTON_STATE_IDLE,
  APS_UNENCRYPTED_TKEY_TESTS_ZR_BUTTON_STATE_PRESSED,
  APS_UNENCRYPTED_TKEY_TESTS_ZR_BUTTON_STATE_UNPRESSED
} aps_unencrypted_tkey_tests_button_state_t;

#define APS_UNENCRYPTED_TKEY_TESTS_ZR_BUTTON_DEBOUNCE_PERIOD ZB_MILLISECONDS_TO_BEACON_INTERVAL(50)

typedef struct aps_unencrypted_tkey_tests_zr_button_s
{
  aps_unencrypted_tkey_tests_button_state_t button_state;
  zb_time_t timestamp;
} aps_unencrypted_tkey_tests_zr_button_t;

typedef struct aps_unencrypted_tkey_tests_zr_ctx_s
{
  aps_unencrypted_tkey_tests_zr_button_t button_leave_net;
}
aps_unencrypted_tkey_tests_zr_ctx_t;

#endif /* APS_UNENCRYPTED_TKEY_TESTS_ZR_H */
