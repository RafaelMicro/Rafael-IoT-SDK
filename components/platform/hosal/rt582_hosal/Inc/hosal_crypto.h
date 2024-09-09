/** @file hosal_uart.h
 * @license
 * @description
 */

#ifndef HOSAL_CRYPTO_H
#define HOSAL_CRYPTO_H


#include "crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                  Constant Definition
//=============================================================================

#define HOSAL_ECC_CURVE_P256_INIT  			0
#define HOSAL_ECDA_CURVE_P256_VERIFY_INIT	1
#define HOSAL_ECC_CURVE_P192_INIT  			2
#define HOSAL_ECC_CURVE_B163_INIT  			3
#define HOSAL_CURVE_C25519_INIT  			4
//=============================================================================
//                  Macro Definition
//=============================================================================
/**
* \brief crypto command
*
* \details 
*        
*/

typedef  enum
{
	HOSAL_ECC_P256_SIGNATURE = 1,
	HOSAL_ECDA_P256_VERIFY = 2,
	HOSAL_ECDA_P256_MULTI = 3,
	HOSAL_ECDA_P192_MULTI = 4,
	HOSAL_ECDA_B163_MULTI = 5,
	HOSAL_CURVE_C25519=6,
	HOSAL_CURVE_C25519_RELEASE=7,
} hosal_crypto_ecc_operation_t;

typedef struct{
	Signature_P256*	signatrue;
	uint32_t *p_hash;
	uint32_t *p_key;
	uint32_t *p_k;
	ECPoint_P256* result;
	ECPoint_P256* base;
	hosal_crypto_ecc_operation_t crypto_operation;
}hosal_crypto_ecc_p256_t;

typedef struct{
    uint32_t *p_result_x;
	uint32_t *p_result_y;
    uint32_t *target_x; 
	uint32_t *target_y;
    uint32_t *target_k;
	hosal_crypto_ecc_operation_t crypto_operation;
}hosal_crypto_ecc_p192_t;

typedef struct{
    uint32_t *p_result_x;
	uint32_t *p_result_y;
    uint32_t *target_x; 
	uint32_t *target_y;
    uint32_t *target_k;
	hosal_crypto_ecc_operation_t crypto_operation;
}hosal_crypto_ecc_b163_t;


typedef struct{

    uint32_t *blind_zr;
	uint32_t *public_key;
    uint32_t *secret_key; 
	uint32_t *base_point;
	hosal_crypto_ecc_operation_t crypto_operation;
}hosal_crypto_curve25519_t;

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int hosal_crypto_ecc_init(int ctl);

int hosal_crypto_curve_c25519_init(int ctl);
	
int hosal_crypot_ecc_p256(hosal_crypto_ecc_p256_t* ecc_p256);

int hosal_crypot_ecc_p192(hosal_crypto_ecc_p192_t* ecc_p192);

int hosal_crypot_ecc_b163(hosal_crypto_ecc_b163_t* ecc_b163);

int hosal_crypot_curve25519(hosal_crypto_curve25519_t* curve25519);

int hosal_crypto_ioctl(int ctl, void *p_arg);


#ifdef __cplusplus
}
#endif

#endif
