#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "rt_crypto.h"
#include "rt_sha256.h"



#define NUMBER_OF_GENERATES 2
/*
 * This is CAVS 14.3 DRBG800-90A test case [SHA-256] count=0
 * line 5073.
 */
void hmac_test_vector1(void) {
    /*  EntropyInput = 06032cd5eed33f39265f49ecb142c511da9aff2af71203bffaf34a9ca5bd9c0d
     *  Nonce = 0e66f71edc43e42a45ad3c6fc6cdc4df
     */

    const uint8_t entropy_input[32] = { 0x06, 0x03, 0x2c, 0xd5, 0xee, 0xd3, 0x3f, 0x39,
                                        0x26, 0x5f, 0x49, 0xec, 0xb1, 0x42, 0xc5, 0x11,
                                        0xda, 0x9a, 0xff, 0x2a, 0xf7, 0x12, 0x03, 0xbf,
                                        0xfa, 0xf3, 0x4a, 0x9c, 0xa5, 0xbd, 0x9c, 0x0d
                                      };

    const uint8_t nonce[16] = { 0x0e, 0x66, 0xf7, 0x1e, 0xdc, 0x43, 0xe4, 0x2a,
                                0x45, 0xad, 0x3c, 0x6f, 0xc6, 0xcd, 0xc4, 0xdf
                              };

    const uint8_t entropy_input_reseed[32] =  { 0x01, 0x92, 0x0a, 0x4e, 0x66, 0x9e, 0xd3, 0xa8,
                                                0x5a, 0xe8, 0xa3, 0x3b, 0x35, 0xa7, 0x4a, 0xd7,
                                                0xfb, 0x2a, 0x6b, 0xb4, 0xcf, 0x39, 0x5c, 0xe0,
                                                0x03, 0x34, 0xa9, 0xc9, 0xa5, 0xa5, 0xd5, 0x52
                                              };

    const uint8_t expect_result[128] = {
        0x76, 0xfc, 0x79, 0xfe, 0x9b, 0x50, 0xbe, 0xcc, 0xc9, 0x91, 0xa1, 0x1b, 0x56, 0x35, 0x78, 0x3a,
        0x83, 0x53, 0x6a, 0xdd, 0x03, 0xc1, 0x57, 0xfb, 0x30, 0x64, 0x5e, 0x61, 0x1c, 0x28, 0x98, 0xbb,
        0x2b, 0x1b, 0xc2, 0x15, 0x00, 0x02, 0x09, 0x20, 0x8c, 0xd5, 0x06, 0xcb, 0x28, 0xda, 0x2a, 0x51,
        0xbd, 0xb0, 0x38, 0x26, 0xaa, 0xf2, 0xbd, 0x23, 0x35, 0xd5, 0x76, 0xd5, 0x19, 0x16, 0x08, 0x42,
        0xe7, 0x15, 0x8a, 0xd0, 0x94, 0x9d, 0x1a, 0x9e, 0xc3, 0xe6, 0x6e, 0xa1, 0xb1, 0xa0, 0x64, 0xb0,
        0x05, 0xde, 0x91, 0x4e, 0xac, 0x2e, 0x9d, 0x4f, 0x2d, 0x72, 0xa8, 0x61, 0x6a, 0x80, 0x22, 0x54,
        0x22, 0x91, 0x82, 0x50, 0xff, 0x66, 0xa4, 0x1b, 0xd2, 0xf8, 0x64, 0xa6, 0xa3, 0x8c, 0xc5, 0xb6,
        0x49, 0x9d, 0xc4, 0x3f, 0x7f, 0x2b, 0xd0, 0x9e, 0x1e, 0x0f, 0x8f, 0x58, 0x85, 0x93, 0x51, 0x24
    };


    /*No PersonalizationString*/

    hmac_drbg_state    hmac_drbg_ctx;

    uint8_t seed_material[64], result[128];

    uint32_t     i;

    memcpy(seed_material, entropy_input, sizeof(entropy_input));
    memcpy(seed_material + sizeof(entropy_input), nonce, sizeof(nonce));

    hmac_drbg_instantiate(&hmac_drbg_ctx, seed_material, sizeof(entropy_input) + sizeof(nonce));


    /*drbg reseed*/
    hmac_drbg_update(&hmac_drbg_ctx, (uint8_t *) entropy_input_reseed, sizeof(entropy_input_reseed));

    for (i = 0; i < NUMBER_OF_GENERATES; i++) {
        hmac_drbg_generate(result, &hmac_drbg_ctx, 128, NULL, 0);
    }

    /*compare result*/
    if ( memcmp(result, expect_result, sizeof(expect_result)) == 0) {
        printf("Success for  CAVS 14.3 DRBG800-90A test case line 5073 \r\n");
    } else {
        printf("OOPS, check hmac_drbg_test case1 \r\n");
    }

    return;

}


