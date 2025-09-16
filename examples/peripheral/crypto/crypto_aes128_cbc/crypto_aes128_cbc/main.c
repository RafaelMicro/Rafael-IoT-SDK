#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes.h"
#include "uart_stdio.h"



uint8_t key[HOSAL_AES_BLOCKLEN] =
{
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

uint8_t   plain_text_buf[128] = {
        "Example string to demonstrate AES CBC mode with padding. This text has 85 characters."
    };

static uint8_t m_encryption_text[128];

static uint8_t m_decryption_text[128];

//------------------ ECC GF(2m) ------------------

int main(void) {

    uint32_t  length, i;
    uint8_t   padding_data;
    uint8_t   *p_string_end;
    hosal_aes_dev_t aes_dev;

    uart_stdio_init();
    /*
     *   Please notice: plain_text_buf is only 85 bytes. It is not 64N...
     * so we need add padding in the end of plain_text. In this example we
     * will add 11 bytes. Add padding has many format, like all zero padding,
     * ISO-7816-4 padding,  PKCS#7, or ....
     * In this example, we use PKCS#7 padding.
     */


    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : crypto aes 128 cbc demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_aes_init();

    length = strlen((char *)plain_text_buf);

    p_string_end = (uint8_t *) plain_text_buf + length;
    /*padding data will equal (16-length%16) */
    padding_data = HOSAL_AES_BLOCKLEN - (length & (HOSAL_AES_BLOCKLEN - 1));

    /*add PKCS#7 padding*/
    for (i = 0; i < padding_data; i++)
    {
        *p_string_end++ = padding_data;
    }

    length = strlen((char *) plain_text_buf);

	aes_dev.crypto_operation = HOSAL_AES_CRYPTO_ENCRYPT;
	aes_dev.bit = HOSAL_AES_128_BIT;
	aes_dev.in_ptr = plain_text_buf;
	aes_dev.out_ptr = m_encryption_text;
	aes_dev.key_ptr = key;
	aes_dev.cbc_length = length;
 
	hosal_crypto_aes_cbc_operation(&aes_dev);
    /*******************************************/

	aes_dev.crypto_operation = HOSAL_AES_CRYPTO_DECRYPT;
	aes_dev.bit = HOSAL_AES_128_BIT;
	aes_dev.in_ptr = m_encryption_text;
	aes_dev.out_ptr = m_decryption_text;
	aes_dev.key_ptr = (uint8_t*)key;
	aes_dev.cbc_length = length;
	
	hosal_crypto_aes_cbc_operation(&aes_dev);
	
   

    if (0 == memcmp((char *) plain_text_buf, (char *) m_decryption_text, length)) {

        printf("AES_128_cbc_test : SUCCESS!\r\n");
    }
    else {

        printf("AES_128_cbc_test : FAILURE!\r\n");
    }

    printf("\r\n\r\n");
    printf("hosal crypto aes 128 cbc finish!\r\n");
    while (1);
}
