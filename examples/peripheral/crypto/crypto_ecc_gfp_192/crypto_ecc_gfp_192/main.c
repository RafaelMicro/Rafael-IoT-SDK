#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes.h"
#include "hosal_crypto_ecc.h"
#include "uart_stdio.h"



//private key
uint8_t prva[24] = {
    0xff, 0x18, 0xa5, 0xf4, 0xef, 0xd2, 0x5e, 0x62,
    0x2b, 0x14, 0x0c, 0xcf, 0xd6, 0xf1, 0x5d, 0x00,
    0x27, 0xdc, 0x8d, 0x91, 0x86, 0x5f, 0x91, 0x07
};

/* Curve P192 Gx & Gy parameters and used for the
  hardware public key caculation result */
uint32_t p_x[secp192r1_op_num] = {
    0x82FF1012, 0xF4FF0AFD, 0x43A18800,
    0x7CBF20EB, 0xB03090F6, 0x188DA80E
};
uint32_t p_y[secp192r1_op_num] = {
    0x1E794811, 0x73F977A1, 0x6B24CDD5,
    0x631011ED, 0xFFC8DA78, 0x07192B95
};

//caculation result verify
uint32_t result_x[secp192r1_op_num] = {
    0x1125f8ed, 0xd2809ea5, 0xfe7e4329,
    0x586f9fc3, 0x984421a6, 0x15207009
};
uint32_t result_y[secp192r1_op_num] = {
    0xb9f7ea25, 0x7fca856f, 0x9dbbaa85,
    0x9f79e4b5, 0x1bc5bd00, 0xb09d42b8
};



//------------------ ECC GF(2m) ------------------

int main(void) {
    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gfp192 demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_ecc_gf_t ecc_p192;
   
    hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P192_INIT);

    ecc_p192.crypto_operation = HOSAL_GFP_P192_MULTI;
    ecc_p192.p_result_x = p_x;
    ecc_p192.p_result_y = p_y;
    ecc_p192.target_x = p_x;
    ecc_p192.target_y = p_y;
    ecc_p192.target_k = (uint32_t*)prva;
    hosal_crypto_ecc_gf_operation(&ecc_p192);

    //compare HW result with pattern
    if ( (0 == memcmp((char*) p_x, (char*) result_x, 24)) &&
         (0 == memcmp((char*) p_y, (char*) result_y, 24)) ) {
        printf("ECC_GFP_test_P192 SUCCESS!\n");
    } else {
        printf("ECC_GFP_test_P192 FAILURE!\n");
    }

    printf("\r\n\r\n");
    printf("hosal crypto ecc gfp192 demo finish\r\n");

    while (1);
}
