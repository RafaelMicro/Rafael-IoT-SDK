/**
 * isp.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bootloader.h"
#include "crc.h"
#include "isp.h"
#include "mcu.h"
#include "slip.h"

typedef enum {
    ISP_WRITE_REG_CMD = 0x02,
    ISP_READ_ID_CMD = 0x04,
    ISP_READ_STATUS_CMD = 0x05,
    ISP_WRITE_STATUS_CMD = 0x06,
    ISP_WRITE_BYTE_CMD = 0x07,
    ISP_ERASE_FLASH_CMD = 0x08,
    ISP_WRITE_MEMORY_VERIFY_CMD = 0x09,
    ISP_WRITE_JUMP_FLASH_CMD = 0x0A,
    ISP_WRITE_SRAM_CMD = 0x0B,
    ISP_CHECK_CHIP_ID_CMD = 0x0C,
    ISP_VERIFY_FLASH_CMD = 0x0E,
} isp_command_t;

uint32_t isp_payload_check(uint8_t* pdata) {
    uint32_t ret = 0;
    uint16_t packet_length, crc16;

    packet_length = ((uint16_t)((pdata[2] << 8) | pdata[3])) + 5;
    crc16 = crc16_ccitt(pdata, packet_length);

    if (pdata[packet_length] != ((crc16 >> 8) & 0xFF)
        || (pdata[packet_length + 1] != (crc16 & 0xFF))) {
        DEBUG_PRINT("CRC16 error\r\n");
        DEBUG_PRINT("calculated crc: %x\r\n", crc16);
        DEBUG_PRINT("receivaed  crc: %x%x\r\n", pdata[packet_length],
                    pdata[packet_length + 1]);
        ret = 1;
    }
    return ret;
}

uint8_t* isp_command_generator(isp_command_meta_t* pt_meta) {
    uint8_t* p_isp_packet = NULL;
    uint16_t crc16;

    do {
        p_isp_packet = malloc(8 + pt_meta->data_len);

        if (p_isp_packet == NULL) {
            break;
        }
        // Response pattern : 0x52, 0xFF, 0x00, 0x02, 0xFF, 0x02, 0x00, 0xFF, 0xFF
        p_isp_packet[0] = 0x52;
        p_isp_packet[1] = pt_meta->seq;
        p_isp_packet[2] = ((pt_meta->data_len + 2) >> 8) & 0xFF;
        p_isp_packet[3] = (pt_meta->data_len + 2) & 0xFF;
        p_isp_packet[4] = ~(p_isp_packet[1] + p_isp_packet[2]
                            + p_isp_packet[3]);
        p_isp_packet[5] = pt_meta->command;
        p_isp_packet[6] = pt_meta->option;
        memcpy(&p_isp_packet[7], pt_meta->pdata, pt_meta->data_len);
        crc16 = crc16_ccitt(p_isp_packet, pt_meta->data_len + 2 + 5);
        p_isp_packet[7 + pt_meta->data_len] = (crc16 >> 8) & 0xFF;
        p_isp_packet[8 + pt_meta->data_len] = crc16 & 0xFF;

    } while (0);

    return p_isp_packet;
}

inline void isp_command_handler(uint8_t* isp_cmd_req) {
    isp_command_meta_t* pt_isp_cmd_rsp_meta = NULL;
    flash_status_t s;
    uint8_t* pisp_cmd_resp = NULL;
    uint8_t* pslip_packet = NULL;
    uint16_t slip_packet_len;
    uint32_t addr, value;
    uint8_t ch;
    switch (isp_cmd_req[5]) {
        case ISP_WRITE_REG_CMD: {
            // Response pattern : 0x52, 0xFF, 0x00, 0x02, 0xFF, 0x02, 0x00, 0xFF, 0xFF
            addr = (isp_cmd_req[7]) + (isp_cmd_req[8] << 8)
                   + (isp_cmd_req[9] << 16) + (isp_cmd_req[10] << 24);
            value = (isp_cmd_req[11]) + (isp_cmd_req[12] << 8)
                    + (isp_cmd_req[13] << 16) + (isp_cmd_req[14] << 24);

            (*((volatile uint32_t*)(addr))) = value;

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->option = 0x64;  // bypass sram verification 
            pt_isp_cmd_rsp_meta->data_len = 0;

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case ISP_WRITE_STATUS_CMD:
        case ISP_WRITE_SRAM_CMD: {
            // Response pattern : 0x52, 0xFF, 0x00, 0x02, 0xFF, 0x08, 0x00, 0xFF, 0xFF
            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->option = 0x00;
            pt_isp_cmd_rsp_meta->data_len = 0;

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case ISP_READ_STATUS_CMD: {
            // Response pattern : 0x52, 0xFF, 0x00, 0x03, 0xFF, 0x05, 0x00, 0x00, 0xFF, 0xFF
            s.require_mode = isp_cmd_req[6];
            flash_get_status_reg(&s);

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->option = 0x00;
            pt_isp_cmd_rsp_meta->data_len = 1;

            if (s.require_mode == 1) {
                pt_isp_cmd_rsp_meta->pdata[0] = s.status1;
            } else if (s.require_mode == 2) {
                pt_isp_cmd_rsp_meta->pdata[0] = s.status2;
            }

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case 0x1A: {
            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->data_len = 16;
            pt_isp_cmd_rsp_meta->option = 0x00; // status
            memset(&pt_isp_cmd_rsp_meta->pdata[1], 0xFF, 16);

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case ISP_CHECK_CHIP_ID_CMD: {
            uint8_t isp_cmd_resp[] = {0x52, 0xFF, 0x00, 0x06, 0xFF, 0x0C, 0x00,
                                      0x4F, 0x70, 0x07, 0x00, 0xFF, 0xFF};

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->option = 0x00;
            pt_isp_cmd_rsp_meta->data_len = 4;
            pt_isp_cmd_rsp_meta->pdata[1] = 0x4F;
            pt_isp_cmd_rsp_meta->pdata[2] = 0x70;
            pt_isp_cmd_rsp_meta->pdata[3] = 0x07;
            pt_isp_cmd_rsp_meta->pdata[4] = 0x00;

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case ISP_READ_ID_CMD: {
            uint32_t flash_id = flash_get_deviceinfo();

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->option = 0x00;
            pt_isp_cmd_rsp_meta->data_len = 4;
            pt_isp_cmd_rsp_meta->pdata[0] = flash_id & 0xFF;
            pt_isp_cmd_rsp_meta->pdata[1] = (flash_id >> 8) & 0xFF;
            pt_isp_cmd_rsp_meta->pdata[2] = (flash_id >> 16) & 0xFF;
            pt_isp_cmd_rsp_meta->pdata[3] = (flash_id >> 24) & 0xFF;

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case ISP_ERASE_FLASH_CMD: {
            addr = isp_cmd_req[10] << 24 | isp_cmd_req[9] << 16
                   | isp_cmd_req[8] << 8 | isp_cmd_req[7];

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->data_len = 0;
            if (bootloader_flash_op_event_trigger(addr, ISP_ERASE_FLASH_CMD,
                                                  isp_cmd_req[6], NULL)
                == 0) {
                pt_isp_cmd_rsp_meta->option = 0x00;
            } else {
                pt_isp_cmd_rsp_meta->option = 0x01;
            }

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);
            break;
        }
        case ISP_WRITE_MEMORY_VERIFY_CMD: {
            uint8_t write_page_resp[] = {0x52, 0xFF, 0x00, 0x02, 0xFF,
                                         0x09, 0x00, 0xFF, 0xFF};

            addr = isp_cmd_req[11] << 24 | isp_cmd_req[10] << 16
                   | isp_cmd_req[9] << 8 | isp_cmd_req[8];

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->data_len = 0;
            uint8_t* p_wdata = malloc(256);
            if (p_wdata == NULL) {
                break;
            }
            memcpy(p_wdata, &isp_cmd_req[12], 256);
            if (bootloader_flash_op_event_trigger(
                    addr, ISP_WRITE_MEMORY_VERIFY_CMD, isp_cmd_req[6], p_wdata)
                == 0) {
                pt_isp_cmd_rsp_meta->option = 0x00;
            } else {
                pt_isp_cmd_rsp_meta->option = 0x01;
            }

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            break;
        }
        case ISP_WRITE_JUMP_FLASH_CMD: {
            uint8_t jump_flash_resp[] = {0x52, 0xFF, 0x00, 0x02, 0xFF,
                                         0x0A, 0x00, 0xFF, 0xFF};

            pt_isp_cmd_rsp_meta = malloc(sizeof(isp_command_meta_t) + 2);

            if (pt_isp_cmd_rsp_meta == NULL) {
                break;
            }

            pt_isp_cmd_rsp_meta->command = isp_cmd_req[5];
            pt_isp_cmd_rsp_meta->seq = isp_cmd_req[1];
            pt_isp_cmd_rsp_meta->data_len = 0;
            pt_isp_cmd_rsp_meta->option = 0x00; // status

            pisp_cmd_resp = isp_command_generator(pt_isp_cmd_rsp_meta);

            if (pisp_cmd_resp == NULL) {
                break;
            }

            slip_packet_len = slip_get_encoded_length(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1);

            pslip_packet = malloc(slip_packet_len);

            if (pslip_packet == NULL) {
                break;
            }

            slip_packet_endcode(pisp_cmd_resp, 8 + pt_isp_cmd_rsp_meta->data_len + 1, pslip_packet,
                                &slip_packet_len);
            bootloader_uart_send(pslip_packet, slip_packet_len);

            if (isp_cmd_req[6] == 0x00) {
                // /*remap interrupt vector to flash*/
                // M32(REMAP) = 0x99;
                // /*jmp to flash*/
                // Jump_Handler(0x90000000);
            } else {
                // /*boot from SRAM*/
                // M32(REMAP) = 0x9B;
                // /*jmp to sram*/
                // Jump_Handler(0x20000000);
            }
            break;
        }
        default: break;
    }

    if (pt_isp_cmd_rsp_meta != NULL) {
        free(pt_isp_cmd_rsp_meta);
    }
    if (pisp_cmd_resp != NULL) {
        free(pisp_cmd_resp);
    }
    if (pslip_packet != NULL) {
        free(pslip_packet);
    }
}