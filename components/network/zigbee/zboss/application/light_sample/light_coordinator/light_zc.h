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
/* PURPOSE: Header file for Light Coordinator
*/

#ifndef LIGHT_ZC_H
#define LIGHT_ZC_H 1

#include "zboss_api.h"

/* Default channel */
#define LIGHT_ZC_CHANNEL_MASK (1l<<21)
/* IEEE address of ZC */
#define LIGHT_ZC_ADDRESS {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

/* Enable joined device check */
#define ZC_AUTO_SEARCH_AND_BIND
#define ZC_AUTO_SEARCH_AND_BIND_LVL_CTRL_CLST

#ifdef ZC_AUTO_SEARCH_AND_BIND

/* Joined device types */
enum simple_dev_type_e
{
  SIMPLE_DEV_TYPE_UNUSED,
  SIMPLE_DEV_TYPE_UNDEFINED,
  SIMPLE_DEV_TYPE_LIGHT,
  SIMPLE_DEV_TYPE_LIGHT_CONTROL,
};

/* Level control roles enumeration */
enum simple_dev_match_step_e
{
  MATCH_STEP_ON_OFF_LVL_CTRL_SERVER,
  MATCH_STEP_ON_OFF_LVL_CTRL_CLIENT,
};

/* Binding steps enumeration */
enum simple_dev_bind_step_e
{
  BIND_STEP_ON_OFF_CLST,
#ifdef ZC_AUTO_SEARCH_AND_BIND_LVL_CTRL_CLST
  BIND_STEP_LVL_CTRL_CLST,
#endif
};

/* Maximum number of devices that can be checked */
#define LIGHT_ZC_MAX_DEVICES 2

/* Context to store joined devices */
typedef struct simple_device_s
{
  zb_uint8_t     dev_type;
  zb_uint8_t     match_step;
  zb_uint8_t     bind_step;
  zb_uint8_t     assign_idx;
  zb_uint8_t     last_zdo_tsn;
  zb_uint8_t     match_ep;
  zb_uint8_t     assign_table[LIGHT_ZC_MAX_DEVICES - 1];
  zb_uint16_t    short_addr;
  zb_ieee_addr_t ieee_addr;
} simple_device_t;

/* Global device context */
typedef struct light_zc_ctx_s
{
  simple_device_t devices[LIGHT_ZC_MAX_DEVICES];
} light_zc_ctx_t;

#endif  /* ZC_AUTO_SEARCH_AND_BIND */

#endif /* LIGHT_H */
