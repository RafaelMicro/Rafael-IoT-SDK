


void sec_aes_ctr(uint8_t *pKey, uint8_t *pIv, uint8_t *pText, uint16_t len, uint8_t *pOut);
void sec_aes_cbc(uint8_t *pKey, uint8_t *pIv, uint8_t *pPlain, uint16_t Plen, uint8_t *Mac);
bool sec_aes_ccm_enc(uint8_t *pKey, uint8_t *pNonce, uint8_t *pHeader, uint16_t HeaderLen,
                     uint8_t *pPlainText, uint16_t textLen, uint8_t *pResult, uint8_t micLen);
bool sec_aes_ccm_dec(uint8_t *pKey, uint8_t *pNonce, uint8_t *pHeader, uint16_t HeaderLen,
                     uint8_t *pPlainText, uint16_t textLen, uint8_t *pResult, uint8_t micLen);
