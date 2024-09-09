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
#ifndef PRODUCTION_CONFIG_INTERFACE_H_
#define PRODUCTION_CONFIG_INTERFACE_H_

typedef struct prod_cfg_param_s
{
  char *ieee;
  char *power;
  char *mask;
  unsigned char g_binary_buffer[1024];
  unsigned int g_config_len;
} prod_cfg_param_t;

int prepare_generic_binary_output(prod_cfg_param_t *prod_cfg_param);
int parse_generic_binary_input(prod_cfg_param_t *prod_cfg_param);

#endif /* PRODUCTION_CONFIG_INTERFACE_H_ */
