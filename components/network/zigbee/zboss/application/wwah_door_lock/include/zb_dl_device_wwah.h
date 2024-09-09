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

#ifndef ZB_DL_DEVICE_WWAH_H
#define ZB_DL_DEVICE_WWAH_H 1

#define DEBUG_REPORT_TABLE_SIZE 10

#define MAX_REDELIVERY_ATTEMPTS() AER_CTX().params.max_re_delivery_attempts
#define FIRST_BACKOFF_TIME() AER_CTX().params.first_backoff_time_in_seconds
#define MAX_BACKOFF_TIME() AER_CTX().params.max_backoff_time_in_seconds
#define COMMON_RATIO() AER_CTX().params.backoff_sequence_common_ratio

void dl_process_wwah_debug_report_query_cb(zb_uint8_t param);
void setup_debug_report(void);

typedef struct dl_wwah_app_event_retry_ctx_s
{
  dl_door_lock_queue_t door_lock_queue;

  zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t params;
  zb_uint_t failed_attempts;
  zb_bufid_t pending_buf;
} dl_wwah_app_event_retry_ctx_t;

zb_bool_t dl_wwah_app_event_retry_should_discard_event(zb_uint8_t failed_attempts);
zb_uint_t dl_wwah_app_event_retry_get_next_timeout(zb_uint8_t failed_attempts);

#endif /* ZB_DL_DEVICE_WWAH_H */
