/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sysctrl.h"
#include "trng.h"
#include "hosal_crypto_ctr_drbg.h"
#include "uart_stdio.h"



#define  NUMBER_OF_GENERATES                  2


/*This entropy is for self test. To compare DRBG800-90A test vector*/
uint32_t test_offset;

uint32_t ctr_drbg_self_test_entropy(void *data, uint8_t *buf, uint32_t len)
{
    uint8_t *p = data;

    memcpy( buf, p + test_offset, len );
    test_offset += len;

    return ( 0 );
}

/*
[AES-128 use df]
[PredictionResistance = True]
[EntropyInputLen = 128]
[NonceLen = 64]
[PersonalizationStringLen = 0]
[AdditionalInputLen = 0]
[ReturnedBitsLen = 512]

COUNT = 0
EntropyInput = 5d4041942bcf68864a4997d8171f1f9f
Nonce = d4f1f4ae08bcb3e1
PersonalizationString =
** INSTANTIATE:
    Key = 627b45e46f8d6e5bb66927d5daa6878e
    V   = 159cb084f8c8c84efbde0a098872bb69

AdditionalInput =
EntropyInputPR = ef55a769b7eaf03fe082029bb32a2b9d
** GENERATE (FIRST CALL):
    Key = a18e8cbe91fbd1e4f8219e5c66e692a3
    V   = 09d2be3972a1bc31198579691577b7a9

AdditionalInput =
EntropyInputPR = 8239e865c0a42e14b964b9c09de85a20
ReturnedBits = 4155320287eedcf7d484c2c2a1e2eb64b9c9ce77c87202a1ae1616c7a5cfd1c687c7a0bfcc85bda48fdd4629fd330c22d0a76076f88fc7cd04037ee06b7af602
** GENERATE (SECOND CALL):
    Key = 2cb6db845423d6c3f631441cb4f9ffb1
    V   = ea688052e7167be04e39846f97ef4f82
*/

const uint8_t test1_entropy_source_pr[48] =
{
    0x5d, 0x40, 0x41, 0x94, 0x2b, 0xcf, 0x68, 0x86, 0x4a, 0x49, 0x97, 0xd8, 0x17, 0x1f, 0x1f, 0x9f,
    0xef, 0x55, 0xa7, 0x69, 0xb7, 0xea, 0xf0, 0x3f, 0xe0, 0x82, 0x02, 0x9b, 0xb3, 0x2a, 0x2b, 0x9d,
    0x82, 0x39, 0xe8, 0x65, 0xc0, 0xa4, 0x2e, 0x14, 0xb9, 0x64, 0xb9, 0xc0, 0x9d, 0xe8, 0x5a, 0x20
};

const uint8_t test1_nonce_pers_pr[8] =
{
    0xd4, 0xf1, 0xf4, 0xae, 0x08, 0xbc, 0xb3, 0xe1
};

const uint8_t test1_result_pr[64] =
{
    0x41, 0x55, 0x32, 0x02, 0x87, 0xee, 0xdc, 0xf7, 0xd4, 0x84, 0xc2, 0xc2, 0xa1, 0xe2, 0xeb, 0x64,
    0xb9, 0xc9, 0xce, 0x77, 0xc8, 0x72, 0x02, 0xa1, 0xae, 0x16, 0x16, 0xc7, 0xa5, 0xcf, 0xd1, 0xc6,
    0x87, 0xc7, 0xa0, 0xbf, 0xcc, 0x85, 0xbd, 0xa4, 0x8f, 0xdd, 0x46, 0x29, 0xfd, 0x33, 0x0c, 0x22,
    0xd0, 0xa7, 0x60, 0x76, 0xf8, 0x8f, 0xc7, 0xcd, 0x04, 0x03, 0x7e, 0xe0, 0x6b, 0x7a, 0xf6, 0x02
};

