/**
 * @file dtm_mode.h
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 * @copyright (C) Rafael Microelectronics. Taiwan
 *
 * @see http://
 */

#ifndef _DTM_MODE_H__
#define _DTM_MODE_H__

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define UART_0 (0)
#define UART_1 (1)
#define DTM_UART (UART_0)

#define DTM_CMD_SIZE        (2)
#define DTM_EVENT_SIZE      (2)

#define DTM_BITRATE                     UART_BAUDRATE_BAUDRATE_Baud19200/**< Serial bitrate on the UART */
#define RUCI_PCI_HEADER (0x21)

/**@details The UART poll cycle in micro seconds.
 *          A baud rate of e.g. 19200 bits / second, and 8 data bits, 1 start/stop bit, no flow control,
 *          give the time to transmit a byte: 10 bits * 1/19200 = approx: 520 us.
 *          To ensure no loss of bytes, the UART should be polled every 260 us.
 */
#if DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud9600
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/9600/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud14400
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/14400/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud19200
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/19200/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud28800
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/28800/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud38400
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/38400/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud57600
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/57600/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud76800
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/768000/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud115200
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/115200/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud230400
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/230400/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud250000
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/250000/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud460800
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/460800/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud921600
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/921600/2))
#elif DTM_BITRATE == UART_BAUDRATE_BAUDRATE_Baud1M
#define UART_POLL_CYCLE ((uint32_t)(10*1e6/1e6/2))
#else
// It is possible to find values that work for other baud rates, but the formula above is not
// guaranteed to work for all values. Suitable values may have to be found by trial and error.
#error "Unsupported baud rate set."
#endif
#define MAX_ITERATIONS_NEEDED_FOR_NEXT_BYTE ((5000 + 2 * UART_POLL_CYCLE) / UART_POLL_CYCLE)

/**@brief Configuration parameters. */
#define LE_TEST_SETUP                   0                               /**< DTM command: Set PHY or modulation, configure upper two bits of length,
                                                                             request matrix of supported features or request max values of parameters. */
#define LE_RECEIVER_TEST                1                               /**< DTM command: Start receive test. */
#define LE_TRANSMITTER_TEST             2                               /**< DTM command: Start transmission test. */
#define LE_TEST_END                     3                               /**< DTM command: End test and send packet report. */

#define LE_TEST_SETUP_RESET             0                               /**< DTM command parameter: Stop TX/RX, reset the packet length upper bits and set the PHY to 1Mbit. */
#define LE_TEST_SETUP_SET_UPPER         1                               /**< DTM command parameter: Set the upper two bits of the length field. */
#define LE_TEST_SETUP_SET_PHY           2                               /**< DTM command parameter: Select the PHY to be used for packets. */
#define LE_TEST_SETUP_SELECT_MODULATION 3                               /**< DTM command parameter: Select standard or stable modulation index. Stable modulation index is not supported. */
#define LE_TEST_SETUP_READ_SUPPORTED    4                               /**< DTM command parameter: Read the supported test case features. */
#define LE_TEST_SETUP_READ_MAX          5                               /**< DTM command parameter: Read the max supported time and length for packets. */

#define LE_PHY_1M                       1                               /**< DTM command parameter: Set PHY for future packets to use 1MBit PHY. */
#define LE_PHY_2M                       2                               /**< DTM command parameter: Set PHY for future packets to use 2MBit PHY. */
#define LE_PHY_LE_CODED_S8              3                               /**< DTM command parameter: Set PHY for future packets to use coded PHY with S=8. */
#define LE_PHY_LE_CODED_S2              4                               /**< DTM command parameter: Set PHY for future packets to use coded PHY with S=2 */

// Configuration options used as parameter 2
// when cmd == LE_TRANSMITTER_TEST and payload == DTM_PKT_VENDORSPECIFIC
// Configuration value, if any, is supplied in parameter 3

#define CARRIER_TEST                    0                               /**< Length=0 indicates a constant, unmodulated carrier until LE_TEST_END or LE_RESET */
#define CARRIER_TEST_STUDIO             1                               /**< nRFgo Studio uses value 1 in length field, to indicate a constant, unmodulated carrier until LE_TEST_END or LE_RESET */
#define SET_TX_POWER                    2                               /**< Set transmission power, value -40..+4 dBm in steps of 4 */
#define SELECT_TIMER                    3                               /**< Select on of the 16 MHz timers 0, 1 or 2 */

#define LE_PACKET_REPORTING_EVENT       0x8000                          /**< DTM Packet reporting event, returned by the device to the tester. */
#define LE_TEST_STATUS_EVENT_SUCCESS    0x0000                          /**< DTM Status event, indicating success. */
#define LE_TEST_STATUS_EVENT_ERROR      0x0001                          /**< DTM Status event, indicating an error. */

