// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_DEF_H__
#define __PRV_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif




/**************************************************************************************************
  Macros
**************************************************************************************************/
/*! Extracts the padding value from the first octet of a Control PDU */
#define EXTRACT_PADDING(byte) ((byte) >> MESH_PRV_GPCF_SIZE)

/*! Extracts the GPCF value from an octet */
#define GPCF(byte) ((byte)&MESH_PRV_GPCF_MASK)

/*! Extracts the SegN or SegIndex value from the first octet of a Control PDU */
#define EXTRACT_SEGX(byte) ((byte) >> MESH_PRV_GPCF_SIZE)

/*! Sets the SegN or SegIndex value from the first octet of a Control PDU */
#define SET_SEGX(byte, segX) ((byte) = (byte) | ((segX) << MESH_PRV_GPCF_SIZE))

/*! Extracts the opcode value from the first octet of a Control PDU */
#define EXTRACT_OPCODE(byte) ((byte) >> MESH_PRV_GPCF_SIZE)

/*! Sets the opcode value from the first octet of a Control PDU */
#define SET_OPCODE(byte, opcode) ((byte) = (byte) | ((opcode) << MESH_PRV_GPCF_SIZE))

/*! Validate link ID with the current session info for a Transaction */
//#define VALIDATE_LINK(linkId) ((provision_info.linkOpened) && \
//                               (provision_info.linkId == (linkId)))
#define VALIDATE_LINK(linkId) ((provision_info.linkId == (linkId)))

/*! Validate link ID and transaction number with the current session info for an ACK */
#define VALIDATE_ACK(linkId, tranNum) ((provision_info.linkOpened) &&        \
                                       (provision_info.linkId == (linkId) && \
                                        provision_info.localTranNum == (tranNum)))

/*! Increment the transaction number for a Provisioner Server */
#define PRV_SR_INC_TRAN_NUM(x) ((x) = ((x) != MESH_PRV_SR_TRAN_NUM_WRAP) ? ((x) + 1) : MESH_PRV_SR_TRAN_NUM_START)

/*! Increment the transaction number for a Provisioner Client */
#define PRV_CL_INC_TRAN_NUM(x) ((x) = ((x) != MESH_PRV_CL_TRAN_NUM_WRAP) ? ((x) + 1) : MESH_PRV_CL_TRAN_NUM_START)

/*! Mark segX as received in the segments mask */
#define MASK_MARK_SEG(segX) ((segX < 32) ? (provision_info.rxSegMask[1] |= (1 << (segX))) : (provision_info.rxSegMask[0] |= (1 << ((segX)-32))))

/*! Check segX is received in the segments mask */
#define MASK_CHECK_SEG(segX) ((segX < 32) ? (provision_info.rxSegMask[1] & (1 << (segX))) : (provision_info.rxSegMask[0] & (1 << ((segX)-32))))

/*! Invalid value for the Provisioning PDU opcode, used to detect new transactions */
#define MESH_PRV_BR_INVALID_OPCODE (0xFF)

/*! Set the PDU retry count to the timer parameters */
#define SET_RETRY_COUNT(param, count) ((param) = ((param)&0x00FF) | ((count) << 8))

/*! Get the PDU retry count from the timer parameters */
#define GET_RETRY_COUNT(param) ((param) >> 8)

/*! Get the PDU retry opcode from the timer parameters */
#define GET_RETRY_OPCODE(param) ((param)&0xFF)

/*! Retry count for Provisioning Control PDUs */
#define PRV_CTL_PDU_RETRY_COUNT 3

#define MESH_PRV_BEACON_INTERVAL               200

/*! Maximum size of an unprovisioned device beacon */
#define MESH_PRV_MAX_BEACON_SIZE               23

/*! Maximum size of an unprovisioned device beacon without an associated URI */
#define MESH_PRV_MAX_NO_URI_BEACON_SIZE        19

/*! Offset of the device UUID in a beacon */
#define MESH_PRV_BEACON_DEVICE_UUID_OFFSET     1

/*! Offset of the OOB info in a beacon */
#define MESH_PRV_BEACON_OOB_INFO_OFFSET        17

/*! Offset of the URI hash in a beacon */
#define MESH_PRV_BEACON_URI_HASH_OFFSET        19

/*! Maximum size of the URI hash is 4 Octets */
#define MESH_PRV_BEACON_URI_HASH_SIZE          4

/*! GPCF value for Transaction Start */
#define MESH_PRV_GPCF_START                    0x00

/*! GPCF value for Transaction ACK */
#define MESH_PRV_GPCF_ACK                      0x01