#define  entropy_input_length                32
#define  nonce_length                        16
#define  entropy_input_reseed_length         32
#define  expect_result_length               128

/*
 * This is CAVS 14.3 DRBG800-90A test case [SHA-256] count=1
 * line 5083
 */
void hmac_test_vector2(void) {

    /*
     * For Keil V5.36.0, build this code will generate a strange warning:
     * .\Objects\Crypto_HMAC_DRBG.axf: Warning: L6096W: String merge section main.o(.conststring) is not null-terminated.
     * To avoid this strange warning, we add 4 bytes 0x00 in the end of each arrary.
     * and define the each input size.
     */

    /*
     * EntropyInput = aadcf337788bb8ac01976640726bc51635d417777fe6939eded9ccc8a378c76a
     * Nonce = 9ccc9d80c89ac55a8cfe0f99942f5a4d
     * EntropyInputReseed = 03a57792547e0c98ea1776e4ba80c007346296a56a270a35fd9ea2845c7e81e2
     * ReturnedBits = 17d09f40a43771f4a2f0db327df637dea972bfff30c98ebc8842dc7a9e3d681c61902f71bffaf5093607fbfba9674a70d048e562ee88f027f630a78522ec6f706bb44ae130e05c8d7eac668bf6980d99b4c0242946452399cb032cc6f9fd96284709bd2fa565b9eb9f2004be6c9ea9ff9128c3f93b60dc30c5fc8587a10de68c
     */

    /*In fact, this entropy_input only need 32 bytes */
    const uint8_t entropy_input[36] = {
        0xaa, 0xdc, 0xf3, 0x37, 0x78, 0x8b, 0xb8, 0xac,
        0x01, 0x97, 0x66, 0x40, 0x72, 0x6b, 0xc5, 0x16,
        0x35, 0xd4, 0x17, 0x77, 0x7f, 0xe6, 0x93, 0x9e,
        0xde, 0xd9, 0xcc, 0xc8, 0xa3, 0x78, 0xc7, 0x6a,
        0x00, 0x00, 0x00, 0x00
    };

    /*In fact, this nonce only need 16 bytes */
    const uint8_t nonce[20] = {
        0x9c, 0xcc, 0x9d, 0x80, 0xc8, 0x9a, 0xc5, 0x5a,
        0x8c, 0xfe, 0x0f, 0x99, 0x94, 0x2f, 0x5a, 0x4d,
        0x00, 0x00, 0x00, 0x00
    };

    /*In fact, this entropy_input_reseed only need 36 bytes */
    const uint8_t entropy_input_reseed[36] = {
        0x03, 0xa5, 0x77, 0x92, 0x54, 0x7e, 0x0c, 0x98,
        0xea, 0x17, 0x76, 0xe4, 0xba, 0x80, 0xc0, 0x07,
        0x34, 0x62, 0x96, 0xa5, 0x6a, 0x27, 0x0a, 0x35,
        0xfd, 0x9e, 0xa2, 0x84, 0x5c, 0x7e, 0x81, 0xe2,
        0x00, 0x00, 0x00, 0x00
    };

    /*In fact, this expect_result only need 128 bytes */
    const uint8_t expect_result[132] = {
        0x17, 0xd0, 0x9f, 0x40, 0xa4, 0x37, 0x71, 0xf4, 0xa2, 0xf0, 0xdb, 0x32, 0x7d, 0xf6, 0x37, 0xde,
        0xa9, 0x72, 0xbf, 0xff, 0x30, 0xc9, 0x8e, 0xbc, 0x88, 0x42, 0xdc, 0x7a, 0x9e, 0x3d, 0x68, 0x1c,
        0x61, 0x90, 0x2f, 0x71, 0xbf, 0xfa, 0xf5, 0x09, 0x36, 0x07, 0xfb, 0xfb, 0xa9, 0x67, 0x4a, 0x70,
        0xd0, 0x48, 0xe5, 0x62, 0xee, 0x88, 0xf0, 0x27, 0xf6, 0x30, 0xa7, 0x85, 0x22, 0xec, 0x6f, 0x70,
        0x6b, 0xb4, 0x4a, 0xe1, 0x30, 0xe0, 0x5c, 0x8d, 0x7e, 0xac, 0x66, 0x8b, 0xf6, 0x98, 0x0d, 0x99,
        0xb4, 0xc0, 0x24, 0x29, 0x46, 0x45, 0x23, 0x99, 0xcb, 0x03, 0x2c, 0xc6, 0xf9, 0xfd, 0x96, 0x28,
        0x47, 0x09, 0xbd, 0x2f, 0xa5, 0x65, 0xb9, 0xeb, 0x9f, 0x20, 0x04, 0xbe, 0x6c, 0x9e, 0xa9, 0xff,
        0x91, 0x28, 0xc3, 0xf9, 0x3b, 0x60, 0xdc, 0x30, 0xc5, 0xfc, 0x85, 0x87, 0xa1, 0x0d, 0xe6, 0x8c,
        0x00, 0x00, 0x00, 0x00
    };


    /*No PersonalizationString*/

    hmac_drbg_state    hmac_drbg_ctx;

    uint8_t seed_material[64], result[128];

    uint32_t     i;

    memcpy(seed_material, entropy_input, entropy_input_length);
    memcpy(seed_material + entropy_input_length, nonce, nonce_length);

    hmac_drbg_instantiate(&hmac_drbg_ctx, seed_material, entropy_input_length + nonce_length);


    /*drbg reseed*/
    hmac_drbg_update(&hmac_drbg_ctx, (uint8_t *) entropy_input_reseed, entropy_input_reseed_length);

    for (i = 0; i < NUMBER_OF_GENERATES; i++) {
        hmac_drbg_generate(result, &hmac_drbg_ctx, 128, NULL, 0);
    }

    /*compare result*/
    if ( memcmp(result, expect_result, expect_result_length) == 0) {
        printf("Success for  CAVS 14.3 DRBG800-90A test case line 5083 \r\n");
    } else {
        printf("OOPS, check hmac_drbg_test case2 \r\n");
    }

    return;

}

