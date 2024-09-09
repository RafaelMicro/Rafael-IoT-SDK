#ifndef __MESH_CFG_MDL_TYPE_H__
#define __MESH_CFG_MDL_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif





typedef struct __attribute__((packed)) _req_publish_frame_tag_
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
    uint32_t ModelID;
} req_publish_frame_t;


typedef struct __attribute__((packed)) _req_publish_virtual_frame_tag_
{
    uint16_t ElementAddress;
    uint8_t  uuid[16];
    uint16_t AppKeyIndex: 12,
    CredentialFlag: 1,
    RFU: 3;
    uint8_t  PublishTTL;
    uint8_t  PublishPeriodStep: 6,
    PublishPeriodResolution: 2;
    uint8_t  PublishRetransmitCount: 3,
    PublishRetransmitIntervalSteps: 5;
    uint32_t ModelID;
} req_publish_virtual_frame_t;


typedef struct __attribute__((packed)) _rsp_publish_frame_tag_
{
    uint8_t  Status;
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
    uint32_t ModelID;
} rsp_publish_frame_t;


typedef struct __attribute__((packed)) _req_subscribe_frame_tag_
{
    uint16_t ElementAddress;
    uint16_t SubscribeAddress;
    uint32_t ModelID;
} req_subscribe_frame_t;


typedef struct __attribute__((packed)) _req_subscribe_virtual_frame_tag_
{
    uint16_t ElementAddress;
    uint8_t  uuid[16];
    uint32_t ModelID;
} req_subscribe_virtual_frame_t;


typedef struct __attribute__((packed)) _rsp_subscribe_frame_tag_
{
    uint8_t  Status;
    uint16_t ElementAddress;
    uint16_t SubscribeAddress;
    uint32_t ModelID;
} rsp_subscribe_frame_t;

typedef struct __attribute__((packed)) _rsp_sig_sub_list_tag_
{
    uint8_t     Status;
    uint16_t    ElementAddress;
    uint16_t    ModelID;
    uint16_t    AddrList[20];
} rsp_sig_sub_list_t;

typedef struct __attribute__((packed)) _rsp_vend_sub_list_tag_
{
    uint8_t     Status;
    uint16_t    ElementAddress;
    uint32_t    ModelID;
    uint16_t    AddrList[20];
} rsp_vend_sub_list_t;


typedef struct __attribute__((packed)) _req_hb_publ_frame_tag_
{
    uint16_t Destination;
    uint8_t  CountLog;
    uint8_t  PeriodLog;
    uint8_t  TTL;
    uint16_t Relay: 1,
    Proxy: 1,
    Friend: 1,
    LowPower: 1,
    RFU: 12;
    uint16_t  NetKeyIndex;
} req_hb_publ_frame_t;

typedef struct __attribute__((packed)) _rsp_hb_publ_frame_tag_
{
    uint8_t  Status;
    uint16_t Destination;
    uint8_t  CountLog;
    uint8_t  PeriodLog;
    uint8_t  TTL;
    uint16_t Relay: 1,
    Proxy: 1,
    Friend: 1,
    LowPower: 1,
    RFU: 12;
    uint16_t  NetKeyIndex;
} rsp_hb_publ_frame_t;


typedef struct __attribute__((packed)) _req_hb_sur_frame_tag_
{
    uint16_t    Source;
    uint16_t    Destination;
    uint8_t     PeriodLog;
} req_hb_sur_frame_t;


typedef struct __attribute__((packed)) _rsp_hb_sur_frame_tag_
{
    uint8_t     Status;
    uint16_t    Source;
    uint16_t    Destination;
    uint8_t     PeriodLog;
    uint8_t     CountLog;
    uint8_t     MinHops;
    uint8_t     MaxHops;
} rsp_hb_sur_frame_t;



typedef struct __attribute__((packed)) _req_key_re_phase_frame_tag_
{
    uint16_t NetKeyIndex;
    uint8_t  Transition;
} req_key_re_phase_frame_t;

typedef struct __attribute__((packed)) _rsp_key_re_phase_frame_tag_
{
    uint8_t  Status;
    uint16_t NetKeyIndex;
    uint8_t  Transition;
} rsp_key_re_phase_frame_t;

#ifdef __cplusplus
};
#endif
#endif /* __MESH_DSM_H__ */