/*! GPCF value for Transaction Continuation */
#define MESH_PRV_GPCF_CONTINUATION             0x02

/*! GPCF value for Provisioning Bearer Control */
#define MESH_PRV_GPCF_CONTROL                  0x03

/*! GPCF mask for a Generic Provisioning PDU */
#define MESH_PRV_GPCF_MASK                     0x03

/*! GPCF size in bits for a Generic Provisioning PDU */
#define MESH_PRV_GPCF_SIZE                     2

/*! Mesh Provisioning Link Open Opcode */
#define MESH_PRV_LINK_OPEN_OPCODE              0x00

/*! Mesh Provisioning Link Open PDU Size */
#define MESH_PRV_LINK_OPEN_PDU_SIZE            17

/*! Mesh Provisioning Link ACK Opcode */
#define MESH_PRV_LINK_ACK_OPCODE               0x01

/*! Mesh Provisioning Link ACK PDU Size */
#define MESH_PRV_LINK_ACK_PDU_SIZE             1

/*! Mesh Provisioning Link Close Opcode */
#define MESH_PRV_LINK_CLOSE_OPCODE             0x02

/*! Mesh Provisioning Link Close PDU Size */
#define MESH_PRV_LINK_CLOSE_PDU_SIZE           2

/*! Mesh Provisioning PB-ADV Transaction Number Offset */
#define MESH_PRV_PB_ADV_TRAN_NUM_OFFSET        4

/*! Mesh Provisioning PB-ADV Generic Provisioning PDU Offset */
#define MESH_PRV_PB_ADV_GEN_PDU_OFFSET         (4 + 1)

/*! Mesh Provisioning PB-ADV Generic Provisioning PDU Data Offset */
#define MESH_PRV_PB_ADV_GEN_DATA_OFFSET        (4 + 1 + 1)

/*! Transaction Number used by a node for the first time over an unprovisioned link */
#define MESH_PRV_SR_TRAN_NUM_START             0x80

/*! Transaction Number wrap value for a node */
#define MESH_PRV_SR_TRAN_NUM_WRAP              0xFF

/*! Transaction Number used by a provisioner for the first time over an unprovisioned link */
#define MESH_PRV_CL_TRAN_NUM_START             0x00

/*! Transaction Number wrap value for a provisioner */
#define MESH_PRV_CL_TRAN_NUM_WRAP              0x7F

/*! Provisioning Bearer Minimum Tx Delay in ms */
#define MESH_PRV_PROVISIONER_MIN_TX_DELAY_MS   60//30

/*! Provisioning Bearer Maximum Tx Delay in ms */
#define MESH_PRV_PROVISIONER_MAX_TX_DELAY_MS   90//50

/*! Mesh Provisioning PB-ADV PDU minimum length */
#define MESH_PRV_MIN_PB_ADV_PDU_SIZE           6

/*! Mesh Provisioning PB-ADV PDU maximum length */
#define MESH_PRV_MAX_PB_ADV_PDU_SIZE           29

/*! Mesh Generic Provisioning PDU maximum length */
#define MESH_PRV_MAX_GEN_PB_PDU_SIZE           24

/*! Mesh Provisioning Data PDU Segment 0 maximum length */
#define MESH_PRV_MAX_SEG0_PB_PDU_SIZE          20

/*! Mesh Provisioning Data PDU Segment 0 header length */
#define MESH_PRV_MAX_SEG0_PB_HDR_SIZE          4

/*! Mesh Provisioning Data PDU Segment X maximum length, except when X is 0 */
#define MESH_PRV_MAX_SEGX_PB_PDU_SIZE          23

/*! Mesh Provisioning Data PDU Segment X header length */
#define MESH_PRV_MAX_SEGX_PB_HDR_SIZE          1

/*! Mesh Provisioning Segment Receive Mask Length - Max 64 fragments */
#define MESH_PRV_SEG_MASK_SIZE                 2

/*! Provisioning transaction timeout in ms */
#define MESH_PRV_TRAN_TIMEOUT_MS               1500

/*! Provisioning Link Establishment timeout in ms */
#define MESH_PRV_LINK_TIMEOUT_MS               10000

/* Provisioning PDU format definitions */

/*! Opcode location */
#define MESH_PRV_PDU_OPCODE_INDEX               0
/*! Opcode size */
#define MESH_PRV_PDU_OPCODE_SIZE                1
/*! Parameter location */
#define MESH_PRV_PDU_PARAM_INDEX                (MESH_PRV_PDU_OPCODE_INDEX \
                                                + MESH_PRV_PDU_OPCODE_SIZE)