#define DTM_PKT_PRBS9                   0x00                            /**< Bit pattern PRBS9. */
#define DTM_PKT_0X0F                    0x01                            /**< Bit pattern 11110000 (LSB is the leftmost bit). */
#define DTM_PKT_0X55                    0x02                            /**< Bit pattern 10101010 (LSB is the leftmost bit). */
#define DTM_PKT_0XFF                    0x03                            /**< Bit pattern 11111111 (Used only for coded PHY). */
#define DTM_PKT_VENDORSPECIFIC          0xFFFFFFFF                      /**< Vendor specific. Nordic: Continuous carrier test, or configuration. */

#define LE_TEST_SWITCH_CODE             0x1AB                           /**< Proprietary command: Switch to LAB test tool. */

#define PHYS_CH_MAX                 (39)    /**< Maximum number of valid channels in BLE. */

#define DTM_HEADER_OFFSET           (0)
#define DTM_HEADER_SIZE             (6)                                         /**< Size of PDU header. */
#define DTM_PAYLOAD_MAX_SIZE        (255)                                       /**< Maximum payload size allowed during dtm execution. */
#define DTM_PDU_MAX_MEMORY_SIZE     (DTM_HEADER_SIZE + DTM_PAYLOAD_MAX_SIZE)  /**< Maximum PDU size allowed during dtm execution. */

#define sRUCI_CRC_LENGTH(a) (a-1)
#define sRUCI_LENGTH(a)     (a-3)

#ifndef SLEEP_TRIGGER_VALUE
#define SLEEP_TRIGGER_VALUE             (0)
#endif

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef enum _dtm_cmd_opcode
{
    DTM_CMD_LE_TEST_SETUP = 0,
    DTM_CMD_LE_RECEIVER_TEST,
    DTM_CMD_LE_TRANSMITTER_TEST,
    DTM_CMD_LE_TEST_END,
} DTM_CMD_OPCODE;
typedef enum _packet_data_type
{
    PRBS9 = 0,
    ONE_ZERO,
    ONE_ZERO_INTERLEAVER,
    //ALL_ONE, //not supported
    VENDOR_SPECIFIC,
    //TBD modified by documemt
} PACKET_DATA_TYPE;

typedef enum _phr_data_type
{
    PHR_PRBS9 = 0,
    PHR_ONE_ZERO = 1,
    PHR_ONE_ZERO_INTERLEAVER = 2,
    //ALL_ONE, //not supported
    PHR_VENDOR_SPECIFIC = 4,
    //TBD modified by documemt
} PHR_DATA_TYPE;

/**@brief Return codes from dtm_cmd(). */
typedef enum _dtm_error_code
{
    DTM_SUCCESS = 0x00,                            /**< Indicate that the DTM function completed with success. */
    DTM_ERROR_ILLEGAL_CHANNEL,                     /**< Physical channel number must be in the range 0..39. */
    DTM_ERROR_INVALID_STATE,                       /**< Sequencing error: Command is not valid now. */
    DTM_ERROR_ILLEGAL_LENGTH,                      /**< Payload size must be in the range 0..37. */
    DTM_ERROR_ILLEGAL_CONFIGURATION,               /**< Parameter out of range (legal range is function dependent). */
    DTM_ERROR_UNINITIALIZED,                       /**< DTM module has not been initialized by the application. */
} DTM_ERROR_CODE ;

typedef enum _state_e
{
    STATE_UNINITIALIZED = 0,                       /**< The DTM is uninitialized. */
    STATE_IDLE = 1,                                /**< State when system has just initialized, or current test has completed. */
    STATE_TRANSMITTER_TEST = 2,                    /**< State used when a DTM Transmission test is running. */
    STATE_CARRIER_TEST = 3,                        /**< State used when a DTM Carrier test is running (Vendor specific test). */
    STATE_RECEIVER_TEST = 4                        /**< State used when a DTM Receive test is running. */
} STATE;

typedef uint32_t dtm_freq_t;                     /**< Physical channel, valid range: 0..39. */
typedef uint32_t dtm_cmd_t;                      /**< DTM command type. */
typedef uint32_t dtm_pkt_type_t;                /**< Type for holding the requested DTM payload type.*/
typedef uint16_t dtm_report_t;                  /**< Type for handling DTM event. */

typedef enum _data_rate
{
    RADIO_PHY_2M       = 0,
    RADIO_PHY_1M       = 1,
    RADIO_PHY_CODED    = 2,
    RADIO_PHY_FSK_200K = 3,
    RADIO_PHY_FSK_100K = 4,
    RADIO_PHY_FSK_50K  = 5,
    RADIO_PHY_FSK_300K = 6,
    RADIO_PHY_FSK_150K = 7
} DATA_RATE;

