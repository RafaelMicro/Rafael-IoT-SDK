/** @file hosal_uart.h
 * @license
 * @description
 */

#ifndef HOSAL_CRYPTO_AES_CCM_H
#define HOSAL_CRYPTO_AES_CCM_H

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

#define HOSAL_AES_KEY128BIT     AES_KEY128 /*!< AES operation for decryption   */
#define HOSAL_AES_KEY192BIT     AES_KEY192 /*!< AES operation for encryption   */
#define	HOSAL_AES_KEY256BIT 	AES_KEY256
#define HOSAL_AES_BLOCKLEN		AES_BLOCKLEN/*!< Block length in bytes, AES is 128bits per block  */


#define HOSAL_NUMBER_OF_GENERATES	2

typedef  enum
{
	HOSAL_AES_CCM_DECRYPT = 0,
	HOSAL_AES_CCM_ENCRYPT = 1,
} hosal_crypto_operation_t;

typedef  enum
{
    HOSAL_AES_KEY128 = 0,     /*!< AES operation for decryption   */
    HOSAL_AES_KEY192 = 1,     /*!< AES operation for encryption   */
	HOSAL_AES_KEY256 = 2,
} hosal_crypto_keybit_t;


//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct{
	uint8_t* 					in_ptr;
	uint8_t*  					out_ptr; 
	uint8_t* 					key_ptr;
	uint8_t* 					iv;
	uint32_t					bit_length;
	uint32_t					cbc_length;
	hosal_crypto_keybit_t 		key_bit;
	hosal_crypto_operation_t	crypto_operation;
}hosal_aes_dev_t;




typedef struct{
	uint8_t*   key_ptr;
	uint32_t   bit_length;
    uint8_t   *payload_buf;      
    uint32_t   payload_length;   
    uint8_t   *nonce;
    uint8_t   *hdr; 		        	
    uint32_t   hdr_len;   	
    uint8_t   *data;      
    uint32_t   data_len;         
    uint32_t   mlen;             
    uint8_t    *out_buf;         
    uint32_t   *out_buf_len; 
    hosal_crypto_keybit_t 		key_bit;    
	hosal_crypto_operation_t	crypto_operation;
}hosal_aes_ccm_dev_t;


//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int hosal_crypto_aes_ccm_init(void);

int hosal_crypto_aes_ccm_operation(hosal_aes_ccm_dev_t* aes_ccm_dev);

int hosal_crypto_aes_ccm_ioctl(int ctl, void *p_arg);

	  
						  
#ifdef __cplusplus
}
#endif

#endif