/*! Invite PDU: Attention parameter index */
#define MESH_PRV_PDU_INVITE_ATTENTION_INDEX     (MESH_PRV_PDU_PARAM_INDEX)
/*! Invite PDU: Attention parameter size */
#define MESH_PRV_PDU_INVITE_ATTENTION_SIZE      1
/*! Invite PDU: Total parameter size */
#define MESH_PRV_PDU_INVITE_PARAM_SIZE          (MESH_PRV_PDU_INVITE_ATTENTION_SIZE)
/*! Invite PDU: Total PDU size */
#define MESH_PRV_PDU_INVITE_PDU_SIZE            (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_INVITE_PARAM_SIZE)

/*! Capabilities PDU: Number of Elements parameter index */
#define MESH_PRV_PDU_CAPAB_NUM_ELEM_INDEX      (MESH_PRV_PDU_PARAM_INDEX)
/*! Capabilitites PDU: Number of Elements parameter size */
#define MESH_PRV_PDU_CAPAB_NUM_ELEM_SIZE        1
/*! Capabilitites PDU: Algorithms parameter index */
#define MESH_PRV_PDU_CAPAB_ALGORITHMS_INDEX     (MESH_PRV_PDU_CAPAB_NUM_ELEM_INDEX \
                                                + MESH_PRV_PDU_CAPAB_NUM_ELEM_SIZE)
/*! Capabilitites PDU: Algorithms parameter size */
#define MESH_PRV_PDU_CAPAB_ALGORITHMS_SIZE      2
/*! Capabilitites PDU: Public Key Type parameter index */
#define MESH_PRV_PDU_CAPAB_PUB_KEY_TYPE_INDEX   (MESH_PRV_PDU_CAPAB_ALGORITHMS_INDEX \
                                                + MESH_PRV_PDU_CAPAB_ALGORITHMS_SIZE)
/*! Capabilitites PDU: Public Key Type parameter size */
#define MESH_PRV_PDU_CAPAB_PUB_KEY_TYPE_SIZE    1
/*! Capabilitites PDU: Static OOB Type parameter index */
#define MESH_PRV_PDU_CAPAB_STATIC_OOB_INDEX     (MESH_PRV_PDU_CAPAB_PUB_KEY_TYPE_INDEX \
                                                + MESH_PRV_PDU_CAPAB_PUB_KEY_TYPE_SIZE)
/*! Capabilitites PDU: Static OOB Type parameter size */
#define MESH_PRV_PDU_CAPAB_STATIC_OOB_SIZE      1
/*! Capabilitites PDU: Output OOB Size parameter index */
#define MESH_PRV_PDU_CAPAB_OUT_OOB_SIZE_INDEX   (MESH_PRV_PDU_CAPAB_STATIC_OOB_INDEX \
                                                + MESH_PRV_PDU_CAPAB_STATIC_OOB_SIZE)
/*! Capabilitites PDU: Output OOB Size parameter size */
#define MESH_PRV_PDU_CAPAB_OUT_OOB_SIZE_SIZE    1
/*! Capabilitites PDU: Output OOB Action parameter index */
#define MESH_PRV_PDU_CAPAB_OUT_OOB_ACT_INDEX    (MESH_PRV_PDU_CAPAB_OUT_OOB_SIZE_INDEX \
                                                + MESH_PRV_PDU_CAPAB_OUT_OOB_SIZE_SIZE)
/*! Capabilitites PDU: Output OOB Action parameter size */
#define MESH_PRV_PDU_CAPAB_OUT_OOB_ACT_SIZE     2
/*! Capabilitites PDU: Input OOB Size parameter index */
#define MESH_PRV_PDU_CAPAB_IN_OOB_SIZE_INDEX    (MESH_PRV_PDU_CAPAB_OUT_OOB_ACT_INDEX \
                                                + MESH_PRV_PDU_CAPAB_OUT_OOB_ACT_SIZE)
/*! Capabilitites PDU: Input OOB Size parameter size */
#define MESH_PRV_PDU_CAPAB_IN_OOB_SIZE_SIZE     1
/*! Capabilitites PDU: Input OOB Action parameter index */
#define MESH_PRV_PDU_CAPAB_IN_OOB_ACT_INDEX     (MESH_PRV_PDU_CAPAB_IN_OOB_SIZE_INDEX \
                                                + MESH_PRV_PDU_CAPAB_IN_OOB_SIZE_SIZE)
