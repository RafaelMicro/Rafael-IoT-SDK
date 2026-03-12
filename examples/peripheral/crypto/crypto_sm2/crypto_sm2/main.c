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
#include "hosal_crypto_ecc.h"
#include "hosal_crypto_sm3.h"
#include "uart_stdio.h"



void lecp_print_hex_bytes(const char* name, const uint8_t* data,
                          uint32_t size) {
    uint32_t i = 0;

    printf("%s = 0x", name);

    while (i < size) {
        printf("%02X", data[i++]);
    }

    printf("\r\n");
}


void becp_print_hex_bytes(const char* name, const uint8_t* data,
                          uint32_t size) {
    uint32_t i = 0;

    printf("%s = 0x", name);

    while (i < size) {
        printf("%02X", data[(size - 1) - i]);
        i++;

        if ((i & 3) == 0) {
            printf(" ");
        }
    }

    printf("\r\n");
}

/************************************************************/
void ecc_gfp_sm2p192_test1(void) {

    /*
     *  sm2 p192
     *  Alice private key = 0x3AC0E717EB61602EFCBB1DE81AA144A272B44BA1F16936AC
     *
     *  Bob private key = 0x25FBB32EFBEC6ECB1314332A026582DB7BE00C051CF2FA80
     *
     *  Remark: This is demo sample only.
     *  HERE PRIVATE KEY IS TEST ONLY. Private Key should be random generated.
     *  It can NOT be hardcode, otherwise key will be extracted from binary or hex file.
     *
     *     Please Notice: Share key is in little endian format...
     *  So if you want to use the share key for some cryption, please notice
     *  key format is little-endian or big-endian
     *
     *
     *
     */

    /*Remark: data is little endian format. that is MSB is the last byte. */

    uint32_t Alice_privK[secp192r1_op_num] = { 0xF16936AC, 0x72B44BA1, 0x1AA144A2, 0xFCBB1DE8, 0xEB61602E, 0x3AC0E717};

    //Public key for Alice.
    uint32_t Alice_Public_x[secp192r1_op_num] ;
    uint32_t Alice_Public_y[secp192r1_op_num] ;

    uint32_t Bob_privK[secp192r1_op_num] = { 0x1CF2FA80, 0x7BE00C05, 0x026582DB, 0x1314332A, 0xFBEC6ECB, 0x25FBB32E};

    //Public key for Bob.
    uint32_t Bob_Public_x[secp192r1_op_num];
    uint32_t Bob_Public_y[secp192r1_op_num];

    /*
     *Share key for Alice
     */
    uint32_t Alice_Share_x[secp192r1_op_num];
    uint32_t Alice_Share_y[secp192r1_op_num];

    //Share key for Bob.
    uint32_t Bob_Share_x[secp192r1_op_num];
    uint32_t Bob_Share_y[secp192r1_op_num];
    int status;
    hosal_crypto_ecc_gf_t ecc_gf;
    /*
     * Expected Share key
     */
    const uint32_t Expected_Share_x[secp192r1_op_num] =
    { 0x2450A425, 0x0B5AFC67, 0xB7B09824, 0xA1688C6E, 0x3080F6B5, 0x3A74DDFA };
    const uint32_t Expected_Share_y[secp192r1_op_num] =
    { 0xACE3F3F6, 0xA90F2F9E, 0xC6C72BD3, 0xB30CD24A, 0xA653D6E1, 0x7FF89712 };

    //gfp_ecc_curve_sm2p192_init();
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P192_INIT);
    /*Alice uses private key to generate Alice's public key*/
    //gfp_point_p192_mult(Alice_Public_x, Alice_Public_y, (uint32_t *) Curve_Gx_sm2p192, (uint32_t *) Curve_Gy_sm2p192, Alice_privK);

    ecc_gf.crypto_operation = HOSAL_GFP_P192_MULTI;
    ecc_gf.p_result_x = Alice_Public_x;
    ecc_gf.p_result_y = Alice_Public_y;
    ecc_gf.target_x = (uint32_t*) Curve_Gx_sm2p192;
    ecc_gf.target_y = (uint32_t*) Curve_Gy_sm2p192;
    ecc_gf.target_k = Alice_privK;

    status = hosal_crypto_ecc_gf_operation(&ecc_gf);

    //gfp_ecc_curve_sm2p192_init();
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P192_INIT);
    /*Bob uses private key to generate Bob's public key*/
    //gfp_point_p192_mult(Bob_Public_x, Bob_Public_y, (uint32_t *) Curve_Gx_sm2p192, (uint32_t *) Curve_Gy_sm2p192, Bob_privK);
    ecc_gf.crypto_operation = HOSAL_GFP_P192_MULTI;
    ecc_gf.p_result_x = Bob_Public_x;
    ecc_gf.p_result_y = Bob_Public_y;
    ecc_gf.target_x = (uint32_t*) Curve_Gx_sm2p192;
    ecc_gf.target_y = (uint32_t*) Curve_Gy_sm2p192;
    ecc_gf.target_k = Bob_privK;
    status = hosal_crypto_ecc_gf_operation(&ecc_gf);
    /*Alice sends her public key to Bob, and Bob sends his public key to Alice*/
    //gfp_ecc_curve_sm2p192_init();
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P192_INIT);
    /*Alice caculate the secert shared key*/
    //gfp_point_p192_mult(Alice_Share_x, Alice_Share_y, Bob_Public_x, Bob_Public_y, Alice_privK);

    ecc_gf.crypto_operation = HOSAL_GFP_P192_MULTI;
    ecc_gf.p_result_x = Alice_Share_x;
    ecc_gf.p_result_y = Alice_Share_y;
    ecc_gf.target_x = Bob_Public_x;
    ecc_gf.target_y = Bob_Public_y;
    ecc_gf.target_k = Alice_privK;
    status = hosal_crypto_ecc_gf_operation(&ecc_gf);

    //gfp_ecc_curve_sm2p192_init();
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P192_INIT);
    /*Bob caculate the secert shared key*/
    //gfp_point_p192_mult(Bob_Share_x, Bob_Share_y, Alice_Public_x, Alice_Public_y, Bob_privK);

    ecc_gf.crypto_operation = HOSAL_GFP_P192_MULTI;
    ecc_gf.p_result_x = Bob_Share_x;
    ecc_gf.p_result_y = Bob_Share_y;
    ecc_gf.target_x = Alice_Public_x;
    ecc_gf.target_y = Alice_Public_y;
    ecc_gf.target_k = Bob_privK;
    status = hosal_crypto_ecc_gf_operation(&ecc_gf);


    if (((0 == memcmp((char*) Alice_Share_x, (char*) Expected_Share_x, 24)) &&
         (0 == memcmp((char*) Alice_Share_y, (char*) Expected_Share_y, 24)))  &&
        ((0 == memcmp((char*) Bob_Share_x, (char*) Expected_Share_x, 24)) &&
         (0 == memcmp((char*) Bob_Share_y, (char*) Expected_Share_y, 24) ))) {
        printf("SM2 ECDH test: SUCCESS!\r\n");
    } else {
        printf("SM2 ECDH test: FAILURE!\r\n");
    }

}

