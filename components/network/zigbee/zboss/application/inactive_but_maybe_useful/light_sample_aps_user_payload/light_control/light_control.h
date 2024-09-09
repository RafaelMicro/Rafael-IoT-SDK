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
/* PURPOSE: Header file for switch sample application for HA
*/

#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H 1

#include "zboss_api.h"

/* Remove that define if your board have not LEDs or buttons */
//#define ZB_USE_BUTTONS

#define LIGHT_CONTROL_ENDPOINT 1

#define LIGHT_CONTROL_IEEE_ADDR {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define LIGHT_CONTROL_CHANNEL_MASK (1l<<21)

//#define LIGHT_CONTROL_START_SEARCH_LIGHT

#define MATCH_DESC_REQ_START_DELAY (2*ZB_TIME_ONE_SECOND)
#define MATCH_DESC_REQ_TIMEOUT (20*ZB_TIME_ONE_SECOND)

#define LIGHT_CONTROL_BUTTON_OFF 0
#define LIGHT_CONTROL_BUTTON_ON 1

#define LIGHT_CONTROL_BUTTON_TRESHOLD ZB_TIME_ONE_SECOND

#define LIGHT_CONTROL_BUTTON_SHORT_POLL_TMO ZB_MILLISECONDS_TO_BEACON_INTERVAL(50)
#define LIGHT_CONTROL_BUTTON_LONG_POLL_TMO  ZB_MILLISECONDS_TO_BEACON_INTERVAL(300)

#define LIGHT_CONTROL_DIMM_STEP 15
#define LIGHT_CONTROL_DIMM_TRANSACTION_TIME 2

typedef struct light_control_bulb_params_s
{
  zb_uint8_t endpoint;
  zb_uint16_t short_addr;
  zb_ieee_addr_t ieee_addr;
} light_control_bulb_params_t;

typedef enum light_control_button_state_s
{
  LIGHT_CONTROL_BUTTON_STATE_IDLE,
  LIGHT_CONTROL_BUTTON_STATE_PRESSED,
  LIGHT_CONTROL_BUTTON_STATE_UNPRESSED
} light_control_button_state_t;

typedef struct light_control_button_s
{
  light_control_button_state_t button_state;
  zb_time_t timestamp;
} light_control_button_t;

#ifndef ZB_USE_BUTTONS
/* If we do not have buttons, schedule on-off switching every BULB_ON_OFF_TIMEOUT. */
#define BULB_ON_OFF_TIMEOUT ZB_TIME_ONE_SECOND * 15
#define BULB_ON_OFF_FAILURE_CNT 10

void light_control_send_on_off_alarm_delayed(zb_uint8_t param);
#endif

typedef struct light_control_ctx_s
{
  light_control_bulb_params_t bulb_params;
  light_control_button_t button;
#ifndef ZB_USE_BUTTONS
  zb_uint8_t bulb_on_off_state;
  zb_uint8_t bulb_on_off_failure_cnt;
#endif
} light_control_ctx_t;

typedef ZB_PACKED_PRE struct light_control_device_nvram_dataset_s
{
  zb_ieee_addr_t bulb_ieee_addr;
  zb_uint16_t bulb_short_addr;
  zb_uint8_t bulb_endpoint;
  zb_uint8_t aligned;
} ZB_PACKED_STRUCT
light_control_device_nvram_dataset_t;

void find_light_bulb(zb_uint8_t param);
void find_light_bulb_alarm(zb_uint8_t param);
void find_light_bulb_cb(zb_uint8_t param);

void bulb_ieee_addr_req(zb_uint8_t param);
void bulb_ieee_addr_req_cb(zb_uint8_t param);

void light_control_bind_bulb(zb_uint8_t param);

void test_leave_nwk(zb_uint8_t param);

void light_control_leave_and_join(zb_uint8_t param);
void light_control_retry_join(zb_uint8_t param);

zb_uint16_t light_control_get_nvram_data_size();
void light_control_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length);
zb_ret_t light_control_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos);

/* Handler for specific zcl commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);

void light_control_button_handler(zb_uint8_t button_no);
void light_control_button_pressed(zb_uint8_t button_no);
void bulb_hal_set_connect(zb_bool_t on);

#endif /* LIGHT_CONTROL_H */
