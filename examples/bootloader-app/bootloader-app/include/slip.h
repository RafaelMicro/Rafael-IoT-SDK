/**
 * @file slip.h
 */

#ifndef SLIP_H
#define SLIP_H

#define SLIP_DELIMITER 0xC0
#define SLIP_ESC       0xDB

#define SLIP_ESC_DELIM 0xDC
#define SLIP_ESC_ESC   0xDD
#include "bootloader.h"
uint16_t slip_get_encoded_length(uint8_t* src, uint16_t src_len);
void slip_packet_endcode(uint8_t* src, uint16_t src_len, uint8_t* dest,
                         uint16_t* dest_len);

uint8_t slip_packet_decode(uint8_t* data, uint16_t idx);

#define SLIP_DATA_READ_FUNC(p, c) bootloader_uart_recv(p, c)

#endif // SLIP_H