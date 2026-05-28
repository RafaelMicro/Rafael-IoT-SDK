
#include "ota_download_cmd_handler.h"
#include "app_ota.h"
#include "app_uart.h"
#include "cli.h"
#include "fota_define.h"
#include "hosal_flash.h"
#include "log.h"
#include "main.h"
#include "miu_ext_mem.h"
#include "uart.h"

#include <string.h>
#include "uart_stdio.h"

#define SWAP_UINT32(x)                                                         \
    (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8)       \
     | ((x) << 24))

typedef struct __attribute__((packed)) {
    uint8_t header[4];
    uint8_t len;
} ota_download_cmd_hdr;

typedef struct __attribute__((packed)) {
    uint32_t command_id;
    uint16_t address;
    uint8_t address_mode;
    uint8_t parameter[];
} ota_download_cmd_pd;

typedef struct __attribute__((packed)) {
    uint8_t cs;
} ota_download_cmd_end;

typedef struct __attribute__((packed)) {
    uint16_t image_type;
    uint16_t manufacturer_code;
    uint32_t file_version;
    uint32_t image_size;
    uint32_t total_pkt;
    uint32_t cur_pkt;
    uint16_t pkt_len;
    uint8_t pkt[];
} ota_img_info_t;

static void _cmd_common_gen_req(uint32_t cmd_id, uint8_t* pkt);

static ota_img_info_t gt_img_info;
static uint8_t* gp_ota_imgae_cache = NULL;

extern void gw_cmd_app_service_handle(uint32_t cmd_id, uint8_t* pkt);

static uint16_t g_tsn_adrr_tbl[0xFF] = {0};
static uint32_t gu32_gw_start_flag = 0;

static uint8_t _gateway_checksum_calc(uint8_t* pBuf, uint8_t len) {
    uint8_t cs = 0;

    for (int i = 0; i < len; i++) {
        cs += pBuf[i];
    }
    return (~cs);
}

void ota_download_cmd_send(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                           uint8_t src_endp, uint8_t* pParam, uint32_t len) {
    uint8_t* ota_download_cmd_pkt = NULL;
    uint32_t pkt_len;
    uint8_t idx = 0;

    do {
        pkt_len = sizeof(ota_download_cmd_hdr) + sizeof(ota_download_cmd_pd)
                  + len + sizeof(ota_download_cmd_end);

        if (src_endp != 0) {
            pkt_len += 1;
        }

        ota_download_cmd_pkt = xMalloc(pkt_len);

        if (ota_download_cmd_pkt == NULL) {
            break;
        }

        ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->header[0] = 0xFF;
        ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->header[1] = 0xFC;
        ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->header[2] = 0xFC;
        ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->header[3] = 0xFF;
        ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->len =
            sizeof(ota_download_cmd_pd) + len;

        if (src_endp != 0) {
            ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->len += 1;
        }

        idx += sizeof(ota_download_cmd_hdr);

        ((ota_download_cmd_pd*)(&ota_download_cmd_pkt[idx]))->command_id =
            cmd_id;
        ((ota_download_cmd_pd*)(&ota_download_cmd_pkt[idx]))->address = addr;
        ((ota_download_cmd_pd*)(&ota_download_cmd_pkt[idx]))->address_mode =
            addr_mode;

        if (src_endp != 0) {
            ((ota_download_cmd_pd*)(&ota_download_cmd_pkt[idx]))->parameter[0] =
                src_endp;
            memcpy(
                ((ota_download_cmd_pd*)(&ota_download_cmd_pkt[idx]))->parameter
                    + 1,
                pParam, len);
        } else {
            memcpy(
                ((ota_download_cmd_pd*)(&ota_download_cmd_pkt[idx]))->parameter,
                pParam, len);
        }

        idx += sizeof(ota_download_cmd_pd) + len;

        if (src_endp != 0) {
            idx += 1;
        }
        ((ota_download_cmd_end*)(&ota_download_cmd_pkt[idx]))->cs =
            _gateway_checksum_calc(
                (uint8_t*)&(
                    ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->len),
                ((ota_download_cmd_hdr*)(ota_download_cmd_pkt))->len + 1);

        log_debug(
            "------------------------      GW >>>> ------------------------");
        log_info_hexdump("  ", ota_download_cmd_pkt, pkt_len);
        uart_stdio_write(ota_download_cmd_pkt, pkt_len);
        if (ota_download_cmd_pkt) {
            xFree(ota_download_cmd_pkt);
        }
    } while (0);
}