/*! Capabilitites PDU: Input OOB Action parameter size */
#define MESH_PRV_PDU_CAPAB_IN_OOB_ACT_SIZE      2
/*! Capabilitites PDU: Total parameter size */
#define MESH_PRV_PDU_CAPAB_PARAM_SIZE           (MESH_PRV_PDU_CAPAB_NUM_ELEM_SIZE \
                                                + MESH_PRV_PDU_CAPAB_ALGORITHMS_SIZE \
                                                + MESH_PRV_PDU_CAPAB_PUB_KEY_TYPE_SIZE \
                                                + MESH_PRV_PDU_CAPAB_STATIC_OOB_SIZE \
                                                + MESH_PRV_PDU_CAPAB_OUT_OOB_SIZE_SIZE \
                                                + MESH_PRV_PDU_CAPAB_OUT_OOB_ACT_SIZE \
                                                + MESH_PRV_PDU_CAPAB_IN_OOB_SIZE_SIZE \
                                                + MESH_PRV_PDU_CAPAB_IN_OOB_ACT_SIZE)
/*! Capabilitites PDU: Total PDU size */
#define MESH_PRV_PDU_CAPAB_PDU_SIZE             (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_CAPAB_PARAM_SIZE)

/*! Start PDU: Algorithm parameter index */
#define MESH_PRV_PDU_START_ALGORITHM_INDEX      (MESH_PRV_PDU_PARAM_INDEX)
/*! Start PDU: Algorithm parameter size */
#define MESH_PRV_PDU_START_ALGORITHM_SIZE       1
/*! Start PDU: Public Key parameter index */
#define MESH_PRV_PDU_START_PUB_KEY_INDEX        (MESH_PRV_PDU_START_ALGORITHM_INDEX \
                                                + MESH_PRV_PDU_START_ALGORITHM_SIZE)
/*! Start PDU: Public Key parameter size */
#define MESH_PRV_PDU_START_PUB_KEY_SIZE         1
/*! Start PDU: Authentication Method parameter index */
#define MESH_PRV_PDU_START_AUTH_METHOD_INDEX    (MESH_PRV_PDU_START_PUB_KEY_INDEX \
                                                + MESH_PRV_PDU_START_PUB_KEY_SIZE)
/*! Start PDU: Authentication Method parameter size */
#define MESH_PRV_PDU_START_AUTH_METHOD_SIZE     1
/*! Start PDU: Authentication Action parameter index */
#define MESH_PRV_PDU_START_AUTH_ACTION_INDEX    (MESH_PRV_PDU_START_AUTH_METHOD_INDEX \
                                                + MESH_PRV_PDU_START_AUTH_METHOD_SIZE)
/*! Start PDU: Authentication Action parameter size */
#define MESH_PRV_PDU_START_AUTH_ACTION_SIZE     1
/*! Start PDU: Authentication Size parameter index */
#define MESH_PRV_PDU_START_AUTH_SIZE_INDEX      (MESH_PRV_PDU_START_AUTH_ACTION_INDEX \
                                                + MESH_PRV_PDU_START_AUTH_ACTION_SIZE)
/*! Start PDU: Authentication Size parameter size */
#define MESH_PRV_PDU_START_AUTH_SIZE_SIZE       1
/*! Start PDU: Total parameter size */
#define MESH_PRV_PDU_START_PARAM_SIZE           (MESH_PRV_PDU_START_ALGORITHM_SIZE \
                                                + MESH_PRV_PDU_START_PUB_KEY_SIZE \
                                                + MESH_PRV_PDU_START_AUTH_METHOD_SIZE \
                                                + MESH_PRV_PDU_START_AUTH_ACTION_SIZE \
                                                + MESH_PRV_PDU_START_AUTH_SIZE_SIZE)
/*! Start PDU: Total PDU size */
#define MESH_PRV_PDU_START_PDU_SIZE             (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_START_PARAM_SIZE)

/*! Public Key PDU: Public Key X parameter index */
#define MESH_PRV_PDU_PUB_KEY_X_INDEX            (MESH_PRV_PDU_PARAM_INDEX)
/*! Public Key PDU: Public Key X parameter size */
#define MESH_PRV_PDU_PUB_KEY_X_SIZE             32
/*! Public Key PDU: Public Key Y parameter index */
#define MESH_PRV_PDU_PUB_KEY_Y_INDEX            (MESH_PRV_PDU_PUB_KEY_X_INDEX \
                                                + MESH_PRV_PDU_PUB_KEY_X_SIZE)
