#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_crypto_aes_ccm.h"
#include "uart_stdio.h"


#define NUM_NIST_KEYS 16
#define NONCE_LEN 13
#define HEADER_LEN 8
#define HEADER_LEN12  12

#define M_LEN8 8
#define M_LEN10 10

#define DATA_BUF_LEN19 19
#define DATA_BUF_LEN23 23
#define DATA_BUF_LEN24 24
#define DATA_BUF_LEN25 25

#define EXPECTED_BUF_LEN31 31
#define EXPECTED_BUF_LEN32 32
#define EXPECTED_BUF_LEN33 33
#define EXPECTED_BUF_LEN34 34
#define EXPECTED_BUF_LEN35 35
#define EXPECTED_BUF_LEN39 39
#define EXPECTED_BUF_LEN40 40
#define EXPECTED_BUF_LEN41 41
#define EXPECTED_BUF_LEN42 42
#define EXPECTED_BUF_LEN43 43
#define EXPECTED_BUF_LEN44 44
#define EXPECTED_BUF_LEN45 45
#define EXPECTED_BUF_LEN46 46

void aes_ccm_vector_7(void) {

    uint32_t   outbuf_length, status;
    uint8_t    outbuf[64], decrypt_buf[64];

    hosal_aes_ccm_dev_t ccm_package;

    /* RFC 3610 test vector #7  */
    uint8_t key[NUM_NIST_KEYS] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    uint8_t nonce[NONCE_LEN] = {
        0x00, 0x00, 0x00, 0x09, 0x08, 0x07, 0x06, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    uint8_t hdr[HEADER_LEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    uint8_t data[DATA_BUF_LEN23] = {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };

    /*expeceted is the result of test pattern after CCM*/
    uint8_t expected[EXPECTED_BUF_LEN41] = {
        0x00, 0x01, 0x02, 0x03,  0x04, 0x05, 0x06, 0x07,
        0x01, 0x35, 0xD1, 0xB2,  0xC9, 0x5F, 0x41, 0xD5,
        0xD1, 0xD4, 0xFE, 0xC1,  0x85, 0xD1, 0x66, 0xB8,
        0x09, 0x4E, 0x99, 0x9D,  0xFE, 0xD9, 0x6C, 0x04,
        0x8C, 0x56, 0x60, 0x2C,  0x97, 0xAC, 0xBB, 0x74,
        0x90
    };

    ccm_package.bit = HOSAL_AES_128_BIT;
    ccm_package.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    ccm_package.key_ptr = key;

    ccm_package.nonce = nonce;
    ccm_package.hdr =  hdr;
    ccm_package.hdr_len = sizeof(hdr);
    ccm_package.data =  data;
    ccm_package.data_len = sizeof(data);
    ccm_package.mlen = M_LEN10;
    ccm_package.out_buf =  outbuf;
    outbuf_length = sizeof(outbuf);
    ccm_package.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_package);

    if (status != STATUS_SUCCESS) {
        printf("\n Please Check input ccm config setting. \r\n");
    } else {
        /*check result*/

        if ((outbuf_length != sizeof(expected)) ||
            (memcmp(expected, outbuf, sizeof(expected)) != 0)) {
            printf("\nccm_encryption produced wrong data !! \r\n");
        } else {
            printf("\nccm_encryption test vector #7 correct !\r\n");
        }

    }

    /*decryption and verification*/
    ccm_package.bit = HOSAL_AES_128_BIT;
    ccm_package.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    ccm_package.key_ptr = key;
    ccm_package.payload_buf = outbuf;
    ccm_package.nonce = nonce;
    ccm_package.hdr_len = HEADER_LEN;
    ccm_package.data_len = DATA_BUF_LEN23;
    ccm_package.mlen = M_LEN10;
    ccm_package.out_buf =  decrypt_buf;
    outbuf_length = sizeof (decrypt_buf);
    ccm_package.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_package);

    if (status == STATUS_SUCCESS) {

        if ((outbuf_length != DATA_BUF_LEN23) ||
            (memcmp(data, decrypt_buf, DATA_BUF_LEN23) != 0)) {
            printf("\nccm_decryption for test vector #7 error ?1 \r\n");
        } else {
            printf("\nccm_decryption and verification for test vector #7 correct !\r\n");
        }

    } else {
        printf("\nccm_dncryption wrong ?! \r\n");
    }
}

