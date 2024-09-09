/** @file hosal_uart.h
 * @license
 * @description
 */

#ifndef HOSAL_CRYPTO_SHA256_H
#define HOSAL_CRYPTO_SHA256_H

#include "mcu.h"
#include "crypto.h"
#include "sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================
/**
* \brief crypto command
*
* \details 
*        
*/
#define HOSLA_CRYPTO_SHA256_INI         2


#define HOSAL_SHA256_DIGEST_SIZE   SHA256_DIGEST_SIZE  
#define HOSAL_SHA256_BLOCK_SIZE    SHA256_BLOCK_SIZE 


#define HOSAL_NUMBER_OF_GENERATES	2

typedef  enum
{
	HOSAL_SHA256_DIGEST = 0,
	HOSAL_SHA256_HMAC = 1,
	HOSAL_SHA256_HKDF = 2,
	HOSAL_SHA256_PBKDF2_HMAC = 3,
	//HOSAL_HMAC_DRBG_HMAC = 4,
} hosal_crypto_sha256_operation_t;

//=============================================================================
//                  Structure Definition
//=============================================================================

typedef struct{
	uint8_t* 	 key_ptr;
    uint8_t* 	 in_ptr;
	uint8_t* 	 out_ptr;
    uint8_t*     secret;	
    uint8_t*     salt;
	uint8_t*     info;
	uint32_t 	 key_length; 
	uint32_t 	 in_length; 
	uint32_t 	 out_len;
	uint32_t     secret_len;
	uint32_t     salt_len;
    uint32_t     info_len;      
	pbkdf2_st*   pbkdf2; 
	hosal_crypto_sha256_operation_t	crypto_operation;
}hosal_sha256_dev_t;


//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int hosal_crypto_sha256_init(void);

int hosal_crypto_sha256_operation(hosal_sha256_dev_t* sha256_dev);

int hosal_crypto_sha256_ioctl(int ctl, void *p_arg);

 
						  
#ifdef __cplusplus
}
#endif

#endif
