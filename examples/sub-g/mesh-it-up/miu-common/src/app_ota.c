/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */
#include <openthread/coap.h>
#include <openthread/platform/misc.h>
#include <openthread/random_noncrypto.h>
#include <openthread/thread_ftd.h>
#include "uart.h"

#include "FreeRTOS.h"
#include "app_ota.h"
#include "app_uart.h"
#include "cli.h"
#include "fota_define.h"
#include "hosal_flash.h"
#include "log.h"
#include "main.h"
#include "miu_bin_version.h"
#include "miu_ext_mem.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "util_string.h"

#include <string.h>

#if CONFIG_APP_TASK_OTA_ENABLE
unsigned int ota_debug_flags = 0;
#define ota_printf(args...)                                                    \
    do {                                                                       \
        if (ota_debug_flags > 0)                                               \
            log_info(args);                                                    \
    } while (0);

#define IS_VALID_PTR(ptr) \
    ((ptr) != NULL && (ptr) != (void*)0xFFFFFFFF && (((uint32_t)(ptr) & 0x3) == 0))

static void ota_response_table_handler(uint16_t* req_table);
static void ota_change_state_and_timer(uint8_t state, uint32_t timeout);
static void (*ota_state_change_cb_signal)(uint8_t state) = NULL;
static void ota_status_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens);
static xQueueHandle ota_event_queue;

static TimerHandle_t ota_timer;
static TimerHandle_t ota_resp_timer;
static TimerHandle_t ota_report_timer;
otIp6Address ota_invalid_addr;
otIp6Address ota_sender_addr;
otCoapResource ota_data_resource;
otCoapResource ota_req_resource;
otCoapResource ota_resp_resource;
otCoapResource ota_rxmode_resource;
otCoapResource ota_status_resource;
otCoapResource ota_execute_resource;
const char ota_data_Uri_Path[] = RAFAEL_OTA_URL_DATA;
const char ota_req_Uri_Path[] = RAFAEL_OTA_URL_REQ;
const char ota_resp_Uri_Path[] = RAFAEL_OTA_URL_RESP;
const char ota_rxmode_Uri_Path[] = RAFAEL_OTA_URL_RXMODE;
const char ota_status_Uri_Path[] = RAFAEL_OTA_URL_STATUS;
const char ota_execute_Uri_Path[] = RAFAEL_OTA_URL_EXECUTE;
static volatile uint8_t g_ota_state = OTA_IDLE;
static uint32_t* g_ota_bitmap = NULL;
static uint8_t g_ota_request_cnt = 0;
static uint8_t g_ota_unicast_try_cnt = 0;
static uint32_t g_ota_request_last_remain = 0;
static uint16_t g_ota_segments_size = 0;
static uint16_t g_ota_data_intervel = 0;
static uint32_t g_ota_image_version = 0;
static uint32_t g_ota_image_size = 0;
static uint32_t g_ota_image_crc = 0;
static uint32_t g_ota_total_num = 0;
static uint32_t g_ota_start_index = 0;
static uint32_t g_ota_last_index = 0;
static bool have_data_ack = false;
static bool need_reboot = true;
static uint8_t g_ota_response_k = 2;
static uint8_t g_ota_response_c = 0;
static uint16_t g_resp_table[OTA_RESPONSE_TABLE_SIZE];
ota_status_report_t g_ota_status_report;

uint32_t crc32checksum(uint32_t flash_offset, uint32_t data_len) {
    uint32_t chkSum = ~0;
    uint8_t page_buf[256] __attribute__((aligned(4)));
    uint32_t processed_len = 0;

    while (processed_len < data_len) {
        /* flash_read_page reads 256 bytes from the 256-byte-aligned page
         * containing the given address. Compute the page base and the byte
         * offset within that page so we read from the correct position. */
        uint32_t cur_addr = flash_offset + processed_len;
        uint32_t page_base = cur_addr & ~0xFFu;
        uint32_t page_off = cur_addr - page_base;
        uint32_t available = 256u - page_off;
        uint32_t read_len = ((data_len - processed_len) > available)
                                ? available
                                : (data_len - processed_len);

        if (hosal_flash_read(HOSAL_FLASH_READ_PAGE, page_base, page_buf) != 0) {
            return 0;
        }

        for (uint32_t i = 0; i < read_len; i++) {
            chkSum ^= page_buf[page_off + i];
            for (uint16_t k = 0; k < 8; k++) {
                chkSum = (chkSum & 1) ? (chkSum >> 1) ^ 0xedb88320
                                      : (chkSum >> 1);
            }
        }
        processed_len += read_len;
    }

    return ~chkSum;
}

/*bit map*/
uint32_t* ota_bitmap_init(uint32_t lens) {
    uint16_t bitmap_size;
    uint32_t* bitmap = NULL;
    bitmap_size = (lens >> 5);
    if (lens % 32) {
        ++bitmap_size;
    }

    ota_printf("lens %d bitmap_size %d ", lens, bitmap_size);
    bitmap_size *= sizeof(uint32_t);

    bitmap = xMalloc(bitmap_size);
    if (!bitmap) {
        log_error("[FOTA] bitmap xMalloc failed, size=%u", bitmap_size);
        return NULL;
    }
    memset(bitmap, 0x0, bitmap_size);
    return bitmap;
}

void ota_bitmap_set(uint32_t* bitmap, uint32_t index) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in set: %p", bitmap);
        return;
    }
    enter_critical_section();
    uint32_t tmp_index = 0, bit = 0;
    tmp_index = index >> 5;
    bit = index % 32;

    bitmap[tmp_index] |= 0x1 << bit;
    leave_critical_section();
}

void ota_bitmap_delete(uint32_t* bitmap) {
    if (bitmap == NULL) {
        return;
    }
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in delete: %p", bitmap);
        return; 
    }
    xFree(bitmap);
}

uint32_t ota_bitmap_get_bit(uint32_t* bitmap, uint32_t index) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in get_bit: %p", bitmap);
        ota_change_state_and_timer(OTA_IDLE, 0);
        return 0;
    }
    uint32_t tmp_index = index >> 5, bit = 0;

    bit = index % 32;
    return (bitmap[tmp_index] & (0x1 << bit));
}

uint32_t ota_bitmap_get_remain(uint32_t* bitmap, uint32_t lens) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in get_remain: %p", bitmap);
        ota_change_state_and_timer(OTA_IDLE, 0);
        return 0;
    }
    uint32_t tmp_lens = lens >> 5, i = 0, j = 0, remain = 0;
    if (lens % 32) {
        ++tmp_lens;
    }

    for (i = 0; i < tmp_lens; i++) {
        if ((i == (tmp_lens - 1) && (lens % 32) != 0)) {
            for (j = 0; j < (lens % 32); j++) {
                if ((bitmap[i] & (0x1 << j)) == 0) {
                    ++remain;
                }
            }
        } else {
            for (j = 0; j < 32; j++) {
                if ((bitmap[i] & (0x1 << j)) == 0) {
                    ++remain;
                }
            }
        }
    }
    return remain;
}

void ota_bitmap_print(uint32_t* bitmap, uint32_t lens) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in print: %p", bitmap);
        return;
    }

    uint32_t tmp_lens = lens >> 5, i = 0;
    if (lens % 32) {
        ++tmp_lens;
    }

    for (i = 0; i < tmp_lens; i++) {
        ota_printf("%X ", bitmap[i]);
    }
    ota_printf("");
}

static void ota_change_state_and_timer(uint8_t state, uint32_t timeout) {
    xTimerStop(ota_timer, 0);
    if (state == OTA_IDLE) {
        ota_bitmap_delete(g_ota_bitmap);
        g_ota_bitmap = NULL;
        g_ota_segments_size = 0;
        g_ota_data_intervel = 0;
        g_ota_total_num = 0;
        g_ota_start_index = 0;
        g_ota_request_cnt = 0;
        g_ota_unicast_try_cnt = 0;
        g_ota_request_last_remain = 0;
        g_ota_last_index = 0;
        have_data_ack = false;
    }
    if (timeout > 0) {
        xTimerChangePeriod(ota_timer, pdMS_TO_TICKS(timeout), 0);
    }

    if (g_ota_state != state) {
        g_ota_state = state;
        if (ota_state_change_cb_signal) {
            ota_state_change_cb_signal(g_ota_state);
        }
    }
}

uint32_t ota_get_image_version() { return g_ota_image_version; }

void ota_set_image_version(uint32_t version) { g_ota_image_version = version; }

uint32_t ota_get_image_size() { return g_ota_image_size; }

void ota_set_image_size(uint32_t size) { g_ota_image_size = size; }

uint32_t ota_get_image_crc() { return g_ota_image_crc; }

void ota_set_image_crc(uint32_t crc) { g_ota_image_crc = crc; }

uint8_t ota_get_state() { return g_ota_state; }

uint8_t ota_reset() {
    uint8_t ret = 0;
    if (g_ota_state == OTA_IDLE) {
        ota_set_image_version(0);
        ota_set_image_size(0);
        ota_set_image_crc(0);
        for (int i = 0; i < OTA_MAX_IMAGE_SECTORS; i++) {
            // Page erase (4096 bytes)
            hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR,
                              (OTA_FLASH_START + (0x1000 * i)));
        }
    } else {
        ret = 1;
    }
    return ret;
}

const char* OtaStateToString(ota_state_t state) {
    static const char* const kOtaStateStrings[] = {
        "Idle",           "DataSending", "DataReceiving", "UnicastReceiving",
        "RequestSending", "Done",        "Reboot",
    };

    return ((state > OTA_REBOOT) || (state < OTA_IDLE))
               ? "invalid"
               : kOtaStateStrings[state - OTA_IDLE];
}

