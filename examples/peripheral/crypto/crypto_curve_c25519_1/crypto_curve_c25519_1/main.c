#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rt_crypto.h"
#include "rt_sha256.h"
#include "hosal_crypto_ctr_drbg.h"
#include "uart_stdio.h"



#define curve25519_in_bytes       32

unsigned char alice_public_key[curve25519_in_bytes],
         alice_shared_key[curve25519_in_bytes];
unsigned char bruce_public_key[curve25519_in_bytes],
         bruce_shared_key[curve25519_in_bytes];

/*this is rfc7748 example... Notice: RFC7748 test vector is little endian format*/

unsigned char alice_secret_key[curve25519_in_bytes] = {
    0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d,
    0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45,
    0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a,
    0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a

};

unsigned char bruce_secret_key[curve25519_in_bytes] = {
    0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b,
    0x79, 0xe1, 0x7f, 0x8b, 0x83, 0x80, 0x0e, 0xe6,
    0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18, 0xb6, 0xfd,
    0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb

};

unsigned char BasePoint[curve25519_in_bytes] = {0x09};



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


    hosal_crypto_curve25519_t curve25519;


    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto curve c25519 demo\r\n");
    printf("----------------------------------------------------------------\r\n");   
    /*notice stack issue...*/

    /* Step 1. init to load curve25519 firmware  */
    hosal_crypto_curve_c25519_init();

    /* Step 2. Alice and Bruce generate their own random secret keys */
    lecp_print_hex_bytes("Alice_secret_key", alice_secret_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_secret_key", bruce_secret_key, curve25519_in_bytes);

    /* Step 3. Alice and Bruce create public keys associated with their secret keys */
    /*         and exchange their public keys by some method... */

    /*Here we set blind_zr to NULL, so the curve25516_point_mul will use default blind_zr.*/
    //curve25519_point_mul(NULL, (uint32_t *)alice_public_key, (uint32_t *)alice_secret_key, (uint32_t *)BasePoint);
    curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
    curve25519.blind_zr = NULL;
    curve25519.public_key = (uint32_t *)alice_public_key;
    curve25519.secret_key = (uint32_t *)alice_secret_key;
    curve25519.base_point = (uint32_t *)BasePoint;
    hosal_crypto_curve25519_operation(&curve25519);

    //curve25519_point_mul(NULL, (uint32_t *)bruce_public_key, (uint32_t *)bruce_secret_key, (uint32_t *)BasePoint);
    curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
    curve25519.blind_zr = NULL;
    curve25519.public_key = (uint32_t *)bruce_public_key;
    curve25519.secret_key = (uint32_t *)bruce_secret_key;
    curve25519.base_point = (uint32_t *)BasePoint;
    hosal_crypto_curve25519_operation(&curve25519);

    lecp_print_hex_bytes("Alice_public_key", alice_public_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_public_key", bruce_public_key, curve25519_in_bytes);

    /* Step 4. Alice and Bruce create their shared key */

    //curve25519_point_mul( NULL, (uint32_t *)alice_shared_key,  (uint32_t *)alice_secret_key, (uint32_t *)bruce_public_key);
    curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
    curve25519.blind_zr = NULL;
    curve25519.public_key = (uint32_t *)alice_shared_key;
    curve25519.secret_key = (uint32_t *)alice_secret_key;
    curve25519.base_point = (uint32_t *)bruce_public_key;
    hosal_crypto_curve25519_operation(&curve25519);

    //curve25519_point_mul( NULL, (uint32_t *)bruce_shared_key, (uint32_t *)bruce_secret_key, (uint32_t *)alice_public_key);
    curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
    curve25519.blind_zr = NULL;
    curve25519.public_key = (uint32_t *)bruce_shared_key;
    curve25519.secret_key = (uint32_t *)bruce_secret_key;
    curve25519.base_point = (uint32_t *)alice_public_key;
    hosal_crypto_curve25519_operation(&curve25519);

    /*Now Alice and Bruce have share private key*/
    lecp_print_hex_bytes("Alice_shared", alice_shared_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_shared", bruce_shared_key, curve25519_in_bytes);

    /*Release crypto accelator*/
    hosal_crypto_curve_c25519_release();

    /* Alice and Bruce should end up with idetntical keys */
    if (memcmp(alice_shared_key, bruce_shared_key, curve25519_in_bytes) != 0) {
        printf("DH key exchange FAILED!!\r\n");
        while (1);
    } else {
        printf("Curve25519 DH key exchange Success!!\r\n");
    }

    printf("\r\n\r\n");
    printf("hosal crypto Curve25519 finish!\r\n");
    while (1);
}
