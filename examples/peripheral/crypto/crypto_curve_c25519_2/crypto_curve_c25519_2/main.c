#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rt_crypto.h"
#include "rt_sha256.h"
#include "hosal_crypto_ctr_drbg.h"
#include "uart_stdio.h"


#define curve25519_in_bytes       32


unsigned char k[curve25519_in_bytes] = {0x09, 0x00};
unsigned char u[curve25519_in_bytes] = {0x09, 0x00};
unsigned char temp[curve25519_in_bytes] = {0x00};// orig_k[curve25519_in_bytes] = {0x00};

unsigned char expect_result[curve25519_in_bytes] = {
    0x68, 0x4C, 0xF5, 0x9B, 0xA8, 0x33, 0x09, 0x55,
    0x28, 0x00, 0xEF, 0x56, 0x6F, 0x2F, 0x4D, 0x3C,
    0x1C, 0x38, 0x87, 0xC4, 0x93, 0x60, 0xE3, 0x87,
    0x5F, 0x2E, 0xB9, 0x4D, 0x99, 0x53, 0x2C, 0x51
};


void lecp_print_hex_bytes(const char* name, const uint8_t* data,
                          uint32_t size) {
    uint32_t i = 0;

    printf("%s = 0x", name);

    while (i < size) {
        printf("%02X", data[i++]);
    }

    printf("\r\n");
}

//------------------ ECC GF(2m) ------------------

int main(void) {

    
     int i;
    hosal_crypto_curve25519_t curve25519;

    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto curve c25519 demo 2\r\n");
    printf("----------------------------------------------------------------\r\n");

    /*this is rfc7748 example... Notice: RFC7748 test vector is little endian format*/
    printf("\r\n-- curve25519 --RFC 7748 test vector example------------\r\n");

    printf("This test could be running more than 2 mins \r\n");

    hosal_crypto_curve_c25519_init();

    for (i = 0; i < 1000; i++) {
        //u is basepoint, k is key. Please see  RFC7748 test vector for this setting.
        /*Here we set blind_zr to NULL, so the curve25516_point_mul will use default blind_zr.*/
        curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
        curve25519.blind_zr = NULL;
        curve25519.public_key = (uint32_t*)temp;
        curve25519.secret_key = (uint32_t*)k;
        curve25519.base_point = (uint32_t*)u;
        hosal_crypto_curve25519_operation(&curve25519);

        memcpy(u, k, curve25519_in_bytes);
        memcpy(k, temp, curve25519_in_bytes);

        printf(".");
        if((i!=0) && (i%64)==0) {
             printf("\r\n");
        }

    }

    hosal_crypto_curve_c25519_release();
    /*Release crypto accelator*/
    /*compare data*/
    for (i = 0; i < curve25519_in_bytes; i++) {
        if (temp[i] != expect_result[i]) {
            printf("rfc7748 TEST LOOP 1000 FAILS \r\n");
            while (1);
        }
    }

    printf("\r\n\r\n");
    printf("curve25519 loop 1000 finsih \r\n");

    while (1);
}
