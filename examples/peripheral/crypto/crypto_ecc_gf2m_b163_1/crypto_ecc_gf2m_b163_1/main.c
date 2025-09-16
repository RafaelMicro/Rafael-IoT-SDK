#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rt_crypto.h"
#include "hosal_crypto_ecc.h"
#include "uart_stdio.h"


/* GF2m_ECC_B163's key should be NOT greater than (<=) 21 bytes */
/*Here we for 4-bytes alignment, so we use 24 bytes.*/

/* Remark: Data is in Little-Endian */

//Curve B163 Gx & Gy parameters and used for the hardware public key caculation result
uint32_t p_x[6] = {
    0xE8343E36, 0xD4994637, 0xA0991168,
    0x86A2D57E, 0xF0EBA162, 0x00000003
};
uint32_t p_y[6] = {
    0x797324f1, 0xb11c5c0c, 0xa2cdd545,
    0x71a0094f, 0xd51fbc6c, 0x00000000
};

/* This Key is for demo use only. It's not a good choice for private key.
  Key = 5846006549323611672814742442876390689256843201577 (decimal)
  Key = 04 00000000 00000000 000292FE 77E70C12 A4234C29   (hex)
  x =   05 07E54141 0F581B0D 6914C218 3C9313E7 CAA10915
  y =   06 0423939F 668BF4D6 BB1DA3A3 AB714CFC E8D58A54
*/
uint32_t key[6] = {
    0xA4234C29, 0x77E70C12, 0x000292FE,
    0x00000000, 0x00000000, 0x00000004
};

uint32_t public_key_x[6];
uint32_t public_key_y[6];

//expected result for verify
uint32_t exp_x[6] = {
    0xCAA10915, 0x3C9313E7, 0x6914C218,
    0x0F581B0D, 0x07E54141, 0x00000005
};

uint32_t exp_y[6] = {
    0xE8D58A54, 0xAB714CFC, 0xBB1DA3A3,
    0x668BF4D6, 0x0423939F, 0x00000006
};

//------------------ ECC GF(2m) ------------------

int main(void) {


    hosal_crypto_ecc_gf_t ecc_b163;



    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gf2m b163 demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_B163_INIT);

    ecc_b163.crypto_operation = HOSAL_GFP_B163_MULTI;
    ecc_b163.p_result_x = public_key_x;
    ecc_b163.p_result_y = public_key_y;
    ecc_b163.target_x = p_x;
    ecc_b163.target_y = p_y;
    ecc_b163.target_k = (uint32_t*)key;
    hosal_crypto_ecc_gf_operation(&ecc_b163);

    if ( 0 == memcmp((char*) exp_x, (char*) public_key_x, 21)  &&
         0 == memcmp((char*) exp_y, (char*) public_key_y, 21)  ) {
        printf("ecc_gf2m_b163_test SUCCESS!\n");
    } else {
        printf("ecc_gf2m_b163_test FAILURE!\n");
    }

    printf("\r\n\r\n");
    printf("hosal crypto ecc gf2m b163 demo finish \r\n");

    while (1);
}
