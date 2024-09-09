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
/* PURPOSE: common definitions for this test.
*/

#ifndef __RTP_BDB_12_
#define __RTP_BDB_12_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZED {0xea, 0xff, 0xc0, 0x50, 0xef, 0xbe, 0xfe, 0xca}

#define RTP_PRODCONG_01_STEP_1_TIME_ZED  (ZB_TIME_ONE_SECOND)
#define RTP_PRODCONG_01_STEP_1_DELAY_ZED (ZB_TIME_ONE_SECOND)

/*** Production config data ***/
typedef ZB_PACKED_PRE struct se_app_production_config_t
{
  zb_uint16_t version; /*!< Version of production configuration (reserved for future changes) */
  zb_uint16_t manuf_code;
  zb_char_t manuf_name[16];
  zb_char_t model_id[16];
}
ZB_PACKED_STRUCT se_app_production_config_t;

#endif /* __RTP_BDB_12_ */
