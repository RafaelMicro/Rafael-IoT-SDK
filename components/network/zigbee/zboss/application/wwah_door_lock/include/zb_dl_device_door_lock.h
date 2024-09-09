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
/* PURPOSE:
*/

#ifndef ZB_DL_DEVICE_DOOR_LOCK_H
#define ZB_DL_DEVICE_DOOR_LOCK_H 1

#define DL_DOOR_LOCK_QUEUE_SIZE 10

typedef struct dl_door_lock_event_s
{
  zb_uint16_t operation_event_code;
  zb_uint16_t timestamp;        // qsec
} dl_door_lock_event_t;

ZB_RING_BUFFER_DECLARE(dl_door_lock_queue, dl_door_lock_event_t, DL_DOOR_LOCK_QUEUE_SIZE);

/* Door lock cluster attributes data */
typedef struct door_lock_attr_s
{
  zb_uint8_t lock_state;
  zb_uint8_t lock_type;
  zb_uint8_t actuator_enabled;
  zb_uint16_t rf_operation_event_mask;
} door_lock_attr_t;

void dl_enable_event_retry(const zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t *params);
void dl_disable_event_retry(void);
void dl_send_notification(zb_bufid_t param, zb_uint16_t operation_event_code);

#endif /* ZB_DL_DEVICE_DOOR_LOCK_H */
