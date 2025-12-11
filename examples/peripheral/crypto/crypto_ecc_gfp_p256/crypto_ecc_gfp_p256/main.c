/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_ecc.h"
#include "hosal_crypto_ctr_drbg.h"
#include "uart_stdio.h"


typedef struct  ECC256_vector
{
    uint8_t    x[64];            /*HEX Table... Notice this is string!! not binary*/
    uint8_t    y[64];
} ECC256_vector_st;

const ECC256_vector_st ecc_256_table[] =
{
    {
        "76A94D138A6B41858B821C629836315FCD28392EFF6CA038A5EB4787E1277C6E",
        "567A019DCBE0D9F2934F5E4A1EE178DF7A665FFCF0387455F162228DB473AEEF"
    },
    {
        "F0454DC6971ABAE7ADFB378999888265AE03AF92DE3A0EF163668C63E59B9D5F",
        "4A46C11BA6D1D2E1B19A6B1AE069BC19D5C4DE328A4A05C0B81A6321F2FCB0C9"
    },
    {
        "54E77A001C3862B97A76647F4336DF3CF126ACBE7A069C5E5709277324D2920B",
        "0A660E43D60BCE8BBDEDE073FA5D183C8E8E15898CAF6FF7E45837D09F2F4C8A",
    },
    {
        "177C837AE0AC495A61805DF2D85EE2FC792E284B65EAD58A98E15D9D46072C01",
        "9C44A731B1415AA85DBF6E524BF0B18DD911EB3D5E04B20C63BC441D10384027",
    },
    {
        "741DD5BDA817D95E4626537320E5D55179983028B2F82C99D500C5EE8624E3C4",
        "F88F4B9463C7A024A98C7CAAB7784EAB71146ED4CA45A358E66A00DD32BB7E2C",
    },
    {
        "3ED113B7883B4C590638379DB0C21CDA16742ED0255048BF433391D374BC21D1",
        "6F66DF64333B375EDB37BC505B0B3975F6F2FB26A16776251D07110317D5C8BF",
    },
    {
        "CEF66D6B2A3A993E591214D1EA223FB545CA6C471C48306E4C36069404C5723F",
        "78799D5CD655517091EDC32262C4B3EFA6F212D7018AE11135CB4455BB50F88C",
    },
    {
        "EA68D7B6FEDF0B71878938D51D71F8729E0ACB8C2C6DF8B3D79E8A4B90949EE0",
        "D5D8BB358D36031978FEB569B5715F37B28EB0165B217DC017A5DDB5B22FB705",
    },
    {
        "62D9779DBEE9B0534042742D3AB54CADC1D238980FCE97DBB4DD9DC1DB6FB393",
        "52A533416E1627DCB00EA288EE98311F5D12AE0A4418958725ABF595F0F66A81",
    },
    {
        "8E533B6FA0BF7B4625BB30667C01FB607EF9F8B8A80FEF5B300628703187B2A3",
        "8C14E2411FCCE7CA92F9607C590A6FFFAC38C9CD34FBE4DE3AA1E5793E0BFF4B",
    },
    {
        "B01A172A76A4602C92D3242CB897DDE3024C740DEBB215B4C6B0AAE93C2291A9",
        "17A3EF8ACDC8252B9013F1D20458FC86E3FF0890E381E9420283B7AC7038801D",
    },
    {
        "51590B7A515140D2D784C85608668FDFEF8C82FD1F5BE52421554A0DC3D033ED",
        "1F3E82566FB58D83751E40C9407586D9F2FED1002B27F7772E2F44BB025E925B",
    },
    {
        "E2534A3532D08FBBA02DDE659EE62BD0031FE2DB785596EF509302446B030852",
        "1F0EA8A4B39CC339E62011A02579D289B103693D0CF11FFAA3BD3DC0E7B12739",
    },
    {
        "5ECBE4D1A6330A44C8F7EF951D4BF165E6C6B721EFADA985FB41661BC6E7FD6C",
        "78CB9BF2B6670082C8B4F931E59B5D1327D54FCAC7B047C265864ED85D82AFCD",
    },
    {
        "7CF27B188D034F7E8A52380304B51AC3C08969E277F21B35A60B48FC47669978",
        "F888AAEE24712FC0D6C26539608BCF244582521AC3167DD661FB4862DD878C2E",
    },

};

