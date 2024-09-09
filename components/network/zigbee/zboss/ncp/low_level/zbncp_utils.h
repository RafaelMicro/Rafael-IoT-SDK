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
/*  PURPOSE: NCP utility functions declaration.
*/
#ifndef ZBNCP_INCLUDE_GUARD_UTILS_H
#define ZBNCP_INCLUDE_GUARD_UTILS_H 1

#include "zbncp_types.h"

#define ZBNCP_CRC8_INIT_VAL   0x00u   /**< Initial value to use for CRC8 calculation */
#define ZBNCP_CRC16_INIT_VAL  0x0000u /**< Initial value to use for CRC16 calculation */

/**
 * @brief Calculate CRC8 of the data buffer starting from initial CRC8 value.
 *
 * @param init - initial CRC8 value
 * @param data - constant pointer to the data buffer
 * @param size - size of the data buffer
 *
 * @return CRC8 of the data buffer
 */
zbncp_uint8_t zbncp_crc8(zbncp_uint8_t init, const void *data, zbncp_size_t size);

/**
 * @brief Calculate CRC16 of the data buffer starting from initial CRC16 value.
 *
 * @param init - initial CRC16 value
 * @param data - constant pointer to the data buffer
 * @param size - size of the data buffer
 *
 * @return CRC16 of the data buffer
 */
zbncp_uint16_t zbncp_crc16(zbncp_uint16_t init, const void *data, zbncp_size_t size);

/**
 * @brief Determine the minimum size value of the two.
 *
 * @param lhs - left hand side value to compare
 * @param lhs - right hand side value to compare
 *
 * @return minimum of the two
 */
static inline zbncp_size_t size_min(zbncp_size_t lhs, zbncp_size_t rhs)
{
  return ((lhs < rhs) ? lhs : rhs);
}

/**
 * @brief Determine the maximum size value of the two.
 *
 * @param lhs - left hand side value to compare
 * @param lhs - right hand side value to compare
 *
 * @return maximum of the two
 */
static inline zbncp_size_t size_max(zbncp_size_t lhs, zbncp_size_t rhs)
{
  return ((lhs > rhs) ? lhs : rhs);
}

#endif /* ZBNCP_INCLUDE_GUARD_UTILS_H */
