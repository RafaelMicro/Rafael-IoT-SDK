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
/* PURPOSE: Samples with all optional attributes of the Price cluster
*/
#ifndef PRICE_SRV_ATTR_SETS_H_
#define PRICE_SRV_ATTR_SETS_H_
/******************************************************************************/
#include "zboss_api_core.h"
#include "zboss_api_aps.h"
#include "zboss_api_nwk.h"
#include "zboss_api_af.h"
#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_price.h"

/*
 * 09/01/17: IA:TODO:
 *    * Add comments for each attribute set definition.
 *    * Add explanation about attribute sets usage.
 */

/* See the zb_zcl_price.h for more information */

/******************************************************************************/
typedef struct zb_zcl_price_tier_label_s
{
  zb_uint8_t value[1 + 12];
} zb_zcl_price_tier_label_t;

typedef struct zb_zcl_price_attr_set_price_label_s
{
  zb_zcl_price_tier_label_t labels[48];
} zb_zcl_price_attr_set_price_label_t;

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(tier, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL_GNR(tier, &(data_ptr)->labels[tier - 1])

#define ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(tierN) \
  { .value = { sizeof("Tier0") - 1, 'T', 'i', 'e', 'r', '0' + tierN,} }

#define ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(tierN, tierM) \
  { .value = { sizeof("Tier00") - 1, 'T', 'i', 'e', 'r', '0' + tierN, '0' + tierM, } }

#define ZB_ZCL_PRICE_ATTR_SET_TIER_LABEL_INIT                      \
{ .labels = {                                                  \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(1),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(2),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(3),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(4),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(5),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(6),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(7),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(8),     \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERN_PRICE_LABEL_INIT(9),     \
                                                                   \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 0), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 1), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 2), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 3), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 4), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 5), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 6), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 7), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 8), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(1, 9), \
                                                                   \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 0), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 1), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 2), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 3), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 4), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 5), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 6), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 7), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 8), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(2, 9), \
                                                                   \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 0), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 1), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 2), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 3), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 4), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 5), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 6), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 7), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 8), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(3, 9), \
                                                                   \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 0), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 1), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 2), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 3), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 4), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 5), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 6), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 7), \
  ZB_ZCL_DECLARE_PRICE_SRV_ATTR_SET_TIERNM_PRICE_LABEL_INIT(4, 8), \
}}

/** Full list of TierNPrice attributes. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_TIER_LABEL(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (1, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (2, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (3, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (4, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (5, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (6, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (7, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (8, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL (9, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(10, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(11, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(12, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(13, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(14, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(15, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(16, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(17, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(18, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(19, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(20, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(21, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(22, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(23, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(24, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(25, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(26, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(27, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(28, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(29, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(30, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(31, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(32, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(33, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(34, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(35, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(36, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(37, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(38, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(39, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(40, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(41, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(42, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(43, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(44, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(45, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(46, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(47, data_ptr),                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL(48, data_ptr)

/******************************************************************************/
/** Block threshold primitive. */
typedef struct zb_zcl_price_block_threshold_attr_element_s
{
  zb_uint48_t value[15];
  zb_uint8_t  count;
} zb_zcl_price_block_threshold_attr_element_t;

#define ZB_ZCL_PRICE_BLOCK_THRESHOLD_ATTR_ELEMENT_INIT {.value = {{0}}, .count = 0}

typedef struct zb_zcl_price_attr_set_block_threshold_s
{
  zb_zcl_price_block_threshold_attr_element_t tier[16];
} zb_zcl_price_attr_set_block_threshold_t;

