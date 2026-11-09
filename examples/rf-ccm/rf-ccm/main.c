/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_uart.h"
#include "gpio.h"
#include "sysctrl.h"

#include "timers.h"
#include "queue.h"

#include "rf_mcu_types.h"
#include "rf_mcu.h"
#include "rf_common_init.h"
#include "rf_test.h"
#include "uart_bridge.h"
#include "rf_aes_ccm.h"
#include "hosal_crypto_aes_ccm.h"
#include "app_hooks.h"
#include "uart_stdio.h"


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define RF_FW_SELECT                    (RF_FW_LOAD_SELECT_RUCI_CMD)


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/

/*
 * Ported from rt584h_sdk's Project/Application/RF_Evaluation/AES_CCM_RF_Test.
 *
 * IMPORTANT: this requires the RT569S radio to be running firmware built
 * from the rt569s Trunk repo with the ccm_loopback.c feature -- the RT569
 * image bundled in this SDK's rt569-fw component does not have it, so the
 * radio must be flashed separately with a matching build before any of
 * the aes_ccm_*_rf() calls below will succeed. See rf_aes_ccm.h.
 *
 * One-shot bring-up self-test comparing the two CCM encryption paths for
 * the same key/nonce/header/data/mode:
 *   - local, host: hosal_crypto_aes_ccm_operation() (HOSAL_AES_CCM_ENCRYPT;
 *     CPU + HW AES accelerator, not pure software)
 *   - RT569S loop-back: aes_ccm_encryption_rf()
 *
 * Uses RFC 3610 test vector #1 (same one
 * examples/peripheral/crypto/crypto_ccm uses), so there are three things
 * to cross-check, not just two: the local host path's own output
 * against the RFC-known ciphertext (catches a broken local path), and the
 * RT569S loop-back output against both (catches a broken loop-back path
 * or a key/nonce mismatch between the two).
 *
 * Must run right after the comm subsystem is up, before any live BLE/
 * Sub-GHz connection could exist -- BMU TX queue 6 is shared with normal
 * connection data (see rf_aes_ccm.h), and aes_ccm_key_set_rf()/
 * aes_ccm_encryption_rf() are bench/idle-time-only for the same reason.
 */
