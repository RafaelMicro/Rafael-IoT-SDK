/**
 * @file cpc_crc.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CPC_CRC_H
#define CPC_CRC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
// -----------------------------------------------------------------------------
// Data Types

// -----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

uint16_t cpc_get_crc_sw(const void* buffer, uint16_t buffer_length);

uint16_t cpc_get_crc_sw_with_security(const void* buffer,
                                      uint16_t buffer_length,
                                      const void* security_tag,
                                      uint16_t security_tag_length);

bool cpc_validate_crc_sw(const void* buffer, uint16_t buffer_length,
                         uint16_t expected_crc);

#ifdef __cplusplus
}
#endif

#endif // CPC_CRC_H
