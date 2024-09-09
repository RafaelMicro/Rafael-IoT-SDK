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
/* PURPOSE: Header file for pan_id_conflict sample application for HA
*/

#ifndef PAN_ID_CONFLICT_H
#define PAN_ID_CONFLICT_H 1

#include "zboss_api.h"

#define APP_IEEE_ADDRESS {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

//#define APP_DEFAULT_APS_CHANNEL_MASK ZB_TRANSCEIVER_ALL_CHANNELS_MASK
#define APP_DEFAULT_APS_CHANNEL_MASK (1l<<21)

typedef struct app_ctx_s
{
  zb_bool_t enable_send_incorrect_beacon;
} app_ctx_t;

#endif /* BULB_H */
