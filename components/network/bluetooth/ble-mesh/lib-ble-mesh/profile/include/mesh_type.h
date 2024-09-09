// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MES_TYPE_H__
#define __MES_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> 
#include "mesh_def.h"

/**************************************************************************************************
  Data Types
**************************************************************************************************/
/*! \brief Mesh element address type */
typedef uint16_t meshAddress_t;

/*! \brief SIG model identifier definition */
typedef uint16_t meshSigModelId_t;

/*! \brief Vendor model identifier definition */
typedef uint32_t meshVendorModelId_t;

/*! \brief Mesh Element identifier definition */
typedef uint8_t meshElementId_t;

/*! \brief Mesh SEQ number type */
typedef uint32_t meshSeqNumber_t;

/*! \brief Supported features bit field data type. See ::meshFeaturesBitMaskValues */
typedef uint16_t meshFeatures_t;

/*! Publish period number of steps. Publish period is number of steps * step resolution
 *  \see MESH_PUBLISH_PERIOD_NUM_STEPS_MAX
 *  \see MESH_PUBLISH_PERIOD_DISABLED_NUM_STEPS
 */
typedef uint8_t meshPublishPeriodNumSteps_t;

/*! \brief Publish Period Step Resolution. See ::meshPublishPeriodStepResValues for valid values */
typedef uint8_t meshPublishPeriodStepRes_t;

/*! \brief Publish security credentials. See ::meshPublishCredValues for valid values */
typedef uint8_t meshPublishFriendshipCred_t;

/*! \brief Publish retransmit count. See ::MESH_PUBLISH_RETRANS_COUNT_MAX */
typedef uint8_t meshPublishRetransCount_t;

/*! Number of 50 millisecond steps between retransmissions of published messages.
 *  See ::MESH_PUBLISH_RETRANS_INTVL_STEPS_MAX
 */
typedef uint8_t meshPublishRetransIntvlSteps_t;

/*! \brief Mesh Relay states data type. See ::meshRelayStatesValues */
typedef uint8_t meshRelayStates_t;

/*! \brief Mesh Secure Network Beacon states data type. See ::meshBeaconStatesValues */
typedef uint8_t meshBeaconStates_t;

/*! \brief Mesh GATT Proxy states data type. See ::meshGattProxyStatesValues */
typedef uint8_t meshGattProxyStates_t;

/*! \brief Mesh Node Identity states data type. See ::meshNodeIdentityStatesValues */
typedef uint8_t meshNodeIdentityStates_t;

/*! \brief Mesh Friend states data type. See ::meshFriendStatesValues */
typedef uint8_t meshFriendStates_t;

/*! \brief Mesh Low Power states data type. See ::meshLowPowerStatesValues */
typedef uint8_t meshLowPowerStates_t;

/*! \brief Mesh Key Refresh Phase states data type. See ::meshKeyRefreshStatesValues */
typedef uint8_t meshKeyRefreshStates_t;

/*! \brief Mesh Key Refresh Transition data types. See ::meshKeyRefreshTransValues */
typedef uint8_t meshKeyRefreshTrans_t;

/*! \brief Mesh GATT Proxy PDU type. See ::meshGattProxyPduTypes */
typedef uint8_t meshGattProxyPduType_t;

/*! \brief Mesh Friendship RSSI factor, see ::meshFriendshipRssiFactorValues */
typedef uint8_t meshFriendshipRssiFactor_t;

/*! \brief Mesh Friendship Receive Window factor, see ::meshFriendshipRecvWinFactorValues */
typedef uint8_t meshFriendshipRecvWinFactor_t;

/*! \brief Mesh Friendship Min Queue size log, see ::meshFriendshipMinQueueSizeLogValues */
typedef uint8_t meshFriendshipMinQueueSizeLog_t;


typedef enum
{
    /** Invalid address. */
    MESH_ADDRESS_TYPE_INVALID,
    /** Unicast address. */
    MESH_ADDRESS_TYPE_UNICAST,
    /** Virtual address. */
    MESH_ADDRESS_TYPE_VIRTUAL,
    /** Group address. */
    MESH_ADDRESS_TYPE_GROUP,
    /** Indirect unicast address. */
    MESH_ADDRESS_TYPE_INDIRECT_UNICAST,
} mesh_address_type_t;

