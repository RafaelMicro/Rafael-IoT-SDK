

#define CONFIRMATION_KEY_TEMP          "prck"
#define SESSION_KEY_TEMP               "prsk"
#define SESSION_NONCE_TEMP             "prsn"
#define DEVICE_KEY_TEMP                "prdk"

#define PROV_HW_SEC                     0


typedef void    (*pf_ecc_complete_cb)(bool b_status);
typedef void    (*pf_check_cb)(bool b_status);
typedef struct
{
    ProvSessionData_t *pSessionData;
    pf_ecc_complete_cb complete_cb;
} sec_public_key_gen_t;

typedef struct
{
    uint8_t u8_pubkey_idx;
    ProvSessionData_t *pSessionData;
    pf_ecc_complete_cb complete_cb;
} sec_caculate_ecdh_t;

typedef struct
{
    uint8_t u8_my_random_idx;
    ProvSessionData_t *pSessionData;
    pf_ecc_complete_cb complete_cb;
} sec_confirmation_gen_t;

typedef struct
{
    ProvSessionData_t *pSessionData;
    pf_check_cb check_cb;
} sec_confirmation_check_t;

typedef struct
{
    ProvSessionData_t *pSessionData;
    pf_ecc_complete_cb complete_cb;
} sec_session_key_gen_t;

ble_err_t mesh_security_init(void);
uint8_t mesh_security_handle(mesh_tlv_t *pt_tlv, uint32_t u32_tlv_wait_tick);


static void GenerateSalt(uint8_t *pu8_confirmation_input, uint8_t u8_input_size, uint8_t *pu8_confirmation_salt);
static void GenerateConfirmationKey(uint8_t *pu8_confirmation_salt, uint8_t *pu8_text, uint8_t u8_text_size, uint8_t *pu8_confirmation_key);
static void GenerateConfirmation(uint8_t *pu8_confirmation_key, uint8_t *pu8_text, uint8_t u8_text_size, uint8_t *pu8_confirmation);
static void GenerateDeviceKey(uint8_t *pu8_provision_salt, uint8_t *pu8_text, uint8_t u8_text_size, uint8_t *pu8_session_key, uint8_t *pu8_session_noce, uint8_t *pu8_device_key);


bool sec_pubkey_gen(sec_public_key_gen_t *pt_pubkey_param);
bool sec_edch_cal(sec_caculate_ecdh_t *pt_ecdh_param);
void sec_prov_confirmation_gen(sec_confirmation_gen_t *pt_confirmation_param);
bool sec_prov_confirmation_chk(sec_confirmation_check_t *pt_confirmation_chk_param);
void sec_prov_session_key_gen(sec_session_key_gen_t *pt_session_key_gen_param);

