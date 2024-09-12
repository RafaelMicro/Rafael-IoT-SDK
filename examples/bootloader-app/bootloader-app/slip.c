/**
 * slip.c
 * 
 * 
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "slip.h"
#include "bootloader.h"

/*SLIP layer defined */

uint16_t slip_get_encoded_length(uint8_t* src, uint16_t src_len) {
    uint8_t* src_ptr;
    uint16_t isp_cmd_length = 0;
    uint16_t slip_packet_idx = 0;

    src_ptr = src;
    isp_cmd_length = src_len;

    slip_packet_idx++;

    while (isp_cmd_length > 0) {
        if (*src_ptr == SLIP_DELIMITER) {
            slip_packet_idx += 2;
        } else if (*src_ptr == SLIP_ESC) {
            slip_packet_idx += 2;
        } else {
            slip_packet_idx++;
        }
        src_ptr++;
        isp_cmd_length--;
    }
    slip_packet_idx++;
    return slip_packet_idx;
}

void slip_packet_endcode(uint8_t* src, uint16_t src_len, uint8_t* dest,
                         uint16_t* dest_len) {
    uint8_t *src_ptr, *dest_ptr;
    uint16_t isp_cmd_length = 0;
    uint16_t slip_packet_idx = 0;

    src_ptr = src;
    dest_ptr = dest;
    isp_cmd_length = src_len;

    *dest_ptr = SLIP_DELIMITER;
    slip_packet_idx++;
    dest_ptr++;

    while (isp_cmd_length > 0) {
        if (*src_ptr == SLIP_DELIMITER) {
            *dest_ptr = SLIP_ESC;
            dest_ptr++;
            *dest_ptr = SLIP_ESC_DELIM;
            dest_ptr++;
            slip_packet_idx += 2;
        } else if (*src_ptr == SLIP_ESC) {
            *dest_ptr = SLIP_ESC;
            dest_ptr++;
            *dest_ptr = SLIP_ESC_ESC;
            dest_ptr++;
            slip_packet_idx += 2;
        } else {
            *dest_ptr = *src_ptr;
            dest_ptr++;
            slip_packet_idx++;
        }

        src_ptr++;
        isp_cmd_length--;
    }

    *dest_ptr = SLIP_DELIMITER;
    slip_packet_idx++;
    dest_ptr++;

    *dest_len = slip_packet_idx;
}

uint8_t slip_packet_decode(uint8_t* data, uint16_t idx) {
    uint8_t* ptr = NULL;
    uint8_t slip_esc_flag = 0;
    uint8_t ch;

    ptr = data;
    while (idx > 0) {
        bootloader_check_flash_op_command();

        if (SLIP_DATA_READ_FUNC(&ch, 1)) {
            if (ch == SLIP_DELIMITER) {
                return 0xFF;
            } else {
                if (slip_esc_flag) {
                    if (ch == SLIP_ESC_DELIM) {
                        *ptr = SLIP_DELIMITER;
                    } else if (ch == SLIP_ESC_ESC) {
                        *ptr = SLIP_ESC;
                    }
                    ptr++;
                    idx--;
                    slip_esc_flag = 0x00;
                } else {
                    if (ch != SLIP_ESC) {
                        *ptr = ch;
                        ptr++;
                        idx--;
                    } else {
                        slip_esc_flag = 0xFF;
                    }
                }
            }
        }
        bootloader_check_flash_op_command();
    }
    return 0x00;
}