typedef enum
{
    /** Key refresh phase 0. Indicates normal device operation. */
    MESH_KEY_REFRESH_PHASE_0,
    /** Key refresh phase 1. Old keys are used for packet transmission, but new keys can be used to receive messages. */
    MESH_KEY_REFRESH_PHASE_1,
    /** Key refresh phase 2. New keys are used for packet transmission, but old keys can be used to receive messages. */
    MESH_KEY_REFRESH_PHASE_2,
    /** Key refresh phase 3. Used to complete a key refresh procedure and transition back to phase 0. */
    MESH_KEY_REFRESH_PHASE_3,
} mesh_key_refresh_phase_t;


/*! \brief Union of SIG and vendor model identifiers */
typedef union modelId_tag
{
    meshSigModelId_t    sigModelId;    /*!< SIG Model identifier */
    meshVendorModelId_t vendorModelId; /*!< Vendor Model identifier */
} modelId_t;

/*! \brief Message operation code structure */
typedef struct meshMsgOpcode_tag
{
    uint8_t opcodeBytes[3];  /*!< Opcode bytes */
} meshMsgOpcode_t;

/*! \brief Data type for storing the product information values */
typedef struct meshProdInfo_tag
{
    uint16_t companyId;  /*!< Company Identifier */
    uint16_t productId;  /*!< Product Identifier */
    uint16_t versionId;  /*!< Version Identifier */
} meshProdInfo_t;

/*! \brief Network Transmit state */
typedef struct meshNwkTransState_tag
{
    uint8_t                   transCount;              /*!< Number of transmissions for each Network
                                                      *   PDU
                                                      */
    uint8_t                   transIntervalSteps10Ms;  /*!< Number of 10-millisecond steps
                                                      *   between transmissions
                                                      */
} meshNwkTransState_t;

/*! \brief Relay Retransmit state */
typedef struct meshRelayRetransState_tag
{
    uint8_t                   retransCount;              /*!< Number of retransmission on advertising
                                                        *   bearer for each Network PDU
                                                        */
    uint8_t                   retransIntervalSteps10Ms;  /*!< Number of 10-millisecond steps
                                                        *   between retransmissions
                                                        */
} meshRelayRetransState_t;

/*! \brief Structure to store Model Publication state informations */
typedef struct meshModelPublicationParams_tag
{
    uint16_t                        publishAppKeyIndex;       /*!< Publish AppKey Index */
    meshPublishPeriodNumSteps_t     publishPeriodNumSteps;    /*!< Publish period number of steps */
    meshPublishPeriodStepRes_t      publishPeriodStepRes;     /*!< Publish period step resolution */
    meshPublishFriendshipCred_t     publishFriendshipCred;    /*!< Publish friendship security
                                                             *   material
                                                             */
    uint8_t                         publishTtl;               /*!< Publish TTL */
    meshPublishRetransCount_t       publishRetransCount;      /*!< Publish retransmit count */
    meshPublishRetransIntvlSteps_t  publishRetransSteps50Ms;  /*!< Publish 50 ms retransmit steps */
} meshModelPublicationParams_t;

/*! \brief NetKey index list for a specific node. */
typedef struct meshNetKeyList_tag
{
    uint8_t   netKeyCount;     /*!< Size of the pNetKeyIndexes array */
    uint16_t *pNetKeyIndexes;  /*!< Array of NetKey indexes */
} meshNetKeyList_t;

/*! \brief Key indexes for a NetKey and a bound AppKey */
typedef struct meshAppNetKeyBind_tag
{
    uint16_t netKeyIndex;  /*!< Associated NetKey index */
    uint16_t appKeyIndex;  /*!< AppKey index */
} meshAppNetKeyBind_t;

/*! \brief AppKey index list bound to a specific NetKey index */
typedef struct meshAppKeyList_tag
{
    uint16_t  netKeyIndex;     /*!< Associated NetKey index */
    uint8_t   appKeyCount;     /*!< Size of the pAppKeyIndexes array */
    uint16_t *pAppKeyIndexes;  /*!< Array of AppKey indexes */
} meshAppKeyList_t;

