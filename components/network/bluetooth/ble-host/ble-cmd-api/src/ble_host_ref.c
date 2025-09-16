/*----------------------------------------------------------------------------*/
/* This file defined HCI & Host Queue Related */
/*----------------------------------------------------------------------------*/
#include <stdint.h>
#include "ble_host_ref.h"

/**************************************************************************
* Definition for BLE Host Stack
**************************************************************************/
/** Define the size of the hci command queue for hci task. */
uint16_t const QUEUE_HCI_COMMAND_EVENT = NUM_QUEUE_HCI_COMMAND_EVENT;

/** Define the size of the acl data queue for hci task. */
uint16_t const QUEUE_HCI_ACL_DATA_TX = NUM_QUEUE_HCI_ACL_DATA_TX;

/** Define the size of application event buffer queue for host task. */
uint16_t const QUEUE_HOST_TO_APP_EVENT = NUM_QUEUE_HOST_TO_APP_EVENT;

/** Define the size of acl data buffer queue for host task. */
uint16_t const QUEUE_HOST_TO_HCI_ACL_DATA = NUM_QUEUE_HOST_TO_HCI_ACL_DATA;

/** Define the size of the rx queue for host task. */
uint16_t const QUEUE_HOST_RX = NUM_QUEUE_HOST_RX;

/** Define the size of the host encrypt queue for host task. */
uint16_t const QUEUE_HOST_ENCRYPT = NUM_QUEUE_HOST_ENCRYPT;

/** Define maximum number of BLE connection link for BLE stack. */
uint8_t const MAX_CONN_NO_APP = BLE_SUPPORT_NUM_CONN_MAX;

/** A predefined and reserved block of memory used for the BLE host timers. */
uint32_t param_host_timer[NUM_LL_LINK][REF_SIZE_CMDTMR >> 2];

/** A predefined and reserved block of memory used for the BLE HCI to reassemble incomplete ACL packets. */
l2cap_sdus_reassembly_t l2cap_sdus_reassembly[NUM_LL_LINK];

/** A predefined and reserved block of memory used for the BLE Identity fuction. */
uint8_t param_host_identity[NUM_LL_LINK][22];

/** A predefined and reserved block of memory used for l2cap identifier variables. */
uint8_t l2cap_identifier[NUM_LL_LINK];

/** A predefined and reserved block of memory used for the BLE L2CAP Credit information. */
uint32_t param_l2cap_credit_based[NUM_LL_LINK][5];
