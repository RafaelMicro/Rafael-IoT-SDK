


bool sec_ecc_gen_public_key(uint8_t *pPrivatekey, uint8_t *pPublickeyX, uint8_t *pPublickeyY);
void sec_ecc_ecdh(uint8_t *pPrivatekey, uint8_t *pPublickeyX, uint8_t *pPublickeyY, uint8_t *pSharedSecret);
void sec_data_rev(uint8_t *pBuf, uint16_t len);

