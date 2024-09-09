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
/* PURPOSE: Header file for APS Unencrypted Transport Key feature coordinator
*/

#ifndef APS_UNENCRYPTED_TKEY_TESTS_ZC_H
#define APS_UNENCRYPTED_TKEY_TESTS_ZC_H 1

#include "zboss_api.h"

#define APS_UNENCRYPTED_TKEY_TESTS_CHANNEL_MASK (1l<<17)

/* IEEE address of ZC */
#define APS_UNENCRYPTED_TKEY_TESTS_ZC_ADDRESS {0xab, 0xbc, 0xcd, 0xde, 0xef, 0x66, 0x66, 0x66}

enum simple_dev_type_e
{
  SIMPLE_DEV_TYPE_UNUSED,
  SIMPLE_DEV_TYPE_APS_UNENCRYPTED_TKEY,
  SIMPLE_DEV_TYPE_APS_ENCRYPTED_TKEY,
};

typedef struct device_s
{
  zb_uint8_t     dev_type;
  zb_uint16_t    nwk_addr;
  zb_ieee_addr_t ieee_addr;
  zb_uint8_t     led_num;
} device_t;

#define APS_UNENCRYPTED_TKEY_TESTS_DEVICES 2

typedef enum aps_unencrypted_tkey_tests_button_state_s
{
  APS_UNENCRYPTED_TKEY_TESTS_ZC_BUTTON_STATE_IDLE,
  APS_UNENCRYPTED_TKEY_TESTS_ZC_BUTTON_STATE_PRESSED,
  APS_UNENCRYPTED_TKEY_TESTS_ZC_BUTTON_STATE_UNPRESSED
} aps_unencrypted_tkey_tests_button_state_t;

#define APS_UNENCRYPTED_TKEY_TESTS_ZC_BUTTON_DEBOUNCE_PERIOD ZB_MILLISECONDS_TO_BEACON_INTERVAL(50)

typedef struct aps_unencrypted_tkey_tests_zc_button_s
{
  aps_unencrypted_tkey_tests_button_state_t button_state;
  zb_time_t timestamp;
} aps_unencrypted_tkey_tests_zc_button_t;

typedef struct aps_unencrypted_tkey_tests_zc_ctx_s
{
  device_t devices[APS_UNENCRYPTED_TKEY_TESTS_DEVICES];
  aps_unencrypted_tkey_tests_zc_button_t button_aps_unencrypted_tkey;
  aps_unencrypted_tkey_tests_zc_button_t button_zdo_leave;
  aps_unencrypted_tkey_tests_zc_button_t button_first_device;
  aps_unencrypted_tkey_tests_zc_button_t button_second_device;
} aps_unencrypted_tkey_tests_zc_ctx_t;


#ifdef ZB_USE_NVRAM
typedef ZB_PACKED_PRE struct application_dataset_s
{
  device_t devices[APS_UNENCRYPTED_TKEY_TESTS_DEVICES];
}ZB_PACKED_STRUCT application_dataset_t;
#endif

void aps_unencrypted_tkey_tests_zc_button_pressed(zb_uint8_t button_no);

#endif /* APS_UNENCRYPTED_TKEY_TESTS_ZC_H */
