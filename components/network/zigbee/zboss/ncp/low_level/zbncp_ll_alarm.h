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
/*  PURPOSE: NCP low level protocol timeout alarm implementation.
*/
#ifndef ZBNCP_INCLUDE_GUARD_LL_ALARM_H
#define ZBNCP_INCLUDE_GUARD_LL_ALARM_H 1

#include "zbncp_types.h"
#include "zbncp_debug.h"
#include "zbncp_mem.h"
#include "zbncp_ll_time.h"

#ifndef ZBNCP_LL_PROTO_T_DEFINED
#define ZBNCP_LL_PROTO_T_DEFINED
typedef struct zbncp_ll_proto_s zbncp_ll_proto_t;
#endif /* ZBNCP_LL_PROTO_T_DEFINED */

/** @brief Type of alert notification callback */
typedef void zbncp_ll_alarm_callback(zbncp_ll_proto_t *ll);

/** @brief Alarm implementation */
typedef struct {
  zbncp_ll_time_t time;           /**< Current time */
  zbncp_ll_time_t timeout;        /**< Timeout to the next alert */
  zbncp_ll_alarm_callback *alert; /**< Alert notification callback */
  zbncp_ll_proto_t *ll;           /**< Pointer to NCP low-level protocol state context */
} zbncp_ll_alarm_t;

/** @brief Predicate to check whether alarm is active (i.e. has a valid timeout to a next alert). */
static inline zbncp_bool_t zbncp_ll_alarm_is_active(const zbncp_ll_alarm_t *alarm)
{
  return (alarm->timeout != ZBNCP_LL_TIMEOUT_INFINITE);
}

/** @brief Obtain the tmeout to a next alert. */
static inline zbncp_ll_time_t zbncp_ll_alarm_timeout(const zbncp_ll_alarm_t *alarm)
{
  return alarm->timeout;
}

/** @brief Predicate to check whether alarm has pending alert */
static inline zbncp_bool_t zbncp_ll_alarm_is_ready(const zbncp_ll_alarm_t *alarm)
{
  return (alarm->timeout == ZBNCP_LL_TIMEOUT_NOW);
}

/** @brief Initialize alarm object */
static inline void zbncp_ll_alarm_init(zbncp_ll_alarm_t *alarm,
  zbncp_ll_alarm_callback *alert, zbncp_ll_proto_t *ll, zbncp_ll_time_t time)
{
  alarm->alert = alert;
  alarm->ll = ll;
  alarm->time = time;
  alarm->timeout = ZBNCP_LL_TIMEOUT_INFINITE;
}

/** @brief Cancel the alarm. */
static inline void zbncp_ll_alarm_cancel(zbncp_ll_alarm_t *alarm)
{
  if (zbncp_ll_alarm_is_ready(alarm)) {
    ZBNCP_DBG_TRACE("alerting");
  } else {
    ZBNCP_DBG_TRACE("%zu", alarm->timeout);
  }
  alarm->timeout = ZBNCP_LL_TIMEOUT_INFINITE;
}

/** @brief Alert the user of the alarm. */
static inline void zbncp_ll_alarm_alert(zbncp_ll_alarm_t *alarm)
{
  if (alarm->alert != ZBNCP_NULL) {
    zbncp_ll_alarm_cancel(alarm);
    alarm->alert(alarm->ll);
  }
}

/** @brief Check if the alarm is ready and alert the user if needed. */
static inline void zbncp_ll_alarm_check(zbncp_ll_alarm_t *alarm)
{
  if (zbncp_ll_alarm_is_ready(alarm)) {
    zbncp_ll_alarm_alert(alarm);
  }
}

/** @brief Set up the alarm to alert the user on timeout. */
static inline void zbncp_ll_alarm_set(zbncp_ll_alarm_t *alarm, zbncp_ll_time_t timeout)
{
  ZBNCP_DBG_TRACE("%zu", timeout);
  alarm->timeout = timeout;
  zbncp_ll_alarm_check(alarm);
}

/** @brief Update the alarm internal state based on the current time. */
static inline void zbncp_ll_alarm_update_time(zbncp_ll_alarm_t *alarm, zbncp_ll_time_t time)
{
  if (zbncp_ll_alarm_is_active(alarm)) {
    zbncp_ll_time_t delta = (time > alarm->time) ? (time - alarm->time) : 0u;
    if (delta > alarm->timeout) {
      delta = alarm->timeout;
    }
    alarm->timeout -= delta;
  }
  alarm->time = time; 
}

#endif /* ZBNCP_INCLUDE_GUARD_LL_ALARM_H */
