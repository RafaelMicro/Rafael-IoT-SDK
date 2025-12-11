/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "FreeRTOS.h"
#include "task.h"
#include "trng.h"
#include "hosal_crypto_ecc.h"
#include "hosal_crypto_ctr_drbg.h"
#include "app_hooks.h"
#include "uart_stdio.h"

void becp_print_hex_bytes(const char *name, const uint8_t *data, uint32_t size)
{
    uint32_t i = 0;

    printf("%s = 0x", name);

    while (i < size)
    {
        printf("%02X", data[(size - 1) - i]);
        i++;
    }

     printf("\r\n");
}

void ecc_gfp_p256_point_add_exmples(void) {
    uint32_t   status;

    /*random choice a number key1  --- little endian*/
    uint32_t key1[secp256r1_op_num];

    /*random choice a number  key2 --- little endian*/
    uint32_t key2[secp256r1_op_num];

    uint32_t result_sum_key[secp256r1_op_num];

    uint32_t    i, k, carry; //j;

    /*  SECP256R1 generator defined in library, used for the
     *  hardware public key caculation result
     */

    //caculation result verify

    ECPoint_P256   result_point, point_1,  point_2, expect_point;
    hosal_crypto_ecc_p256_t ecc_p256;
    /*
     * caculate result_sum_key = key1+key2
     *
     */

    for (i = 0; i < 1024; i++) {
        printf("test case %"PRIu32"/1024 \r\n", (i + 1));

re_generate_key:
        /*key last 8 bytes are random generate*/
        get_random_number(key1, 8);

        get_random_number(key2, 8);

        /*point add.*/
        carry = 0;
        for (k = 0; k < secp256r1_op_num; k++) {
            result_sum_key[k] = key1[k] + key2[k] + carry;
            if (result_sum_key[k] < key1[k]) {
                /*overflow*/
                carry = 1;
            } else {
                carry = 0;
            }
        }

        if (carry == 1) {
            /* bad news, key1+key2 > order, the sum must be (key1+key2) mod N
             * we don't this check..so skip this case
             * In real case, it is impossible using private key to add..
             * so we can forget this case happen..
             * it always using public key add... not private key add.
             */
            goto re_generate_key;
        }
        


        becp_print_hex_bytes("key1 :", (uint8_t*) (key1), (secp256r1_op_num << 2));
        becp_print_hex_bytes("key2 :", (uint8_t*) (key2), (secp256r1_op_num << 2));
        becp_print_hex_bytes("sum  :", (uint8_t*) (result_sum_key),
                             (secp256r1_op_num << 2));


        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);


        ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
        ecc_p256.result = (ECPoint_P256*) &point_1;
        ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
        ecc_p256.p_key = key1;

        hosal_crypto_ecc_p256(&ecc_p256);

        becp_print_hex_bytes("point1 x :", (uint8_t*) (point_1.x),
                             (secp256r1_op_num << 2));
        becp_print_hex_bytes("point1 y :", (uint8_t*) (point_1.y),
                             (secp256r1_op_num << 2));
        printf("\r\n");


        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);


        ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
        ecc_p256.result = (ECPoint_P256*) &point_2;
        ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
        ecc_p256.p_key = key2;

        hosal_crypto_ecc_p256(&ecc_p256);

        becp_print_hex_bytes("point2 x :", (uint8_t*) (point_2.x),
                             (secp256r1_op_num << 2));
        becp_print_hex_bytes("point2 y :", (uint8_t*) (point_2.y),
                             (secp256r1_op_num << 2));
         printf("\r\n");



        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);

        ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
        ecc_p256.result = (ECPoint_P256*) &expect_point;
        ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
        ecc_p256.p_key = result_sum_key;

        hosal_crypto_ecc_p256(&ecc_p256);

        ecc_p256.crypto_operation = HOSAL_GFP_P256_ADD;
        ecc_p256.p_point_result = (ECPoint_P256*) &result_point;
        ecc_p256.p_point_x1 =  (ECPoint_P256*) &point_1;
        ecc_p256.p_point_x2 = (ECPoint_P256*) &point_2;

        status = hosal_crypto_ecc_p256(&ecc_p256);

        if (status != STATUS_SUCCESS) {
            printf("Oops...gfp_point_p256_add return fail \n ");
            while (1);
        } else {
            becp_print_hex_bytes("result x :", (uint8_t*) (result_point.x),
                                 (secp256r1_op_num << 2));
            becp_print_hex_bytes("result y :", (uint8_t*) (result_point.y),
                                 (secp256r1_op_num << 2));
            printf("\r\n");
        }

        ecc_p256.crypto_operation = HOSAL_GFP_P256_VAILD_VERIFY;
        ecc_p256.result = (ECPoint_P256*) &result_point;
        status =hosal_crypto_ecc_p256(&ecc_p256);

        if (status != STATUS_SUCCESS) {
            /*this error should not happen*/
            printf("Error... point should be in SECP256R1 curve \n");
            while (1);
        }

        if ( ( memcmp((uint8_t*) (result_point.x), (uint8_t*) (expect_point.x),
                      (secp256r1_op_num << 2)) != 0) ||
             ( memcmp((uint8_t*) (result_point.y), (uint8_t*) (expect_point.y),
                      (secp256r1_op_num << 2)) != 0) ) {
            /*this error should not happen*/
            printf("Error... point sum is not point multi \n");
            while (1);
        }

    }

}

//------------------ ECC GF(2m) ------------------
int main(void) {
    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gfp_p256 add demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    ecc_gfp_p256_point_add_exmples();

    printf("\r\n\r\n");
    printf("hosal crypto ecc gfp_p25_add finish \n");
    while (1);
}
