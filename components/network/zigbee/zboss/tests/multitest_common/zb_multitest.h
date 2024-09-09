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
/*  PURPOSE:
*/
#ifndef ZB_MULTITEST_H
#define ZB_MULTITEST_H 1

#include "zb_common.h"
#include "zb_test_step.h"

#define ZB_MULTITEST_DEAULT_PAGE    0
#define ZB_MULTITEST_DEAULT_CHANNEL 11
#define ZB_MULTITEST_DEAULT_MODE ZB_TEST_CONTROL_ALARMS

typedef struct zb_test_table_s
{
  char *test_name;
  void (*main_p)();
  void (*startup_complete_p)(zb_uint8_t param);
}
zb_test_table_t;

typedef struct zb_test_ctx_s
{
  zb_uint8_t page;
  zb_uint8_t channel;
  zb_test_control_mode_t mode;
} zb_test_ctx_t;


/**
 * @brief Resets multitest context
 */
void zb_multitest_reset_test_ctx(void);


/**
 * @brief Returns multitest context
 *
 * @return zb_multitest_ctx_t* multitest context
 */
zb_test_ctx_t* zb_multitest_get_test_ctx(void);


/**
 * @brief Perform multitest initialization.
 *
 * This function should be implemented in specific multitest.
 */
void zb_multitest_init(void);


/**
 * @brief Returns tests table for the multitest.
 *
 * This function should be implemented in specific multitest.
 *
 * @return zb_test_table_t* tests table
 */
const zb_test_table_t* zb_multitest_get_tests_table(void);


/**
 * @brief Returns tests table size for the multitest.
 *
 * This function should be implemented in specific multitest.
 *
 * @return zb_uindex_t tests table size
 */
zb_uindex_t zb_multitest_get_tests_table_size(void);


#endif /* ZB_MULTITEST_H */