void ecc_gfp_sm2p192_test2(void) {
    /*Remark: data is little endian format. that is MSB is the last byte. */
    uint32_t k[secp192r1_op_num] = { 0xF16936AC, 0x72B44BA1, 0x1AA144A2, 0xFCBB1DE8, 0xEB61602E, 0x3AC0E717};

    //Public key.
    uint32_t public_x[secp192r1_op_num];
    uint32_t public_y[secp192r1_op_num];

    uint32_t  i, status;
    hosal_crypto_ecc_gf_t ecc_gf;

    for (i = 0; i < 20; i++) {
        //gfp_ecc_curve_sm2p192_init();
        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P192_INIT);
        //status = gfp_point_p192_mult(public_x, public_y, (uint32_t *) Curve_Gx_sm2p192, (uint32_t *) Curve_Gy_sm2p192, k);
        ecc_gf.crypto_operation = HOSAL_GFP_P192_MULTI;
        ecc_gf.p_result_x = public_x;
        ecc_gf.p_result_y = public_y;
        ecc_gf.target_x = (uint32_t*) Curve_Gx_sm2p192;
        ecc_gf.target_y = (uint32_t*) Curve_Gy_sm2p192;
        ecc_gf.target_k = k;
        status = hosal_crypto_ecc_gf_operation(&ecc_gf);

        if (status == STATUS_SUCCESS) {
            becp_print_hex_bytes("sm2p192 x:", (uint8_t*) public_x, 24);
            becp_print_hex_bytes("sm2p192 y:", (uint8_t*) public_y, 24);

            printf("\r\n");
            printf("\r\n");
        } else {
            printf("k could not be zero or rank value \r\n");
            break;
        }

        k[0]++;
    }

}

void ecc_gfp_sm2p192_test3(void) {
    /*Remark: data is little endian format. that is MSB is the last byte. */
    /*The following test vectors are extracted from SM2 Parr4: Parameter Definition*/

    const uint32_t   test_key[2][secp192r1_op_num] = {
        { 0x60355AFD, 0x313455FE, 0x1DFAA1AC, 0xBF67288A, 0x7074F53F, 0x58892B80 },
        { 0xE15B2CB5, 0xD37982A3, 0x30A96204, 0xE7A16543, 0x3073AEEC, 0x384F3035 }
    };

    //result point
    uint32_t public_x[secp192r1_op_num];
    uint32_t public_y[secp192r1_op_num];

    /*the following results are extracted from SM2 Part5 parameter definition. */
    const uint32_t   expected_result_x[2][secp192r1_op_num] = {
        { 0x49F4AF4A, 0x36BCFC81, 0x0D30A565, 0x531508B3, 0x7AC6D100, 0x79F0A954 },
        { 0xFA0D4CF5, 0x83DE4D41, 0x6E0C38D8, 0xDF34DBE7, 0x124294DF, 0x23FC680B }
    };

    const uint32_t   expected_result_y[2][secp192r1_op_num] = {
        { 0x4672F912, 0x994BC792, 0x65A8BCC8, 0x9C19935A, 0x890838DF, 0xAE38F2D8 },
        { 0xDE31EE1F, 0xD31EEFB9, 0x16B16824, 0x777F738D, 0x0DAF0C4D, 0x70CF14F2 }
    };

    uint32_t  i, status, *key;

    uint8_t*   exp_x, *exp_y;
    hosal_crypto_ecc_gf_t ecc_gf;

    printf("\r\n ecc_gfp_sm2p192_test3 : \r\n");


    for (i = 0; i < 2; i++) {
        key = (uint32_t*) & (test_key[i]);

        //gfp_ecc_curve_sm2p192_init();
        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P192_INIT);

        //status = gfp_point_p192_mult(public_x, public_y, (uint32_t *) Curve_Gx_sm2p192, (uint32_t *) Curve_Gy_sm2p192, key);

        ecc_gf.crypto_operation = HOSAL_GFP_P192_MULTI;
        ecc_gf.p_result_x = public_x;
        ecc_gf.p_result_y =  public_y;
        ecc_gf.target_x = (uint32_t*) Curve_Gx_sm2p192;
        ecc_gf.target_y = (uint32_t*) Curve_Gy_sm2p192;
        ecc_gf.target_k = key;
        status = hosal_crypto_ecc_gf_operation(&ecc_gf);
        //
        if (status == STATUS_SUCCESS) {
            becp_print_hex_bytes("sm2p192 key:", (uint8_t*) key, 24);
            becp_print_hex_bytes("sm2p192 x:  ", (uint8_t*) public_x, 24);
            becp_print_hex_bytes("sm2p192 y:  ", (uint8_t*) public_y, 24);

            printf("\r\n");

            /*compare expected value.*/
            exp_x = (uint8_t*) & (expected_result_x[i]);
            exp_y = (uint8_t*) & (expected_result_y[i]);

            if (( memcmp((uint8_t*) public_x, exp_x, (secp192r1_op_num << 2)) != 0) ||
                ( memcmp((uint8_t*) public_y, exp_y, (secp192r1_op_num << 2)) != 0) ) {
                printf("OOPs... test vector mismatched! \r\n");
                while (1);
            }

        } else {
            printf("error \r\n");
            while (1);
        }

    }

    printf("SM2 P192 ECC test vector correct! \r\n");
}






void ecc_gfp_sm2p256r_test1(void) {
    /*Remark: data is little endian format. that is MSB is the last byte. */
    /*The following test vectors are extracted from SM2 Parr5: Parameter Definition*/

    const uint32_t   test_key[4][secp256r1_op_num] = {
        {  0x4DF7C5B8, 0x42FB81EF,  0x2860B51A, 0x88939369,  0xC6D39F95, 0x3F36E38A, 0x7B2144B1, 0x3945208F },
        {  0xEAC1BC21, 0x6D54B80D,  0x3CDBE4CE, 0xEF3CC1FA,  0xD9C02DCC, 0x16680F3A, 0xD506861A, 0x59276E27 },
        {  0x48230029, 0x678418BE,  0x3D6C4AE1, 0x72AE2CD6,  0x5F906952, 0x6DF11649, 0x41BB5AF1, 0x81EB26E9 },
        {  0xEFA229B5, 0xF14AE10D,  0xEB199088, 0xEAADDA6C,  0x56B82338, 0x5437A593, 0x7D45A9EA, 0x78512991 },

    };

    //result point
    ECPoint_P256  result_point;


    /*the following results are extracted from SM2 Part5 parameter definition. */
    const uint32_t   expected_result_x[4][secp256r1_op_num] = {
        { 0x56F35020, 0x6BB08FF3, 0x1833FC07, 0x72179FAD, 0x1E4BC5C6, 0x50DD7D16, 0x1E5421A1, 0x09F9DF31 },
        { 0x2E149A73, 0x4F640ECD, 0x0E073C0F, 0x415E2EDE, 0x8E77FEB6, 0x62043226, 0x8E8D1798, 0x04EBFC71 },
        { 0x49C94232, 0xDB9540AF, 0xE26AA6F6, 0xD3CCF4FF, 0xB96748FB, 0x1DD812FE, 0x7DF4EDB6, 0x160E1289 },
        { 0x76F16DFB, 0x6F7357D5, 0x591B8B56, 0x8BA64C64, 0x2286AF07, 0xB5FA99EB, 0x7C53C7B1, 0x6AE848C5 },

    };

    const uint32_t   expected_result_y[4][secp256r1_op_num] = {
        { 0x2DA9AD13, 0x6632F607, 0xF35E084A, 0x0AED05FB, 0x8CC1AA60, 0x2DC6EA71, 0xE26775A5, 0xCCEA490C },
        { 0x7E124DF0, 0x283AFF76, 0x63094D99, 0x64E6EE6A, 0x8F950A3C, 0x7B36DAAB, 0x1E5430A5, 0xE858F9D8 },
        { 0xB223007F, 0x741B78B4, 0x1BFCF8C4, 0x6649975E, 0x20AA489D, 0x31694BEB, 0xBB9A4595, 0x4A7DAD08 },
        { 0x05621C4D, 0x54DFF693, 0x86F3FBEA, 0x09A92643, 0x2062E9CD, 0x36C5C799, 0x1621A27B, 0xEE489D77 },

    };

    uint32_t  i, status, *key;

    uint8_t*   exp_x, *exp_y;

    /*Curve_Gx_sm2p256 Generator defined in crypto.c*/
    hosal_crypto_ecc_p256_t ecc_p256;
    for (i = 0; i < 4; i++) {
        key = (uint32_t*) & (test_key[i]);

        //gfp_ecc_curve_sm2p256_init();
        //status = gfp_point_p256_mult((ECPoint_P256 *)&result_point, (ECPoint_P256 *) &Curve_Gx_sm2p256, key);

        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);


        ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
        ecc_p256.result = (ECPoint_P256*)&result_point;
        ecc_p256.base = (ECPoint_P256*) &Curve_Gx_sm2p256;
        ecc_p256.p_key = key;
        status = hosal_crypto_ecc_p256(&ecc_p256);

        if (status == STATUS_SUCCESS) {
            becp_print_hex_bytes("sm2p256 key:", (uint8_t*) key, 32);
            becp_print_hex_bytes("sm2p256 x:  ", (uint8_t*) (result_point.x), 32);
            becp_print_hex_bytes("sm2p256 y:  ", (uint8_t*) (result_point.y), 32);

            printf("\r\n");

            /*compare expected value.*/
            exp_x = (uint8_t*) & (expected_result_x[i]);
            exp_y = (uint8_t*) & (expected_result_y[i]);

            if (( memcmp((uint8_t*) (result_point.x), exp_x, (secp256r1_op_num << 2)) != 0)
                ||
                ( memcmp((uint8_t*) (result_point.y), exp_y, (secp256r1_op_num << 2)) != 0) ) {
                printf("OOPs... test vector mismatched! \r\n");
                while (1);
            }

        } else {
            printf("error \r\n");
            while (1);
        }

    }

    printf("SM2 P256 ECC test vector correct! \r\n");
}




