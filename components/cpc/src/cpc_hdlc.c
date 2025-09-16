/**
 * @file cpc_hdlc.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cpc_hdlc.h"
#include "cpc_crc.h"

void cpc_hdlc_create_header(uint8_t* header_buf, uint8_t address,
                            uint16_t length, uint8_t control,
                            bool compute_crc) {
    header_buf[0] = CPC_HDLC_FLAG_VAL;
    header_buf[1] = address;
    header_buf[2] = (uint8_t)length;
    header_buf[3] = (uint8_t)(length >> 8);
    header_buf[4] = control;

    if (compute_crc) {
        uint16_t hcs = cpc_get_crc_sw(header_buf, CPC_HDLC_HEADER_SIZE);
        header_buf[5] = (uint8_t)hcs;
        header_buf[6] = (uint8_t)(hcs >> 8);
    }
}