void drbg_test_vector1(void)
{
    ctr_drbg_context ctx;
    unsigned char buf[64];
	int stauts;
    test_offset = 0;

	hosal_ctr_drbg_t ctr_drbg;
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_INI;
	ctr_drbg.f_entropy = ctr_drbg_self_test_entropy;
	ctr_drbg.p_entropy = (void *)test1_entropy_source_pr;
	ctr_drbg.custom = (uint8_t*)test1_nonce_pers_pr;
	ctr_drbg.len =8;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
	
    if ( stauts != STATUS_SUCCESS )
    {
        goto error_1;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_PR_ON;
	hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
    //ctr_drbg_set_prediction_resistance( &ctx, CTR_DRBG_PR_ON );

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( stauts != 0 )
    {
        goto error_1;
    }
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);

    if ( stauts != 0 )
    {
        goto error_1;
    }	

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
		
    if ( stauts != 0 )
    {
        goto error_1;
    }
	

    printf("drbg_test_vector1 success \r\n");
    return;

error_1:

    printf( "drbg_test_vector1 failed\r\n");
    while (1);

}


/**
[AES-128 use df]
[PredictionResistance = True]
[EntropyInputLen = 128]
[NonceLen = 64]
[PersonalizationStringLen = 0]
[AdditionalInputLen = 128]
[ReturnedBitsLen = 512]

COUNT = 1
EntropyInput = 91cf7f4f5a2dd7519bf910f2168b7019
Nonce = ed978aeff489993f
PersonalizationString =
** INSTANTIATE:
    Key = db6ff8f247272fed6f530210bb163887
    V   = 33e9a675bcc532d81df0b4d18a69b711

AdditionalInput = d9bd7f0e621e15a6439d9b94c42985ca
EntropyInputPR = 5ff796698e198d68abac26ea2fa321e6
** GENERATE (FIRST CALL):
    Key = 9bdea5905cade26b8274b7c3d592f61c
    V   = 42efc49446049a4fa46129ef1e0081ba

AdditionalInput = 036b4f6a118b4525fcdfe7e86c3bd0ef
EntropyInputPR = c197aa6f4022c290a407a6a53781150e
ReturnedBits = c4ee4206825726471a592b9bb8b66115ebdf014776c5e4170ad960d6976bb35409aeed2ef2d1d653a1b6e2bf28c7b2f2f73f3348488b4c46dc12dd0b5a906e42
** GENERATE (SECOND CALL):
    Key = 21a6f5e7334c0403a5b181e684e1d639
    V   = 3a619f96a1b1136a310366a09b975af1
*/

const uint8_t test2_entropy_source_pr[48] =
{
    0x91, 0xcf, 0x7f, 0x4f, 0x5a, 0x2d, 0xd7, 0x51, 0x9b, 0xf9, 0x10, 0xf2, 0x16, 0x8b, 0x70, 0x19,
    0x5f, 0xf7, 0x96, 0x69, 0x8e, 0x19, 0x8d, 0x68, 0xab, 0xac, 0x26, 0xea, 0x2f, 0xa3, 0x21, 0xe6,
    0xc1, 0x97, 0xaa, 0x6f, 0x40, 0x22, 0xc2, 0x90, 0xa4, 0x07, 0xa6, 0xa5, 0x37, 0x81, 0x15, 0x0e
};

const uint8_t test2_nonce_pers_pr[8] =
{
    0xed, 0x97, 0x8a, 0xef, 0xf4, 0x89, 0x99, 0x3f
};

const uint8_t test2_add_pers_pr1[16] =
{
    0xd9, 0xbd, 0x7f, 0x0e, 0x62, 0x1e, 0x15, 0xa6, 0x43, 0x9d, 0x9b, 0x94, 0xc4, 0x29, 0x85, 0xca
};

const uint8_t test2_add_pers_pr2[16] =
{
    0x03, 0x6b, 0x4f, 0x6a, 0x11, 0x8b, 0x45, 0x25, 0xfc, 0xdf, 0xe7, 0xe8, 0x6c, 0x3b, 0xd0, 0xef
};