/*
 * This test vector is in SM2 Part 5: Parameter definition
 * Notice: All SM2 ECC value is Little Endian. But SM3 caculate in Big Endian.
 *
 */
void ecc_gfp_sm2p256_signature(void) {
    sm3_context     sm3_cntx;

    const uint8_t  ID_A[] = {
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38
    };

    uint8_t  ENTL_A[4];     /* ENTLa is only 2 bytes, we allocate 4 bytes here for alignment*/

    const uint8_t  message[] = {
        'm', 'e', 's', 's', 'a', 'g', 'e', ' ',
        'd', 'i', 'g', 'e', 's', 't'
    };

    uint32_t   private_key[secp256r1_op_num] = {
        0x4DF7C5B8, 0x42FB81EF, 0x2860B51A, 0x88939369,
        0xC6D39F95, 0x3F36E38A, 0x7B2144B1, 0x3945208F
    };

    /* the random key should be generated by hardware. we hardcode it
     * for matched the example in document.
     */

    uint32_t   random_number[secp256r1_op_num] = {
        0xEAC1BC21, 0x6D54B80D, 0x3CDBE4CE, 0xEF3CC1FA,
        0xD9C02DCC, 0x16680F3A, 0xD506861A, 0x59276E27
    };

    const uint32_t  expect_signature_r[secp256r1_op_num] = {
        0xEEE720B3, 0x43AC7EAC, 0x27D5B741, 0x5944DA38,
        0xE1BB81A1, 0x0EEAC513, 0x48D2C463, 0xF5A03B06
    };

    const uint32_t  expect_signature_s[secp256r1_op_num] = {
        0x85BBC1AA, 0x840B69C4, 0x1F7F42D4, 0xBB9038FD,
        0x0D421CA1, 0x763182BC, 0xDF212FD8, 0xB1B6AA29
    };

    ECPoint_P256  A_point;

    ECPoint_P256  signature;


    /* because  data in operation is little endian, but sm3 is big endian.
     * We need a temp swap buffer to save big endian data.
     */
    uint32_t  temp_buffer[secp256r1_op_num];

    uint8_t   digest[SM3_DIGEST_SIZE];
    uint32_t  status;

    ENTL_A[0] = (sizeof(ID_A) >> 5) & 0xFF;     /*in bits, MSB, one byte*/
    ENTL_A[1] = (sizeof(ID_A) << 3) & 0xFF;    /*in bits, LSB, one byte*/


    /*sm2p256 Gx Gy defined in crypto.c*/
    /*caculate A point*/
    hosal_crypto_ecc_p256_t ecc_p256;
    hosal_sm3_dev_t sm3_dev;

    //gfp_ecc_curve_sm2p256_init();
    //status = gfp_point_p256_mult((ECPoint_P256 *) &A_point, (ECPoint_P256 *) &Curve_Gx_sm2p256, private_key);


    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);


    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*)&A_point;
    ecc_p256.base = (ECPoint_P256*) &Curve_Gx_sm2p256;
    ecc_p256.p_key = private_key;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    if (status != STATUS_SUCCESS) {
        /*This error should not happen.*/
        printf("Oops, check what's wrong \r\n");
    }

    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);
    //sm3_init(&sm3_cntx);


    //sm3_update(&sm3_cntx, ENTL_A, 2);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = 2;
    sm3_dev.in_ptr = ENTL_A;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_update(&sm3_cntx, (uint8_t *) ID_A, sizeof(ID_A));

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(ID_A);
    sm3_dev.in_ptr = (uint8_t*) ID_A;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*
     * we should convert little endian to big endian before SM3 caculate
     * Notice: secp256r1_op_num is "uin32_t" uint... but sm3_update is in bytes... so
     * we need to use (secp256r1_op_num<<2) for length!
     */
    /*Polynomial A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) param_a_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*Polynomial B*/
    buffer_endian_exchange(temp_buffer,  (uint32_t*) param_b_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*G_x*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*G_y*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.y),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*A x coordinate*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (A_point.x), secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*A y coordinate*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (A_point.y), secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);


    /*now we will generate Intermediate values in the steps of signature*/
    //sm3_init(&sm3_cntx);
    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);

    /* SM3 (Z_A || M) */
    //sm3_update(&sm3_cntx, digest, SM3_DIGEST_SIZE);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = SM3_DIGEST_SIZE;
    sm3_dev.in_ptr = (uint8_t*) digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_update(&sm3_cntx, (uint8_t *) message, sizeof(message));

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(message);
    sm3_dev.in_ptr = (uint8_t*) message;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);           /*This digest is e in document.*/

    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*sm3 digest is big endian, but sm2 ecc use little endian to caculate*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) digest, secp256r1_op_num);

    /*load firmware again.*/
    //gfp_ecc_curve_sm2p256_init();

    //status = gfp_ecdsa_sm2p256_signature(( ECPoint_P256 *) & (signature),
    //                                   (uint32_t *) temp_buffer,          /*this temp_buffer is little endian for digest*/
    //                                  (uint32_t *) private_key, (uint32_t *) random_number );


    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_ECC_SM2P256_SIGNATURE;
    ecc_p256.sm2_signatrue = (ECPoint_P256*)&signature;
    ecc_p256.p_hash = (uint32_t*) temp_buffer;
    ecc_p256.p_key = (uint32_t*) private_key;
    ecc_p256.p_k = (uint32_t*) random_number ;
    status = hosal_crypto_ecc_p256(&ecc_p256);


    if (status == STATUS_SUCCESS) {

        becp_print_hex_bytes("sm2p256 r:  ", (uint8_t*) (signature.x), 32);
        becp_print_hex_bytes("sm2p256 s:  ", (uint8_t*) (signature.y), 32);

        if (( memcmp((uint8_t*) (signature.x), expect_signature_r,
                     (secp256r1_op_num << 2)) != 0) ||
            ( memcmp((uint8_t*) (signature.y), expect_signature_s,
                     (secp256r1_op_num << 2)) != 0) ) {
            /*code should not go to here.*/
            printf("OOPs... test vector mismatched! \r\n");
            while (1);
        } else {
            printf("GOOD NEWS, SUCCESS \r\n");
        }

    } else {
        /*code should not go to here.*/
        printf("Oops.. this error should not happened \r\n");
        while (1);
    }

}