void ota_bootloader_info_check() {
    uint32_t file_ver, crc32;
    fota_information_t t_bootloader_ota_info = {0};
    hosal_flash_read_n_bytes(FOTA_UPDATE_BANK_INFO_ADDRESS,
                             (uint8_t*)&t_bootloader_ota_info,
                             sizeof(t_bootloader_ota_info));
    file_ver = (uint32_t)t_bootloader_ota_info.reserved[0];

    do {
        if (t_bootloader_ota_info.fotabank_ready == FOTA_IMAGE_READY) {
            if (t_bootloader_ota_info.fota_result == FOTA_RESULT_SUCCESS) {
                ota_set_image_version(file_ver);
                ota_set_image_size((t_bootloader_ota_info.fotabank_datalen
                                    + OTA_INFO_HEADER_OFFSET));
                ota_set_image_crc(t_bootloader_ota_info.fotabank_crc);
            } else {
                log_warn("[FOTA] OTA result: 0x%02X", t_bootloader_ota_info.fota_result);
            }
            t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY + 1;
            t_bootloader_ota_info.fotabank_startaddr = 0;
            t_bootloader_ota_info.fotabank_datalen = 0;
            t_bootloader_ota_info.fotabank_crc = 0;
        } else {
            if (t_bootloader_ota_info.fotabank_startaddr
                    == (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET)
                && t_bootloader_ota_info.fotabank_datalen
                       <= SIZE_OF_FOTA_BANK_1MB) {
                uint32_t first_word = 0xFFFFFFFF;
                hosal_flash_read_n_bytes(t_bootloader_ota_info.fotabank_startaddr,
                                         (uint8_t*)&first_word, sizeof(first_word));
                if (first_word == 0xFFFFFFFF) {
                    /* OTA buffer was erased (e.g. ISP re-flash) — silent cleanup */
                    t_bootloader_ota_info.fotabank_startaddr = 0;
                    t_bootloader_ota_info.fotabank_datalen = 0;
                    t_bootloader_ota_info.fotabank_crc = 0;
                } else {
                    crc32 = crc32checksum(t_bootloader_ota_info.fotabank_startaddr,
                                          t_bootloader_ota_info.fotabank_datalen);
                    if (crc32 == t_bootloader_ota_info.fotabank_crc) {
                        ota_set_image_version(file_ver);
                        ota_set_image_size((t_bootloader_ota_info.fotabank_datalen
                                            + OTA_INFO_HEADER_OFFSET));
                        ota_set_image_crc(t_bootloader_ota_info.fotabank_crc);
                    } else {
                        log_warn("ota crc fail %04X %04X", crc32,
                                  t_bootloader_ota_info.fotabank_crc);
                        ota_reset();
                        t_bootloader_ota_info.fotabank_startaddr = 0;
                        t_bootloader_ota_info.fotabank_datalen = 0;
                        t_bootloader_ota_info.fotabank_crc = 0;
                    }
                }
                t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY + 1;
            }
        }
        enter_critical_section();
        hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR,
                          FOTA_UPDATE_BANK_INFO_ADDRESS);
        hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, FOTA_UPDATE_BANK_INFO_ADDRESS,
                          (uint8_t*)&t_bootloader_ota_info);
        leave_critical_section();
    } while (0);

    log_info("ota ver             : 0x%08x ", ota_get_image_version());
}

static void ota_reboot_handler() {
    ota_printf("Reboot");
    otPlatReset(NULL);
}

static void ota_bootinfo_ready() {
    uint32_t program_data_u32[256 / 4] __attribute__((aligned(4)));
    uint8_t* const program_data = (uint8_t*)program_data_u32;
    fota_information_t t_bootloader_ota_info = {0};

    hosal_flash_read_n_bytes(FOTA_UPDATE_BANK_INFO_ADDRESS,
                             (uint8_t*)&t_bootloader_ota_info,
                             sizeof(t_bootloader_ota_info));

    t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY;

    memset(program_data, 0xFF, 256);
    memcpy(program_data, (uint8_t*)&t_bootloader_ota_info, sizeof(fota_information_t));

    enter_critical_section();
    while (flash_check_busy());
    flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);

    while (flash_check_busy());

    flash_write_page((uint32_t)program_data, FOTA_UPDATE_BANK_INFO_ADDRESS);
    while (flash_check_busy());
    leave_critical_section();

    ota_printf("bootinfo ready");
}

void ota_bootinfo_reset() {
    uint32_t program_data_u32[256 / 4] __attribute__((aligned(4)));
    uint8_t* const program_data = (uint8_t*)program_data_u32;
    fota_information_t t_bootloader_ota_info = {0};

    uint32_t img_size = ota_get_image_size();
    uint32_t img_ver = ota_get_image_version();
    uint32_t img_crc = ota_get_image_crc();

    if (img_size == 0xFFFFFFFF || img_ver == 0xFFFFFFFF || img_crc == 0xFFFFFFFF || img_size == 0) {
        log_error("[FOTA] Core param corrupt! Flash write rejected to prevent bricking. Size:0x%X, Ver:0x%X", 
                  (unsigned int)img_size, (unsigned int)img_ver);
        return; 
    }

    t_bootloader_ota_info.fotabank_startaddr = (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET);
    t_bootloader_ota_info.signature_len = 0;
    t_bootloader_ota_info.target_startaddr = APP_START_ADDRESS;
    t_bootloader_ota_info.fotabank_datalen = img_size - OTA_INFO_HEADER_OFFSET;
    t_bootloader_ota_info.fota_image_info = FOTA_IMAGE_INFO_COMPRESSED;
    t_bootloader_ota_info.reserved[0] = img_ver;
    t_bootloader_ota_info.fota_result = 0xFF;
    t_bootloader_ota_info.fotabank_crc = img_crc;

    memset(program_data, 0xFF, 256);
    memcpy(program_data, (uint8_t*)&t_bootloader_ota_info, sizeof(fota_information_t));

    enter_critical_section();
    while (flash_check_busy());
    flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);

    while (flash_check_busy());
    flash_write_page((uint32_t)program_data, FOTA_UPDATE_BANK_INFO_ADDRESS);
    while (flash_check_busy());
    leave_critical_section();

    ota_printf("bootinfo reset status success");
}

void ota_flash_read_bin_type(uint8_t out_bin_type[12]) {
    /* Must be 4-byte aligned for DMA (same requirement as ota_download_cmd_handler). */
    static uint32_t read_buf_u32[OTA_FLASH_SECTOR_SIZE / 4];
    uint8_t* const read_buf = (uint8_t*)read_buf_u32;
    while (flash_check_busy())
        ;
    flash_read_page((uint32_t)(read_buf), FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB);
    while (flash_check_busy())
        ;
    memcpy(out_bin_type, read_buf + 4, 12);
}

static void ota_check_bin_tpye() {
    const uint8_t* self_bin_type = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
    uint8_t img_bin_type[12];
    ota_flash_read_bin_type(img_bin_type);

    uint8_t zero_type[12] = {0};
    if (memcmp(img_bin_type, self_bin_type, 12) != 0
        && memcmp(img_bin_type, zero_type, 12) != 0) {
        log_info("bin type different ");
        log_info_hexdump("self : ", self_bin_type, 12);
        log_info_hexdump("form ota : ", img_bin_type, 12);
        /* ota_reset() only clears version/size/crc when state == OTA_IDLE.
         * Switch to IDLE first so ota_reset() actually takes effect. */
        ota_change_state_and_timer(OTA_IDLE, 0);
        ota_reset();
    } else {
        // wait ota done after reboot
        ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
    }
}

static void ota_event_queue_push(uint8_t event, uint8_t* data,
                                 uint16_t data_lens, otIp6Address ipv6) {
    ota_event_data_t push_event_data;
    if (data_lens > OTA_EVENT_DATA_MAX_SIZE) {
        ota_printf("ota_event_queue_push data_lens exceed %u ", data_lens);
        leave_critical_section();
        return;
    }
    push_event_data.data = xMalloc(data_lens);
    if (push_event_data.data) {
        enter_critical_section();
        push_event_data.event = event;
        memcpy(push_event_data.data, data, data_lens);
        push_event_data.data_lens = data_lens;
        memcpy((void*)&push_event_data.ipv6, (void*)&ipv6,
               sizeof(otIp6Address));
        leave_critical_section();
        if (xQueueSend(ota_event_queue, &push_event_data, 0) == pdPASS) {
            ot_app_task_post(ota_event_handler, NULL);
        } else {
            xFree(push_event_data.data);
            log_warn("ota_event_queue full, drop packet \r\n");
        }
    } else {
        log_warn("ota_event_queue_push alloc fail \r\n");
    }
}

static uint16_t calc_payload_data_len(uint32_t seq, uint32_t size,
                                      uint16_t segments) {
    uint32_t total_num = (size + segments - 1) / segments;
    uint16_t len = (seq == total_num - 1) ? (uint16_t)(size % segments)
                                          : segments;
    return len ? len : segments;
}

static int ota_data_parse(uint8_t type, uint8_t* payload,
                          uint16_t payloadlength, void* data) {
    if (payload == NULL || data == NULL)
        return 1;

    uint8_t* tmp = payload;
    uint16_t expected_data_lens = 0;

    switch (type) {
        case OTA_PAYLOAD_TYPE_DATA: {
            ota_data_t* dest = (ota_data_t*)data;
            // 1. Define the header size (excluding the last data array).
            const uint16_t header_size = offsetof(ota_data_t, data);
            if (payloadlength < header_size)
                return 1;

            // 2. One-time copy of Header
            memcpy(dest, tmp, header_size);
            tmp += header_size;

            // 3. Calculate the required data length for this package.
            expected_data_lens = calc_payload_data_len(dest->seq, dest->size,
                                                       dest->segments);

            // 4. Copy the remaining payload data
            if (header_size + expected_data_lens > payloadlength)
                return 1;
            memcpy(dest->data, tmp, expected_data_lens);
            tmp += expected_data_lens;
            break;
        }

        case OTA_PAYLOAD_TYPE_DATA_ACK: {
            const uint16_t ack_size = sizeof(ota_data_ack_t); // 7 bytes
            if (payloadlength < ack_size)
                return 1;
            memcpy(data, tmp, ack_size);
            tmp += ack_size;
            break;
        }

        case OTA_PAYLOAD_TYPE_REQUEST: {
            ota_request_t* dest = (ota_request_t*)data;
            const uint16_t req_header_size = offsetof(
                ota_request_t, req_table); // version(4)+size(4)+segments(2)
            if (payloadlength
                < (req_header_size + (OTA_REQUEST_TABLE_SIZE * 2)))
                return 1;

            memcpy(dest, tmp, req_header_size);
            tmp += req_header_size;
            memcpy(dest->req_table, tmp, (OTA_REQUEST_TABLE_SIZE * 2));
            tmp += (OTA_REQUEST_TABLE_SIZE * 2);
            break;
        }

        case OTA_PAYLOAD_TYPE_RESPONSE: {
            ota_response_t* dest = (ota_response_t*)data;
            const uint16_t res_header_size = offsetof(ota_response_t,
                                                      data); // no data[0]
            if (payloadlength < res_header_size)
                return 1;

            memcpy(dest, tmp, res_header_size);
            tmp += res_header_size;

            expected_data_lens = calc_payload_data_len(dest->seq, dest->size,
                                                       dest->segments);

            if (res_header_size + expected_data_lens > payloadlength)
                return 1;
            memcpy(dest->data, tmp, expected_data_lens);
            tmp += expected_data_lens;
            break;
        }

        default: ota_printf("unknown parse type %u", type); return 1;
    }

    // payloadlength check
    if ((uint16_t)(tmp - payload) != payloadlength) {
        ota_printf("parse len mismatch: %u/%u", (uint16_t)(tmp - payload),
                   payloadlength);
        return 1;
    }
    return 0;
}

