


#ifndef MESH_LOCAL_CONFIG_TYPES_H
#define MESH_LOCAL_CONFIG_TYPES_H



#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! Data type for storing both SIG and Vendor model identifiers */
typedef struct meshModelId_tag
{
    bool_t    isSigModel; /*!< Determines if model type is SIG or Vendor */
    modelId_t modelId;    /*!< Model Id */
} meshModelId_t;

/*! Data type for Subscription list entries */
typedef struct meshLocalCfgModelSubscrListEntry_tag
{
    uint16_t    subscrAddressIndex;  /*!< Address index into one of the address lists */
    bool_t      subscrToLabelUuid;   /*!< Subscription Address is in Label UUID list */
} meshLocalCfgModelSubscrListEntry_t;

typedef struct meshLocalCfgModelSubscrListInfo_tag
{
    meshLocalCfgModelSubscrListEntry_t *pSubscrList;    /*!< Pointer to the start location of the
                                                       *   Subscription List in memory
                                                       */
    uint16_t                           subscrListSize;  /*!< Size of the Subscription List */
} meshLocalCfgModelSubscrListInfo_t;

/*! Structure to store Model Publication state informations */
typedef struct meshLocalCfgModelPublication_tag
{
    uint16_t                        publishAddressIndex;      /*!< Publish Address index in the
                                                             *   address list
                                                             */
    uint16_t                        publishAppKeyEntryIndex;  /*!< Publish AppKey index in the
                                                             *   AppKey list
                                                             */
    meshPublishPeriodNumSteps_t     publishPeriodNumSteps;    /*!< Publish period number of steps */
    meshPublishPeriodStepRes_t      publishPeriodStepRes;     /*!< Publish period step resolution */
    meshPublishFriendshipCred_t     publishFriendshipCred;    /*!< Publish friendship security
                                                             *   material
                                                             */
    uint8_t                         publishTtl;               /*!< Publish TTL */
    meshPublishRetransCount_t       publishRetransCount;      /*!< Publish retransmit count */
    meshPublishRetransIntvlSteps_t  publishRetransSteps50Ms;  /*!< Publish 50 ms retransmit steps */
    bool_t                          publishToLabelUuid;       /*!< Publish Address is virtual */
} meshLocalCfgModelPublication_t;

/*! Data type for Model instance list entries */
typedef struct meshLocalCfgModelEntry_tag
{
    meshModelId_t                    modelId;                /*!< Pointer to the Model structure */
    meshElementId_t                  elementId;              /*!< Element index in the Element array
                                                            */
    meshLocalCfgModelPublication_t   publicationState;       /*!< Model publication state */
    uint16_t                         subscrListStartIdx;     /*!< Start index in the Subscription list
                                                            *   for a specific model
                                                            */
    uint16_t                         appKeyBindListStartIdx; /*!< Start index in the AppKey Bind List
                                                            *   for a specific model
                                                            */
    uint8_t                          subscrListSize;         /*!< Subscription list size for the
                                                            *   model
                                                            */
    uint8_t                          appKeyBindListSize;     /*!< Size of AppKey Bind list for the
                                                            *   model
                                                            */
} meshLocalCfgModelEntry_t;

/*! Local structure to store Models informations */
typedef struct meshLocalCfgModelInfo_tag
{
    meshLocalCfgModelEntry_t *pModelArray;    /*!< Pointer to array describing models */
    uint16_t                 modelArraySize;  /*!< Size of the Models List */
} meshLocalCfgModelInfo_t;

/*! Local structure for attention timer */
typedef struct meshLocalCfgAttTmr_tag
{
    TimerHandle_t attTmr;          /*!< WSF timer */
    uint8_t    remainingSec;    /*!< Remaining seconds */
} meshLocalCfgAttTmr_t;