const uint8_t test2_result_pr[64] =
{
    0xc4, 0xee, 0x42, 0x06, 0x82, 0x57, 0x26, 0x47, 0x1a, 0x59, 0x2b, 0x9b, 0xb8, 0xb6, 0x61, 0x15,
    0xeb, 0xdf, 0x01, 0x47, 0x76, 0xc5, 0xe4, 0x17, 0x0a, 0xd9, 0x60, 0xd6, 0x97, 0x6b, 0xb3, 0x54,
    0x09, 0xae, 0xed, 0x2e, 0xf2, 0xd1, 0xd6, 0x53, 0xa1, 0xb6, 0xe2, 0xbf, 0x28, 0xc7, 0xb2, 0xf2,
    0xf7, 0x3f, 0x33, 0x48, 0x48, 0x8b, 0x4c, 0x46, 0xdc, 0x12, 0xdd, 0x0b, 0x5a, 0x90, 0x6e, 0x42
};


void drbg_test_vector2(void)
{
    ctr_drbg_context ctx;
    unsigned char buf[64];
	int stauts;
    test_offset = 0;

	hosal_ctr_drbg_t ctr_drbg;
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_INI;
	ctr_drbg.f_entropy = ctr_drbg_self_test_entropy;
	ctr_drbg.p_entropy = (void *)test2_entropy_source_pr;
	ctr_drbg.custom = (uint8_t*)test2_nonce_pers_pr;
	ctr_drbg.len =8;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
	
    if ( stauts != STATUS_SUCCESS )
    {
        goto error_2;
    }
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_PR_ON;
	hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
    //ctr_drbg_set_prediction_resistance( &ctx, CTR_DRBG_PR_ON );

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM_ADD;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	ctr_drbg.additional = (void *)test2_add_pers_pr1;
	ctr_drbg.add_len = 16;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	

    if ( stauts != 0 )
    {
        goto error_2;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM_ADD;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	ctr_drbg.additional = (void *)test2_add_pers_pr2;
	ctr_drbg.add_len = 16;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( stauts != 0 )
    {
        goto error_2;
    }

    if ( memcmp( buf, test2_result_pr, 64 ) != 0 )
    {
        goto error_2;
    }

    printf("drbg_test_vector2 success \r\n");
    return;

error_2:

    printf( "drbg_test_vector2 failed\r\n");
    while (1);

}

/**
[AES-128 use df]
[PredictionResistance = False]
[EntropyInputLen = 128]
[NonceLen = 64]
[PersonalizationStringLen = 0]
[AdditionalInputLen = 0]
[ReturnedBitsLen = 512]

COUNT = 2
EntropyInput = 7a3b24c17b87513675c431519e771ce6
Nonce = abe47800414d25dd
PersonalizationString =
** INSTANTIATE:
    Key = ffa777d4673cee792528285e3a393f90
    V   = 9b6d5cd1777d53e9f8e5114faad3f5c6

EntropyInputReseed = b6ffefc408e41f77e2cad479a669274d
AdditionalInputReseed =
** RESEED:
    Key = 6e8b5e614c4f35eabf27ee44fd8f571e
    V   = aceef88f3de4c1199ac6a9888dbe0ca2

AdditionalInput =
** GENERATE (FIRST CALL):
    Key = 7f609f698f568d444eb784c6c5e042e2
    V   = b21772bc078fb91c784a2191139f2410

AdditionalInput =
ReturnedBits = cdc469c1547903b9fee583409d411e0ac763a00cd687d4f8c811e9c74dc3b78b27b66fe66a249b4178bd3bd08008ea258c5a908d2ea737158d163d1f34f93ea3
** GENERATE (SECOND CALL):
    Key = 6a4960bb46644f98c0e8d2b5d217f640
    V   = 512b3db186d27c6a97324ef87796abed
*/
const uint8_t test3_entropy_source_pr[32] =
{
    0x7a, 0x3b, 0x24, 0xc1, 0x7b, 0x87, 0x51, 0x36, 0x75, 0xc4, 0x31, 0x51, 0x9e, 0x77, 0x1c, 0xe6,
    0xb6, 0xff, 0xef, 0xc4, 0x08, 0xe4, 0x1f, 0x77, 0xe2, 0xca, 0xd4, 0x79, 0xa6, 0x69, 0x27, 0x4d
};

const uint8_t test3_nonce_pers_pr[8] =
{
    0xab, 0xe4, 0x78, 0x00, 0x41, 0x4d, 0x25, 0xdd
};

const uint8_t test3_result_pr[64] =
{
    0xcd, 0xc4, 0x69, 0xc1, 0x54, 0x79, 0x03, 0xb9, 0xfe, 0xe5, 0x83, 0x40, 0x9d, 0x41, 0x1e, 0x0a,
    0xc7, 0x63, 0xa0, 0x0c, 0xd6, 0x87, 0xd4, 0xf8, 0xc8, 0x11, 0xe9, 0xc7, 0x4d, 0xc3, 0xb7, 0x8b,
    0x27, 0xb6, 0x6f, 0xe6, 0x6a, 0x24, 0x9b, 0x41, 0x78, 0xbd, 0x3b, 0xd0, 0x80, 0x08, 0xea, 0x25,
    0x8c, 0x5a, 0x90, 0x8d, 0x2e, 0xa7, 0x37, 0x15, 0x8d, 0x16, 0x3d, 0x1f, 0x34, 0xf9, 0x3e, 0xa3
};

void drbg_test_vector3(void)
{
    ctr_drbg_context ctx;
    unsigned char buf[64];
	int status;
    test_offset = 0;

	hosal_ctr_drbg_t ctr_drbg;
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_INI;
	ctr_drbg.f_entropy = ctr_drbg_self_test_entropy;
	ctr_drbg.p_entropy = (void *)test3_entropy_source_pr;
	ctr_drbg.custom = (uint8_t*)test3_nonce_pers_pr;
	ctr_drbg.len =8;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
    if ( status != STATUS_SUCCESS )
    {
        goto error_3;
    }

    //ctr_drbg_reseed(&ctx, NULL, 0);

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RESEED;
	ctr_drbg.additional = NULL;
	ctr_drbg.add_len = 0;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( status != 0 )
    {
        goto error_3;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( status != 0 )
    {
        goto error_3;
    }

    if ( memcmp( buf, test3_result_pr, 64 ) != 0 )
    {
        goto error_3;
    }

    printf("drbg_test_vector3 success \r\n");
    return;

error_3:

    printf( "drbg_test_vector3 failed\r\n");
    while (1);

}

/**
[AES-128 use df]
[PredictionResistance = False]
[EntropyInputLen = 128]
[NonceLen = 64]
[PersonalizationStringLen = 128]
[AdditionalInputLen = 128]
[ReturnedBitsLen = 512]

COUNT = 0
EntropyInput = e14ed7064a97814dd326b9a05bc44543
Nonce = 876240c1f7de3dba
PersonalizationString = 26ccf56848a048721d0aad87d6fc65f0
** INSTANTIATE:
    Key = c5f6207dddce79208d4c8630923fb9c1
    V   = 033b9f1df720c2677abbea63f5c425fa

EntropyInputReseed = 7ec4ac660fa0bbfa66ac3802e511901f
AdditionalInputReseed = 8835d28e7f85a4e95087bdd1bb7ad57e
** RESEED:
    Key = 9161985f966d7f675e783f39f39b3bc8
    V   = 373f52ce22f3690c351ad65ca0424303

AdditionalInput = 2a9bd50bbb20fefe24649f5f80eede66
** GENERATE (FIRST CALL):
    Key = 14e9256df61235c867233af47107739f
    V   = f60e23434fd8f63712553a23ce4ff918

AdditionalInput = f7ce3d5c6c381e56b25410c6909c1074
ReturnedBits = d2f3130d309bed1da65545b9d793e035fd2564303d1fdcfb6c7fee019500d9f5d434fab2d3c8d15e39a25f965aaa804c7141407e90c4a86a6c8d303ce83bfb34
** GENERATE (SECOND CALL):
    Key = e68df737b3c0edfb66a9e357121c85ae
    V   = d81e79df9064de25368697716c01b7fa
*/

const uint8_t test4_entropy_source_pr[32] =
{
    0xe1, 0x4e, 0xd7, 0x06, 0x4a, 0x97, 0x81, 0x4d, 0xd3, 0x26, 0xb9, 0xa0, 0x5b, 0xc4, 0x45, 0x43,
    0x7e, 0xc4, 0xac, 0x66, 0x0f, 0xa0, 0xbb, 0xfa, 0x66, 0xac, 0x38, 0x02, 0xe5, 0x11, 0x90, 0x1f
};

const uint8_t test4_nonce_pers_pr[24] =
{
    0x87, 0x62, 0x40, 0xc1, 0xf7, 0xde, 0x3d, 0xba,
    0x26, 0xcc, 0xf5, 0x68, 0x48, 0xa0, 0x48, 0x72,
    0x1d, 0x0a, 0xad, 0x87, 0xd6, 0xfc, 0x65, 0xf0
};


const uint8_t test4_add_pers_reseed[16] =
{
    0x88, 0x35, 0xd2, 0x8e, 0x7f, 0x85, 0xa4, 0xe9, 0x50, 0x87, 0xbd, 0xd1, 0xbb, 0x7a, 0xd5, 0x7e
};

const uint8_t test4_add_pers_pr1[16] =
{
    0x2a, 0x9b, 0xd5, 0x0b, 0xbb, 0x20, 0xfe, 0xfe, 0x24, 0x64, 0x9f, 0x5f, 0x80, 0xee, 0xde, 0x66
};

const uint8_t test4_add_pers_pr2[16] =
{
    0xf7, 0xce, 0x3d, 0x5c, 0x6c, 0x38, 0x1e, 0x56, 0xb2, 0x54, 0x10, 0xc6, 0x90, 0x9c, 0x10, 0x74
};


const uint8_t test4_result_pr[64] =
{
    0xd2, 0xf3, 0x13, 0x0d, 0x30, 0x9b, 0xed, 0x1d, 0xa6, 0x55, 0x45, 0xb9, 0xd7, 0x93, 0xe0, 0x35,
    0xfd, 0x25, 0x64, 0x30, 0x3d, 0x1f, 0xdc, 0xfb, 0x6c, 0x7f, 0xee, 0x01, 0x95, 0x00, 0xd9, 0xf5,
    0xd4, 0x34, 0xfa, 0xb2, 0xd3, 0xc8, 0xd1, 0x5e, 0x39, 0xa2, 0x5f, 0x96, 0x5a, 0xaa, 0x80, 0x4c,
    0x71, 0x41, 0x40, 0x7e, 0x90, 0xc4, 0xa8, 0x6a, 0x6c, 0x8d, 0x30, 0x3c, 0xe8, 0x3b, 0xfb, 0x34
};

void drbg_test_vector4(void)
{
    ctr_drbg_context ctx;
    unsigned char buf[64];

	int status;
    test_offset = 0;

	hosal_ctr_drbg_t ctr_drbg;
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_INI;
	ctr_drbg.f_entropy = ctr_drbg_self_test_entropy;
	ctr_drbg.p_entropy = (void *)test4_entropy_source_pr;
	ctr_drbg.custom = (uint8_t*)test4_nonce_pers_pr;
	ctr_drbg.len =24;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 

    if ( status != STATUS_SUCCESS )
    {
        goto error_4;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RESEED;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_reseed;
	ctr_drbg.add_len = 16;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 	
    //ctr_drbg_reseed(&ctx, test4_add_pers_reseed, 16);

	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM_ADD;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_pr1;
	ctr_drbg.add_len = 16;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
	
    if ( status != 0 )
    {
        goto error_4;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM_ADD;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_pr2;
	ctr_drbg.add_len = 16;
	status = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( status != 0 )
    {
        goto error_4;
    }

    if ( memcmp( buf, test4_result_pr, 64 ) != 0 )
    {
        goto error_4;
    }

    printf("drbg_test_vector4 success\r\n");
    return;

error_4:

    printf( "drbg_test_vector4 failed\r\n");
    while (1);

}

void lecp_print_hex_bytes(const char *name, const uint8_t *data, uint32_t size)
{
    uint32_t i = 0;

    printf("%s = 0x", name);

    while (i < size)
    {
        printf("%02X", data[i++]);
    }

    printf("\r\n");
}


void becp_print_hex_bytes(const char *name, const uint8_t *data, uint32_t size)
{
    uint32_t i = 0;

    printf("%s = 0x", name);

    while (i < size)
    {
        printf("%02X", data[(size - 1) - i]);
        i++;
    }

    printf("\r\n");
}

#define curve25519_in_bytes       32

void curve_c25519_test1( void)
{
    /*notice stack issue...*/

    static unsigned char alice_public_key[curve25519_in_bytes], alice_shared_key[curve25519_in_bytes];
    static unsigned char bruce_public_key[curve25519_in_bytes], bruce_shared_key[curve25519_in_bytes];

    /*this is rfc7748 example... Notice: RFC7748 test vector is little endian format*/

    static unsigned char alice_secret_key[curve25519_in_bytes] =
    {
        0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d,
        0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45,
        0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a,
        0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a

    };

    static unsigned char bruce_secret_key[curve25519_in_bytes] =
    {
        0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b,
        0x79, 0xe1, 0x7f, 0x8b, 0x83, 0x80, 0x0e, 0xe6,
        0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18, 0xb6, 0xfd,
        0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb

    };

    static unsigned char BasePoint[curve25519_in_bytes] = {0x09};

	hosal_crypto_curve25519_t curve25519;
	
 
    printf("\n-- curve25519 -- key exchange test -----------------------------\r\n");
    /* Step 1. init to load curve25519 firmware  */
    //curve_c25519_init();
	hosal_crypto_curve_c25519_init();
    /* Step 2. Alice and Bruce generate their own random secret keys */

    lecp_print_hex_bytes("Alice_secret_key", alice_secret_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_secret_key", bruce_secret_key, curve25519_in_bytes);


    /* Step 3. Alice and Bruce create public keys associated with their secret keys */
    /*         and exchange their public keys by some method... */

    /*Here we set blind_zr to NULL, so the curve25516_point_mul will use default blind_zr.*/

    //curve25519_point_mul(NULL, (uint32_t *)alice_public_key, (uint32_t *)alice_secret_key, (uint32_t *)BasePoint);
	//curve25519_point_mul(NULL, (uint32_t *)bruce_public_key, (uint32_t *)bruce_secret_key, (uint32_t *)BasePoint);

	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)alice_public_key;
	curve25519.secret_key = (uint32_t *)alice_secret_key;
	curve25519.base_point = (uint32_t *)BasePoint;
	hosal_crypto_curve25519_operation(&curve25519);
	
	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)bruce_public_key;
	curve25519.secret_key = (uint32_t *)bruce_secret_key;
	curve25519.base_point = (uint32_t *)BasePoint;
	hosal_crypto_curve25519_operation(&curve25519);
	
    lecp_print_hex_bytes("Alice_public_key", alice_public_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_public_key", bruce_public_key, curve25519_in_bytes);

    /* Step 4. Alice and Bruce create their shared key */

    //curve25519_point_mul( NULL, (uint32_t *)alice_shared_key,  (uint32_t *)alice_secret_key, (uint32_t *)bruce_public_key);
    //curve25519_point_mul( NULL, (uint32_t *)bruce_shared_key, (uint32_t *)bruce_secret_key, (uint32_t *)alice_public_key);

	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)alice_shared_key;
	curve25519.secret_key = (uint32_t *)alice_secret_key;
	curve25519.base_point = (uint32_t *)bruce_public_key;
	hosal_crypto_curve25519_operation(&curve25519);
	
	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)bruce_shared_key;
	curve25519.secret_key = (uint32_t *)bruce_secret_key;
	curve25519.base_point = (uint32_t *)alice_public_key;
	hosal_crypto_curve25519_operation(&curve25519);

    /*Now Alice and Bruce have share private key*/
    lecp_print_hex_bytes("Alice_shared", alice_shared_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_shared", bruce_shared_key, curve25519_in_bytes);

	curve25519.crypto_operation = HOSAL_CURVE_C25519_RELEASE;
	hosal_crypto_curve25519_operation(&curve25519);
    /*Release crypto accelator*/
    //curve25519_release();

    /* Alice and Bruce should end up with idetntical keys */
    if (memcmp(alice_shared_key, bruce_shared_key, curve25519_in_bytes) != 0)
    {
        printf("DH key exchange FAILED!!\r\n");
        while (1);
    }
    else
    {
        printf("Curve25519 DH key exchange Success!!\r\n");
    }

    return;
}

void curve_c25519_test2( uint8_t *alice_secret_key, uint8_t *bruce_secret_key)
{
    /*notice stack issue...*/

    unsigned char alice_public_key[curve25519_in_bytes], alice_shared_key[curve25519_in_bytes];
    unsigned char bruce_public_key[curve25519_in_bytes], bruce_shared_key[curve25519_in_bytes];


    unsigned char BasePoint[curve25519_in_bytes] = {0x09};

	hosal_crypto_curve25519_t curve25519;
    printf("\n-- curve25519 -- key exchange test -----------------------------\r\n");

    /* Step 1. init to load curve25519 firmware  */
    //curve_c25519_init();
	hosal_crypto_curve_c25519_init();

    /* Step 2. Alice and Bruce generate their own random secret keys */

    lecp_print_hex_bytes("Alice_secret_key", alice_secret_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_secret_key", bruce_secret_key, curve25519_in_bytes);


    /* Step 3. Alice and Bruce create public keys associated with their secret keys */
    /*         and exchange their public keys by some method... */

    /*Here we set blind_zr to NULL, so the curve25516_point_mul will use default blind_zr.*/

    //curve25519_point_mul(NULL, (uint32_t *)alice_public_key, (uint32_t *)alice_secret_key, (uint32_t *)BasePoint);
    //curve25519_point_mul(NULL, (uint32_t *)bruce_public_key, (uint32_t *)bruce_secret_key, (uint32_t *)BasePoint);

	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)alice_public_key;
	curve25519.secret_key = (uint32_t *)alice_secret_key;
	curve25519.base_point = (uint32_t *)BasePoint;
	hosal_crypto_curve25519_operation(&curve25519);
	
	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)bruce_public_key;
	curve25519.secret_key = (uint32_t *)bruce_secret_key;
	curve25519.base_point = (uint32_t *)BasePoint;
	hosal_crypto_curve25519_operation(&curve25519);
	
    lecp_print_hex_bytes("Alice_public_key", alice_public_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_public_key", bruce_public_key, curve25519_in_bytes);

    /* Step 4. Alice and Bruce create their shared key */

    //curve25519_point_mul( NULL, (uint32_t *)alice_shared_key,  (uint32_t *)alice_secret_key, (uint32_t *)bruce_public_key);
    //curve25519_point_mul( NULL, (uint32_t *)bruce_shared_key, (uint32_t *)bruce_secret_key, (uint32_t *)alice_public_key);

	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)alice_shared_key;
	curve25519.secret_key = (uint32_t *)alice_secret_key;
	curve25519.base_point = (uint32_t *)bruce_public_key;
	hosal_crypto_curve25519_operation(&curve25519);
	
	curve25519.crypto_operation = HOSAL_CURVE_C25519_MUL;
	curve25519.blind_zr = NULL;
	curve25519.public_key = (uint32_t *)bruce_shared_key;
	curve25519.secret_key = (uint32_t *)bruce_secret_key;
	curve25519.base_point = (uint32_t *)alice_public_key;
	hosal_crypto_curve25519_operation(&curve25519);
	
    /*Now Alice and Bruce have share private key*/
    lecp_print_hex_bytes("Alice_shared", alice_shared_key, curve25519_in_bytes);
    lecp_print_hex_bytes("Bruce_shared", bruce_shared_key, curve25519_in_bytes);

    /*Release crypto accelator*/
	curve25519.crypto_operation = HOSAL_CURVE_C25519_RELEASE;
	hosal_crypto_curve25519_operation(&curve25519);

    /* Alice and Bruce should end up with idetntical keys */
    if (memcmp(alice_shared_key, bruce_shared_key, curve25519_in_bytes) != 0)
    {
        printf("DH key exchange FAILED!!\r\n");;
        while (1);
    }
    else
    {
        printf("Curve25519 DH key exchange Success!!\r\n");
    }

    return;
}

void drbg_test_vector5(void)
{
    ctr_drbg_context ctx;
    unsigned char buf[64];

	int stauts;
    test_offset = 0;

	hosal_ctr_drbg_t ctr_drbg;
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_INI;
	ctr_drbg.f_entropy = ctr_drbg_self_test_entropy;
	ctr_drbg.p_entropy = (void *)test4_entropy_source_pr;
	ctr_drbg.custom = (uint8_t*)test4_nonce_pers_pr;
	ctr_drbg.len =24;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
	

    if ( stauts != STATUS_SUCCESS )
    {
        goto error_5;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RESEED;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_reseed;
	ctr_drbg.add_len = 16;
	hosal_crypto_ctr_drbg_operation(&ctr_drbg); 	
	
    //ctr_drbg_reseed(&ctx, test4_add_pers_reseed, 16);

    curve_c25519_test1();

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM_ADD;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_pr1;
	ctr_drbg.add_len = 16;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( stauts != 0 )
    {
        goto error_5;
    }

    curve_c25519_test2(buf, (buf + 32));

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM_ADD;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_pr2;
	ctr_drbg.add_len = 16;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( stauts != 0 )
    {
        goto error_5;
    }

    curve_c25519_test2(buf, (buf + 32));

    if ( memcmp( buf, test4_result_pr, 64 ) != 0 )
    {
        goto error_5;
    }

    printf("drbg_test_vector4 with C25519 success \r\n");
    return;

error_5:

    printf( "drbg_test_vector4 with C25519 failed\r\n");
    while (1);

}

/* Notice: if you want to use rt584 native TRNG
 * buf must be 4-bytes alignment.
 * and length must be 4*n bytes
 */
uint32_t random_trng(void *data, uint8_t *buf, uint32_t len)
{
    get_random_number((uint32_t *) buf, (len / 4));

    return STATUS_SUCCESS;
}

void drbg_test(void)
{
    ctr_drbg_context ctx;
    unsigned char buf[64];


	int stauts;
    test_offset = 0;

	hosal_ctr_drbg_t ctr_drbg;
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_INI;
	ctr_drbg.f_entropy = random_trng;
	ctr_drbg.p_entropy = NULL;
	ctr_drbg.custom = (uint8_t*)test4_nonce_pers_pr;
	ctr_drbg.len =24;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg); 
	
    /* This example using rt584 TRNG to generate entropy
     * and test4_nonce_pers_pr is nonce for each device ...
     */

    if ( stauts != STATUS_SUCCESS )
    {
        goto error_5;
    }

    //ctr_drbg_reseed(&ctx, test4_add_pers_reseed, 16);
	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RESEED;
	ctr_drbg.additional = (uint8_t*)test4_add_pers_reseed;
	ctr_drbg.add_len = 16;
	hosal_crypto_ctr_drbg_operation(&ctr_drbg); 

	
	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( stauts != 0 )
    {
        goto error_5;
    }

	ctr_drbg.crypto_operation = HOSAL_CTR_DRBG_RANDOM;
	ctr_drbg.output_ptr =buf;
	ctr_drbg.output_len = 64;
	stauts = hosal_crypto_ctr_drbg_operation(&ctr_drbg);
	
    if ( stauts != 0 )
    {
        goto error_5;
    }

    lecp_print_hex_bytes("ctr_drbg PRNG", buf, 64);

    printf("drbg_test success \r\n");
    return;

error_5:

    printf( "drbg_test_vector4 with C25519 failed\r\n");
    while (1);

}


int main(void)
{
    uart_stdio_init();
    
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal crypto aes 128 ctr drbg demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_ctr_drbg_init();

    printf("PR = TRUE \r\n");
    drbg_test_vector1();
    drbg_test_vector2();

    printf("PR = FALSE \r\n");

    drbg_test_vector3();
    drbg_test_vector4();

    /*this test function to show using ctr_drgb to generate
      PRNG for secure key that used in Curve25519 */
    drbg_test_vector5();

    drbg_test();

    printf("\r\n\r\n");
    printf("hosal crypto NIST ctr_drbg finish!\r\n");

    while (1);
}