/*! Public Key PDU: Public Key Y parameter size */
#define MESH_PRV_PDU_PUB_KEY_Y_SIZE             32
/*! Public Key PDU: Total parameter size */
#define MESH_PRV_PDU_PUB_KEY_PARAM_SIZE         (MESH_PRV_PDU_PUB_KEY_X_SIZE \
                                                + MESH_PRV_PDU_PUB_KEY_Y_SIZE)
/*! Public Key PDU: Total PDU size */
#define MESH_PRV_PDU_PUB_KEY_PDU_SIZE           (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_PUB_KEY_PARAM_SIZE)

/*! Input Complete PDU: Total parameter size */
#define MESH_PRV_PDU_INPUT_COMPLETE_PARAM_SIZE  0
/*! Input Complete PDU: Total PDU size */
#define MESH_PRV_PDU_INPUT_COMPLETE_PDU_SIZE    (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_INPUT_COMPLETE_PARAM_SIZE)

/*! Confirmation PDU: Confirmation parameter index */
#define MESH_PRV_PDU_CONFIRM_CONFIRM_INDEX      (MESH_PRV_PDU_PARAM_INDEX)
/*! Confirmation PDU: Confirmation parameter size */
#define MESH_PRV_PDU_CONFIRM_CONFIRM_SIZE       16
/*! Confirmation PDU: Total parameter size */
#define MESH_PRV_PDU_CONFIRM_PARAM_SIZE         (MESH_PRV_PDU_CONFIRM_CONFIRM_SIZE)
/*! Confirmation PDU: Total PDU size */
#define MESH_PRV_PDU_CONFIRM_PDU_SIZE           (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_CONFIRM_PARAM_SIZE)

/*! Random PDU: Random parameter index */
#define MESH_PRV_PDU_RANDOM_RANDOM_INDEX        (MESH_PRV_PDU_PARAM_INDEX)
/*! Random PDU: Random parameter size */
#define MESH_PRV_PDU_RANDOM_RANDOM_SIZE         16
/*! Random PDU: Total parameter size */
#define MESH_PRV_PDU_RANDOM_PARAM_SIZE          (MESH_PRV_PDU_RANDOM_RANDOM_SIZE)
/*! Random PDU: Total PDU size */
#define MESH_PRV_PDU_RANDOM_PDU_SIZE            (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_RANDOM_PARAM_SIZE)

/*! Data PDU: Encrypted Provisioning Data parameter index */
#define MESH_PRV_PDU_DATA_ENC_DATA_INDEX        (MESH_PRV_PDU_PARAM_INDEX)
/*! Data PDU: Encrypted Provisioning Data parameter size */
#define MESH_PRV_PDU_DATA_ENC_DATA_SIZE         25
/*! Data PDU: Provisioning Data MIC parameter index */
#define MESH_PRV_PDU_DATA_MIC_INDEX             (MESH_PRV_PDU_DATA_ENC_DATA_INDEX \
                                                + MESH_PRV_PDU_DATA_ENC_DATA_SIZE)
/*! Data PDU: Provisioning Data MIC parameter size */
#define MESH_PRV_PDU_DATA_MIC_SIZE              8
/*! Data PDU: Total parameter size */
#define MESH_PRV_PDU_DATA_PARAM_SIZE            (MESH_PRV_PDU_DATA_ENC_DATA_SIZE \
                                                + MESH_PRV_PDU_DATA_MIC_SIZE)
/*! Data PDU: Total PDU size */
#define MESH_PRV_PDU_DATA_PDU_SIZE              (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_DATA_PARAM_SIZE)

/*! Decrypted Data: Network Key parameter index */
#define MESH_PRV_DECRYPTED_DATA_NETKEY_INDEX      0
/*! Decrypted Data: Network Key parameter size */
#define MESH_PRV_DECRYPTED_DATA_NETKEY_SIZE       16
/*! Decrypted Data: NetKey Index parameter index */
#define MESH_PRV_DECRYPTED_DATA_NETKEYIDX_INDEX   (MESH_PRV_DECRYPTED_DATA_NETKEY_INDEX \
                                                  + MESH_PRV_DECRYPTED_DATA_NETKEY_SIZE)
/*! Decrypted Data: NetKey Index parameter size */
#define MESH_PRV_DECRYPTED_DATA_NETKEYIDX_SIZE    2
/*! Decrypted Data: Flags parameter index */
#define MESH_PRV_DECRYPTED_DATA_FLAGS_INDEX       (MESH_PRV_DECRYPTED_DATA_NETKEYIDX_INDEX \
                                                  + MESH_PRV_DECRYPTED_DATA_NETKEYIDX_SIZE)