/*! Local structure to store Elements informations */
typedef struct meshLocalCfgElementInfo_tag
{
    const meshElement_t        *pElementArray;          /*!< Pointer to array describing elements
                                                       *   present in the node
                                                       */
    meshLocalCfgAttTmr_t       *pAttTmrArray;           /*!< Pointer to array attention timer
                                                       *   for each element
                                                       */
    meshSeqNumber_t            *pSeqNumberArray;        /*!< Pointer to SEQ number array for each
                                                       *   element
                                                       */
    meshSeqNumber_t            *pSeqNumberThreshArray;  /*!< Pointer to SEQ number threshold array for
                                                       *   each element
                                                       */
    uint8_t                    elementArrayLen;         /*!< Length of pElementArray and
                                                       *   pElementLocalCfgArray. Both have the
                                                       *   same length and indexes
                                                       */
} meshLocalCfgElementInfo_t;

/*! Data type for non-virtual address list entries */
typedef struct meshLocalCfgAddressListEntry_tag
{
    meshAddress_t  address;                /*!< Non-virtual address */
    uint16_t       referenceCountPublish;  /*!< Number of allocations of this address for
                                          *   publication
                                          */
    uint16_t       referenceCountSubscr;   /*!< Number of allocations of this address for
                                          *   subscription
                                          */
} meshLocalCfgAddressListEntry_t;

/*! Local structure to store Non-virtual Address informations */
typedef struct meshLocalCfgAddressListInfo_tag
{
    meshLocalCfgAddressListEntry_t *pAddressList;    /*!< Pointer to the start location of the
                                                    *   Non-virtual Address List in memory
                                                    */
    uint16_t                       addressListSize;  /*!< Size of the Address List */
} meshLocalCfgAddressListInfo_t;

/*! Data type for Virtual Address list entries */
typedef struct meshLocalCfgVirtualAddrListEntry_tag
{
    meshAddress_t  address;                           /*!< Virtual Address */
    uint8_t        labelUuid[MESH_LABEL_UUID_SIZE];   /*!< Label UUID */
    uint16_t       referenceCountPublish;             /*!< Number of allocations of this address
                                                     *  for publication
                                                     */
    uint16_t       referenceCountSubscr;              /*!< Number of allocations of this address
                                                     *  for subscription
                                                     */
} meshLocalCfgVirtualAddrListEntry_t;

/*! Local structure to store Label UUID informations */
typedef struct meshLocalCfgVirtualAddrListInfo_tag
{
    meshLocalCfgVirtualAddrListEntry_t *pVirtualAddrList;    /*!< Pointer to the start location of the
                                                            *   Virtual Address List in memory
                                                            */
    uint16_t                           virtualAddrListSize;  /*!< Size of the Virtual Address List */
} meshLocalCfgVirtualAddrListInfo_t;

/*! Local structure to store AppKey to Model ID bind informations */
typedef struct meshLocalCfgAppKeyBindListInfo_tag
{
    uint16_t *pAppKeyBindList;     /*!< Pointer to the start location of the AppKeyBind list
                                  *   in memory
                                  */
    uint16_t  appKeyBindListSize;  /*!< Size of the AppKeyBind List */
} meshLocalCfgAppKeyBindListInfo_t;

/*! Data type for AppKey list entries */
typedef struct meshLocalCfgAppKeyListEntry_tag
{
    uint16_t netKeyEntryIndex;              /*!< Index in the table of NetKeys for the NetKey binded
                                           *   to the AppKey
                                           */
    uint16_t appKeyIndex;                   /*!< AppKey Index to identify the AppKey in the list */
    uint8_t  appKeyOld[MESH_KEY_SIZE_128];  /*!< Old Application Key value */
    uint8_t  appKeyNew[MESH_KEY_SIZE_128];  /*!< New Application Key value */
    bool_t   newKeyAvailable;               /*!< Flag to signal that a new key is available */
} meshLocalCfgAppKeyListEntry_t;