//------------------ ECC GF(2m) ------------------
void compare_and_disaply(const char* name, const uint8_t* data, uint32_t size,
                         uint8_t* table_index) {
    uint32_t i = 0, ERROR = 0;

    uint8_t  value, *ptr;
    printf("%s = 0x", name);

    ptr = table_index;

    while (i < size) {
        printf("%02X", data[(size - 1) -
                                       i]);   /*little endian data to be print in big endian*/

        value = 0;

        /*Hex string to binary --- HERE we don't care invalid value..because table is create by us.*/
        if (*ptr >= 0x41) {
            value = ((*ptr - 0x41) + 0xA) << 4;
        } else {
            value = (*ptr - 0x30) << 4;
        }

        ptr++;

        if (*ptr >= 0x41) {
            value += ((*ptr - 0x41) + 0xA);
        } else {
            value += (*ptr - 0x30) ;
        }

        ptr++;

        if (data[(size - 1) - i] != value) {
            ERROR = 1;
        }

        i++;

    }

    if (ERROR == 1) {
        printf("\n ERROR output value is not correct!!\n");
        while (1);
    }

    printf("\r\n");

}

int main(void) {
    
    

    int        i;
    uint32_t   status;

   


    /*
     * Notice: p256  order "N" is
     *       0xFFFFFFFF 00000000 FFFFFFFF FFFFFFFF BCE6FAAD A7179E84 F3B9CAC2 FC632551
     *
     *  NG = 0. "0" is not in ECC curve. It's illegal value.
     *  private Key should not be equal or greater than this value.
     *
     */

    uint32_t key[secp256r1_op_num] = {
        0xFC632551 - 16, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD,
        0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
    };

    /*  SECP256R1 generator defined in library, used for the
     *  hardware public key caculation result
     */

    ECPoint_P256   result_point;

    uint8_t*  table_index;
    hosal_crypto_ecc_p256_t ecc_p256;

    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gfp_p256 demo \r\n");
    printf("----------------------------------------------------------------\r\n");

    for (i = 0; i < 15; i++) {

        //gfp_ecc_curve_p256_init();
        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_P256_INIT);

        ecc_p256.crypto_operation = HOSAL_GFP_P256_MULTI;
        ecc_p256.result = (ECPoint_P256*) &result_point;
        ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
        ecc_p256.p_key = key;

        status = hosal_crypto_ecc_p256(&ecc_p256);

        /*try to dump vector*/

        if (status == STATUS_SUCCESS) {
            table_index = (uint8_t*) (&ecc_256_table[i].x);
            compare_and_disaply("p256 x:", (uint8_t*) (result_point.x), 32, table_index);

            table_index = (uint8_t*) (&ecc_256_table[i].y);
            compare_and_disaply("p256 y:", (uint8_t*) (result_point.y), 32, table_index);

            printf("\n");
        } else {
            printf("Invalid key.. key=0 or k=N\n");
            break;
        }

        /*let we check result_x and resulty is in SECP256R1 or not... */

        ecc_p256.crypto_operation = HOSAL_GFP_P256_VAILD_VERIFY;
        ecc_p256.result = (ECPoint_P256*) &result_point;
        ecc_p256.base =  (ECPoint_P256*) &Curve_Gx_p256;
        ecc_p256.p_key = key;

        status = hosal_crypto_ecc_p256(&ecc_p256);

        if (gfp_valid_point_p256_verify( &result_point) != STATUS_SUCCESS) {
            /*this error should not happen*/
            printf("Error... point should be in SECP256R1 curve %x \n", i);
            while (1);
        }


        key[0]++;
    }


    printf("\r\n\r\n");
    printf("hosal crypto ecc gfp 256 finish\r\n");
    while (1);
}
