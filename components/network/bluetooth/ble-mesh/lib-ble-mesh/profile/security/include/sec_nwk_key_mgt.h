

#define BLE_MESH_KEY_SIZE   16

typedef struct __attribute__((packed)) _key_set_t_
{
    uint8_t EncryptionKey[16];
    uint8_t PrivacyKey[16];
    uint8_t NID;
} key_set_t;

typedef struct _nwk_sec_set_t_
{
    uint32_t    NetKeyindex;
    uint8_t     NetKey[BLE_MESH_KEY_SIZE];
    uint8_t     IdentifyKey[BLE_MESH_KEY_SIZE];
    uint8_t     BeaconKey[BLE_MESH_KEY_SIZE];
    uiby8_t     NetworkID[8];
    key_set_t   NetKeySet;
} nwk_sec_set_t;
