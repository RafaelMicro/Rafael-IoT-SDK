/** @file hosal_uart.c
 * @license
 * @description
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mcu.h"
#include "ecc.h"
#include "hosal_crypto.h"
#include "hosal_crypto_sha256.h"

int hosal_crypto_ecc_init(int ctl) {

	int status= STATUS_SUCCESS;
	
	switch(ctl)
	{
		case HOSAL_ECC_CURVE_P256_INIT:
				
			gfp_ecc_curve_p256_init();
			
			break;
		
		case HOSAL_ECDA_CURVE_P256_VERIFY_INIT:
			
			gfp_ecdsa_p256_verify_init();
			
			break;
		
		case HOSAL_ECC_CURVE_P192_INIT:
			
			gfp_ecc_curve_p192_init();
		
			break;
		
		case HOSAL_ECC_CURVE_B163_INIT:
			
			gf2m_ecc_curve_b163_init();
		
			break;
		
		case HOSAL_CURVE_C25519_INIT:
			
			   curve_c25519_init();
		
			 break;
	}
	return status;
}


int hosal_crypto_curve_c25519_init(int ctl) {

	int status= STATUS_SUCCESS;
	
	switch(ctl)
	{		
		case HOSAL_CURVE_C25519_INIT:
			
			   curve_c25519_init();
		
			break;
		
		default:
				 return -1;
	}
	return status;
}

int hosal_crypot_ecc_p256(hosal_crypto_ecc_p256_t* ecc_p256)
{
	
	int status = STATUS_SUCCESS;
	
	if(ecc_p256->crypto_operation==HOSAL_ECC_P256_SIGNATURE)
	{
		status = gfp_ecdsa_p256_signature(ecc_p256->signatrue,ecc_p256->p_hash,ecc_p256->p_key,ecc_p256->p_k);
	}
	else if(ecc_p256->crypto_operation==HOSAL_ECDA_P256_VERIFY)
	{
		status = gfp_ecdsa_p256_verify(ecc_p256->signatrue,ecc_p256->p_hash,ecc_p256->base);
	
	}
	else if(ecc_p256->crypto_operation==HOSAL_ECDA_P256_MULTI)
	{
		status = gfp_point_p256_mult(ecc_p256->result,ecc_p256->base,ecc_p256->p_key);
	}

	return status;
}

int hosal_crypot_ecc_p192(hosal_crypto_ecc_p192_t* ecc_p192)
{
	
	int status = STATUS_SUCCESS;
	

	if(ecc_p192->crypto_operation==HOSAL_ECDA_P192_MULTI)
	{
		gfp_point_p192_mult(ecc_p192->p_result_x, ecc_p192->p_result_y, ecc_p192->target_x, ecc_p192->target_y, ecc_p192->target_k);
	}
	
	return status;
}

int hosal_crypot_ecc_b163(hosal_crypto_ecc_b163_t* ecc_b163)
{
	
	int status = STATUS_SUCCESS;
	

	if(ecc_b163->crypto_operation==HOSAL_ECDA_B163_MULTI)
	{
		gf2m_point_b163_mult(ecc_b163->p_result_x, ecc_b163->p_result_y, ecc_b163->target_x, ecc_b163->target_y, ecc_b163->target_k);
	}
	
	return status;
}


int hosal_crypot_curve25519(hosal_crypto_curve25519_t* curve25519)
{
	
	int status = STATUS_SUCCESS;
	

	if(curve25519->crypto_operation==HOSAL_CURVE_C25519)
	{
		curve25519_point_mul(curve25519->blind_zr, curve25519->public_key, curve25519->secret_key, curve25519->base_point);
	}
	else
	{
	    curve25519_release();
	}
	
	return status;
}

int hosal_crypot_ecc_p256_verify(hosal_crypto_ecc_p256_t* ecda)
{
	

	return -1;
}


int hosal_crypto_ioctl(int ctl, void *p_arg) {

	int status;
	
	status = STATUS_SUCCESS;
	


	return STATUS_SUCCESS;
}


   

