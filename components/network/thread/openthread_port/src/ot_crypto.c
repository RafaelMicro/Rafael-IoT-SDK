/**
 * @file ot_crypto.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <openthread/config.h>
// #include "openthread-system.h"
#include "mcu.h"
#include "rt_aes.h"
#include <openthread/instance.h>
#include <openthread/platform/crypto.h>
#include <openthread/platform/entropy.h>
#include <openthread/platform/time.h>

static struct aes_ctx saes_ecbctx;

otError otPlatCryptoAesInit(otCryptoContext *aContext)
{
    otError                error = OT_ERROR_NONE;

    aes_acquire(&saes_ecbctx);

    return error;
}

otError otPlatCryptoAesSetKey(otCryptoContext *aContext, const otCryptoKey *aKey)
{
    otError                error = OT_ERROR_NONE;

    aes_key_init(&saes_ecbctx, aKey->mKey, AES_KEY128);
    aes_load_round_key(&saes_ecbctx);

    return error;
}

otError otPlatCryptoAesEncrypt(otCryptoContext *aContext, const uint8_t *aInput, uint8_t *aOutput)
{
    otError                error = OT_ERROR_NONE;

   aes_ecb_encrypt(&saes_ecbctx, (uint8_t *)aInput, aOutput);


    return error;
}

otError otPlatCryptoAesFree(otCryptoContext *aContext)
{
    otError                error = OT_ERROR_NONE;

    aes_release(&saes_ecbctx);
    return error;
}
