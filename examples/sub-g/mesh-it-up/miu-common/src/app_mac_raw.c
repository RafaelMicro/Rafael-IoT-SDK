/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */
#include <miu_port.h>
#include "FreeRTOS.h"
#include "openthread-core-config.h"

#include <main.h>
#include "app_control_cmd.h"
#include "cli.h"
#include "hosal_crypto_aes.h"
#include "log.h"
#include "queue.h"
#include "string.h"
#include "util_string.h"

uint8_t key[AES_BLOCKLEN] = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
                             0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};

#define THREAD_MAC_RAW_EVENT_SEND     0x1
#define THREAD_MAC_RAW_EVENT_RECEIVED 0x2

typedef struct {
    uint8_t event;
    uint8_t mac_addr[8]; /*dst or src*/
    int8_t rssi;         /*src rssi*/
    uint8_t channel;     /*dst*/
    uint16_t panid;      /*dst*/
    uint8_t buf[OT_RADIO_FRAME_MAX_SIZE + 1];
    uint16_t lens;
} app_mac_raw_msg_t;

static app_mac_raw_msg_t app_mac_raw_msg;
static xQueueHandle app_mac_raw_msg_queue;

uint8_t temp_data_buf[OT_RADIO_FRAME_MAX_SIZE];

static uint16_t pkcs7_padding(const uint8_t* input_buf, uint16_t input_len,
                              uint8_t* output_buf) {
    uint16_t padded_len;
    uint8_t padding_val;

    padding_val = HOSAL_AES_BLOCKLEN - (input_len % HOSAL_AES_BLOCKLEN);
    if (padding_val == 0) {
        padding_val = HOSAL_AES_BLOCKLEN;
    }

    padded_len = input_len + padding_val;

    memcpy(output_buf, input_buf, input_len);
    memset(output_buf + input_len, padding_val, padding_val);

    return padded_len;
}

static uint16_t pkcs7_unpadding(const uint8_t* input_buf, uint16_t input_len) {
    if (input_buf == NULL || input_len == 0
        || input_len % HOSAL_AES_BLOCKLEN != 0) {
        return 0;
    }

    uint8_t padding_val = input_buf[input_len - 1];

    if (padding_val == 0 || padding_val > HOSAL_AES_BLOCKLEN) {
        log_error("Unpadding error: Invalid padding value %u\r\n", padding_val);
        return 0;
    }

    for (int i = 0; i < padding_val; i++) {
        if (input_buf[input_len - 1 - i] != padding_val) {
            log_error("Unpadding error: Padding check failed\r\n");
            return 0;
        }
    }

    return input_len - padding_val;
}

void app_macRawReceived_task(uint8_t* data, uint16_t data_lens,
                             uint8_t* src_mac, int8_t src_rssi) {
#if CONFIG_APP_TASK_CONTROL_CMD_ENABLE
    if (app_ctrl_received(src_mac, data, data_lens) == 0) {
        // log_info("app_ctrl_received success");
    } else
#endif
    {
        log_info(
            "lens(%d), src(%02x%02x%02x%02x%02x%02x%02x%02x), rssi(%d) \r\n",
            data_lens, src_mac[0], src_mac[1], src_mac[2], src_mac[3],
            src_mac[4], src_mac[5], src_mac[6], src_mac[7], src_rssi);
        log_info_hexdump("mac_rx", data, data_lens);
    }
}

int app_macRawSend_task(uint8_t* data, uint16_t data_lens, uint8_t* dst_mac,
                        uint8_t dst_channel, uint16_t dst_panid) {

    otInstance* instance = otrGetInstance();
    return radio_mac_send(instance, dst_mac, data, data_lens, dst_channel,
                          dst_panid);
}

void app_macRaw_task(void* arg) {

    if (xQueueReceive(app_mac_raw_msg_queue, &app_mac_raw_msg, 0) == pdPASS) {

        if (app_mac_raw_msg.event == THREAD_MAC_RAW_EVENT_SEND) {
            int ret = app_macRawSend_task(
                app_mac_raw_msg.buf, app_mac_raw_msg.lens,
                app_mac_raw_msg.mac_addr, app_mac_raw_msg.channel,
                app_mac_raw_msg.panid);
            if (ret != 0) {
                log_info("app_macRawSend_task fail %d", ret);
            }
        } else if (app_mac_raw_msg.event == THREAD_MAC_RAW_EVENT_RECEIVED) {
            app_macRawReceived_task(app_mac_raw_msg.buf, app_mac_raw_msg.lens,
                                    app_mac_raw_msg.mac_addr,
                                    app_mac_raw_msg.rssi);
        }
    }
}