static void ota_data_piece(uint8_t type, uint8_t* payload,
                           uint16_t* payloadlength, void* data) {
    if (payload == NULL || data == NULL || payloadlength == NULL)
        return;

    uint8_t* tmp = payload;
    uint16_t data_lens = 0;

    switch (type) {
        case OTA_PAYLOAD_TYPE_DATA: {
            ota_data_t* src = (ota_data_t*)data;
            const uint16_t header_size = offsetof(ota_data_t, data);

            memcpy(tmp, src, header_size);
            tmp += header_size;

            data_lens = calc_payload_data_len(src->seq, src->size,
                                              src->segments);
            memcpy(tmp, src->data, data_lens);
            tmp += data_lens;
            break;
        }

        case OTA_PAYLOAD_TYPE_DATA_ACK: {
            memcpy(tmp, data, sizeof(ota_data_ack_t));
            tmp += sizeof(ota_data_ack_t);
            break;
        }

        case OTA_PAYLOAD_TYPE_REQUEST: {
            ota_request_t* src = (ota_request_t*)data;
            const uint16_t req_header_size = offsetof(ota_request_t, req_table);
            memcpy(tmp, src, req_header_size);
            tmp += req_header_size;
            memcpy(tmp, src->req_table, (OTA_REQUEST_TABLE_SIZE * 2));
            tmp += (OTA_REQUEST_TABLE_SIZE * 2);
            break;
        }

        case OTA_PAYLOAD_TYPE_RESPONSE: {
            ota_response_t* src = (ota_response_t*)data;
            const uint16_t res_header_size = offsetof(ota_response_t, data);
            memcpy(tmp, src, res_header_size);
            tmp += res_header_size;

            data_lens = calc_payload_data_len(src->seq, src->size,
                                              src->segments);
            memcpy(tmp, src->data, data_lens);
            tmp += data_lens;
            break;
        }

        default: ota_printf("unknown piece type %u", type); break;
    }

    *payloadlength = (uint16_t)(tmp - payload);
}

/*coap proccess*/
static void ota_coap_data_proccess(void* aContext, otMessage* aMessage,
                                   const otMessageInfo* aMessageInfo) {
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    uint8_t* buf = NULL;
    uint16_t length, payloadlength = 0;
    otMessage* responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_EMPTY;
    ota_data_t ota_data;
    ota_data_ack_t ota_data_ack;
    otError error = OT_ERROR_NONE;
    uint8_t* payload = NULL;

    otIp6AddressToString(&aMessageInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    do {
        if (length > 0) {
            buf = xMalloc(length);
            if (NULL != buf) {
                otMessageRead(aMessage, otMessageGetOffset(aMessage), buf,
                              length);
                ota_event_queue_push(OTA_DATA_RECEIVE_EVENT, buf, length,
                                     aMessageInfo->mPeerAddr);
                if (otCoapMessageGetType(aMessage)
                    == OT_COAP_TYPE_CONFIRMABLE) {
                    if (ota_data_parse(OTA_PAYLOAD_TYPE_DATA, buf, length,
                                       &ota_data)) {
                        break;
                    }
                    /*do ack packet*/
                    responseCode = OT_COAP_CODE_VALID;
                    responseMessage = otCoapNewMessage(otrGetInstance(), NULL);
                    if (responseMessage == NULL) {
                        error = OT_ERROR_NO_BUFS;
                        break;
                    }
                    error = otCoapMessageInitResponse(
                        responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT,
                        responseCode);
                    if (error != OT_ERROR_NONE) {
                        break;
                    }
                    ota_data_ack.data_type = OTA_PAYLOAD_TYPE_DATA_ACK;
                    ota_data_ack.version = ota_data.version;
                    ota_data_ack.seq = ota_data.seq;
                    payload = xMalloc(sizeof(ota_data_ack_t));
                    if (payload) {
                        ota_data_piece(OTA_PAYLOAD_TYPE_DATA_ACK, payload,
                                       &payloadlength, &ota_data_ack);
                        error = otCoapMessageSetPayloadMarker(responseMessage);
                        if (error != OT_ERROR_NONE) {
                            break;
                        }
                        error = otMessageAppend(responseMessage, payload,
                                                payloadlength);
                        if (error != OT_ERROR_NONE) {
                            break;
                        }
                    }
                    error = otCoapSendResponseWithParameters(
                        otrGetInstance(), responseMessage, aMessageInfo, NULL);
                    if (error != OT_ERROR_NONE) {
                        break;
                    }
                }
            }
        }
    } while (0);
    if (buf) {
        xFree(buf);
    }
    if (payload) {
        xFree(payload);
    }
    if (error != OT_ERROR_NONE && responseMessage != NULL) {
        otMessageFree(responseMessage);
    }
}

static void ota_coap_generic_process(otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo,
                                     uint8_t event_type) {
    uint16_t length = otMessageGetLength(aMessage)
                      - otMessageGetOffset(aMessage);
    if (length > 0) {
        uint8_t* buf = xMalloc(length);
        if (buf != NULL) {
            otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, length);

            ota_event_queue_push(event_type, buf, length,
                                 aMessageInfo->mPeerAddr);

            xFree(buf);
        } else {
            log_warn("CoAP process xMalloc fail\r\n");
        }
    }
}

static void ota_coap_request_proccess(void* aContext, otMessage* aMessage,
                                      const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_REQUEST_RECEIVE_EVENT);
}

static void ota_coap_response_proccess(void* aContext, otMessage* aMessage,
                                       const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo,
                             OTA_RESPONSE_RECEIVE_EVENT);
}

static void ota_coap_rxmode_proccess(void* aContext, otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_RXMODE_RECEIVE_EVENT);
}

static void ota_coap_status_proccess(void* aContext, otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_STATUS_RECEIVE_EVENT);
}

static void ota_coap_execute_proccess(void* aContext, otMessage* aMessage,
                                      const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_EXECUTE_RECEIVE_EVENT);
}

static void ota_coap_ack_process(void* aContext, otMessage* aMessage,
                                 const otMessageInfo* aMessageInfo,
                                 otError aResult) {
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    uint8_t* buf = NULL;
    uint16_t length;
    ota_data_ack_t ota_data_ack;
    otIp6AddressToString(&aMessageInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    do {
        if (length > 0) {
            buf = xMalloc(length);
            if (NULL != buf) {
                otMessageRead(aMessage, otMessageGetOffset(aMessage), buf,
                              length);
                if (buf[0] == OTA_PAYLOAD_TYPE_DATA_ACK) {
                    if (ota_data_parse(OTA_PAYLOAD_TYPE_DATA_ACK, buf, length,
                                       &ota_data_ack)) {
                        break;
                    }
                    if (ota_data_ack.version != ota_get_image_version()) {
                        ota_printf("ota_data_ack version %x %x",
                                   ota_data_ack.version,
                                   ota_get_image_version());
                        break;
                    }
                    ota_printf("[r] ota ack : %d/%d ", ota_data_ack.seq,
                               g_ota_start_index);
                    if (ota_data_ack.seq == g_ota_start_index) {
                        have_data_ack = true;
                        ota_change_state_and_timer(OTA_DATA_SENDING, 1);
                    }
                }
            }
        }
    } while (0);

    if (buf) {
        xFree(buf);
    }
}

otError ota_coap_request(otCoapCode aCoapCode, otIp6Address coapDestinationIp,
                         otCoapType coapType, uint8_t* payload,
                         uint16_t payloadLength, const char* coap_Path) {
    otError error = OT_ERROR_NONE;
    otMessage* message = NULL;
    otMessageInfo messageInfo;

    // Default parameters

    do {
        message = otCoapNewMessage(otrGetInstance(), NULL);
        if (NULL == message) {
            error = OT_ERROR_NO_BUFS;
            break;
        }
        otCoapMessageInit(message, coapType, aCoapCode);
        otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);

        error = otCoapMessageAppendUriPathOptions(message, coap_Path);
        if (OT_ERROR_NONE != error) {
            break;
        }

        if (payloadLength > 0) {
            error = otCoapMessageSetPayloadMarker(message);
            if (OT_ERROR_NONE != error) {
                break;
            }
        }

        // Embed content into message if given
        if (payloadLength > 0) {
            error = otMessageAppend(message, payload, payloadLength);
            if (OT_ERROR_NONE != error) {
                break;
            }
        }

        memset(&messageInfo, 0, sizeof(messageInfo));
        messageInfo.mPeerAddr = coapDestinationIp;
        messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

        if ((coapType == OT_COAP_TYPE_CONFIRMABLE)
            || (aCoapCode == OT_COAP_CODE_GET)) {
            error = otCoapSendRequestWithParameters(
                otrGetInstance(), message, &messageInfo, &ota_coap_ack_process,
                NULL, NULL);
        } else {
            error = otCoapSendRequestWithParameters(
                otrGetInstance(), message, &messageInfo, NULL, NULL, NULL);
        }
    } while (0);

    if ((error != OT_ERROR_NONE) && (message != NULL)) {
        otMessageFree(message);
    }
    return error;
}