void ecc_gfp_sm2p256_verify(void) {
    sm3_context     sm3_cntx;

    const uint8_t  ID_A[] = {
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38
    };

    uint8_t  ENTL_A[4];     /* ENTLa is only 2 bytes, we allocate 4 bytes here for alignment*/

    const uint8_t  message[] = {
        'm', 'e', 's', 's', 'a', 'g', 'e', ' ',
        'd', 'i', 'g', 'e', 's', 't'
    };

    /*
     * for verify, we got public key from somewhere, like X509 protocol...
     * This test vector is extracted from sm2 document.
     * notice this is little endian
     */

    const ECPoint_P256  public_key = {
        {
            0x20, 0x50, 0xF3, 0x56,
            0xF3, 0x8F, 0xB0, 0x6B,
            0x07, 0xFC, 0x33, 0x18,
            0xAD, 0x9F, 0x17, 0x72,
            0xC6, 0xC5, 0x4B, 0x1E,
            0x16, 0x7D, 0xDD, 0x50,
            0xA1, 0x21, 0x54, 0x1E,
            0x31, 0xDF, 0xF9, 0x09
        },
        {
            0x13, 0xAD, 0xA9, 0x2D,
            0x07, 0xF6, 0x32, 0x66,
            0x4A, 0x08, 0x5E, 0xF3,
            0xFB, 0x05, 0xED, 0x0A,
            0x60, 0xAA, 0xC1, 0x8C,
            0x71, 0xEA, 0xC6, 0x2D,
            0xA5, 0x75, 0x67, 0xE2,
            0x0C, 0x49, 0xEA, 0xCC
        }
    };

    /*
     * we got signature r and s from somewhere...(internet, X509, ....)
     */

    const ECPoint_P256 signature_point = {
        {
            0xB3, 0x20, 0xE7, 0xEE,
            0xAC, 0x7E, 0xAC, 0x43,
            0x41, 0xB7, 0xD5, 0x27,
            0x38, 0xDA, 0x44, 0x59,
            0xA1, 0x81, 0xBB, 0xE1,
            0x13, 0xC5, 0xEA, 0x0E,
            0x63, 0xC4, 0xD2, 0x48,
            0x06, 0x3B, 0xA0, 0xF5
        },
        {
            0xAA, 0xC1, 0xBB, 0x85,
            0xC4, 0x69, 0x0B, 0x84,
            0xD4, 0x42, 0x7F, 0x1F,
            0xFD, 0x38, 0x90, 0xBB,
            0xA1, 0x1C, 0x42, 0x0D,
            0xBC, 0x82, 0x31, 0x76,
            0xD8, 0x2F, 0x21, 0xDF,
            0x29, 0xAA, 0xB6, 0xB1
        }
    };


    /* because  data in operation is little endian, but sm3 is big endian.
     * We need a temp swap buffer to save big endian data.
     */
    uint32_t  temp_buffer[secp256r1_op_num];

    /*result can be ignored.*/
    uint32_t  result[secp256r1_op_num];

    uint8_t   digest_doc[SM3_DIGEST_SIZE] = {
        0xF0, 0xB4, 0x3E, 0x94, 0xBA, 0x45, 0xAC, 0xCA,
        0xAC, 0xE6, 0x92, 0xED, 0x53, 0x43, 0x82, 0xEB,
        0x17, 0xE6, 0xAB, 0x5A, 0x19, 0xCE, 0x7B, 0x31,
        0xF4, 0x48, 0x6F, 0xDF, 0xC0, 0xD2, 0x86, 0x40
    };

    uint8_t   digest[SM3_DIGEST_SIZE];

    uint32_t  status;
    hosal_sm3_dev_t sm3_dev;
    hosal_crypto_ecc_p256_t ecc_p256;

    printf("SM2P256V1 verify Test: \r\n");


    ENTL_A[0] = (sizeof(ID_A) >> 5) & 0xFF;    /*in bits, MSB, one byte*/
    ENTL_A[1] = (sizeof(ID_A) << 3) & 0xFF;    /*in bits, LSB, one byte*/

    //sm3_init(&sm3_cntx);

    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);


    //sm3_update(&sm3_cntx, ENTL_A, 2);
    //sm3_update(&sm3_cntx, (uint8_t *) ID_A, sizeof(ID_A));

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = 2;
    sm3_dev.in_ptr = ENTL_A;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(ID_A);
    sm3_dev.in_ptr = (uint8_t*)ID_A;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*
     * we should convert little endian to big endian before SM3 caculate
     * Notice: secp256r1_op_num is "uin32_t" uint... but sm3_update is in bytes... so
     * we need to use (secp256r1_op_num<<2) for length!
     */
    /*Polynomial A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) param_a_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    //sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*Polynomial B*/
    buffer_endian_exchange(temp_buffer,  (uint32_t*) param_b_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    //sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*G_x*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*G_y*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.y),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    //sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*X_A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (public_key.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    //sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*Y_A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (public_key.y),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    //sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);


    /*now we will generate Intermediate values in the steps of signature*/
    //sm3_init(&sm3_cntx);
    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);
    /* SM3 (Z_A||M) */
    //sm3_update(&sm3_cntx, digest, SM3_DIGEST_SIZE);
    //sm3_update(&sm3_cntx, (uint8_t *) message, sizeof(message));

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = SM3_DIGEST_SIZE;
    sm3_dev.in_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(message);
    sm3_dev.in_ptr = (uint8_t*)message;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);           /*This digest is e in document.*/
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    if (memcmp(digest_doc, digest, 32) != 0) {
        /*SM3 error...*/
        printf("Oops....SM3 error \r\n");
        while (1);
    }

    /*sm3 digest is big endian, but sm2 ecc use little endian to caculate*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) digest, secp256r1_op_num);

    /*load firmware again.*/
    //gfp_ecc_curve_sm2p256_init();

    //status = gfp_ecdsa_sm2p256_verify(
    //             (uint32_t *) result,
    //             (ECPoint_P256 *) &signature_point,
    //             (uint32_t *) temp_buffer,          /*this temp_buffer is little endian for digest*/
    //             (ECPoint_P256 *)  &public_key);



    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_ECDA_SM2P256_VERIFY;
    ecc_p256.p_result_x = (uint32_t*) result;
    ecc_p256.p_signatrue = (ECPoint_P256*) &signature_point;
    ecc_p256.p_hash_message = (uint32_t*) temp_buffer;
    ecc_p256.p_public_key = (ECPoint_P256*)  &public_key;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    //gfp_ecdsa_sm2p256_verify(ecc_p256->p_result_x,ecc_p256->p_signatrue,ecc_p256->p_hash_message,ecc_p256->p_public_key);


    if (status == STATUS_SUCCESS) {

        /*result is */
        becp_print_hex_bytes("result:     ", (uint8_t*) result, 32);

        if ( memcmp((uint8_t*) result, (uint8_t*)(signature_point.x),
                    (secp256r1_op_num << 2)) != 0) {
            /*code should not go to here.*/
            printf("Oops.. this error should not happened\r\n");
            while (1);
        }

        printf("SM2P256V1 verify sueeccful, OK! \r\n");

    } else {
        /*code should not go to here.*/
        printf("Oops.. this error should not happened \r\n");
        while (1);
    }

}