/*! Decrypted Data: Flags parameter size */
#define MESH_PRV_DECRYPTED_DATA_FLAGS_SIZE        1
/*! Decrypted Data: IV Index parameter index */
#define MESH_PRV_DECRYPTED_DATA_IVIDX_INDEX       (MESH_PRV_DECRYPTED_DATA_FLAGS_INDEX \
                                                  + MESH_PRV_DECRYPTED_DATA_FLAGS_SIZE)
/*! Decrypted Data: IV Index parameter size */
#define MESH_PRV_DECRYPTED_DATA_IVIDX_SIZE        4
/*! Decrypted Data: Address parameter index */
#define MESH_PRV_DECRYPTED_DATA_ADDRESS_INDEX     (MESH_PRV_DECRYPTED_DATA_IVIDX_INDEX \
                                                  + MESH_PRV_DECRYPTED_DATA_IVIDX_SIZE)
/*! Decrypted Data: Address parameter size */
#define MESH_PRV_DECRYPTED_DATA_ADDRESS_SIZE      2

/*! Complete PDU: Total parameter size */
#define MESH_PRV_PDU_COMPLETE_PARAM_SIZE        0
/*! Complete PDU: Total PDU size */
#define MESH_PRV_PDU_COMPLETE_PDU_SIZE          (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_COMPLETE_PARAM_SIZE)

/*! Failed PDU: Error Code parameter index */
#define MESH_PRV_PDU_FAILED_ERROR_CODE_INDEX    (MESH_PRV_PDU_PARAM_INDEX)
/*! Failed PDU: Error Code parameter size */
#define MESH_PRV_PDU_FAILED_ERROR_CODE_SIZE     1
/*! Failed PDU: Total parameter size */
#define MESH_PRV_PDU_FAILED_PARAM_SIZE          (MESH_PRV_PDU_FAILED_ERROR_CODE_SIZE)
/*! Failed PDU: Total PDU size */
#define MESH_PRV_PDU_FAILED_PDU_SIZE            (MESH_PRV_PDU_OPCODE_SIZE \
                                                + MESH_PRV_PDU_FAILED_PARAM_SIZE)

/* Provisioning crypto constants */

/*! Size of the ConfirmationInputs array */
#define MESH_PRV_CONFIRMATION_INPUTS_SIZE       (MESH_PRV_PDU_INVITE_PARAM_SIZE \
                                                + MESH_PRV_PDU_CAPAB_PARAM_SIZE \
                                                + MESH_PRV_PDU_START_PARAM_SIZE \
                                                + 2 * MESH_PRV_PDU_PUB_KEY_PARAM_SIZE)
/*! Size of the ConfirmationSalt value */
#define MESH_PRV_CONFIRMATION_SALT_SIZE         16
/*! Size of the AuthValue */
#define MESH_PRV_AUTH_VALUE_SIZE                16
#define MESH_PRV_CONFIRMATION_KEY_TEMP          "prck"
/*! Size of the ProvisioningSalt value */
#define MESH_PRV_PROVISIONING_SALT_SIZE         16
#define MESH_PRV_SESSION_KEY_TEMP               "prsk"
#define MESH_PRV_SESSION_NONCE_TEMP             "prsn"
#define MESH_PRV_DEVICE_KEY_TEMP                "prdk"
#define MESH_PRV_SESSION_NONCE_SIZE             13
#define MESH_PRV_MAX_OOB_SIZE                   8
#define MESH_PRV_NUMERIC_OOB_SIZE_OCTETS        4

enum meshPrvAlgorithms
{
    MESH_PRV_ALGO_FIPS_P256_ELLIPTIC_CURVE = (1 << 0),  /*!< FIPS P-256 Elliptic Curve bit */
    MESH_PRV_ALGO_RFU_BITMASK              = 0xFFFE,    /*!< RFU bitmask */
};

enum meshPrvPublicKeyType
{
    MESH_PRV_PUB_KEY_OOB     = (1 << 0),  /*!< Public Key OOB information available bit */
    MESH_PRV_PUB_RFU_BITMASK = 0xFE,      /*!< RFU bits  */
};

enum meshPrvStaticOobType
{
    MESH_PRV_STATIC_OOB_INFO_AVAILABLE = (1 << 0),  /*!< Static OOB information available bit */
    MESH_PRV_STATIC_OOB_RFU_BITMASK    = 0xFE,      /*!< RFU bits */
};


