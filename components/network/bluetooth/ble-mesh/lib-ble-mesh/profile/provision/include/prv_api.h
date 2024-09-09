// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_API_H__
#define __PRV_API_H__

#ifdef __cplusplus
extern "C" {
#endif



void ecc_complete_cb(bool b_status);
void confirmation_chk_cb(bool b_status);
bool prov_api_validate_pubKey(ProvSessionData_t *pSessionData, uint8_t *pRecvPubKey, uint8_t u8_peerType);
bool prov_api_check_confirmation(ProvSessionData_t *pSessionData, uint8_t *pRecvRanom, uint8_t u8_devType);
void prov_api_caculate_session_key(ProvSessionData_t *pSessionData);
void prov_api_send_data(ProvSessionData_t *pSessionData, uint8_t *pRecvRanom);
bool prov_api_decrypt_data(ProvSessionData_t *pSessionData, uint8_t *pEncryptedDataAndMic);
uint8_t SetBitPosition(uint16_t bitMask);
void prov_api_send_failed(uint8_t errorCode);
void prov_api_send_input_complete(ProvSessionData_t *pSessionData, uint8_t oobSize, uint8_t *pu8_oobData);
#ifdef __cplusplus
};
#endif
#endif /* __PRV_API_H__ */