/*! \brief AppKey index list bound to a specific model */
typedef struct meshModelAppList_tag
{
    meshAddress_t         elemAddr;         /*!< Address of the element containing the model */
    modelId_t             modelId;          /*!< Model identifier */
    bool_t                isSig;            /*!< TRUE if model identifier is SIG, FALSE for vendor */
    uint8_t               appKeyCount;      /*!< Size of the pAppKeyIndexes array */
    uint16_t             *pAppKeyIndexes;   /*!< Array of AppKey indexes */
} meshModelAppList_t;

/*! \brief Composition Data */
typedef struct meshCompData_tag
{
    uint8_t  pageNumber;  /*!< Page number */
    uint16_t pageSize;    /*!< Size of the pPage array */
    uint8_t  *pPage;      /*!< Page in raw octet format (as received over the air) */
} meshCompData_t;

/*! \brief Heartbeat Publication state data */
typedef struct meshHbPub_tag
{
    meshAddress_t           dstAddr;      /*!< Destination address for heartbeat message */
    uint8_t                 countLog;     /*!< Number of heartbeat messages to be sent */
    uint8_t                 periodLog;    /*!< Period for sending heartbeat messages */
    uint8_t                 ttl;          /*!< TTL used when sending heartbeat messages */
    meshFeatures_t          features;     /*!< Bit field for features that trigger heartbeat messages */
    uint16_t                netKeyIndex;  /*!< Associated NetKey index */
} meshHbPub_t;

/*! \brief Heartbeat Subscription state data */
typedef struct meshHbSub_tag
{
    meshAddress_t           srcAddr;      /*!< Source address for heartbeat message */
    meshAddress_t           dstAddr;      /*!< Destination address for heartbeat message */
    uint8_t                 periodLog;    /*!< Period for sending heartbeat messages */
    uint8_t                 countLog;     /*!< Number of heartbeat messages to be sent */
    uint8_t                 minHops;      /*!< Min hops when receiving heartbeats */
    uint8_t                 maxHops;      /*!< Max hops when receiving heartbeats */
} meshHbSub_t;


/*! \brief Mesh Friendship Criteria structure */
typedef struct meshFriendshipCriteria_tag
{
    meshFriendshipRssiFactor_t        rssiFactor;              /*!< RSSI factor */
    meshFriendshipRecvWinFactor_t     recvWinFactor;           /*!< Receive Window factor */
    meshFriendshipMinQueueSizeLog_t   minQueueSizeLog;         /*!< Min Queue size log */
} meshFriendshipCriteria_t;


/*! \brief Mesh Model event for ::MESH_MODEL_EVT_MSG_RECV */
typedef struct meshModelMsgRecvEvt_tag
{
    meshElementId_t      elementId;         /*!< Identifier of the element which received the message */
    meshAddress_t        srcAddr;           /*!< Address off the element that sent the message */
    uint8_t              ttl;               /*!< TTL of the received message. */
    bool_t               recvOnUnicast;     /*!< Indicates if message was received on unicast */
    meshMsgOpcode_t      opCode;            /*!< Opcode of the message */
    uint8_t              *pMessageParams;   /*!< Pointer to the message parameters */
    uint16_t             messageParamsLen;  /*!< Message parameters data length */
    uint16_t             appKeyIndex;       /*!< Global Application Key identifier. */
    modelId_t            modelId;           /*!< Model identifier. */
} meshModelMsgRecvEvt_t;

/*! \brief Mesh Model event for ::MESH_MODEL_EVT_PERIODIC_PUB. */
typedef struct meshModelPeriodicPubEvt_tag
{
    meshElementId_t      elementId;         /*!< Identifier of the element which received the message
                                           */
    uint32_t             nextPubTimeMs;     /*!< Next publication time in ms */
    modelId_t            modelId;           /*!< Model identifier. */
    bool_t               isVendorModel;     /*!< Vendor Model identifier */
} meshModelPeriodicPubEvt_t;

/*! \brief Mesh Model event union. */
typedef union meshModelEvt_tag
{
    meshModelMsgRecvEvt_t     msgRecvEvt;     /*!< Mesh Model message received event */
    meshModelPeriodicPubEvt_t periodicPubEvt; /*!< Mesh Model periodic publish timer expired event */
} meshModelEvt_t;

