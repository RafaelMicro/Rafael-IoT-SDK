

//#include "pal_types.h"
#include "P256-cortex-ecdh.h"
#include "sec_ecc.h"
#include "sec_aes.h"
#include "sec_aes_cmac.h"
#include "sec_aes_ccm.h"
#include "sec_mesh_hash.h"
#include "sec_main.h"
#include "sec_keygen.h"

typedef void (*pf_complete_cb)(bool b_status);
typedef struct SECURITY_PARAM_T
{
    uint8_t *pkey;
    uint8_t pNonce[13];
    uint8_t *pPlainText;
    uint8_t *pClear;
    uint8_t *pMic;
    uint16_t textLen;
    uint16_t clearLen;
    uint8_t *pResult;
    uint8_t micLen;
    pf_complete_cb complete_cb;
} sec_param_t;

bool sec_sw_ccm_dec(sec_param_t *pt_sec_param);
bool sec_sw_ccm_enc(sec_param_t *pt_sec_param);


