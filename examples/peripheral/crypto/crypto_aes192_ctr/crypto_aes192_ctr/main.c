#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes.h"
#include "uart_stdio.h"




uint8_t m_encryption_text[120];
uint8_t m_decryption_text[120];

uint8_t  in[36] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23
};


uint8_t  expected_cipher[36] = {
    0x96, 0x89, 0x3F, 0xC5, 0x5E, 0x5C, 0x72, 0x2F,
    0x54, 0x0B, 0x7D, 0xD1, 0xDD, 0xF7, 0xE7, 0x58,
    0xD2, 0x88, 0xBC, 0x95, 0xC6, 0x91, 0x65, 0x88,
    0x45, 0x36, 0xC8, 0x11, 0x66, 0x2F, 0x21, 0x88,
    0xAB, 0xEE, 0x09, 0x35,
};

uint8_t  key[24] = {
    0x02, 0xBF, 0x39, 0x1E, 0xE8, 0xEC, 0xB1, 0x59,
    0xB9, 0x59, 0x61, 0x7B, 0x09, 0x65, 0x27, 0x9B,
    0xF5, 0x9B, 0x60, 0xA7, 0x86, 0xD3, 0xE0, 0xFE
};

uint8_t  iv[16]  = {
    0x00, 0x07, 0xBD, 0xFD, 0x5C, 0xBD, 0x60, 0x27,
    0x8D, 0xCC, 0x09, 0x12, 0x00, 0x00, 0x00, 0x01
};

uint8_t  stream_block[16];
//------------------ ECC GF(2m) ------------------
int main(void) {
 

    hosal_aes_dev_t aes_dev;


    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto aes 192 ctr demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_aes_init();

    aes_dev.crypto_operation = HOSAL_AES_CRYPTO_ENCRYPT;
    aes_dev.bit = HOSAL_AES_192_BIT;
    aes_dev.in_ptr = in;
    aes_dev.out_ptr = m_encryption_text;
    aes_dev.key_ptr = key;
    aes_dev.iv = iv;

    hosal_crypto_aes_ctr_operation(&aes_dev);


    if (0 == memcmp((char*) m_encryption_text, (char*) expected_cipher, 36) ) {
        printf("AES_192_CTR_test encryption: SUCCESS!\r\n");
    } else {
        printf("AES_192_CTR_test encryption: FAILURE!\r\n");
    }

    /*Remark: CTR use AES Encryption only!!*/
    aes_dev.crypto_operation = HOSAL_AES_CRYPTO_DECRYPT;
    aes_dev.bit = HOSAL_AES_192_BIT;
    aes_dev.in_ptr = m_encryption_text;
    aes_dev.out_ptr = m_decryption_text;
    aes_dev.key_ptr = key;
    aes_dev.iv = iv;
    hosal_crypto_aes_ctr_operation(&aes_dev);


    if (0 == memcmp((char*) m_decryption_text, (char*) in, 36) ) {
        printf("AES_192_CTR_test decryption: SUCCESS!\r\n");
    } else {
        printf("AES_192_CTR_test decryption: FAILURE!\r\n");
    }

 
    printf("\r\n\r\n");
    printf("hosal crypto aes 192 ctr finish!\r\n");
    
    while (1);
}
