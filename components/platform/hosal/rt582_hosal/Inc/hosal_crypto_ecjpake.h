/** @file hosal_uart.h
 * @license
 * @description
 */

#ifndef HOSAL_CRYPTO_ECJPAKE_H
#define HOSAL_CRYPTO_ECJPAKE_H

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
#define HOSLA_CRYPTO_ECJPAKE_INI    1
#define HOSAL_NUMBER_OF_GENERATES	2

typedef  enum
{
	HOSAL_ECJPAKE_GENERATE_ZKP = 0,
	HOSAL_ECJPAKE_GENERATE_VERIFY = 1,
	HOSAL_ECJPAKE_GENERATE_ZKP_2 = 2,
	HOSAL_ECJPAKE_GENERATE_VERIFY_2 = 3,
	HOSAL_ECJPAKE_COMPUTE_KEY = 4,
} hosal_crypto_ecjpake_operation_t;


//=============================================================================
//                  Structure Definition
//=============================================================================

typedef struct{ 
	ECJPAKE_CTX  *ctx;
	ECJPAKEKeyKP *key; 
	ECPoint_P256 *gen;
	uint8_t *private_key;
	uint8_t *pms;
	hosal_crypto_ecjpake_operation_t	crypto_operation;
}hosal_ecjpake_dev_t;


//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int hosal_crypto_ecjpake_init(void);

int hosal_crypto_ecjpake_operation(hosal_ecjpake_dev_t* ecjpake_dev);

int hosal_crypto_ecjpake_ioctl(int ctl, void *p_arg);

 
						  
#ifdef __cplusplus
}
#endif

#endif