static void ota_download_handle(uint32_t cmd_id, uint8_t* pBuf) {
    //  +--------------------------+-------> 0x0000_0000
    //  |     Bootloader (32K)     |
    //  +--------------------------+-------> 0x0000_8000
    //  |                          |
    //  |     Application (580K)   |
    //  |                          |
    //  +--------------------------+-------> 0x0009_9000
    //  |                          |
    //  |     OTA Target  (348K)   |
    //  |                          |
    //  +--------------------------+-------> 0x000F_0000
    //  |     Reserved    (16K)    |
    //  +--------------------------+-------> 0x000F_4000

    int i;
    uint32_t status = 0;
    static uint32_t flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB;

    // Use a static 256-byte buffer instead of a 4KB dynamic allocation to save RAM
    static uint32_t page_buf_u32[64]; /* 4-byte aligned for DMA */
    uint8_t* const page_buf = (uint8_t*)page_buf_u32;
    static uint32_t buf_len = 0;

    uint32_t crc32 = 0;
    ota_img_info_t* upg_data;

    // Erase command
    if (cmd_id == 0xF0000000) {
        for (i = 0; i < OTA_MAX_IMAGE_SECTORS; i++) {
            // Page erase (4096 bytes per sector)
            hosal_flash_erase(
                HOSAL_FLASH_ERASE_SECTOR,
                (FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB + (0x1000 * i)));
            flush_cache();
        }
        // Send ACK for erase completion
        ota_download_cmd_send(0xF0008000, 0, 0, 0, (uint8_t*)&status, 4);

    } else if (cmd_id == 0xF0000001) {
        // Write data command
        do {
            upg_data = (ota_img_info_t*)pBuf;

            // Initialization on the first packet
            if (upg_data->cur_pkt == 0) {
                memcpy((uint8_t*)&gt_img_info, pBuf, sizeof(gt_img_info));

                log_info("File Type: 0x%X", gt_img_info.image_type);
                log_info("Manufacturer Code: 0x%X",
                         gt_img_info.manufacturer_code);
                log_info("File Version: 0x%X", gt_img_info.file_version);
                log_info("File Size: 0x%X", gt_img_info.image_size);

                // Reset buffer length and set default flash erased state (0xFF)
                buf_len = 0;
                memset(page_buf, 0xFF, 256);
                flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB;
            }

            log_info("block %d / %d", upg_data->cur_pkt,
                     (gt_img_info.total_pkt - 1));

            // Process the currently received packet payload
            uint32_t remain_pkt_len = upg_data->pkt_len;
            uint8_t* p_pkt_data = upg_data->pkt;

            // Stream incoming data into the 256-byte flash page buffer
            while (remain_pkt_len > 0) {
                uint32_t copy_len = 256 - buf_len;
                if (copy_len > remain_pkt_len) {
                    copy_len = remain_pkt_len;
                }

                // Copy data to the buffer
                memcpy(&page_buf[buf_len], p_pkt_data, copy_len);
                buf_len += copy_len;
                p_pkt_data += copy_len;
                remain_pkt_len -= copy_len;

                // Once the buffer is full (256 bytes), write it to flash immediately
                if (buf_len == 256) {
                    while (flash_check_busy())
                        ;
                    flash_write_page((uint32_t)page_buf, flash_addr);
                    while (flash_check_busy())
                        ;
                    flush_cache();
                    flash_addr += 256;

                    // Reset the buffer and fill with 0xFF for the next chunk
                    buf_len = 0;
                    memset(page_buf, 0xFF, 256);
                }
            }

            // Handle the final packet
            if (upg_data->cur_pkt == (gt_img_info.total_pkt - 1)) {
                // Force write any remaining data in the buffer (less than 256 bytes)
                if (buf_len > 0) {
                    while (flash_check_busy())
                        ;
                    flash_write_page((uint32_t)page_buf, flash_addr);
                    while (flash_check_busy())
                        ;
                    flush_cache();
                    flash_addr += 256;
                }
                while (flash_check_busy())
                    ;

                // Verify the downloaded image via CRC check (4-byte aligned for DMA)
                static uint32_t read_buf_u32[64];
                uint8_t* read_buf = (uint8_t*)read_buf_u32;
                hosal_flash_read(HOSAL_FLASH_READ_PAGE,
                                 FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB,
                                 read_buf);

                uint32_t img_size = SWAP_UINT32(*(uint32_t*)(read_buf + 24))
                                    + 0x20;
                uint32_t img_ver = SWAP_UINT32(*(uint32_t*)read_buf);
                uint32_t img_crc = SWAP_UINT32(*(uint32_t*)(read_buf + 16));

                if (img_size == gt_img_info.image_size) {
                    crc32 = crc32checksum(
                        (FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB + 0x20),
                        (img_size - 0x20));
                } else {
                    // Size mismatch error
                    status = 0xFFFFFFED;
                    break; // Exit do-while loop
                }

                if (crc32 == SWAP_UINT32(*(uint32_t*)(read_buf + 16))) {
                    // CRC matched, update OTA boot information
                    ota_set_image_size(img_size);
                    ota_set_image_version(img_ver);
                    ota_set_image_crc(img_crc);
                    ota_bootinfo_reset();
                } else {
                    // CRC mismatch error
                    status = 0xFFFFFFEE;
                    break; // Exit do-while loop
                }
            }

            // If no errors occurred, update status to the current packet number
            status = upg_data->cur_pkt;

        } while (0);

        // Send ACK/NACK status back to the sender
        // This is outside the final-packet check to ensure every packet gets an ACK
        if (status != upg_data->cur_pkt) {
            // Send error status
            ota_download_cmd_send(0xF0009000, 0, 0, 0, (uint8_t*)&status, 4);
            app_uart0_disable();
            otPlatUartEnable();
            log_set_level(LOG_LEVEL_INFO);
        } else {
            // Send success status
            ota_download_cmd_send(0xF0008000, 0, 0, 0, (uint8_t*)&status, 4);
        }

    } else if (cmd_id == 0xF0000002) {
        // UART 0 back to CLI command
        ota_download_cmd_send(0xF0008000, 0, 0, 0, (uint8_t*)&status, 4);
        app_uart0_disable();
        otPlatUartEnable();
        log_set_level(LOG_LEVEL_INFO);

    }
}

