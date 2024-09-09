

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>






#define MAGIC_NUMBER    0x16581688

#define NETWORK_CACHE_ENTRY_COUNT   300


enum _product_const_
{
    PRODUCT_ELEMENT_COUNT = 1,                 /*!< Number of elements supported by product */
    ADDRESS_LIST_SIZE = 20,                    /*!< Maximum number of non-virtual addresses stored */
    VIRTUAL_ADDRESS_LIST_SIZE = 20,            /*!< Maximum number of virtual addresses stored */
    APP_KEY_LIST_SIZE = 20,                    /*!< Maximum number of AppKeys stored */
    NET_KEY_LIST_SIZE = 20,                    /*!< Maximum number of NetKeys stored */
    NWK_CACHE_SIZE = 32,                       /*!< Maximum number of elements in Level 1 Network Cache */
    REPLAY_LIST_SIZE = 32,                     /*!< Maximum number of elements in Replay Protection List */
    //   NWK_UPPER_CACHE_SIZE = 32,               /*!< Maximum number of elements in Level 2 Network */
    NWK_OUTPUT_FILTER_SIZE = 20,               /*!< Maximum number of element addresses in the output filter of a network interface */
    SAR_RX_TRAN_HISTROY_SIZE = 20,             /*!< Maximum number of elements in SAR Rx Transaction History */
    SAR_RX_TRAN_INFO_SIZE = 20,                /*!< Maximum number of elements in SAR Rx Transaction Info Table */
    SAR_TX_MAX_TRANS_SIZE = 20,                /*!< Maximum number of SAR TX transactions */
    MAX_NUM_FRIENDS = 10,                      /*!< Maximum number of friendships this node can establish */
    MAX_NUM_FRIEND_Q_RNTRIES = 20,             /*!< Maximum number of entries for a friend queue */
    MAX_FRIEND_SUBSCR_LIST_SIZE = 10,          /*!< Maximum number of subscription addresses  for a friendship */
    GATT_CONNECTION_NUMBER = 10,

    /* for client device only */
    MAX_NUM_CONFIG_DEVICE_KEY_SUPPORT = 0,      /*!< Maximum number of Configuration Servers supported simultaneously by the Configuration Client */

    PRODUCT_TOTAL_MODELS = 1,
    MODEL_APP_KEY_BINDING_LIST_SIZE = 2,
    MODEL_SUBSCR_LIST_SIZE = 2,


    PRODUCT_FRIEND_FEATURE_SUPPORTED = 1,
    PRODUCT_RELAY_FEATURE_SUPPORTED = 1,
    PRODUCT_PROXY_FEATURE_SUPPORTED = 1,
    PRODUCT_LOWPOWER_FEATURE_SUPPORTED = 1,

    PRODUCT_DEFAULT_TTL_INIT = 0x20,
    PRODUCT_PROXY_STATE_INIT = 1,
    PRODUCT_RELAY_STATE_INIT = 1,
    PRODUCT_FRIEND_STATE_INIT = 1,

    PRODUCT_SECURE_BEACON_INTERVAL = 10000,  /*!< 10000 ms, 10s */

};



