#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes.h"
#include "uart_stdio.h"

uint8_t m_encryption_text[120];

uint8_t m_decryption_text[120];

    /*Please see RFC 3686 Test Vector #3 for this example*/
    uint8_t  in[36] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23
    };

    uint8_t  expected_cipher[36] =
    {
        0xC1, 0xCF, 0x48, 0xA8, 0x9F, 0x2F, 0xFD, 0xD9,
        0xCF, 0x46, 0x52, 0xE9, 0xEF, 0xDB, 0x72, 0xD7,
        0x45, 0x40, 0xA4, 0x2B, 0xDE, 0x6D, 0x78, 0x36,
        0xD5, 0x9A, 0x5C, 0xEA, 0xAE, 0xF3, 0x10, 0x53,
        0x25, 0xB2, 0x07, 0x2F
    };

    uint8_t  key[16] =
    {
        0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8,
        0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC
    };

    uint8_t  iv[16]  =
    {
        0x00, 0xE0, 0x01, 0x7B, 0x27, 0x77, 0x7F, 0x3F,
        0x4A, 0x17, 0x86, 0xF0, 0x00, 0x00, 0x00, 0x01
    };

    uint8_t  stream_block[16];

//------------------ AES ------------------
int main(void) {
    

    hosal_aes_dev_t aes_dev;

    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : crypto aes 128 ctr demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_aes_init();

	
	aes_dev.crypto_operation = HOSAL_AES_CRYPTO_ENCRYPT;
	aes_dev.bit = HOSAL_AES_128_BIT;
	aes_dev.in_ptr = in;
	aes_dev.out_ptr = m_encryption_text;
	aes_dev.key_ptr = key;
	aes_dev.iv = iv;
	
	hosal_crypto_aes_ctr_operation(&aes_dev);
	
    if (0 == memcmp((char *) m_encryption_text, (char *) expected_cipher, 36) )
    {
        printf("AES_128_CTR encryption: SUCCESS!\r\n");
    }
    else
    {
        printf("AES_128_CTR encryption: FAILURE!\r\n");
    }

    /*Remark: CTR use AES Encryption only!!*/
	aes_dev.crypto_operation = HOSAL_AES_CRYPTO_DECRYPT;
	aes_dev.bit = HOSAL_AES_128_BIT;
	aes_dev.in_ptr = m_encryption_text;
	aes_dev.out_ptr = m_decryption_text;
	aes_dev.key_ptr = key;
	aes_dev.iv = iv;
	hosal_crypto_aes_ctr_operation(&aes_dev);

    if (0 == memcmp((char *) m_decryption_text, (char *) in, 36) )
    {
        printf("AES_128_CTR decryption: SUCCESS!\r\n");
    }
    else
    {
        printf("AES_128_CTR decryption: FAILURE!\r\n");
    }

    printf("\r\n\r\n");
    printf("hosal crypto aes 128 ctr finish!\r\n");
    while (1);
}
