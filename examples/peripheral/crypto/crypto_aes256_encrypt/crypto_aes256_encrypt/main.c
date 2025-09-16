#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes.h"
#include "uart_stdio.h"


    uint8_t key[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    /*cleartext buffer is data for encryption */
    uint8_t cleartext_buf[]  =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    /*this is expected encryption buffer data*/
    uint8_t expect_out_buf[] =
    {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
        0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    uint8_t hw_out_buf[16];

int main(void) {


    hosal_aes_dev_t aes_dev;

    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto aes 256 encrypt demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_aes_init();
    
	
	aes_dev.crypto_operation = HOSAL_AES_CRYPTO_ENCRYPT;
	aes_dev.bit = HOSAL_AES_256_BIT;
	aes_dev.in_ptr =  cleartext_buf;
	aes_dev.out_ptr = hw_out_buf;
	aes_dev.key_ptr = (uint8_t*)key;
	
	hosal_crypto_aes_operation(&aes_dev);	
	
    if (memcmp((char *) expect_out_buf, (char *) hw_out_buf, 16) == 0)
    {
        printf("AES_256_test hardware encryption  ECB: SUCCESS!\n");
    }
    else
    {
        printf("AES_256_test hardware encryption  ECB: FAILURE!\n");
    }

    printf("\r\n\r\n");
    printf("hosal crypto aes 256 decrypt finish!\r\n");

    while (1);
}
