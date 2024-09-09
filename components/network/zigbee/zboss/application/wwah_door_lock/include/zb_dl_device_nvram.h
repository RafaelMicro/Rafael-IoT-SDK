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

#ifndef ZB_DL_DEVICE_NVRAM_H
#define ZB_DL_DEVICE_NVRAM_H 1

#ifndef ZB_USE_NVRAM
#error "NVRAM support is needed - define ZB_USE_NVRAM"
#endif

typedef ZB_PACKED_PRE struct wwah_door_lock_device_nvram_dataset_s
{
  zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t app_event_retry;
  zb_uint8_t aligned[1];
} ZB_PACKED_STRUCT
dl_device_nvram_dataset_t;

/* Check dataset alignment for IAR compiler and ARM Cortex target platfrom */
ZB_ASSERT_IF_NOT_ALIGNED_TO_4(dl_device_nvram_dataset_t);


/*** Production config data ***/
typedef ZB_PACKED_PRE struct dl_production_config_t
{
  zb_uint16_t version; /*!< Version of production configuration (reserved for future changes) */
  zb_char_t manuf_name[16];
  zb_char_t model_id[16];
  zb_uint16_t manuf_code;
  zb_uint16_t overcurrent_ma;
  zb_uint16_t overvoltage_dv;
} ZB_PACKED_STRUCT dl_production_config_t;

zb_uint16_t dl_get_nvram_data_size(void);
void dl_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length);
zb_ret_t dl_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos);
void dl_write_app_data(zb_uint8_t param);

#endif /*ZB_DL_DEVICE_NVRAM_H */
