#ifndef __SEC_AES_CMAC_H__
#define __SEC_AES_CMAC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void sec_aes_cmac(uint8_t *key, uint8_t *input, uint32_t length, uint8_t *mac);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SEC_AES_CMAC_H__ */