static void app_macRawReceived_cb(uint8_t* data, uint16_t lens, int8_t rssi,
                                  uint8_t* src_addr) {
    hosal_aes_dev_t aes_dev;
    uint16_t original_len = 0;

    memset((void*)&app_mac_raw_msg, 0, sizeof(app_mac_raw_msg_t));
    app_mac_raw_msg.event = THREAD_MAC_RAW_EVENT_RECEIVED;
    app_mac_raw_msg.lens = lens;
    memcpy(app_mac_raw_msg.mac_addr, src_addr, 8);
    app_mac_raw_msg.rssi = rssi;

    if (lens > 0 && lens % HOSAL_AES_BLOCKLEN == 0
        && app_mac_raw_msg.lens < (OT_RADIO_FRAME_MAX_SIZE + 1)) {
        hosal_crypto_aes_init();
        for (uint16_t off = 0; off < lens; off += HOSAL_AES_BLOCKLEN) {
            hosal_aes_dev_t aes_dev;
            memset(&aes_dev, 0, sizeof(aes_dev));
            aes_dev.crypto_operation = HOSAL_AES_CRYPTO_DECRYPT;
            aes_dev.bit = HOSAL_AES_128_BIT;
            aes_dev.in_ptr = data + off;
            aes_dev.out_ptr = app_mac_raw_msg.buf + off;
            aes_dev.key_ptr = key;
            hosal_crypto_aes_operation(&aes_dev);
        }

        original_len = pkcs7_unpadding(app_mac_raw_msg.buf, lens);
        if (original_len == 0) {
            log_error("Decryption successful, but unpadding failed. Discarding "
                      "message.\r\n");
            return;
        }
        app_mac_raw_msg.lens = original_len;

        // log_info_hexdump("mac_rx_decrypt", app_mac_raw_msg.buf,
        //                  app_mac_raw_msg.lens);

        if (xQueueSend(app_mac_raw_msg_queue, &app_mac_raw_msg, 0) == pdPASS) {
            ot_app_task_post(app_macRaw_task, NULL);
        }
    } else {
        log_error(
            "Mac Raw Received len %u is not a valid AES block multiple\r\n",
            lens);
    }
}

int app_macRawSend(uint8_t* dstaddr, uint8_t* data, uint16_t len,
                   uint16_t panid, uint8_t channel, bool fromISR) {
    otError error = OT_ERROR_BUSY;
    hosal_aes_dev_t aes_dev;
    uint16_t padded_len = 0;

    memset((void*)&app_mac_raw_msg, 0, sizeof(app_mac_raw_msg_t));
    app_mac_raw_msg.event = THREAD_MAC_RAW_EVENT_SEND;
    app_mac_raw_msg.lens = len;
    memcpy(app_mac_raw_msg.mac_addr, dstaddr, 8);
    app_mac_raw_msg.channel = channel;
    app_mac_raw_msg.panid = panid;

    if (app_mac_raw_msg.lens < (OT_RADIO_FRAME_MAX_SIZE + 1)) {
        memset(temp_data_buf, 0, OT_RADIO_FRAME_MAX_SIZE);
        padded_len = pkcs7_padding(data, len, temp_data_buf);

        if (padded_len > OT_RADIO_FRAME_MAX_SIZE) {
            log_error("Data length too big after padding: %u\r\n", padded_len);
            return 1;
        }

        hosal_crypto_aes_init();

        for (uint16_t off = 0; off < padded_len; off += HOSAL_AES_BLOCKLEN) {
            hosal_aes_dev_t aes_dev;

            memset(&aes_dev, 0, sizeof(aes_dev));
            aes_dev.crypto_operation = HOSAL_AES_CRYPTO_ENCRYPT;
            aes_dev.bit = HOSAL_AES_128_BIT;
            aes_dev.in_ptr = temp_data_buf + off;
            aes_dev.out_ptr = app_mac_raw_msg.buf + off;
            aes_dev.key_ptr = key;

            hosal_crypto_aes_operation(&aes_dev);
        }
        app_mac_raw_msg.lens = padded_len;
        // log_info_hexdump("mac_tx_encrypt", app_mac_raw_msg.buf,
        //  app_mac_raw_msg.lens);
        if (fromISR) {
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            if (xQueueSendFromISR(app_mac_raw_msg_queue, &app_mac_raw_msg,
                                  &pxHigherPriorityTaskWoken)
                == pdPASS) {
                ot_app_task_post(app_macRaw_task, NULL);
                error = OT_ERROR_NONE;
            }
        } else {
            if (xQueueSend(app_mac_raw_msg_queue, &app_mac_raw_msg, 0)
                == pdPASS) {
                ot_app_task_post(app_macRaw_task, NULL);
                error = OT_ERROR_NONE;
            }
        }

    } else {
        log_info("App Mac Raw Send len %u/%u to big \r\n", len,
                 (OT_RADIO_FRAME_MAX_SIZE + 1));
    }
    return (error != OT_ERROR_NONE);
}

uint8_t app_macRawInit(otInstance* instance) {
    otSockAddr sockAddr;

    uint8_t ret = 0;

    otPlatRadioMacReceivedCallback(app_macRawReceived_cb);

    app_mac_raw_msg_queue = xQueueCreate(5, sizeof(app_mac_raw_msg_t));
    return ret;
}