enum meshPrvOutputOobSize
{
    MESH_PRV_OUTPUT_OOB_NOT_SUPPORTED    = 0x00,  /*!< The device does not support output OOB */
    MESH_PRV_OUTPUT_OOB_SIZE_ONE_OCTET   = 0x01,  /*!< Maximum 1 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_TWO_OCTET   = 0x02,  /*!< Maximum 2 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_THREE_OCTET = 0x03,  /*!< Maximum 3 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_FOUR_OCTET  = 0x04,  /*!< Maximum 4 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_FIVE_OCTET  = 0x05,  /*!< Maximum 5 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_SIX_OCTET   = 0x06,  /*!< Maximum 6 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_SEVEN_OCTET = 0x07,  /*!< Maximum 7 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_EIGHT_OCTET = 0x08,  /*!< Maximum 8 octet can be output */
    MESH_PRV_OUTPUT_OOB_SIZE_RFU_START   = 0x09,  /*!< All numbers above this value are RFUs */
};

enum meshPrvOutputOobAction
{
    MESH_PRV_OUTPUT_OOB_ACTION_BLINK           = (1 << 0),  /*!< Blink - Numeric */
    MESH_PRV_OUTPUT_OOB_ACTION_BEEP            = (1 << 1),  /*!< Beep - Numeric */
    MESH_PRV_OUTPUT_OOB_ACTION_VIBRATE         = (1 << 2),  /*!< Vibrate - Numeric */
    MESH_PRV_OUTPUT_OOB_ACTION_OUTPUT_NUMERIC  = (1 << 3),  /*!< Output Numeric - Numeric */
    MESH_PRV_OUTPUT_OOB_ACTION_OUTPUT_ALPHANUM = (1 << 4),  /*!< Output Alphanumeric - Octets array
                                                           */
    MESH_PRV_OUTPUT_OOB_ACTION_RFU_BITMASK     = 0xFFE0,    /*!< RFU bits */
};

enum meshPrvInputOobSize
{
    MESH_PRV_INPUT_OOB_NOT_SUPPORTED    = 0x00,  /*!< The device does not support input OOB */
    MESH_PRV_INPUT_OOB_SIZE_ONE_OCTET   = 0x01,  /*!< Maximum 1 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_TWO_OCTET   = 0x02,  /*!< Maximum 2 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_THREE_OCTET = 0x03,  /*!< Maximum 3 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_FOUR_OCTET  = 0x04,  /*!< Maximum 4 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_FIVE_OCTET  = 0x05,  /*!< Maximum 5 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_SIX_OCTET   = 0x06,  /*!< Maximum 6 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_SEVEN_OCTET = 0x07,  /*!< Maximum 7 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_EIGHT_OCTET = 0x08,  /*!< Maximum 8 octet can be input */
    MESH_PRV_INPUT_OOB_SIZE_RFU_START   = 0x09,  /*!< All numbers above this value are RFUs */
};

enum meshPrvInputOobAction
{
    MESH_PRV_INPUT_OOB_ACTION_PUSH           = (1 << 0),  /*!< Push - Numeric */
    MESH_PRV_INPUT_OOB_ACTION_TWIST          = (1 << 1),  /*!< Twist - Numeric */
    MESH_PRV_INPUT_OOB_ACTION_INPUT_NUMERIC  = (1 << 2),  /*!< Input number - Numeric */
    MESH_PRV_INPUT_OOB_ACTION_INPUT_ALPHANUM = (1 << 3),  /*!< Input Alphanumeric - Octets array */
    MESH_PRV_INPUT_OOB_ACTION_RFU_BITMASK    = 0xFFF0,    /*!< RFU bits */
};

enum meshPrvPduTypes
{
    MESH_PRV_PDU_INVITE         = 0x00,  /*!< Indicates invitation to join a mesh network */
    MESH_PRV_PDU_CAPABILITIES   = 0x01,  /*!< Indicates the capabilities of the device */
    MESH_PRV_PDU_START          = 0x02,  /*!< Indicates provisioning method selected by the
                                        *   provisioner based on the capabilities of the device
                                        */
    MESH_PRV_PDU_PUB_KEY        = 0x03,  /*!< Contains the public key data of the device or
                                        *   provisioner
                                        */
    MESH_PRV_PDU_INPUT_COMPLETE = 0x04,  /*!< Indicates user has completed inputting the values */
    MESH_PRV_PDU_CONFIRMATION   = 0x05,  /*!< Contains the provisioning confirmation value of the
                                        *   device or the Provisioner
                                        */
    MESH_PRV_PDU_RANDOM         = 0x06,  /*!< Contains the provisioning random value of the device or
                                        *   the Provisioner
                                        */
    MESH_PRV_PDU_DATA           = 0x07,  /*!< Includes the assigned unicast address of the primary
                                        *   element, a network key, NetKey Index, Flags and the
                                        *   IV Index
                                        */
    MESH_PRV_PDU_COMPLETE       = 0x08,  /*!< Indicates that provisioning is complete */
    MESH_PRV_PDU_FAILED         = 0x09,  /*!< Indicates that provisioning was unsuccessful */
    MESH_PRV_PDU_RFU_START      = 0x0A,  /*!< First RFU PDU type */
};

