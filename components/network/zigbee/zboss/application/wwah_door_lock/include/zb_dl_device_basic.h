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

#ifndef ZB_DL_DEVICE_BASIC_H
#define ZB_DL_DEVICE_BASIC_H 1

#define DL_INIT_BASIC_HW_VERSION        2
#define DL_INIT_BASIC_MANUF_NAME	  "DSR"

#define DL_INIT_BASIC_MODEL_ID     "SPv313p691"
#define DL_INIT_BASIC_DATE_CODE    "2017-02-01"
#define DL_INIT_BASIC_LOCATION_ID  "US"
#define DL_INIT_BASIC_PH_ENV       0

/* attributes of Basic cluster */
typedef struct dl_device_basic_attr_s
{
  zb_uint8_t zcl_version;
  zb_uint8_t app_version;
  zb_uint8_t stack_version;
  zb_uint8_t hw_version;
  zb_char_t mf_name[32];
  zb_char_t model_id[32];
  zb_char_t date_code[16];
  zb_uint8_t power_source;
  zb_char_t location_id[5];
  zb_uint8_t ph_env;
  zb_char_t sw_build_id[3];
} dl_device_basic_attr_t;

#endif /* ZB_DL_DEVICE_BASIC_H */
