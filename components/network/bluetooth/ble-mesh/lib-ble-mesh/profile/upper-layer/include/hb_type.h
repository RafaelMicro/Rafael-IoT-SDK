

typedef struct _heartbeat_state_tag_
{
    uint16_t DestAddress;
    uint16_t PublCount;
    uint8_t  PublPeriodLog;
    uint8_t  PublTTL;
    uint16_t Relay: 1,
             Proxy: 1,
             Friend: 1,
             LowPower: 1,
             RFU: 12;
    uint16_t NetkeyIndex;

    uint16_t SubSource;
    uint16_t SubDestination;
    uint16_t SubCount;
    uint32_t SubPeriod;
    uint8_t  SubMinHops;
    uint8_t  SubMaxHops;
} heartbeat_state_t;



typedef struct __attribute__((packed)) _heartbeat_state_frame_tag_
{
    uint8_t     InitTTL: 7,
                RFU_TTL: 1;
    uint8_t     tmp;
    uint8_t     Relay: 1,
                Proxy: 1,
                Friend: 1,
                LowPower: 1,
                RFU_FEATURE: 4;
} heartbeat_state_frame_t;