static void ota_data_send_handler() {
    ota_data_t ota_data;
    uint8_t* payload = NULL;
    uint16_t payloadlength = 0, ota_data_lens = 0, i = 0;
    uint32_t start_addr;
    bool is_unicst = false;
    do {
        if (g_ota_total_num == 0) {
            ota_printf("g_ota_total_num 0");
            ota_change_state_and_timer(OTA_IDLE, 0);
            break;
        }
        if (ota_sender_addr.mFields.m8[0] != 0xff
            && ota_sender_addr.mFields.m8[1] != 0x03) {
            is_unicst = true;
        }
        if (g_ota_start_index < (g_ota_total_num - 1)) {
            ota_data_lens = g_ota_segments_size;
        } else if (g_ota_start_index == (g_ota_total_num - 1)) {
            ota_data_lens = (ota_get_image_size() % g_ota_segments_size);
        } else {
            if (is_unicst == true) {
                ota_change_state_and_timer(OTA_IDLE, 0);
            } else {
                ota_printf("============wait for ota request============");
                ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
            }
            break;
        }

        if (is_unicst == true) {
            if (have_data_ack == true) {
                ++g_ota_start_index;
            }
        }

        if (g_ota_start_index == g_ota_last_index) {
            ++g_ota_unicast_try_cnt;
            if (g_ota_unicast_try_cnt >= 10) {
                ota_printf("try to max %d ", g_ota_unicast_try_cnt);
                ota_change_state_and_timer(OTA_IDLE, 0);
                break;
            }
        } else {
            g_ota_last_index = g_ota_start_index;
            g_ota_unicast_try_cnt = 0;
        }

        ota_data.version = ota_get_image_version();
        ota_data.size = ota_get_image_size();
        ota_data.crc = ota_get_image_crc();
        ota_data.seq = g_ota_start_index;
        ota_data.segments = g_ota_segments_size;
        ota_data.intervel = g_ota_data_intervel;
        ota_data.is_unicast = is_unicst;
        enter_critical_section();
        start_addr = (OTA_FLASH_START + (ota_data.seq * ota_data.segments));
        for (int i = 0; i < ota_data_lens; i++) {
            hosal_flash_read(HOSAL_FLASH_READ_BYTE, (start_addr + i),
                             &ota_data.data[i]);
        }
        leave_critical_section();

        payload = xMalloc(sizeof(ota_data_t));
        if (payload) {
            memset(payload, 0x0, sizeof(ota_data_t));
            ota_data_piece(OTA_PAYLOAD_TYPE_DATA, payload, &payloadlength,
                           &ota_data);
            ota_event_queue_push(OTA_DATA_SEND_EVENT, payload, payloadlength,
                                 ota_invalid_addr);
            ota_printf("[s] ota data: [%3d%%] [%u/%u] ",
                       ((ota_data.seq * 100) / (g_ota_total_num - 1)),
                       ota_data.seq, (g_ota_total_num - 1));
            if (is_unicst == true) {
                have_data_ack = false;
            } else {
                g_ota_start_index++;
            }
        }
        ota_change_state_and_timer(OTA_DATA_SENDING, g_ota_data_intervel);
    } while (0);

    if (payload) {
        xFree(payload);
    }
    return;
}

static void ota_response_send_handler() {
    uint8_t* payload = NULL;
    uint8_t random_index = 0;
    uint16_t resp_table_num = 0, i = 0;
    ota_response_t ota_response;
    uint16_t payloadlength = 0, ota_response_data_lens = 0;
    uint32_t start_addr;
    uint32_t toatol_num = (g_ota_image_size / g_ota_segments_size);
    if (g_ota_image_size % g_ota_segments_size) {
        ++toatol_num;
    }

    do {
        for (i = 0; i < OTA_RESPONSE_TABLE_SIZE; i++) {
            if (g_resp_table[i] != 0xFFFF) {
                ++resp_table_num;
            }
        }
        if (resp_table_num == 0) {
            break;
        }
        if (resp_table_num > 1) {
            random_index = otRandomNonCryptoGetUint8InRange(1, resp_table_num);
        } else {
            random_index = 1;
        }
        resp_table_num = 0;

        for (i = 0; i < OTA_RESPONSE_TABLE_SIZE; i++) {
            if (g_resp_table[i] != 0xFFFF) {
                ++resp_table_num;
                if (resp_table_num == random_index) {
                    random_index = i;
                    break;
                }
            }
        }
        if (i >= OTA_RESPONSE_TABLE_SIZE) {
            ota_printf("not find random index %u ", random_index);
            break;
        }
        if (random_index >= OTA_RESPONSE_TABLE_SIZE) {
            ota_printf("random index %u is bit than %u ", random_index,
                       OTA_RESPONSE_TABLE_SIZE);
            break;
        }

        if (0 == ota_bitmap_get_bit(g_ota_bitmap, g_resp_table[random_index])) {
            ota_printf("doesn't have %u ", g_resp_table[random_index]);
            break;
        }

        ota_response.version = g_ota_image_version;
        ota_response.size = g_ota_image_size;
        ota_response.seq = g_resp_table[random_index];
        ota_response.segments = g_ota_segments_size;
        ota_response.crc = g_ota_image_crc;
        if (ota_response.seq != (toatol_num - 1)) {
            ota_response_data_lens = g_ota_segments_size;
        } else {
            ota_response_data_lens = g_ota_image_size % g_ota_segments_size;
        }
        enter_critical_section();
        start_addr = (OTA_FLASH_START
                      + (ota_response.seq * g_ota_segments_size));

        for (int i = 0; i < ota_response_data_lens; i++) {
            hosal_flash_read(HOSAL_FLASH_READ_BYTE, (start_addr + i),
                             &ota_response.data[i]);
        }
        leave_critical_section();

        payload = xMalloc(sizeof(ota_response_t));
        if (payload) {
            memset(payload, 0x0, sizeof(ota_response_t));
            ota_data_piece(OTA_PAYLOAD_TYPE_RESPONSE, payload, &payloadlength,
                           &ota_response);
            ota_event_queue_push(OTA_RESPONSE_SEND_EVENT, payload,
                                 payloadlength, ota_invalid_addr);
            ota_printf("[s] ota response %u %u %08x ", random_index,
                       ota_response.seq, start_addr);
            g_resp_table[random_index] = 0xFFFF;
        }

    } while (0);

    if (payload) {
        xFree(payload);
    }

    resp_table_num = 0;
    for (i = 0; i < OTA_RESPONSE_TABLE_SIZE; i++) {
        if (g_resp_table[i] != 0xFFFF) {
            ++resp_table_num;
        }
    }
    if (!xTimerIsTimerActive(ota_resp_timer) && resp_table_num > 0) {
        uint16_t timeout = otRandomNonCryptoGetUint16InRange(
            1, (OTA_RESPONESE_TIMEOUT));
        if (0 == timeout) {
            timeout = 1;
        }
        xTimerChangePeriod(ota_resp_timer, pdMS_TO_TICKS(timeout), 0);
        xTimerStart(ota_resp_timer, 0);
        g_ota_response_c = 0;
    }
}

static void ota_request_handler() {
    ota_request_t ota_request;
    uint16_t i = 0, remain_index = 0, index = 0;
    uint8_t* payload = NULL;
    uint16_t payloadlength = 0;
    uint32_t timeout = 0, remain = 0, crc32 = 0, intervel = 0, req_index = 0;
    uint32_t toatol_num = (g_ota_image_size / g_ota_segments_size);
    if (g_ota_image_size % g_ota_segments_size) {
        ++toatol_num;
    }
    remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);
    if (remain != 0) {
        do {
            if (g_ota_request_last_remain == remain) {
                ++g_ota_request_cnt;

                if (g_ota_request_cnt > OTA_REQUEST_TRY_MAX) {
                    ota_printf("ota_request fail ");
                    ota_change_state_and_timer(OTA_IDLE, 0);
                    break;
                }
            } else {
                g_ota_request_cnt = 0;
                g_ota_request_last_remain = remain;
            }

            intervel = g_ota_data_intervel;
            if (intervel < 200) {
                intervel = 200;
            }
            timeout = intervel;
            if (remain > OTA_REQUEST_TABLE_SIZE) {
                timeout += otRandomNonCryptoGetUint32InRange(
                    1, (OTA_REQUEST_TABLE_SIZE * OTA_RESPONESE_TIMEOUT));
            } else if (g_ota_request_cnt != 0) {
                timeout = OTA_REQUEST_TABLE_SIZE * OTA_RESPONESE_TIMEOUT;
            } else {
                timeout += otRandomNonCryptoGetUint32InRange(
                    1, (remain * OTA_RESPONESE_TIMEOUT));
            }

            if (timeout == 0) {
                timeout = 1;
            }

            ota_request.version = g_ota_image_version;
            ota_request.size = g_ota_image_size;
            ota_request.segments = g_ota_segments_size;
            memset(ota_request.req_table, 0xff, sizeof(ota_request.req_table));

            req_index = otRandomNonCryptoGetUint32InRange(0, remain);
            ota_printf("[s] req [ ");
            for (i = 0; i < toatol_num; i++) {
                if (0 == ota_bitmap_get_bit(g_ota_bitmap, i)) {
                    if (remain > OTA_REQUEST_TABLE_SIZE) {
                        if ((remain - req_index) < OTA_REQUEST_TABLE_SIZE) {
                            if (remain_index < (OTA_REQUEST_TABLE_SIZE
                                                - (remain - req_index - 1))
                                || (remain_index >= req_index)) {
                                ota_request.req_table[index] = i;
                                ota_printf("%u ", ota_request.req_table[index]);
                                index++;
                            }
                        } else {
                            if ((remain_index >= req_index)) {
                                ota_request.req_table[index] = i;
                                ota_printf("%u ", ota_request.req_table[index]);
                                index++;
                            }
                        }
                        remain_index++;
                    } else {
                        ota_request.req_table[index] = i;
                        ota_printf("%u ", ota_request.req_table[index]);
                        index++;
                    }

                    if (index > OTA_REQUEST_TABLE_SIZE) {
                        break;
                    }
                }
            }
            ota_printf(" ] ");
            payload = xMalloc(sizeof(ota_request_t));
            if (payload) {
                memset(payload, 0x0, sizeof(ota_request_t));
                ota_data_piece(OTA_PAYLOAD_TYPE_REQUEST, payload,
                               &payloadlength, &ota_request);
                ota_event_queue_push(OTA_REQUEST_SEND_EVENT, payload,
                                     payloadlength, ota_invalid_addr);
                ota_printf("[s] ota request rm %u try %u", remain,
                           g_ota_request_cnt);
                ota_change_state_and_timer(OTA_REQUEST_SENDING, timeout);
            } else {
                ota_change_state_and_timer(
                    OTA_REQUEST_SENDING,
                    (OTA_REQUEST_TABLE_SIZE * OTA_RESPONESE_TIMEOUT));
                ota_printf("ota request alloc fail ");
            }
        } while (0);
    } else {
        crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                              (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
        if (crc32 != g_ota_image_crc) {
            ota_printf("ota request upgrade fail %X %X", crc32,
                       g_ota_image_crc);
            ota_change_state_and_timer(OTA_IDLE, 0);
        } else {
            ota_check_bin_tpye();
        }
    }

    if (payload) {
        xFree(payload);
    }
}