/*! Local structure to store AppKey List related informations */
typedef struct meshLocalCfgAppKeyListInfo_tag
{
    meshLocalCfgAppKeyListEntry_t *pAppKeyList;    /*!< Pointer to the start location of the
                                                  *   AppKeyList in memory
                                                  */
    uint16_t                      appKeyListSize;  /*!< Size of the AppKey List */
} meshLocalCfgAppKeyListInfo_t;

/*! Data type for NetKey List entries */
typedef struct meshLocalCfgNetKeyListEntry_tag
{
    uint16_t                 netKeyIndex: 12,
                             RFU: 2,
                             newKeyAvailable: 1,
                             isLocated: 1;

    network_secmat_t         netKeyOld;
    network_secmat_t         netKeyNew;
    meshKeyRefreshStates_t   keyRefreshState;

    uint8_t                  NwkID[MESH_NETID_SIZE];
    uint8_t                  BeaconKey[MESH_KEY_SIZE_128];
    uint8_t                  IdentityKey[MESH_KEY_SIZE_128];

    pb_adv_sec_beacon_t      *pPrvSecureBeaconPtr;

} meshLocalCfgNetKeyListEntry_t;

/*! Data type for Node Identity State List entries. See::meshNodeIdentityStates_t */
typedef meshNodeIdentityStates_t meshLocalCfgNodeIdentityListEntry_t;

/*! Local structure to store NetKey List related informations */
typedef struct meshLocalCfgNetKeyListInfo_tag
{
    meshLocalCfgNetKeyListEntry_t        *pNetKeyList;       /*!< Pointer to the start location of
                                                            *   the NetKeyList in memory
                                                            */
    meshLocalCfgNodeIdentityListEntry_t  *pNodeIdentityList; /*!< Pointer to the start location of
                                                            *   the Node Identity State list in memory
                                                            */
    uint16_t                             netKeyListSize;     /*!< Size of the NetKey List */
} meshLocalCfgNetKeyListInfo_t;

/*! Mesh Local Config Friend Subscription notification event types enumeration */
enum meshLocalCfgFriendSubscrEventTypes
{
    MESH_LOCAL_CFG_FRIEND_SUBSCR_ADD,   /*!< Local Config subscription address add */
    MESH_LOCAL_CFG_FRIEND_SUBSCR_RM,    /*!< Local Config subscription address remove */
};

/*! Mesh Local Config Friend Subscription notification event type.
 *  See ::meshLocalCfgFriendSubscrEventTypes
 */
typedef uint8_t meshLocalCfgFriendSubscrEvent_t;

/*! Mesh Local Config Friend Subscription Event notification */
typedef struct meshLocalCfgFriendSubscrEventParams_tag
{
    meshAddress_t  address;        /*!< Address */
    uint16_t       idx;            /*!< Index in address list */
} meshLocalCfgFriendSubscrEventParams_t;


/*! Mesh Local Config Local structure */
typedef struct meshLocalCfgLocalInfo_tag
{
    meshAddress_t          address;                           /*!< 16-bit element address */
    meshProdInfo_t         prodInfo;                          /*!< Product Info */
    uint8_t                deviceKey[MESH_KEY_SIZE_128];      /*!< Device Key */
    uint8_t                defaultTtl;                        /*!< Default TTL value */
    meshRelayStates_t      relayState;                        /*!< Relay state
                                                             *   \see meshRelayStates_t
                                                             */
    meshBeaconStates_t     beaconState;                       /*!< Beacon state
                                                             *   \see meshBeaconStates_t
                                                             */
    meshGattProxyStates_t  gattProxyState;                    /*!< GATT Proxy state
                                                             *   \see meshGattProxyStates_t
                                                             */
    meshFriendStates_t     friendState;                       /*!< Friend state
                                                             *   \see meshFriendStates_t
                                                             */
    meshLowPowerStates_t   lowPowerState;                     /*!< Low Power state
                                                             *   \see meshLowPowerStates_t
                                                             */
    uint8_t                nwkTransCount;                     /*!< Network Transmission Count */
    uint8_t                nwkIntvlSteps;                     /*!< Network Interval Steps */
    uint8_t                relayRetransCount;                 /*!< Relay Retransmission Count */
    uint8_t                relayRetransIntvlSteps;            /*!< Relay Retransmission Interval
                                                             *   Steps
                                                             */
    bool_t                 ivUpdtInProg;                      /*!< IV Index update in progress
                                                             *   flag
                                                             */
} meshLocalCfgLocalInfo_t;





