#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_ecc.h"
#include "hosal_crypto_ctr_drbg.h"
#include "uart_stdio.h"



void ecc_gfp_p256_ecdh_example(void) {
    /*
     *  secp256r1
     *
     *  Remark: This is demo sample only.
     *  HERE PRIVATE KEY IS TEST ONLY. Private Key should be random generated.
     *  It can NOT be hardcode, otherwise key will be extracted from binary or hex file.
     *
     *     Please Notice: Share key is in little endian format...
     *  So if you want to use the share key for some cryption, please notice
     *  key format is little-endian or big-endian
     */

    /*Remark: data is little endian format. that is MSB is the last byte. */

    uint32_t Alice_privK[8] = {
        0x944e176d, 0x093564e1, 0x89284eba, 0x33753259,
        0x43a17f6a, 0x7092e5fd, 0xc486e0f6, 0x5aeb8123
    };

    uint32_t Bob_privK[8] = {
        0x49b882ce, 0xa8683d84, 0x60907cb1, 0xcc562765,
        0xec0b1f52, 0xd6185566, 0xf975a3bf, 0x711c0eef
    };

    ECPoint_P256   Alice_Public_key;
    ECPoint_P256   Bob_Public_key;

    ECPoint_P256   Alice_Share_key;
    ECPoint_P256   Bob_Share_key;

    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);

    /*Alice uses private key to generate Alice's public key*/

    hosal_crypto_ecc_p256_t ecc_p256;
    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*) &Alice_Public_key;
    ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
    ecc_p256.p_key = Alice_privK;

    hosal_crypto_ecc_p256(&ecc_p256);

   
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);
    /*Bob uses private key to generate Bob's public key*/


    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*) &Bob_Public_key;
    ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
    ecc_p256.p_key = Bob_privK;

    hosal_crypto_ecc_p256(&ecc_p256);

    /*Alice sends her public key to Bob, and Bob sends his public key to Alice*/

    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);
    /*Alice caculate the secert shared key*/


    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*) &Alice_Share_key;
    ecc_p256.base =  (ECPoint_P256*) &Bob_Public_key;
    ecc_p256.p_key = Alice_privK;

    hosal_crypto_ecc_p256(&ecc_p256);

    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);
    /*Bob caculate the secert shared key*/
    ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
    ecc_p256.result = (ECPoint_P256*) &Bob_Share_key;
    ecc_p256.base =  (ECPoint_P256*) &Alice_Public_key;
    ecc_p256.p_key = Bob_privK;

    hosal_crypto_ecc_p256(&ecc_p256);


    if ( (0 == memcmp((char*) (Alice_Share_key.x), (char*) (Bob_Share_key.x), 32))
         &&
         (0 == memcmp((char*) (Alice_Share_key.y), (char*) (Bob_Share_key.y), 32)) ) {
        printf("ECDH256 test: SUCCESS!\n");
    } else {
        printf("ECDH256 test: FAILURE!\n");
    }

}




//------------------ ECC GF(2m) ------------------

int main(void) {

    
    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gfp_p256 ecdh demo \r\n");
    printf("----------------------------------------------------------------\r\n");


    ecc_gfp_p256_ecdh_example();

    printf("\r\n\r\n");
    printf("hosal crypto ecc gfp_p25_ecdh finish \n");

    while (1);
}
