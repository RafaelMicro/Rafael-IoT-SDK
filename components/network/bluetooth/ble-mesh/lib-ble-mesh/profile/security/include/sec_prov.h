
#if 0
typedef struct _provision_data_set_t_
{
    uint8_t PrivateKey[32];
    uint8_t MyPublicKeyX[32];
    uint8_t MyPublicKeyY[32];
    uint8_t PeerPublicKeyX[32];
    uint8_t PeerPublicKeyY[32];
    uint8_t EDCHSecret[32];
    uint8_t ConfirmationInput[1 + 11 + 5 + 64 + 64];
    uint8_t ConfirmationSalt[32];
    uint8_t MyConfirmationData[32];
    uint8_t PeerConfirmationData[32];
    uint8_t MyConfirmation[16];
    uint8_t PeerConfirmation[16];
    uint8_t SessionKey[16];
    uint8_t SessionNonce[16];
    uint8_t ProvisionData[33];
} provision_data_set_t;

typedef struct __attribute__((packed)) _provision_data_set_t_
{
    uint8_t     NetKey[16];
    uint16_t    KeyIndex;
    uint8_t     Flag;
    uint32_t    IV_Index;
    uint16_t    UniAddr;
} provision_data_set_t;
#endif


void sec_provision_confirmation(uint8_t *pECDHSecret, uint16_t ECDHSecret_ln,
                                uint8_t *pConfirmationInput, uint16_t ConfirmationInput_len,
                                uint8_t *pConfirmationData, uint16_t ConfirmationData_len,
                                uint8_t *pConfirmation);

bool sec_provision_dec(uint8_t *pProvisionInput, uint16_t ProvisionInput_len,
                       uint8_t *pECDHSecret, uint16_t ECDHSecret_len,
                       uint8_t *pProvisionData, uint16_t ProvisionData_len,
                       uint16_t Mic_len);

bool sec_provision_enc(uint8_t *pProvisionInput, uint16_t ProvisionInput_len,
                       uint8_t *pECDHSecret, uint16_t ECDHSecret_len,
                       uint8_t *pProvisionData, uint16_t ProvisionData_len,
                       uint16_t Mic_len);