void ota_download_cmd_proc(uint8_t* pBuf, uint32_t len) {
    uint32_t cmd_index;
    uint32_t cmdID;
    uint32_t status = 0;
    if (len >= 5) {
        cmdID = ((ota_download_cmd_pd*)(&pBuf[5]))->command_id;
        uint8_t timeoutNum = 0;

        log_debug(
            "------------------------ >>>> GW      ------------------------");
        log_info_hexdump("  ", pBuf, len);

        cmd_index = cmdID;
        ota_download_cmd_pd* pt_pd = (ota_download_cmd_pd*)&pBuf[5];

        if (cmd_index >= 0xF0000000 && cmd_index <= 0xF0000002) {
            ota_download_handle(cmd_index, pt_pd->parameter);
        }
    }
}

uart_handler_data_sts_t ota_download_cmd_parser(uint8_t* pBuf, uint16_t plen,
                                                uint16_t* datalen,
                                                uint16_t* offset) {
    //+-------------+-----------+---------------+------------+-----------------+--------------+--------+
    //|  Header(4)  | Length(1) | Command ID(4) | Address(2) | Address Mode(1) | Parameter(N) | CS (1) |
    //+-------------+-----------+---------------+------------+-----------------+--------------+--------+
    //| FF FC FC FF |           |               |            |                 |              |        |
    //+-------------+-----------+---------------+------------+-----------------+--------------+--------+
    uart_handler_data_sts_t t_return = UART_DATA_INVALID;

    uint16_t idx = 0;
    uint8_t cs = 0;

    uint8_t find = 0;
    uint16_t totalLen = 0;

    ota_download_cmd_hdr* hdr = NULL;
    ota_download_cmd_end* end = NULL;
    /* find tag */
    if (plen < 5) {
        return t_return;
    }

    if ((pBuf[0] == 0xFF) && (pBuf[1] == 0xFC) && (pBuf[2] == 0xFC)
        && (pBuf[3] == 0xFF)) {
        idx = 0;
        find = 1;
    }

    if (!find) {
        /* 4+ bytes present but header is wrong — buffer is corrupt, flush */
        return (plen >= 4) ? UART_DATA_CS_ERROR : t_return;
    }

    if ((plen - idx) < (sizeof(ota_download_cmd_hdr)
                        + sizeof(ota_download_cmd_end))) {
        return t_return;
    }

    hdr = (ota_download_cmd_hdr*)(pBuf + idx);
    if (plen < (idx + sizeof(ota_download_cmd_hdr) + hdr->len
                + sizeof(ota_download_cmd_end))) {
        return t_return;
    }

    end = (ota_download_cmd_end*)&pBuf[idx + sizeof(ota_download_cmd_hdr)
                                       + hdr->len];

    *datalen = sizeof(ota_download_cmd_hdr) + hdr->len
               + sizeof(ota_download_cmd_end);
    *offset = idx;

    cs = _gateway_checksum_calc(&pBuf[idx + 4], (hdr->len + 1));
    if (cs != end->cs) {
        log_error("Checksum error: expected 0x%x, got 0x%x", cs, end->cs);
        return UART_DATA_CS_ERROR;
    }

    return UART_DATA_VALID;
}
