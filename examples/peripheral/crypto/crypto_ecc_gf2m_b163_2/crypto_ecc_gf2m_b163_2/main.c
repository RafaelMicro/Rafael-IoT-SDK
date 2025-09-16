#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

uint32_t key[6] = {
    0xA4234C1F,  0x77E70C12, 0x000292FE,
    0x00000000, 0x00000000,  0x00000004
};

uint32_t public_key_x[6];
uint32_t public_key_y[6];

typedef struct  ECC163_vector
{
    uint8_t    x[42];            /*HEX Table...Notice this is string!! not binary*/
    uint8_t    y[42];
} ECC_B163_vector_st;

const ECC_B163_vector_st ecc_b163_table[] =
{
    {
        "00AED08C6DDCF8E345006BD2F6989C3F92CB508A82",
        "02FD44F3B8F6E5D138CA9EF0B7EA5E711375724831",
    },
    {
        "01A55C68CE800F55118C74751EE8F99770A65B14F6",
        "00143159A1B6CC1C0CC69CC6B129255B94B24C4014",
    },
    {
        "01F2DC4C1A649043F1622F611986E84074EBE3F692",
        "020B000373EAB5B2EE6250306F4401C257ADA1A485",
    },
    {
        "0714E4DADA0AB682D036AF06DDBA3CCAD123E5734B",
        "015DFA5097A6C1DC5E031CB15CC84597BC330B214B",
    },
    {
        "041FBD3ADBAB2C4349F5518C8BC4BD531F079DC92B",
        "047EA309BE3CCFEA8A5E130DCFB7A09746A2CDC635",
    },
    {
        "01880F725B918ABA057E6DE329ABDFEEF475AE9483",
        "03A84E2CAF05206BCC4986826A1A5F7E4B3F809195",
    },
    {
        "03566B99AE5EEEB921C9618E514A8AD50506A73F75",
        "07F7D27AB8B1694676449475D45E3468D821ABE66E",
    },
    {
        "07C565F87A02BFBAD2E0F3517F74392AC60036A5EB",
        "0500331999BF9D5C74C34C114CC7DA7FD20900FCBE",
    },
    {
        "05AA3CAE634590B66A3F18A64E47B1C9B3B509E80C",
        "07F21FEA6242FCB87C08820563B724CA9BEF1703EA",
    },
    {
        "0696E27054D49E19B15ED4240AA2F5942A06F25BB5",
        "04855489972178974DF1F20080A27A622A95B19CC9",
    },
    {
        "0507E541410F581B0D6914C2183C9313E7CAA10915",
        "060423939F668BF4D6BB1DA3A3AB714CFCE8D58A54",
    },
    {
        "04802FB7306AE7CAA87F08815BABDFEEBBA9E7A7D3",
        "0198A816A93DDA46F6515C7B241D5A7224F35DA585",
    },
    {
        "04547BD66270DF7A9601351A616FEF080D44528B03",
        "05C778E64F13EAEAA0B172531CAD1EB399FF6F18C7",
    },
    {
        "043EAAAF4BEA5A8C0A3EB105B31A0CF6ABAD87B13A",
        "01C4726118438D7149522989C98927FD79DF0E1045",
    },
    {
        "065AD02C42180EA317348FFE342FB1CF2A3E896195",
        "060E06D566BA86A842334A656F593B72FCB6B5A8DA",
    },
    {
        "07205899683630522F4C657BB52764867DA449F864",
        "04220BE69D6B9DF2B99764B1CC271E757C911933F8",
    },
    {
        "04053748C8CCD84AF888D3E7623F4FF3B75D153F39",
        "024E3E405C57B2C979DB46DC64F2596F744CE0C29E",
    },
    {
        "0634000577F86AA315009D6F9B906691F6EDD691FE",
        "0235A3DB7A94446301E666CAFEA5E12CB331F4A140",
    },
    {
        "01AEB33FED9C49E0200A0C561EA66D5AB85BD4C2D4",
        "049ED3BE7F510E30E2462C517AD39038E493FC573C",
    },
    {
        "03F0EBA16286A2D57EA0991168D4994637E8343E36",
        "0325F41D0EF702DC310254C42D65851A3B91471AC7",
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
    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto ecc gf2m b163 demo 2 \r\n");
    printf("----------------------------------------------------------------\r\n");

    printf("compare test vector please see http://point-at-infinity.org/ecc/nisttv \r\n");
    uint32_t   i;
    uint8_t*  table_index;

    hosal_crypto_ecc_gf_t ecc_b163;

    for (i = 0; i < 20; i++) {
        //Crypto engine init
        hosal_crypto_ecc_init(HOSAL_ECC_CURVE_B163_INIT);

        ecc_b163.crypto_operation = HOSAL_GFP_B163_MULTI;
        ecc_b163.p_result_x = public_key_x;
        ecc_b163.p_result_y = public_key_y;
        ecc_b163.target_x = p_x;
        ecc_b163.target_y = p_y;
        ecc_b163.target_k = (uint32_t*)key;
        hosal_crypto_ecc_gf_operation(&ecc_b163);
        //start the crypto engine caculation
        table_index = (uint8_t*) (&ecc_b163_table[i].x);
        compare_and_disaply("B163 x:", (uint8_t*) public_key_x, 21, table_index);

        table_index = (uint8_t*) (&ecc_b163_table[i].y);
        compare_and_disaply("B163 y:", (uint8_t*) public_key_y, 21, table_index);

        printf("\r\n");

        key[0]++;
    }

    printf("\r\n\r\n");
    printf("hosal crypto ecc gf2m b163 demo 2 finish\r\n");

    while (1);
}