void ecc_gfp_sm2p256_signature2(void) {
    sm3_context     sm3_cntx;

    const uint8_t  ID_A[] = {
        0x41, 0x4C, 0x49, 0x43, 0x45, 0x31, 0x32, 0x33,
        0x40, 0x59, 0x41, 0x48, 0x4F, 0x4F, 0x2E, 0x43,
        0x4F, 0x4D
    };

    uint8_t  ENTL_A[4];     /* ENTLa is only 2 bytes, we allocate 4 bytes here for alignment*/

    const uint8_t  message[] = {
        'm', 'e', 's', 's', 'a', 'g', 'e', ' ',
        'd', 'i', 'g', 'e', 's', 't'
    };

    uint32_t   private_key[secp256r1_op_num] = {
        0x15897263, 0x0C23661D, 0x171B1B65, 0x2A519A55,
        0x3DFF7979, 0x068C8D80, 0xBD433C6C, 0x128B2FA8,
    };

    /* the random key should be generated by hardware. we hardcode it
     * for matched the example in document.
     */

    uint32_t   random_number[secp256r1_op_num] = {
        0x1FB2F96F, 0x260DBAAE, 0xDD72B727, 0xC176D925,
        0x4817663F, 0x94F94E93, 0x385C175C, 0x6CB28D99
    };

    ECPoint_P256  A_point;

    ECPoint_P256  signature;

    /* because  data in operation is little endian, but sm3 is big endian.
     * We need a temp swap buffer to save big endian data.
     */
    uint32_t  temp_buffer[secp256r1_op_num];

    uint8_t   digest[SM3_DIGEST_SIZE];
    uint32_t  status;
    hosal_sm3_dev_t sm3_dev;
    hosal_crypto_ecc_p256_t ecc_p256;

    ENTL_A[0] = (sizeof(ID_A) >> 5) & 0xFF;     /*in bits, MSB, one byte*/
    ENTL_A[1] = (sizeof(ID_A) << 3) & 0xFF;    /*in bits, LSB, one byte*/


    /*sm2p256 Gx Gy defined in crypto.c*/
    /*caculate A point*/
    //gfp_ecc_curve_sm2p256_init();
    //status = gfp_point_p256_mult((ECPoint_P256 *) &A_point, (ECPoint_P256 *) &Curve_Gx_sm2p256, private_key);
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*)&A_point;
    ecc_p256.base = (ECPoint_P256*) &Curve_Gx_sm2p256;
    ecc_p256.p_key = private_key;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    if (status != STATUS_SUCCESS) {
        /*This error should not happen.*/
        printf("Oops, check what's wrong \r\n");
    }


    //sm3_init(&sm3_cntx);

    //sm3_update(&sm3_cntx, ENTL_A, 2);
    //sm3_update(&sm3_cntx, (uint8_t *) ID_A, sizeof(ID_A));

    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = 2;
    sm3_dev.in_ptr = ENTL_A;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(ID_A);
    sm3_dev.in_ptr = (uint8_t*)ID_A;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*
     * we should convert little endian to big endian before SM3 caculate
     * Notice: secp256r1_op_num is "uin32_t" uint... but sm3_update is in bytes... so
     * we need to use (secp256r1_op_num<<2) for length!
     */
    /*Polynomial A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) param_a_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*Polynomial B*/
    buffer_endian_exchange(temp_buffer,  (uint32_t*) param_b_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*G_x*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*G_y*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*X_A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (A_point.x), secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*Y_A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (A_point.y), secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*now we will generate Intermediate values in the steps of signature*/
    //    sm3_init(&sm3_cntx);

    //    /* SM3 (Z_A || M) */
    //    sm3_update(&sm3_cntx, digest, SM3_DIGEST_SIZE);
    //    sm3_update(&sm3_cntx, (uint8_t *) message, sizeof(message));

    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = SM3_DIGEST_SIZE;
    sm3_dev.in_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(message);
    sm3_dev.in_ptr = (uint8_t*)message;
    hosal_crypto_sm3_operation(&sm3_dev);


    //sm3_final(&sm3_cntx, digest);           /*This digest is e in document.*/
    //sm3_final(&sm3_cntx, digest);
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*sm3 digest is big endian, but sm2 ecc use little endian to caculate*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) digest, secp256r1_op_num);

    //    /*load firmware again.*/
    //    gfp_ecc_curve_sm2p256_init();

    //    status = gfp_ecdsa_sm2p256_signature( (ECPoint_P256 *) & (signature),
    //                                          (uint32_t *) temp_buffer,          /*this temp_buffer is little endian for digest*/
    //                                          (uint32_t *) private_key, (uint32_t *) random_number );

    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_ECC_SM2P256_SIGNATURE;
    ecc_p256.sm2_signatrue = (ECPoint_P256*)&signature;
    ecc_p256.p_hash = (uint32_t*) temp_buffer;
    ecc_p256.p_key = (uint32_t*) private_key;
    ecc_p256.p_k = (uint32_t*) random_number ;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    if (status == STATUS_SUCCESS) {

        becp_print_hex_bytes("sm2p256 r:  ", (uint8_t*) (signature.x), 32);
        becp_print_hex_bytes("sm2p256 s:  ", (uint8_t*) (signature.y), 32);

    } else {
        /*code should not go to here.*/
        printf("Oops.. this error should not happened \r\n");
        while (1);
    }

}


void ecc_gfp_sm2p256_verify2(void) {
    sm3_context     sm3_cntx;

    const uint8_t  ID_A[] = {
        0x41, 0x4C, 0x49, 0x43, 0x45, 0x31, 0x32, 0x33,
        0x40, 0x59, 0x41, 0x48, 0x4F, 0x4F, 0x2E, 0x43,
        0x4F, 0x4D
    };

    uint8_t  ENTL_A[4];     /* ENTLa is only 2 bytes, we allocate 4 bytes here for alignment*/

    const uint8_t  message[] = {
        'm', 'e', 's', 's', 'a', 'g', 'e', ' ',
        'd', 'i', 'g', 'e', 's', 't'
    };

    /*
     * for verify, we got public key from somewhere, like X509 protocol...
     * This test vector is extracted from sm2 document.
     */

    const ECPoint_P256  public_key = {
        {
            0xDD, 0x28, 0xAF, 0x0C,
            0x81, 0xDC, 0x21, 0x82,
            0xC5, 0xF3, 0xFA, 0x9D,
            0x51, 0xE0, 0x1A, 0x8A,
            0xAF, 0x64, 0x74, 0xD5,
            0x6C, 0x50, 0xA3, 0x50,
            0x61, 0xB5, 0xCB, 0x25,
            0x78, 0x8C, 0x54, 0xD5
        },
        {
            0x3E, 0x53, 0x79, 0xD4,
            0x46, 0x89, 0x16, 0x4D,
            0x26, 0x27, 0x70, 0x53,
            0x86, 0x30, 0xD2, 0xFE,
            0x73, 0xCF, 0x45, 0x94,
            0xA4, 0x79, 0x4E, 0xE5,
            0x9C, 0xD5, 0xE3, 0x8F,
            0x76, 0x73, 0x10, 0x92
        }
    };


    /*
     * we got signature r and s from somewhere...
     */

    const ECPoint_P256  signature = {
        {
            0x9F, 0x2E, 0xB6, 0x2C,
            0x6E, 0xD2, 0x0A, 0x08,
            0x20, 0xCB, 0x82, 0x48,
            0xCA, 0x8D, 0x5E, 0x2D,
            0xE2, 0xEC, 0xDD, 0x42,
            0xE0, 0x6E, 0x65, 0xA3,
            0xEE, 0xDA, 0x50, 0x63,
            0x65, 0xA4, 0x7B, 0x07
        },
        {
            0xAC, 0xD0, 0x00, 0xBE,
            0xC4, 0x95, 0x03, 0xA7,
            0xFC, 0xF1, 0xB0, 0xA3,
            0x38, 0xB6, 0x21, 0x2A,
            0xF1, 0x0B, 0xE2, 0xDD,
            0x88, 0x48, 0x92, 0x0F,
            0xEE, 0x6E, 0xF8, 0xAF,
            0xF4, 0x29, 0xF3, 0x2B
        }
    };

    /* because  data in operation is little endian, but sm3 is big endian.
     * We need a temp swap buffer to save big endian data.
     */
    uint32_t  temp_buffer[secp256r1_op_num];

    /*result can be ignored.*/
    uint32_t  result[secp256r1_op_num];

    uint8_t   digest[SM3_DIGEST_SIZE];
    uint32_t  status;
    hosal_sm3_dev_t sm3_dev;
    hosal_crypto_ecc_p256_t ecc_p256;
    printf("SM2P256V1 verify Test:\r\n");

    ENTL_A[0] = (sizeof(ID_A) >> 5) & 0xFF;    /*in bits, MSB, one byte*/
    ENTL_A[1] = (sizeof(ID_A) << 3) & 0xFF;    /*in bits, LSB, one byte*/

    //sm3_init(&sm3_cntx);

    //sm3_update(&sm3_cntx, ENTL_A, 2);
    //sm3_update(&sm3_cntx, (uint8_t *) ID_A, sizeof(ID_A));

    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = 2;
    sm3_dev.in_ptr = ENTL_A;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = sizeof(ID_A);
    sm3_dev.in_ptr = (uint8_t*)ID_A;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*
     * we should convert little endian to big endian before SM3 caculate
     * Notice: secp256r1_op_num is "uin32_t" uint... but sm3_update is in bytes... so
     * we need to use (secp256r1_op_num<<2) for length!
     */
    /*Polynomial A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) param_a_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*Polynomial B*/
    buffer_endian_exchange(temp_buffer,  (uint32_t*) param_b_sm2p256,
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*G_x*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*G_y*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (Curve_Gx_sm2p256.y),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*X_A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (public_key.x),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*Y_A*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) (public_key.y),
                           secp256r1_op_num);
    //sm3_update(&sm3_cntx, (uint8_t *) temp_buffer, (secp256r1_op_num << 2));
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)temp_buffer;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);


    /*now we will generate Intermediate values in the steps of signature*/
    //sm3_init(&sm3_cntx);
    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);
    /* SM3 (Z_A||M) */
    //sm3_update(&sm3_cntx, digest, SM3_DIGEST_SIZE);
    //sm3_update(&sm3_cntx, (uint8_t *) message, sizeof(message));

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = SM3_DIGEST_SIZE;
    sm3_dev.in_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.in_length = sizeof(message);
    sm3_dev.in_ptr = (uint8_t*)message;
    hosal_crypto_sm3_operation(&sm3_dev);

    //sm3_final(&sm3_cntx, digest);           /*This digest is e in document.*/
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = digest;
    hosal_crypto_sm3_operation(&sm3_dev);
    /*sm3 digest is big endian, but sm2 ecc use little endian to caculate*/
    buffer_endian_exchange(temp_buffer, (uint32_t*) digest, secp256r1_op_num);

    /*load firmware again.*/
    //    gfp_ecc_curve_sm2p256_init();

    //    status = gfp_ecdsa_sm2p256_verify(
    //                 (uint32_t *) result,
    //                 (ECPoint_P256 *) &signature,
    //                 (uint32_t *) temp_buffer,          /*this temp_buffer is little endian for digest*/
    //                 (ECPoint_P256 *) &public_key);


    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_ECDA_SM2P256_VERIFY;
    ecc_p256.p_result_x = (uint32_t*) result;
    ecc_p256.p_signatrue = (ECPoint_P256*) &signature;
    ecc_p256.p_hash_message = (uint32_t*) temp_buffer;
    ecc_p256.p_public_key = (ECPoint_P256*)  &public_key;
    //
    status = hosal_crypto_ecc_p256(&ecc_p256);


    if (status == STATUS_SUCCESS) {

        /*result is */
        becp_print_hex_bytes("result:     ", (uint8_t*) result, 32);

        if ( memcmp((uint8_t*) result, (uint8_t*) (signature.x),
                    (secp256r1_op_num << 2)) != 0) {
            /*code should not go to here.*/
            printf("Oops.. this error should not happened \r\n");
            while (1);
        }

        printf("SM2P256V1 verify sueeccful, OK! \r\n");

    } else {
        /*code should not go to here.*/
        printf("Oops.. this error should not happened \r\n");
        while (1);
    }

}


