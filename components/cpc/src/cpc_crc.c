/**
 * @file cpc_crc.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "cpc_crc.h"
#include "log.h"

static uint16_t cpc_compute_crc16(uint8_t new_byte, uint16_t prev_result);

uint16_t cpc_get_crc_sw(const void* buffer, uint16_t buffer_length) {
    uint16_t i;
    uint16_t crc = 0;
    for (i = 0; i < buffer_length; i++)
        crc = cpc_compute_crc16((uint8_t)((uint8_t*)buffer)[i], crc);
    return crc;
}

uint16_t cpc_get_crc_sw_with_security(const void* buffer,
                                      uint16_t buffer_length,
                                      const void* security_tag,
                                      uint16_t security_tag_length) {
    uint16_t i;
    uint16_t crc = 0;
    for (i = 0; i < buffer_length; i++)
        crc = cpc_compute_crc16((uint8_t)((uint8_t*)buffer)[i], crc);

    for (i = 0; i < security_tag_length; i++)
        crc = cpc_compute_crc16((uint8_t)((uint8_t*)security_tag)[i], crc);
    return crc;
}

bool cpc_validate_crc_sw(const void* buffer, uint16_t buffer_length,
                         uint16_t expected_crc) {
    return (cpc_get_crc_sw(buffer, buffer_length) == expected_crc);
}

static uint16_t cpc_compute_crc16(uint8_t new_byte, uint16_t prev_result) {

#ifdef CONFIG_CPC_CRC_0
    prev_result = ((uint16_t)(prev_result >> 8))
                  | ((uint16_t)(prev_result << 8));
    prev_result ^= new_byte;
    prev_result ^= (prev_result & 0xff) >> 4;
    prev_result ^= (uint16_t)(((uint16_t)(prev_result << 8)) << 4);
    prev_result ^=
        ((uint8_t)(((uint8_t)(prev_result & 0xff)) << 5))
        | ((uint16_t)((uint16_t)((uint8_t)(((uint8_t)(prev_result & 0xff))
                                           >> 3))
                      << 8));
#else
    for (uint8_t bit = 0; bit < 8; bit++) {
        prev_result ^= (new_byte & 0x01);
        prev_result = (prev_result & 0x01) ? (prev_result >> 1) ^ 0x8408
                                           : (prev_result >> 1);
        new_byte = new_byte >> 1;
    }
#endif
    return prev_result;
}