typedef enum
{
    BAND_SUBG          = 0,
    BAND_2P4G          = 1
} BAND_TYPE;

typedef enum
{
    MOD_0P5            = 0,
    MOD_1              = 1
} FSK_MOD;

typedef enum
{
    FSK_CRC_16              = 0,
    FSK_CRC_32              = 1
} FSK_CRC_TYPE;

typedef enum
{
    WHITEN_DISABLE     = 0,
    WHITEN_ENABLE      = 1
} WHITEN_FLAG;

typedef enum _coded_scheme
{
    CODED_PHY_SCHEME_S8 = 0,
    CODED_PHY_SCHEME_S2 = 1,
} CODED_SCHEME;

typedef enum _radio_clock
{
    RADIO_CLOCK_32MHZ = 1,
    RADIO_CLOCK_16MHZ = 0,
} RADIO_CLOCK;
typedef enum _dtm_com_id_e
{
    // address mode enums
    DTM_SET_REQUEST                     = 0x01,
    DTM_SET_CONFIRM                     = 0x02,
    DTM_TX_START_REQUEST                = 0x03,
    DTM_TX_START_CONFIRM                = 0x04,
    DTM_TX_STOP_REQUEST                 = 0x05,
    DTM_TX_STOP_CONFIRM                 = 0x06,
    DTM_RX_START_REQUEST                = 0x07,
    DTM_RX_START_CONFIRM                = 0x08,
    DTM_RXPKT_INDICATION                = 0x09,
    DTM_RX_STOP_REQUEST                 = 0x0A,
    DTM_RX_STOP_CONFIRM                 = 0x0B  // TBD modify documemt
} DTM_CMD_ID_e;

typedef void (*RFET_INIT_CB)(void);
void rfet_init_cb_registration (RFET_INIT_CB init_cb);

void dtm_uart_deinit(void);

/* DTM RF set check event flag */
void dtm_set_check_event_flag(void);
/* DTM RF check CRC event */
bool dtm_check_crc_event(uint32_t *crc_count_get, uint32_t *crc_fail_count_get);
/* DTM RF check TX done event */
bool dtm_check_tx_burst_done_event(void);
/* DTM RF get CRC count */
bool dtm_get_crc_count(volatile uint32_t *crc_count_get, volatile uint32_t *crc_fail_count_get);
/* DTM RF set sfd */
bool dtm_set_sfd_cmd(uint32_t sfd);
/* DTM RF set phy */
bool dtm_set_phy_cmd(uint32_t phy);
/* DTM RF set frequency */
bool dtm_set_rf_freq_cmd(uint32_t freq);
/* DTM RF set RX disabled */
bool dtm_set_rx_disable_cmd(void);
/* DTM RF set RX enabled */
bool dtm_set_rx_enable_cmd(uint8_t report_cycle);
/* DTM RF set TX disable */
bool dtm_set_tx_disable_cmd(void);
/* DTM RF set TX enable */
bool dtm_set_tx_enable_cmd(uint8_t data_type, uint8_t phr_type, uint16_t tx_interval, uint16_t packet_len, uint16_t tx_count);
/* DTM RF BLE Mode init */
bool dtm_ble_init_cmd(void);
/* DTM RF ZigBee Mode init */
bool dtm_zigbee_init_cmd(void);
/* Set Clock Mode */
bool dtm_set_clock_cmd(bool enable_32m);

/* Set DTM Command for HTOL Mode */
bool dtm_htol_mode_command_set(uint16_t dtm_cmd);
void dtm_htol_mode_set_enable(bool en);

/* DTM RF WiSUN Mode init */
bool dtm_wisun_init_cmd(void);
/* DTM RF WiSUN Modem set */
bool dtm_wisun_set_modem_cmd(uint8_t data_rate, uint8_t modulation_idx);
/* DTM RF WiSUN Mac set */
bool dtm_wisun_set_mac_cmd(uint8_t crc_type, uint8_t whitening_enable);
/* DTM RF WiSUN preamble set */
bool dtm_wisun_set_preamble_cmd(uint8_t preamble_length);
/* DTM RF WiSUN SFD set */
bool dtm_wisun_set_sfd_cmd(uint32_t sfd_content);

/* DTM flow control */
void dtm_sys_init(RF_FW_LOAD_SELECT fw_select, bool reinit_rf);
bool dtm_sys_is_enable(void);
void dtm_sys_set_enable(bool is_en);
void dtm_sys_common_init(void);
bool dtm_ub_is_enable(void);
void dtm_ub_set_enable(bool is_en);
//void dtm_task(void);
void dtm_mode_init(void);

#endif