/*This kdf is SM2 KDF...*/
uint32_t sm2_kdf(uint8_t* key, uint32_t key_length, uint32_t bit_length,
                 uint8_t* output_hash_buf) {
    sm3_context   sm3_cntx;
    uint8_t       digest[SM3_DIGEST_SIZE];

    uint32_t      counter = 1;
    uint32_t      interval, i, remain_bytes, copy_size;
    uint32_t      counter_be;
    hosal_sm3_dev_t sm3_dev;
    /*Notice: output_hash_buf space MUST be more than (bit_length/8) bytes */
    /*key_length is in bytes.  for SM2P256, key_length is 32 bytes, for SM2P192 key_length is 24 bytes*/

    if ((bit_length == 0) || (output_hash_buf == NULL)) {
        return STATUS_INVALID_PARAM;
    }

    /*SM3 output 32 bytes, so each interval is 32byts. Here caculate in bits */
    interval = (bit_length + (SM3_DIGEST_SIZE << 3) - 1) >> 8;

    remain_bytes = (bit_length >> 3);   /*byte length*/


    for (i = 0; i < interval; i++) {
        /*SM3 use big endian, so we need change the order*/
        counter_be =  __REV(counter);

        //        sm3_init(&sm3_cntx);
        //        sm3_update(&sm3_cntx, key, key_length);        /*x||y*/
        //        sm3_update(&sm3_cntx, (uint8_t *) (&counter_be), 4);

        //        sm3_final(&sm3_cntx, digest);


        sm3_dev.crypto_operation = HOSAL_SM3_INIT;
        hosal_crypto_sm3_operation(&sm3_dev);


        sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
        sm3_dev.in_length = key_length;
        sm3_dev.in_ptr = (uint8_t*)key;
        hosal_crypto_sm3_operation(&sm3_dev);

        sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
        sm3_dev.in_length = 4;
        sm3_dev.in_ptr = (uint8_t*) (&counter_be);
        hosal_crypto_sm3_operation(&sm3_dev);

        //sm3_final(&sm3_cntx, digest);           /*This digest is e in document.*/
        sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
        sm3_dev.out_ptr = digest;
        hosal_crypto_sm3_operation(&sm3_dev);


        if (remain_bytes >= SM3_DIGEST_SIZE) {
            copy_size = SM3_DIGEST_SIZE;
            remain_bytes -= SM3_DIGEST_SIZE;
        } else {
            copy_size = remain_bytes;
        }

        memcpy(output_hash_buf, digest, copy_size);

        output_hash_buf += copy_size;
        counter++;
    }

    return STATUS_SUCCESS;
}

/*
 * We don't expect Message length is too many bytes..
 */
#define  MAX_KDF_SIZE      128

/* Notice: In SM2 part5 Annex C
 * there is a very obviously error...
 * It's C2 and C3 location mismatched!!
 */
const uint8_t expected_output_cipher_text_c[] = {
    0x04,
    0x04, 0xEB, 0xFC, 0x71, 0x8E, 0x8D, 0x17, 0x98,
    0x62, 0x04, 0x32, 0x26, 0x8E, 0x77, 0xFE, 0xB6,
    0x41, 0x5E, 0x2E, 0xDE, 0x0E, 0x07, 0x3C, 0x0F,
    0x4F, 0x64, 0x0E, 0xCD, 0x2E, 0x14, 0x9A, 0x73,
    0xE8, 0x58, 0xF9, 0xD8, 0x1E, 0x54, 0x30, 0xA5,
    0x7B, 0x36, 0xDA, 0xAB, 0x8F, 0x95, 0x0A, 0x3C,
    0x64, 0xE6, 0xEE, 0x6A, 0x63, 0x09, 0x4D, 0x99,
    0x28, 0x3A, 0xFF, 0x76, 0x7E, 0x12, 0x4D, 0xF0,

    0x21, 0x88, 0x6C,
    0xA9, 0x89, 0xCA, 0x9C, 0x7D, 0x58, 0x08, 0x73,
    0x07, 0xCA, 0x93, 0x09, 0x2D, 0x65, 0x1E, 0xFA,

    0x59, 0x98, 0x3C, 0x18, 0xF8, 0x09, 0xE2, 0x62,
    0x92, 0x3C, 0x53, 0xAE, 0xC2, 0x95, 0xD3, 0x03,
    0x83, 0xB5, 0x4E, 0x39, 0xD6, 0x09, 0xD1, 0x60,
    0xAF, 0xCB, 0x19, 0x08, 0xD0, 0xBD, 0x87, 0x66

};

