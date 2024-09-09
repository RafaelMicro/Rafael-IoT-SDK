


/* publish data entry defition */

typedef struct  _publish_info_entry_tag_
{
    uint16_t ElementAddress;
    uint16_t PublishAddress;
    uint16_t AppKeyIndex: 12,
             CredentialFlag: 1,
             RFU: 3;
    uint8_t  PublishTTL;
    uint8_t  PublishPeriodStep: 6,
             PublishPeriodResolution: 2;
    uint8_t  PublishRetransmitCount: 3,
             PublishRetransmitIntervalSteps: 5;
    bool     isSigMidel;
    uint32_t ModelID;
} publish_info_entry_t;


typedef struct _publish_virtual_info_entry_tag_
{
    uint16_t ElementAddress;
    uint8_t  PublishAddress[16];
    uint16_t VirtualAddress;
    uint16_t AppKeyIndex: 12,
             CredentialFlag: 1,
             RFU: 3;
    uint8_t  PublishTTL;
    uint8_t  PublishPeriodStep: 6,
             PublishPeriodResolution: 2;
    uint8_t  PublishRetransmitCount: 3,
             PublishRetransmitIntervalSteps: 5;
    bool     isSigMidel;
    uint32_t ModelID;
} publish_virtual_info_entry_t;

/* publish using single timer in 10ms period to check the pulish state*/
typedef struct _publish_tx_cb_tag_
{
    uint16_t isLocated: 1,
             isNeedReTx: 1,
             RFU: 6,
             PublishTxCurrentInterval: 5,
             PublishReTxCount: 3;
    uint16_t PublishEntryIndex;
    uint32_t PublishPeriodCurrentTime;
} publish_tx_cb_t;


/* subscribe data entry defition */

typedef struct _subscribe_info_entry_tag_
{
    uint16_t ElementAddress;
    uint16_t SubscribeAddress;
    bool     isSigMidel;
    uint32_t Model;
} subscribe_info_entry_t;

typedef struct _subscribe_virtual_info_entry_tag_
{
    uint16_t ElementAddress;
    uint8_t  UUID[16];
    uint16_t VirtualAddress;
    bool     isSigMidel;
    uint32_t Model;
} subscribe_virtual_info_entry_t;