static void ota_response_table_handler(uint16_t* req_table) {
    uint16_t timeout = 0, i = 0, j = 0;
    bool need_response = false;
    if (!xTimerIsTimerActive(ota_resp_timer)) {
        memset(g_resp_table, 0xff, sizeof(g_resp_table));
    }
    enter_critical_section();
    for (i = 0; i < OTA_REQUEST_TABLE_SIZE; i++) {
        if (req_table[i] != 0xFFFF) {
            if (ota_bitmap_get_bit(g_ota_bitmap, req_table[i])) {
                for (j = 0; j < OTA_RESPONSE_TABLE_SIZE; j++) {
                    if (g_resp_table[j] == req_table[i]) {
                        break;
                    }
                }
                if (j >= OTA_RESPONSE_TABLE_SIZE) {
                    for (j = 0; j < OTA_RESPONSE_TABLE_SIZE; j++) {
                        if (g_resp_table[j] == 0xFFFF) {
                            g_resp_table[j] = req_table[i];
                            break;
                        }
                    }
                }
            }
        }
    }
    for (j = 0; j < OTA_RESPONSE_TABLE_SIZE; j++) {
        if (g_resp_table[j] != 0xFFFF) {
            need_response = true;
            break;
        }
    }
    leave_critical_section();
    if (need_response == true && !xTimerIsTimerActive(ota_resp_timer)) {
        timeout = otRandomNonCryptoGetUint16InRange(1, (OTA_RESPONESE_TIMEOUT));
        if (0 == timeout) {
            timeout = 1;
        }
        xTimerChangePeriod(ota_resp_timer, pdMS_TO_TICKS(timeout), 0);
        xTimerStart(ota_resp_timer, 0);
        g_ota_response_c = 0;
    }
}

static void ota_request_start() {
    uint32_t timeout = 0, remain = 0;
    uint32_t toatol_num = (g_ota_image_size / g_ota_segments_size);
    if (g_ota_image_size % g_ota_segments_size) {
        ++toatol_num;
    }
    remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);
    if (remain > OTA_REQUEST_TABLE_SIZE) {
        remain = OTA_REQUEST_TABLE_SIZE;
    }
    timeout = otRandomNonCryptoGetUint32InRange(1, (remain * 2000));
    if (timeout == 0) {
        timeout = 1;
    }
    ota_change_state_and_timer(OTA_REQUEST_SENDING, timeout);
}

static void ota_image_received_handler() {
    uint32_t crc32 = 0, remain = 0;
    uint32_t toatol_num = (g_ota_image_size / g_ota_segments_size);

    if (g_ota_image_size % g_ota_segments_size) {
        ++toatol_num;
    }

    remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);

    ota_printf("ota data remain %u (%3d%%) ", remain,
               (((toatol_num - remain) * 100) / (toatol_num - 1)));

    if (remain == 0) {
        crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                              (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
        if (crc32 != g_ota_image_crc) {
            ota_printf("ota data upgrade fail ");
            ota_change_state_and_timer(OTA_IDLE, 0);
            //restart ota request
        }
        //wait request packet
    } else {
        //do the ota request
        ota_request_start();
    }
}

static void ota_unicast_received_handler() {
    uint32_t crc32 = 0, remain = 0;
    uint32_t toatol_num = (g_ota_image_size / g_ota_segments_size);

    if (g_ota_image_size % g_ota_segments_size) {
        ++toatol_num;
    }

    remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);

    ota_printf("ota data remain %u (%3d%%) ", remain,
               ((remain * 100) / (toatol_num - 1)));

    if (remain == 0) {
        crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                              (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
        if (crc32 != g_ota_image_crc) {
            ota_printf("ota data upgrade fail ");
            ota_change_state_and_timer(OTA_IDLE, 0);
            //restart ota request
        }
        //wait request packet
    } else {
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
}

static void ota_response_timer_handler() { ota_response_send_handler(); }

static void ota_report_timer_handler(TimerHandle_t xTimer) {
    ota_status_report_t* ota_status_report =
        (ota_status_report_t*)pvTimerGetTimerID(xTimer);

    ota_status_sended_event_handler(ota_status_report->dstipv6,
                                    (uint8_t*)ota_status_report,
                                    sizeof(ota_status_report_t));
    xTimerDelete(ota_report_timer, 0);
    ota_report_timer = NULL;
}

static void ota_timer_handler() {
    // ota_printf("state %s ",OtaStateToString(g_ota_state));
    switch (g_ota_state) {
        case OTA_DATA_SENDING: ota_data_send_handler(); break;
        case OTA_DATA_RECEIVING: ota_image_received_handler(); break;
        case OTA_UNICAST_RECEIVING: ota_unicast_received_handler(); break;
        case OTA_REQUEST_SENDING: ota_request_handler(); break;
        case OTA_DONE:
            // Waiting for the initialer send execute command
            break;
        case OTA_REBOOT: ota_reboot_handler(); break;
        default: break;
    }
}

static void ota_data_sended_event_handler(uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otIp6Address coapDestinationIp;
    otCoapType coapType;
    char peer_addr[] = "ff03:0:0:0:0:0:0:1";
    if (ota_sender_addr.mFields.m8[0] != 0xff
        && ota_sender_addr.mFields.m8[1] != 0x03) {
        coapDestinationIp = ota_sender_addr;
        coapType = OT_COAP_TYPE_CONFIRMABLE;
    } else {
        error = otIp6AddressFromString(peer_addr, &coapDestinationIp);
        coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    }
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, coapDestinationIp, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_DATA);
}

static void ota_request_sended_event_handler(uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otIp6Address coapDestinationIp;
    char peer_addr[] = "ff02:0:0:0:0:0:0:1";
    error = otIp6AddressFromString(peer_addr, &coapDestinationIp);
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, coapDestinationIp, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_REQ);
}

static void ota_response_sended_event_handler(uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otIp6Address coapDestinationIp;
    char peer_addr[] = "ff02:0:0:0:0:0:0:1";
    error = otIp6AddressFromString(peer_addr, &coapDestinationIp);
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, coapDestinationIp, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_RESP);
}

static void ota_rxmode_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    error = ota_coap_request(CoapCode, dst_ipaddr, coapType, data, lens,
                             RAFAEL_OTA_URL_RXMODE);
}

static void ota_status_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, dst_ipaddr, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_STATUS);
}

static void ota_execute_sended_event_handler(otIp6Address dst_ipaddr,
                                             uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, dst_ipaddr, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_EXECUTE);
}

static void ota_data_received_event_handler(uint8_t* data, uint16_t lens) {
    ota_data_t ota_data;
    uint32_t i = 0;
    uint32_t toatol_num = 0, timeout = 0, ota_data_lens = 0, tmp_addr = 0,
             crc32 = 0;

    do {
        if (ota_data_parse(OTA_PAYLOAD_TYPE_DATA, data, lens, &ota_data)) {
            break;
        }
        toatol_num = (ota_data.size / ota_data.segments);
        if (ota_data.size % ota_data.segments) {
            ++toatol_num;
        }

        if (ota_get_state() == OTA_IDLE
            && ota_get_image_version() == ota_data.version) {
            crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                  (ota_data.size - OTA_INFO_HEADER_OFFSET));
            if (crc32 == ota_get_image_crc()) {
                // Initiator bitmap set all
                if (g_ota_bitmap) {
                    ota_bitmap_delete(g_ota_bitmap);
                    g_ota_bitmap = NULL;
                }
                g_ota_bitmap = ota_bitmap_init(toatol_num);
                if (!g_ota_bitmap) {
                    log_error("[FOTA] bitmap init failed (line ~1429)");
                    ota_change_state_and_timer(OTA_IDLE, 0);
                    break;
                }
                for (i = 0; i < toatol_num; i++) {
                    ota_bitmap_set(g_ota_bitmap, i);
                }
                g_ota_total_num = toatol_num;
                g_ota_segments_size = ota_data.segments;
                ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
                need_reboot = false;
            }
        }

        if (OTA_IDLE != ota_get_state() && 0 != ota_get_image_version()
            && ota_get_image_version() != ota_data.version) {
            ota_printf("different version %08X %08X ", ota_data.version,
                       ota_get_image_version());
            break;
        }

        if (ota_get_state() == OTA_DONE) {
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
            break;
        }

        timeout = 1 + (ota_data.intervel * 2 * (toatol_num - ota_data.seq));
        ota_data_lens = ota_data.segments;
        if (ota_data.seq == (toatol_num - 1)) {
            ota_data_lens = ota_data.size % ota_data.segments;
        }

        if (NULL != g_ota_bitmap) {
            if (g_ota_state == OTA_IDLE) {
                ota_printf("ota data OTA_IDLE");
                break;
            }
            if (ota_bitmap_get_bit(g_ota_bitmap, ota_data.seq)) {
                ota_printf("ota upgrade data same %u", ota_data.seq);
                break;
            }
        } else {
            if (ota_data.is_unicast == true && ota_data.seq != 0) {
                ota_printf("is not seq 0 ");
                break;
            }
            g_ota_image_version = ota_data.version;
            g_ota_image_size = ota_data.size;
            g_ota_image_crc = ota_data.crc;
            g_ota_segments_size = ota_data.segments;
            g_ota_data_intervel = ota_data.intervel;
            ota_bootinfo_reset();
            g_ota_bitmap = ota_bitmap_init(toatol_num);
            if (!g_ota_bitmap) {
                log_error("[FOTA] bitmap init failed");
                ota_change_state_and_timer(OTA_IDLE, 0);
                break;
            }
            if (ota_data.is_unicast == true) {
                ota_change_state_and_timer(OTA_UNICAST_RECEIVING, timeout);
            } else {
                ota_change_state_and_timer(OTA_DATA_RECEIVING, timeout);
            }
            /* Erase outside critical section: each sector takes ~10-20 ms.
             * Holding all IRQs for 87 sectors (~1.7 s) breaks Thread connectivity. */
            for (i = 0; i < OTA_MAX_IMAGE_SECTORS; i++) {
                while (flash_check_busy())
                    ;
                flash_erase(FLASH_ERASE_SECTOR, OTA_FLASH_START + (0x1000 * i));
                while (flash_check_busy())
                    ;
            }
        }

        ota_printf("[R] ota data seq %u remain %u", ota_data.seq,
                   ota_bitmap_get_remain(g_ota_bitmap, toatol_num));

        if (ota_data.seq >= toatol_num) {
            ota_printf("[R] seq %u out of range (total %u), drop", ota_data.seq,
                       toatol_num);
            break;
        }
        ota_bitmap_set(g_ota_bitmap, ota_data.seq);

        enter_critical_section();
        tmp_addr = OTA_FLASH_START + (ota_data.segments * ota_data.seq);
        for (i = 0; i < ota_data_lens; i++) {
            while (flash_check_busy())
                ;
            flash_write_byte(tmp_addr + i, ota_data.data[i]);
            while (flash_check_busy())
                ;
        }
        leave_critical_section();

        if (0 == ota_bitmap_get_remain(g_ota_bitmap, toatol_num)) {
            crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                  (ota_data.size - OTA_INFO_HEADER_OFFSET));
            if (crc32 != ota_data.crc) {
                ota_printf("ota data upgrade fail %X %X", crc32, ota_data.crc);
                ota_change_state_and_timer(OTA_IDLE, 0);
            } else {
                ota_check_bin_tpye();
                if (ota_get_state() == OTA_DONE) {
                    ota_bootinfo_ready();
                    ota_change_state_and_timer(OTA_REBOOT, 1);
                }
            }
            break;
        } else {
            if (ota_data.is_unicast == true) {
                ota_change_state_and_timer(OTA_UNICAST_RECEIVING, timeout);
            } else {
                ota_change_state_and_timer(OTA_DATA_RECEIVING, timeout);
            }
        }

    } while (0);
}

