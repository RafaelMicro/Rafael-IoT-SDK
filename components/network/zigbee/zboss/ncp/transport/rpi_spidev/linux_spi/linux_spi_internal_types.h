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
/*  PURPOSE: Linux SPI internal types.
*/
#ifndef LINUX_SPI_INTERNAL_TYPES_
#define LINUX_SPI_INTERNAL_TYPES_

//#define TEST_SPI
#ifdef TEST_SPI
#include "test_cfg.h"
#endif

#include <stdbool.h>

typedef bool spi_bool_t;
#define SPI_FALSE false
#define SPI_TRUE  true

#endif /* LINUX_SPI_INTERNAL_TYPES_ */
