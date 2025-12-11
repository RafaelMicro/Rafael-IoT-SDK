/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file bootloader.h
 */

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define __reloc __attribute__((used, section("reloc_text")))

#ifndef FLASH_UNLOCK_PATTER
#define FLASH_UNLOCK_PATTER 0x52414254
#endif

#if defined(CONFIG_BOOTLOADER_DEBUG)
#define DEBUG_PRINT(fmt, args...) printf(fmt, ##args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#if defined(CONFIG_RT584_NONE_OS)
typedef struct
{
    uint32_t            magic_header;           /* Magic number */
    uint32_t            length;                 /* Include magic header and length */
    uint8_t             key_x[32];              /* Data length of FW in Bank 1 */
    uint8_t             key_y[32];              /* The starting flash address of fota bank */
    uint8_t             option[48];             /* Max of 48 Byte */
} bootloader_signature_info_t;
#endif

void bootloader_uart_init(void);
void bootloader_uart_send(uint8_t* buf, uint16_t len);
uint8_t bootloader_uart_peek(uint8_t* pbuf, uint16_t len);
size_t bootloader_uart_recv(uint8_t* pbuf, size_t len);
void bootloader_check_flash_op_command(void);
uint32_t bootloader_flash_op_event_trigger(uint32_t address, uint8_t cmd,
                                           uint8_t op, uint8_t* pdata);
uint32_t bootloader_run_loop(void);


#endif // BOOTLOADER_H