uint8_t cipher_text_c_result[256];


void ecc_gfp_sm2p256_msg_encryption(void) {
    /*
     * This test vector example is extracted from Annex C of SM2 Part5
     */

    sm3_context   sm3_cntx;
    uint8_t       C3_digest[SM3_DIGEST_SIZE];

    uint32_t      status;
    uint32_t      i, msg_len;

    uint8_t*       cipher_ptr;

    const uint8_t   message[] =             /*encryption standard*/
    {
        0x65, 0x6E, 0x63, 0x72, 0x79, 0x70, 0x74, 0x69,
        0x6F, 0x6E, 0x20, 0x73, 0x74, 0x61, 0x6E, 0x64,
        0x61, 0x72, 0x64
    };

    /*
     * In test example, it has private key information for better understand the
     * algorithm.
     * However, in real case, client does NOT have private key information...
     * Client and server share secret Z is public key only!
     *
     * Public key information get from some PKI protocol like X.509 or manufacture OTP...
     *
     */
    const ECPoint_P256 PB_Key = {
        {
            0x20, 0x50, 0xF3, 0x56, 0xF3, 0x8F, 0xB0, 0x6B, 0x07, 0xFC, 0x33, 0x18, 0xAD, 0x9F, 0x17, 0x72,
            0xC6, 0xC5, 0x4B, 0x1E, 0x16, 0x7D, 0xDD, 0x50, 0xA1, 0x21, 0x54, 0x1E, 0x31, 0xDF, 0xF9, 0x09
        },
        {
            0x13, 0xAD, 0xA9, 0x2D, 0x07, 0xF6, 0x32, 0x66, 0x4A, 0x08, 0x5E, 0xF3, 0xFB, 0x05, 0xED, 0x0A,
            0x60, 0xAA, 0xC1, 0x8C, 0x71, 0xEA, 0xC6, 0x2D, 0xA5, 0x75, 0x67, 0xE2, 0x0C, 0x49, 0xEA, 0xCC
        }
    };

    /*
     * the random key should be generated by hardware.
     * Here for compare the test vector example, so we hardcode it
     *
     */

    const uint32_t   random_k[secp256r1_op_num] = {
        0xEAC1BC21, 0x6D54B80D, 0x3CDBE4CE, 0xEF3CC1FA,
        0xD9C02DCC, 0x16680F3A, 0xD506861A, 0x59276E27
    };

    uint32_t  key_buff_bigendian[(secp256r1_op_num *
                                  2)];     /*SM2P256 is only 32 bytes for one point coordinate, so 32*2 is 64*/

    uint8_t   kdf_t[MAX_KDF_SIZE], C2[MAX_KDF_SIZE];
    hosal_sm3_dev_t sm3_dev;
    hosal_crypto_ecc_p256_t ecc_p256;
    /* C1 = k*G  */
    ECPoint_P256  C1_point;
    /* K*PB */
    ECPoint_P256  KPB_point;

    msg_len = sizeof (message);

    /*caculate C1 */
    //gfp_ecc_curve_sm2p256_init();
    //status = gfp_point_p256_mult( (ECPoint_P256 *) &C1_point, (ECPoint_P256 *) &Curve_Gx_sm2p256, (uint32_t *)random_k);
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);


    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*)&C1_point;
    ecc_p256.base = (ECPoint_P256*) &Curve_Gx_sm2p256;
    ecc_p256.p_key = (uint32_t*)random_k;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    /*caculate random_k*PB */
    //gfp_ecc_curve_sm2p256_init();
    //status = gfp_point_p256_mult( (ECPoint_P256 *) &KPB_point, (ECPoint_P256 *) &PB_Key, (uint32_t *)random_k);
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*)&KPB_point;
    ecc_p256.base = (ECPoint_P256*) &PB_Key;
    ecc_p256.p_key = (uint32_t*)random_k;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    /* buffer_endian_exchange length is in uint32 unit
     * Because in our ecc caculate, all data is little endian, but SM3 is big endian
     * so we need convert little endian to big endian.
     */

    buffer_endian_exchange( key_buff_bigendian, (uint32_t*)(KPB_point.x),
                            secp256r1_op_num);
    buffer_endian_exchange((key_buff_bigendian + secp256r1_op_num),
                           (uint32_t*)(KPB_point.y), secp256r1_op_num);

    /*
     * key length is byte unit, secp256r1_op_num is in "4bytes" unit, and we have x and y coordinate
     * so key_length is secp256r1_op_num*4*2  bytes!
     */
    status = sm2_kdf((uint8_t*) key_buff_bigendian, (secp256r1_op_num << 3),
                     (msg_len << 3), kdf_t);

    for (i = 0; i < msg_len; i++) {
        C2[i] = message[i] ^
                kdf_t[i];      /*of course, you can use "kdf_t[i] =  message[i] ^ kdf_t[i]" too. */
    }

    //    sm3_init(&sm3_cntx);
    //    sm3_update(&sm3_cntx, (uint8_t *) key_buff_bigendian, (secp256r1_op_num << 2)); /* X2 */
    //    sm3_update(&sm3_cntx, (uint8_t *) message, msg_len);                             /* M  */
    //    sm3_update(&sm3_cntx, (uint8_t *) (key_buff_bigendian + secp256r1_op_num), (secp256r1_op_num << 2)); /* Y2 */

    //    sm3_final(&sm3_cntx, C3_digest);



    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)key_buff_bigendian;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = msg_len;
    sm3_dev.in_ptr = (uint8_t*)message;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)(key_buff_bigendian + secp256r1_op_num);
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = C3_digest;
    hosal_crypto_sm3_operation(&sm3_dev);




    /*combine cipher_text*/
    cipher_ptr =  cipher_text_c_result;

    /*uncompressed form of C1*/
    *cipher_ptr++ = 0x04;           /*PC is 04*/

    /*again... C1 is little endian in our ECC caculate, but SM2 part4 need it in big endian*/
    buffer_endian_exchange( key_buff_bigendian, (uint32_t*)(C1_point.x),
                            secp256r1_op_num);
    buffer_endian_exchange((key_buff_bigendian + secp256r1_op_num),
                           (uint32_t*)(C1_point.y), secp256r1_op_num);

    /*unfortunately... data is not 4 bytes alignment.*/
    memcpy(cipher_ptr, (uint8_t*)key_buff_bigendian, (secp256r1_op_num << 3));
    cipher_ptr += (secp256r1_op_num << 3);

    /*C2*/
    memcpy(cipher_ptr, C2, sizeof (message));
    cipher_ptr += msg_len;

    /*C3*/
    memcpy(cipher_ptr, C3_digest, SM3_DIGEST_SIZE);
    cipher_ptr += SM3_DIGEST_SIZE;

    /*now we can compare data with expected value*/
    if ( memcmp(cipher_text_c_result, expected_output_cipher_text_c,
                (SM3_DIGEST_SIZE + (secp256r1_op_num << 3) + 1 + msg_len) ) != 0) {
        /*Code should not run to here.*/
        printf("Oops... message encryption part4 fail... check why \r\n");
        while (1);
    } else {
        printf("SM2 Message encryption part4 successful \r\n");
    }

    return;

}

