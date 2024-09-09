/** @file hosal_uart.c
 * @license
 * @description
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mcu.h"
#include "crypto.h"
#include "hosal_crypto_sha256.h"


int hosal_crypto_sha256_init(void) {

	int status;
	
	sha256_vector_init();
	
	return status;
}


int hosal_crypto_sha256_operation(hosal_sha256_dev_t* sha256_dev)
{
	int status;
	sha256_context sha_cnxt;
	status = STATUS_SUCCESS;
	
	if(sha256_dev->crypto_operation==HOSAL_SHA256_HMAC)
	{
	   hmac_sha256(sha256_dev->key_ptr, sha256_dev->key_length, sha256_dev->in_ptr, sha256_dev->in_length, sha256_dev->out_ptr);
	}
	else if(sha256_dev->crypto_operation==HOSAL_SHA256_HKDF)
	{
	   status = hkdf_sha256(sha256_dev->out_ptr, sha256_dev->out_len,sha256_dev->in_ptr,  sha256_dev->in_length,  sha256_dev->salt, sha256_dev->salt_len, sha256_dev->info, sha256_dev->info_len);
	}
	else if(sha256_dev->crypto_operation==HOSAL_SHA256_DIGEST)
	{
		sha256_init(&sha_cnxt);
		sha256_update(&sha_cnxt, sha256_dev->in_ptr, sha256_dev->in_length);           //test sha256("abc")
		sha256_finish(&sha_cnxt, sha256_dev->out_ptr);	
	}
	else if(sha256_dev->crypto_operation==HOSAL_SHA256_PBKDF2_HMAC)
	{
		status = pbkdf2_hmac(sha256_dev->pbkdf2);	
	}	
	else	
	{
		return -1;
	}

	return status;
}


int hosal_crypto_sha256_ioctl(int ctl, void *p_arg) {

	int status;
	
	status = STATUS_SUCCESS;
	
	
	return STATUS_SUCCESS;
}


   