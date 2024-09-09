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
#ifndef PRODUCTION_CONFIG_GENERATOR_H
#define PRODUCTION_CONFIG_GENERATOR_H 1

#define VERBOSE_PRINTF(...)                     \
  do {                                          \
    if (verbose_flag)                           \
      printf(__VA_ARGS__);                      \
  } while (0);

#define SET_AND_CHECK(...)                     \
   if (__VA_ARGS__ != RET_OK)                  \
   {                                           \
      printf("Set param failed\n");            \
      ret = RET_ERROR;                         \
      break;                                   \
   }

typedef enum application_type_e {
  GENERIC_DEVICE,
#if 0
  IAS_ZONE_SENSOR,
  SMOKE_DEVICE,
#endif
  NETWORK_COPROCESSOR,
  APPLICATION_TYPE_INVALID,
} application_type_t;

/* Pointer to function, which, given pointer to application specific section
 * and key and value string, will parse those string and set corresponding field in app section */
typedef zb_ret_t (*config_parameter_setter_t)(void *section, char *key, char* value);

typedef struct application_config_type_s
{
  char *name; /* Application name which will be read from input file */
  int app_config_block_size; /* Size of application specific section that should be passed to Zigbee application */
  config_parameter_setter_t setter; /* see config_parameter_setter_t */
} application_config_type_t;

#define PRODUCTION_CONFIG_HEADER             {0xE7, 0x37, 0xDD, 0xF6}

#define TX_POWER_MIN_VALUE_DBM               -40
#define TX_POWER_MAX_VALUE_DBM               50

#define MIN_CHANNEL_NUMBER                   11
#define MAX_CHANNEL_NUMBER                   26

zb_uint32_t zb_calculate_production_config_crc(zb_uint8_t *prod_cfg_ptr, zb_uint16_t prod_cfg_size);
zb_uint16_t zb_crc16(zb_uint8_t *p, zb_uint16_t crc, zb_uint_t len);

#endif /* PRODUCTION_CONFIG_GENERATOR_H */