/*
 * This is CAVS 14.3 DRBG800-90A test case [SHA-256]
 * line 5231
 */

void hmac_test_vector3(void) {
    /*
     *   EntropyInput = 05ac9fc4c62a02e3f90840da5616218c6de5743d66b8e0fbf833759c5928b53d
     *   Nonce = 2b89a17904922ed8f017a63044848545
     *
     *   EntropyInputReseed = 2791126b8b52ee1fd9392a0a13e0083bed4186dc649b739607ac70ec8dcecf9b
     *   AdditionalInputReseed = 43bac13bae715092cf7eb280a2e10a962faf7233c41412f69bc74a35a584e54c
     *   AdditionalInput = 3f2fed4b68d506ecefa21f3f5bb907beb0f17dbc30f6ffbba5e5861408c53a1e
     *   AdditionalInput = 529030df50f410985fde068df82b935ec23d839cb4b269414c0ede6cffea5b68
     *   ReturnedBits = 02ddff5173da2fcffa10215b030d660d61179e61ecc22609b1151a75f1cbcbb4363c3a89299b4b63aca5e581e73c860491010aa35de3337cc6c09ebec8c91a6287586f3a74d9694b462d2720ea2e11bbd02af33adefb4a16e6b370fa0effd57d607547bdcfbb7831f54de7073ad2a7da987a0016a82fa958779a168674b56524
     */

    const uint8_t entropy_input[32] = {
        0x05, 0xac, 0x9f, 0xc4, 0xc6, 0x2a, 0x02, 0xe3, 0xf9, 0x08, 0x40, 0xda, 0x56, 0x16, 0x21, 0x8c,
        0x6d, 0xe5, 0x74, 0x3d, 0x66, 0xb8, 0xe0, 0xfb, 0xf8, 0x33, 0x75, 0x9c, 0x59, 0x28, 0xb5, 0x3d
    };

    const uint8_t nonce[16] = {
        0x2b, 0x89, 0xa1, 0x79, 0x04, 0x92, 0x2e, 0xd8, 0xf0, 0x17, 0xa6, 0x30, 0x44, 0x84, 0x85, 0x45
    };

    const uint8_t entropy_input_reseed[32] = {
        0x27, 0x91, 0x12, 0x6b, 0x8b, 0x52, 0xee, 0x1f, 0xd9, 0x39, 0x2a, 0x0a, 0x13, 0xe0, 0x08, 0x3b,
        0xed, 0x41, 0x86, 0xdc, 0x64, 0x9b, 0x73, 0x96, 0x07, 0xac, 0x70, 0xec, 0x8d, 0xce, 0xcf, 0x9b
    };

    const uint8_t additional_input_reseed[32] = {
        0x43, 0xba, 0xc1, 0x3b, 0xae, 0x71, 0x50, 0x92, 0xcf, 0x7e, 0xb2, 0x80, 0xa2, 0xe1, 0x0a, 0x96,
        0x2f, 0xaf, 0x72, 0x33, 0xc4, 0x14, 0x12, 0xf6, 0x9b, 0xc7, 0x4a, 0x35, 0xa5, 0x84, 0xe5, 0x4c
    };

    const uint8_t additional_input[2][32] = {
        {
            0x3f, 0x2f, 0xed, 0x4b, 0x68, 0xd5, 0x06, 0xec, 0xef, 0xa2, 0x1f, 0x3f, 0x5b, 0xb9, 0x07, 0xbe,
            0xb0, 0xf1, 0x7d, 0xbc, 0x30, 0xf6, 0xff, 0xbb, 0xa5, 0xe5, 0x86, 0x14, 0x08, 0xc5, 0x3a, 0x1e
        },
        {
            0x52, 0x90, 0x30, 0xdf, 0x50, 0xf4, 0x10, 0x98, 0x5f, 0xde, 0x06, 0x8d, 0xf8, 0x2b, 0x93, 0x5e,
            0xc2, 0x3d, 0x83, 0x9c, 0xb4, 0xb2, 0x69, 0x41, 0x4c, 0x0e, 0xde, 0x6c, 0xff, 0xea, 0x5b, 0x68
        }
    };

    const uint8_t expect_result[128] = {
        0x02, 0xdd, 0xff, 0x51, 0x73, 0xda, 0x2f, 0xcf, 0xfa, 0x10, 0x21, 0x5b, 0x03, 0x0d, 0x66, 0x0d,
        0x61, 0x17, 0x9e, 0x61, 0xec, 0xc2, 0x26, 0x09, 0xb1, 0x15, 0x1a, 0x75, 0xf1, 0xcb, 0xcb, 0xb4,
        0x36, 0x3c, 0x3a, 0x89, 0x29, 0x9b, 0x4b, 0x63, 0xac, 0xa5, 0xe5, 0x81, 0xe7, 0x3c, 0x86, 0x04,
        0x91, 0x01, 0x0a, 0xa3, 0x5d, 0xe3, 0x33, 0x7c, 0xc6, 0xc0, 0x9e, 0xbe, 0xc8, 0xc9, 0x1a, 0x62,
        0x87, 0x58, 0x6f, 0x3a, 0x74, 0xd9, 0x69, 0x4b, 0x46, 0x2d, 0x27, 0x20, 0xea, 0x2e, 0x11, 0xbb,
        0xd0, 0x2a, 0xf3, 0x3a, 0xde, 0xfb, 0x4a, 0x16, 0xe6, 0xb3, 0x70, 0xfa, 0x0e, 0xff, 0xd5, 0x7d,
        0x60, 0x75, 0x47, 0xbd, 0xcf, 0xbb, 0x78, 0x31, 0xf5, 0x4d, 0xe7, 0x07, 0x3a, 0xd2, 0xa7, 0xda,
        0x98, 0x7a, 0x00, 0x16, 0xa8, 0x2f, 0xa9, 0x58, 0x77, 0x9a, 0x16, 0x86, 0x74, 0xb5, 0x65, 0x24
    };


    /*No PersonalizationString*/

    hmac_drbg_state    hmac_drbg_ctx;

    uint8_t seed_material[64], result[128];

    uint32_t     i;

    memcpy(seed_material, entropy_input, sizeof(entropy_input));
    memcpy(seed_material + sizeof(entropy_input), nonce, sizeof(nonce));

    hmac_drbg_instantiate(&hmac_drbg_ctx, seed_material, sizeof(entropy_input) + sizeof(nonce));


    /*drbg reseed  entropy_input+ additional_input_reseed*/

    memcpy(seed_material, entropy_input_reseed, sizeof(entropy_input_reseed));
    memcpy(seed_material + sizeof(entropy_input_reseed), additional_input_reseed, sizeof(additional_input_reseed));

    /*drbg reseed*/
    hmac_drbg_update(&hmac_drbg_ctx, (uint8_t *) seed_material, sizeof(entropy_input_reseed) + sizeof(additional_input_reseed));

    for (i = 0; i < NUMBER_OF_GENERATES; i++) {
        hmac_drbg_generate(result, &hmac_drbg_ctx, 128,  (uint8_t *)(additional_input[i]), 32);
    }

    /*compare result*/
    if ( memcmp(result, expect_result, sizeof(expect_result)) == 0) {
        printf("Success for  CAVS 14.3 DRBG800-90A test case line 5231 \r\n");
    } else {
        printf("OOPS, check hmac_drbg_test case3 \r\n");
    }

    return;
}

int main(void) {


    printf("Crypto NIST HMAC DRBG %s %s \r\n", __DATE__, __TIME__);

    sha256_vector_init();

    hmac_test_vector1();

    hmac_test_vector2();

    hmac_test_vector3();

    printf("finish crypto NIST hmac_drbg test\r\n");

    while (1);
}