static void aes_ccm_compare_selftest(void)
{
    /* RFC 3610 test vector #1 */
    static const uint8_t key[16] =
    {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    static const uint8_t nonce[13] =
    {
        0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    static const uint8_t hdr[8] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    static const uint8_t data[23] =
    {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };
    static const uint8_t expected[39] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
        0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
        0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84, 0x17,
        0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0
    };
    const uint32_t mlen = 8;

    hosal_aes_ccm_dev_t sw_dev;
    uint8_t  sw_out[sizeof(hdr) + sizeof(data) + 16];
    uint32_t sw_out_len;

    struct aes_ccm_encryption_packet_rf rf_packet;
    uint8_t  rf_out[sizeof(hdr) + sizeof(data) + 16];
    uint32_t rf_out_len;

    /* 1. Local host CCM path (HOSAL) */
    sw_dev.bit = HOSAL_AES_128_BIT;
    sw_dev.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    sw_dev.key_ptr = (uint8_t *)key;
    sw_dev.nonce = (uint8_t *)nonce;
    sw_dev.hdr = (uint8_t *)hdr;
    sw_dev.hdr_len = sizeof(hdr);
    sw_dev.data = (uint8_t *)data;
    sw_dev.data_len = sizeof(data);
    sw_dev.mlen = mlen;
    sw_out_len = sizeof(sw_out);
    sw_dev.out_buf = sw_out;
    sw_dev.out_buf_len = &sw_out_len;

    if (hosal_crypto_aes_ccm_operation(&sw_dev) != STATUS_SUCCESS)
    {
        printf("[AES CCM Compare] local host path failed\r\n");
        return;
    }

    if ((sw_out_len != sizeof(expected)) || (memcmp(sw_out, expected, sizeof(expected)) != 0))
    {
        printf("[AES CCM Compare] local host path result MISMATCHES RFC3610 vector #1\r\n");
        return;
    }

    /* 2. RT569S loop-back path, same key/nonce/header/data/mode */
    if (!aes_ccm_key_set_rf(128, key))
    {
        printf("[AES CCM Compare] RT569S key-set request failed\r\n");
        return;
    }

    rf_packet.nonce = (uint8_t *)nonce;
    rf_packet.hdr = (uint8_t *)hdr;
    rf_packet.hdr_len = sizeof(hdr);
    rf_packet.data = (uint8_t *)data;
    rf_packet.data_len = sizeof(data);
    rf_packet.mlen = mlen;
    rf_out_len = sizeof(rf_out);
    rf_packet.out_buf = rf_out;
    rf_packet.out_buf_len = &rf_out_len;

    if (!aes_ccm_encryption_rf(&rf_packet))
    {
        printf("[AES CCM Compare] RT569S loop-back request failed\r\n");
        return;
    }

    /* 3. Compare */
    if ((rf_out_len != sw_out_len) || (memcmp(rf_out, sw_out, sw_out_len) != 0))
    {
        printf("[AES CCM Compare] MISMATCH between local host and RT569S loop-back results\r\n");
        return;
    }

    printf("[AES CCM Compare] MATCH: local host and RT569S loop-back both produced the "
           "correct RFC3610 vector #1 ciphertext (%lu bytes)\r\n", (unsigned long)sw_out_len);
}

/*
 * One-shot bring-up self-test for the RT569S decrypt-loopback path
 * (aes_ccm_decryption_rf()), cross-checked against the local host
 * decrypt path (hosal_crypto_aes_ccm_operation(), HOSAL_AES_CCM_DECRYPT).
 * Uses the same RFC 3610 test vector #1 ciphertext
 * aes_ccm_compare_selftest() above produces/checks, so together they
 * cover a full encrypt/decrypt round trip on both engines.
 *
 * Also runs a negative case: a single corrupted ciphertext byte must make
 * the RT569S path reject the packet (MIC mismatch), not just silently
 * decrypt garbage -- that's the one property that actually matters for a
 * security check, and it's cheap to verify here.
 *
 * Same bench/idle-time-only constraint as aes_ccm_compare_selftest().
 */
static void aes_ccm_decry_compare_selftest(void)
{
    /* Same RFC 3610 test vector #1 as aes_ccm_compare_selftest() */
    static const uint8_t key[16] =
    {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    static const uint8_t nonce[13] =
    {
        0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    static const uint8_t plaintext[23] =
    {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };
    /* hdr || enc_data || mic, from RFC 3610 test vector #1 */
    static const uint8_t ciphertext[39] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
        0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
        0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84, 0x17,
        0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0
    };
    const uint32_t hdr_len = 8;
    const uint32_t data_len = sizeof(plaintext);
    const uint32_t mlen = 8;

    hosal_aes_ccm_dev_t sw_dev;
    uint8_t  sw_out[sizeof(plaintext)];
    uint32_t sw_out_len;

    struct aes_ccm_decryption_packet_rf rf_packet;
    uint8_t  rf_out[sizeof(plaintext)];
    uint32_t rf_out_len;
    uint8_t  tampered[sizeof(ciphertext)];

    /* 1. Local host decrypt+verify path (HOSAL) */
    sw_dev.bit = HOSAL_AES_128_BIT;
    sw_dev.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    sw_dev.key_ptr = (uint8_t *)key;
    sw_dev.payload_buf = (uint8_t *)ciphertext;
    sw_dev.nonce = (uint8_t *)nonce;
    sw_dev.hdr_len = hdr_len;
    sw_dev.data_len = data_len;
    sw_dev.mlen = mlen;
    sw_out_len = sizeof(sw_out);
    sw_dev.out_buf = sw_out;
    sw_dev.out_buf_len = &sw_out_len;

    if (hosal_crypto_aes_ccm_operation(&sw_dev) != STATUS_SUCCESS)
    {
        printf("[AES CCM Decrypt Compare] local host decrypt/verify failed\r\n");
        return;
    }

    if ((sw_out_len != data_len) || (memcmp(sw_out, plaintext, data_len) != 0))
    {
        printf("[AES CCM Decrypt Compare] local host decrypt result MISMATCHES known plaintext\r\n");
        return;
    }

    /* 2. RT569S decrypt-loopback path, same key/nonce/ciphertext */
    if (!aes_ccm_key_set_rf(128, key))
    {
        printf("[AES CCM Decrypt Compare] RT569S key-set request failed\r\n");
        return;
    }

    rf_packet.payload_buf = (uint8_t *)ciphertext;
    rf_packet.nonce = (uint8_t *)nonce;
    rf_packet.hdr_len = hdr_len;
    rf_packet.data_len = data_len;
    rf_packet.mlen = mlen;
    rf_out_len = sizeof(rf_out);
    rf_packet.out_buf = rf_out;
    rf_packet.out_buf_len = &rf_out_len;

    if (!aes_ccm_decryption_rf(&rf_packet))
    {
        printf("[AES CCM Decrypt Compare] RT569S loop-back decrypt/verify failed\r\n");
        return;
    }

    if ((rf_out_len != sw_out_len) || (memcmp(rf_out, sw_out, sw_out_len) != 0))
    {
        printf("[AES CCM Decrypt Compare] MISMATCH between local host and RT569S loop-back plaintext\r\n");
        return;
    }

    printf("[AES CCM Decrypt Compare] MATCH: local host and RT569S loop-back both recovered the "
           "correct plaintext (%lu bytes)\r\n", (unsigned long)sw_out_len);

    /* 3. Negative case: corrupt one ciphertext byte, RT569S must reject it */
    memcpy(tampered, ciphertext, sizeof(tampered));
    tampered[hdr_len] ^= 0x01;   /* flip a bit in the first encrypted byte */

    rf_packet.payload_buf = tampered;
    rf_out_len = sizeof(rf_out);

    if (aes_ccm_decryption_rf(&rf_packet))
    {
        printf("[AES CCM Decrypt Compare] FAIL: RT569S loop-back accepted a tampered packet!\r\n");
        return;
    }

    printf("[AES CCM Decrypt Compare] OK: RT569S loop-back correctly rejected a tampered packet\r\n");
}

/*
 * Boundary/stress self-test: round-trips a near-AES_CCM_RF_MAX_PAYLOAD
 * (2047 byte) data through the RT569S loop-back path only -- encrypts via
 * aes_ccm_encryption_rf(), then decrypts the result back via
 * aes_ccm_decryption_rf() -- and checks the recovered plaintext matches
 * the original byte-for-byte.
 *
 * RF-loop-back-only, not cross-checked against the local host path:
 * the local CCM path is documented as limited to hdr_len+data_len < 480
 * bytes, well under the 2047-byte HW register limit this exercises.
 *
 * Uses hdr_len=0 so the full 2047 bytes goes to data_len (the field
 * "input data" naturally refers to); a_data_len and m_data_len are
 * independent 11-bit registers so a_data could separately be pushed to
 * 2047 too, but that combination isn't what's exercised here.
 *
 * Same bench/idle-time-only constraint as the other self-tests here.
 */
static void aes_ccm_max_size_roundtrip_selftest(void)
{
    static const uint8_t key[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    static const uint8_t nonce[13] =
    {
        0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    static const uint8_t empty_hdr[1] = { 0 };
    const uint32_t data_len = 2047;   /* AES_CCM_RF_MAX_PAYLOAD */
    const uint32_t mlen = 4;

    static uint8_t plaintext[2047];
    static uint8_t ciphertext[2047 + 16];
    static uint8_t decrypted[2047];
    uint32_t i;

    struct aes_ccm_encryption_packet_rf enc_packet;
    struct aes_ccm_decryption_packet_rf dec_packet;
    uint32_t cipher_len;
    uint32_t plain_len;

    for (i = 0; i < data_len; i++)
    {
        plaintext[i] = (uint8_t)(i & 0xFF);
    }

    if (!aes_ccm_key_set_rf(128, key))
    {
        printf("[AES CCM Max-Size] RT569S key-set request failed\r\n");
        return;
    }

    enc_packet.nonce = (uint8_t *)nonce;
    enc_packet.hdr = (uint8_t *)empty_hdr;
    enc_packet.hdr_len = 0;
    enc_packet.data = plaintext;
    enc_packet.data_len = data_len;
    enc_packet.mlen = mlen;
    cipher_len = sizeof(ciphertext);
    enc_packet.out_buf = ciphertext;
    enc_packet.out_buf_len = &cipher_len;

    if (!aes_ccm_encryption_rf(&enc_packet))
    {
        printf("[AES CCM Max-Size] encrypt request failed\r\n");
        return;
    }

    if (cipher_len != (data_len + mlen))
    {
        printf("[AES CCM Max-Size] unexpected ciphertext length %lu (expected %lu)\r\n",
               (unsigned long)cipher_len, (unsigned long)(data_len + mlen));
        return;
    }

    dec_packet.payload_buf = ciphertext;
    dec_packet.nonce = (uint8_t *)nonce;
    dec_packet.hdr_len = 0;
    dec_packet.data_len = data_len;
    dec_packet.mlen = mlen;
    plain_len = sizeof(decrypted);
    dec_packet.out_buf = decrypted;
    dec_packet.out_buf_len = &plain_len;

    if (!aes_ccm_decryption_rf(&dec_packet))
    {
        printf("[AES CCM Max-Size] decrypt/verify request failed\r\n");
        return;
    }

    if ((plain_len != data_len) || (memcmp(decrypted, plaintext, data_len) != 0))
    {
        printf("[AES CCM Max-Size] FAIL: recovered plaintext does not match original %lu-byte input\r\n",
               (unsigned long)data_len);
        return;
    }

    printf("[AES CCM Max-Size] OK: %lu-byte encrypt->decrypt round trip via RT569S loop-back matches "
           "original input exactly\r\n", (unsigned long)data_len);
}

/*
 * Performance comparison: encrypts the same 1000-byte payload via both CCM
 * paths back-to-back, bracketing one GPIO per path around each call so the
 * two durations can be measured externally (oscilloscope/logic analyzer):
 *   - local, host: hosal_crypto_aes_ccm_operation() -- GPIO28 (CPU + HW AES accelerator)
 *   - RT569S loop-back: aes_ccm_encryption_rf()          -- GPIO29
 *
 * The local host CCM engine enforces hdr_len+data_len+mlen <= 1024
 * bytes total, so 1000 with hdr_len=0/mlen=4 leaves headroom while both
 * paths still see the exact same input. rf_aes_ccm.c's own internal
 * GPIO28/29 timing instrumentation (AES_CCM_RF_TIMING_MEASUREMENT) is off,
 * so each pin carries one clean pulse per call.
 *
 * Correctness is checked too (not just timed), so a broken path shows up
 * as a printed failure rather than a silently wrong timing number.
 *
 * Same bench/idle-time-only constraint as the other self-tests here.
 */
static void aes_ccm_perf_encrypt_selftest(void)
{
    static const uint8_t key[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    static const uint8_t nonce[13] =
    {
        0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    static const uint8_t empty_hdr[1] = { 0 };
    const uint32_t data_len = 1000;
    const uint32_t mlen = 4;

    static uint8_t data[1000];
    static uint8_t sw_out[1000 + 4];
    static uint8_t rf_out[1000 + 4];
    uint32_t i;

    hosal_aes_ccm_dev_t sw_dev;
    uint32_t sw_out_len;
    uint32_t sw_status;

    struct aes_ccm_encryption_packet_rf rf_packet;
    uint32_t rf_out_len;
    bool rf_ok;

    for (i = 0; i < data_len; i++)
    {
        data[i] = (uint8_t)(i & 0xFF);
    }

    /* 1. Local host CCM path (HOSAL) -- GPIO28 brackets the call */
    sw_dev.bit = HOSAL_AES_128_BIT;
    sw_dev.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    sw_dev.key_ptr = (uint8_t *)key;
    sw_dev.nonce = (uint8_t *)nonce;
    sw_dev.hdr = (uint8_t *)empty_hdr;
    sw_dev.hdr_len = 0;
    sw_dev.data = data;
    sw_dev.data_len = data_len;
    sw_dev.mlen = mlen;
    sw_out_len = sizeof(sw_out);
    sw_dev.out_buf = sw_out;
    sw_dev.out_buf_len = &sw_out_len;

    gpio_pin_set(GPIO28);
    sw_status = hosal_crypto_aes_ccm_operation(&sw_dev);
    gpio_pin_clear(GPIO28);

    if (sw_status != STATUS_SUCCESS)
    {
        printf("[AES CCM Perf Encrypt] local host path failed\r\n");
        return;
    }

    /* 2. RT569S loop-back path, same key/nonce/data -- GPIO29 brackets the call */
    if (!aes_ccm_key_set_rf(128, key))
    {
        printf("[AES CCM Perf Encrypt] RT569S key-set request failed\r\n");
        return;
    }

    rf_packet.nonce = (uint8_t *)nonce;
    rf_packet.hdr = (uint8_t *)empty_hdr;
    rf_packet.hdr_len = 0;
    rf_packet.data = data;
    rf_packet.data_len = data_len;
    rf_packet.mlen = mlen;
    rf_out_len = sizeof(rf_out);
    rf_packet.out_buf = rf_out;
    rf_packet.out_buf_len = &rf_out_len;

    gpio_pin_set(GPIO29);
    rf_ok = aes_ccm_encryption_rf(&rf_packet);
    gpio_pin_clear(GPIO29);

    if (!rf_ok)
    {
        printf("[AES CCM Perf Encrypt] RT569S loop-back request failed\r\n");
        return;
    }

    /* 3. Correctness check */
    if ((rf_out_len != sw_out_len) || (memcmp(rf_out, sw_out, sw_out_len) != 0))
    {
        printf("[AES CCM Perf Encrypt] MISMATCH between local host and RT569S loop-back results\r\n");
        return;
    }

    printf("[AES CCM Perf Encrypt] MATCH: both paths encrypted %lu bytes correctly -- "
           "see GPIO28 (host aes-ccm) / GPIO29 (radio aes-ccm) for timing\r\n",
           (unsigned long)data_len);
}

/*
 * Performance comparison: decrypts+verifies the same 1000-byte CCM packet
 * (produced once via the host path, untimed) via both decrypt paths
 * back-to-back, with the same pin assignment as
 * aes_ccm_perf_encrypt_selftest() above:
 *   - local, host: hosal_crypto_aes_ccm_operation() -- GPIO28 (CPU + HW AES accelerator)
 *   - RT569S loop-back: aes_ccm_decryption_rf()          -- GPIO29
 *
 * Correctness is checked too (recovered plaintext must match the
 * original), same bench/idle-time-only constraint as the other
 * self-tests here.
 */
static void aes_ccm_perf_decrypt_selftest(void)
{
    static const uint8_t key[16] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    static const uint8_t nonce[13] =
    {
        0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    static const uint8_t empty_hdr[1] = { 0 };
    const uint32_t data_len = 1000;
    const uint32_t mlen = 4;

    static uint8_t plaintext[1000];
    static uint8_t ciphertext[1000 + 4];
    static uint8_t sw_out[1000];
    static uint8_t rf_out[1000];
    uint32_t i;

    hosal_aes_ccm_dev_t enc_dev;
    uint32_t cipher_len;

    hosal_aes_ccm_dev_t sw_dev;
    uint32_t sw_out_len;
    uint32_t sw_status;

    struct aes_ccm_decryption_packet_rf rf_packet;
    uint32_t rf_out_len;
    bool rf_ok;

    for (i = 0; i < data_len; i++)
    {
        plaintext[i] = (uint8_t)(i & 0xFF);
    }

    /* 0. Build a valid ciphertext once via the host path (untimed setup) */
    enc_dev.bit = HOSAL_AES_128_BIT;
    enc_dev.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    enc_dev.key_ptr = (uint8_t *)key;
    enc_dev.nonce = (uint8_t *)nonce;
    enc_dev.hdr = (uint8_t *)empty_hdr;
    enc_dev.hdr_len = 0;
    enc_dev.data = plaintext;
    enc_dev.data_len = data_len;
    enc_dev.mlen = mlen;
    cipher_len = sizeof(ciphertext);
    enc_dev.out_buf = ciphertext;
    enc_dev.out_buf_len = &cipher_len;

    if (hosal_crypto_aes_ccm_operation(&enc_dev) != STATUS_SUCCESS)
    {
        printf("[AES CCM Perf Decrypt] setup: local host encrypt failed\r\n");
        return;
    }

    /* 1. Local host decrypt+verify path (HOSAL) -- GPIO28 brackets the call */
    sw_dev.bit = HOSAL_AES_128_BIT;
    sw_dev.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    sw_dev.key_ptr = (uint8_t *)key;
    sw_dev.payload_buf = ciphertext;
    sw_dev.nonce = (uint8_t *)nonce;
    sw_dev.hdr_len = 0;
    sw_dev.data_len = data_len;
    sw_dev.mlen = mlen;
    sw_out_len = sizeof(sw_out);
    sw_dev.out_buf = sw_out;
    sw_dev.out_buf_len = &sw_out_len;

    gpio_pin_set(GPIO28);
    sw_status = hosal_crypto_aes_ccm_operation(&sw_dev);
    gpio_pin_clear(GPIO28);

    if (sw_status != STATUS_SUCCESS)
    {
        printf("[AES CCM Perf Decrypt] local host decrypt/verify failed\r\n");
        return;
    }

    if ((sw_out_len != data_len) || (memcmp(sw_out, plaintext, data_len) != 0))
    {
        printf("[AES CCM Perf Decrypt] local host decrypt result MISMATCHES original plaintext\r\n");
        return;
    }

    /* 2. RT569S decrypt-loopback path, same key/nonce/ciphertext -- GPIO29 brackets the call */
    if (!aes_ccm_key_set_rf(128, key))
    {
        printf("[AES CCM Perf Decrypt] RT569S key-set request failed\r\n");
        return;
    }

    rf_packet.payload_buf = ciphertext;
    rf_packet.nonce = (uint8_t *)nonce;
    rf_packet.hdr_len = 0;
    rf_packet.data_len = data_len;
    rf_packet.mlen = mlen;
    rf_out_len = sizeof(rf_out);
    rf_packet.out_buf = rf_out;
    rf_packet.out_buf_len = &rf_out_len;

    gpio_pin_set(GPIO29);
    rf_ok = aes_ccm_decryption_rf(&rf_packet);
    gpio_pin_clear(GPIO29);

    if (!rf_ok)
    {
        printf("[AES CCM Perf Decrypt] RT569S loop-back decrypt/verify failed\r\n");
        return;
    }

    if ((rf_out_len != data_len) || (memcmp(rf_out, plaintext, data_len) != 0))
    {
        printf("[AES CCM Perf Decrypt] MISMATCH between RT569S loop-back plaintext and original\r\n");
        return;
    }

    printf("[AES CCM Perf Decrypt] MATCH: both paths recovered %lu bytes correctly -- "
           "see GPIO28 (host aes-ccm) / GPIO29 (radio aes-ccm) for timing\r\n",
           (unsigned long)data_len);
}

void set_priotity(void)
{
    //NVIC_SetPriority(Gpio_IRQn,             IRQ_PRIORITY_LOW);
    NVIC_SetPriority(CommSubsystem_IRQn,    IRQ_PRIORITY_HIGH);
    NVIC_SetPriority(Uart0_IRQn,            IRQ_PRIORITY_HIGH);
}


/*this is pin mux setting*/
void init_default_pin_mux(void)
{
    /* UART0 (uart_stdio) pin mux is handled inside hosal_uart_init -- do
     * not mux GPIO16/17 here too. */

    /* GPIO28/29 as GPIO debug-timing outputs: the perf self-tests bracket
     * GPIO28 (host CCM) and GPIO29 (RT569S loop-back CCM). */
    pin_set_mode(GPIO28, MODE_GPIO);
    gpio_cfg_output(GPIO28);
    gpio_pin_clear(GPIO28);

    pin_set_mode(GPIO29, MODE_GPIO);
    gpio_cfg_output(GPIO29);
    gpio_pin_clear(GPIO29);

    /* GPIO 0~7 as Debug Port (P0) */
    for (uint32_t pin = 0 ; pin < 8 ; pin++)
    {
        pin_set_mode(pin, MODE_DBG0 + pin);
    }
    pin_set_mode(GPIO20, MODE_DBG2);     /* GPIO20 as DIO */
    pin_set_mode(GPIO21, MODE_DBG3);     /* GPIO21 as DIO */

    SYSCTRL->sys_test.bit.dbg_out_sel = 25;
}


int main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();

    /*init gpio pin setting*/
    init_default_pin_mux();

    /* RF system priority set */
    set_priotity();

    hosal_crypto_aes_ccm_init();

    uasb_init_cmd_interface(RF_FW_SELECT, true);

    /* RF system sleep disable -- the loop-back self-tests below are
     * bench/idle-time-only and must not race a radio sleep transition. */
    rf_set_rf_sleep_direct(0);   /* EnableFlag: 0 = Leave Sleep */

    printf("[Init]: AES CCM RF Test - comm subsystem init done\r\n");

    /* Comm subsystem is up and nothing else has touched BMU TX queue 6 yet
     * -- the only window in which it's guaranteed idle. */
    aes_ccm_compare_selftest();
    aes_ccm_decry_compare_selftest();
    aes_ccm_max_size_roundtrip_selftest();
    aes_ccm_perf_encrypt_selftest();
    aes_ccm_perf_decrypt_selftest();

    printf("[Done]: AES CCM RF self-test complete\r\n");

    /* Not starting the scheduler: every self-test above runs synchronously
     * via blocking polls (RfMcu_RxQueueCheck() / RfMcu_TxQueueSendById()),
     * so FreeRTOS doesn't need to be running for them. */
    while (1) {};
}
