#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes.h"
#include "hosal_crypto_ecc.h"
#include "uart_stdio.h"



    /*
     *  secp192r1
     *  Alice private key = 0x7092e5fd43a17f6a3375325989284eba093564e1944e176d
     *
     *  Bob private key = 0xd6185566ec0b1f52cc56276560907cb1a8683d8449b882ce
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

    uint32_t Alice_privK[secp192r1_op_num] = { 0x944e176d, 0x093564e1, 0x89284eba, 0x33753259, 0x43a17f6a, 0x7092e5fd};

    //Curve P192 Gx & Gy parameters and used for the hardware public key caculation result
    uint32_t p_x[secp192r1_op_num] = {0x82FF1012, 0xF4FF0AFD, 0x43A18800, 0x7CBF20EB, 0xB03090F6, 0x188DA80E};
    uint32_t p_y[secp192r1_op_num] = {0x1E794811, 0x73F977A1, 0x6B24CDD5, 0x631011ED, 0xFFC8DA78, 0x07192B95};

    //Public key for Alice.
    uint32_t Alice_Public_x[secp192r1_op_num] ;
    uint32_t Alice_Public_y[secp192r1_op_num] ;

    uint32_t Bob_privK[secp192r1_op_num] = { 0x49b882ce, 0xa8683d84, 0x60907cb1, 0xcc562765, 0xec0b1f52, 0xd6185566};

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


//------------------ ECC GF(2m) ------------------

int main(void) {
    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gfp192 ecdh demo \r\n");
    printf("----------------------------------------------------------------\r\n");

	hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P192_INIT);
	
	hosal_crypto_ecc_gf_t ecc_p192;
	
    //int hosal_crypto_ecc_p192(hosal_crypto_ecc_p192_t* ecc_p192)
    /*Alice uses private key to generate Alice's public key*/
	ecc_p192.crypto_operation=HOSAL_GFP_P192_MULTI;
	ecc_p192.p_result_x = Alice_Public_x; 
	ecc_p192.p_result_y =Alice_Public_y; 
	ecc_p192.target_x = p_x;
	ecc_p192.target_y = p_y;
	ecc_p192.target_k = Alice_privK;
	hosal_crypto_ecc_gf_operation(&ecc_p192);
		

	//gfp_ecc_curve_p192_init();
	hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P192_INIT);
    /*Bob uses private key to generate Bob's public key*/
	ecc_p192.crypto_operation=HOSAL_GFP_P192_MULTI;
	ecc_p192.p_result_x = Bob_Public_x; 
	ecc_p192.p_result_y =Bob_Public_y; 
	ecc_p192.target_x = p_x;
	ecc_p192.target_y = p_y;
	ecc_p192.target_k = Bob_privK;
	hosal_crypto_ecc_gf_operation(&ecc_p192);
    /*Alice sends her public key to Bob, and Bob sends his public key to Alice*/

	//gfp_ecc_curve_p192_init();
	hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P192_INIT);
    /*Alice caculate the secert shared key*/
	ecc_p192.crypto_operation=HOSAL_GFP_P192_MULTI;
	ecc_p192.p_result_x = Alice_Share_x; 
	ecc_p192.p_result_y =Alice_Share_y; 
	ecc_p192.target_x = Bob_Public_x;
	ecc_p192.target_y = Bob_Public_y;
	ecc_p192.target_k = Alice_privK;
	hosal_crypto_ecc_gf_operation(&ecc_p192);

	//gfp_ecc_curve_p192_init();
	hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P192_INIT);
    /*Bob caculate the secert shared key*/
	ecc_p192.crypto_operation=HOSAL_GFP_P192_MULTI;
	ecc_p192.p_result_x = Bob_Share_x; 
	ecc_p192.p_result_y =Bob_Share_y; 
	ecc_p192.target_x = Alice_Public_x;
	ecc_p192.target_y = Alice_Public_y;
	ecc_p192.target_k = Bob_privK;
	hosal_crypto_ecc_gf_operation(&ecc_p192);

    if ((0 == memcmp((char *) Alice_Share_x, (char *) Bob_Share_x, 24)) &&
            (0 == memcmp((char *) Alice_Share_y, (char *) Bob_Share_y, 24) ) )
    {
        printf("ecc gfp192 ecdh test: SUCCESS!\n");
    }
    else
    {
        printf("ecc gfp192 ecdh test: FAILURE!\n");

    }

    printf("\r\n\r\n");
    printf("hosal crypto ecc gfp192 ecdh finish\r\n");
    while (1);
}