/*! Mesh Local Config Heartbeat Local structure */
typedef  struct meshLocalCfgHbLocalInfo_tag
{
    uint16_t       pubDstAddressIndex;  /*!< Publication Destination Address index in list */
    uint16_t       subSrcAddressIndex;  /*!< Subscription Source Address index in list */
    uint16_t       subDstAddressIndex;  /*!< Subscription Destination Address index in list */
    meshFeatures_t pubFeatures;         /*!< Publication Features */
    uint16_t       pubNetKeyEntryIndex; /*!< Publication NetKey entry index in NetKey list */
    uint8_t        pubCountLog;         /*!< Publication Count Log */
    uint8_t        subCountLog;         /*!< Subscription Count Log */
    uint8_t        pubPeriodLog;        /*!< Publication Period Log */
    uint8_t        subPeriodLog;        /*!< Subscription Period Log */
    uint8_t        pubTtl;              /*!< Publication TTL */
    uint8_t        subMinHops;          /*!< Subscription Minimum Hops */
    uint8_t        subMaxHops;          /*!< Subscription Maximum Hops */
} meshLocalCfgHbLocalInfo_t;


uint16_t meshLocalCfginit(void);

meshBeaconStates_t MeshLocalCfgGetBeaconState(void);
void MeshLocalCfgSetDefaultTtl(uint8_t defaultTtl);

uint8_t MeshLocalCfgGetDefaultTtl(void);

void MeshLocalCfgSetRelayState(meshRelayStates_t relayState);

meshRelayStates_t MeshLocalCfgGetRelayState(void);
void MeshLocalCfgSetGattProxyState(meshGattProxyStates_t gattProxyState);

meshGattProxyStates_t MeshLocalCfgGetGattProxyState(void);
void MeshLocalCfgSetFriendState(meshFriendStates_t friendState);

meshFriendStates_t MeshLocalCfgGetFriendState(void);

void MeshLocalCfgSetNwkTransmitCount(uint8_t transCount);

uint8_t MeshLocalCfgGetNwkTransmitCount(void);
void MeshLocalCfgSetNwkTransmitIntvlSteps(uint8_t intvlSteps);
uint8_t MeshLocalCfgGetNwkTransmitIntvlSteps(void);
void MeshLocalCfgSetRelayRetransmitCount(uint8_t retransCount);





uint8_t MeshLocalCfgGetRelayRetransmitCount(void);
void MeshLocalCfgSetRelayRetransmitIntvlSteps(uint8_t intvlSteps);

uint8_t MeshLocalCfgGetRelayRetransmitIntvlSteps(void);
bool MeshLocalCfgCheckModelId(bool isSIG, uint32_t ModelID);

bool MeshLocalCfgCheckElementAddr(uint16_t address);

void meshCfgMsgUnpackRelay(uint8_t *pBuf, meshRelayStates_t *pRelayState, meshRelayRetransState_t *pRetranState);

void meshCfgMsgPackRelay(uint8_t *pBuf, meshRelayStates_t *pRelayState, meshRelayRetransState_t *pRetranState);

void meshCfgMsgPackNwkTrans(uint8_t *pBuf, meshNwkTransState_t *pState);

void meshCfgMsgUnpackNwkTrans(uint8_t *pBuf, meshNwkTransState_t *pState);


#ifdef __cplusplus
}
#endif

#endif /* MESH_LOCAL_CONFIG_TYPES_H */