void aes_ccm_vector_4(void) {

    uint32_t   outbuf_length, status;
    uint8_t    outbuf[64], decrypt_buf[64];

    hosal_aes_ccm_dev_t ccm_input_packet;

    /* RFC 3610 test vector #3  */
    uint8_t key[NUM_NIST_KEYS] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    uint8_t nonce[NONCE_LEN] = {
        0x00, 0x00, 0x00, 0x06, 0x05, 0x04, 0x03, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    uint8_t hdr[HEADER_LEN12] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B
    };
    uint8_t data[DATA_BUF_LEN19] = {
        0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e
    };
    uint8_t expected[EXPECTED_BUF_LEN39] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0xA2, 0x8C, 0x68, 0x65,
        0x93, 0x9A, 0x9A, 0x79, 0xFA, 0xAA, 0x5C, 0x4C,
        0x2A, 0x9D, 0x4A, 0x91, 0xCD, 0xAC, 0x8C, 0x96,
        0xC8, 0x61, 0xB9, 0xC9, 0xE6, 0x1E, 0xF1
    };
    //------------------ AES ------------------
    ccm_input_packet.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    ccm_input_packet.bit = HOSAL_AES_128_BIT;
    ccm_input_packet.key_ptr = key;

    ccm_input_packet.nonce = nonce;
    ccm_input_packet.hdr =  hdr;
    ccm_input_packet.hdr_len = sizeof(hdr);
    ccm_input_packet.data =  data;
    ccm_input_packet.data_len = sizeof(data);
    ccm_input_packet.mlen = M_LEN8;
    ccm_input_packet.out_buf =  outbuf;
    outbuf_length = sizeof(outbuf);
    ccm_input_packet.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_input_packet);

    if (status != STATUS_SUCCESS) {
        printf("\n Please Check input ccm config setting. \r\n");
    } else {
        /*check result*/
        if ((outbuf_length != sizeof(expected)) ||
            (memcmp(expected, outbuf, sizeof(expected)) != 0)) {
            printf("\nccm_encryption produced wrong data !! \r\n");
        } else {
            printf("\nccm_encryption test vector #3 correct !\r\n");
        }

    }

    /*decryption and verification*/

    ccm_input_packet.bit = HOSAL_AES_128_BIT;
    ccm_input_packet.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    ccm_input_packet.key_ptr = key;
    ccm_input_packet.payload_buf = outbuf;
    ccm_input_packet.nonce = nonce;
    ccm_input_packet.hdr_len = HEADER_LEN12;
    ccm_input_packet.data_len = DATA_BUF_LEN19;
    ccm_input_packet.mlen = M_LEN8;
    ccm_input_packet.out_buf =  decrypt_buf;
    outbuf_length = sizeof (decrypt_buf);
    ccm_input_packet.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_input_packet);

    if (status == STATUS_SUCCESS) {

        if ((outbuf_length != DATA_BUF_LEN19) ||
            (memcmp(data, decrypt_buf, DATA_BUF_LEN19) != 0)) {
            printf("\nccm_decryption for test vector #4 error ?1 \r\n");
        } else {
            printf("\nccm_decryption and verification for test vector #4 correct !\r\n");
        }

    } else {
        printf("\nccm_dncryption wrong ?!\r\n");
    }
}