/*! Provisioning Start PDU: Algorithm values */
typedef enum
{
    MESH_PRV_START_ALGO_FIPS_P_256_EC  = 0x00,  /*!< FIPS-P256 Elliptic Curve */
    MESH_PRV_START_ALGO_RFU_START      = 0x01,  /*!< All numbers above this value are RFUs */
} meshPrvPduStartAlgorithmValues;

typedef enum
{
    MESH_PRV_CL_NO_OBB_AUTH       = 0x00,   /*!< No OOB authentication is used. Provisioning is insecure. */
    MESH_PRV_CL_USE_STATIC_OOB    = 0x01,   /*!< Use 16-octet static OOB data for authentication. */
    MESH_PRV_CL_USE_OUTPUT_OOB    = 0x02,   /*!< Use output OOB data. */
    MESH_PRV_CL_USE_INPUT_OOB     = 0x03,   /*!< Use input OOB data. */
} meshPrvClOobAuthMethodValues;


typedef enum
{
    E_PROV_STATE_IDLE,
    E_PROV_STATE_START_PROV,
    E_PROV_STATE_LINK_OPEN,
    E_PROV_STATE_SEND_INVITE,
    E_PROV_STATE_RECV_START,
    E_PROV_STATE_WAIT_CAPABILITIES,
    E_PROV_STATE_WAIT_SELECT_AUTH,
    E_PROV_STATE_SEND_START,
    E_PROV_STATE_SEND_PUB_KEY,
    E_PROV_STATE_SEND_CONFIRMATION,
    E_PROV_STATE_SEND_PRV_COMPLETE,
} prov_state;


typedef enum
{
    MESH_PRV_OOB_INFO_OTHER                     = (1 << 0),   /*!< Other source */
    MESH_PRV_OOB_INFO_ELECTRONIC_URI            = (1 << 1),   /*!< Electronic/URI */
    MESH_PRV_OOB_INFO_2D_MACHINE_READABLE_CODE  = (1 << 2),   /*!< 2D machine-readable code */
    MESH_PRV_OOB_INFO_BAR_CODE                  = (1 << 3),   /*!< Bar code */
    MESH_PRV_OOB_INFO_NFC                       = (1 << 4),   /*!< Near Field Communication (NFC) */
    MESH_PRV_OOB_INFO_NUMBER                    = (1 << 5),   /*!< Number */
    MESH_PRV_OOB_INFO_STRING                    = (1 << 6),   /*!< String */
    MESH_PRV_OOB_INFO_ON_BOX                    = (1 << 11),  /*!< On box */
    MESH_PRV_OOB_INFO_INSIDE_BOX                = (1 << 12),  /*!< Inside box */
    MESH_PRV_OOB_INFO_ON_PIECE_OF_PAPER         = (1 << 13),  /*!< On piece of paper */
    MESH_PRV_OOB_INFO_INSIDE_MANUAL             = (1 << 14),  /*!< Inside manual */
    MESH_PRV_OOB_INFO_ON_DEVICE                 = (1 << 15),  /*!< On device */
} prov_oob_info_src_type;
typedef enum
{
    MESH_PRV_NO_OBB_AUTH       = 0x00,   /*!< No OOB authentication is used. Provisioning is insecure. */
    MESH_PRV_USE_STATIC_OOB    = 0x01,   /*!< Use 16-octet static OOB data for authentication. */
    MESH_PRV_USE_OUTPUT_OOB    = 0x02,   /*!< Use output OOB data. */
    MESH_PRV_USE_INPUT_OOB     = 0x03,   /*!< Use input OOB data. */
} prov_oob_auth_type;


/*! Mesh Provisioning PB-ADV Link Close reason types enumeration */
enum meshPrvBrReasonTypes
{
    MESH_PRV_BR_REASON_SUCCESS = 0x00, /*!< The provisioning was successful */
    MESH_PRV_BR_REASON_TIMEOUT = 0x01, /*!< The provisioning transaction timed out */
    MESH_PRV_BR_REASON_FAIL    = 0x02  /*!< The provisioning failed */
};


#ifdef __cplusplus
};
#endif
#endif /* __PRV_DEF_H__ */
