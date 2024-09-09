#ifndef __SEC_AES_H__
#define __SEC_AES_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void sec_aes_enc(uint8_t *key, uint8_t *din, uint8_t *dout);

void sec_obfuscation(uint8_t *pKey, uint8_t *pIv, uint8_t *pText, uint16_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SEC_AES_H__ */
