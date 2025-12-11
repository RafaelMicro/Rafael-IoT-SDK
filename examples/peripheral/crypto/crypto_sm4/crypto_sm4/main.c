/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_sm4.h"
#include "uart_stdio.h"


void sm4_test_encode(void) {

    uint32_t   i;
    hosal_sm4_dev_t sm4_dev;
    uint8_t mkey[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };


    uint8_t plain_text[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };

    uint8_t cypher_text[16];

    uint8_t  expect_once_cypher_text[16] = {
        0x68, 0x1E, 0xDF, 0x34, 0xD2, 0x06, 0x96, 0x5E,
        0x86, 0xB3, 0xE9, 0x4F, 0x53, 0x6E, 0x42, 0x46
    };

    uint8_t expect_1m_cypher_text[16] = {
        0x59, 0x52, 0x98, 0xC7, 0xC6, 0xFD, 0x27, 0x1F,
        0x04, 0x02, 0xF8, 0x04, 0xC3, 0x3D, 0x3F, 0x66
    };

    printf("encode SM test vector once \r\n");



    sm4_dev.crypto_operation =  HOSAL_SM4_ENCODE;
    sm4_dev.out_ptr = cypher_text;
    sm4_dev.in_ptr = plain_text;
    sm4_dev.mkey_ptr = mkey;
    sm4_dev.loop = 1;
    hosal_crypto_sm4_operation(&sm4_dev);

    printf("sm4 result: \r\n");

    for (i = 0; i < 16; i++) {
        printf("%02X ", cypher_text[i]);
    }

    printf("\r\n");

    if (memcmp(expect_once_cypher_text, cypher_text, 16) != 0) {
        /*almost impossible*/
        printf("Oops, wrong vector check why\r\n");
        while (1);
    } else {
        printf("sm4 encode once correct \r\n");
    }

    printf("encode SM test vector 1000000 times \r\n");
    printf("need to wait 3 min \r\n");
    /*test encode 1000000 times */
    sm4_dev.crypto_operation =  HOSAL_SM4_ENCODE;
    sm4_dev.out_ptr = plain_text;
    sm4_dev.in_ptr = plain_text;
    sm4_dev.mkey_ptr = mkey;
    sm4_dev.loop = 1000000;
    hosal_crypto_sm4_operation(&sm4_dev);

    printf("sm4 result: \r\n");

    for (i = 0; i < 16; i++) {
        printf("%02X ", plain_text[i]);
    }

    printf("\r\n");

    if (memcmp(expect_1m_cypher_text, plain_text, 16) != 0) {
        /*almost impossible*/
        printf("Oops, wrong vector check why\r\n");
        while (1);
    } else {
        printf("sm4 encode 1000000 times correct \r\n");
    }


}

void sm4_test_decode(void) {

    uint32_t   i;
    hosal_sm4_dev_t sm4_dev;

    uint8_t mkey[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };

    uint8_t plain_text[16] ;

    uint8_t cypher_text[16] = {
        0x68, 0x1E, 0xDF, 0x34, 0xD2, 0x06, 0x96, 0x5E,
        0x86, 0xB3, 0xE9, 0x4F, 0x53, 0x6E, 0x42, 0x46
    };


    uint8_t cypher_1m_text[16] = {
        0x59, 0x52, 0x98, 0xC7, 0xC6, 0xFD, 0x27, 0x1F,
        0x04, 0x02, 0xF8, 0x04, 0xC3, 0x3D, 0x3F, 0x66
    };


    const uint8_t  expect_plain_text[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };

    printf("decode SM test vector once \r\n");

    sm4_dev.crypto_operation =  HOSAL_SM4_DECODE;
    sm4_dev.out_ptr = plain_text;
    sm4_dev.in_ptr = cypher_text;
    sm4_dev.mkey_ptr = mkey;
    sm4_dev.loop = 1;
    hosal_crypto_sm4_operation(&sm4_dev);

    printf("sm4 result: \r\n");

    for (i = 0; i < 16; i++) {
        printf("%02X ", plain_text[i]);
    }

    printf("\r\n");

    if (memcmp(expect_plain_text, plain_text, 16) != 0) {
        /*almost impossible*/
        printf("Oops, wrong vector check why\r\n");
        while (1);
    } else {
        printf("sm4 decode once correct \r\n");;
    }

    printf("decode SM test vector 1000000 times \r\n");
    printf("need to wait 3 min \r\n");
    /*test decode 1000000 times */
    sm4_dev.crypto_operation =  HOSAL_SM4_DECODE;
    sm4_dev.out_ptr = cypher_1m_text;
    sm4_dev.in_ptr = cypher_1m_text;
    sm4_dev.mkey_ptr = mkey;
    sm4_dev.loop = 1000000;
    hosal_crypto_sm4_operation(&sm4_dev);

    printf("sm4 result: \r\n");

    for (i = 0; i < 16; i++) {
        printf("%02X ", cypher_1m_text[i]);
    }

    printf("\r\n");

    if (memcmp(expect_plain_text, cypher_1m_text, 16) != 0) {
        /*almost impossible*/
        printf("Oops, wrong vector check why\r\n");
        while (1);
    } else {
        printf("sm4 decode 1000000 times correct \r\n");
    }

}

int main(void) {

    
    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto SM4 decode demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_init();

    sm4_test_encode();

    sm4_test_decode();

    printf("\r\n\r\n");
    printf("hosal crypto SM4 finish \r\n");

    while (1) {;}

}

