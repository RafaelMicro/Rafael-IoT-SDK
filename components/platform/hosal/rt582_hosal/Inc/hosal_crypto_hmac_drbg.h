/** @file hosal_uart.h
 * @license
 * @description
 */

#ifndef HOSAL_CRYPTO_HAMC_DRBG_H
#define HOSAL_CRYPTO_HMAC_DRBG_H

#include "mcu.h"
#include "crypto.h"
#include "sha256.h"
#include "aes.h"

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
//#define HOSLA_CRYPTO_AES_INI          	1  /**< \brief baud set */
//#define HOSLA_CRYPTO_SHA256_INI         2


//#define HOSAL_AES_KEY128BIT     AES_KEY128 /*!< AES operation for decryption   */
//#define HOSAL_AES_KEY192BIT     AES_KEY192 /*!< AES operation for encryption   */
//#define	HOSAL_AES_KEY256BIT 	AES_KEY256
//#define HOSAL_AES_BLOCKLEN		AES_BLOCKLEN/*!< Block length in bytes, AES is 128bits per block  */


//#define HOSAL_SHA256_DIGEST_SIZE   SHA256_DIGEST_SIZE  
//#define HOSAL_SHA256_BLOCK_SIZE    SHA256_BLOCK_SIZE 


#define HOSAL_NUMBER_OF_GENERATES	2

typedef  enum
{
	HOSAL_HMAC_DRBG_HMAC = 1,
} hosal_crypto_hmac_drbg_operation_t;

//typedef  enum
//{
//    HOSAL_AES_KEY128 = 0,     /*!< AES operation for decryption   */
//    HOSAL_AES_KEY192 = 1,     /*!< AES operation for encryption   */
//	HOSAL_AES_KEY256 = 2,
//} hosal_crypto_keybit_t;


//=============================================================================
//                  Structure Definition
//=============================================================================

typedef struct{
	uint8_t *    seed_material; 
	uint8_t* 	 in_ptr;
	uint8_t* 	 out_ptr;
	uint8_t* 	 data_ptr;
	uint32_t     seed_material_length;
	uint32_t 	 in_length; 	
	uint32_t 	 out_len;
	uint32_t	 data_len;
	hosal_crypto_hmac_drbg_operation_t	crypto_operation;
}hosal_hmac_drbg_dev_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int hosal_crypto_hmac_drbg_init(void);

int hosal_crypto_hmac_drbg_operation(volatile hosal_hmac_drbg_dev_t* hmac_drbg);

int hosal_hmac_drbg_generate_k(hosal_hmac_drbg_dev_t* hmac_drbg);

int hosal_crypto_hmac_drbg_ioctl(int ctl, void *p_arg);
						  
void  hosal_hmac_drbg_update(hmac_drbg_state *state, uint8_t *data, uint32_t  data_length);

void  hosal_hmac_drbg_generate(uint8_t *out_result, hmac_drbg_state *state, uint32_t request_bytes,uint8_t *data, uint32_t data_length);

uint32_t hosal_hmac_drbg_instantiate(hmac_drbg_state *state, uint8_t *seed_material, uint32_t seed_material_length);


#ifdef __cplusplus
}
#endif

#endif