uint32_t ecc_gfp_sm2p256_msg_decryption(uint8_t* encryption_msg,
                                        uint32_t length, uint8_t* message, uint32_t* msg_length) {
    sm3_context   sm3_cntx;
    uint8_t       C3_digest[SM3_DIGEST_SIZE];

    uint32_t   key_buff_endian_convert[(secp256r1_op_num *
                                        2)];     /*SM2P256 is only 32 bytes for one point coordinate, so 32*2 is 64*/

    /*
     *  Of course, decryption side should know itself private key, and it should save the key in somewhere secure register place.
     *  It should read from some secure protected register...
     *  In this example, it is hardcode to match the test vector just for deom.
     */
    const uint32_t private_key[secp256r1_op_num] = {
        0x4DF7C5B8, 0x42FB81EF, 0x2860B51A, 0x88939369,
        0xC6D39F95, 0x3F36E38A, 0x7B2144B1, 0x3945208F
    };

    ECPoint_P256  C1_point_BE;      /*C1 point Big Endian*/
    ECPoint_P256  C1_point_LE;      /*C1 point little Endian*/

    ECPoint_P256  X_point;          /*X point little endian*/


    //uint32_t   X2[secp256r1_op_num] ;
    //uint32_t   Y2[secp256r1_op_num] ;

    uint32_t   i, message_length;
    uint32_t   status;

    uint8_t*    ptr_c1_x, *ptr_c1_y, *ptr_message;

    uint8_t    kdf_t[MAX_KDF_SIZE];
    hosal_sm3_dev_t sm3_dev;
    hosal_crypto_ecc_p256_t ecc_p256;
    /*check encryption_msg*/
    if ((encryption_msg == NULL)
        || (length <= (secp256r1_op_num * 8 + SM3_DIGEST_SIZE + 1))
        || (*encryption_msg != 0x04)) {
        return STATUS_ERROR;
    }

    message_length = length - (secp256r1_op_num * 8 + SM3_DIGEST_SIZE + 1);

    if (message_length > *msg_length) {
        return STATUS_INVALID_REQUEST;      /*buffer too small... need a large buffer.*/
    }

    /*
     * unfortunately BigEndian C1 point x y coordination is not 4 bytes alignment (because there is PC(0x04) in head),
     * so we need adjust it to 4 bytes alignment.
     * (buffer_endian_exchange requried data is 4 bytes alignment)
     * Remember secp256r1_op_num is 32 bytes uint.
     */

    ptr_c1_x = (encryption_msg + 1);
    ptr_c1_y = (encryption_msg + (secp256r1_op_num * 4) + 1);

    /*we don't think memcpy can do fast because unalignment*/
    for (i = 0; i < (secp256r1_op_num << 2); i++) {
        C1_point_BE.x[i] = *ptr_c1_x++;
        C1_point_BE.y[i] = *ptr_c1_y++;
    }

    /*Big Endian to little endian*/
    buffer_endian_exchange( (uint32_t*) (C1_point_LE.x),
                            (uint32_t*) (C1_point_BE.x), secp256r1_op_num);
    buffer_endian_exchange( (uint32_t*) (C1_point_LE.y),
                            (uint32_t*) (C1_point_BE.y), secp256r1_op_num);


    /*TODO: check C1 C1 is in ECC curve. Not check yet.*/

    /*caculate share secure key x y*/
    //gfp_ecc_curve_sm2p256_init();
    //hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);
    //status = gfp_point_p256_mult( (ECPoint_P256 *) &X_point, (ECPoint_P256 *) &C1_point_LE, (uint32_t *)private_key);

    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_SM2P256_INIT);

    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*)&X_point;
    ecc_p256.base = (ECPoint_P256*) &C1_point_LE;
    ecc_p256.p_key = (uint32_t*)private_key;
    status = hosal_crypto_ecc_p256(&ecc_p256);

    /* buffer_endian_exchange length is in uint32 unit
     * Because in our ecc caculate, all data is little endian, but SM3 is big endian
     * so we need convert little endian to big endian.
     */

    buffer_endian_exchange( key_buff_endian_convert, (uint32_t*) (X_point.x),
                            secp256r1_op_num);
    buffer_endian_exchange((key_buff_endian_convert + secp256r1_op_num),
                           (uint32_t*) (X_point.y), secp256r1_op_num);

    /*
     * key length is byte unit, secp256r1_op_num is in "4bytes" unit, and we have x and y coordinate
     * so key_length is secp256r1_op_num*4*2  bytes!
     */
    status = sm2_kdf((uint8_t*) key_buff_endian_convert, (secp256r1_op_num << 3),
                     (message_length << 3), kdf_t);

    /*decode M*/
    ptr_message = encryption_msg + (secp256r1_op_num << 3) + 1; /*+1 is "PC"*/

    for (i = 0; i < message_length; i++) {
        message[i] = *ptr_message ^ kdf_t[i];
        ptr_message++;
    }

    /*the last, verify decode message is correct or not */


    //    sm3_init(&sm3_cntx);
    //    sm3_update(&sm3_cntx, (uint8_t *) key_buff_endian_convert, (secp256r1_op_num << 2)); /* X2 */
    //    sm3_update(&sm3_cntx, (uint8_t *) message, message_length);                          /* M  */
    //    sm3_update(&sm3_cntx, (uint8_t *) (key_buff_endian_convert + secp256r1_op_num), (secp256r1_op_num << 2)); /* Y2 */

    //    sm3_final(&sm3_cntx, C3_digest);

    sm3_dev.crypto_operation = HOSAL_SM3_INIT;
    hosal_crypto_sm3_operation(&sm3_dev);


    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*)key_buff_endian_convert;
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length = message_length;
    sm3_dev.in_ptr = (uint8_t*) (message);
    hosal_crypto_sm3_operation(&sm3_dev);

    sm3_dev.crypto_operation = HOSAL_SM3_UPDATE;
    sm3_dev.in_length =  (secp256r1_op_num << 2);
    sm3_dev.in_ptr = (uint8_t*) (key_buff_endian_convert + secp256r1_op_num);
    hosal_crypto_sm3_operation(&sm3_dev);
    //sm3_final(&sm3_cntx, digest);           /*This digest is e in document.*/
    sm3_dev.crypto_operation = HOSAL_SM3_FINAL;
    sm3_dev.out_ptr = C3_digest;
    hosal_crypto_sm3_operation(&sm3_dev);

    /*compare the result*/

    ptr_message = encryption_msg + (secp256r1_op_num << 3) + message_length + 1;

    if ( memcmp(ptr_message, C3_digest, SM3_DIGEST_SIZE) == 0) {
        printf("SM2 part4  recommend cureve test vector success!! \r\n");
        return STATUS_SUCCESS;
    } else {
        printf("Oops ... this error should not happen check why \r\n");
        while (1);
    }

}


int main(void) {

    uint8_t   message[128];
    uint32_t msg_length = 128;


    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto SM2P256V1 demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    /* Notice:
     * For some variables have "const" attribute just for demo used.
     * For real used, those variables could NOT be hardcode!!
     *
     */

    /*
     * Notice: Enable  CRYPTO_SM2P256V1_ENABLE and
     * enable CRYPTO_SECP256R1_ENABLE
     */

    ecc_gfp_sm2p256r_test1();       /*simple test sm2 recommand parameter curve */

    printf("test ecc_gfp_sm2p256_signature: \r\n");
    ecc_gfp_sm2p256_signature();

    printf("\r\n");

    ecc_gfp_sm2p256_verify();

    printf("\r\n test ecc_gfp_sm2p256_signature: \r\n");

    ecc_gfp_sm2p256_signature2();

    printf("\r\n");

    ecc_gfp_sm2p256_verify2();


    /*
     * test sm2 part4 message_encryption and decryption
     * The test vector list in SM2 part5
     *
     */


    ecc_gfp_sm2p256_msg_encryption();

    /*here we don't expect message more than 128 bytes.*/
    msg_length = 128;           /*in factm, msg_length can be caculate in frist*/

    ecc_gfp_sm2p256_msg_decryption((uint8_t*) expected_output_cipher_text_c,
                                   sizeof(expected_output_cipher_text_c), message,  &msg_length);






    /*
     * Notice: Enable  CRYPTO_SM2_P192_ENABLE and
     * enable CRYPTO_SECP256R1_ENABLE
     *
     */
    ecc_gfp_sm2p192_test1();
    ecc_gfp_sm2p192_test2();
    ecc_gfp_sm2p192_test3();



    printf("\r\n\r\n");
    printf("hosal crypto SM2P256V1 finish \r\n");

    while (1);
}