static void ota_request_received_event_handler(uint8_t* data, uint16_t lens) {
    ota_request_t ota_request;
    uint16_t index = 0, req_num = 0;
    uint32_t crc32 = 0, toatol_num = 0, i = 0;
    do {
        if (ota_data_parse(OTA_PAYLOAD_TYPE_REQUEST, data, lens,
                           &ota_request)) {
            break;
        }

        toatol_num = (ota_request.size / ota_request.segments);
        if (ota_request.size % ota_request.segments) {
            ++toatol_num;
        }

        if (ota_get_state() == OTA_IDLE) {
            if (ota_get_image_version() == ota_request.version) {
                crc32 = crc32checksum(
                    (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                    (ota_request.size - OTA_INFO_HEADER_OFFSET));
                if (crc32 == ota_get_image_crc()) {
                    // Initiator bitmap set all
                    ota_printf("same version and idle ");
                    if (g_ota_bitmap) {
                        ota_bitmap_delete(g_ota_bitmap);
                        g_ota_bitmap = NULL;
                    }
                    g_ota_bitmap = ota_bitmap_init(toatol_num);
                    if (!g_ota_bitmap) {
                        log_error("[FOTA] bitmap init failed");
                        ota_change_state_and_timer(OTA_IDLE, 0);
                        break;
                    }
                    for (i = 0; i < toatol_num; i++) {
                        ota_bitmap_set(g_ota_bitmap, i);
                    }
                    g_ota_total_num = toatol_num;
                    g_ota_segments_size = ota_request.segments;
                    ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
                    need_reboot = false;
                } else {
                    break;
                }
            } else {
                ota_printf("ota request OTA_IDLE");
                break;
            }
        }

        if (ota_request.segments != g_ota_segments_size) {
            ota_printf("ota request segments fail %u %u ", ota_request.segments,
                       g_ota_segments_size);
            break;
        }

        if (ota_request.version != g_ota_image_version) {
            ota_printf("ota request differnt version %x %x ",
                       ota_request.version, g_ota_image_version);
            break;
        }

        if (NULL == g_ota_bitmap) {
            ota_printf("ota request bitmap NULL");
            break;
        }
        //have other node not upgrade, need wait
        if (g_ota_state == OTA_DONE) {
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
        }
        ota_printf("[R] res seq : ");
        for (index = 0; index < OTA_REQUEST_TABLE_SIZE; index++) {
            if (ota_request.req_table[index] == 0xFFFF) {
                break;
            }
            ota_printf("%u ", ota_request.req_table[index]);
            ++req_num;
        }
        ota_printf("");
        if (req_num > 0) {
            ota_response_table_handler(ota_request.req_table);
        }
    } while (0);
}

static void ota_response_received_event_handler(uint8_t* data, uint16_t lens) {
    ota_response_t ota_response;
    uint16_t i = 0, ota_data_lens = 0;
    uint32_t toatol_num = 0, tmp_addr = 0, crc32 = 0;
    do {
        if (ota_data_parse(OTA_PAYLOAD_TYPE_RESPONSE, data, lens,
                           &ota_response)) {
            break;
        }
        toatol_num = ota_response.size / ota_response.segments;

        if (ota_response.size % ota_response.segments) {
            ++toatol_num;
        }

        if (g_ota_state == OTA_IDLE) {
            ota_printf("ota response %s", OtaStateToString(g_ota_state));
            if (ota_get_image_version() == ota_response.version) {
                crc32 = crc32checksum(
                    (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                    (ota_response.size - OTA_INFO_HEADER_OFFSET));
                if (crc32 == ota_get_image_crc()) {
                    // Initiator bitmap set all
                    ota_printf("response same version and idle ");
                    if (g_ota_bitmap) {
                        ota_bitmap_delete(g_ota_bitmap);
                        g_ota_bitmap = NULL;
                    }
                    g_ota_bitmap = ota_bitmap_init(toatol_num);
                    if (!g_ota_bitmap) {
                        log_error("[FOTA] bitmap init failed");
                        ota_change_state_and_timer(OTA_IDLE, 0);
                        break;
                    }
                    for (i = 0; i < toatol_num; i++) {
                        ota_bitmap_set(g_ota_bitmap, i);
                    }
                    g_ota_total_num = toatol_num;
                    g_ota_segments_size = ota_response.segments;
                    ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
                    need_reboot = false;
                } else {
                    ota_printf("crc fail %x %x ", crc32, ota_get_image_crc());
                    if (g_ota_bitmap) {
                        ota_bitmap_delete(g_ota_bitmap);
                        g_ota_bitmap = NULL;
                    }
                    g_ota_bitmap = ota_bitmap_init(toatol_num);
                    if (!g_ota_bitmap) {
                        log_error("[FOTA] bitmap init failed");
                        ota_change_state_and_timer(OTA_IDLE, 0);
                        break;
                    }
                    g_ota_total_num = toatol_num;
                    g_ota_segments_size = ota_response.segments;
                    ota_bootinfo_reset();
                    enter_critical_section();
                    for (i = 0; i < OTA_MAX_IMAGE_SECTORS; i++) {
                        // Page erase (4096 bytes)
                        while (flash_check_busy())
                            ;
                        flash_erase(FLASH_ERASE_SECTOR,
                                    OTA_FLASH_START + (0x1000 * i));
                        while (flash_check_busy())
                            ;
                    }
                    leave_critical_section();
                    ota_change_state_and_timer(
                        OTA_REQUEST_SENDING,
                        (OTA_REQUEST_TABLE_SIZE * OTA_RESPONESE_TIMEOUT));
                    break;
                }
            } else {
                break;
            }
        }

        if (ota_response.segments != g_ota_segments_size) {
            ota_printf("ota response segments fail %u %u ",
                       ota_response.segments, g_ota_segments_size);
            break;
        }

        if (NULL == g_ota_bitmap) {
            ota_printf("ota response bitmap NULL");
            break;
        }

        if (ota_response.version != g_ota_image_version) {
            ota_printf("ota response differnt version %x %x ",
                       ota_response.version, g_ota_image_version);
            break;
        }
        //have other node not upgrade, need wait
        if (g_ota_state == OTA_DONE) {
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
        }
        enter_critical_section();
        for (i = 0; i < OTA_RESPONSE_TABLE_SIZE; i++) {
            if (g_resp_table[i] == ota_response.seq) {
                if (xTimerIsTimerActive(ota_resp_timer)) {
                    if (++g_ota_response_c >= g_ota_response_k) {
                        g_ota_response_c = 0;
                        xTimerStop(ota_resp_timer, 0);
                    }
                }
                g_resp_table[i] = 0xFFFF;
                break;
            }
        }
        g_ota_request_cnt = 0;
        leave_critical_section();
        if (ota_bitmap_get_bit(g_ota_bitmap, ota_response.seq)) {
            // ota_printf("ota response seq same %u",ota_response_header->seq);
            break;
        } else {
            ota_data_lens = ota_response.segments;
            if (ota_response.seq == (toatol_num - 1)) {
                ota_data_lens = ota_response.size % ota_response.segments;
            }

            ota_bitmap_set(g_ota_bitmap, ota_response.seq);
            ota_printf("[R] ota response %u remain %u", ota_response.seq,
                       ota_bitmap_get_remain(g_ota_bitmap, toatol_num));

            enter_critical_section();
            tmp_addr = OTA_FLASH_START
                       + (ota_response.segments * ota_response.seq);
            // ota_bitmap_print(ota_bitmap,toatol_num);

            for (i = 0; i < ota_data_lens; i++) {
                while (flash_check_busy())
                    ;
                flash_write_byte(tmp_addr + i, ota_response.data[i]);
                while (flash_check_busy())
                    ;
            }
            leave_critical_section();
            if (0 == ota_bitmap_get_remain(g_ota_bitmap, toatol_num)) {
                crc32 = crc32checksum(
                    (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                    (ota_response.size - OTA_INFO_HEADER_OFFSET));
                if (crc32 != ota_response.crc) {
                    ota_printf("ota response upgrade fail %X %X", crc32,
                               ota_response.crc);
                    ota_change_state_and_timer(OTA_IDLE, 0);
                } else {
                    ota_check_bin_tpye();
                }
                break;
            }
        }
    } while (0);
}

static void ota_rxmode_received_event_handler(uint8_t* data, uint16_t lens) {
    /*data is null*/
    otDeviceRole changeRole = otThreadGetDeviceRole(otrGetInstance());
    ota_rxmode_t* rxmode = (ota_rxmode_t*)data;
#if CONFIG_MIU_DEVICE_TYPE_FTD
    if (changeRole == OT_DEVICE_ROLE_ROUTER
        || changeRole == OT_DEVICE_ROLE_LEADER) {
        uint16_t maxChildrens = 0, sleep_node = 0;
        otChildInfo childInfo;
        maxChildrens = otThreadGetMaxAllowedChildren(otrGetInstance());
        for (uint16_t i = 0; i < maxChildrens; i++) {
            if ((otThreadGetChildInfoByIndex(otrGetInstance(), i, &childInfo)
                 != OT_ERROR_NONE)
                || childInfo.mIsStateRestoring) {
                continue;
            }
            if (childInfo.mRxOnWhenIdle == false) {
                otIp6Address dst_ipaddr = *otThreadGetRloc(otrGetInstance());
                dst_ipaddr.mFields.m8[14] = (childInfo.mRloc16 >> 8) & 0xff;
                dst_ipaddr.mFields.m8[15] = childInfo.mRloc16 & 0xff;
                ota_rxmode_sended_event_handler(dst_ipaddr, data, lens);
                ++sleep_node;
            }
        }
        ota_printf("sleep node %u ", sleep_node);
    } else
#endif
    {
        if (ota_get_state() != OTA_IDLE) {
            ota_printf("ota_rxmode_received state %s ",
                       OtaStateToString(ota_get_state()));
            return;
        }
        /* Wake / sleep the radio first, regardless of version.
           Same-version check only applies to actual data download, not wake-up. */
        if (rxmode->OnWhenIdle == true) {
            if (otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle == false) {
                ota_printf("wake up ");
                otLinkModeConfig config;
                config.mRxOnWhenIdle = true;
                config.mNetworkData = false;
                config.mDeviceType = false;

                if (otThreadSetLinkMode(otrGetInstance(), config)
                    != OT_ERROR_NONE) {
                    ota_printf("wake up set fail");
                }
            }
        } else {
            if (otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle == true) {
                ota_printf("go to sleep ");
                otLinkModeConfig config;
                config.mRxOnWhenIdle = false;
                config.mNetworkData = false;
                config.mDeviceType = false;

                if (otThreadSetLinkMode(otrGetInstance(), config)
                    != OT_ERROR_NONE) {
                    ota_printf("go to sleep set fail");
                }
            }
        }
        if (ota_get_image_version() != 0 && rxmode->version != 0
            && ota_get_image_version() == rxmode->version) {
            ota_printf("ota_rxmode_received same version %x %x ",
                       rxmode->version, ota_get_image_version());
            return;
        }
    }
}

static void ota_status_received_event_handler(uint8_t* data, uint16_t lens,
                                              otIp6Address src_ipv6) {
    uint8_t flag = data[0];
    if (flag == OTA_STATUS_GET) {
        ota_status_get_t* ota_status_get = (ota_status_get_t*)data;
        bool need_send_version = false;
#if CONFIG_MIU_DEVICE_TYPE_FTD
        if (ota_status_get->device_type == OTA_STATUS_TYPE_FTD) {
            need_send_version = true;
        }
#endif
#if CONFIG_MIU_DEVICE_TYPE_MTD
        if (ota_status_get->device_type == OTA_STATUS_TYPE_MTD) {
            need_send_version = true;
        }
#endif
        else if (ota_status_get->device_type == OTA_STATUS_TYPE_ALL) {
            need_send_version = true;
        } else {
            ota_printf("unknow ota staus device type");
        }
        if (need_send_version == true) {
            if (NULL == ota_report_timer) {
                enter_critical_section();
                g_ota_status_report.flag = OTA_STATUS_REPORT;
                if (ota_get_state() == OTA_IDLE) {
                    g_ota_status_report.version = GET_BIN_VERSION(
                        systeminfo.sysinfo);
                } else {
                    g_ota_status_report.version = ota_get_image_version();
                }
                g_ota_status_report.rxmode =
                    otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle;
                g_ota_status_report.state = ota_get_state();
                memcpy(g_ota_status_report.bin_type,
                       GET_BIN_TYPE_PTR(systeminfo.sysinfo), 12);
                memcpy(&g_ota_status_report.dstipv6.mFields.m8,
                       &src_ipv6.mFields.m8, OT_IP6_ADDRESS_SIZE);

                if (ota_get_state() == OTA_IDLE
                    || g_ota_segments_size == 0
                    || !IS_VALID_PTR(g_ota_bitmap)) {
                    g_ota_status_report.progress_bar = 0;
                } else {
                    uint32_t toatol_num = (g_ota_image_size
                                           / g_ota_segments_size);
                    if (g_ota_image_size % g_ota_segments_size) {
                        ++toatol_num;
                    }
                    uint32_t remain = ota_bitmap_get_remain(g_ota_bitmap,
                                                            toatol_num);
                    g_ota_status_report.progress_bar =
                        (toatol_num <= 1) ? 0
                        : (((toatol_num - remain) * 100) / (toatol_num - 1));
                }
                leave_critical_section();
                uint16_t random_time = otRandomNonCryptoGetUint16InRange(
                    1, ota_status_get->report_time);

                ota_printf("send staus report %d ms", random_time);
                ota_report_timer = xTimerCreate("ota_report_timer", random_time,
                                                false, &g_ota_status_report,
                                                ota_report_timer_handler);
                xTimerStart(ota_report_timer, 0);
            } else {
                ota_printf("ota_report_timer exist");
            }
        }
    } else if (flag == OTA_STATUS_REPORT) {
        ota_status_report_t* ota_status_report = (ota_status_report_t*)data;
        char string[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&src_ipv6, string, sizeof(string));
        log_info("OTA_STATUS|%s|%08x|%s|%u|%u%%|%.12s", string,
                 ota_status_report->version,
                 OtaStateToString(ota_status_report->state),
                 ota_status_report->rxmode, ota_status_report->progress_bar,
                 ota_status_report->bin_type);
    } else {
        ota_printf("unknow ota staus flag");
    }
}

static void ota_execute_received_event_handler(uint8_t* data, uint16_t lens,
                                               otIp6Address src_ipv6) {
    ota_execute_t* ota_execute = (ota_execute_t*)data;
    if (lens < sizeof(ota_execute_t)) {
        return;
    }
    if (ota_execute->flag == OTA_EXECUTE_REBOOT) {
        if (ota_get_state() == OTA_DONE) {
            uint32_t crc32 = crc32checksum(
                (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
            if (crc32 != g_ota_image_crc) {
                ota_printf("ota request upgrade fail %X %X", crc32,
                           g_ota_image_crc);
                ota_change_state_and_timer(OTA_IDLE, 0);
            } else {
                ota_bootinfo_ready();
                ota_change_state_and_timer(OTA_REBOOT, ota_execute->time);
                ota_printf("%u ms after reboot", ota_execute->time);
            }
        } else {
            ota_printf("state is not done");
        }
    } else if (ota_execute->flag == OTA_EXECUTE_STOP) {
        /* Always erase OTA flash on stop, even if already Idle.
         * Without this, a device that naturally timed out from OTA_DONE back
         * to OTA_IDLE retains the image in flash — the version stays non-zero
         * and the next TC's same-version pre-check would incorrectly skip OTA. */
        ota_change_state_and_timer(OTA_IDLE, 0);
        ota_reset();
        ota_printf("stop+erase done");
    } else {
        ota_printf("unknow ota staus flag");
    }
}

void ota_event_handler() {
    ota_event_data_t event_data;
    memset(&event_data, 0x0, sizeof(ota_event_data_t));
    /*process ota event*/
    do {
        enter_critical_section();
        if (xQueueReceive(ota_event_queue, &event_data, 0) != pdPASS) {
            leave_critical_section();
            break;
        }
        leave_critical_section();
        switch (event_data.event) {
            case OTA_DATA_SEND_EVENT:
                ota_data_sended_event_handler(event_data.data,
                                              event_data.data_lens);
                break;
            case OTA_REQUEST_SEND_EVENT:
                ota_request_sended_event_handler(event_data.data,
                                                 event_data.data_lens);
                break;
            case OTA_RESPONSE_SEND_EVENT:
                ota_response_sended_event_handler(event_data.data,
                                                  event_data.data_lens);
                break;
            case OTA_DATA_RECEIVE_EVENT:
                ota_data_received_event_handler(event_data.data,
                                                event_data.data_lens);
                break;
            case OTA_REQUEST_RECEIVE_EVENT:
                ota_request_received_event_handler(event_data.data,
                                                   event_data.data_lens);
                break;
            case OTA_RESPONSE_RECEIVE_EVENT:
                ota_response_received_event_handler(event_data.data,
                                                    event_data.data_lens);
                break;
            case OTA_RXMODE_RECEIVE_EVENT:
                ota_rxmode_received_event_handler(event_data.data,
                                                  event_data.data_lens);
                break;
            case OTA_STATUS_RECEIVE_EVENT:
                ota_status_received_event_handler(
                    event_data.data, event_data.data_lens, event_data.ipv6);
                break;
            case OTA_EXECUTE_RECEIVE_EVENT:
                ota_execute_received_event_handler(
                    event_data.data, event_data.data_lens, event_data.ipv6);
                break;
            default: ota_printf("unknow event %u", event_data.event); break;
        }
        if (event_data.data) {
            xFree(event_data.data);
            event_data.data = NULL;
        }
    } while (0);
}

void ota_start(uint16_t segments_size, uint16_t intervel) {
    otError error = OT_ERROR_NONE;
    if (g_ota_state == OTA_DONE) {
        // Allow restarting from DONE without broadcasting stop to network
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
    if (g_ota_state != OTA_IDLE) {
        log_info("ota in progress %u ", g_ota_state);
        return;
    }
    if (segments_size > OTA_SEGMENTS_MAX_SIZE) {
        log_info("ota segments_size size can't big than 256 (%u) ",
                 segments_size);
        return;
    }
    // Initiator send parameter
    g_ota_segments_size = 255;
    g_ota_data_intervel = 1000;
    if (segments_size != 0) {
        g_ota_segments_size = segments_size;
    }
    if (intervel != 0) {
        g_ota_data_intervel = intervel;
    }

    if (ota_get_image_size() == 0) {
        log_info("ota size is 0 ");
        return;
    }

    g_ota_total_num = ota_get_image_size() / g_ota_segments_size;
    g_ota_start_index = 0;
    if (ota_get_image_size() % g_ota_segments_size) {
        ++g_ota_total_num;
    }
    // Initiator bitmap set all
    g_ota_bitmap = ota_bitmap_init(g_ota_total_num);
    if (!g_ota_bitmap) {
        log_error("[FOTA] bitmap init failed");
        ota_change_state_and_timer(OTA_IDLE, 0);
        return;
    }
    for (uint32_t i = 0; i < g_ota_total_num; i++) {
        ota_bitmap_set(g_ota_bitmap, i);
    }

    ota_printf("ota_toatol_num %u ", g_ota_total_num);
    uint32_t crc32 = crc32checksum(
        (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
        (ota_get_image_size() - OTA_INFO_HEADER_OFFSET));
    if (crc32 != ota_get_image_crc()) {
        log_info("crc fail %x %x ", crc32, ota_get_image_crc());
        return;
    } else {
        ota_printf("crc %x %x ", crc32, ota_get_image_crc());
    }
    char peer_addr[] = "ff03:0:0:0:0:0:0:1";
    error = otIp6AddressFromString(peer_addr, &ota_sender_addr);

    ota_change_state_and_timer(OTA_DATA_SENDING, g_ota_data_intervel);
    need_reboot = false;
}

void ota_send(char* ipaddr_str) {
    otError error = OT_ERROR_NONE;
    if (g_ota_state == OTA_DONE) {
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
    if (g_ota_state != OTA_IDLE) {
        log_info("ota in progress %u ", g_ota_state);
        return;
    }
    error = otIp6AddressFromString(ipaddr_str, &ota_sender_addr);
    if (OT_ERROR_NONE == error) {
        g_ota_segments_size = 255;
        g_ota_data_intervel = 3000;
        g_ota_total_num = ota_get_image_size() / g_ota_segments_size;
        g_ota_start_index = 0;
        have_data_ack = false;
        if (ota_get_image_size() % g_ota_segments_size) {
            ++g_ota_total_num;
        }
        if (ota_get_image_size() == 0) {
            log_info("ota size is 0 ");
            return;
        }
        ota_printf("ota_toatol_num %u ", g_ota_total_num);
        uint32_t crc32 = crc32checksum(
            (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
            (ota_get_image_size() - OTA_INFO_HEADER_OFFSET));
        if (crc32 != ota_get_image_crc()) {
            log_info("crc fail %x %x ", crc32, ota_get_image_crc());
            return;
        } else {
            ota_printf("crc %x %x ", crc32, ota_get_image_crc());
        }
        ota_change_state_and_timer(OTA_DATA_SENDING, g_ota_data_intervel);
        need_reboot = false;
    } else {
        log_info("ip error(%u) %s ", error, ipaddr_str);
    }
}

void ota_send_rxmode(bool OnWhenIdle) {
    otError error = OT_ERROR_NONE;
    otIp6Address dst_ipaddr;
    char dst_ipaddr_str[] = "ff03:0:0:0:0:0:0:1";
    error = otIp6AddressFromString(dst_ipaddr_str, &dst_ipaddr);
    ota_rxmode_t ota_rxmode;
    ota_rxmode.OnWhenIdle = OnWhenIdle; //set rx on mode
    ota_rxmode.version = ota_get_image_version();
    ota_rxmode_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_rxmode,
                                    sizeof(ota_rxmode_t));
#if CONFIG_MIU_DEVICE_TYPE_FTD
    otDeviceRole changeRole = otThreadGetDeviceRole(otrGetInstance());
    if (changeRole == OT_DEVICE_ROLE_ROUTER
        || changeRole == OT_DEVICE_ROLE_LEADER) {
        uint16_t maxChildrens = 0, sleep_node = 0;
        otChildInfo childInfo;
        maxChildrens = otThreadGetMaxAllowedChildren(otrGetInstance());
        for (uint16_t i = 0; i < maxChildrens; i++) {
            if ((otThreadGetChildInfoByIndex(otrGetInstance(), i, &childInfo)
                 != OT_ERROR_NONE)
                || childInfo.mIsStateRestoring) {
                continue;
            }
            if (childInfo.mRxOnWhenIdle == false) {
                dst_ipaddr = *otThreadGetRloc(otrGetInstance());
                dst_ipaddr.mFields.m8[14] |= childInfo.mRloc16 & 0xff00 >> 0;
                dst_ipaddr.mFields.m8[15] = childInfo.mRloc16 & 0xff;
                ota_rxmode_sended_event_handler(
                    dst_ipaddr, (uint8_t*)&ota_rxmode, sizeof(ota_rxmode_t));
                ++sleep_node;
            }
        }
        ota_printf("sleep node %u ", sleep_node);
    }
#endif
}

void ota_send_status_get(otIp6Address dst_ipaddr, uint8_t status_type,
                         uint16_t report_time) {
    otError error = OT_ERROR_NONE;
    ota_status_get_t ota_stauts_get;
    ota_stauts_get.flag = OTA_STATUS_GET;
    ota_stauts_get.device_type = status_type;
    ota_stauts_get.report_time = report_time;

    ota_status_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_stauts_get,
                                    sizeof(ota_stauts_get));
}

void ota_send_execute(otIp6Address dst_ipaddr, uint8_t execute_flag,
                      uint16_t time) {
    ota_execute_t ota_execute;
    ota_execute.flag = execute_flag;
    ota_execute.time = time;
    memset(ota_execute.bin_type, 0, sizeof(ota_execute.bin_type));

    ota_execute_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_execute,
                                     sizeof(ota_execute_t));
}

void ota_stop() { ota_change_state_and_timer(OTA_IDLE, 0); }

void ota_update_self() {
    uint32_t crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                   (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
    if (crc32 != g_ota_image_crc) {
        ota_printf("ota request upgrade fail %X %X", crc32, g_ota_image_crc);
        ota_change_state_and_timer(OTA_IDLE, 0);
    } else {
        ota_bootinfo_ready();
        ota_change_state_and_timer(OTA_REBOOT, 10);
    }
}

void ota_debug_level(unsigned int level) { ota_debug_flags = level; }

uint32_t ota_debug_level_get() { return ota_debug_flags; }

otError ota_init(otInstance* aInstance,
                 void (*ota_state_change_cb)(uint8_t state)) {
    otError error = OT_ERROR_NONE;

    do {
        error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
        if (error != OT_ERROR_NONE) {
            break;
        }

        memset(&ota_data_resource, 0, sizeof(ota_data_resource));
        memset(&ota_req_resource, 0, sizeof(ota_req_resource));
        memset(&ota_resp_resource, 0, sizeof(ota_resp_resource));
        memset(&ota_rxmode_resource, 0, sizeof(ota_rxmode_resource));
        memset(&ota_status_resource, 0, sizeof(ota_status_resource));
        memset(&ota_execute_resource, 0, sizeof(ota_execute_resource));

        ota_data_resource.mUriPath = RAFAEL_OTA_URL_DATA;
        ota_data_resource.mContext = aInstance;
        ota_data_resource.mHandler = &ota_coap_data_proccess;
        ota_data_resource.mNext = NULL;

        ota_req_resource.mUriPath = RAFAEL_OTA_URL_REQ;
        ota_req_resource.mContext = aInstance;
        ota_req_resource.mHandler = &ota_coap_request_proccess;
        ota_req_resource.mNext = NULL;

        ota_resp_resource.mUriPath = RAFAEL_OTA_URL_RESP;
        ota_resp_resource.mContext = aInstance;
        ota_resp_resource.mHandler = &ota_coap_response_proccess;
        ota_resp_resource.mNext = NULL;

        ota_rxmode_resource.mUriPath = RAFAEL_OTA_URL_RXMODE;
        ota_rxmode_resource.mContext = aInstance;
        ota_rxmode_resource.mHandler = &ota_coap_rxmode_proccess;
        ota_rxmode_resource.mNext = NULL;

        ota_status_resource.mUriPath = RAFAEL_OTA_URL_STATUS;
        ota_status_resource.mContext = aInstance;
        ota_status_resource.mHandler = &ota_coap_status_proccess;
        ota_status_resource.mNext = NULL;

        ota_execute_resource.mUriPath = RAFAEL_OTA_URL_EXECUTE;
        ota_execute_resource.mContext = aInstance;
        ota_execute_resource.mHandler = &ota_coap_execute_proccess;
        ota_execute_resource.mNext = NULL;

        otCoapAddResource(aInstance, &ota_data_resource);
        otCoapAddResource(aInstance, &ota_req_resource);
        otCoapAddResource(aInstance, &ota_resp_resource);
        otCoapAddResource(aInstance, &ota_rxmode_resource);
        otCoapAddResource(aInstance, &ota_status_resource);
        otCoapAddResource(aInstance, &ota_execute_resource);

        if (NULL == ota_timer) {
            ota_timer = xTimerCreate("ota_timer_t", portMAX_DELAY, false, NULL,
                                     ota_timer_handler);
        } else {
            ota_printf("ota_timer exist");
        }

        if (NULL == ota_resp_timer) {
            ota_resp_timer = xTimerCreate("ota_resp_timer", portMAX_DELAY,
                                          false, NULL,
                                          ota_response_timer_handler);
        } else {
            ota_printf("ota_resp_timer exist");
        }
        char peer_addr[] = "ff03:0:0:0:0:0:0:1";
        error = otIp6AddressFromString(peer_addr, &ota_sender_addr);

        /* Init rx done queue*/
        ota_event_queue = xQueueCreate(10, sizeof(ota_event_data_t));

        ota_state_change_cb_signal = ota_state_change_cb;

    } while (0);

    return error;
}

#endif // CONFIG_APP_TASK_OTA_ENABLE