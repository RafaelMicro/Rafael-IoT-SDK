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
/* PURPOSE: human-readable formatting utility functions
*/
#ifndef ZB_SE_HRF_UTILS_H
#define ZB_SE_HRF_UTILS_H 1

#include "zb_types.h"
#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_price.h"

/** @def SE_HRF_MAX_UINT48_DIGITS
 *  @brief max number of digits in text representation of any 48-bit unsigned integer
 */
#define SE_HRF_MAX_UINT48_DIGITS 15

/** @def SE_HRF_UINT48_BUF_LEN
 *  @brief buffer size for formatted text representation of 48-bit unsigned integer
 *
 *  buf_len = max_number_of_digits + 1 (.) + 1 (\0)
 */
#define SE_HRF_UINT48_BUF_LEN (SE_HRF_MAX_UINT48_DIGITS + 1 + 1)


/* buf_len =
 *  10 ((str(2 ^ 32 - 1))) +
 *  1 (.) +
 *  3 (currency) +
 *  1 (/) + units (20)
 *  10 (spaces + reserved) + len('\0')
 */
#define SE_HRF_PRICE_BUF_LEN (10 + 1 + 3 + 1 + 20 + 10 + 1)

/** @fn const zb_char_t *se_hrf_metering_device_type(zb_uint8_t val)
 *  @brief converts integer MeteringDeviceType value to human-readable format
 */
const zb_char_t *se_hrf_metering_device_type(zb_uint8_t val);

/** @fn const zb_char_t *se_hrf_metering_unit_of_measure(zb_uint8_t val)
 *  @brief converts integer UnitOfMeasure value to human-readable format
 */
const zb_char_t *se_hrf_metering_unit_of_measure(zb_uint8_t val);

/** @fn zb_char_t *se_hrf_metering_format_uint48_value(zb_uint48_t value, zb_uint8_t fmt_left, zb_uint8_t fmt_right, zb_uint8_t fmt_suppr, zb_char_t *buf)
 *  @brief formats 48-bit integer value and puts its text representation to the given buffer
 *  @returns pointer to the formatted value first char within the buffer or NULL if buffer is small
 */
zb_char_t *se_hrf_metering_format_uint48_value(zb_uint48_t value,
                                               zb_uint8_t fmt_left,
                                               zb_uint8_t fmt_right,
                                               zb_uint8_t fmt_suppr,
                                               zb_char_t *buf,
                                               zb_uint8_t buflen);

zb_char_t *se_hrf_format_price(zb_uint32_t raw_price,
                               zb_uint16_t currency,
                               zb_uint8_t trailing_digit,
                               zb_uint8_t unit,
                               zb_char_t *buf,
                               zb_uint8_t buflen);
#endif /* ZB_SE_HRF_UTILS_H */
