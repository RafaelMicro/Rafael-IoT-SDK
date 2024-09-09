

typedef struct _product_info_t
{
    uint32_t    CID: 16,
                PID: 16;
    uint32_t    VID: 16,
                RFU: 16;
    uint32_t    SW_Major_ver: 8,
                SW_SubMajor_ver: 8,
                SW_Minor_ver: 8,
                SW_SubMinor_ver: 8;
    uint32_t    SW_Build_code;
    uint8_t     *SDK_Ver;
    uint8_t     *HW_ver;
    uint8_t     *Model_ID;
    uint8_t     *Model_Name;

    uint8_t     Feature_Relay;
    uint8_t     Feature_Proxy;
    uint8_t     Feature_Friend;
    uint8_t     Feature_LowPoer;

} product_info_t;


typedef struct _prod_info_t_
{
    //uint32_t    MagicNum;
    bool        isProvisioned;
    uint16_t    CID;
    uint16_t    PID;
    uint16_t    VID;
    uint16_t    CRPL;
    uint16_t    F_Relay: 1,
                F_Proxy: 1,
                F_Friend: 1,
                F_LowPower: 1;
    uint32_t    HW_Major_ver: 16,
                HW_SubMajor_ver: 16;
    uint32_t    SW_Major_ver: 16,
                SW_SubMajor_ver: 16;
    uint32_t    SW_Build_code;
    uint8_t     *Model_ID;
    uint8_t     *Model_Name;
    uint8_t     UUID[16];
    uint8_t     MacAddr[6];
    uint8_t     PeerMacAddr[6];
    uint32_t    PB_ADV_Scan_Interval;
    uint32_t    PB_GATT_Adv_Interval;
    uint16_t    OOB;
    uint32_t    prov_dev_type;
    uint16_t    oobinfosrc;
    uint8_t     uuid[16];
    uint8_t     private_key[32];
    uint8_t     *p_public_key;
    uint8_t     static_oob[16];
    uint8_t     net_key[16];
    uint8_t     app_key[16];
    uint8_t     networkid[8];
    uint8_t     devkey[16];
    uint8_t     peerdevkey[16];
    uint32_t    attentionDuration;
    uint32_t    ivIndex;
    uint16_t    netKeyIndex;
    uint16_t    prov_start_address;
    uint16_t    element_addr;
    uint8_t     PrvFlag;
    uint8_t     ivupdate_test_flag;
    uint8_t     ivupdate_96_flag;
    ble_mesh_prov_evt_oob_caps_t t_caps;
    bool        prov_by_gatt;
    uint32_t    record_sequence_number;
    uint32_t    trs_sequence_number;
    uint16_t    mac_addr_type;

} prod_info_t;


/* for certification using only, have to build the real modle instance in the feature */

typedef struct __sig_moidel_subscribe_info_entry_tag_
{
    uint16_t ElementAddress;
    bool     isSIG;
    uint32_t ModelID;
    uint8_t  AddrCount;
    uint8_t  VAddrCount;
    uint16_t Address[5];
    uint8_t  UUID[5][16];
    uint16_t VirAddress[5];
} model_subscribe_info_t;