void aes_ccm_vector_3(void) {

    uint32_t   outbuf_length, status;
    uint8_t    outbuf[64], decrypt_buf[64];

    hosal_aes_ccm_dev_t ccm_input_packet;

    /* RFC 3610 test vector #3  */
    uint8_t key[NUM_NIST_KEYS] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    uint8_t nonce[NONCE_LEN] = {
        0x00, 0x00, 0x00, 0x05, 0x04, 0x03, 0x02, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    uint8_t hdr[HEADER_LEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    uint8_t data[DATA_BUF_LEN25] = {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20
    };
    uint8_t expected[EXPECTED_BUF_LEN41] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x51, 0xb1, 0xe5, 0xf4, 0x4a, 0x19, 0x7d, 0x1d,
        0xa4, 0x6b, 0x0f, 0x8e, 0x2d, 0x28, 0x2a, 0xe8,
        0x71, 0xe8, 0x38, 0xbb, 0x64, 0xda, 0x85, 0x96,
        0x57, 0x4a, 0xda, 0xa7, 0x6f, 0xbd, 0x9f, 0xb0,
        0xc5
    };


    ccm_input_packet.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    ccm_input_packet.bit = HOSAL_AES_128_BIT;
    ccm_input_packet.key_ptr = key;

    ccm_input_packet.nonce = nonce;
    ccm_input_packet.hdr =  hdr;
    ccm_input_packet.hdr_len = sizeof(hdr);
    ccm_input_packet.data =  data;
    ccm_input_packet.data_len = sizeof(data);
    ccm_input_packet.mlen = M_LEN8;
    ccm_input_packet.out_buf =  outbuf;
    outbuf_length = sizeof(outbuf);
    ccm_input_packet.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_input_packet);

    if (status != STATUS_SUCCESS) {
        printf("\n Please Check input ccm config setting. \r\n");
    } else {
        /*check result*/
        if ((outbuf_length != sizeof(expected)) ||
            (memcmp(expected, outbuf, sizeof(expected)) != 0)) {
            printf("\nccm_encryption produced wrong data !! \r\n");
        } else {
            printf("\nccm_encryption test vector #3 correct !\r\n");
        }

    }

    /*decryption and verification*/
    ccm_input_packet.bit = HOSAL_AES_128_BIT;
    ccm_input_packet.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    ccm_input_packet.key_ptr = key;
    ccm_input_packet.payload_buf = outbuf;
    ccm_input_packet.nonce = nonce;
    ccm_input_packet.hdr_len = HEADER_LEN;
    ccm_input_packet.data_len = DATA_BUF_LEN25;
    ccm_input_packet.mlen = M_LEN8;
    ccm_input_packet.out_buf =  decrypt_buf;
    outbuf_length = sizeof (decrypt_buf);
    ccm_input_packet.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_input_packet);

    if (status == STATUS_SUCCESS) {

        if ((outbuf_length != DATA_BUF_LEN25) ||
            (memcmp(data, decrypt_buf, DATA_BUF_LEN25) != 0)) {
            printf("\nccm_decryption for test vector #3 error ?1\r\n");
        } else {
            printf("\nccm_decryption and verification for test vector #3 correct !\r\n");
        }

    } else {
        printf("\nccm_dncryption wrong ?! \r\n");
    }

}

