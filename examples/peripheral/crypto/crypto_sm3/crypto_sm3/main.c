/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_sysctrl.h"
#include "hosal_crypto_sm3.h"
#include "uart_stdio.h"


uint8_t    sm3_digest[32], sm3_digest2[32], sm3_digest_caseA[32];
/*The following test pattern is listed in SM3 document*/
const uint8_t   expect_sm3_hash[2][32] = {
    /*abc*/
    {
        0x66, 0xC7, 0xF0, 0xF4, 0x62, 0xEE, 0xED, 0xD9, 0xD1, 0xF2, 0xD4, 0x6B, 0xDC, 0x10, 0xE4, 0xE2,
        0x41, 0x67, 0xC4, 0x87, 0x5C, 0xF2, 0xF7, 0xA2, 0x29, 0x7D, 0xA0, 0x2B, 0x8F, 0x4B, 0xA8, 0xE0
    },

    /* ("abcd" repeat 16 times)*/
    {
        0xDE, 0xBE, 0x9F, 0xF9, 0x22, 0x75, 0xB8, 0xA1, 0x38, 0x60, 0x48, 0x89, 0xC1, 0x8E, 0x5A, 0x4D,
        0x6F, 0xDB, 0x70, 0xE5, 0x38, 0x7E, 0x57, 0x65, 0x29, 0x3D, 0xCB, 0xA3, 0x9C, 0x0C, 0x57, 0x32
    }
};



int main(void) {

    uint8_t            test[64], temp[4];
    uint32_t           i, error = 0;
    uint8_t*  ptr;

    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto SM3 demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    for (i = 0; i < 26; i++) {
        test[i] = 0x61 + i;
    }

    temp[0] = 0x64;
    temp[1] = 0x61;
    temp[2] = 0x62;
    temp[3] = 0x63;


    hosal_sm3_dev_t sm3_dev;
    sm3_dev.crypto_operation = HOSAL_SM3_DIGEST;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 3;
    sm3_dev.out_ptr = sm3_digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_DIGEST;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 3;
    sm3_dev.out_ptr = sm3_digest2;
    hosal_crypto_sm3_operation(&sm3_dev);


    for (i = 0; i < 16; i++) {
        test[ (i * 4)] = 0x61;
        test[ (i * 4) + 1] = 0x62;
        test[ (i * 4) + 2] = 0x63;
        test[ (i * 4) + 3] = 0x64;
    }

    sm3_dev.crypto_operation = HOSAL_SM3_DIGEST;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 64;
    sm3_dev.out_ptr = sm3_digest_caseA;
    hosal_crypto_sm3_operation(&sm3_dev);


    ptr = (uint8_t*) & (expect_sm3_hash[0]);

    printf("sm3 for abc is : \r\n");


    for (i = 0; i < 32; i++) {

        printf("%02X", sm3_digest[i]);

        if (sm3_digest[i] != *ptr++) {
            error = 1;
        }

    }

    if (error == 1) {
        printf("\nOops... check SM3 abc data error \r\n");
        while (1);
    }
    printf("\r\n");

    printf("sm3 for abc again is :\r\n");

    ptr = (uint8_t*) & (expect_sm3_hash[0]);

    for (i = 0; i < 32; i++) {
        printf("%02X", sm3_digest2[i]);

        if (sm3_digest2[i] != *ptr++) {
            error = 1;
        }
    }

    if (error == 1) {
        printf("Oops... check SM3 abc data error \r\n");
        while (1);
    }

    printf("\r\n");

    printf("sm3 hash abcd repeat 16 times is: \r\n");

    ptr = (uint8_t*) & (expect_sm3_hash[1]);

    for (i = 0; i < 32; i++) {
        printf("%02X", sm3_digest_caseA[i]);

        if (sm3_digest_caseA[i] != *ptr++) {
            error = 1;
        }
    }

    if (error == 1) {
        printf("\nOops... check SM3 abcd repeat 16 times data error \r\n");
        while (1);
    }

    printf("\r\n");

    sm3_dev.crypto_operation = HOSAL_SM3_DIGEST;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 1;
    sm3_dev.out_ptr = sm3_digest_caseA;
    hosal_crypto_sm3_operation(&sm3_dev);

    for (i = 0; i < 32; i++) {
        printf("%02X", sm3_digest_caseA[i]);

        if (sm3_digest_caseA[i] != *ptr++) {
            error = 1;
        }
    }

    printf("\r\n");


    sm3_dev.crypto_operation = HOSAL_SM3_DIGEST;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 5;
    sm3_dev.out_ptr = sm3_digest_caseA;
    hosal_crypto_sm3_operation(&sm3_dev);

    for (i = 0; i < 32; i++) {
        printf("%02X", sm3_digest_caseA[i]);

        if (sm3_digest_caseA[i] != *ptr++) {
            error = 1;
        }
    }

    printf("\r\n");

    sm3_dev.crypto_operation = HOSAL_SM3_DIGEST;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 7;
    sm3_dev.out_ptr = sm3_digest_caseA;
    hosal_crypto_sm3_operation(&sm3_dev);

    for (i = 0; i < 32; i++) {
        printf("%02X", sm3_digest_caseA[i]);

        if (sm3_digest_caseA[i] != *ptr++) {
            error = 1;
        }
    }

    printf("\r\n");


    printf("again sm3 for abcdabc is :\r\n");
    /*the following code is test non-alignment */
    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_ptr = test;
    sm3_dev.in_length = 3;
    sm3_dev.out_ptr = sm3_digest_caseA;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_ptr = temp;
    sm3_dev.in_length = 4;
    sm3_dev.out_ptr = sm3_digest_caseA;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    hosal_crypto_sm3_operation(&sm3_dev);

    for (i = 0; i < 32; i++) {
        printf("%02X", sm3_digest_caseA[i]);

        if (sm3_digest_caseA[i] != *ptr++) {
            error = 1;
        }
    }

    printf("\r\n\r\n");
    printf("hosal crypto SM3 finish \r\n");
    
    while (1);
}

