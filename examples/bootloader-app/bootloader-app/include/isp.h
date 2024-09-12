/**
 * @file isp.h
 */

#ifndef ISP_H
#define ISP_H

#include <stdint.h>
#include <stdio.h>

#include "bootloader.h"

typedef struct {
    uint8_t command;
    uint8_t option;
    uint8_t seq;
    uint16_t data_len;
    uint8_t pdata[256];
} isp_command_meta_t;

static inline uint32_t isp_header_check(uint8_t* pdata, uint16_t* p_length) {
    uint32_t ret = 0;
    uint8_t checksum;
    uint16_t length;

    checksum = ~(pdata[1] + pdata[2] + pdata[3]);
    length = (uint16_t)((pdata[2] << 8) | pdata[3]);

    if (pdata[0] != 0x52 || checksum != pdata[4] || length > 264) {
        DEBUG_PRINT("Checksum error: (%x, %x)\r\n", pdata[0], pdata[4]);
        DEBUG_PRINT("Checksum: %x\r\n", checksum);
        ret = 1;
    }
    *p_length = length;
    return ret;
}

uint32_t isp_payload_check(uint8_t* pdata);
uint8_t* isp_command_generator(isp_command_meta_t* pt_meta);
void isp_command_handler(uint8_t* isp_cmd_req);
#endif // ISP_H