#define ZB_ZCL_PRICE_ATTR_SET_BLOCK_THRESHOLD_INIT { \
  .tier =  { \
    ZB_ZCL_PRICE_BLOCK_THRESHOLD_ATTR_ELEMENT_INIT, \
  }, \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(blockN, data_ptr)  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD_GNR(blockN, &((data_ptr)->value[blockN - 1]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_THRESHOLD_COUNT(data_ptr)  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_THRESHOLD_COUNT_GNR(&((data_ptr)->count))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD_LIST(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(1,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(1,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(2,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(3,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(4,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(5,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(6,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(7,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(8,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(9,  data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(10, data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(11, data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(12, data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(13, data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(14, data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD(15, data_ptr),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_THRESHOLD_COUNT(data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(blockN, tierN, data_ptr)  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD_GNR(blockN, tierN, &((data_ptr)->tier[tierN].value[blockN - 1]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_COUNT(tierN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_COUNT_GNR(tierN, &((data_ptr)->tier[tierN].count))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(tier, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(1,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(2,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(3,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(4,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(5,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(6,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(7,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(8,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(9,  tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(10, tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(11, tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(12, tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(13, tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(14, tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD(15, tier, data_ptr),      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_COUNT(tier, data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_BLOCK_THRESHOLD(data_ptr)                \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD_LIST(&((data_ptr)->tier[0])),\
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_LIST(15, data_ptr)

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_block_period_s {

  /* A UTCTime field to denote the time at which the block tariff period starts. */
  zb_uint32_t start_of_block_period;

  /** An unsigned 24-bit field to denote the block tariff period. */
  zb_uint24_t block_period_duration;

 /** BlockThresholdMultiplier provides a value to be multiplied against Threshold
   * parameter(s). If present, this attribute must be applied to all BlockThreshold
   * values to derive values that can be compared against the @e CurrentBlockPeriodConsumptionDelivered
   * attribute within the Metering cluster. */
  zb_uint24_t threshold_multiplier;

 /** @e BlockThresholdDivisor provides a value to divide the result of applying the
   * @e ThresholdMultiplier attribute to @e BlockThreshold values to derive values that
   * can be compared against the CurrentBlockPeriodConsumptionDelivered attribute
   * within the Metering cluster. In case no divisor is defined,
   * this field shall be set to 1. */
  zb_uint24_t threshold_divisor;

 /** An 8-bit bitmap where the least significant nibble is an enumerated sub-field
   * indicating the time base used for the duration, and the most significant nibble
   * is an enumerated sub-field providing duration control.
   * @see zb_zcl_price_block_period_duration_type_e */
  zb_uint8_t  block_period_duration_type;
} zb_zcl_price_attr_set_block_period_t;

#define ZB_ZCL_PRICE_ATTR_SET_BLOCK_PERIOD_INIT {0}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_BLOCK_PERIOD(data_ptr)                                              \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_START_OF_BLOCK_PERIOD(       &((data_ptr)->start_of_block_period)),     \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION(       &((data_ptr)->block_period_duration)),     \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_MULTIPLIER(         &((data_ptr)->threshold_multiplier)),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_DIVISOR(           &((data_ptr)->threshold_divisor)),         \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION_TYPE(  &((data_ptr)->block_period_duration_type))

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_commodity_s
{
  zb_uint8_t  commodity_type;
  zb_uint32_t standing_charge;
  zb_uint32_t conversion_factor;
  zb_uint32_t conversion_trailing_digit;
  zb_uint32_t calorific_value;
  zb_uint8_t  calorific_value_unit;
  zb_uint8_t  calorific_value_trailing_digit;
} zb_zcl_price_attr_set_commodity_t;

#define ZB_ZCL_PRICE_ATTR_SET_COMMODITY_INIT { \
  .conversion_factor = 0x10000000, \
  .conversion_trailing_digit = 0x70, \
  .calorific_value = 0x2625A00, \
  .calorific_value_unit = 0x1, \
  .calorific_value_trailing_digit = 0x60, \
}

#define  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_COMMODITY(data_ptr)                                                        \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_COMMODITY_TYPE( &((data_ptr)->commodity_type)),                                 \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_STANDING_CHARGE( &((data_ptr)->standing_charge)),                               \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR( &((data_ptr)->conversion_factor)),                           \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR_TRAILING_DIGIT( &((data_ptr)->conversion_trailing_digit)),    \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE( &((data_ptr)->calorific_value)),                               \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_UNIT( &((data_ptr)->calorific_value_unit)),                     \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_TRAILING_DIGIT( &((data_ptr)->calorific_value_trailing_digit))

/******************************************************************************/
typedef struct zb_zcl_price_block_price_information_attrs_tier_s
{
  zb_uint32_t block_price[16];
} zb_zcl_price_block_price_information_attrs_tier_t;

typedef struct zb_zcl_price_attr_set_block_price_information_s
{
  zb_zcl_price_block_price_information_attrs_tier_t tier[16];
} zb_zcl_price_attr_set_block_price_information_t;

#define ZB_ZCL_PRICE_ATTR_SET_BLOCK_PRICE_INFORMATION_INIT { .tier = { {.block_price = {0}}}}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(blockN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE_GNR(blockN, &((data_ptr)->tier[0].block_price[blockN - 1]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, blockM, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE_GNR(tierN, blockM, &((data_ptr)->tier[tierN].block_price[blockM - 1]))


#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK_PRICE(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(15, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE(16, data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(tierN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 15, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE(tierN, 16, data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK_PRICE(data_ptr),   \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_PRICE(15, data_ptr)

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_extended_price_information_s
{
  /** Extended tiers (16 to 48). */
  zb_uint32_t tiers[48 - 16 + 1];
  zb_uint32_t cpp[2];
} zb_zcl_price_attr_set_extended_price_information_t;

#define ZB_ZCL_PRICE_ATTR_SET_EXTENDED_PRICE_INFORMATION_INIT {. tiers = {0}, .cpp = {0}}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(tierN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN_GNR(tierN,&((data_ptr)->tiers[tierN - 16]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CPPN_PRICE(cppN, data_ptr)   \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CPPN_PRICE_GNR(cppN, &((data_ptr)->cpp[cppN - 1]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(16, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(17, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(18, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(19, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(20, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(21, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(22, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(23, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(24, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(25, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(26, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(27, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(28, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(29, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(30, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(31, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(32, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(33, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(34, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(35, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(36, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(37, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(38, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(39, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(40, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(41, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(42, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(43, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(44, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(45, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(46, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(47, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN(48, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CPPN_PRICE (1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CPPN_PRICE (2,  data_ptr)

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_tariff_information_s
{
  zb_uint8_t  tariff_label[1 + 24];
  zb_uint8_t  number_of_price_tiers_in_use;
  zb_uint8_t  number_of_block_thresholds_in_use;
  zb_uint8_t  tier_block_mode;
  zb_uint8_t  unit_of_measure;
  zb_uint16_t currency;
  zb_uint8_t  price_trailing_digit;
  zb_uint8_t  tariff_resolution_period;
  zb_uint32_t co2;
  zb_uint8_t  co2_unit;
  zb_uint8_t  co2_trailing_digit;
} zb_zcl_price_attr_set_tariff_information_t;

#define ZB_ZCL_PRICE_ATTR_SET_TARIFF_INFORMATION_INIT { \
  .tier_block_mode = 0xFF,                           \
  .co2 = 185,                                        \
  .co2_unit = 1,                                     \
}                                                    \

#define ZB_ZCL_PRICE_ATTR_SET_TARIFF_INFORMATION_INIT_T \
  (zb_zcl_price_attr_set_tariff_information_t)          \
  ZB_ZCL_PRICE_ATTR_SET_TARIFF_INFORMATION_INIT

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_TARIFF_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL( &((data_ptr)->tariff_label)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_PRICE_TIERS_IN_USE( &((data_ptr)->number_of_price_tiers_in_use)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE( &((data_ptr)->number_of_block_thresholds_in_use)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_UNIT_OF_MEASURE( &((data_ptr)->unit_of_measure)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CURRENCY( &((data_ptr)->currency)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TRAILING_DIGIT( &((data_ptr)->price_trailing_digit)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TARIFF_RESOLUTION_PERIOD( &((data_ptr)->tariff_resolution_period)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CO2( &((data_ptr)->co2)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CO2_UNIT( &((data_ptr)->co2_unit)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CO2_TRAILING_DIGIT( &((data_ptr)->co2_trailing_digit))

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_billing_information_s
{
  zb_uint32_t current_billing_period_start;
  zb_uint24_t current_billing_period_duration;
  zb_uint32_t last_billing_period_start;
  zb_uint24_t last_billing_period_duration;
  zb_uint32_t last_billing_period_consolidated_bill;
} zb_zcl_price_attr_set_billing_information_t;

#define ZB_ZCL_PRICE_ATTR_SET_BILLING_INFORMATION_INIT {0}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_BILLING_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_START( &(data_ptr)->current_billing_period_start), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_DURATION( &(data_ptr)->current_billing_period_duration), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_START( &(data_ptr)->last_billing_period_start), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_DURATION( &(data_ptr)->last_billing_period_duration), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_CONSOLIDATED_BILL( &(data_ptr)->last_billing_period_consolidated_bill) \

/******************************************************************************/
typedef struct zb_zcl_price_credit_payment_attr_elm_s
{
  zb_uint8_t value;
  zb_uint8_t date;
  zb_uint8_t ref;
} zb_zcl_price_credit_payment_attr_elm_t;

typedef struct zb_zcl_price_attr_set_credit_payment_s
{
  zb_uint32_t due_date;
  zb_uint8_t  status;
  zb_int32_t  over_due_amount;
  zb_int32_t  credit_payment;
  zb_uint8_t  credit_payment_period;
  zb_zcl_price_credit_payment_attr_elm_t credit_payments[5];
} zb_zcl_price_attr_set_credit_payment_t;


#define ZB_ZCL_PRICE_ATTR_SET_CREDIT_PAYMENT_INIT {0}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_GROUP(paymentN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_N(paymentN, &(data_ptr + paymentN - 1)->value), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_N(paymentN, &(data_ptr + paymentN - 1)->date), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_N(paymentN, &(data_ptr + paymentN - 1)->ref)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENTS(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_GROUP(1, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_GROUP(2, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_GROUP(3, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_GROUP(4, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_GROUP(5, data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_CREDIT_PAYMENT(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DUE_DATE( &(data_ptr)->due_date), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_STATUS( &(data_ptr)->status), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_OVER_DUE_AMOUNT( &(data_ptr)->over_due_amount), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT( &(data_ptr)->credit_payment), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_PERIOD( &(data_ptr)->credit_payment_period), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENTS( (data_ptr)->credit_payments)

/******************************************************************************/
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(tierN, data_ptr) \
 ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL_GNR(tierN,  &(data_ptr)->labels[tierN - 1])

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_TIER_LABEL(data_ptr)            \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(15, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(16, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(17, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(18, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(19, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(20, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(21, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(22, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(23, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(24, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(25, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(26, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(27, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(28, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(29, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(30, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(31, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(32, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(33, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(34, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(35, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(36, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(37, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(38, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(39, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(40, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(41, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(42, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(43, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(44, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(45, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(46, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(47, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL(48, data_ptr)

/******************************************************************************/
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(blockN, data_ptr)  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD_GNR(blockN, &((data_ptr)->values[blockN - 1]))

typedef struct zb_zcl_price_attr_set_received_block_threshold_s
{
  zb_uint48_t values[15];
} zb_zcl_price_attr_set_received_block_threshold_t;

#define ZB_ZCL_PRICE_ATTR_SET_RECEIVED_BLOCK_THRESHOLD_INIT {.values = {{0}}}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_THRESHOLD(data_ptr)      \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD(15, data_ptr)

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_received_block_period_s
{
  zb_uint32_t start_of_block_period;
  zb_uint24_t block_period_duration;
  zb_uint24_t threshold_multiplier;
  zb_uint24_t threshold_divisor;
} zb_zcl_price_attr_set_received_block_period_t;

#define ZB_ZCL_PRICE_ATTR_SET_RECEIVED_BLOCK_PERIOD_INIT {0}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PERIOD(data_ptr)                                 \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_START_OF_BLOCK_PERIOD( &((data_ptr)->start_of_block_period)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK_PERIOD_DURATION( &((data_ptr)->block_period_duration)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_MULTIPLIER( &((data_ptr)->threshold_multiplier)),     \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_DIVISOR( &((data_ptr)->threshold_divisor))

/******************************************************************************/
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(blockN, data_ptr)  \
 ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE_GNR(blockN, &((data_ptr)->tier[0].block_price[blockN - 1]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, blockM, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE_GNR(tierN, blockM, &((data_ptr)->tier[tierN].block_price[blockM - 1]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK_PRICE(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(15, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE(16, data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(tierN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 15, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE(tierN, 16, data_ptr)

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PRICE_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(1,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(2,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(3,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(4,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(5,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(6,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(7,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(8,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(9,  data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(10, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(11, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(12, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(13, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(14, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCK_PRICE(15, data_ptr)

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_received_extended_price_information_s
{
  /** Extended tiers (16 to 48). */
  zb_uint32_t tiers[48 - 16 + 1];
} zb_zcl_price_attr_set_received_extended_price_information_t;

#define ZB_ZCL_PRICE_ATTR_SET_RECEIVED_EXTENDED_PRICE_INFORMATION_INIT {.tiers = {0}}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(tierN, data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN_GNR(tierN, &((data_ptr)->tiers[tierN - 16]))

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_EXTENDED_PRICE_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(16, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(17, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(18, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(19, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(20, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(21, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(22, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(23, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(24, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(25, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(26, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(27, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(28, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(29, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(30, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(31, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(32, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(33, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(34, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(35, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(36, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(37, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(38, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(39, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(40, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(41, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(42, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(43, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(44, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(45, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(46, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(47, data_ptr), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN(48, data_ptr)

/******************************************************************************/
typedef struct zb_zcl_price_attr_set_received_tariff_information_s
{
  zb_uint8_t  tariff_label[1 + 24];
  zb_uint8_t  number_of_price_tiers_in_use;
  zb_uint8_t  number_of_block_thresholds_in_use;
  zb_uint8_t  tier_block_mode;
  zb_uint8_t  tariff_resolution_period;
  zb_uint32_t co2;
  zb_uint8_t  co2_unit;
  zb_uint8_t  co2_trailing_digit;
} zb_zcl_price_attr_set_received_tariff_information_t;

#define ZB_ZCL_PRICE_ATTR_SET_RECEIVED_TARIFF_INFORMATION_INIT \
  ZB_ZCL_PRICE_ATTR_SET_TARIFF_INFORMATION_INIT

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION(data_ptr) \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_LABEL( &((data_ptr)->tariff_label)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_PRICE_TIERS_IN_USE( &((data_ptr)->number_of_price_tiers_in_use)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE( &((data_ptr)->number_of_block_thresholds_in_use)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER_BLOCK_MODE( &((data_ptr)->tier_block_mode)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_RESOLUTION_PERIOD( &((data_ptr)->tariff_resolution_period)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2( &((data_ptr)->co2)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_UNIT( &((data_ptr)->co2_unit)), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_TRAILING_DIGIT( &((data_ptr)->co2_trailing_digit))

/******************************************************************************/
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_PRICE_SET_RECEIVED_BILLING_INFORMATION(data_ptr)                                                  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_START( &(data_ptr)->current_billing_period_start),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_DURATION( &(data_ptr)->current_billing_period_duration), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_START( &(data_ptr)->last_billing_period_start),             \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_DURATION( &(data_ptr)->last_billing_period_duration),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_CONSOLIDATED_BILL( &(data_ptr)->last_billing_period_consolidated_bill)

/******************************************************************************/
typedef struct zb_zcl_price_attr_delivered_values_s {
  zb_zcl_price_attr_set_price_label_t                 tier_label;
  zb_zcl_price_attr_set_block_threshold_t             block_threshold;
  zb_zcl_price_attr_set_block_period_t                block_period;
  zb_zcl_price_attr_set_commodity_t                   commodity;
  zb_zcl_price_attr_set_block_price_information_t     block_price_information;
  zb_zcl_price_attr_set_extended_price_information_t  extended_price_information;
  zb_zcl_price_attr_set_tariff_information_t          tariff_information;
  zb_zcl_price_attr_set_billing_information_t         billing_information;
  zb_zcl_price_attr_set_credit_payment_t              credit_payment;
} zb_zcl_price_attr_delivered_values_t;

typedef struct zb_zcl_price_attr_received_values_s {
  zb_zcl_price_attr_set_price_label_t                         tier_label;
  zb_zcl_price_attr_set_received_block_threshold_t            block_threshold;
  zb_zcl_price_attr_set_received_block_period_t               block_period;
  zb_zcl_price_attr_set_block_price_information_t             block_price_information;
  zb_zcl_price_attr_set_received_extended_price_information_t extended_price_information;
  zb_zcl_price_attr_set_received_tariff_information_t         tariff_information;
  zb_zcl_price_attr_set_billing_information_t                 billing_information;
} zb_zcl_price_attr_received_values_t;

typedef struct zb_zcl_price_attr_values_s {
  zb_zcl_price_attr_delivered_values_t delv;
  zb_zcl_price_attr_received_values_t  recv;
} zb_zcl_price_attr_values_t;

#define ZB_ZCL_PRICE_ATTR_VALUES_INIT {                                                             \
  .delv = {                                                                                         \
    .tier_label                 = ZB_ZCL_PRICE_ATTR_SET_TIER_LABEL_INIT,                            \
    .block_threshold            = ZB_ZCL_PRICE_ATTR_SET_BLOCK_THRESHOLD_INIT,                       \
    .block_period               = ZB_ZCL_PRICE_ATTR_SET_BLOCK_PERIOD_INIT,                          \
    .commodity                  = ZB_ZCL_PRICE_ATTR_SET_COMMODITY_INIT,                             \
    .block_price_information    = ZB_ZCL_PRICE_ATTR_SET_BLOCK_PRICE_INFORMATION_INIT,               \
    .extended_price_information = ZB_ZCL_PRICE_ATTR_SET_EXTENDED_PRICE_INFORMATION_INIT,            \
    .tariff_information         = ZB_ZCL_PRICE_ATTR_SET_TARIFF_INFORMATION_INIT,                    \
    .billing_information        = ZB_ZCL_PRICE_ATTR_SET_BILLING_INFORMATION_INIT,                   \
    .credit_payment             = ZB_ZCL_PRICE_ATTR_SET_CREDIT_PAYMENT_INIT,                        \
  },                                                                                                \
  .recv = {                                                                                         \
    .tier_label                  = ZB_ZCL_PRICE_ATTR_SET_TIER_LABEL_INIT,                           \
    .block_threshold             = ZB_ZCL_PRICE_ATTR_SET_RECEIVED_BLOCK_THRESHOLD_INIT,             \
    .block_period                = ZB_ZCL_PRICE_ATTR_SET_RECEIVED_BLOCK_PERIOD_INIT,                \
    .block_price_information     = ZB_ZCL_PRICE_ATTR_SET_BLOCK_PRICE_INFORMATION_INIT,              \
    .extended_price_information  = ZB_ZCL_PRICE_ATTR_SET_RECEIVED_EXTENDED_PRICE_INFORMATION_INIT,  \
    .tariff_information          = ZB_ZCL_PRICE_ATTR_SET_RECEIVED_TARIFF_INFORMATION_INIT,          \
    .billing_information         = ZB_ZCL_PRICE_ATTR_SET_BILLING_INFORMATION_INIT,                  \
  }                                                                                                 \
}

#define ZB_ZCL_DECLARE_PRICE_SRV_ALL_ATTR_LIST(attr_list, values)                                               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_TIER_LABEL,                 &(values)->delv.tier_label)                 \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_BLOCK_THRESHOLD,            &(values)->delv.block_threshold)            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_BLOCK_PERIOD,               &(values)->delv.block_period)               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_COMMODITY,                  &(values)->delv.commodity)                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION,    &(values)->delv.block_price_information)    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION, &(values)->delv.extended_price_information) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_TARIFF_INFORMATION,         &(values)->delv.tariff_information)         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_BILLING_INFORMATION,        &(values)->delv.billing_information)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT,             &(values)->delv.credit_payment)             \
                                                                                                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_TIER_LABEL,                  &(values)->recv.tier_label)                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_THRESHOLD,             &(values)->recv.block_threshold)                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PERIOD,                &(values)->recv.block_period)                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PRICE_INFORMATION,     &(values)->recv.block_price_information)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_EXTENDED_PRICE_INFORMATION,  &(values)->recv.extended_price_information)     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION,          &(values)->recv.tariff_information)             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_PRICE_SET_RECEIVED_BILLING_INFORMATION,         &(values)->recv.billing_information)            \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/******************************************************************************/
/* Price Cluster attribute definitions sample */

typedef struct zb_zcl_price_cli_attr_values_s
{
  zb_uint8_t price_increase_randomize_minutes;
  zb_uint8_t price_decrease_randomize_minutes;
  zb_uint8_t commodity_type;
} zb_zcl_price_cli_attr_values_t;

#define ZB_ZCL_DECLARE_PRICE_CLI_ATTR_LIST_FROM_STRUCT(attr_list, values)         \
  ZB_ZCL_DECLARE_PRICE_CLI_ATTR_LIST(attr_list,                                   \
                                     &(values)->price_increase_randomize_minutes, \
                                     &(values)->price_decrease_randomize_minutes, \
                                     &(values)->commodity_type)

/******************************************************************************/
#endif /* PRICE_SRV_ATTR_SETS_H_ */
