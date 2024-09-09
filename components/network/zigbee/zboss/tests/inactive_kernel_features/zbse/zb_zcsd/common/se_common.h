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
/* PURPOSE: Common samples header file.
*/

#ifndef SE_SAMPLE_COMMON_H
#define SE_SAMPLE_COMMIN_H 1

#ifndef ZB_PRODUCTION_CONFIG
#define ENABLE_RUNTIME_APP_CONFIG
/* #define ENABLE_PRECOMMISSIONED_REJOIN */
#endif

#define SE_CRYPTOSUITE_1

/*** Production config data ***/
typedef ZB_PACKED_PRE struct se_app_production_config_t
{
  zb_uint16_t version; /*!< Version of production configuration (reserved for future changes) */
  zb_uint16_t manuf_code;
  zb_char_t manuf_name[16];
  zb_char_t model_id[16];
}
ZB_PACKED_STRUCT se_app_production_config_t;

enum se_app_prod_cfg_version_e
{
  SE_APP_PROD_CFG_VERISON_1_0 = 1,
};

#define SE_APP_PROD_CFG_CURRENT_VERSION SE_APP_PROD_CFG_VERISON_1_0

#endif /* SE_SAMPLE_COMMON_H */