/*! \brief Mesh SIG model definition */
typedef struct meshSigModel_tag
{
    void                   *pModelDescriptor;   /*!< Pointer to the model descriptor */
    void         *pHandlerId;         /*!< Pointer to Model WSF handler ID */
    const meshMsgOpcode_t  *pRcvdOpcodeArray;   /*!< Pointer to array of supported received
                                               *   SIG opcodes
                                               */
    meshSigModelId_t       modelId;             /*!< Model ID, as assigned by the SIG */
    uint8_t                opcodeCount;         /*!< Number of SIG defined opcodes
                                               *   supported
                                               */
    uint8_t                subscrListSize;      /*!< Subscription list size */
    uint8_t                appKeyBindListSize;  /*!< AppKey to Model bind list size */
} meshSigModel_t;

/*! \brief Mesh Vendor model definition */
typedef struct meshVendorModel_tag
{
    void                      *pModelDescriptor;   /*!< Pointer to the model descriptor */
    void                      *pHandlerId;         /*!< Pointer to Model WSF handler ID */
    const meshMsgOpcode_t     *pRcvdOpcodeArray;   /*!< Pointer to array of supported received
                                                  *   SIG opcodes
                                                  */
    meshVendorModelId_t       modelId;             /*!< Model ID, as assigned by vendor */
    uint8_t                   opcodeCount;         /*!< Number of SIG defined opcodes supported */
    uint8_t                   subscrListSize;      /*!< Subscription list size */
    uint8_t                   appKeyBindListSize;  /*!< AppKey to Model bind list size */
} meshVendorModel_t;


/*!
 *  Mesh element definition
 *
 *  Usage:
 *  \code
 *  const meshSigModel_t elem0SigModels[2] = {{.modelId = 0xaaaa}, {.modelId = 0xbbbb}};
 *
 *  meshElement_t firstElement =
 *  {
 *    .numSigModels         = 2,
 *    .numVendorModels      = 0,
 *    .pSigModelArray       = elem0SigModels,
 *    .pVendorModedArray    = NULL
 *  };
 *  \endcode
 */
typedef struct meshElement_tag
{
    uint16_t                locationDescriptor;  /*!< Location descriptor as defined
                                                *   in the GATT Bluetooth Namespace
                                                */
    uint8_t                 numSigModels;        /*!< Number of SIG models in this element */
    uint8_t                 numVendorModels;     /*!< Number of Vendor models in this element */
    const meshSigModel_t    *pSigModelArray;     /*!< Pointer to array containing numSigModels
                                                *   number of SIG models in this element
                                                */
    const meshVendorModel_t *pVendorModelArray;  /*!< Pointer to array containing numVendorModels
                                                *   number of vendor models for this element
                                                */
} meshElement_t;




typedef struct
{
    /** Address type. */
    mesh_address_type_t type;
    /** Address value. */
    uint16_t value;
    /** 128-bit virtual label UUID, will be NULL if type is not @c MESH_ADDRESS_VIRTUAL . */
    uint8_t *p_virtual_uuid;
} mesh_address_t;



typedef struct _mesh_pkt_info_tag_
{
    uint16_t Src;
    uint16_t Dst;
    uint16_t AppKeyIndex;
    uint16_t NetKeyIndex;
    uint16_t Interface;
    uint8_t  PktSts;
} mesh_pkt_info_t;


typedef struct
{
    uint8_t key[MESH_KEY_SIZE];
    uint8_t net_id[MESH_NETID_SIZE];
    uint8_t identity_key[MESH_KEY_SIZE];
} mesh_beacon_secmat_t;

typedef struct
{
    uint16_t rx_count;
    uint8_t observation_count;
    uint16_t interval;
} mesh_beacon_tx_info_t;

typedef struct
{
    bool iv_update_permitted;
    mesh_beacon_tx_info_t *p_tx_info;
    mesh_beacon_secmat_t secmat;
    mesh_beacon_secmat_t secmat_updated;
} mesh_beacon_info_t;


#endif /* __MES_TYPE_H__ */