void aes_ccm_vector_2(void) {


    /* RFC 3610 test vector #1 */
    uint8_t key[NUM_NIST_KEYS] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    uint8_t nonce[NONCE_LEN] = {
        0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5

    };
    uint8_t hdr[HEADER_LEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    uint8_t data[DATA_BUF_LEN24] = {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    uint8_t expected[EXPECTED_BUF_LEN40] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x72, 0xc9, 0x1a, 0x36, 0xe1, 0x35, 0xf8, 0xcf,
        0x29, 0x1c, 0xa8, 0x94, 0x08, 0x5c, 0x87, 0xe3,
        0xcc, 0x15, 0xc4, 0x39, 0xc9, 0xe4, 0x3a, 0x3b,
        0xa0, 0x91, 0xd5, 0x6e, 0x10, 0x40, 0x09, 0x16
    };


    uint32_t   outbuf_length, status;
    uint8_t    outbuf[64], decrypt_buf[64];
    hosal_aes_ccm_dev_t ccm_package;

    ccm_package.bit = HOSAL_AES_128_BIT;
    ccm_package.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    ccm_package.key_ptr = key;

    ccm_package.nonce = nonce;
    ccm_package.hdr =  hdr;
    ccm_package.hdr_len = sizeof(hdr);
    ccm_package.data =  data;
    ccm_package.data_len = sizeof(data);
    ccm_package.mlen = M_LEN8;
    ccm_package.out_buf =  outbuf;
    outbuf_length = sizeof(outbuf);
    ccm_package.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_package);

    if (status != STATUS_SUCCESS) {
        printf("\n Please Check input ccm config setting. \r\n");
    } else {
        /*check result*/

        if ((outbuf_length != sizeof(expected)) ||
            (memcmp(expected, outbuf, sizeof(expected)) != 0)) {
            printf("\nccm_encryption produced wrong data !! \r\n");
        } else {
            printf("\nccm_encryption test vector #2 correct !\r\n");
        }

    }


    /*decryption and verification*/
    ccm_package.bit = HOSAL_AES_128_BIT;
    ccm_package.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    ccm_package.key_ptr = key;
    ccm_package.payload_buf = outbuf;
    ccm_package.nonce = nonce;
    ccm_package.hdr_len = HEADER_LEN;
    ccm_package.data_len = DATA_BUF_LEN24;
    ccm_package.mlen = M_LEN8;
    ccm_package.out_buf =  decrypt_buf;
    outbuf_length = sizeof (decrypt_buf);
    ccm_package.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_package);

    if (status == STATUS_SUCCESS) {

        if ((outbuf_length != DATA_BUF_LEN24) ||
            (memcmp(data, decrypt_buf, DATA_BUF_LEN24) != 0)) {
            printf("\nccm_decryption for test vector #2 error ?1 \r\n");
        } else {
            printf("\nccm_decryption and verification for test vector #2 correct !\r\n");
        }

    } else {
        printf("\nccm_dncryption wrong ?! \r\n");
    }


}
void aes_ccm_vector_1(void) {
    uint32_t   outbuf_length, status;
    uint8_t    outbuf[64], decrypt_buf[64];
    hosal_aes_ccm_dev_t ccm_input_packet;

    /* RFC 3610 test vector #1 */
    uint8_t key[NUM_NIST_KEYS] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };

    uint8_t nonce[NONCE_LEN] = {
        0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    uint8_t hdr[HEADER_LEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    uint8_t data[DATA_BUF_LEN23] = {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };
    uint8_t expected[EXPECTED_BUF_LEN39] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
        0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
        0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84, 0x17,
        0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0
    };

    ccm_input_packet.crypto_operation = HOSAL_AES_CCM_ENCRYPT;
    ccm_input_packet.bit = HOSAL_AES_128_BIT;
    ccm_input_packet.key_ptr = key;

    ccm_input_packet.nonce = nonce;
    ccm_input_packet.hdr =  hdr;
    ccm_input_packet.hdr_len = HEADER_LEN;
    ccm_input_packet.data =  data;
    ccm_input_packet.data_len = DATA_BUF_LEN23;
    ccm_input_packet.mlen = M_LEN8;
    ccm_input_packet.out_buf =  outbuf;
    outbuf_length = sizeof(outbuf);
    ccm_input_packet.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_input_packet);

    if (status != STATUS_SUCCESS) {
        printf("\n Please Check input ccm config setting. \r\n");
    } else {
        /* check result*/
        /* after function aes_ccm_encryption,  outbuf_length will be the real data length
         * of ouput buffer in bytes.
         */
        if ((outbuf_length != EXPECTED_BUF_LEN39) ||
            (memcmp(expected, outbuf, EXPECTED_BUF_LEN39) != 0)) {
            printf("\nccm_encryption produced wrong data !! \r\n");
        } else {
            printf("\nccm_encryption test vector #1 correct !\r\n");
        }

    }

    ccm_input_packet.bit = HOSAL_AES_128_BIT;
    ccm_input_packet.crypto_operation = HOSAL_AES_CCM_DECRYPT;
    ccm_input_packet.key_ptr = key;
    ccm_input_packet.payload_buf = outbuf;
    ccm_input_packet.nonce = nonce;
    ccm_input_packet.hdr_len = HEADER_LEN;
    ccm_input_packet.data_len = DATA_BUF_LEN23;
    ccm_input_packet.mlen = M_LEN8;
    ccm_input_packet.out_buf =  decrypt_buf;
    outbuf_length = sizeof (decrypt_buf);
    ccm_input_packet.out_buf_len = &outbuf_length;

    status = hosal_crypto_aes_ccm_operation(&ccm_input_packet);

    if (status == STATUS_SUCCESS) {

        if ((outbuf_length != DATA_BUF_LEN23) ||
            (memcmp(data, decrypt_buf, DATA_BUF_LEN23) != 0)) {
            printf("\nccm_decryption for test vector #1 error ?1 \r\n");
        } else {
            printf("\nccm_decryption and verification for test vector #1 correct !\r\n");
        }

    } else {
        printf("\nccm_dncryption wrong ?! \r\n");
    }
}

//------------------ AES ------------------


int main(void) {


    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto CCM vector demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_crypto_aes_ccm_init();

    aes_ccm_vector_1();
    aes_ccm_vector_2();
    aes_ccm_vector_3();
    aes_ccm_vector_4();
    aes_ccm_vector_7();

    printf("\r\n\r\n");
    printf("hosal crypto aes ccm finish!\r\n");

    while (1);
}