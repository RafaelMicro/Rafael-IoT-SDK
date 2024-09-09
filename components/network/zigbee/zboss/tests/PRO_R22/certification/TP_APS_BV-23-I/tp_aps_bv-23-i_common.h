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
#ifndef __APS__BV__35__H
#define __APS__BV__35__H 1

#define ZB_EXIT( _p )

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R3 {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}

//#define TEST_CHANNEL (1l << 24)
#define TEST_PAN_ID  0x1AAA

#define USE_NWK_MULTICAST       ZB_FALSE

#define TEST_BUFFER_LEN         0x10

#define GROUP_3                 0x03
#define GROUP_4                 0x04

#define ZC_EP_SRC               0x01

#define ZR1_EP_SRC              0x01
#define ZR1_EP_DST              0x01

#define ZR2_BIND_EP             0xF0
#define ZR2_BIND_ADDR           GROUP_3

#define ZR3_BIND_EP             0xF0
#define ZR3_BIND_ADDR           GROUP_4

/* ZC */
#define TIME_ZC_CONNECTION      (50 * ZB_TIME_ONE_SECOND)

/* group 3 binding */
#define TP_APS_BV_23_I_STEP_1_DELAY_ZC  (1 * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TP_APS_BV_23_I_STEP_1_TIME_ZC   (30 * ZB_TIME_ONE_SECOND)
/* group 4 binding */
#define TP_APS_BV_23_I_STEP_7_TIME_ZC   (40 * ZB_TIME_ONE_SECOND)
/* group 3 unbinding */
#define TP_APS_BV_23_I_STEP_9_TIME_ZC   (5 * ZB_TIME_ONE_SECOND)


/* ZR1 */
#define TIME_ZR1_CONNECTION             (55 * ZB_TIME_ONE_SECOND)

/* Send test buffer to Group 3 */
#define TP_APS_BV_23_I_STEP_5_DELAY_DUTZR  (5  * ZB_TIME_ONE_SECOND + TIME_ZR1_CONNECTION)
#define TP_APS_BV_23_I_STEP_5_TIME_DUTZR  (35  * ZB_TIME_ONE_SECOND)
/* Send test buffer to Group 4 */
#define TP_APS_BV_23_I_STEP_8_TIME_DUTZR  (30  * ZB_TIME_ONE_SECOND)
/* Send test buffer to Group 3 */
#define TP_APS_BV_23_I_STEP_10_TIME_DUTZR (10  * ZB_TIME_ONE_SECOND)
/* Send test buffer to Group 4 */
#define TP_APS_BV_23_I_STEP_11_TIME_DUTZR (5  * ZB_TIME_ONE_SECOND)


#ifndef NCP_MODE_HOST
#define PRINT_BIND_TABLE() {\
  zb_uint8_t i = 0, src_ind = 0;\
  if (!ZG->aps.binding.dst_n_elements)\
  {\
    TRACE_MSG(TRACE_INFO3, "###binding table is empty (dst array size = 0)", (FMT__0));\
  }\
  for (i = 0; i < ZG->aps.binding.dst_n_elements; ++i)\
  {\
    if (ZG->aps.binding.dst_table[i].dst_addr_mode == ZB_APS_BIND_DST_ADDR_LONG)\
    {\
      TRACE_MSG(TRACE_INFO3, "###bind_dst_table[%hd]: long_addr_ref=%hd; ep=%hd; src_index=%hd;", (FMT__H_H_H_H,\
        i,\
        ZG->aps.binding.dst_table[i].u.long_addr.dst_addr,\
        ZG->aps.binding.dst_table[i].u.long_addr.dst_end,\
        ZG->aps.binding.dst_table[i].src_table_index));\
      src_ind = ZG->aps.binding.dst_table[i].src_table_index;\
      TRACE_MSG(TRACE_INFO3, "###-->bind_src_table[%hd]: long_addr_ref=%hd; ep=%hd; cluster=0x%x;", (FMT__H_H_H_H,\
        src_ind,\
        ZG->aps.binding.src_table[src_ind].src_addr,\
        ZG->aps.binding.src_table[src_ind].src_end,\
        ZG->aps.binding.src_table[src_ind].cluster_id));\
    }\
    else\
    {\
      TRACE_MSG(TRACE_INFO3, "###bind_dst_table[%hd]: group_addr=%hd; src_index=%hd;", (FMT__H_H_H,\
        i,\
        ZG->aps.binding.dst_table[i].u.group_addr,\
        ZG->aps.binding.dst_table[i].src_table_index));\
      src_ind = ZG->aps.binding.dst_table[i].src_table_index;\
      TRACE_MSG(TRACE_INFO3, "###-->bind_src_table[%hd]: long_addr_ref=%hd; ep=%hd; cluster=0x%x;", (FMT__H_H_H_H,\
        src_ind,\
        ZG->aps.binding.src_table[src_ind].src_addr,\
        ZG->aps.binding.src_table[src_ind].src_end,\
        ZG->aps.binding.src_table[src_ind].cluster_id));\
    }\
  } \
}
#else
#define PRINT_BIND_TABLE() { \
  ZB_ASSERT(ZB_FALSE && "TODO: use NCP API here");\
}
#endif /* NCP_MODE_HOST */

#endif
